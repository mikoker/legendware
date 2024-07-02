// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"

void __fastcall hooked_shutdown(void* ecx, void* edx)
{
	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
}