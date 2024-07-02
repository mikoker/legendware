// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define WIN32_LEAN_AND_MEAN


#include "includes.h"
#include "data.h"
#include "globals.h"
#include "render.h"
#include "features\features.h"
#include "features\aim.h"
#include "features\animations.h"
#include "features\anti_aim.h"
#include "features\build_bones.h"
#include "features\chams.h"
#include "features\dormant.h"
#include "features\draw_beams.h"
#include "features\esp.h"
#include "features\exploits.h"
#include "features\fake_lag.h"

#include "features\knife_aim.h"
#include "features\legit_aim.h"
#include "features\features.h"
#include "features\local_animations.h"
#include "features\misc.h"
#include "features\movement.h"

#include "features\penetration.h"
#include "features\prediction.h"

#include "features\sound_esp.h"
#include "features\spectator_list.h"
#include "features\third_person.h"
#include "features\visuals.h"
#include "features\world_color.h"
#include "features\world_esp.h"

#include "features\logs.h"

#include "hooks\hooks.h"
#include "hooks\proxy.h"
#include "menu\menu.h"
#include "features\chams.h"

#include "json.hpp"
//#include "tls_client\httplib.h"
#include "Lua\Clua.h"

#include "new_threads.h"

HMODULE base_address = nullptr;
size_t module_size = 0;

LONG CALLBACK exception_handler(EXCEPTION_POINTERS* exception_pointers)
{
	if (exception_pointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
		return EXCEPTION_CONTINUE_EXECUTION;

	if ((uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress < (uintptr_t)base_address || (uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress > (uintptr_t)base_address + module_size)
		return EXCEPTION_CONTINUE_SEARCH;

	std::stringstream text;

	text << crypt_str("An internal exception occurred!") << std::endl;
	text << crypt_str("Address: 0x") << std::hex << (uintptr_t)exception_pointers->ExceptionRecord->ExceptionAddress - (uintptr_t)base_address << std::endl;
	text << crypt_str("Code: 0x") << std::hex << exception_pointers->ExceptionRecord->ExceptionCode;

	MessageBox(nullptr, text.str().c_str(), crypt_str("Legendware"), MB_OK | MB_ICONERROR);
	TerminateProcess((HANDLE)-1, EXIT_SUCCESS);

	return EXCEPTION_CONTINUE_SEARCH;
}

DWORD WINAPI main_function(LPVOID module_address)
{
	for (auto i = 0; i < 30; ++i)
	{
		if (GetModuleHandle(crypt_str("serverbrowser.dll")))
			break;

		Sleep(1000);
	}

	ctx = new Ctx;

#ifdef RELEASE // idk use debug
	/*
	for (auto i = 0; i < 32; ++i)
		ctx->token[i] = ((char*)module_address)[i];

	ctx->token[32] = '\0';

	auto client = httplib::SSLClient(crypt_str("legendware.pw"));
	auto result = client.Get((crypt_str("/auth.php?cheat=1&loader_session_id=") + (std::string)ctx->token).c_str());

	if (!result)
		return EXIT_FAILURE;

	if (result->status != 200)
		return EXIT_FAILURE;

	auto json_data = nlohmann::json::parse(result->body);

	if (!json_data[crypt_str("success")].get <bool> ())
		return EXIT_FAILURE;

	ctx->avatar = json_data[crypt_str("avatar_path")].get <std::string> ();
	ctx->username = json_data[crypt_str("username")].get <std::string> ();
	ctx->days = json_data[crypt_str("remaining_days")].get <std::string> ();
	ctx->user_id = json_data[crypt_str("user_id")].get <int> ();
	*/
#else
	if (!AllocConsole())
		return EXIT_FAILURE;

	FILE* file;

	if (freopen_s(&file, crypt_str("CONOUT$"), crypt_str("w"), stdout))
		return EXIT_FAILURE;
#endif

	interfaces_manager = new Interfaces;
	netvars_manager = new Netvars;
	convars_manager = new Convars;
	hooks_manager = new Hooks;
	proxy = new Proxy;
	config = new Config;
	signatures_manager = new Signatures;
	aim = new Aim;
	animations = new Animations;
	anti_aim = new AntiAim;
	build_bones = new BuildBones;
	chams = new Chams;
	dormant = new Dormant_esp;
	bullet_tracer = new BulletTracer;
	esp = new Esp;
	exploits = new Exploits;
	fake_lag = new FakeLag;

	knife_aim = new KnifeAim;
	legit_aim = new LegitAim;
	local_animations = new LocalAnimations;
	logs = new Logs;
	misc = new Misc;
	movement_system = new Movement;

	penetration = new Penetration;
	engine_prediction = new Prediction;



	sound_esp = new SoundEsp;
	spectator_list = new SpectatorList;
	third_person = new ThirdPerson;
	visuals = new Visuals;
	world_color = new WorldColor;
	world_esp = new WorldEsp;



	resources = new resource_manager;
	thread_pool = new ThreadPool;
	//Lua_manager = new c_lua;


	signatures_initialize();
	interfaces_initialize();
	netvars_initialize();
	convars_initialize();




	hooks_initialize();

	//OpenSSL_add_all_algorithms(); //useless
	//SSL_library_init();

	proxy->initialize();

	//Lua_manager->initialize();

	config->initialize();

	auto tier0 = GetModuleHandle(crypt_str("tier0.dll"));

	if (tier0)
	{
		thread_pool->init();
	}

#if !RELEASE && !MININ//lezyjedenskibidi
	if (file)
		fclose(file);

	FreeConsole();
#endif

    return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE module_address, DWORD reason_for_call, LPVOID reserved)
{
	base_address = module_address;

	if (reason_for_call == DLL_PROCESS_ATTACH)
	{
		AddVectoredExceptionHandler(TRUE, exception_handler);
		DisableThreadLibraryCalls(module_address);

		if (!CreateThread(nullptr, 0, main_function, module_address, 0, nullptr)) //-V718 //-V513
			return FALSE;
	}
	else if (reason_for_call == DLL_PROCESS_DETACH)
		base_address = nullptr;

    return TRUE;
}