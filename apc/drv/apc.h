#pragma once

typedef enum _KAPC_ENVIRONMENT
{
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
}KAPC_ENVIRONMENT, *PKAPC_ENVIRONMENT;

typedef VOID(NTAPI *PKNORMAL_ROUTINE)(
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
	);

typedef VOID KKERNEL_ROUTINE(
	PRKAPC Apc,
	PKNORMAL_ROUTINE *NormalRoutine,
	PVOID *NormalContext,
	PVOID *SystemArgument1,
	PVOID *SystemArgument2
);

typedef KKERNEL_ROUTINE(NTAPI *PKKERNEL_ROUTINE);

typedef VOID(NTAPI *PKRUNDOWN_ROUTINE)(
	PRKAPC Apc
	);

typedef
void (*pfKeInitializeApc)(
	PRKAPC Apc,
	PRKTHREAD Thread,
	KAPC_ENVIRONMENT Environment,
	PKKERNEL_ROUTINE KernelRoutine,
	PKRUNDOWN_ROUTINE RundownRoutine,
	PKNORMAL_ROUTINE NormalRoutine,
	KPROCESSOR_MODE ProcessorMode,
	PVOID NormalContext
);

typedef
BOOLEAN (*pfKeInsertQueueApc)(
	PRKAPC Apc,
	PVOID SystemArgument1,
	PVOID SystemArgument2,
	KPRIORITY Increment
);

PKAPC
CreateApc(
	unsigned long ThreadId,
	PKNORMAL_ROUTINE NormalRoutine
);

void NTAPI
KernelRoutine(
	PKAPC apc,
	PKNORMAL_ROUTINE* NormalRoutine,
	PVOID* NormalContext,
	PVOID* SystemArgument1,
	PVOID* SystemArgument2
);

NTSTATUS
QueueApc(
	PKAPC Apc
);