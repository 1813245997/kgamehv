// dear imgui: Renderer Backend for DirectX11
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'ID3D11ShaderResourceView*' as texture identifier. Read the FAQ about ImTextureID/ImTextureRef!
//  [X] Renderer: Large meshes support (64k+ vertices) even with 16-bit indices (ImGuiBackendFlags_RendererHasVtxOffset).
//  [X] Renderer: Texture updates support for dynamic font atlas (ImGuiBackendFlags_RendererHasTextures).
//  [X] Renderer: Expose selected render state for draw callbacks to use. Access in '(ImGui_ImplXXXX_RenderState*)GetPlatformIO().Renderer_RenderState'.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2025-06-11: DirectX11: Added support for ImGuiBackendFlags_RendererHasTextures, for dynamic font atlas.
//  2025-05-07: DirectX11: Honor draw_data->FramebufferScale to allow for custom backends and experiment using it (consistently with other renderer backends, even though in normal condition it is not set under Windows).
//  2025-01-06: DirectX11: Expose VertexConstantBuffer in ImGui_ImplDX11_RenderState. Reset projection matrix in ImDrawCallback_ResetRenderState handler.
//  2024-10-07: DirectX11: Changed default texture sampler to Clamp instead of Repeat/Wrap.
//  2024-10-07: DirectX11: Expose selected render state in ImGui_ImplDX11_RenderState, which you can access in 'void* platform_io.Renderer_RenderState' during draw callbacks.
//  2022-10-11: Using 'nullptr' instead of 'NULL' as per our switch to C++11.
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2021-05-19: DirectX11: Replaced direct access to ImDrawCmd::TextureId with a call to ImDrawCmd::GetTexID(). (will become a requirement)
//  2021-02-18: DirectX11: Change blending equation to preserve alpha in output buffer.
//  2019-08-01: DirectX11: Fixed code querying the Geometry Shader state (would generally error with Debug layer enabled).
//  2019-07-21: DirectX11: Backup, clear and restore Geometry Shader is any is bound when calling ImGui_ImplDX11_RenderDrawData. Clearing Hull/Domain/Compute shaders without backup/restore.
//  2019-05-29: DirectX11: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX11: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2018-12-03: Misc: Added #pragma comment statement to automatically link with d3dcompiler.lib when using D3DCompile().
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-08-01: DirectX11: Querying for IDXGIFactory instead of IDXGIFactory1 to increase compatibility.
//  2018-07-13: DirectX11: Fixed unreleased resources in Init and Shutdown functions.
//  2018-06-08: Misc: Extracted imgui_impl_dx11.cpp/.h away from the old combined DX11+Win32 example.
//  2018-06-08: DirectX11: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX11_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2016-05-07: DirectX11: Disabling depth-write.


#include "imgui.h"
//#ifndef IMGUI_DISABLE
#include "imgui_impl_dx11.h"

#include "../global_defs.h"
#include "../dx11.h"

#include <cstddef> 
#include "../../ia32/ia32.hpp"
#define	D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION	( 16384 )
// DirectX
//#include <stdio.h>
//#include <d3d11.h>
//#include <d3dcompiler.h>
//#ifdef _MSC_VER
//#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
//#endif
//
//// Clang/GCC warnings with -Weverything
//#if defined(__clang__)
//#pragma clang diagnostic ignored "-Wold-style-cast"         // warning: use of old-style cast                            // yes, they are more terse.
//#pragma clang diagnostic ignored "-Wsign-conversion"        // warning: implicit conversion changes signedness
//#endif
//
// DirectX11 data
struct ImGui_ImplDX11_Texture
{
    PVOID            pTexture;
    PVOID     pTextureView;
};

struct ImGui_ImplDX11_Data
{
	 PVOID  pd3dDevice;
     PVOID  pd3dDeviceContext;
     PVOID  pFactory;
     PVOID  pVB;
     PVOID  pIB;
     PVOID  pVertexShader;
     PVOID  pInputLayout;
     PVOID  pVertexConstantBuffer;
     PVOID  pPixelShader;
	 PVOID  pFontSampler;
     PVOID  pRasterizerState;
     PVOID  pBlendState;
     PVOID  pDepthStencilState;
	int                         VertexBufferSize;
	int                         IndexBufferSize;

