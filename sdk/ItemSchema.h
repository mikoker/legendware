#pragma once
#include "../data.h"
#include <cstddef>
#if defined( _WIN32 ) || defined( WIN32 )
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else	//_WIN32
#define PATHSEPARATOR(c) ((c) == '/')
#endif	//_WIN32

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)

#define PAD(size) \
private: \
    std::byte PAD_NAME(__COUNTER__) [size]; \
public:

namespace VirtualMethod
{
	template <typename T, std::size_t Idx, typename ...Args>
	constexpr T call(void* classBase, Args... args) noexcept
	{
		return (*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[Idx](classBase, args...); //-V206
	}
}

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
returnType name args noexcept \
{ \
    return VirtualMethod::call<returnType, idx>argsRaw; \
}

enum class WeaponId : short;

template <typename T>
struct UtlMemory {
	T& operator[](int i) noexcept { return memory[i]; }; //-V302

	T* memory;
	int allocationCount;
	int growSize;
};

template <typename Key, typename Value>
struct Node {
	int left;
	int right;
	int parent;
	int type;
	Key key;
	Value value;
};

template <typename Key, typename Value>
struct UtlMap {
	auto begin() const noexcept { return memory.memory; }
	auto end() const noexcept { return memory.memory + numElements; }

	int find(Key key) const noexcept
	{
		auto curr = root;

		while (curr != -1) {
			 auto el = memory.memory[curr];

			if (el.key < key)
				curr = el.right;
			else if (el.key > key)
				curr = el.left;
			else
				break;
		}
		return curr;
	}

	void* lessFunc;
	UtlMemory<Node<Key, Value>> memory;
	int root;
	int numElements;
	int firstFree;
	int lastAlloc;
	Node<Key, Value>* elements;
};
static unsigned g_nRandomValues[256] =
{
	238,	164,	191,	168,	115,	 16,	142,	 11,	213,	214,	 57,	151,	248,	252,	 26,	198,
	 13,	105,	102,	 25,	 43,	 42,	227,	107,	210,	251,	 86,	 66,	 83,	193,	126,	108,
	131,	  3,	 64,	186,	192,	 81,	 37,	158,	 39,	244,	 14,	254,	 75,	 30,	  2,	 88,
	172,	176,	255,	 69,	  0,	 45,	116,	139,	 23,	 65,	183,	148,	 33,	 46,	203,	 20,
	143,	205,	 60,	197,	118,	  9,	171,	 51,	233,	135,	220,	 49,	 71,	184,	 82,	109,
	 36,	161,	169,	150,	 63,	 96,	173,	125,	113,	 67,	224,	 78,	232,	215,	 35,	219,
	 79,	181,	 41,	229,	149,	153,	111,	217,	 21,	 72,	120,	163,	133,	 40,	122,	140,
	208,	231,	211,	200,	160,	182,	104,	110,	178,	237,	 15,	101,	 27,	 50,	 24,	189,
	177,	130,	187,	 92,	253,	136,	100,	212,	 19,	174,	 70,	 22,	170,	206,	162,	 74,
	247,	  5,	 47,	 32,	179,	117,	132,	195,	124,	123,	245,	128,	236,	223,	 12,	 84,
	 54,	218,	146,	228,	157,	 94,	106,	 31,	 17,	 29,	194,	 34,	 56,	134,	239,	246,
	241,	216,	127,	 98,	  7,	204,	154,	152,	209,	188,	 48,	 61,	 87,	 97,	225,	 85,
	 90,	167,	155,	112,	145,	114,	141,	 93,	250,	  4,	201,	156,	 38,	 89,	226,	196,
	  1,	235,	 44,	180,	159,	121,	119,	166,	190,	144,	 10,	 91,	 76,	230,	221,	 80,
	207,	 55,	 58,	 53,	175,	  8,	  6,	 52,	 68,	242,	 18,	222,	103,	249,	147,	129,
	138,	243,	 28,	185,	 62,	 59,	240,	202,	234,	 99,	 77,	 73,	199,	137,	 95,	165,
};

struct StickerData_t
{
	char	m_szStickerModelPath[128];
	char	m_szStickerMaterialPath[128];
	Vector	m_vWorldModelProjectionStart;
	Vector	m_vWorldModelProjectionEnd;
	char	m_szStickerBoneParentName[32];
};
class CAttribute_String
{
public:
	virtual ~CAttribute_String() = 0;

