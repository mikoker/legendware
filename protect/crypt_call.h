#pragma once

__declspec(naked) inline void modify_return_address()
{
    __asm
    {
        add dword ptr ss:[esp], 0x1
        ret
    }
}

__forceinline void crypt_call()
{
    __asm
    {
        call modify_return_address
        ret
    }
}

#define crypt_call crypt_call()