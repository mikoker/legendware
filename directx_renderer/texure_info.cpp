#include "texture_info.hpp"
#include <vector>
#include "../render.h"
std::vector<texture_info*>* textures_list()
{
    static auto textures_list = std::vector<texture_info*>();

    return &textures_list;
}


IDirect3DDevice9* texture_info::device = nullptr;

void texture_info::for_each(void(*pred)(texture_info*))
{
    for (auto texture_info : *textures_list())
        pred(texture_info);
}

void texture_info::on_lost_device()
{
    for_each(
        [](texture_info* t)
        {
            t->release();
        }
    );
}

void texture_info::on_reset_device()
{
    for_each(
        [](texture_info* t)
        {
            t->initialize();
        }
    );
}

texture_info::texture_info(unsigned char* data, size_t size, ImVec2 resolution) :
    data(data),
    size(size),
    resolution(resolution),
    texture(nullptr)
{
    textures_list()->push_back(this);
}

texture_info::~texture_info()
{
    auto list = textures_list();

    for (auto it = list->begin();
        it != list->end(); it++)
    {
        if (*it == this)
        {
            list->erase(it);
            break;
        }
    }
}

void texture_info::initialize()
{
    if (data)
    {
        auto result = D3DXCreateTextureFromFileInMemoryEx(
            device,
            data,
            size,
            static_cast<unsigned>(resolution.x),
            static_cast<unsigned>(resolution.y),
            D3DX_DEFAULT,
            D3DUSAGE_DYNAMIC,
            D3DFMT_UNKNOWN,
            D3DPOOL_DEFAULT,
            D3DX_DEFAULT,
            D3DX_DEFAULT,
            0,
            NULL,
            NULL,
            &texture
        );

        assert(result == D3D_OK);
    }
}

void texture_info::release()
{
    if (texture)
    {
        texture->Release();
        texture = nullptr;
    }
}

void texture_info::draw(ImVec2 pos, ImU32 col) const
{
    ImGui::GetWindowDrawList()->AddImage(
        texture,
        pos,
        ImVec2(
            pos.x + resolution.x,
            pos.y + resolution.y
        ),
        ImVec2(0, 0),
        ImVec2(1, 1),
        col
    );
}
