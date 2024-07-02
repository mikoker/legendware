#pragma once

#ifndef UTILS_H
#define UTILS_H

namespace u
{
    std::string random_string(size_t length = 8);

    void fade(bool toggle, float& result, float min,
        float max, float speed = 5.5f);

    void fade_noclamp(bool toggle,
        float& result, float speed = 5.5f);

    ImColor fade_color(ImU32 old_clr, ImU32 new_clr,
        ImVec4& clr_out, float speed = 4.5f);

    float dpi_ex(float value, int index);
    float dpi(float value, float offset = 0.0f);

    ImVec2 dpi(ImVec2 value, ImVec2 offset = ImVec2(0.0f, 0.0f));
}

#endif // !UTILS_H