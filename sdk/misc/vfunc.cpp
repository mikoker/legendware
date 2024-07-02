#include "vfunc.hpp"

__declspec(noinline) int __fastcall hidden_call_virtual(void* instance, int index)
{
	++(*(char**)_AddressOfReturnAddress());
	return (int)((*(void***)instance)[index]);
}