#pragma once

#include "../devkit/includes.h"



/* tabs */
#include "tabs/legit.h"
#include "tabs/rage.h"
#include "tabs/visuals.h"
#include "tabs/misc.h"
#include "tabs/skins.h"
#include "tabs/settings.h"
#include "tabs/plist.h"
#include "tabs/lua.h"
#include "tabs/antiaim.h"
#include "../../data.h"

namespace core
{
	enum class
		e_team : int
	{
		none = -1,
		ct = 0,
		t,
		both
	};
	enum class
		e_def_index : int
	{
		WEAPON_NONE,
		WEAPON_DEAGLE,
		WEAPON_ELITE,
		WEAPON_FIVESEVEN,
		WEAPON_GLOCK,
		WEAPON_AK47 = 7,
		WEAPON_AUG,
		WEAPON_AWP,
		WEAPON_FAMAS,
		WEAPON_G3SG1,
		WEAPON_GALILAR = 13,
		WEAPON_M249,
		WEAPON_M4A1 = 16,
		WEAPON_MAC10,
		WEAPON_P90 = 19,
		WEAPON_ZONE_REPULSOR,
		WEAPON_MP5SD = 23,
		WEAPON_UMP45,
		WEAPON_XM1014,
		WEAPON_BIZON,
		WEAPON_MAG7,
		WEAPON_NEGEV,
		WEAPON_SAWEDOFF,
		WEAPON_TEC9,
		WEAPON_TASER,
		WEAPON_HKP2000,
		WEAPON_MP7,
		WEAPON_MP9,
		WEAPON_NOVA,
		WEAPON_P250,
		WEAPON_SHIELD,
		WEAPON_SCAR20,
		WEAPON_SG553,
		WEAPON_SSG08,
		WEAPON_KNIFEGG,
		WEAPON_KNIFE,
		WEAPON_FLASHBANG,
		WEAPON_HEGRENADE,
		WEAPON_SMOKEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE,
		WEAPON_C4,
		WEAPON_HEALTHSHOT = 57,
		WEAPON_KNIFE_T = 59,
		WEAPON_M4A1_SILENCER,
		WEAPON_USP_SILENCER,
		WEAPON_CZ75A = 63,
		WEAPON_REVOLVER,
		WEAPON_TAGRENADE = 68,
		WEAPON_FISTS,
		WEAPON_BREACHCHARGE,
		WEAPON_TABLET = 72,
		WEAPON_MELEE = 74,
		WEAPON_AXE,
		WEAPON_HAMMER,
		WEAPON_SPANNER = 78,
		WEAPON_KNIFE_GHOST = 80,
		WEAPON_FIREBOMB,
		WEAPON_DIVERSION,
		WEAPON_FRAG_GRENADE,
		WEAPON_SNOWBALL,
		WEAPON_BUMPMINE,
		WEAPON_KNIFE_BAYONET = 500,
		WEAPON_KNIFE_CSS = 503,
		WEAPON_KNIFE_FLIP = 505,
		WEAPON_KNIFE_GUT,
		WEAPON_KNIFE_KARAMBIT,
		WEAPON_KNIFE_M9_BAYONET,
		WEAPON_KNIFE_TACTICAL,
		WEAPON_KNIFE_FALCHION = 512,
		WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
		WEAPON_KNIFE_BUTTERFLY,
		WEAPON_KNIFE_PUSH,
		WEAPON_KNIFE_CORD,
		WEAPON_KNIFE_CANIS,
		WEAPON_KNIFE_URSUS,
		WEAPON_KNIFE_GYPSY_JACKKNIFE,
		WEAPON_KNIFE_OUTDOOR,
		WEAPON_KNIFE_STILETTO,
		WEAPON_KNIFE_WIDOWMAKER,
		WEAPON_KNIFE_SKELETON = 525,
		GLOVE_STUDDED_BLOODHOUND = 5027,
		GLOVE_T_SIDE = 5028,
		GLOVE_CT_SIDE = 5029,
		GLOVE_SPORTY = 5030,
		GLOVE_SLICK = 5031,
		GLOVE_LEATHER_WRAP = 5032,
		GLOVE_MOTORCYCLE = 5033,
		GLOVE_SPECIALIST = 5034,
		GLOVE_HYDRA = 5035,
		//		agent
				/* etc */
	};
	inline bool is_agent(const std::string& index)
	{
		if (strstr(index.c_str(), crypt_str("customplayer")))
			return true;
		return false;//return index == e_def_index::agent;
	}

	inline bool is_music_kit(const int index)
	{
		return index == MUSIC_KIT;
	}


