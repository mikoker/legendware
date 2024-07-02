#pragma once

#ifndef D3D_H
#define D3D_H

class c_d3d9
{
public:
	c_d3d9() = default;
	~c_d3d9() = default;

public:
	bool create_device(HWND hwnd);
	bool reset_device();
	void clean_device();

public:
	bool resize_window(WPARAM w_param, LPARAM l_param);

public:
	LPDIRECT3DDEVICE9 get_device() { return m_device; }

private:
	LPDIRECT3D9 m_d3d = nullptr;
	LPDIRECT3DDEVICE9 m_device = nullptr;
	D3DPRESENT_PARAMETERS m_params = {};
};

extern c_d3d9 d3d9;

#endif // !D3D_H