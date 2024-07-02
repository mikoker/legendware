#include "settings.h"
#include "..\imgui_ex.h"
#include "..\..\features\features.h"
#include "..\..\features\logs.h"
#include "..\..\Lua\Clua.h"
#include "..\imgui_ex\lua_editor.h"

namespace core::tabs::settings
{
	/* configs tab */
	std::vector<std::string> files;
	static char name[256]{};
	static int loaded_config;

	static int process_config;
	bool is_popUp_save = false;
	bool is_popUp_del = false;

	void popUp_save()
	{
		float size_x = ImGui::GetWindowSize().x;
		ImGui::SetNextWindowSize(ImVec2(305.0f, 155.0f));
		ImGui::SetNextWindowPos(ImGui::GetWindowPos() + ImVec2((
			size_x - 305.0f) / 2.0f, 100.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowShadow,
			ImGui::GetColorU32(ImGuiCol_WindowShadow, 0.2f));

		ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
			IM_COL32A(26, 26, 26) : IM_COL32A(245, 245, 245));

		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 5));

		ImGui::OpenPopup(crypt_str("save_cfg"));



		if (ImGui::BeginPopupModal(crypt_str("save_cfg"), nullptr, core::s::main_focus_flags | ImGuiWindowFlags_NoMove))
		{
			ImGui::GetWindowDrawList()->AddText(core::f::dinpro_bold, 18.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 25.0f),
				ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Save config?"));

			ImGui::GetWindowDrawList()->AddText(core::f::dinpro, 17.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 55.0f),
				ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Are you sure you want to save the config,\nis this action irreversible?"));

			ImGui::SetCursorPos(ImVec2(25.0f, 100.0f));
			ImGui::BeginGroup();
			{
				if (imgui_ex::button_lua(crypt_str("No"), ImVec2(120.0f, 22.0f), true))
				{

					is_popUp_save = false;
				}

				ImGui::SameLine();

				if (imgui_ex::button_lua(crypt_str("Yes"), ImVec2(120.0f, 22.0f), false))
				{
					if (std::string(name).empty()) {
						std::thread([]() {
							config->send_cfg(config->selected_config);
							}).detach();
					}
					else {
						std::thread([]() {
							config->send_cfg(config->selected_config, name);
							}).detach();
					}
					is_popUp_save = false;
				}
			}

			ImGui::EndGroup();
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(3);
	}

	void popUp_del() {
		float size_x = ImGui::GetWindowSize().x;
		ImGui::SetNextWindowSize(ImVec2(305.0f, 155.0f));
		ImGui::SetNextWindowPos(ImGui::GetWindowPos() + ImVec2((
			size_x - 305.0f) / 2.0f, 100.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowShadow,
			ImGui::GetColorU32(ImGuiCol_WindowShadow, 0.2f));

		ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
			IM_COL32A(26, 26, 26) : IM_COL32A(245, 245, 245));

		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 5));

		ImGui::OpenPopup(crypt_str("del_cfg"));



		if (ImGui::BeginPopupModal(crypt_str("del_cfg"), nullptr, core::s::main_focus_flags | ImGuiWindowFlags_NoMove))
		{
			ImGui::GetWindowDrawList()->AddText(core::f::dinpro_bold, 18.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 25.0f),
				ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Delete config?"));

			ImGui::GetWindowDrawList()->AddText(core::f::dinpro, 17.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 55.0f),
				ImGui::GetColorU32(ImGuiCol_Text), crypt_str("Are you sure you want to delete the \nconfig,is this action irreversible ?"));

			ImGui::SetCursorPos(ImVec2(25.0f, 100.0f));
			ImGui::BeginGroup();
			{
				if (imgui_ex::button_lua(crypt_str("No"), ImVec2(120.0f, 22.0f), true))
				{
					is_popUp_del = false;
				}

				ImGui::SameLine();

				if (imgui_ex::button_lua(crypt_str("Yes"), ImVec2(120.0f, 22.0f), false))
				{
					std::thread([]() {
						config->delete_cfg(config->selected_config);
						}).detach();

						is_popUp_del = false;
				}
			}

			ImGui::EndGroup();
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(3);
	}

	void configs_first_tab()
	{
		if (is_popUp_save)
			popUp_save();

		if (is_popUp_del)
			popUp_del();


		files.clear();

		for (auto i = 0; i < config->cfgs.size(); i++)
		{

			if (i == process_config && config->processing)
				files.emplace_back(config->cfgs.at(i).name + config->process);
			else if (config->m_loaded.id == config->cfgs.at(i).id)
				files.emplace_back(std::string(config->cfgs.at(i).name) + crypt_str(" [loaded]"));
			else
				files.emplace_back(config->cfgs.at(i).name);
		}

		ImGui::SetCursorPos(ImVec2(3.0f, 15.0f));
		ImGui::BeginGroup();
		{
			for (size_t i = 0u; i < files.size(); i++)
			{
				const bool is_selected = config->selected_config == i;
				if (imgui_ex::selectable(files.at(i).c_str(), is_selected, 0, ImVec2(0.0f, 0.0f), 0,i))
				{
					config->selected_config = i;
				}
			}
		}
		ImGui::Spacing();
		ImGui::EndGroup();
	}

	void configs_second_tab()
	{
		imgui_ex::input_text(crypt_str("Name"), name, 32, ImVec2(491.0f, 30.0f));
		{

		}
		static bool is_locked = false;
		if (!config->processing && config->selected_config < config->cfgs.size()) {
			if (imgui_ex::button(crypt_str("Load"), ImVec2(240.0f, 30.0f)))
			{
				if (!config->processing && config->selected_config < config->cfgs.size())
				{
					process_config = config->selected_config;
					loaded_config = config->selected_config;
					std::thread([&]() {
						config->load_cfg(config->selected_config);
						}).detach();
				}
			}
		}
		else
			imgui_ex::button_disabled(crypt_str("Load"), ImVec2(240.0f, 30.0f));
		ImGui::SameLine();
		imgui_ex::button_lock(crypt_str("Lock saving and deleting"), ImVec2(240.0f, 30.0f), is_locked);

		if (config->processing || (!config->cfgs.size() || config->selected_config > config->cfgs.size() || config->selected_config < 0))
			imgui_ex::button_disabled(crypt_str("Save"), ImVec2(240.0f, 30.0f));
		else {

			if (std::string(name).empty()) {
				if (imgui_ex::button(crypt_str("Save"), ImVec2(240.0f, 30.0f)))
				{
					if (!config->processing && config->selected_config < config->cfgs.size())
					{
						process_config = config->selected_config;
						if (is_locked) {
							is_popUp_save = true;
						}
						else
							std::thread(&Config::send_cfg, config.get(), config->selected_config, "").detach();
					}
				}
				if (config->selected_config < config->cfgs.size() && (config->m_loaded.author != -1 && config->cfgs.at(config->selected_config).author != config->m_loaded.author))
				{
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
							ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
						ImGui::SetTooltip(crypt_str("If you save the contents of this config to another config, you cannot share it because you are not its author"));
						ImGui::PopStyleVar();
						ImGui::PopStyleColor();
					}
				}
			}
			else {
				if (imgui_ex::button(crypt_str("Save and rename"), ImVec2(240.0f, 30.0f)))
				{
					if (!config->processing && config->selected_config < config->cfgs.size())
					{
						process_config = config->selected_config;
						if (is_locked) {
							is_popUp_save = true;
						}
						else {
							std::thread(&Config::send_cfg, config.get(), config->selected_config, std::string(name)).detach();
						}
						memset(name, 0, ARRAYSIZE(name));
					}
				}
				if (config->selected_config < config->cfgs.size() && config->m_loaded.author != -1 && config->cfgs.at(config->selected_config).author != ctx->user_id)
				{
					if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
							ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
						ImGui::SetTooltip(crypt_str("If you save this config, then you will not be able to share it anymore, \nsince you are trying to save data not from your config"));
						ImGui::PopStyleVar();
						ImGui::PopStyleColor();
					}
				}
			}
		}

		ImGui::SameLine();
		if (!config->cfgs.size() || config->selected_config > config->cfgs.size() || config->selected_config < 0)
			imgui_ex::button_disabled(crypt_str("Share"), ImVec2(240.0f, 30.0f));
		else if (!config->cfgs.at(config->selected_config).can_share)
		{
			imgui_ex::button_disabled(crypt_str("Share"), ImVec2(240.0f, 30.0f));
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
				ImGui::PushStyleColor(ImGuiCol_PopupBg, core::s::is_theme_dark ?
					ImGui::GetColorU32(ImGuiCol_WindowBg) : IM_COL32_WHITE);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
				ImGui::SetTooltip(config->cfgs.at(config->selected_config).cant_share_reason);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}

		}
		else 
		{
			int ret = imgui_ex::dropdown_button(crypt_str("Share"), ImVec2(240.0f, 30.0f), { crypt_str("Create one-time link"),crypt_str("Open site") });

			if (ret == 0)
				std::thread(&Config::one_time_link, config.get(), config->selected_config).detach();
				
			if (ret == 1)
				std::thread(&Config::open_share_url, config.get(), config->selected_config).detach();
			
			
		}
		if (imgui_ex::button(crypt_str("Create"), ImVec2(240.0f, 30.0f)))
		{
			std::thread([]() {
				if (!config->processing)
				{
					config->create_cfg(name);
					memset(name, 0, sizeof(name));

					for (auto& cfg : config->cfgs)
					{
						if (cfg.id == config->m_loaded.id) {
							config->selected_config = loaded_config;
							break;
						}
					}
				}}).detach();
		}

		ImGui::SameLine();

		if (imgui_ex::button(crypt_str("Open Recycle bin"), ImVec2(240.0f, 30.0f)))
			std::thread(&Config::open_recucle_bin, config.get()).detach();	
		
		if (!config->processing && config->selected_config < config->cfgs.size() && config->selected_config > -1)
		{
			if (imgui_ex::button(crypt_str("Delete"), ImVec2(240.0f, 30.0f)))
			{
				if (is_locked)
					is_popUp_del = true;
				else {
					process_config = config->selected_config;
					std::thread([&]() {
						config->delete_cfg(config->selected_config);
						}).detach();

				}
			}
		}
		else
			imgui_ex::button_disabled(crypt_str("Delete"), ImVec2(240.0f, 30.0f));


		ImGui::SameLine();

		const char* dpi[] = { "75%", "100%", "125%",
			"150%", "175%", "200%", "225%", "250%", "275%", "300%" };

		ImGui::SetCursorPosY(u::dpi(190.0f));
		imgui_ex::slider_dpi(crypt_str("DPI scale"), &s::dpi_scale, 1, 10, dpi, 240.0f);
	}

	/* scripts tab */
	int selected_script = 0;
	void scripts_first_tab(
		std::string& script_name)
	{
		/*files = Lua_manager->scripts;
		std::vector<std::string> files_show = Lua_manager->scripts;
		if (!files.empty()) {
			for (auto& script : files)
			{
				auto script_id = Lua_manager->get_script_id(script);

				if (script_id == -1)
					continue;

				if (Lua_manager->loaded.at(script_id))
					files_show.at(script_id) += crypt_str(" [loaded]");
			}
		}
		static int current_lua = 0;
		ImGui::SetCursorPos(ImVec2(3.0f, 15.0f));
		ImGui::BeginGroup();
		{
			for (size_t i = 0u; i < files_show.size(); i++)
			{
				const bool is_selected = selected_script == i;
				if (imgui_ex::selectable(files_show[i].c_str(), is_selected))
				{
					selected_script = i;
					script_name = files_show[selected_script];
				}
			}
		}
		ImGui::Spacing();
		ImGui::EndGroup();*/
	}

	void scripts_second_tab()
	{
		//static char name[256]{};

		//imgui_ex::input_text(crypt_str("Name"), name, 256, ImVec2(491.0f, 30.0f));

		//if (imgui_ex::button(crypt_str("Load"), ImVec2(240.0f, 30.0f)) && selected_script != -1 && selected_script < Lua_manager->scripts.size())
		//{
		//	Lua_manager->load_script(selected_script);
		//	Lua_manager->refresh_scripts();
		//}

		//ImGui::SameLine();
		//if (imgui_ex::button(crypt_str("Edit script"), ImVec2(240.0f, 30.0f)) && selected_script != -1 && selected_script < Lua_manager->scripts.size())
		//{
		//	std::string file_path;

		//	auto get_dir = [&]() -> void
		//	{
		//		static TCHAR path[MAX_PATH];

		//		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
		//			file_path = std::string(path) + crypt_str("\\Legendware\\Scripts\\");

		//		CreateDirectory(file_path.c_str(), NULL);
		//	};

		//	get_dir();
		//	lua_editor.toggle();

		//	if (lua_editor.is_open() && selected_script != -1)
		//	{
		//		lua_editor.set_folder(file_path);
		//		lua_editor.set_script(Lua_manager->scripts.at(selected_script));
		//		lua_editor.set_id(selected_script);
		//		lua_editor.set_text();
		//	}
		//}

		//static bool is_locked = false;

		//if (imgui_ex::button(crypt_str("Unload"), ImVec2(240.0f, 30.0f)) && !Lua_manager->scripts.empty() && selected_script != -1 && selected_script < Lua_manager->scripts.size())
		//{
		//	Lua_manager->unload_script(selected_script);
		//	Lua_manager->refresh_scripts();
		//}
		//ImGui::SameLine();

		//if (imgui_ex::button(crypt_str("Create"), ImVec2(240.0f, 30.0f)))
		//{
		//	std::string folder;

		//	if (strlen(name) > 0)
		//	{
		//		auto get_dir = [&folder]() -> void
		//		{
		//			static TCHAR path[MAX_PATH];

		//			if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
		//				folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\") + std::string(name) + crypt_str(".lua");

		//			//CreateDirectory(folder.c_str(), NULL);
		//		};

		//		get_dir();
		//		std::ofstream new_file;
		//		new_file.open(folder, std::ios::_Noreplace);
		//		new_file.close();
		//		Lua_manager->refresh_scripts();
		//	}
		//	/*Lua_manager->unload_script(selected_script);
		//	Lua_manager->refresh_scripts();*/
		//}

		//if (imgui_ex::button(crypt_str("Refresh"), ImVec2(240.0f, 30.0f)))
		//	Lua_manager->refresh_scripts();

		//ImGui::SameLine();

		//if (imgui_ex::button(crypt_str("Delete"), ImVec2(240.0f, 30.0f)) && selected_script != -1 && selected_script < Lua_manager->scripts.size())
		//{
		//	Lua_manager->unload_script(selected_script);
		//	std::string folder;
		//	auto get_dir = [&folder]() -> void
		//	{
		//		static TCHAR path[MAX_PATH];

		//		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
		//			folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\") + Lua_manager->scripts.at(selected_script);

		//		//CreateDirectory(folder.c_str(), NULL);
		//	};

		//	if (selected_script != -1)
		//		get_dir();

		//	remove(folder.c_str());
		//	Lua_manager->refresh_scripts();
		//}

		//if (imgui_ex::button(crypt_str("Open scripts folder"), ImVec2(240.0f, 30.0f)))
		//{
		//	{
		//		std::string folder;

		//		auto get_dir = [&folder]() -> void
		//		{
		//			static TCHAR path[MAX_PATH];

		//			if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
		//				folder = std::string(path) + crypt_str("\\Legendware\\Scripts\\");

		//			CreateDirectory(folder.c_str(), NULL);
		//		};

		//		get_dir();
		//		ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
		//	}
		//}
		//ImGui::SameLine();


		//const char* dpi[] = { "75%", "100%", "125%",
		//	"150%", "175%", "200%", "225%", "250%", "275%", "300%" };

		//ImGui::SetCursorPosY(u::dpi(190.0f));
		//imgui_ex::slider_dpi(crypt_str("DPI scale"), &s::dpi_scale, 1, 10, dpi, 240.0f);
	}
}