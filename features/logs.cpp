// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "logs.h"
#include "..\ImGui Render\ImGUI_Renderer.h"

void Logs::draw()
{
	if (logs.empty())
		return;

	while (logs.size() > 10)
		logs.pop_back();

	auto last_y = 146.0f;

	for (size_t i = 0; i < logs.size(); i++)
	{
		auto& log = logs.at(i); //-V831
		auto logs_size_inverted = 10 - logs.size();

		if (globals->realtime - log.log_time > 4.6f)
		{
			auto factor = log.log_time + 5.0f - globals->realtime;
			auto opacity = (int)(255.0f * factor);

			if (opacity < 2)
			{
				logs.erase(logs.begin() + i);
				continue;
			}

			log.color.SetAlpha(opacity);
			log.y -= factor * 1.25f;
		}

		last_y -= 14.0f;
		renderer_imgui.RenderText(log.message, u::dpi(ImVec2(6.0f, last_y + log.y - (float)logs_size_inverted * 14.0f)), u::dpi(11.0f), log.color, false, core::f::logs_font, false);
	}
}

void Logs::add(string log, Color color, string title, bool force) //-V813
{
	if (config->misc.logs_position[LOGS_POSITION_SCREEN] || force)
		logs.emplace_front(Log(log, globals->realtime, color));

	if (config->misc.logs_position[LOGS_POSITION_CONSOLE] || force)
	{
		if (!title.empty())
			cvar->ConsoleColorPrintf(Color::White, title.c_str());

		cvar->ConsoleColorPrintf(color, log.c_str());
		cvar->ConsolePrintf(crypt_str("\n"));
	}
}


std::string Logs::join(const std::vector<std::string>& v, const std::string& delimiter)
{
	return std::accumulate(v.begin() + 1, v.end(), v[0],
		[&delimiter](std::string x, std::string y) {
			return x + delimiter + y;
		}
	);
}

std::string Logs::detailed_data(std::string& log, std::vector<std::string>& additional)
{
	log += " (";
	log += join(additional);
	log += ')';
	return log;
}