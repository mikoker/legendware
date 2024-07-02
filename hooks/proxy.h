#pragma once
#include "hooks.h"

enum
{
	PROXY_VELOCITY_MODIFIER,
	PROXY_VIEW_MODEL_SEQUENCE,
	PROXY_SPOTTED,
	PROXY_MAX
};

class Proxy 
{
	RecvProxy originals[PROXY_MAX];
	unordered_map <uint32_t, RecvProxy> proxies;

public:
	virtual void initialize();
	virtual void process_table(const char* name, crypt_ptr <RecvTable> table);
	virtual RecvProxy get_original(int index);
};

extern crypt_ptr <Proxy> proxy;