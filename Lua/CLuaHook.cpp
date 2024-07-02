//// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
//
//#include "CLuaHook.h"
//#include "Clua.h"
//void c_lua_hookManager::registerHook(std::string eventName, sol::protected_function func) { //-V813
//	c_lua_hook hk = {func };
//
//	this->hooks[eventName].emplace_back(hk);
//}
//
//void c_lua_hookManager::unregisterHooks() {
//	this->hooks.clear();
//}
//
//std::vector<c_lua_hook> c_lua_hookManager::getHooks(std::string eventName) { 
//	return this->hooks[eventName];
//}
