#include "CBlurProcessor.h"
#include "../../third-party/imgui/impl/imgui_impl_dx9.h"
#include "../../../protect/crypt_str.h"
#include "../../../render.h"
#include <array>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
template <unsigned int N>
constexpr auto DecodeBase85(const char(&input)[N]) noexcept
{
    std::array<char, N * 4 / 5> out{};

    constexpr auto decode85Byte = [](char c) constexpr -> unsigned int { return c >= '\\' ? c - 36 : c - 35; };

    for (unsigned int i = 0, j = 0; i < N - 1; i += 5, j += 4) {
        unsigned int tmp = decode85Byte(input[i]) + 85 * (decode85Byte(input[i + 1]) + 85 * (decode85Byte(input[i + 2]) + 85 * (decode85Byte(input[i + 3]) + 85 * decode85Byte(input[i + 4]))));
        out[j] = ((tmp >> 0) & 0xFF); out[j + 1] = ((tmp >> 8) & 0xFF); out[j + 2] = ((tmp >> 16) & 0xFF); out[j + 3] = ((tmp >> 24) & 0xFF);
    }

    return out;
}

// File: 'blur_x' (600 bytes)
// Exported using binary_to_compressed_c.cpp
constexpr auto blur_x = DecodeBase85(
    "%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####OS'##J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
    "`-.:CVjSuY&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHVd;#K7#####X/'.7`7r'7$QUV$*%XHV"
    "d:i[7bmhf6##########D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#W.]/Lpu$GV+>uu#PYr.LOV%JLou$GV&5YY#Q`%/Lpv*PV(>uu#Sf./L5hJcLdfIl["
    "(>uu#Rf./L4_/GLdfIl[&5YY#Y.]/Lqu$GV+>uu#RYr.LQV%JLou$GV&5YY#S`%/Lpv*PV(>uu#Uf./L7hJcLdfIl[(>uu#Tf./L6_/GLdfIl[i>uu#_XF0L4_/GL[BA`Vi>uu#`XF0L"
    "5_/GL[BA`Vi>uu#aXF0L6_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#W(S/L5_/GLpw0YV+G:;$W(S/L3_/GLpx6cV5_/GL+G:;$V(S/L4_/GLpw0YV5_/GL+G:;$V(S/L7_/GLqv*PV"
    "4_/GL+G:;$U(S/L6_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##");

constexpr auto blur_y = DecodeBase85(
    "%/P:vP>$(#>T$<8?####Q$###%/P:v%####?####OS'##J$###h####&####$####t#########.$###%####$####:$########tn=j=$8HlEQ2TuY3l:$#%),##$#########0g'WC"
    "Qk;nDhpF/#&5>##%),##$#########C,.33UnSc;'AViF6JrEH<Sk:0+bVe=K&&PDlf1eGdfX1F$*fUCs'el1K>,C5AH3I3b48(#$QUV$)%XHV9miVaX/'.7`7r'7d;#K7$QUV$*%XHV"
    "9lXhabmhf6d:i[7#####D5YY#NSi.L,nHS[D5YY#_9r:Q0=XHVi>uu#^XF0LdfIl[[BA`V&5YY#PYr.L)[-S[&5YY#Y4f/LT*A`V+G:;$Q`%/Lou$GVPV%JL)]3][&5YY#RYr.L)[-S["
    "+G:;$S`%/Lou$GVPSi.L)]3][&5YY#QYr.L)[-S[&5YY#Wr@/LU*A`V+G:;$R`%/Lou$GVRSi.L)]3][&5YY#SYr.L)[-S[+G:;$T`%/Lou$GVRUu@L)]3][i>uu#_XF0L4_/GL[BA`V"
    "i>uu#aXF0L6_/GL[BA`Vi>uu#`XF0L5_/GL[BA`Vi>uu#bXF0L7_/GL[BA`V+>uu#X(S/L6_/GLpv*PV+G:;$X(S/L3_/GLpw0YV6_/GL+G:;$V(S/L4_/GLpv*PV6_/GL+G:;$V(S/L"
    "5_/GLqv*PV4_/GL+G:;$U(S/L7_/GLqv*PV4_/GL&5YY#fqF0L3_/GL#),##");


void CBlurProcessor::BeginBlur(IDirect3DDevice9* pDevice)
{
    // initialize horizontal shader
    if (pBlurShaderX == nullptr)
    {
        pDevice->CreatePixelShader(reinterpret_cast<const DWORD*>(blur_x.data()), &pBlurShaderX);
    }

    // initialize vertical shader
    if (pBlurShaderY == nullptr)
    {
        pDevice->CreatePixelShader(reinterpret_cast<const DWORD*>(blur_y.data()), &pBlurShaderY);
    }

    // get backbuffer
    IDirect3DSurface9* pBackBuffer = nullptr;
    pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

    D3DSURFACE_DESC backDesc = { };
    pBackBuffer->GetDesc(&backDesc);

    // reset textures
    if (uBackBufferWidth != backDesc.Width || uBackBufferHeight != backDesc.Height)
    {
        if (pBlurFirst != nullptr)
        {
            pBlurFirst->Release();
            pBlurFirst = nullptr;
        }

        if (pBlurSecond != nullptr)
        {
            pBlurSecond->Release();
            pBlurSecond = nullptr;
        }

        uBackBufferWidth = backDesc.Width;
        uBackBufferHeight = backDesc.Height;
    }

    // initialize textures
    if (pBlurFirst == nullptr)
        pDevice->CreateTexture(uBackBufferWidth / nBlurDownsample, uBackBufferHeight / nBlurDownsample, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pBlurFirst, nullptr);

    if (pBlurSecond == nullptr)
        pDevice->CreateTexture(uBackBufferWidth / nBlurDownsample, uBackBufferHeight / nBlurDownsample, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &pBlurSecond, nullptr);

    pDevice->GetRenderTarget(0UL, &pSurfaceBackup);

    IDirect3DSurface9* pSurface = nullptr;
    pBlurFirst->GetSurfaceLevel(0U, &pSurface);

    if (ComPtr<IDirect3DSurface9> backBuffer; device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, backBuffer.GetAddressOf()) == D3D_OK) {
        if (ComPtr<IDirect3DSurface9> surface; pBlurFirst->GetSurfaceLevel(0, surface.GetAddressOf()) == D3D_OK)
            device->StretchRect(backBuffer.Get(), nullptr, surface.Get(), nullptr, D3DTEXF_LINEAR);
    }

    //pDevice->StretchRect(pBackBuffer, nullptr, pSurface, nullptr, D3DTEXF_LINEAR);

    pSurface->Release();
    pBackBuffer->Release();

    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    //pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
}

