#pragma once
#include "..\includes.h"

#define SHA256_SIZE 32

#pragma pack(push)
#pragma pack(1)

template <class T>
struct UNICODE_STRING_T
{
	union
	{
		struct
		{
			WORD Length;
			WORD MaximumLength;
		};

		T dummy;
	};

	T Buffer;
};

template <class T, class NGF, int A>
struct _PEB_T
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE _SYSTEM_DEPENDENT_01;
		};

		T dummy01;
	};

	T Mutant;
	T ImageBaseAddress;
	T Ldr;
	T ProcessParameters;
	T SubSystemData;
	T ProcessHeap;
	T FastPebLock;
	T _SYSTEM_DEPENDENT_02;
	T _SYSTEM_DEPENDENT_03;
	T _SYSTEM_DEPENDENT_04;

	union
	{
		T KernelCallbackTable;
		T UserSharedInfoPtr;
	};

	DWORD SystemReserved;
	DWORD _SYSTEM_DEPENDENT_05;
	T _SYSTEM_DEPENDENT_06;
	T TlsExpansionCounter;
	T TlsBitmap;
	DWORD TlsBitmapBits[2];
	T ReadOnlySharedMemoryBase;
	T _SYSTEM_DEPENDENT_07;
	T ReadOnlyStaticServerData;
	T AnsiCodePageData;
	T OemCodePageData;
	T UnicodeCaseTableData;
	DWORD NumberOfProcessors;

	union
	{
		DWORD NtGlobalFlag;
		NGF dummy02;
	};

	LARGE_INTEGER CriticalSectionTimeout;
	T HeapSegmentReserve;
	T HeapSegmentCommit;
	T HeapDeCommitTotalFreeThreshold;
	T HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	T ProcessHeaps;
	T GdiSharedHandleTable;
	T ProcessStarterHelper;
	T GdiDCAttributeList;
	T LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	T ImageSubsystemMinorVersion;

	union
	{
		T ImageProcessAffinityMask;
		T ActiveProcessAffinityMask;
	};

	T GdiHandleBuffer[A];
	T PostProcessInitRoutine;
	T TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	T SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	T pShimData;
	T AppCompatInfo;
	UNICODE_STRING_T <T> CSDVersion;
	T ActivationContextData;
	T ProcessAssemblyStorageMap;
	T SystemDefaultActivationContextData;
	T SystemAssemblyStorageMap;
	T MinimumStackCommit;
};

typedef _PEB_T <DWORD, DWORD64, 34> PEB32;
typedef _PEB_T <DWORD64, DWORD, 30> PEB64;

#pragma pack(pop)

typedef struct _KSYSTEM_TIME
{
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
}
KSYSTEM_TIME, *PKSYSTEM_TIME;

typedef enum _NT_PRODUCT_TYPE
{
	NtProductWinNt = 1,
	NtProductLanManNt = 2,
	NtProductServer = 3
}
NT_PRODUCT_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
	StandardDesign = 0,
	NEC98x86 = 1,
	EndAlternatives = 2
}
ALTERNATIVE_ARCHITECTURE_TYPE;

typedef struct _KUSER_SHARED_DATA
{
	ULONG TickCountLowDeprecated;
	ULONG TickCountMultiplier;
	KSYSTEM_TIME InterruptTime;
	KSYSTEM_TIME SystemTime;
	KSYSTEM_TIME TimeZoneBias;
	WORD ImageNumberLow;
	WORD ImageNumberHigh;
	WCHAR NtSystemRoot[MAX_PATH];
	ULONG MaxStackTraceDepth;
	ULONG CryptoExponent;
	ULONG TimeZoneId;
	ULONG LargePageMinimum;
	ULONG Reserved2[7];
	NT_PRODUCT_TYPE NtProductType;
	UCHAR ProductTypeIsValid;
	ULONG NtMajorVersion;
	ULONG NtMinorVersion;
	UCHAR ProcessorFeatures[64];
	ULONG Reserved1;
	ULONG Reserved3;
	ULONG TimeSlip;
	ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
	LARGE_INTEGER SystemExpirationDate;
	ULONG SuiteMask;
	UCHAR KdDebuggerEnabled;
	UCHAR NXSupportPolicy;
	ULONG ActiveConsoleId;
	ULONG DismountCount;
	ULONG ComPlusPackage;
	ULONG LastSystemRITEventTickCount;
	ULONG NumberOfPhysicalPages;
	UCHAR SafeBootMode;
	ULONG SharedDataFlags;
	ULONG DbgErrorPortPresent : 1;
	ULONG DbgElevationEnabled : 1;
	ULONG DbgVirtEnabled : 1;
	ULONG DbgInstallerDetectEnabled : 1;
	ULONG SystemDllRelocated : 1;
	ULONG SpareBits : 27;
	UINT64 TestRetInstruction;
	ULONG SystemCall;
	ULONG SystemCallReturn;
	UINT64 SystemCallPad[3];

	union
	{
		KSYSTEM_TIME TickCount;
		UINT64 TickCountQuad;
	};

	ULONG Cookie;
	INT64 ConsoleSessionForegroundProcessId;
	ULONG Wow64SharedInformation[16];
	WORD UserModeGlobalLogger[8];
	ULONG HeapTracingPid[2];
	ULONG CritSecTracingPid[2];
	ULONG ImageFileExecutionOptions;

	union
	{
		UINT64 AffinityPad;
		ULONG ActiveProcessorAffinity;
	};

	UINT64 InterruptTimeBias;
}
KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;