	ImGui_ImplDX11_Data() { memset((void*)this, 0, sizeof(*this)); VertexBufferSize = 5000; IndexBufferSize = 10000; }
};
//
//struct VERTEX_CONSTANT_BUFFER_DX11
//{
//    float   mvp[4][4];
//};
//
// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplDX11_Data* ImGui_ImplDX11_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplDX11_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Functions
static void ImGui_ImplDX11_SetupRenderState(ImDrawData* draw_data, PVOID device_ctx)
{
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();

    D3D11_VIEWPORT * vp = (D3D11_VIEWPORT*)  utils::strong_dx::g_user_buffer;
    // Setup viewport
    
    vp->Width = draw_data->DisplaySize.x * draw_data->FramebufferScale.x;
    vp->Height = draw_data->DisplaySize.y * draw_data->FramebufferScale.y;
    vp->MinDepth = 0.0f;
    vp->MaxDepth = 1.0f;
    vp->TopLeftX = vp->TopLeftY = 0;

    
  

    
    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device_ctx, 44),
        (unsigned long long)device_ctx,
        1,
        (unsigned long long) vp,
        0

    );

     
	// Map staging texture 读写
	 

    D3D11_MAPPED_SUBRESOURCE* mapped_resource = (D3D11_MAPPED_SUBRESOURCE*)utils::strong_dx::g_user_buffer;
	// Setup orthographic projection matrix into our constant buffer
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	auto  usercall_retval_ptr = utils:: user_call::call6(
		reinterpret_cast<unsigned long long>(
			utils::vfun_utils::get_vfunc(device_ctx, 14)),
		reinterpret_cast<unsigned long long>(device_ctx),
		reinterpret_cast<unsigned long long>(bd->pVertexConstantBuffer),
		0,
        D3D11_MAP_WRITE_DISCARD,
		0,
        reinterpret_cast<unsigned long long>(mapped_resource)
	);

   ;
   HRESULT	hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
   if (hr == S_OK)
   {
      


	   VERTEX_CONSTANT_BUFFER_DX11* constant_buffer = (VERTEX_CONSTANT_BUFFER_DX11*)mapped_resource->pData;
	   float L = draw_data->DisplayPos.x;
	   float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	   float T = draw_data->DisplayPos.y;
	   float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	   float mvp[4][4] =
	   {
		   { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
		   { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
		   { 0.0f,         0.0f,           0.5f,       0.0f },
		   { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
	   };
	   memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));

	 

       utils::user_call::call(
		   reinterpret_cast<unsigned long long>(
			   utils::vfun_utils::get_vfunc(device_ctx, 15)),
		   reinterpret_cast<unsigned long long>(device_ctx),
		   reinterpret_cast<unsigned long long>(bd->pVertexConstantBuffer),
		   0,
		   0
	   );

	   
   }

    
 

    // Setup shader and vertex buffers
 
    
   
     
     
   
  
   
   
    
   
  
   
   
   
    utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 17),
        (unsigned long long)device_ctx,
        (unsigned long long) bd->pInputLayout,
        0,
        0

    );

   
    unsigned int* stride = (unsigned int*)utils::strong_dx::g_user_buffer;  // sizeof(ImDrawVert);
	unsigned int  * offset = (unsigned int*)utils::strong_dx::g_user_buffer+8;
    *stride = sizeof(ImDrawVert);
	utils::user_call::call6(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 18),
        (unsigned long long)device_ctx,
		0,
		1,
        (unsigned long long) &bd->pVB,
        (unsigned long long) stride,
        (unsigned long long) offset
	);
   * stride =*(unsigned int*)(utils::strong_dx::g_user_buffer + 4);
   * offset = *(unsigned int*)(utils::strong_dx::g_user_buffer + 8 );

    

    utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 19),
        (unsigned long long)device_ctx,
        (unsigned long long)bd->pIB,
        sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
        0
    );

    
    utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 24),
        (unsigned long long)device_ctx,
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        0,
        0

    );

    utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 11),
        (unsigned long long)device_ctx,
        (unsigned long long)bd->pVertexShader,
        0,
        0
    );

 

    utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 7),
        (unsigned long long)device_ctx,
        0,
        1,
        (unsigned long long)&bd->pVertexConstantBuffer
    );

	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 9),
		(unsigned long long)device_ctx,
        (unsigned long long) bd->pPixelShader,
		0,
		0
	);
    
    utils::memory::mem_copy((PVOID)utils::strong_dx::g_user_buffer, &bd->pFontSampler, sizeof(PVOID));

	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 10),
			(unsigned long long)device_ctx,
            0,
            1,
        (unsigned long long)& bd->pFontSampler);
    

	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 23),
		(unsigned long long)device_ctx,
		0,
		0,
		0);
    
	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 60),
		(unsigned long long)device_ctx,
		0,
		0,
		0);

	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 64),
		(unsigned long long)device_ctx,
		0,
		0,
		0);
    
	utils::user_call::call(
        (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 69),
		(unsigned long long)device_ctx,
		0,
		0,
		0);
  

    float* blend_factor =(float*)utils::strong_dx::g_user_buffer;

    blend_factor[0 ]= { 0.f};
    blend_factor[1] = { 0.f };
    blend_factor[2] = { 0.f };
    blend_factor[3] = { 0.f };

 		utils::user_call::call(
            (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 35),
			(unsigned long long)device_ctx,
            (unsigned long long) bd->pBlendState,
            (unsigned long long) blend_factor,
            0xffffffff);
    // Setup render state
		utils::user_call::call(
            (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 36),
			(unsigned long long)device_ctx,
			(unsigned long long) bd->pDepthStencilState,
            0,
            0);
		utils::user_call::call(
            (unsigned long long)utils::vfun_utils::get_vfunc(device_ctx, 43),
			(unsigned long long)device_ctx,
			(unsigned long long) bd->pRasterizerState,
			0,
			0);
     
  
}

