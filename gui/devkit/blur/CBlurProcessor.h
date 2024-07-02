#pragma once
#include <d3d9.h>
#include "../../third-party/imgui/imgui.h"

template <typename T>
class CSingleton
{
protected:
    CSingleton() { }
    ~CSingleton() { }

    CSingleton(const CSingleton&) = delete;
    CSingleton& operator=(const CSingleton&) = delete;

    CSingleton(CSingleton&&) = delete;
    CSingleton& operator=(CSingleton&&) = delete;
public:
    static T& Get()
    {
        static T pInstance{ };
        return pInstance;
    }
};
class CBlurProcessor : public CSingleton<CBlurProcessor>
{
public:
    CBlurProcessor() = default;

    // Get
    static void Start(const ImDrawList* pDrawList, const ImDrawCmd* pCmd)
    {
        Get().BeginBlur(static_cast<IDirect3DDevice9*>(pCmd->UserCallbackData));
    }

    static void End(const ImDrawList* pDrawList, const ImDrawCmd* pCmd)
    {
        Get().EndBlur(static_cast<IDirect3DDevice9*>(pCmd->UserCallbackData));
    }

    static void First(const ImDrawList* pDrawList, const ImDrawCmd* pCmd)
    {
        Get().FirstBlurPass(static_cast<IDirect3DDevice9*>(pCmd->UserCallbackData));
    }

    static void Second(const ImDrawList* pDrawList, const ImDrawCmd* pCmd)
    {
        Get().SecondBlurPass(static_cast<IDirect3DDevice9*>(pCmd->UserCallbackData));
    }

    // Values
    /* first pass blur texture */
    IDirect3DTexture9* pBlurFirst = nullptr;
    /* second pass blur texture */
    IDirect3DTexture9* pBlurSecond = nullptr;
    /* full backbuffer width */
    unsigned int uBackBufferWidth = 0U;
    /* full backbuffer height */
    unsigned int uBackBufferHeight = 0U;

private:
    // Main
    void BeginBlur(IDirect3DDevice9* pDevice);
    void EndBlur(IDirect3DDevice9* pDevice) const;
    void FirstBlurPass(IDirect3DDevice9* pDevice) const;
    void SecondBlurPass(IDirect3DDevice9* pDevice) const;

    // Values
    /* saved render target to restore it after our modifications */
    IDirect3DSurface9* pSurfaceBackup = nullptr;
    /* horizontal blur shader */
    IDirect3DPixelShader9* pBlurShaderX = nullptr;
    /* vertical blur shader */
    IDirect3DPixelShader9* pBlurShaderY = nullptr;
    /* backbuffer texture downsample */
    static constexpr int nBlurDownsample = 2;
};


