// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "chams.h"
#include "..\Lua\Clua.h"
void color_modulate(Color color, IMaterial* material)
{
	if (material)
	{
		auto found = false;
		auto var = material->FindVar(crypt_str("$envmaptint"), &found);

		if (found)
			var->set_vec_value(std::clamp(color.r() / 255.0f, 0.0f, 1.0f), std::clamp(color.g() / 255.0f, 0.0f, 1.0f), std::clamp(color.b() / 255.0f, 0.0f, 1.0f));
	}

	renderview->SetColorModulation(std::clamp(color.r() / 255.0f, 0.0f, 1.0f), std::clamp(color.g() / 255.0f, 0.0f, 1.0f), std::clamp(color.b() / 255.0f, 0.0f, 1.0f));
}

void color_modulate(float* color, IMaterial* material)
{
	if (material)
	{
		auto found = false;
		auto var = material->FindVar(crypt_str("$envmaptint"), &found);

		if (found)
			var->set_vec_value(std::clamp(color[0], 0.0f, 1.0f), std::clamp(color[1], 0.0f, 1.0f), std::clamp(color[2], 0.0f, 1.0f));
	}

	renderview->SetColorModulation(std::clamp(color[0], 0.0f, 1.0f), std::clamp(color[1], 0.0f, 1.0f), std::clamp(color[2], 0.0f, 1.0f));
}

void Chams::MaterialOverride(int mat, float clr[4], bool ignorez, bool modifer, float alpha_modifer)
{
	crypt_ptr<IMaterial> material = nullptr;
	//if (!modifer) {
	//	if (mat > MATERIAL_FLAT + Lua_manager->materials.size())
	//	{
	//		mat = MATERIAL_FLAT + Lua_manager->materials.size(); //-V103
	//	}
	//	if (mat > MATERIAL_FLAT)
	//	{
	//		material = Lua_manager->materials.at(mat - MATERIAL_FLAT - 1).m_material;
	//	}
	//	else
	//	{
	//		material = this->materials[mat];
	//	}
	//}
	//else
	{
		material = this->materials[mat];
	}
	if (material) {
		renderview->SetBlend(std::clamp(clr[3] * alpha_modifer, 0.0f, 1.0f));

		color_modulate(clr, material.get());

		material->IncrementReferenceCount();
		material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
		//material->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
		modelrender->ForcedMaterialOverride(material.get());
	}
}

void Chams::MaterialOverride(IMaterial* mat, float clr[3], float alpha)
{
	renderview->SetBlend(std::clamp(alpha , 0.0f, 1.0f));
	color_modulate(clr, mat);
	mat->IncrementReferenceCount();
	modelrender->ForcedMaterialOverride(mat);
}

crypt_ptr<IMaterial> Chams::create_material(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? crypt_str("VertexLitGeneric") : crypt_str("UnlitGeneric");

	auto matname = crypt_str("legendware_") + std::to_string(created);
	++created;
	KeyValues* pKeyValues = static_cast<KeyValues*>(KeyValues::operator new(sizeof(KeyValues)));
	pKeyValues->Init(type.data());
	pKeyValues->LoadFromBuffer(matname.data(), material_data.c_str());
	return materialsystem->CreateMaterial(matname.c_str(), pKeyValues);
}

void Chams::MaterialOverride(int* mat, float clr[4], bool ignorez, bool modifer, float alpha_modifer)
{
	crypt_ptr<IMaterial> material = nullptr;
	//if (!modifer) {
	//	if (*mat > MATERIAL_FLAT + Lua_manager->materials.size())
	//	{
	//		*mat = MATERIAL_FLAT + Lua_manager->materials.size(); //-V103
	//	}
	//	if (*mat > MATERIAL_FLAT)
	//	{
	//		material = Lua_manager->materials.at(*mat - MATERIAL_FLAT - 1).m_material;
	//	}
	//	else
	//	{
	//		material = this->materials[*mat];
	//	}
	//}
	//else
	{
		material = this->materials[*mat];
	}
	if (material) {
		renderview->SetBlend(std::clamp(clr[3] * alpha_modifer, 0.0f, 1.0f));
		
		color_modulate(clr, material.get());

		material->IncrementReferenceCount();
		material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);
		//material->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
		modelrender->ForcedMaterialOverride(material.get());
	}
}

void Chams::MaterialReset()
{
	modelrender->ForcedMaterialOverride(nullptr);
	renderview->SetColorModulation(1.0f, 1.0f, 1.0f); //-V807
	renderview->SetBlend(1);
}


