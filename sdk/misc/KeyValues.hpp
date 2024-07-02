#pragma once
#include "..\..\data.h"

struct IBaseFileSystem
{
	int Read(void* pOutput, int size, void* file)
	{
		using Fn = int(__thiscall*)(void*, void*, int, void*);
		return call_virtual<Fn>(this, 0)(this, pOutput, size, file);
	}
	// if pathID is NULL, all paths will be searched for the file
	void* Open(const char* pFileName, const char* pOptions, const char* pathID = nullptr)
	{
		using Fn = void* (__thiscall*)(void*, const char*, const char*, const char*);
		return call_virtual<Fn>(this, 2)(this, pFileName, pOptions, pathID);
	}
	void Close(void* file)
	{
		using Fn = void(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 3)(this, file);
	}
	unsigned int Size(void* file)
	{
		using Fn = unsigned int(__thiscall*)(void*, void*);
		return call_virtual<Fn>(this, 7)(this, file);
	}
};
using KeyValuesSystemFn = void* (__cdecl*)();
class KeyValues
{
public:
	enum MergeKeyValuesOp_t
	{
		MERGE_KV_ALL,
		MERGE_KV_UPDATE,
		MERGE_KV_DELETE,
		MERGE_KV_BORROW
	};

	enum EKeyTypes : int
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,
		TYPE_COMPILED_INT_0,
		TYPE_COMPILED_INT_1,
		TYPE_NUMTYPES
	};

	void* operator new(std::size_t nAllocSize)
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(GetProcAddress(GetModuleHandleA(crypt_str("vstdlib.dll")), crypt_str("KeyValuesSystem")));

			if (oKeyValuesSystem != nullptr)
				pKeyValuesSystem = oKeyValuesSystem();
		}
		using Fn = void* (__thiscall*)(void*, std::size_t);
		return call_virtual<Fn>(pKeyValuesSystem, 2)(pKeyValuesSystem, nAllocSize);
	}
	void operator delete(void* this_ptr);
	
	void Init(const char* szKeyName);


	void LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* szFileSystem = nullptr, const char* szPathID = nullptr, void* pfnEvaluateSymbolProc = nullptr);

	bool LoadFromFile(const DWORD filesystem, const char* resourceName, const char* pathID = NULL,
		const DWORD pfnEvaluateSymbolProc = NULL);

	void MergeFrom(KeyValues* kvMerge, const MergeKeyValuesOp_t eOp);

	KeyValues* FindKey(const char* szKeyName, bool bCreate);

	void SetString(const char* szKeyName, const char* szValue);

	void SetInt(const char* szKeyName, int iValue)
	{
		KeyValues* pKey = FindKey(szKeyName, true);

		if (pKey == nullptr)
			return;

		*reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(pKey) + 0x10) = iValue;
		*reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(pKey) + 0x14) = TYPE_INT;
	}
	inline void SetBool(const char* szKeyName, bool bValue)
	{
		SetInt(szKeyName, bValue ? 1 : 0);
	}

	void SetFloat(const char* szKeyName, float bValue);

	int GetInt(const char* keyName = NULL, int defaultValue = 0);

	const char* GetString(const char* keyName = NULL, const char* defaultValue = "");
	float GetFloat(const char* keyName = NULL, float defaultValue = 0);
	static KeyValues* fromString(const char* name, const char* value, uintptr_t from_string) noexcept
	{
		KeyValues* keyValues;
		__asm
		{
			push 0
			mov edx, value
			mov ecx, name
			call from_string
			add esp, 4
			mov keyValues, eax
		}
		return keyValues;
	}
private:
	std::byte		pad0[72];
};