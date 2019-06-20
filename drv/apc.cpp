#include <ntifs.h>
#include "apc.h"

extern pfKeInitializeApc KeInitializeApc;
extern pfKeInsertQueueApc KeInsertQueueApc;

PKAPC
CreateApc(
	unsigned long ThreadId,
	PKNORMAL_ROUTINE NormalRoutine
)
{
	PETHREAD Thread;
	auto Status = PsLookupThreadByThreadId(reinterpret_cast<HANDLE>(ThreadId), &Thread);
	if (STATUS_SUCCESS != Status)
	{
		DbgPrint("PsLookupThreadByThreadId failed for 0x%08X\n", ThreadId);
		return nullptr;
	}

	PKAPC Apc = reinterpret_cast<PKAPC>(ExAllocatePool(NonPagedPool, sizeof(KAPC)));

	KeInitializeApc(Apc, Thread, OriginalApcEnvironment, KernelRoutine, NULL, NormalRoutine, UserMode, NULL);

	return Apc;
}

NTSTATUS
QueueApc(
	PKAPC Apc
)
{
	auto Inserted = KeInsertQueueApc(Apc, NULL, NULL, IO_NO_INCREMENT);
	DbgPrint("Inside QueueApc\n");

	if (!Inserted)
	{
		DbgPrint("QueueApc Failed.\n");

		return STATUS_ILLEGAL_ELEMENT_ADDRESS;
	}
	
	return 0;
}

void NTAPI 
KernelRoutine(
	PKAPC apc,
	PKNORMAL_ROUTINE* NormalRoutine,
	PVOID* NormalContext,
	PVOID* SystemArgument1,
	PVOID* SystemArgument2
	)
{
	NormalRoutine;
	NormalContext;
	SystemArgument1;
	SystemArgument2;
	
	DbgPrint("Inside kernel routine apc\n");

	ExFreePool(apc);
}
