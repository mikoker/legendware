#pragma once
#include "../../data.h"
class UiPanel
{
public:

	const char* GetPanelId()
	{
		using Fn = const char* (__thiscall*)(void*);
		return call_virtual<Fn>(this, 9)(this);
	}

	UiPanel* GetParent()
	{
		using Fn = UiPanel* (__thiscall*)(void*);
		return call_virtual<Fn>(this, 25)(this);
	}

	void SetVisible(bool state)
	{
		using Fn = void (__thiscall*)(void*,bool);
		return call_virtual<Fn>(this, 27)(this, state);
	}
};

class UiEngine
{
public:

	bool IsValidPanelPtr(const UiPanel* panel)
	{
		using Fn = bool(__thiscall*)(void*, const UiPanel*);
		return call_virtual<Fn>(this, 36)(this, panel);
	}

	UiPanel* GetLastTargetPanel()
	{
		using Fn = UiPanel* (__thiscall*)(void*);
		return call_virtual<Fn>(this, 56)(this);
	}

	int RunScript(UiPanel* panel, const char* js, const char* schema_path, int a5 = 8, int a6 = 10, bool a7 = false, bool a8 = false)
	{
		using Fn = int(__thiscall*)(void*, void*, char const*, char const*, int, int, bool, bool);
		return call_virtual<Fn>(this, 113)(this, panel, js, schema_path, a5, a6, a7, a8);
	}
};

class IPanorama
{
public:
	UiPanel* GetRootPanel()
	{
		static auto engine = this->AccessUiEngine();
		if (!engine)
			return nullptr;

		auto itr = engine->GetLastTargetPanel();
		if (!itr)
			return nullptr;

		UiPanel* ret = nullptr;

		while (itr != nullptr && engine->IsValidPanelPtr(itr))
		{
			std::string t = itr->GetPanelId();
			if (crypt_hash_r(itr->GetPanelId()) == crypt_hash("CSGOHud"))
			{
				ret = itr;
				break;
			}

			if (crypt_hash_r(itr->GetPanelId()) == crypt_hash("CSGOMainMenu"))
			{
				ret = itr;
				break;
			}
			itr = itr->GetParent();
		}

		return ret;
	}

	void RunScript(const char* js)
	{
		auto root_panel = this->GetRootPanel();

		if (!root_panel)
			return;

		static auto engine = this->AccessUiEngine();
		if (!engine) 
			engine = this->AccessUiEngine();
		
		if (!engine)
			return;

		engine->RunScript(root_panel, js, crypt_str("panorama/layout/base_mainmenu.xml"));

	}	

	UiEngine* AccessUiEngine()
	{
		using Fn = UiEngine* (__thiscall*)(void*);
		return call_virtual<Fn>(this, 11)(this);
	}
};