// Render function
 void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    PVOID device = bd->pd3dDeviceContext;
 
    // Catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to do.
    // (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding or disabling texture updates).
    if (draw_data->Textures != nullptr)
        for (ImTextureData* tex : *draw_data->Textures)
            if (tex->Status != ImTextureStatus_OK)
                ImGui_ImplDX11_UpdateTexture(tex); 
  
    // Create and grow vertex/index buffers if needed
    if (!bd->pVB || bd->VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (bd->pVB) { utils::vfun_utils::release(bd->pVB)  ; bd->pVB = nullptr; }
        bd->VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = bd->VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

      
        utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, &desc, sizeof(D3D11_BUFFER_DESC));

        unsigned long long CreateBufferfun = (unsigned long long)  utils::vfun_utils::get_vfunc(bd->pd3dDevice, 3);

        auto 	usercall_retval_ptr =   utils::user_call::call(
            CreateBufferfun,
            (unsigned long long)bd->pd3dDevice,
            utils::dwm_draw::g_imgui_buffer,
            0,
            (unsigned long long) &bd->pVB
          );

        HRESULT hr = *(PULONG)usercall_retval_ptr;


        if (hr < 0)
            return;
        
    }


    
    if (!bd->pIB || bd->IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (bd->pIB) { utils::vfun_utils::release(bd->pIB) ; bd->pIB = nullptr; }
        bd->IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = bd->IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	 
		utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, &desc, sizeof(D3D11_BUFFER_DESC));
		unsigned long long CreateBufferfun = (unsigned long long)  utils::vfun_utils::get_vfunc(bd->pd3dDevice, 3);

		auto 	usercall_retval_ptr = utils::user_call::call(
			CreateBufferfun,
			(unsigned long long)bd->pd3dDevice,
			utils::dwm_draw::g_imgui_buffer,
			0,
            (unsigned long long) &bd->pIB);

		HRESULT hr = *(PULONG)usercall_retval_ptr;


		if (hr < 0)
			return;
      
       
    }


  
    // Upload vertex/index data into a single contiguous GPU buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;

    auto map_fun = reinterpret_cast<unsigned long long>(
        utils::vfun_utils::get_vfunc(device, 14));

    auto  usercall_retval_ptr = utils::user_call::call6(
        map_fun,
        reinterpret_cast<unsigned long long>(device),
        reinterpret_cast<unsigned long long>(bd->pVB),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        utils::strong_dx:: g_user_buffer
    );

    HRESULT hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
    
    if (hr != S_OK)
    {
        return;
    }
    memcpy(&vtx_resource, reinterpret_cast<PVOID>(utils::strong_dx::g_user_buffer), sizeof(D3D11_MAPPED_SUBRESOURCE));
   
	   usercall_retval_ptr = utils::user_call::call6(
		map_fun,
		reinterpret_cast<unsigned long long>(device),
		reinterpret_cast<unsigned long long>(bd->pIB),
		0,
           D3D11_MAP_WRITE_DISCARD,
		0,
		utils::strong_dx::g_user_buffer
	);

	  hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);

	if (hr != S_OK)
	{
		return;
	}
	memcpy(&idx_resource, reinterpret_cast<PVOID>(utils::strong_dx::g_user_buffer), sizeof(D3D11_MAPPED_SUBRESOURCE));
  
    //渲染
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
    
    
      
		const ImDrawList* draw_list = draw_data->CmdLists[n];
		memcpy(vtx_dst, draw_list->VtxBuffer.Data, draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, draw_list->IdxBuffer.Data, draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		 
		vtx_dst += draw_list->VtxBuffer.Size;
		idx_dst += draw_list->IdxBuffer.Size;
    }


	auto unmap_fun = reinterpret_cast<unsigned long long>(
		utils::vfun_utils::get_vfunc(device, 15));

	utils:: user_call::call(
		unmap_fun,
		reinterpret_cast<unsigned long long>(device),
		reinterpret_cast<unsigned long long>(bd->pVB),
		0,
		0
	);
	utils::user_call::call(
		unmap_fun,
		reinterpret_cast<unsigned long long>(device),
		reinterpret_cast<unsigned long long>(bd->pIB),
		0,
		0
	);
 

    // Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
    struct BACKUP_DX11_STATE
    {
        UINT                        ScissorRectsCount, ViewportsCount;
        D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        PVOID      RS;
        PVOID           BlendState;
        FLOAT                       BlendFactor[4];
        UINT                        SampleMask;
        UINT                        StencilRef;
        PVOID     DepthStencilState;
        PVOID    PSShaderResource;
        PVOID       PSSampler;
        PVOID       PS;
        PVOID       VS;
        PVOID       GS;
        UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
         PVOID         *PSInstances[256], *VSInstances[256], *GSInstances[256];   // 256 is max according to PSSetShader documentation
        D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
        PVOID               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
        UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
        DXGI_FORMAT                 IndexBufferFormat;
        PVOID          InputLayout;
    };
    BACKUP_DX11_STATE *  old = (BACKUP_DX11_STATE*)utils::strong_dx::g_old_user_buffer;
     
     
	old->ScissorRectsCount = old->ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

     

    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 96),
        (unsigned long long)  device,
        (unsigned long long)&old->ScissorRectsCount,
        (unsigned long long) old->ScissorRects,
        0
    );

      
    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 95),
        (unsigned long long)  device,
        (unsigned long long)&old->ViewportsCount,
        (unsigned long long)old->Viewports,
        0
    );

 


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 94),
		(unsigned long long)  device,
        (unsigned long long)&old->RS,
	    0,
		0
	);
     

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 91),
		(unsigned long long)  device,
        (unsigned long long)&old->BlendState,
        (unsigned long long)  old->BlendFactor,
        (unsigned long long)&old->SampleMask
	);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 92),
		(unsigned long long)  device,
        (unsigned long long)&old->DepthStencilState,
        (unsigned long long)&old->StencilRef,
        0
	 
	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 73),
		(unsigned long long)  device,
        0, 
        1,
        (unsigned long long)&old->PSShaderResource
	 
	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 75),
		(unsigned long long)  device,
        0, 
        1, 
        (unsigned long long)&old->PSSampler

	);
   
    old->PSInstancesCount = old->VSInstancesCount = old->GSInstancesCount = 256;
   
     

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 74),
		(unsigned long long)  device,
        (unsigned long long)&old->PS,
        (unsigned long long) old->PSInstances,
        (unsigned long long)&old->PSInstancesCount

	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 76),
		(unsigned long long)  device,
        (unsigned long long)&old->VS, 
        (unsigned long long) old->VSInstances,
        (unsigned long long)&old->VSInstancesCount

	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 72),
		(unsigned long long)  device,
        0,
        1, 
        (unsigned long long)&old->VSConstantBuffer

	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 82),
		(unsigned long long)  device,
        (unsigned long long)&old->GS,
        (unsigned long long)old->GSInstances,
        (unsigned long long)&old->GSInstancesCount

	);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 83),
		(unsigned long long)  device,
        (unsigned long long)&old->PrimitiveTopology,
		 0,
        0

	);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 80),
		(unsigned long long)  device,
        (unsigned long long)&old->IndexBuffer,
        (unsigned long long)&old->IndexBufferFormat,
        (unsigned long long)&old->IndexBufferOffset

	);

	utils::user_call::call6(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 79),
		(unsigned long long)  device,
        0, 1, 
        (unsigned long long)&old->VertexBuffer,
        (unsigned long long)&old->VertexBufferStride,
        (unsigned long long)&old->VertexBufferOffset

	);


	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 78),
		(unsigned long long)  device,
        (unsigned long long)&old->InputLayout,
        0,
        0
		 

	);

 

    // Setup desired DX state
    ImGui_ImplDX11_SetupRenderState(draw_data, device);

	// 2. 分配用户态内存
	static 	 PVOID user_render_state{};
	if (!user_render_state)
	{
		NTSTATUS status = utils::memory::allocate_user_memory(&user_render_state,
			sizeof(ImGui_ImplDX11_RenderState),
			PAGE_READWRITE,
			true, false);
		if (!NT_SUCCESS(status))
			return;
	}

    // Setup render state structure (for callbacks and custom texture bindings)
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	// 1. 创建原始 RenderState
	ImGui_ImplDX11_RenderState* render_state =(ImGui_ImplDX11_RenderState*) user_render_state;
	render_state->Device = bd->pd3dDevice;
	render_state->DeviceContext = bd->pd3dDeviceContext;
	render_state->SamplerDefault = bd->pFontSampler;
	render_state->VertexConstantBuffer = bd->pVertexConstantBuffer;
	platform_io.Renderer_RenderState = render_state;
    
    
    //下面循环有问题
    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_idx_offset = 0;
    int global_vtx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    ImVec2 clip_scale = draw_data->FramebufferScale;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* draw_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDX11_SetupRenderState(draw_data, device);
                else
                    pcmd->UserCallback(draw_list, pcmd);
            }
            else
            {

             
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                const D3D11_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };

 
                utils::memory::mem_copy((PVOID)utils::strong_dx::g_user_buffer,(PVOID) &r, sizeof(D3D11_RECT));

                utils::user_call::call(
                    (unsigned long long)  utils::vfun_utils::get_vfunc(device, 45),
                    (unsigned long long)device,
                    1,
                    utils::strong_dx::g_user_buffer, 0);



              

                // Bind texture, Draw
                 PVOID texture_srv = (PVOID)pcmd->GetTexID();

                 utils::memory::mem_copy((PVOID)utils::strong_dx::g_user_buffer, (PVOID)&texture_srv, sizeof(PVOID));

                 utils::user_call::call(
                     (unsigned long long)  utils::vfun_utils::get_vfunc(device, 8),
                     (unsigned long long)device,
                     0,
                     1,
                     utils::strong_dx::g_user_buffer
                 );
                
					 utils::user_call::call(
                         (unsigned long long)  utils::vfun_utils::get_vfunc(device, 12),
						 (unsigned long long)device,
                         pcmd->ElemCount,
                         pcmd->IdxOffset + global_idx_offset,
                         pcmd->VtxOffset + global_vtx_offset
					 );
                
            }
        }
        global_idx_offset += draw_list->IdxBuffer.Size;
        global_vtx_offset += draw_list->VtxBuffer.Size;
    }
    
  
    platform_io.Renderer_RenderState = nullptr;

    // Restore modified DX state
    
    
    
    
   
 
   
   
    
    
    
     
    
 
    //GSSetShader
    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 45),
        (unsigned long long)device,
        old->ScissorRectsCount,
        (unsigned long long)old->ScissorRects,
        0

    );

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 44),
		(unsigned long long)device,
        old->ViewportsCount,
        (unsigned long long) old->Viewports,
		0

	);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 43),
		(unsigned long long)device,
        (unsigned long long)old->RS,
		0,
		0

	);
  
    if (old->RS)
    {
        utils::vfun_utils::release(old->RS);
    }
    
	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 35),
		(unsigned long long)device,
		(unsigned long long)old->BlendState,
        (unsigned long long) old->BlendFactor, 
        old->SampleMask

	);

    if (old->BlendState) utils::vfun_utils::release(old->BlendState);
    

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 36),
		(unsigned long long)device,
		(unsigned long long)old->DepthStencilState,
        old->StencilRef,
		 0

	);

    if (old->DepthStencilState) utils::vfun_utils::release (old->DepthStencilState);
   
    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 8),
        (unsigned long long)device,
        0,
        1,
        (unsigned long long)&old->PSShaderResource
    );

    if (old->PSShaderResource) utils::vfun_utils::release(old->PSShaderResource);


    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 10),
        (unsigned long long)device,
        0,
        1,
        (unsigned long long)&old->PSSampler
    );


    if (old->PSSampler) utils::vfun_utils::release (old->PSSampler);
  

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 9),
		(unsigned long long)device,
        (unsigned long long) old->PS,
        (unsigned long long) old->PSInstances,
        old->PSInstancesCount
		 
	);

    if (old->PS) utils::vfun_utils::release (old->PS) ;
    //PSSetShaderfun
     
    
    for (UINT i = 0; i < old->PSInstancesCount; i++) if (old->PSInstances[i])utils::vfun_utils::release( old->PSInstances[i]) ;

    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 11),
        (unsigned long long)device,
        (unsigned long long)old->VS,
        (unsigned long long) old->VSInstances,
        old->VSInstancesCount


    );

    if (old->VS) utils::vfun_utils::release (old->VS) ;
     
	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 7),
		(unsigned long long)device,
		   0,
		 1,
        (unsigned long long) & old->VSConstantBuffer


	);
  
    if (old->VSConstantBuffer) utils::vfun_utils::release (old->VSConstantBuffer);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 23),
		(unsigned long long)device,
        (unsigned long long) old->GS,
        (unsigned long long) old->GSInstances,
        old->GSInstancesCount


	);
  
    if (old->GS)  utils::vfun_utils::release (old->GS);
    
    for (UINT i = 0; i < old->VSInstancesCount; i++) if (old->VSInstances[i]) utils::vfun_utils::release(old->VSInstances[i]);

	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 24),
		(unsigned long long)device,
		(unsigned long long)old->PrimitiveTopology,
		0,
		0


	);
    
	utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 19),
		(unsigned long long)device,
		(unsigned long long)old->IndexBuffer,
        old->IndexBufferFormat,
        old->IndexBufferOffset


	);
    if (old->IndexBuffer) utils::vfun_utils::release(old->IndexBuffer);
   
	utils::user_call::call6(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 18),
		(unsigned long long)device,
        0, 
        1,
        (unsigned long long) &old->VertexBuffer,
        (unsigned long long) &old->VertexBufferStride,
        (unsigned long long) &old->VertexBufferOffset


	);
  
    if (old->VertexBuffer) utils::vfun_utils::release (old->VertexBuffer);


    utils::user_call::call(
        (unsigned long long) utils::vfun_utils::get_vfunc(device, 17),
        (unsigned long long)device,
        (unsigned long long) old->InputLayout,
        0,
        0

    );
    if (old->InputLayout) utils::vfun_utils::release (old->InputLayout) ;

   
}

