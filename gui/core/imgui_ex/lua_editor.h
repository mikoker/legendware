//#pragma once
//
//#include <shlobj.h>
//#include <fstream>
//#include "../../third-party/imgui_text_editor/TextEditor.h"
//#include "../core.h"
//
//class c_lua_editor
//{
//public:
//	void init();
//	void draw();
//
//public:
//	void save();
//
//public:
//	void set_folder(
//		const std::string& path)
//	{
//		file_path_ = path;
//	}
//	void set_id(int id)
//	{
//		id_ = id;
//	}
//
//	void set_script(
//		const std::string& name)
//	{
//		script_name_ = name;
//	}
//
//	void set_text()
//	{
//		str_old_ = "";
//		in_.open(file_path_ + script_name_);
//
//		if (in_.good())
//		{
//			std::string str((std::istreambuf_iterator<char>(in_)),
//				std::istreambuf_iterator<char>());
//
//			editor_ctx_.SetText(str);
//			str_old_ = editor_ctx_.GetText();
//			in_.close();
//		}
//		else
//			editor_ctx_.SetText("");
//	}
//
//public:
//	void toggle() { is_opened_ = !is_opened_; }
//	bool is_open() const { return is_opened_; }
//
//private:
//	TextEditor editor_ctx_{};
//
//private:
//	std::ifstream in_{};
//	std::ofstream out_{};
//
//private:
//	std::string file_path_ = "";
//	std::string script_name_ = "";
//	int id_ = -1;
//	std::string str_old_ = "";
//
//private:
//	bool is_opened_ = false;
//	bool is_opened_2 = true;
//
//	bool is_script_loaded_ = false;
//	bool is_file_saved_ = false;
//};
//
//extern c_lua_editor lua_editor;
