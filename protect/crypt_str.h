#pragma once
#include <type_traits>

template <class _Ty>
using clean_type = typename std::remove_const_t <std::remove_reference_t <_Ty>>;

template <int _size, char _key1, char _key2, typename T>
class crypter
{
public:
	__forceinline constexpr crypter(T* data)
	{
		crypt(data);
	}

	__forceinline int size()
	{
		return _size;
	}

	__forceinline char key()
	{
		return _key1;
	}

	__forceinline T* encrypt()
	{
		if (!is_encrypted())
			crypt(_storage);

		return _storage;
	}

	__forceinline  T* decrypt()
	{
		if (is_encrypted())
			crypt(_storage);

		return _storage;
	}

	__forceinline bool is_encrypted()
	{
		return _storage[_size - 1];
	}

	__forceinline void clear()
	{
		for (auto i = 0; i < _size; i++)
			_storage[i] = 0;
	}

	__forceinline T* get()
	{
		decrypt();
		return _storage;
	}

private:
	__forceinline constexpr void crypt(T* data)
	{
		for (auto i = 0; i < _size; i++)
			_storage[i] = data[i] ^ (_key1 + i % (_key2 + 1));
	}

	T _storage[_size]
	{

	};
};

#ifdef RELEASE
#define crypt_str(s) (char*)crypt_key(s, __TIME__[4], __TIME__[7]).get()
#define crypt_key(s, key1, key2) []() { constexpr static auto crypted = crypter <sizeof(s) / sizeof(s[0]), key1, key2, clean_type <decltype(s[0])>> ((clean_type <decltype(s[0])>*)s); return crypted; }()
#else
#define crypt_str(s) s
#endif