static void ImGui_ImplDX11_DestroyTexture(ImTextureData* tex)
{
    ImGui_ImplDX11_Texture* backend_tex = (ImGui_ImplDX11_Texture*)tex->BackendUserData;
    if (backend_tex == nullptr)
        return;
    IM_ASSERT(backend_tex->pTextureView == (PVOID)(intptr_t)tex->TexID);
    utils::vfun_utils::release(backend_tex->pTextureView);
    utils::vfun_utils::release(backend_tex->pTexture);
   
    IM_DELETE(backend_tex);

    // Clear identifiers and mark as destroyed (in order to allow e.g. calling InvalidateDeviceObjects while running)
    tex->SetTexID(ImTextureID_Invalid);
    tex->SetStatus(ImTextureStatus_Destroyed);
    tex->BackendUserData = nullptr;
}
 
void ImGui_ImplDX11_UpdateTexture(ImTextureData* tex)
{
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    if (tex->Status == ImTextureStatus_WantCreate)
    {
        // Create and upload new texture to graphics system
        //IMGUI_DEBUG_LOG("UpdateTexture #%03d: WantCreate %dx%d\n", tex->UniqueID, tex->Width, tex->Height);
        IM_ASSERT(tex->TexID == ImTextureID_Invalid && tex->BackendUserData == nullptr);
        IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);
        unsigned int* pixels = (unsigned int*)tex->GetPixels();
        ImGui_ImplDX11_Texture* backend_tex = IM_NEW(ImGui_ImplDX11_Texture)();

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        RtlZeroMemory(&desc, sizeof(desc));
        desc.Width = (UINT)tex->Width;
        desc.Height = (UINT)tex->Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
     

		// 创建 staging texture
		auto create_texture2D_fun = reinterpret_cast<unsigned long long>(
			utils::vfun_utils::get_vfunc(bd->pd3dDevice, 5));
      
         utils::memory::mem_copy((PVOID)utils::strong_dx::g_user_buffer, &desc, sizeof(desc));
         utils::memory::mem_copy((PVOID)(utils::strong_dx::g_user_buffer+ sizeof(D3D11_TEXTURE2D_DESC)), &subResource, sizeof(D3D11_SUBRESOURCE_DATA));

        
        
       
	    auto usercall_retval_ptr= utils::user_call::call(
			create_texture2D_fun,
			reinterpret_cast<unsigned long long>(bd->pd3dDevice),
           utils::strong_dx::g_user_buffer,
            utils::strong_dx::g_user_buffer + sizeof(D3D11_TEXTURE2D_DESC),
            utils::strong_dx::g_texture_buffer
		);

  HRESULT		hr = *reinterpret_cast<PULONG>(usercall_retval_ptr);
		if (FAILED(hr))
			return;

        backend_tex->pTexture = *(PVOID*)utils::strong_dx::g_texture_buffer;
        
         
        IM_ASSERT(backend_tex->pTexture != nullptr && "Backend failed to create texture!");

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        RtlZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        unsigned long long  CreateShaderResourceViewfun =(unsigned long long)utils::vfun_utils::get_vfunc(bd->pd3dDevice, 7);

		utils::memory::mem_copy((PVOID)utils::strong_dx::g_user_buffer, &srvDesc, sizeof(srvDesc));
	 
        utils::user_call::call(
            CreateShaderResourceViewfun,
            reinterpret_cast<unsigned long long> (bd->pd3dDevice),
            reinterpret_cast<unsigned long long> (backend_tex->pTexture),
            utils::strong_dx::g_user_buffer ,
            utils::strong_dx::g_user_buffer+ sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC)
             
        );
        backend_tex->pTextureView  =*(PVOID*)  ( utils::strong_dx::g_user_buffer + sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
      
        IM_ASSERT(backend_tex->pTextureView != nullptr && "Backend failed to create texture!");

        // Store identifiers
        tex->SetTexID((ImTextureID)(intptr_t)backend_tex->pTextureView);
        tex->SetStatus(ImTextureStatus_OK);
        tex->BackendUserData = backend_tex;
    }
    else if (tex->Status == ImTextureStatus_WantUpdates)
    {
        // Update selected blocks. We only ever write to textures regions which have never been used before!
        // This backend choose to use tex->Updates[] but you can use tex->UpdateRect to upload a single region.
        ImGui_ImplDX11_Texture* backend_tex = (ImGui_ImplDX11_Texture*)tex->BackendUserData;
        IM_ASSERT(backend_tex->pTextureView == (ID3D11ShaderResourceView*)(intptr_t)tex->TexID);
        for (ImTextureRect& r : tex->Updates)
        {
            D3D11_BOX box = { (UINT)r.x, (UINT)r.y, (UINT)0, (UINT)(r.x + r.w), (UINT)(r.y + r .h), (UINT)1 };
           
           unsigned long long  UpdateSubresourcefun = (unsigned long long)  utils::vfun_utils::get_vfunc(bd->pd3dDeviceContext, 48);
           utils::memory::mem_zero((PVOID)utils::dwm_draw::g_imgui_buffer, 0x1000);
           utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, &box, sizeof(D3D11_BOX));

           utils::user_call::call7(
               UpdateSubresourcefun,
               (unsigned long long)bd->pd3dDeviceContext,
               (unsigned long long) backend_tex->pTexture,
               0,
               utils::dwm_draw::g_imgui_buffer,
               (unsigned long long)  tex->GetPixelsAt(r.x, r.y),
               (unsigned long long)tex->GetPitch(),
               0
           );

          
        }
        tex->SetStatus(ImTextureStatus_OK);
    }
    if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0)
        ImGui_ImplDX11_DestroyTexture(tex);
}