void CBlurProcessor::EndBlur(IDirect3DDevice9* pDevice) const
{
    pDevice->SetRenderTarget(0, pSurfaceBackup);
    pSurfaceBackup->Release();

    pDevice->SetPixelShader(pPixelShader);
    //pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
}

void CBlurProcessor::FirstBlurPass(IDirect3DDevice9* pDevice) const
{
    IDirect3DSurface9* pSurface = nullptr;
    pBlurSecond->GetSurfaceLevel(0, &pSurface);
    pDevice->SetRenderTarget(0, pSurface);
    pSurface->Release();

    pDevice->SetPixelShader(pBlurShaderX);
    const float arrParameters[4] = { 1.0f / static_cast<float>(uBackBufferWidth / nBlurDownsample) };
    pDevice->SetPixelShaderConstantF(0U, arrParameters, 1U);
}

void CBlurProcessor::SecondBlurPass(IDirect3DDevice9* pDevice) const
{
    IDirect3DSurface9* pSurface = nullptr;
    pBlurFirst->GetSurfaceLevel(0, &pSurface);
    pDevice->SetRenderTarget(0, pSurface);
    pSurface->Release();

    pDevice->SetPixelShader(pBlurShaderY);
    const float arrParameters[4] = { 1.0f / static_cast<float>(uBackBufferHeight / nBlurDownsample) };
    pDevice->SetPixelShaderConstantF(0U, arrParameters, 1U);
}

void DrawBackgroundBlur(ImDrawList* pDrawList, IDirect3DDevice9* pDevice)
{
    pDrawList->AddCallback(&CBlurProcessor::Start, pDevice);

    const unsigned int uBackBufferWidth = CBlurProcessor::Get().uBackBufferWidth;
    const unsigned int uBackBufferHeight = CBlurProcessor::Get().uBackBufferHeight;

    // process blur callback for needed strength
    for (int i = 0; i < 1; i++)
    {
        pDrawList->AddCallback(&CBlurProcessor::First, pDevice);
        pDrawList->AddImage(CBlurProcessor::Get().pBlurFirst, ImVec2(0.0f, 0.0f), ImVec2(static_cast<float>(uBackBufferWidth), static_cast<float>(uBackBufferHeight)));
        pDrawList->AddCallback(&CBlurProcessor::Second, pDevice);
        pDrawList->AddImage(CBlurProcessor::Get().pBlurSecond, ImVec2(0.0f, 0.0f), ImVec2(static_cast<float>(uBackBufferWidth), static_cast<float>(uBackBufferHeight)));
    }

    pDrawList->AddCallback(&CBlurProcessor::End, pDevice);

    pDrawList->AddImage(CBlurProcessor::Get().pBlurFirst, ImVec2(0.0f, 0.0f), ImVec2(static_cast<float>(CBlurProcessor::Get().uBackBufferWidth), static_cast<float>(CBlurProcessor::Get().uBackBufferHeight)), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImGui::GetColorU32(IM_COL32_WHITE));

}

void DrawBackgroundBlurRounded(ImDrawList* pDrawList, IDirect3DDevice9* pDevice, ImVec2 min, ImVec2 max, float rounding, ImDrawCornerFlags rounding_corners = 15)
{
    pDrawList->AddCallback(&CBlurProcessor::Start, pDevice);
    
    const unsigned int uBackBufferWidth = CBlurProcessor::Get().uBackBufferWidth;
    const unsigned int uBackBufferHeight = CBlurProcessor::Get().uBackBufferHeight;

    ImVec2 uv_min { min.x / (float)render.width, min.y / (float)render.height };
    ImVec2 uv_max { max.x / (float)render.width, max.y / (float)render.height };
    // process blur callback for needed strength
    for (int i = 0; i < 1; i++)
    {
        pDrawList->AddCallback(&CBlurProcessor::First, pDevice);
        pDrawList->AddImage(CBlurProcessor::Get().pBlurFirst, min, max, uv_min, uv_max, IM_COL32_WHITE);
        pDrawList->AddCallback(&CBlurProcessor::Second, pDevice);
        pDrawList->AddImage(CBlurProcessor::Get().pBlurSecond, min, max, uv_min, uv_max, IM_COL32_WHITE);
    }

    pDrawList->AddCallback(&CBlurProcessor::End, pDevice);

    pDrawList->AddImage(CBlurProcessor::Get().pBlurFirst, min, max, uv_min, uv_max, IM_COL32_WHITE);
    
}