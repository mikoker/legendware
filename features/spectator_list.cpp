#include "spectator_list.h"

typedef struct
{
    void* steam_client;
    void* steam_user;
    void* steam_friends;
    void* steam_utils;
    void* steam_matchmaking;
    void* steam_user_stats;
    void* steam_apps;
    void* steam_matchmakingservers;
    void* steam_networking;
    void* steam_remotestorage;
    void* steam_screenshots;
    void* steam_http;
    void* steam_unidentifiedmessages;
    void* steam_controller;
    void* steam_ugc;
    void* steam_applist;
    void* steam_music;
    void* steam_musicremote;
    void* steam_htmlsurface;
    void* steam_inventory;
    void* steam_video;
}
S_steamApiCtx_t;

void SpectatorList::run()
{
    if (!ctx->local())
        return;

    crypt_ptr<Player> check_player;
    if (ctx->local()->valid()) {
        check_player = ctx->local();
    }
    else {
        check_player = ctx->local()->m_hObserverTarget().Get();
    }
    std::unordered_map<uint64_t, bool > show;
    for (int i = 1; i < globals->maxclients; i++)
    {
        crypt_ptr<Player> ent = static_cast<Player*>(entitylist->GetClientEntity(i));

        if (!ent)
            continue;

        if (ent->m_lifeState() == LIFE_ALIVE)
            continue;

        if (ent->IsDormant())
            continue;

        if (ent->m_hObserverTarget().Get() != check_player.get())
            continue;

        if (ent.get() == ctx->local().get())
            continue;

        player_info_t player_info;
        engine->GetPlayerInfo(i, &player_info);

        if (player_info.fakeplayer)
            continue;
        show[player_info.steamID64] = true;
        if (this->m_avatars.find(player_info.steamID64) == this->m_avatars.end() || this->m_names.find(player_info.steamID64) == this->m_names.end() || !this->m_ready[player_info.steamID64])
        {
            this->m_ready[player_info.steamID64] = false;
            typedef int(__thiscall* ISteamFriends_GetSmallFriendAvatar)(void*, uint64_t);

            auto steamid = 76561197960265728ULL + player_info.iSteamID;
            auto steamapi = *(S_steamApiCtx_t**)(signatures_manager->signatures[SIGNATURE_STEAM_API]);
            auto handle = call_virtual<ISteamFriends_GetSmallFriendAvatar>(steamapi->steam_friends, 34)(steamapi->steam_friends, steamid);


            if (handle) {
                uint32 width, height;
                typedef bool(__thiscall* ISteamUtils_GetImageSize)(void*, int, uint32_t*, uint32_t*);
                if (call_virtual< ISteamUtils_GetImageSize>(steamapi->steam_utils, 5)(steamapi->steam_utils, handle, &width, &height)) {
                    if (width > 0 && height > 0) {
                        auto rgba_buffer_size = width * height * 4;
                        uint8_t* rgba_buffer = new uint8_t[rgba_buffer_size];
                        typedef bool(__thiscall* ISteamUtils_GetImageRGBA)(void*, int, unsigned char*, int);

                        if (call_virtual<ISteamUtils_GetImageRGBA>(steamapi->steam_utils, 6)(steamapi->steam_utils, handle, rgba_buffer, rgba_buffer_size))
                        {
                            auto res = render->device->CreateTexture(width,
                                height,
                                1,
                                D3DUSAGE_DYNAMIC,
                                D3DFMT_A8R8G8B8,
                                D3DPOOL_DEFAULT,
                                &m_avatars[player_info.steamID64],
                                nullptr);

                            std::vector<uint8_t> texData;
                            texData.resize(width * height * 4u);

                            static auto CopyConvert = [](const uint8_t* rgba, uint8_t* out, const size_t size) {
                                auto in = reinterpret_cast<const uint32_t*>(rgba);
                                auto buf = reinterpret_cast<uint32_t*>(out);
                                for (auto i = 0u; i < (size / 4); ++i)
                                {
                                    const auto pixel = *in++;
                                    *buf++ = (pixel & 0xFF00FF00) | ((pixel & 0xFF0000) >> 16) | ((pixel & 0xFF) << 16);
                                }
                            };

                            CopyConvert(rgba_buffer,
                                texData.data(),
                                width * height * 4u);

                            D3DLOCKED_RECT rect;
                            res = this->m_avatars[player_info.steamID64]->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);

                            auto src = texData.data();
                            auto dst = reinterpret_cast<uint8_t*>(rect.pBits);
                            for (auto y = 0u; y < height; ++y)
                            {
                                std::copy(src, src + (width * 4), dst);

                                src += width * 4;
                                dst += rect.Pitch;
                            }

                            res = this->m_avatars[player_info.steamID64]->UnlockRect(0);
                            delete[] rgba_buffer;
                        }
                        else
                        {
                            delete[] rgba_buffer;
                            continue;
                        }

                    }
                }


            }

            this->m_names[player_info.steamID64] = player_info.szName;
            this->m_ready[player_info.steamID64] = true;
        }

    }
    this->m_show = show;
}

std::vector<std::pair<std::string, LPDIRECT3DTEXTURE9>> SpectatorList::get_spectators()
{
    std::vector<std::pair<std::string, LPDIRECT3DTEXTURE9>> out_data;
    for (const auto& [key, value] : this->m_ready) {
        if (!value)
            continue;
        if (this->m_show.find(key) != this->m_show.end()) {
            if (this->m_show[key])
                out_data.emplace_back(std::make_pair(this->m_names[key], this->m_avatars[key]));
        }

    }
    return out_data;
}

void SpectatorList::clear()
{
    this->m_avatars.clear();
    this->m_names.clear();
    this->m_ready.clear();
    this->m_show.clear();
}