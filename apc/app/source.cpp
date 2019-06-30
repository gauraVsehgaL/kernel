#include <iostream>
#include <Windows.h>
#include <thread>
#include "..\queueapc\comm.h"

VOID NTAPI 
ApcNormalRoutine(
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
)
{
	std::wcout << L"************************Normal Apc Routine*********************\n";
}


bool
CallDriverToCreateApc()
{
	auto hFile = CreateFileW(LR"(\\.\sym_queueapc2)", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"createfile failed " << GetLastError();
		return false;
	}

	std::wcout << L"CreateFile success from CreateApc\n";

	// Call driver to queue apc to this thread itself. 
	DWORD dwBytesReturned;
	ApcInfo Info;
	Info.ThreadId = GetCurrentThreadId();
	Info.NormalRoutine = ApcNormalRoutine;

	if (!DeviceIoControl(hFile, CTL_CREATE_APC, static_cast<void*>(&Info), sizeof(ApcInfo), nullptr, 0, &dwBytesReturned, nullptr))
		std::wcout << L"Failed to send data via buffered io from CreateApc\n";

	std::wcout << L"Successfully sent data to driver from CreateApc. Now waiting.\n";

	//	apc is created for this thread. Now after the apc is queued this thread should be in alertable wait state for apc to be delivered.
	//      apc will be qeueued in any sort of sleep
	//      but will be delivered only if alertable sleep.
	std::wcout << "Before normal sleep\n";
	Sleep(15000);
	std::wcout << "Before sleeping with alertable";

	//The above was non-alertable sleep so apc would not be delivered.
	SleepEx(15000, true);
	//apc must have been delivered since this was alertable sleep.
	std::wcout << L"After sleeping with alertable.\n";

	return true;
}


bool
CallDriverToQueueApc()
{
	auto hFile = CreateFileW(LR"(\\.\sym_queueapc2)", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"createfile failed " << GetLastError();
		return false;
	}

	std::wcout << L"CreateFile success from QueueuApc\n";


	DWORD dwBytesReturned;
	ApcInfo Info;
	Info.ThreadId = GetCurrentThreadId();
	Info.NormalRoutine = ApcNormalRoutine;

	if (!DeviceIoControl(hFile, CTL_QUEUE_APC, nullptr, 0, nullptr, 0, &dwBytesReturned, nullptr))
		std::wcout << L"Failed to send data via buffered io from QueueuApc\n";

	std::wcout << L"Successfully sent data to driver from QueueuApc\n";
	return true;
}

int main()
{
	std::thread CreateApc(CallDriverToCreateApc);
	
	std::wcout << L"Enter 'q' to ask driver to queue apc. Anything else to quit.\n";
	char inp;
	std::cin >> inp;
	if (inp == 'q')
	{
		CallDriverToQueueApc();
		std::wcout << L"Called driver to queue apc.\n";
	}

	CreateApc.join();

	return 0;
}
