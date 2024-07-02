#include "../includes.h"

c_d3d9 d3d9;

bool c_d3d9::create_device(HWND hwnd)
{
    m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!m_d3d)
        return false;

    memset(&m_params, 0, sizeof(m_params));

    m_params.BackBufferFormat = D3DFMT_UNKNOWN;
    m_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_params.Windowed = TRUE;
    m_params.EnableAutoDepthStencil = TRUE;
    m_params.AutoDepthStencilFormat = D3DFMT_D16;
    m_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_params, &m_device) < 0)
        return false;

    return true;
}

void c_d3d9::clean_device()
{   
    if (m_device)
    { 
        m_device->Release(); 
        m_device = nullptr; 
    }

    if (m_d3d)
    { 
        m_d3d->Release();
        m_d3d = nullptr; 
    }
}

bool c_d3d9::resize_window(WPARAM w_param, LPARAM l_param)
{
    if (!m_device)
        return false;

    if (w_param == SIZE_MINIMIZED)
        return false;

    m_params.BackBufferWidth = LOWORD(l_param);
    m_params.BackBufferHeight = HIWORD(l_param);

    if (!reset_device())
        return false;

    return true;
}

bool c_d3d9::reset_device()
{
    if (!m_device)
        return false;

    ImGui_ImplDX9_InvalidateDeviceObjects();

    HRESULT hr = m_device->Reset(&m_params);
   
    if (hr == D3DERR_INVALIDCALL)
        return false;

    if (!ImGui_ImplDX9_CreateDeviceObjects())
        return false;

    return true;
}