void Chams::initialize()
{
	if (this->init)
		return;

	materials[MATERIAL_REGULAR] = materialsystem->FindMaterial(crypt_str("debug/debugambientcube"), TEXTURE_GROUP_OTHER);


	materials[MATERIAL_FLAT] = create_material(false, crypt_str(
		R"#("UnlitGeneric"
			{
				"$basetexture"				"vgui/white"
				"$ignorez"					"0"
				"$envmap"					" "
				"$nofog"					"1"
				"$model"					"1"
				"$nocull"					"0"
				"$selfillum"				"1"
				"$halflambert"				"1"
				"$znearer"					"0"
				"$flat"						"1"
				"$wireframe"				"0"
			}
		)#"));


	materials[MATERIAL_GLASS] = materialsystem->FindMaterial(crypt_str("models/inventory_items/trophy_majors/gloss"), TEXTURE_GROUP_OTHER);
	materials[MATERIAL_GLOW] = create_material(true, crypt_str(
		R"#("VertexLitGeneric"
			{
				"$envmap"					"models/effects/cube_white"
				"$envmaptint"				"[1 1 1]"
				"$envmapfresnel"			"1"
				"$envmapfresnelminmaxexp"	"[0 1 2]"
				"$alpha"					"0.8"
				"$nocull"					"1"
			}
		)#"));
	materials[MATERIAL_WIREFRAME] = create_material(true, crypt_str(
		R"#("VertexLitGeneric"
			{
				"$envmap" "models/effects/cube_white"
				"$envmaptint" "[1 1 1]"
				"$envmapfresnel" "1"
				"$envmapfresnelminmaxexp" "[0 1 2]"
				"$alpha" "0.8"
				"$wireframe" "1"
			}
		)#"));
	this->init = true;
}


inline void KeyValues::Init(const char* szKeyName)
{
	using InitKeyValuesFn = void(__thiscall*)(void*, const char*, void*, void*);
	static auto oInitKeyValues = reinterpret_cast<InitKeyValuesFn>(signatures_manager->signatures[SIGNATURE_KEY_VALUES_INIT]); // @xref: "OldParticleSystem_Destroy"

	if (oInitKeyValues == nullptr)
		return;

	oInitKeyValues(this, szKeyName, nullptr, nullptr);
}

void KeyValues::LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* szFileSystem , const char* szPathID, void* pfnEvaluateSymbolProc )
{
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
	static auto oLoadFromBuffer = reinterpret_cast<LoadFromBufferFn>(signatures_manager->signatures[SIGNATURE_LOAD_FROM_BUFFER]); // @xref: "KeyValues::LoadFromBuffer(%s%s%s): Begin"

	if (oLoadFromBuffer == nullptr)
		return ;

	return oLoadFromBuffer(this, szResourceName, szBuffer, szFileSystem, szPathID, pfnEvaluateSymbolProc, nullptr);
}

KeyValues* KeyValues::FindKey(const char* szKeyName, bool bCreate)
{
	using FindKeyFn = KeyValues * (__thiscall*)(KeyValues*, const char*, bool);
	static auto oFindKey = reinterpret_cast<FindKeyFn>(signatures_manager->signatures[SIGNATURE_FIND_KEY]);
	return oFindKey(this, szKeyName, bCreate);
}

void KeyValues::SetFloat(const char* szKeyName, float bValue)
{
	return reinterpret_cast<void(__thiscall*)(void*, const char*, float)>
		(signatures_manager->signatures[SIGNATURE_SET_VALUE_KEY_VALUE])
		(this, szKeyName, bValue);
}

int KeyValues::GetInt(const char* keyName, int defaultValue)
{
	return reinterpret_cast<int(__thiscall*)(void*, const char*, int)>
		(signatures_manager->signatures[SIGNATURE_GET_INT_KEY_VALUE])
		(this, keyName, defaultValue);
}
const char* KeyValues::GetString(const char* keyName, const char* defaultValue)
{
	return reinterpret_cast<const char*(__thiscall*)(void*, const char*, const char*)>
		(signatures_manager->signatures[SIGNATURE_GET_STR_KEY_VALUE])
		(this, keyName, defaultValue);
}

float KeyValues::GetFloat(const char* keyName, float defaultValue)
{
	return reinterpret_cast<float (__thiscall*)(void*, const char*, float)>
		(signatures_manager->signatures[SIGNATURE_GET_FLOAT_KEY_VALUE])
		(this, keyName, defaultValue);
}
void KeyValues::SetString(const char* szKeyName, const char* szValue)
{
	KeyValues* pKey = FindKey(szKeyName, true);

	if (pKey == nullptr)
		return;

	using SetStringFn = void(__thiscall*)(void*, const char*);
	static auto oSetString = reinterpret_cast<SetStringFn>(signatures_manager->signatures[SIGNATURE_SET_STRING]);
	oSetString(pKey, szValue);
}

IMaterial* Chams::GetMaterial(int id)
{
	return this->materials[id].get();
}

bool& IMaterialSystem::DisableRenderTargetAllocationForever()
{
	static auto uDisableRenderTargetAllocationForever = *reinterpret_cast<std::uintptr_t*>(signatures_manager->signatures[SIGNATURE_DISABLE_RENDER_TARGET_ALOCATION_FOREVER]);
	return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uDisableRenderTargetAllocationForever);
}
bool KeyValues::LoadFromFile(const DWORD filesystem, const char* resourceName, const char* pathID,
	const DWORD pfnEvaluateSymbolProc) {
	return reinterpret_cast<bool(__thiscall*)(void*, DWORD, const char*, const char*, DWORD)>
		(signatures_manager->signatures[SIGNATURE_LOAD_FROM_FILE_KEY_VALUE])
		(this, filesystem ? filesystem  : 0, resourceName, pathID, pfnEvaluateSymbolProc);
}

void KeyValues::MergeFrom(KeyValues* kvMerge, const MergeKeyValuesOp_t eOp)
{
	reinterpret_cast<void(__thiscall*)(void*, KeyValues*, MergeKeyValuesOp_t)>(signatures_manager->signatures[SIGNATURE_MERGE_FROM_KEY_VALUE])(this, kvMerge, eOp);
}

void KeyValues::operator delete(void* this_ptr)
{
	reinterpret_cast<void(__thiscall*)(void*, bool)>(signatures_manager->signatures[SIGNATURE_DESTRUCTOR_KEY_VALUE]) (this_ptr, true);
}