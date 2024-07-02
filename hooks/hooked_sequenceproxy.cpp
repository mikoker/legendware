// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "proxy.h"
#include "..\features\features.h"




void hooked_sequenceproxy(RecvProxyData& data, void* arg2, void* arg3)
{
	proxy->get_original(PROXY_VIEW_MODEL_SEQUENCE)(data, arg2, arg3);
}