	inline bool is_knife(const e_def_index& index)
	{
		if ((int)index >= WEAPON_KNIFE_BAYONET && (int)index <= WEAPON_KNIFE_SKELETON)
		{
			return true;
		}
		return false;
	}


	inline bool is_glove(const e_def_index& index)
	{

		if ((int)index == GLOVE_BROKENFANG || ((int)index >= GLOVE_STUDDED_BLOODHOUND && (int)index <= GLOVE_HYDRA))
		{
			return true;
		}

		return false;
	}

	struct s_skin_info
	{
		~s_skin_info()
		{
			/*if (this->image) {
				this->image->Release();
				this->image = nullptr;
			}
			if (!this->stickers.empty()) {
				for (int i = 0; i <= 3; i++)
				{
					if (this->stickers.at(i))
					{
						this->stickers[i]->Release();
						this->stickers[i] = nullptr;
					}
				}
			}*/
		}

		s_skin_info() = default;
		s_skin_info(uint64_t uid, IDirect3DTexture9* image, int rarity, e_team team,
			const std::string& weapon, const std::string& skin, bool stattrak,
			int stattrak_v, bool stattrak_auto_update, int seed, float wear, const e_def_index& index,
			const std::vector<IDirect3DTexture9*>& stickers, const std::string& path = "", int kit = 0, void* item = nullptr, const std::vector<int>& stickers_ids = {}, int mask_id = 0, std::string name_tag = "",float colors[4][4] = nullptr)
		{

		}

		s_skin_info(s_skin_info& skin, IDirect3DDevice9* device)
		{

			

		}

		bool can_create_preview()
		{
			return !is_music_kit((int)index) && !is_glove(index) && !is_agent(path_preview);
		}
		uint64_t uid = 0;
		int paint_kit = 0;

		std::string path_preview;
		IDirect3DTexture9* image = nullptr;

		int rarity = 0;
		e_team team = e_team::ct;

		std::string weapon = crypt_str("none");
		std::string skin = crypt_str("none");

		bool stattrak = false;
		int stattrak_v = 0;
		bool stattrak_auto_update = false;

		int seed = 0;
		float wear = 0.0f;

		int mask_id = 0;

		e_def_index index = e_def_index::WEAPON_NONE;

		std::vector<IDirect3DTexture9*> stickers;

		std::vector<int> stickers_id;

		void* item_ptr = nullptr;

		int slot_use_t = -1;
		int slot_use_ct = -1;
		int slot_use_no_team = -1;

		char name_tag[64] = { 0 };

		float colors[4][4] = { 0 };

	};
	inline core::s_skin_info current_skin_preview;
	inline core::e_team get_weapon_team(core::e_def_index weapon)
	{
		if (weapon == core::e_def_index::WEAPON_USP_SILENCER ||
			weapon == core::e_def_index::WEAPON_HKP2000 ||
			weapon == core::e_def_index::WEAPON_FIVESEVEN ||
			weapon == core::e_def_index::WEAPON_MP9 ||
			weapon == core::e_def_index::WEAPON_FAMAS ||
			weapon == core::e_def_index::WEAPON_M4A1 ||
			weapon == core::e_def_index::WEAPON_M4A1_SILENCER ||
			weapon == core::e_def_index::WEAPON_AUG ||
			weapon == core::e_def_index::WEAPON_SCAR20 ||
			weapon == core::e_def_index::WEAPON_MAG7)
			return core::e_team::ct;
		else if (
			weapon == core::e_def_index::WEAPON_GLOCK ||
			weapon == core::e_def_index::WEAPON_TEC9 ||
			weapon == core::e_def_index::WEAPON_MAC10 ||
			weapon == core::e_def_index::WEAPON_GALILAR ||
			weapon == core::e_def_index::WEAPON_AK47 ||
			weapon == core::e_def_index::WEAPON_SG553 ||
			weapon == core::e_def_index::WEAPON_G3SG1 ||
			weapon == core::e_def_index::WEAPON_SAWEDOFF)
			return core::e_team::t;
		else
			return core::e_team::both;
	}




	/* fonts */
	namespace f
	{

		inline ImFont* dinpro = nullptr;
		inline ImFont* dinpro_s = nullptr;
		inline ImFont* dinpro_l = nullptr;
		inline ImFont* dinpro_m = nullptr;

		inline ImFont* dinpro_bold = nullptr;

		inline ImFont* weapon_icons = nullptr;

		inline ImFont* icons = nullptr;
		extern ImFont* awesome;

		inline ImFont* icons_weapon = nullptr;
		inline ImFont* esp_font = nullptr;
		inline ImFont* logs_font = nullptr;