	int32_t FieldSet{};
	char** Value{};

private:
	int32_t pad0[2]{};
};
union attribute_data_union_t
{
	bool bValue;
	int iValue;
	float fValue;
	Vector vValue[3];
	CAttribute_String* szValue;
};
struct Static_Attrib_t // Get a CEconItemAttributeDefinition* with m_Attributes[static_attrib_t.id]
{
	uint16_t id;
	attribute_data_union_t value;
	bool force_gc_to_generate;
};
struct String {
	UtlMemory<char> buffer;
	int length;

	char* data() noexcept { return buffer.memory; }
};

struct PaintKit {
	int id;
	String name;
	String description;
	String itemName;
	String sameNameFamilyAggregate;
	String pattern;
	String normal;
	String logoMaterial;
	bool baseDiffuseOverride;
	int rarity;
	int style;
	Color rgbaColor[4];
	Color rgbaLogoColor[4];
	float wearDefault;
	float wearRemapMin;
	float wearRemapMax;
	int8_t seed;
	int8_t phongexponent;
	int8_t phongalbedoboost;
	int8_t phongintensity;
	float patternScale;
	float patternOffset_x_start;
	float patternOffset_x_end;
	float patternOffset_y_start;
	float patternOffset_y_end;
	float patternRotateStart;
	float patternRotateEnd;
	float logoScale;
	float logoOffset_x;
	float logoOffset_y;
	float logoRotation;
	bool ignore_weapon_size_scale;
	char pad2[3];
	int view_model_exponent_override_size;
	bool bOnlyFirstMaterial;
	bool use_normal;
	char pad[2];
	float pearlescent;
};

struct StickerKit {
	int id;
	int rarity;
	String name;
	String description;
	String itemName;
	String sMaterialPath;
	String sMaterialPathNoDrips;
	String m_strInventoryImage;


	int m_nEventID;
	int m_nEventTeamID;
	int m_nPlayerID;

	bool bMaterialPathIsAbsolute;

	float flRotateStart;
	float flRotateEnd;

	float flScaleMin;
	float flScaleMax;

	float flWearMin;
	float flWearMax;
	CUtlString m_sIconURLSmall;
	CUtlString m_sIconURLLarge;
	KeyValues* m_pKVItem;
};
struct EconMusicDefinition {
	int id;
	const char* name;
	const char* nameLocalized;
	PAD(2 * sizeof(const char*))
		const char* inventoryImage;
};

struct WeaponPaintableMaterial_t
{
	char Name[128];
	char OrigMat[128];
	char FolderName[128];
	int ViewmodelDim;
	int WorldDim;
	float WeaponLength;
	float UVScale;
	bool BaseTextureOverride;
	bool MirrorPattern;
};
struct InventoryImageData_t
{
	QAngle m_pCameraAngles;
	Vector m_pCameraOffset;
	float m_cameraFOV;
	LightDesc_t m_pLightDesc[4];
	bool m_bOverrideDefaultLight;

};

class EconItemDefinition {
public:


	VIRTUAL_METHOD(WeaponId, getWeaponId, 0, (), (this))
		VIRTUAL_METHOD(const char*, getItemBaseName, 2, (), (this))
		VIRTUAL_METHOD(const char*, getItemTypeName, 3, (), (this))
		VIRTUAL_METHOD(const char*, getInventoryImage, 5, (), (this))
		VIRTUAL_METHOD(const char*, getPlayerDisplayModel, 6, (), (this))
		VIRTUAL_METHOD(const char*, getWorldDisplayModel, 7, (), (this))
		VIRTUAL_METHOD(KeyValues*, getKeyValuse, 10, (), (this))

		VIRTUAL_METHOD(std::uint8_t, getRarity, 12, (), (this))



		VIRTUAL_METHOD(char*, getIconDisplayModel, 38, (), (this))
		VIRTUAL_METHOD(int, getNumSupportedStickerSlots, 44, (), (this))
		VIRTUAL_METHOD(char*, getStickerSlotModelBySlotIndex, 45, (uint32 slot), (this, slot))
		VIRTUAL_METHOD(const char*, getStickerSlotMaterialBySlotIndex, 49, (uint32 slot), (this, slot))
		


	int getCapabilities() noexcept
	{
		return *reinterpret_cast<int*>(this + 0x148); //-V1032
	}

	bool isPaintable() noexcept
	{
		return getCapabilities() & 1; // ITEM_CAP_PAINTABLE
	}

	const char* getDefinitionName() noexcept
	{
		return *reinterpret_cast<const char**>(this + 0x1DC); //-V1032
	}

	int getLoadoutSlot(int team) noexcept
	{
		if (team >= 0 && team <= 3)
			return reinterpret_cast<int*>(std::uintptr_t(this) + 0x28C)[static_cast<int>(team)];
		return *reinterpret_cast<int*>(std::uintptr_t(this) + 0x268);
	}

