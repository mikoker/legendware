// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "proxy.h"

void hooked_spotted(RecvProxyData& data, void* entity, void* output)
{

proxy->get_original(PROXY_SPOTTED)(data, entity, output);
}