bool    ImGui_ImplDX11_CreateDeviceObjects()
{
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    if (!bd->pd3dDevice)
        return false;
    ImGui_ImplDX11_InvalidateDeviceObjects();
    
    // By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
    // If you would like to use this DX11 sample code but remove this dependency you can:
    //  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
    //  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
    // See https://github.com/ocornut/imgui/pull/638 for sources and details.

		// 假设 g_imgui_buffer 已经分配 0x1000 字节
	// 1. 动态分配用户缓冲区存放返回值（ID3DBlob*）
	 
    unsigned char* buffer_ptr = reinterpret_cast<unsigned char*>(utils::strong_dx:: g_user_buffer);
	// 5. 分配用户缓冲区存放返回的 ID3DBlob* 指针
	PVOID blob_ptr = (PVOID)utils::strong_dx::g_texture_buffer;
	 
    // Create the vertex shader
    {
        static const char* vertexShader =
            "cbuffer vertexBuffer : register(b0) \
            {\
              float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
              float2 pos : POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
              PS_INPUT output;\
              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
              output.col = input.col;\
              output.uv  = input.uv;\
              return output;\
            }";

       
      

	

	

		// 1. 拷贝 vertexShader 源码
		size_t shader_len = strlen(vertexShader) + 1;
		memcpy(buffer_ptr, vertexShader, shader_len);

		// 2. 拷贝 pEntryPoint ("main") 到缓冲区末尾
		size_t entry_offset = shader_len;
		memcpy(buffer_ptr + entry_offset, "main", 5); // 包含 '\0'
		unsigned long long pEntryPoint_addr = reinterpret_cast<unsigned long long>(buffer_ptr + entry_offset);

		// 3. 拷贝 pTarget ("vs_4_0") 到缓冲区末尾
		size_t target_offset = entry_offset + 5;
		memcpy(buffer_ptr + target_offset, "vs_4_0", 7); // 包含 '\0'
		unsigned long long pTarget_addr = reinterpret_cast<unsigned long long>(buffer_ptr + target_offset);

		// 3. 调用 D3DCompile (R0→R3)
        auto ret_ptr  =   utils::user_call::call11(
            utils::dwm_draw::g_d3dcompile_fun,     // 函数地址
            reinterpret_cast<unsigned long long>(buffer_ptr), // LPCVOID pSrcData
            shader_len,                             // SIZE_T SrcDataSize
            0,                                      // LPCSTR pSourceName
            0,                                      // const D3D_SHADER_MACRO* pDefines
            0,                                      // ID3DInclude* pInclude
            pEntryPoint_addr,                                // pEntryPoint
            pTarget_addr,                                    // pTarget
            0,                                      // UINT Flags1
            0,                                      // UINT Flags2
            reinterpret_cast<unsigned long long>(blob_ptr), // 输出 ID3DBlob* 存放地址
            0);

		HRESULT hr = *reinterpret_cast<HRESULT*>(ret_ptr);
		if (FAILED(hr))
			return false;

       
        
        PVOID vertexShaderBlob = *(PVOID*)blob_ptr;

        utils::memory::mem_zero(buffer_ptr, 0x1000);

        unsigned long long  GetBufferPointerfun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(vertexShaderBlob, 3));

        unsigned  long long GetBufferSizefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(vertexShaderBlob, 4));

        ret_ptr = utils::user_call::call(GetBufferPointerfun, reinterpret_cast<unsigned  long long>( vertexShaderBlob), 0, 0, 0);
        unsigned  long long BufferPointer = *reinterpret_cast<PULONG64>(ret_ptr);

		ret_ptr = utils::user_call::call(GetBufferSizefun, reinterpret_cast<unsigned  long long>(vertexShaderBlob), 0, 0, 0);
		unsigned  long long BufferSize = *reinterpret_cast<PULONG64>(ret_ptr);

        unsigned long long CreateVertexShaderfun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 12));



		 

        utils::user_call::call6(CreateVertexShaderfun,
            reinterpret_cast<unsigned  long long>(bd->pd3dDevice),
            BufferPointer,
            BufferSize,
            0,
            reinterpret_cast<unsigned long long>(buffer_ptr),
            0
        );

		  hr = *reinterpret_cast<HRESULT*>(ret_ptr);
		if (FAILED(hr))
			return false;

          bd->pVertexShader  = *(PVOID*)buffer_ptr;
        
          utils::memory::mem_zero(buffer_ptr, 0x1000);

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC local_layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(ImDrawVert, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(ImDrawVert, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(ImDrawVert, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

       
     



		// === 获取 Shader Buffer 信息 ===
		ret_ptr = utils::user_call::call(GetBufferPointerfun, (uint64_t)vertexShaderBlob, 0, 0, 0);
		BufferPointer = *reinterpret_cast<PULONG64>(ret_ptr);

		ret_ptr = utils::user_call::call(GetBufferSizefun, (uint64_t)vertexShaderBlob, 0, 0, 0);
		BufferSize = *reinterpret_cast<PULONG64>(ret_ptr);

		// === 准备临时内存布局 ===
		// layout 数组副本
		uint8_t* cursor = reinterpret_cast<uint8_t*>(buffer_ptr);
		D3D11_INPUT_ELEMENT_DESC* desc_copy = reinterpret_cast<D3D11_INPUT_ELEMENT_DESC*>(cursor);
		memcpy(desc_copy, local_layout, sizeof(local_layout));
		cursor += sizeof(local_layout);

		// layout 对应的字符串
		for (int i = 0; i < _countof(local_layout); i++)
		{
			const char* src_str = local_layout[i].SemanticName;
			size_t len = strlen(src_str) + 1;

			memcpy(cursor, src_str, len);

			// 修正 desc_copy 内部指针
			desc_copy[i].SemanticName = reinterpret_cast<LPCSTR>(cursor);

			cursor += len;
		}

	  
	 

		  // === 调用 CreateInputLayout ===
		  ret_ptr = utils::user_call::call6(
              reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 11)),
			  (uint64_t)bd->pd3dDevice,
			  (uint64_t)desc_copy,
			  _countof(local_layout),
			  BufferPointer,
			  BufferSize,
			  (uint64_t)buffer_ptr+0x500
		  );

		  hr = *reinterpret_cast<HRESULT*>(ret_ptr);
		  if (FAILED(hr))
			  return false;

		  // 保存返回的 InputLayout
		  bd->pInputLayout = *(PVOID*)(reinterpret_cast<unsigned  long long>(buffer_ptr + 0x500)); 
           
 
        utils::vfun_utils::release(vertexShaderBlob);
     
     
        // Create the constant buffer
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER_DX11);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            unsigned  long long  CreateBufferfun =reinterpret_cast<unsigned long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 3));
            utils::memory::mem_copy(buffer_ptr, &desc, sizeof(D3D11_BUFFER_DESC));
            utils::user_call::call(
                CreateBufferfun,
                reinterpret_cast<unsigned long long>(bd->pd3dDevice),
                reinterpret_cast<unsigned long long>(buffer_ptr),
                0,
                reinterpret_cast<unsigned long long>(&bd->pVertexConstantBuffer)
            );
          
            
   
        }
    }

    // Create the pixel shader
    {
        static const char* pixelShader =
            "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";


		// 1. 拷贝 vertexShader 源码
		size_t shader_len = strlen(pixelShader) + 1;
		memcpy(buffer_ptr, pixelShader, shader_len);

		 
		size_t entry_offset = shader_len;
		memcpy(buffer_ptr + entry_offset, "main", 5); // 包含 '\0'
		unsigned long long pEntryPoint_addr = reinterpret_cast<unsigned long long>(buffer_ptr + entry_offset);

	 
		size_t target_offset = entry_offset + 5;
		memcpy(buffer_ptr + target_offset, "ps_4_0", 7); // 包含 '\0'
		unsigned long long pTarget_addr = reinterpret_cast<unsigned long long>(buffer_ptr + target_offset);

        utils::memory::mem_zero(blob_ptr, 0x1000);

		auto ret_ptr = utils::user_call::call11(
			utils::dwm_draw::g_d3dcompile_fun,     // 函数地址
			reinterpret_cast<unsigned long long>( buffer_ptr ), // LPCVOID pSrcData
			shader_len,                             // SIZE_T SrcDataSize
			0,                                      // LPCSTR pSourceName
			0,                                      // const D3D_SHADER_MACRO* pDefines
			0,                                      // ID3DInclude* pInclude
			pEntryPoint_addr,                                // pEntryPoint
			pTarget_addr,                                    // pTarget
			0,                                      // UINT Flags1
			0,                                      // UINT Flags2
			reinterpret_cast<unsigned long long>(blob_ptr), // 输出 ID3DBlob* 存放地址
			0);

		HRESULT hr = *reinterpret_cast<HRESULT*>(ret_ptr);
		if (FAILED(hr))
			return false;

        PVOID pixelShaderBlob = *(PVOID*)blob_ptr;





		unsigned long long  GetBufferPointerfun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(pixelShaderBlob, 3));

		unsigned  long long GetBufferSizefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(pixelShaderBlob, 4));

		ret_ptr = utils::user_call::call(GetBufferPointerfun, reinterpret_cast<unsigned  long long>(pixelShaderBlob), 0, 0, 0);
		unsigned  long long BufferPointer = *reinterpret_cast<PULONG64>(ret_ptr);

		ret_ptr = utils::user_call::call(GetBufferSizefun, reinterpret_cast<unsigned  long long>(pixelShaderBlob), 0, 0, 0);
		unsigned  long long BufferSize = *reinterpret_cast<PULONG64>(ret_ptr);

        unsigned long long    CreatePixelShaderfun = reinterpret_cast<unsigned long long> ( utils::vfun_utils::get_vfunc(bd->pd3dDevice, 15));
       

        utils::user_call::call6(
            CreatePixelShaderfun,
            reinterpret_cast<unsigned long long>  (bd->pd3dDevice),
            BufferPointer,
            BufferSize,
            0,
            reinterpret_cast<unsigned long long>(&bd->pPixelShader),
            0
        );
		  hr = *reinterpret_cast<HRESULT*>(ret_ptr);
        if (FAILED(hr))
        {
            utils::vfun_utils::release(pixelShaderBlob);
            return false;

        }

      
 

    }

    // Create the blending setup
    {
        D3D11_BLEND_DESC desc;
        RtlZeroMemory(&desc, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        utils::memory::mem_copy(buffer_ptr, &desc, sizeof(D3D11_BLEND_DESC));
        auto  CreateBlendStatefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 20));
        utils::user_call::call(
            CreateBlendStatefun,
            reinterpret_cast<unsigned  long long>(bd->pd3dDevice),
            reinterpret_cast<unsigned  long long>(buffer_ptr),
            reinterpret_cast<unsigned  long long>(&bd->pBlendState),
            0
        );
        
     
       
    }

    // Create the rasterizer state
    {
        D3D11_RASTERIZER_DESC desc;
        RtlZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = true;
        desc.DepthClipEnable = true;
        auto  CreateRasterizerStatefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 22));

        utils::memory::mem_copy(buffer_ptr, &desc, sizeof(D3D11_RASTERIZER_DESC));
		utils::user_call::call(
            CreateRasterizerStatefun,
			reinterpret_cast<unsigned  long long>(bd->pd3dDevice),
			reinterpret_cast<unsigned  long long>(buffer_ptr),
			reinterpret_cast<unsigned  long long>(&bd->pRasterizerState),
			0
		);
     
    
        
    }

    // Create depth-stencil State
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        RtlZeroMemory(&desc, sizeof(desc));
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;
        auto  CreateDepthStencilStatefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 21));
		utils::memory::mem_copy(buffer_ptr, &desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		utils::user_call::call(
            CreateDepthStencilStatefun,
			reinterpret_cast<unsigned  long long>(bd->pd3dDevice),
			reinterpret_cast<unsigned  long long>(buffer_ptr),
			reinterpret_cast<unsigned  long long>(&bd->pDepthStencilState),
			0
		);


		 
      
    }

    // Create texture sampler
    // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
    {
        D3D11_SAMPLER_DESC desc;
        RtlZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
		auto   CreateSamplerStatefun = reinterpret_cast<unsigned  long long> (utils::vfun_utils::get_vfunc(bd->pd3dDevice, 23));
		utils::memory::mem_copy(buffer_ptr, &desc, sizeof(D3D11_SAMPLER_DESC));
		utils::user_call::call(
            CreateSamplerStatefun,
			reinterpret_cast<unsigned  long long>(bd->pd3dDevice),
			reinterpret_cast<unsigned  long long>(buffer_ptr),
			reinterpret_cast<unsigned  long long>(&bd->pFontSampler),
			0
		);

       
	 
    }

    return true;
}