	CUtlVector< WeaponPaintableMaterial_t >* GetPaintData() {
		return reinterpret_cast<CUtlVector< WeaponPaintableMaterial_t >*>(std::uintptr_t(this) + 0x1B8);
	}

};

struct ItemListEntry {
	int itemDef;
	int paintKit;
	PAD(20)

		auto weaponId() const noexcept
	{
	
		return static_cast<WeaponId>(itemDef);
	}
};

class EconLootListDefinition {
public:
	VIRTUAL_METHOD(const char*, getName, 0, (), (this))
		VIRTUAL_METHOD(const CUtlVector<ItemListEntry>&, getLootListContents, 1, (), (this))
};

class EconItemSetDefinition {
public:
	VIRTUAL_METHOD(const char*, getLocKey, 1, (), (this))
		VIRTUAL_METHOD(int, getItemCount, 4, (), (this))
		VIRTUAL_METHOD(WeaponId, getItemDef, 5, (int index), (this, index))
		VIRTUAL_METHOD(int, getItemPaintKit, 6, (int index), (this, index))
};

struct EconItemQualityDefinition {
	int value;
	const char* name;
	unsigned weight;
	bool explicitMatchesOnly;
	bool canSupportSet;
	const char* hexColor;
};

struct AlternateIconData {
	String simpleName;
	String largeSimpleName;
	String iconURLSmall;
	String iconURLLarge;
	PAD(0x1C)
};
class CPaintKit
{
public:
	enum { NUM_COLORS = 4 };

	// Generic fields
	int id;
	CUtlString sName;
	CUtlString sDescriptionString;
	CUtlString sDescriptionTag;

	CUtlString sPattern;
	CUtlString sLogoMaterial;
	bool bBaseDiffuseOverride;
	int nRarity;
	int nStyle;
	Color rgbaColor[NUM_COLORS];
	Color rgbaLogoColor[NUM_COLORS];
	float flWearDefault;
	float flWearRemapMin;
	float flWearRemapMax;
	unsigned char nFixedSeed;
	unsigned char uchPhongExponent;
	unsigned char uchPhongAlbedoBoost;
	unsigned char uchPhongIntensity;
	float flPatternScale;
	float flPatternOffsetXStart;
	float flPatternOffsetXEnd;
	float flPatternOffsetYStart;
	float flPatternOffsetYEnd;
	float flPatternRotateStart;
	float flPatternRotateEnd;
	float flLogoScale;
	float flLogoOffsetX;
	float flLogoOffsetY;
	float flLogoRotation;
	bool bIgnoreWeaponSizeScale;
	int nViewModelExponentOverrideSize;
	bool bOnlyFirstMaterial;

	// Character paint kit fields
	CUtlString sVmtPath;
	KeyValues* kvVmtOverrides;
};

class ItemSchema {
public:
	PAD(0x88)
	UtlMap<int, EconItemQualityDefinition> qualities;
	PAD(0x48)
	UtlMap<int, EconItemDefinition*> itemsSorted;
	PAD(0x104)
	UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
	PAD(0x48)
	UtlMap<int, PaintKit*> paintKits;
	UtlMap<int, StickerKit*> stickerKits;
	PAD(0x11C)
	UtlMap<int, EconMusicDefinition*> musicKits;

	VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionInterface, 4, (WeaponId id), (this, id))
	VIRTUAL_METHOD(const char*, getRarityName, 19, (uint8_t rarity), (this, rarity))
	VIRTUAL_METHOD(int, getItemSetCount, 28, (), (this))
	VIRTUAL_METHOD(EconItemSetDefinition*, getItemSet, 29, (int index), (this, index))
	VIRTUAL_METHOD(EconLootListDefinition*, getLootList, 32, (int index), (this, index))
	VIRTUAL_METHOD(int, getLootListCount, 34, (), (this))
	VIRTUAL_METHOD(EconItemDefinition*, getItemDefinitionByName, 42, (const char* name), (this, name))

	PaintKit* GetPaintKitDefinition(int painkit_id)
	{	
		return reinterpret_cast <PaintKit*(__thiscall*)(void*, unsigned int)> (signatures_manager->signatures[SIGNATURE_GET_PAINT_KIT_DEFINITION])(this, painkit_id);
	}
	const StickerKit* GetStickerKitDefinition(int iStickerKitID);

};

class ItemSystem {
public:
	VIRTUAL_METHOD(ItemSchema*, getItemSchema, 0, (), (this))
};
