#pragma once
#include <intrin.h>

__declspec(noinline) int __fastcall hidden_call_virtual(void* instance, int index);

template <typename T>
__forceinline T call_virtual(void* instance, int index)
{
	__asm
	{
		mov ecx, instance
		mov edx, index
		call hidden_call_virtual
		retn
	}
}