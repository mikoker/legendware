//#pragma once
//#define SOL_ALL_SAFETIES_ON 1
//#include "sol.hpp"
//
//#include <map>
//
//struct c_lua_hook {
//	sol::protected_function func;
//};
//
//class c_lua_hookManager {
//public:
//	void registerHook(std::string eventName,  sol::protected_function func);
//	void unregisterHooks();
//
//	std::vector<c_lua_hook> getHooks(std::string eventName);
//
//private:
//	std::map<std::string, std::vector<c_lua_hook>> hooks;
//};