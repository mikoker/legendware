// dear imgui: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2020-04-24: DirectX9: Add support for updating font textures.
//  2019-05-29: DirectX9: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX9: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-03-29: Misc: Fixed erroneous assert in ImGui_ImplDX9_InvalidateDeviceObjects().
//  2019-01-16: Misc: Disabled fog before drawing UI's. Fixes issue #2288.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-08: Misc: Extracted imgui_impl_dx9.cpp/.h away from the old combined DX9+Win32 example.
//  2018-06-08: DirectX9: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-07: Render: Saving/restoring Transform because they don't seem to be included in the StateBlock. Setting shading mode to Gouraud.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX9_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.

#include "../imgui.h"
#include "imgui_impl_dx9.h"
#include "../../shaders/generic_vertex.h"
#include "../../shaders/circle_scissor_pixel.h"

// DirectX
#include <d3d9.h>

static const BYTE g_PixelShaderData[] = {
	// HLSL source code
	/*
		sampler tex0 : register(s0);

		struct PS_Input
		{
			float4 pos : VPOS;
			float2 uv  : TEXCOORD0;
			float4 col : COLOR0;
		};

		struct PS_Output
		{
			float4 col : COLOR;
		};

		PS_Output main(PS_Input input)
		{
			PS_Output output;

			output.col = input.col * tex2D(tex0, input.uv);

			return output;
		};
	*/
	// Precompile with Visual Studio 2019 HLSL compiler, ps_2_0
	   0,   2, 255, 255, 254, 255,
	  31,   0,  67,  84,  65,  66,
	  28,   0,   0,   0,  79,   0,
	   0,   0,   0,   2, 255, 255,
	   1,   0,   0,   0,  28,   0,
	   0,   0,   0,   1,   0,   0,
	  72,   0,   0,   0,  48,   0,
	   0,   0,   3,   0,   0,   0,
	   1,   0,   2,   0,  56,   0,
	   0,   0,   0,   0,   0,   0,
	 116, 101, 120,  48,   0, 171,
	 171, 171,   4,   0,  12,   0,
	   1,   0,   1,   0,   1,   0,
	   0,   0,   0,   0,   0,   0,
	 112, 115,  95,  50,  95,  48,
	   0,  77, 105,  99, 114, 111,
	 115, 111, 102, 116,  32,  40,
	  82,  41,  32,  72,  76,  83,
	  76,  32,  83, 104,  97, 100,
	 101, 114,  32,  67, 111, 109,
	 112, 105, 108, 101, 114,  32,
	  49,  48,  46,  49,   0, 171,
	  31,   0,   0,   2,   0,   0,
	   0, 128,   0,   0,   3, 176,
	  31,   0,   0,   2,   0,   0,
	   0, 128,   0,   0,  15, 144,
	  31,   0,   0,   2,   0,   0,
	   0, 144,   0,   8,  15, 160,
	  66,   0,   0,   3,   0,   0,
	  15, 128,   0,   0, 228, 176,
	   0,   8, 228, 160,   5,   0,
	   0,   3,   0,   0,  15, 128,
	   0,   0, 228, 128,   0,   0,
	 228, 144,   1,   0,   0,   2,
	   0,   8,  15, 128,   0,   0,
	 228, 128, 255, 255,   0,   0
};

// DirectX data
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static LPDIRECT3DTEXTURE9       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 4096, g_IndexBufferSize = 8196;
static IDirect3DVertexDeclaration9* pVertexDeclaration = nullptr;
static IDirect3DVertexShader9* pVertexShader = nullptr;
IDirect3DPixelShader9* pPixelShader = nullptr;
static IDirect3DPixelShader9* pCircleScissorPixelShader = nullptr;



static int                      g_FontTextureWidth = 0;
static int                      g_FontTextureHeight = 0;
struct CUSTOMVERTEX
{
	float	 pos[2];
	float    uv[2];
	unsigned int col;
};

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

