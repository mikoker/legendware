#pragma once

#include "..\includes.h"
#include "..\data.h"
#include "..\globals.h"
#include "..\render.h"

class SpectatorList {
public:
    virtual void run();
    virtual std::vector<std::pair<std::string, LPDIRECT3DTEXTURE9>> get_spectators();
    virtual void clear();
private:
    std::unordered_map<uint64_t, LPDIRECT3DTEXTURE9 > m_avatars;
    std::unordered_map<uint64_t, std::string > m_names;
    std::unordered_map<uint64_t, bool > m_ready;
    std::unordered_map<uint64_t, bool > m_show;
};