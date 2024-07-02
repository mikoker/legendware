#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"

struct Log
{
	string message;
	float log_time;
	Color color;
	float y;

	Log(string message, float log_time, const Color& color)  //-V818
	{
		this->log_time = log_time;
		this->message = message; //-V820
		this->color = color;

		y = 0.0f;
	}
};

class Logs
{
	deque <Log> logs;
	virtual std::string join(const std::vector<std::string>& v, const std::string& delimiter = ", ");

public:
	virtual void draw();

	virtual std::string detailed_data(std::string& log, std::vector<std::string>& additional);

#ifdef RELEASE
#if BETA
	virtual void add(string log, Color color, string title = crypt_str("[ LEGENDWARE BETA ] "), bool force = false);
	virtual void add_force_title(string log, Color color, bool force)
	{
		this->add(log, color, crypt_str("[ LEGENDWARE BETA ] "), force);
}
#else
	virtual void add(string log, Color color, string title = crypt_str("[ LEGENDWARE ] "), bool force = false);
	virtual void add_force_title(string log, Color color, bool force)
	{
		this->add(log, color, crypt_str("[ LEGENDWARE ] "), force);
	}
#endif
#else
	void add(string log, Color color, string title = crypt_str("[ LEGENDWARE ALPHA ] "), bool force = false);
	void add_force_title(string log, Color color, bool force)
	{
		this->add(log, color, crypt_str("[ LEGENDWARE ALPHA ] "), force);
	}
#endif
};