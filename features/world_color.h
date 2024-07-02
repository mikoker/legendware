#pragma once
#include "..\globals.h"
#include "..\config\config.h"

class MaterialBackup //-V802
{ //-V802
public:
	MaterialHandle_t handle;
	IMaterial* material;
	float color[3] = { 0 };
	float alpha;
	bool translucent;
	bool nodraw;

	MaterialBackup(MaterialHandle_t h, IMaterial* p)
	{
		handle = h;
		material = p;

		material->GetColorModulation(&color[0], &color[1], &color[2]);
		alpha = material->GetAlphaModulation();

		translucent = material->GetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT);
		nodraw = material->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW);
	}

	virtual void restore()
	{
		material->ColorModulate(color[0], color[1], color[2]);
		material->AlphaModulate(alpha);
		material->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, translucent);
		material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, nodraw);
	}
};

class WorldColor
{
	virtual void modulate(crypt_ptr <IMaterial> material);
public:
	bool force_update = false;
	virtual void apply();
};