// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "vfunc_hook.hpp"

vmthook::vmthook()  //-V730
{
	
}

vmthook::vmthook(crypt_ptr <PDWORD> ppdwClassBase) 
{
	initialize(ppdwClassBase);
}

vmthook::~vmthook() 
{
	unhook();
}

bool vmthook::initialize(crypt_ptr <PDWORD> ppdwClassBase)
{
	m_ClassBase = ppdwClassBase;
	m_OldVT = *ppdwClassBase.get();
	m_VTSize = get_vt_count(*ppdwClassBase.get());

	m_NewVT = new DWORD[m_VTSize + 1]; //-V121
	memcpy(&m_NewVT.get()[1], m_OldVT.get(), sizeof(DWORD) * m_VTSize);
	m_NewVT.get()[0] = (uintptr_t)m_OldVT.get()[-1]; //-V201 //-V103

	DWORD old;
	VirtualProtect(ppdwClassBase.get(), sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
	*ppdwClassBase.get() = &m_NewVT.get()[1];
	VirtualProtect(ppdwClassBase.get(), sizeof(DWORD), old, &old);

	return true;
}

void vmthook::unhook() 
{
	if (m_ClassBase)
	{
		DWORD old;
		VirtualProtect(m_ClassBase.get(), sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*m_ClassBase.get() = m_OldVT.get();
		VirtualProtect(m_ClassBase.get(), sizeof(DWORD), old, &old);
	}
}

DWORD vmthook::hook_function(DWORD dwNewFunc, unsigned int iIndex) 
{
	unsigned int index = 0;

	while (true)
	{
		if (crypt_hash_rn(index) == iIndex)
			break;

		++index;
	}

	if (m_NewVT && m_OldVT && index <= m_VTSize)
	{
		m_NewVT.get()[index + 1] = dwNewFunc;
		return m_OldVT.get()[index];
	}

	return 0;
}

DWORD vmthook::get_vt_count(crypt_ptr <DWORD> pdwVMT)
{
	DWORD index;

	for (index = 0; pdwVMT.get()[index]; ++index)
		if (IS_INTRESOURCE((FARPROC)pdwVMT.get()[index]))
			break;

	return index;
}