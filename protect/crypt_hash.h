#pragma once
#include "../includes.h"

class Fnv
{
    static const unsigned int FNV_PRIME = 16777619;
    static const unsigned int OFFSET_BASIS = 2166136261;

    template <unsigned int N>
    constexpr unsigned int fnv_hash_const(const char(&str)[N], unsigned int i = N)
    {
        return i == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnv_hash_const(str, i - 1) ^ str[i - 1]) * FNV_PRIME;
    }

    __forceinline unsigned int str_len(const char* str)
    {
        auto length = 0;

        while (*str++)
            ++length;

        return length;
    }

    __forceinline unsigned int fnv_hash(const char* str)
    {
        auto length = str_len(str) + 1;
        auto hash = OFFSET_BASIS;

        for (auto i = 0; i < length; ++i)
        {
            hash ^= *str++;
            hash *= FNV_PRIME;
        }

        return hash;
    }

    struct Wrapper
    {
        Wrapper(const char* str) : str(str)
        {

        }

        const char* str;
    };

    template <unsigned int N>
    constexpr unsigned int fnv_hash_const(const wchar_t(&str)[N], unsigned int I = N)
    {
        return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnv_hash_const(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
    }

    __forceinline unsigned int wstr_len(const wchar_t* str)
    {
        auto length = 0;

        while (*str++)
            ++length;

        return length;
    }

    __forceinline unsigned int fnv_hash(const wchar_t* str)
    {
        auto length = wstr_len(str) + 1;
        auto hash = OFFSET_BASIS;

        for (auto i = 0; i < length; ++i)
        {
            hash ^= *str++;
            hash *= FNV_PRIME;
        }

        return hash;
    }

    struct WideWrapper
    {
        __forceinline WideWrapper(const wchar_t* str) : str(str)
        {

        }

        const wchar_t* str;
    };

    unsigned int hash_value;
public:
    __forceinline Fnv(Wrapper wrapper) : hash_value(fnv_hash(wrapper.str))
    {

    }

    __forceinline Fnv(WideWrapper wrapper) : hash_value(fnv_hash(wrapper.str))
    {

    }

    template <unsigned int N>
    constexpr Fnv(const char(&str)[N]) : hash_value(fnv_hash_const(str))
    {

    }

    template <unsigned int N>
    constexpr Fnv(const wchar_t(&str)[N]) : hash_value(fnv_hash_const(str))
    {

    }

    constexpr operator unsigned int() const
    {
        return hash_value;
    }
};

template <unsigned int N>
struct const_fnv
{
    constexpr unsigned int get()
    {
        return N;
    }
};

#define crypt_hash(s) const_fnv <Fnv(s)> ().get()
#define crypt_hash_r(s) Fnv(s)

#define crypt_hash_n(s) const_fnv <Fnv(#s)> ().get()
#define crypt_hash_rn(s) Fnv(std::to_string(s).c_str())