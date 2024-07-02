//// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//
//#include "lua_editor.h"
//#include "../../../Lua/Clua.h"
//
//namespace imgui_ex
//{
//	bool button_lua(const char* label,
//		const ImVec2& size_arg, bool is_red);
//}
//
//c_lua_editor lua_editor;
//
//void c_lua_editor::init()
//{
//	std::string file_path = "";
//
//	static char path[MAX_PATH];
//
//	if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path)))
//		file_path = std::string(path) + crypt_str(R"(\Legendware\Scripts\)");
//
//	CreateDirectoryA(file_path.c_str(), nullptr);
//
//	editor_ctx_.SetLanguageDefinition(
//		TextEditor::LanguageDefinition::Lua());
//
//	editor_ctx_.SetReadOnly(false);
//}
//
//void c_lua_editor::draw()
//{
//	static bool op = false;
//
//	if (!is_opened_2)
//		op = true;
//
//	static std::string last_script = script_name_;
//
//	if (last_script != script_name_)
//	{
//		//set_text();
//
//		last_script = script_name_;
//	}
//
//	if (is_file_saved_)
//	{
//		save();
//
//		is_file_saved_ = false;
//	}
//
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
//	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
//	std::string title;
//	if (Lua_manager->loaded.at(id_))
//		title = "Lua Editor (" + script_name_ + " [loaded])";
//	else
//		title = "Lua Editor (" + script_name_ + ")";
//
//
//	const ImVec2& size = ImVec2(800.0f, 600.0f);
//
//	ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
//
//	static auto start_title = title;
//	ImGuiWindow* window = ImGui::FindWindowByName(start_title.c_str());
//	if (window && strcmp(title.c_str(), window->Name) != 0)
//	{
//		size_t buf_len = (size_t)window->NameBufLen;
//		window->Name = ImStrdupcpy(window->Name, &buf_len, title.c_str());
//		window->NameBufLen = (int)buf_len;
//	}
//	ImGui::Begin(start_title.c_str(), &is_opened_2, core::s::editor_flags);
//	{
//		if (ImGui::BeginMenuBar())
//		{
//			if (ImGui::BeginMenu("File"))
//			{
//				if (ImGui::MenuItem("Load"))
//				{
//					if (str_old_ != editor_ctx_.GetText())
//						save();
//					if (Lua_manager->loaded.at(id_))
//						Lua_manager->unload_script(id_);
//					Lua_manager->load_script(id_);
//				}
//				if (ImGui::MenuItem("Unload"))
//				{
//					if (Lua_manager->loaded.at(id_))
//						Lua_manager->unload_script(id_);
//				}
//				if (ImGui::MenuItem("Save"))
//					is_file_saved_ = true;
//
//				if (ImGui::MenuItem("Close"))
//				{
//					if (!is_file_saved_)
//					{
//						op = true;
//						is_opened_2 = true;
//					}
//					else
//					{
//						toggle();
//
//						is_file_saved_ = false;
//						is_opened_2 = true;
//					}
//				}
//
//				ImGui::EndMenu();
//			}
//
//			if (ImGui::BeginMenu("Edit"))
//			{
//				bool ro = editor_ctx_.IsReadOnly();
//
//				if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
//					editor_ctx_.SetReadOnly(ro);
//
//				ImGui::Separator();
//
//				if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor_ctx_.CanUndo()))
//					editor_ctx_.Undo();
//
//				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor_ctx_.CanRedo()))
//					editor_ctx_.Redo();
//
//				ImGui::Separator();
//
//				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor_ctx_.HasSelection()))
//					editor_ctx_.Copy();
//
//				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor_ctx_.HasSelection()))
//					editor_ctx_.Cut();
//
//				if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor_ctx_.HasSelection()))
//					editor_ctx_.Delete();
//
//				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
//					editor_ctx_.Paste();
//
//				ImGui::Separator();
//
//				if (ImGui::MenuItem("Select all", nullptr, nullptr))
//					editor_ctx_.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor_ctx_.GetTotalLines(), 0));
//
//				ImGui::EndMenu();
//			}
//
//			editor_ctx_.SetPalette(core::s::is_theme_dark ?
//				TextEditor::GetDarkPalette() : TextEditor::GetLightPalette());
//
//			ImGui::EndMenuBar();
//		}
//
//		if (!is_file_saved_)
//		{
//			if (op)
//			{
//				if (str_old_ != editor_ctx_.GetText() && str_old_ != "")
//					ImGui::OpenPopup("save_file");
//				else
//				{
//					toggle();
//					is_file_saved_ = true;
//					is_opened_2 = true;
//				}
//				op = false;
//			}
//
//			float size_x = ImGui::GetWindowSize().x;
//			ImGui::SetNextWindowSize(ImVec2(305.0f, 155.0f));
//			ImGui::SetNextWindowPos(ImGui::GetWindowPos() + ImVec2((
//				size_x - 305.0f) / 2.0f, 100.0f));
//			ImGui::PushStyleColor(ImGuiCol_WindowShadow,
//				ImGui::GetColorU32(ImGuiCol_WindowShadow, 0.2f));
//
//			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImGui::GetColorU32(
//				core::s::is_theme_dark ? ImGuiCol_WindowBg : ImGuiCol_FrameBg));
//
//			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 5));
//
//			if (ImGui::BeginPopup("save_file", core::s::main_focus_flags | ImGuiWindowFlags_NoMove))
//			{
//				ImGui::GetWindowDrawList()->AddText(core::f::dinpro_bold, 18.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 25.0f),
//					ImGui::GetColorU32(ImGuiCol_Text), "Save file?");
//
//				ImGui::GetWindowDrawList()->AddText(core::f::dinpro, 17.0f, ImGui::GetWindowPos() + ImVec2(25.0f, 55.0f),
//					ImGui::GetColorU32(ImGuiCol_Text), "Save file before exiting?");
//
//				ImGui::SetCursorPos(ImVec2(25.0f, 100.0f));
//				ImGui::BeginGroup();
//				{
//					if (imgui_ex::button_lua("No", ImVec2(120.0f, 22.0f), true))
//					{
//						toggle();
//
//						is_file_saved_ = false;
//						is_opened_2 = true;
//					}
//
//					ImGui::SameLine();
//
//					if (imgui_ex::button_lua("Yes", ImVec2(120.0f, 22.0f), false))
//					{
//						save();
//						toggle();
//
//						is_file_saved_ = true;
//						is_opened_2 = true;
//					}
//				}
//
//				ImGui::EndGroup();
//				ImGui::EndPopup();
//			}
//
//			ImGui::PopStyleColor(3);
//		}
//
//		editor_ctx_.Render("##lua_editor");
//	}
//
//	ImGui::End();
//	ImGui::PopStyleVar(2);
//}
//
//void c_lua_editor::save()
//{
//	std::string text_to_save = editor_ctx_.GetText();
//
//	text_to_save = text_to_save.substr(0,
//		editor_ctx_.GetText().size() - 1);
//
//	out_.open(file_path_ + script_name_);
//	out_ << text_to_save;
//	out_.close();
//	editor_ctx_.SetText(text_to_save);
//	str_old_ = editor_ctx_.GetText();
//}