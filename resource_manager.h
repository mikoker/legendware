#pragma once
#include "includes.h"
#include "json.hpp"

struct data_info
{
    int size;
    uint8_t* buffer;
};

class resource_manager
{
public:
    void start_download();
    bool get_done() { return m_is_done; }
    data_info get_data(std::string id) { return m_data[id]; }
    void clear() {};
private:
    bool m_is_done = false;
    std::unordered_map<std::string, data_info> m_data;
    void download(std::string link);

    std::vector <std::string> resources =
    {
        crypt_str("/resources/fonts/g_awesome_font_data"),
        crypt_str("/resources/fonts/g_csgo_icons_compressed_data"),
        crypt_str("/resources/fonts/g_dinpro_bold_data"),
        crypt_str("/resources/fonts/g_dinpro_data"),
        crypt_str("/resources/fonts/g_icons_data"),
        crypt_str("/resources/fonts/g_montserrat_data"),
        crypt_str("/resources/fonts/g_weapon_icons_data"),

        crypt_str("/resources/images/g_hitbox_arms"),
        crypt_str("/resources/images/g_hitbox_arms_ph"),
        crypt_str("/resources/images/g_hitbox_body"),
        crypt_str("/resources/images/g_hitbox_body_ph"),
        crypt_str("/resources/images/g_hitbox_feet"),
        crypt_str("/resources/images/g_hitbox_feet_ph"),
        crypt_str("/resources/images/g_hitbox_head"),
        crypt_str("/resources/images/g_hitbox_head_ph"),
        crypt_str("/resources/images/g_hitbox_legs"),
        crypt_str("/resources/images/g_hitbox_legs_ph"),
        crypt_str("/resources/images/g_hitbox_pelvis"),
        crypt_str("/resources/images/g_hitbox_pelvis_ph"),
        crypt_str("/resources/images/g_logo2"),
        crypt_str("/resources/images/g_small_logo"),
        crypt_str("/resources/images/g_large_logo"),
        crypt_str("/resources/images/g_playerlist_ph")
    };

    nlohmann::json hashes;
};