void    ImGui_ImplDX11_InvalidateDeviceObjects()
{
    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    if (!bd->pd3dDevice)
        return;

    // Destroy all textures
    for (ImTextureData* tex : ImGui::GetPlatformIO().Textures)
        if (tex->RefCount == 1)
            ImGui_ImplDX11_DestroyTexture(tex);

	if (bd->pFontSampler) { utils::vfun_utils::release(bd->pFontSampler); bd->pFontSampler = nullptr; }
	if (bd->pIB) { utils::vfun_utils::release(bd->pIB); bd->pIB = nullptr; }
	if (bd->pVB) { utils::vfun_utils::release(bd->pVB); bd->pVB = nullptr; }
	if (bd->pBlendState) { utils::vfun_utils::release(bd->pBlendState); bd->pBlendState = nullptr; }
	if (bd->pDepthStencilState) { utils::vfun_utils::release(bd->pDepthStencilState); bd->pDepthStencilState = nullptr; }
	if (bd->pRasterizerState) { utils::vfun_utils::release(bd->pRasterizerState); bd->pRasterizerState = nullptr; }
	if (bd->pPixelShader) { utils::vfun_utils::release(bd->pPixelShader); bd->pPixelShader = nullptr; }
	if (bd->pVertexConstantBuffer) { utils::vfun_utils::release(bd->pVertexConstantBuffer); bd->pVertexConstantBuffer = nullptr; }
	if (bd->pInputLayout) { utils::vfun_utils::release(bd->pInputLayout); bd->pInputLayout = nullptr; }
	if (bd->pVertexShader) { utils::vfun_utils::release(bd->pVertexShader); bd->pVertexShader = nullptr; }
}

