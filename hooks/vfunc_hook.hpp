#pragma once
#include "..\includes.h"

class vmthook
{
public:
	vmthook();
	vmthook(crypt_ptr <PDWORD> ppdwClassBase);
	virtual ~vmthook();
	 
	virtual bool initialize(crypt_ptr <PDWORD> ppdwClassBase);
	virtual void unhook();

	DWORD hook_function(DWORD dwNewFunc, unsigned int iIndex);

	template <typename Fn>
	Fn get_func_address(unsigned int iIndex)
	{
		unsigned int index = 0;

		while (true)
		{
			if (crypt_hash_rn(index) == iIndex)
				break;

			++index;
		}

		if (index <= (int)m_VTSize && m_OldVT)
			return (Fn)m_OldVT.get()[index];

		return nullptr;
	}
private:
	virtual DWORD get_vt_count(crypt_ptr <DWORD> pdwVMT);

	crypt_ptr <PDWORD> m_ClassBase;
	crypt_ptr <DWORD> m_NewVT, m_OldVT;
	DWORD m_VTSize;
};