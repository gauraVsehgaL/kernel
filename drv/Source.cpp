#include <ntddk.h>
#include "apc.h"
#include "comm.h"

PDEVICE_OBJECT g_pDeviceObject = nullptr;
UNICODE_STRING g_usSymLink;

pfKeInitializeApc KeInitializeApc = nullptr;
pfKeInsertQueueApc KeInsertQueueApc = nullptr;
PKAPC g_Apc = nullptr;

void
DriverUnload(PDRIVER_OBJECT)
{
	IoDeleteSymbolicLink(&g_usSymLink);
	IoDeleteDevice(g_pDeviceObject);
}

NTSTATUS
CreateClose(PDEVICE_OBJECT, PIRP Irp)
{
	Irp->IoStatus.Status = 0;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);
	
	return 0;
}


NTSTATUS
Dispatch(PDEVICE_OBJECT, PIRP Irp)
{
	auto slo = IoGetCurrentIrpStackLocation(Irp);
	auto ctl = slo->Parameters.DeviceIoControl.IoControlCode;
	if (ctl == CTL_CREATE_APC)
	{
		ApcInfo *pApcInfo = static_cast<ApcInfo*>(Irp->AssociatedIrp.SystemBuffer);
		DbgPrint("Received Request to create apc\n");

		g_Apc = CreateApc(pApcInfo->ThreadId, reinterpret_cast<PKNORMAL_ROUTINE>(pApcInfo->NormalRoutine));
	}
	else
	{
		QueueApc(g_Apc);
		DbgPrint("Received Request to deliver apc\n");
	}

	Irp->IoStatus.Status = 0;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);

	return 0;
}


extern "C"
NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING)
{
	UNICODE_STRING usDeviceName;
	RtlInitUnicodeString(&usDeviceName, LR"(\DEVICE\queueapc2)");
	auto Status = IoCreateDevice(DriverObject, 0, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &g_pDeviceObject);
	if (STATUS_SUCCESS != Status)
	{
		return Status;
	}

	RtlInitUnicodeString(&g_usSymLink, LR"(\DosDevices\sym_queueapc2)");
	Status = IoCreateSymbolicLink(&g_usSymLink, &usDeviceName);

	if (STATUS_SUCCESS != Status)
	{
		return Status;
	}

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Dispatch;

	UNICODE_STRING usKeInitializeApc;
	RtlInitUnicodeString(&usKeInitializeApc, L"KeInitializeApc");
	KeInitializeApc = reinterpret_cast<pfKeInitializeApc>(MmGetSystemRoutineAddress(&usKeInitializeApc));

	UNICODE_STRING usKeInsertQueueApc;
	RtlInitUnicodeString(&usKeInsertQueueApc, L"KeInsertQueueApc");
	KeInsertQueueApc = reinterpret_cast<pfKeInsertQueueApc>(MmGetSystemRoutineAddress(&usKeInsertQueueApc));

	if (!KeInitializeApc || !KeInsertQueueApc)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}