struct KUserSharedDataInformation
{
	WCHAR NtSystemRoot[MAX_PATH];
	UCHAR ProcessorFeatures[64];
	ULONG NumberOfPhysicalPages;
};

struct sha256_buff {
	uint64_t data_size;
	uint32_t h[8];
	uint8_t last_chunk[64];
	uint8_t chunk_size;
};

__forceinline void* _memcpy(void* destination, const void* source, size_t size) {
	auto destination_pointer = (char*)destination;
	auto source_pointer = (const char*)source;

	while (size--)
		*destination_pointer++ = *source_pointer++;

	return destination;
}

__forceinline void sha256_init(struct sha256_buff* buff) {
	buff->h[0] = 0x6a09e667;
	buff->h[1] = 0xbb67ae85;
	buff->h[2] = 0x3c6ef372;
	buff->h[3] = 0xa54ff53a;
	buff->h[4] = 0x510e527f;
	buff->h[5] = 0x9b05688c;
	buff->h[6] = 0x1f83d9ab;
	buff->h[7] = 0x5be0cd19;
	buff->data_size = 0;
	buff->chunk_size = 0;
}

const static uint32_t k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define rotate_r(val, bits) (val >> bits | val << (32 - bits))

__forceinline void sha256_calc_chunk(struct sha256_buff* buff, const uint8_t* chunk) {
	uint32_t w[64];
	uint32_t tv[8];
	uint32_t i;

	for (i = 0; i < 16; ++i) {
		w[i] = (uint32_t)chunk[0] << 24 | (uint32_t)chunk[1] << 16 | (uint32_t)chunk[2] << 8 | (uint32_t)chunk[3];
		chunk += 4;
	}

	for (i = 16; i < 64; ++i) {
		uint32_t s0 = rotate_r(w[i - 15], 7) ^ rotate_r(w[i - 15], 18) ^ (w[i - 15] >> 3);
		uint32_t s1 = rotate_r(w[i - 2], 17) ^ rotate_r(w[i - 2], 19) ^ (w[i - 2] >> 10);
		w[i] = w[i - 16] + s0 + w[i - 7] + s1;
	}

	for (i = 0; i < 8; ++i)
		tv[i] = buff->h[i];

	for (i = 0; i < 64; ++i) {
		uint32_t S1 = rotate_r(tv[4], 6) ^ rotate_r(tv[4], 11) ^ rotate_r(tv[4], 25);
		uint32_t ch = (tv[4] & tv[5]) ^ (~tv[4] & tv[6]);
		uint32_t temp1 = tv[7] + S1 + ch + k[i] + w[i];
		uint32_t S0 = rotate_r(tv[0], 2) ^ rotate_r(tv[0], 13) ^ rotate_r(tv[0], 22);
		uint32_t maj = (tv[0] & tv[1]) ^ (tv[0] & tv[2]) ^ (tv[1] & tv[2]);
		uint32_t temp2 = S0 + maj;

		tv[7] = tv[6];
		tv[6] = tv[5];
		tv[5] = tv[4];
		tv[4] = tv[3] + temp1;
		tv[3] = tv[2];
		tv[2] = tv[1];
		tv[1] = tv[0];
		tv[0] = temp1 + temp2;
	}

	for (i = 0; i < 8; ++i)
		buff->h[i] += tv[i];
}

