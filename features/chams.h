#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "..\hooks\hooks.h"

enum
{
	MATERIAL_REGULAR,
	MATERIAL_FLAT,
	MATERIAL_GLASS,
	MATERIAL_GLOW,
	MATERIAL_WIREFRAME,
	MATERIAL_MAX 
};


void color_modulate(Color color, IMaterial* material = nullptr);
void color_modulate(float* color, IMaterial* material = nullptr);

struct Attachment_info {
	Attachment_info() {};
	Attachment_info(int team_num, int owner_id) :
		team_num(team_num), owner_id(owner_id) 
	{
	
	};
	int team_num = -1;
	int owner_id = -1;
};
class Chams
{
public:
	crypt_ptr <IMaterial> materials[MATERIAL_MAX];

	virtual crypt_ptr <IMaterial> create_material(bool lit, const std::string& material_data);
	virtual void MaterialOverride(int* mat, float clr[4],bool ignorez = false, bool modifer = false, float alpha_modifer = 1.0f);
	virtual void MaterialOverride(int mat, float clr[4],bool ignorez = false, bool modifer = false, float alpha_modifer = 1.0f);
	virtual void MaterialOverride(IMaterial* mat, float clr[3], float alpha);
	virtual void MaterialReset();
	virtual void initialize();
	virtual IMaterial* GetMaterial(int id);
	std::unordered_map <int, Attachment_info> attachment_info;
	bool was_change = false;
	bool init = false;
};