		inline ImFont* montserrat = nullptr;
	}

	/* images */
	namespace i
	{
		inline IDirect3DTexture9* logo = nullptr;
		inline IDirect3DTexture9* small_logo = nullptr;

		inline IDirect3DTexture9* hitboxes[6];
		inline IDirect3DTexture9* hitboxes_ph[6];

		inline IDirect3DTexture9* osok_sticker = nullptr;
		inline int sticker_id[4] = { 0,0,0,0 };
		inline int sticker_rarity[4] = { 0,0,0,0 };
		inline std::string sticker_path[4];
		inline IDirect3DTexture9* stickers[4]{};

		inline IDirect3DTexture9* pfp = nullptr;
		inline IDirect3DTexture9* large_logo = nullptr;
		inline IDirect3DTexture9* playerlist_ph = nullptr;
	}

	/* vars */
	namespace v
	{
		/* menu vars */
		inline const char* smooth[] =
		{
			"None",
			"Static",
			"Dynamic"
		};

		inline const char* body_aim[] =
		{
			"None",
			"Prefer",
			"Force"
		};

		inline const char* double_tap_speed[] =
		{
			"Low",
			"Medium",
			"High"
		};

		inline const char* modifiers[] =
		{
			"Move between shots",
			"Force accuracy",
			"Early"
		};

		inline const char* pitch[] =
		{
			"None",
			"Minimal",
			"Maximal"
		};

		inline const char* target_yaw[] =
		{
			"Local view",
			"At targets"
		};

		inline const char* yaw_modifier[] =
		{
			"Static",
			"Jitter",
			"Spin",
			"Three-way jitter"
		};

		inline const char* freestanding_yaw_modifier[] =
		{
			"None",
			"Static",
			"Jitter",
			"Spin",
			"Three-way jitter"
		};

		inline const char* desync_type[] =
		{
			"None",
			"Static",
			"Jitter",
			"Flick"
		};

		inline const char* fake_lag_type[] =
		{
			"Static",
			"Dynamic",
			"Fluctuate"
		};

		inline const char* leg_movement[] =
		{
			"None",
			"Avoid slide",
			"Force slide"
		};

		inline const char* automatic_strafes[] =
		{
			"None",
			"Default",
			"Directional"
		};

		inline const char* logs[] =
		{
			"Hits",
			"Misses",
			"Hurts",
			"Purchases"
		};

		inline const char* logs_position[] =
		{
			"Screen",
			"Console"
		};

		inline const char* automatic_peek_mode[] =
		{
			"Retreat on shot",
			"Retreat on key"
		};

		inline const char* bypasses[] =
		{
			"Convar sv_cheats",
			"Convar sv_pure",
			"Inventory restriction"
		};

		/* ��������� �� ���� */
		inline int legit_head_pr = 0, legit_body_pr = 0, legit_pelvis_pr = 0,
			legit_legs_pr = 0, legit_feet_pr = 0, legit_hands_pr = 0;
	}

	/* settings */
	namespace s
	{
		const int main_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

		const int main_focus_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

		const int flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		const int editor_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;

		const int popups_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;

		const int flags_nm = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;

		const int skins_tab_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoBringToFrontOnFocus;
		inline int tab = 1;
		inline int vis_subtab = 0;
		inline int legit_subtab = 0;
		inline int rage_subtab = 0;
		inline int settings_subtab = 0;
		inline int visuals_world_subtab = 0;
		inline bool is_opened = true;

		inline ImVec2 win_size = ImVec2(500.0f, 500.0f);

		inline int dpi_scale = 2;
		extern int temp_dpi_scale;

		inline float menu_alpha = 255.0f;
		inline float tab_alpha = 0.0f;

		inline float keybind_alpha = 255.0f;
		inline float spectatorlist_alpha = 255.0f;
		inline float watermark_alpha = 255.0f;

		inline bool is_taking_pixel = false;
		inline bool is_theme_dark = false;

		inline int g_slot = 0;
		inline std::vector<s_skin_info> g_items{};
	}

	/* animatons */
	namespace a
	{
		inline float poses[9] = {
			900.0f,  /* legit */
			900.0f,  /* rage */
			900.0f,  /* visuals */
			900.0f,  /* misc */
			900.0f,  /* skins */
			900.0f,  /* settings */
			900.0f,   /* plist */
			900.0f  /* scripts */
		};

		inline float subposes[9] = {
			-100.0f,
			-100.0f, /* rage */
			-100.0f, /* visuals */
			-100.0f,
			-100.0f,
			-100.0f, /* settings */
			-100.0f,
			-100.0f  /* scripts */
		};