__forceinline void sha256_update(struct sha256_buff* buff, const void* data, size_t size) {
	const uint8_t* ptr = (const uint8_t*)data;
	buff->data_size += size;
	/* If there is data left in buff, concatenate it to process as new chunk */
	if (size + buff->chunk_size >= 64) {
		uint8_t tmp_chunk[64];
		_memcpy(tmp_chunk, buff->last_chunk, buff->chunk_size);
		_memcpy(tmp_chunk + buff->chunk_size, ptr, 64 - buff->chunk_size);
		ptr += (64 - buff->chunk_size);
		size -= (64 - buff->chunk_size);
		buff->chunk_size = 0;
		sha256_calc_chunk(buff, tmp_chunk);
	}
	/* Run over data chunks */
	while (size >= 64) {
		sha256_calc_chunk(buff, ptr);
		ptr += 64;
		size -= 64;
	}

	/* Save remaining data in buff, will be reused on next call or finalize */
	_memcpy(buff->last_chunk + buff->chunk_size, ptr, size);
	buff->chunk_size += size;
}

__forceinline void sha256_finalize(struct sha256_buff* buff) {
	buff->last_chunk[buff->chunk_size] = 0x80;
	buff->chunk_size++;
	SecureZeroMemory(buff->last_chunk + buff->chunk_size, 64 - buff->chunk_size);

	/* If there isn't enough space to fit int64, pad chunk with zeroes and prepare next chunk */
	if (buff->chunk_size > 56) {
		sha256_calc_chunk(buff, buff->last_chunk);
		SecureZeroMemory(buff->last_chunk, 64);
	}

	/* Add total size as big-endian int64 x8 */
	uint64_t size = buff->data_size * 8;
	int i;
	for (i = 8; i > 0; --i) {
		buff->last_chunk[55 + i] = size & 255;
		size >>= 8;
	}

	sha256_calc_chunk(buff, buff->last_chunk);
}

__forceinline void sha256_read(const struct sha256_buff* buff, uint8_t* hash) {
	uint32_t i;
	for (i = 0; i < 8; i++) {
		hash[i * 4] = (buff->h[i] >> 24) & 255;
		hash[i * 4 + 1] = (buff->h[i] >> 16) & 255;
		hash[i * 4 + 2] = (buff->h[i] >> 8) & 255;
		hash[i * 4 + 3] = buff->h[i] & 255;
	}
}

__forceinline void sha256_easy_hash(const void* data, size_t size, uint8_t* hash) {
	struct sha256_buff buff;
	sha256_init(&buff);
	sha256_update(&buff, data, size);
	sha256_finalize(&buff);
	sha256_read(&buff, hash);
}

__forceinline unsigned long long get_time()
{
	auto kuser_shared_data = (PKUSER_SHARED_DATA)0x7ffe0000;
	auto time = *(unsigned long long*)&kuser_shared_data->SystemTime;

	return time / 10000000 - 11644473600;
}

__forceinline void crypt_bytes(DWORD module_address, uint8_t* bytes, size_t size)
{
	KUserSharedDataInformation kuser_shared_data_information;
	SecureZeroMemory(&kuser_shared_data_information, sizeof(kuser_shared_data_information));

	auto kuser_shared_data = (PKUSER_SHARED_DATA)0x7ffe0000;

	for (auto i = 0; i < MAX_PATH; ++i)
		kuser_shared_data_information.NtSystemRoot[i] = kuser_shared_data->NtSystemRoot[i];

	for (auto i = 0; i < sizeof(kuser_shared_data_information.ProcessorFeatures); ++i)
		kuser_shared_data_information.ProcessorFeatures[i] = kuser_shared_data->ProcessorFeatures[i];

	kuser_shared_data_information.NumberOfPhysicalPages = kuser_shared_data->NumberOfPhysicalPages;

	uint8_t hash[SHA256_SIZE];

	sha256_easy_hash(&kuser_shared_data_information, sizeof(kuser_shared_data_information), hash);
	SecureZeroMemory(&kuser_shared_data_information, sizeof(kuser_shared_data_information));

	auto time = get_time() / 86400;

	for (auto i = 0; i < size; ++i)
	{
		if (!(i % SHA256_SIZE))
		{
			for (auto i = 0; i < SHA256_SIZE; ++i)
			{
				if (i)
					hash[i] += hash[i - 1];
				else
					hash[i] += hash[SHA256_SIZE - 1];
			}
		}

		auto key = hash[i % SHA256_SIZE];

		key ^= ((uint8_t*)&module_address)[i % sizeof(module_address)];
		key ^= ((uint8_t*)&time)[i % sizeof(time)];

		bytes[i] ^= key;
	}

	SecureZeroMemory(hash, SHA256_SIZE);
}