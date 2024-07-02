// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "proxy.h"
#include "..\data.h"
#include "..\features\logs.h"
#include "..\features\features.h"

crypt_ptr <Proxy> proxy;

void Proxy::initialize()
{
	for (auto client_class = client->GetAllClasses(); client_class; client_class = client_class->m_pNext)
		proxy->process_table(client_class->m_pNetworkName, client_class->m_pRecvTable);
}

RecvProxy Proxy::get_original(int index)
{
	return originals[index];
}

void Proxy::process_table(const char* name, crypt_ptr <RecvTable> table)
{
	for (auto i = 0; i < table->propCount; i++)
	{
		auto& prop = table->props[i];

		if (isdigit(prop.name[0]))
			continue;

		if (crypt_hash_r(prop.name) == crypt_hash("baseclass"))
			continue;

		if (prop.type == DPT_DataTable && prop.dataTable && prop.dataTable->netTableName[0] == 'D')
			process_table(name, prop.dataTable);

		auto current_hash = crypt_hash_r((name + (string)crypt_str("->") + prop.name).c_str());

		switch (current_hash)
		{
		case crypt_hash("CCSPlayer->m_flVelocityModifier"):
			originals[PROXY_VELOCITY_MODIFIER] = prop.proxy;
			proxies[current_hash] = prop.proxy;
			prop.proxy = hooked_velocitymodifier;
			break;
		case crypt_hash("CBaseViewModel->m_nSequence"):
			originals[PROXY_VIEW_MODEL_SEQUENCE] = prop.proxy;
			proxies[current_hash] = prop.proxy;
			prop.proxy = hooked_sequenceproxy;
			break;
		case crypt_hash("CBaseEntity->m_bSpotted"):
			originals[PROXY_SPOTTED] = prop.proxy;
			proxies[current_hash] = prop.proxy;
			prop.proxy = hooked_spotted;
			break;
		}
	}
}