		inline void reset_all_except(
			const int tab)
		{
			for (int i = 0; i < 9; i++)
			{
				if (i == tab)
					continue;

				poses[i] = 900.0f;
				subposes[i] = -100.0f;
			}

			s::tab_alpha = 0.0f;
		}
	}

	/* preview */
	namespace p
	{
		/* ��������� �� ���� */

		/* enemies */
		namespace en
		{
			inline bool esp_enable = true;
			inline bool esp_box = false;

			inline bool esp_hp_bar = true;
			inline bool esp_hp_bar_custom_color = false;

			inline float esp_hp_bar_color[4] = { 0.04f, 0.65f, 0.12f, 1.0f };

			inline bool esp_skeleton = true;

			inline bool esp_flags = true;
			inline bool esp_flags_arr[] = {
				true, true, true, false, false,
			};

			inline const char* esp_flags_combo[] = {
				"Has Defuser / C4", "Money",
				"Scoped", "Defuse / Plant", "Flashed"
			};

			inline float esp_flags_combo_clr[][4] = {
				{ 1.0f, 0.2f, 0.7f, 1.0f },
				{ 0.1f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.5f, 0.1f, 1.0f },
				{ 0.5f, 0.3f, 1.0f, 1.0f },
				{ 0.3f, 0.1f, 0.6f, 1.0f }
			};

			inline bool esp_weapon = true;
			inline bool esp_weapon_arr[2] = { false, false };
			inline const char* esp_weapon_combo[2] = { "Icon", "Text" };

			inline bool esp_armor = true;
			inline bool esp_armor_arr[2];

			inline bool esp_ammo = true;
			inline bool esp_ammo_arr[2];
		}

		/* local */
		namespace lc
		{
			inline bool esp_enable = true;
			inline bool esp_box = false;

			inline bool esp_hp_bar = true;
			inline bool esp_hp_bar_custom_color = false;

			inline float esp_hp_bar_color[4] = { 0.04f, 0.65f, 0.12f, 1.0f };

			inline bool esp_skeleton = true;

			inline bool esp_flags = true;
			inline bool esp_flags_arr[] = {
				true, true, true, false, false,
			};

			inline const char* esp_flags_combo[] = {
				"Has Defuser / C4", "Money",
				"Scoped", "Defuse / Plant", "Flashed"
			};

			inline float esp_flags_combo_clr[][4] = {
				{ 1.0f, 0.2f, 0.7f, 1.0f },
				{ 0.1f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.5f, 0.1f, 1.0f },
				{ 0.5f, 0.3f, 1.0f, 1.0f },
				{ 0.3f, 0.1f, 0.6f, 1.0f }
			};

			inline bool esp_weapon = true;
			inline bool esp_weapon_arr[2] = { false, false };
			inline const char* esp_weapon_combo[2] = { "Icon", "Text" };

			inline bool esp_armor = true;
			inline bool esp_armor_arr[2];

			inline bool esp_ammo = true;
			inline bool esp_ammo_arr[2];
		}

		/* teammates */
		namespace tm
		{
			inline bool esp_enable = true;
			inline bool esp_box = false;

			inline bool esp_hp_bar = true;
			inline bool esp_hp_bar_custom_color = false;

			inline float esp_hp_bar_color[4] = { 0.04f, 0.65f, 0.12f, 1.0f };

			inline bool esp_skeleton = true;

			inline bool esp_flags = true;
			inline bool esp_flags_arr[] = {
				true, true, true, false, false,
			};

			inline const char* esp_flags_combo[5] = {
				"Has Defuser / C4", "Money",
				"Scoped", "Defuse / Plant", "Flashed"
			};

			inline float esp_flags_combo_clr[][4] = {
				{ 1.0f, 0.2f, 0.7f, 1.0f },
				{ 0.1f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.5f, 0.1f, 1.0f },
				{ 0.5f, 0.3f, 1.0f, 1.0f },
				{ 0.3f, 0.1f, 0.6f, 1.0f }
			};

			inline bool esp_weapon = true;
			inline bool esp_weapon_arr[2] = { false, false };
			inline const char* esp_weapon_combo[2] = { "Icon", "Text" };

			inline bool esp_armor = true;
			inline bool esp_armor_arr[2];

			inline bool esp_ammo = true;
			inline bool esp_ammo_arr[2];
		}

		/* world */
		namespace wd
		{
			inline bool esp_enable = true;
			inline bool esp_box = false;

			inline bool esp_ammo = false;
			inline bool esp_ammo_arr[2] = { false, false };
			inline const char* esp_ammo_combo[2] = { "Bar", "Text" };
		}
	}
}