bool    ImGui_ImplDX11_Init(PVOID device, PVOID  device_context)
{
    ImGuiIO& io = ImGui::GetIO();
     IMGUI_CHECKVERSION();
     IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_ImplDX11_Data* bd = IM_NEW(ImGui_ImplDX11_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_dx11";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;   // We can honor ImGuiPlatformIO::Textures[] requests during render.

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_TextureMaxWidth = platform_io.Renderer_TextureMaxHeight = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;



   

	static const IID IID_IDXGIDevice =
	{ 0x54EC77FA, 0x1377, 0x44E6, { 0x8C, 0x32, 0x88, 0xFD, 0x5F, 0x44, 0xC8, 0x4C } };
  
	static const IID IID_IDXGIAdapter =
	{ 0x2411E7E1, 0x12AC, 0x4CCF, { 0xBD, 0x14, 0x97, 0x98, 0xE8, 0x53, 0x4D, 0xC0 } };
	static const IID IID_IDXGIFactory =
	{ 0x7B7166EC, 0x21C7, 0x44AE, { 0xB2, 0x1A, 0xC9, 0xAE, 0x32, 0x1A, 0xE3, 0x69 } };

	// Get factory from device
	PVOID pDXGIDevice = nullptr;
	PVOID pDXGIAdapter = nullptr;
	PVOID pFactory = nullptr;


	// 1. 获取 IDXGIDevice
	utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, (PVOID)&IID_IDXGIDevice, sizeof(IID_IDXGIDevice));
	auto ret_ptr = utils::user_call::call(
		(unsigned long long)utils::vfun_utils::get_vfunc(device, 0), // QueryInterface
		(unsigned long long)device,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIDevice),
		0
	);
	HRESULT hr = *reinterpret_cast<HRESULT*>(ret_ptr);
	if (FAILED(hr))
		return false;
	pDXGIDevice = *(PVOID*)(utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIDevice));

	// 2. 从 IDXGIDevice 获取 IDXGIAdapter
	utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, (PVOID)&IID_IDXGIAdapter, sizeof(IID_IDXGIAdapter));
	ret_ptr = utils::user_call::call(
		(unsigned long long)utils::vfun_utils::get_vfunc(pDXGIDevice,6), // GetParent
		(unsigned long long)pDXGIDevice,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIAdapter),
		0
	);
	hr = *reinterpret_cast<HRESULT*>(ret_ptr);
	if (FAILED(hr))
		return false;
	pDXGIAdapter = *(PVOID*)(utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIAdapter));

	// 3. 从 IDXGIAdapter 获取 IDXGIFactory
	utils::memory::mem_copy((PVOID)utils::dwm_draw::g_imgui_buffer, (PVOID)&IID_IDXGIFactory, sizeof(IID_IDXGIFactory));
	ret_ptr = utils::user_call::call(
		(unsigned long long)utils::vfun_utils::get_vfunc(pDXGIAdapter, 6), // GetParent
		(unsigned long long)pDXGIAdapter,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer,
		(unsigned long long)utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIFactory),
		0
	);
	hr = *reinterpret_cast<HRESULT*>(ret_ptr);
	if (FAILED(hr))
		return false;
	pFactory = *(PVOID*)(utils::dwm_draw::g_imgui_buffer + sizeof(IID_IDXGIFactory));

	// 存到 bd
	bd->pd3dDevice = device;
	bd->pd3dDeviceContext = device_context;
	bd->pFactory = pFactory;

	// AddRef
	utils::vfun_utils::add_ref(device);
	utils::vfun_utils::add_ref(device_context);

	// Release 中间对象
	if (pDXGIDevice) utils::vfun_utils::release(pDXGIDevice);
	if (pDXGIAdapter) utils::vfun_utils::release(pDXGIAdapter);

    return true;
}
//
//void ImGui_ImplDX11_Shutdown()
//{
//    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
//    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
//    ImGuiIO& io = ImGui::GetIO();
//
//    ImGui_ImplDX11_InvalidateDeviceObjects();
//    if (bd->pFactory)             { bd->pFactory->Release(); }
//    if (bd->pd3dDevice)           { bd->pd3dDevice->Release(); }
//    if (bd->pd3dDeviceContext)    { bd->pd3dDeviceContext->Release(); }
//    io.BackendRendererName = nullptr;
//    io.BackendRendererUserData = nullptr;
//    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures);
//    IM_DELETE(bd);
//}
//
void ImGui_ImplDX11_NewFrame()
{
	ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
	IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplDX11_Init()?");

	if (!bd->pVertexShader)
		if (!ImGui_ImplDX11_CreateDeviceObjects())
			IM_ASSERT(0 && "ImGui_ImplDX11_CreateDeviceObjects() failed!");
}
//
////-----------------------------------------------------------------------------
//
//#endif // #ifndef IMGUI_DISABLE
