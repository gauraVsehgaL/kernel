#pragma once
struct ApcInfo
{
	unsigned long ThreadId;
	void *NormalRoutine;
};

#define CTL_CREATE_APC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA) //irp->associrp->sysbuffer & ditto
#define CTL_QUEUE_APC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA) //irp->associrp->sysbuffer & ditto
