// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\features\features.h"
#include "..\features\exploits.h"

int hooked_processinterpolatedlist()
{
	static auto allow_extrapolation = crypt_ptr <bool> (*(bool**)signatures_manager->signatures[SIGNATURE_ALLOW_EXTRAPOLATION]);

    if (allow_extrapolation)
        *allow_extrapolation.get() = false;

    return ((ProcessInterpolatedList)original_processinterpolatedlist)();
}