static void ImGui_ImplDX9_SetupRenderState(ImDrawData* draw_data)
{
	// Setup viewport
	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = (DWORD)draw_data->DisplaySize.x;
	vp.Height = (DWORD)draw_data->DisplaySize.y;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	g_pd3dDevice->SetViewport(&vp);

	g_pd3dDevice->SetVertexDeclaration(pVertexDeclaration);
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetIndices(g_pIB);

	float L = draw_data->DisplayPos.x + 0.5f;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
	float T = draw_data->DisplayPos.y + 0.5f;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;

	// used formula of D3DXMatrixOrthoOffCenterLH() function
	D3DMATRIX matProjection =
	{ { {
		2.0f / (R - L),     0.0f,               0.0f,  0.0f,
		0.0f,               2.0f / (T - B),     0.0f,  0.0f,
		0.0f,               0.0f,               0.5f,  0.0f,
		(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f
	} } };

	g_pd3dDevice->SetVertexShaderConstantF(0U, &matProjection.m[0][0], 4U);

	// Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
	g_pd3dDevice->SetVertexShader(pVertexShader);
	g_pd3dDevice->SetPixelShader(pPixelShader);
	g_pd3dDevice->SetSamplerState(0UL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetSamplerState(0UL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	g_pd3dDevice->SetSamplerState(0UL, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0UL, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// rasterizer states
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	// depth stencil states
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// blend states
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);

	// clipping states
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);

	// disabled states
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

}

// Render function.
void ImGui_ImplDX9_RenderDrawData(ImDrawData* draw_data)
{
	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		return;

	// Create and grow buffers if needed
	if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 4096;
		if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(ImDrawVert), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0UL, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
		g_IndexBufferSize = draw_data->TotalIdxCount + 8196;
		if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
			return;
	}

	// Backup the DX9 state
	IDirect3DStateBlock9* d3d9_state_block = NULL;
	if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
		return;
	if (d3d9_state_block->Capture() < 0)
	{
		d3d9_state_block->Release();
		return;
	}

	// Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't appear to)
	//D3DMATRIX last_world, last_view, last_projection;
	//g_pd3dDevice->GetTransform(D3DTS_WORLD, &last_world);
	//g_pd3dDevice->GetTransform(D3DTS_VIEW, &last_view);
	//g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &last_projection);

	// Allocate buffers
	ImDrawVert* vtx_dst;
	ImDrawIdx* idx_dst;
	if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(ImDrawVert)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
	{
		d3d9_state_block->Release();
		return;
	}
	if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
	{
		g_pVB->Unlock();
		d3d9_state_block->Release();
		return;
	}

	// Copy and convert all vertices into a single contiguous buffer, convert colors to DX9 default format.
	// FIXME-OPT: This is a minor waste of resource, the ideal is to use imconfig.h and
	//  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
	//  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
		{
			vtx_dst->pos[0] = vtx_src->pos.x;
			vtx_dst->pos[1] = vtx_src->pos.y;
			//vtx_dst->pos[2] = 0.0f;
			vtx_dst->col = IMGUI_COL_TO_DX9_ARGB(vtx_src->col);
			vtx_dst->uv[0] = vtx_src->uv.x;
			vtx_dst->uv[1] = vtx_src->uv.y;
			vtx_dst++;
			vtx_src++;
		}
		// memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		// vtx_dst += cmd_list->VtxBuffer.Size;
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	g_pVB->Unlock();
	g_pIB->Unlock();

	// Setup desired DX state
	ImGui_ImplDX9_SetupRenderState(draw_data);

	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	int global_vtx_offset = 0;
	int global_idx_offset = 0;
	ImVec2 clip_off = draw_data->DisplayPos;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != NULL)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
					ImGui_ImplDX9_SetupRenderState(draw_data);
				else
					pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const LPDIRECT3DTEXTURE9 texture = (LPDIRECT3DTEXTURE9)pcmd->TextureId;
				g_pd3dDevice->SetTexture(0, texture);

				if (pcmd->ClipRect.ClipRectCircle)
				{
					// set circle scissoring min, max, lenght
					float arrCircleScissorParameters[3] = { };
					arrCircleScissorParameters[0] = static_cast<float>(pcmd->ClipRect.v.x + pcmd->ClipRect.v.z) * 0.5f;
					arrCircleScissorParameters[1] = static_cast<float>(pcmd->ClipRect.v.y + pcmd->ClipRect.v.w) * 0.5f;
					arrCircleScissorParameters[2] = static_cast<float>(pcmd->ClipRect.v.z - pcmd->ClipRect.v.x) * 0.5f;
					arrCircleScissorParameters[2] *= arrCircleScissorParameters[2];
					g_pd3dDevice->SetPixelShaderConstantF(0U, arrCircleScissorParameters, 1U);

					// set sampler state
					const BOOL bIsSampler = (texture != nullptr && texture != ImGui::GetIO().Fonts->TexID) ? TRUE : FALSE;
					g_pd3dDevice->SetPixelShaderConstantB(0U, &bIsSampler, 1U);

					// apply circle scissor shader
					g_pd3dDevice->SetPixelShader(pCircleScissorPixelShader);
				}

				const RECT r = { (LONG)(pcmd->ClipRect.v.x - clip_off.x), (LONG)(pcmd->ClipRect.v.y - clip_off.y), (LONG)(pcmd->ClipRect.v.z - clip_off.x), (LONG)(pcmd->ClipRect.v.w - clip_off.y) };
				g_pd3dDevice->SetScissorRect(&r);

				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, pcmd->VtxOffset + global_vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);
				if (pcmd->ClipRect.ClipRectCircle)
					g_pd3dDevice->SetPixelShader(pPixelShader);
			}
		}
		global_idx_offset += cmd_list->IdxBuffer.Size;
		global_vtx_offset += cmd_list->VtxBuffer.Size;
	}

	// Restore the DX9 state
	d3d9_state_block->Apply();
	d3d9_state_block->Release();
}

bool ImGui_ImplDX9_Init(IDirect3DDevice9* device)
{
	// Setup backend capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "imgui_impl_dx9";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

	g_pd3dDevice = device;
	g_pd3dDevice->AddRef();
	return true;
}

void ImGui_ImplDX9_Shutdown()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }


}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	// Convert RGBA32 to BGRA32 (because RGBA32 is not well supported by DX9 devices)


	// Upload texture to graphics system
	g_FontTexture = NULL;
	if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	g_FontTexture->UnlockRect(0);

	// Store our identifier
	io.Fonts->SetTexID((ImTextureID)g_FontTexture);



	return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	// create generic vertex declaration
	if (pVertexDeclaration == nullptr)
	{
		/*
		* @note: declare vertex format constants (stream-mapping)
		* to describe the contents of vertices interleaved in a single data stream
		* that will be processed by the programmable vertex pipeline
		* similar to SetFVF but we are not using fixed-function pipeline
		*/
		constexpr D3DVERTEXELEMENT9 arrVertexElements[]
		{
			// Vector2D vecPosition
			{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			// Vector2D vecUV
			{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			// Color32_t uColor
			{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			D3DDECL_END()
		};

		g_pd3dDevice->CreateVertexDeclaration(arrVertexElements, &pVertexDeclaration);
	}

	if (pVertexShader == nullptr)
	{
		if (g_pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(arrVertexShaderData), &pVertexShader) != D3D_OK)
			return false;
	}

	if (pPixelShader == nullptr)
	{
		if (g_pd3dDevice->CreatePixelShader(reinterpret_cast<const DWORD*>(g_PixelShaderData), &pPixelShader) != D3D_OK)
			return false;
	}

	if (pCircleScissorPixelShader == nullptr)
	{
		if (g_pd3dDevice->CreatePixelShader(reinterpret_cast<const DWORD*>(arrCircleScissorShaderData), &pCircleScissorPixelShader) != D3D_OK)
			return false;
	}
	if (!ImGui_ImplDX9_CreateFontsTexture())
		return false;
	return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
	if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
	if (pVertexDeclaration != nullptr)
	{
		pVertexDeclaration->Release();
		pVertexDeclaration = nullptr;
	}

	if (pVertexShader != nullptr)
	{
		pVertexShader->Release();
		pVertexShader = nullptr;
	}

	if (pPixelShader)
	{
		pPixelShader->Release();
		pPixelShader = nullptr;
	}

	if (pCircleScissorPixelShader != nullptr)
	{
		pCircleScissorPixelShader->Release();
		pCircleScissorPixelShader = nullptr;
	}
	if (!g_pd3dDevice)
		return;
	if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
	if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }

	if (g_FontTexture) { g_FontTexture->Release(); g_FontTexture = NULL; ImGui::GetIO().Fonts->TexID = NULL; } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.

}
static bool ImGui_ImplDX9_UpdateFontsTexture()
{
	ImGuiIO& io = ImGui::GetIO();

	// Build texture atlas
	unsigned char* pixels;
	int width, height;
	int dirty_x, dirty_y, dirty_width, dirty_height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL, &dirty_x, &dirty_y, &dirty_width, &dirty_height);

	if ((!g_FontTexture) || (g_FontTextureWidth != width) || (g_FontTextureHeight != height))
	{
		// (Re-)create texture
		if (g_FontTexture)
			g_FontTexture->Release();
		io.Fonts->TexID = NULL;
		g_FontTexture = NULL;
		if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
			return false;

		// Store size
		g_FontTextureWidth = width;
		g_FontTextureHeight = height;

		// Store our identifier
		io.Fonts->SetTexID((ImTextureID)g_FontTexture);

		// If we recreated the texture we need to upload everything
		dirty_x = dirty_y = 0;
		dirty_width = width;
		dirty_height = height;
	}

	if ((dirty_width > 0) && (dirty_height > 0))
	{
		// Upload the dirty region

		D3DLOCKED_RECT tex_locked_rect;
		RECT dirty_rect;
		dirty_rect.left = dirty_x;
		dirty_rect.right = dirty_x + dirty_width;
		dirty_rect.top = dirty_y;
		dirty_rect.bottom = dirty_y + dirty_height;
		if (g_FontTexture->LockRect(0, &tex_locked_rect, &dirty_rect, 0) != D3D_OK)
			return false;

		if ((dirty_x == 0) && (dirty_y == 0) && (dirty_width == width) && (dirty_height == height) && (tex_locked_rect.Pitch == (width * 4)))
		{
			memcpy(tex_locked_rect.pBits, pixels, width * height * 4); // Fast path for full image upload
		}
		else
		{
			// Sub-region upload
			const int src_stride = width * 4;
			const int dest_stride = tex_locked_rect.Pitch;
			const int copy_bytes = dirty_width * 4; // Bytes to copy for each line
			unsigned char* read_ptr = pixels + (dirty_x * 4) + (dirty_y * src_stride);
			char* write_ptr = (char*)tex_locked_rect.pBits;

			for (int y = 0; y < dirty_height; y++)
			{
				memcpy(write_ptr, read_ptr, copy_bytes);
				write_ptr += dest_stride;
				read_ptr += src_stride;
			}
		}
		g_FontTexture->UnlockRect(0);
	}

	return true;
}



void ImGui_ImplDX9_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplDX9_CreateDeviceObjects();
	ImGuiIO& io = ImGui::GetIO();

	if (io.Fonts->IsDirty())
		ImGui_ImplDX9_UpdateFontsTexture();

}

