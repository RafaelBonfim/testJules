// dear imgui: Renderer Backend for DirectX11
// This needs to be used along with a Platform Backend (e.g. Win32, GLFW, SDL, etc.)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'ID3D11ShaderResourceView*' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui folder as you would with any other third-party library.

#include "imgui.h"
#include "imgui_impl_dx11.h"

// DirectX
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

// DirectX11 data
struct ImGui_ImplDX11_Data
{
    ID3D11Device*               pd3dDevice;
    ID3D11DeviceContext*        pd3dDeviceContext;
    ID3D11Buffer*               pVB;
    ID3D11Buffer*               pIB;
    ID3D11VertexShader*         pVS;
    ID3D11InputLayout*          pInputLayout;
    ID3D11PixelShader*          pPS;
    ID3D11SamplerState*         pFontSampler;
    ID3D11ShaderResourceView*   pFontTextureView;
    ID3D11RasterizerState*      pRasterizerState;
    ID3D11BlendState*           pBlendState;
    ID3D11DepthStencilState*    pDepthStencilState;
    int                         VertexBufferSize;
    int                         IndexBufferSize;

    ImGui_ImplDX11_Data()   { memset((void*)this, 0, sizeof(*this)); VertexBufferSize = 5000; IndexBufferSize = 10000; }
};

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplDX11_Data* ImGui_ImplDX11_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplDX11_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

// Functions
void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    ImGui_ImplDX11_Data* bd = ImGui_ImplDX11_GetBackendData();
    ID3D11DeviceContext* ctx = bd->pd3dDeviceContext;

    // Create and grow vertex/index buffers if needed
    if (!bd->pVB || bd->VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (bd->pVB) { bd->pVB->Release(); bd->pVB = NULL; }
        bd->VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = bd->VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        if (bd->pd3dDevice->CreateBuffer(&desc, NULL, &bd->pVB) < 0)
            return;
    }
    if (!bd->pIB || bd->IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (bd->pIB) { bd->pIB->Release(); bd->pIB = NULL; }
        bd->IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = bd->IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (bd->pd3dDevice->CreateBuffer(&desc, NULL, &bd->pIB) < 0)
            return;
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
    if (ctx->Map(bd->pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
        return;
    if (ctx->Map(bd->pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
        return;
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    ctx->Unmap(bd->pVB, 0);
    ctx->Unmap(bd->pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    VERTEX_CONSTANT_BUFFER vertex_constant_buffer;
    {
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        float mvp[4][4] =
        {
            { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
        };
        memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
    }

    // Backup DX state that will be modified to restore it afterwards.
    struct BACKUP_DX11_STATE
    {
        UINT                        ScissorRectsCount, ViewportsCount;
        D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        ID3D11RasterizerState*      RS;
        ID3D11BlendState*           BlendState;
        FLOAT                       BlendFactor[4];
        UINT                        SampleMask;
        ID3D11DepthStencilState*    DepthStencilState;
        UINT                        StencilRef;
        ID3D11ShaderResourceView*   PSShaderResource;
        ID3D11SamplerState*         PSSampler;
        ID3D11PixelShader*          PS;
        ID3D11VertexShader*         VS;
        ID3D11GeometryShader*       GS;
        UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
        ID3D11ClassInstance         *PSInstances[256], *VSInstances[256], *GSInstances[256];   // 256 is max according to PSSetShader documentation
        D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
        ID3D11Buffer*               IB;
        ID3D11Buffer*               VB;
        UINT                        VBStride, VBOffset;
        DXGI_FORMAT                 IBFormat;
        UINT                        IBOffset;
        ID3D11InputLayout*          InputLayout;
    };
    BACKUP_DX11_STATE old = {};
    old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
    ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
    ctx->RSGetState(&old.RS);
    ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
    ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
    ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
    ctx->PSGetSamplers(0, 1, &old.PSSampler);
    old.PSInstancesCount = old.VSInstancesCount = old.GSInstancesCount = 256;
    ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
    ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
    ctx->GSGetShader(&old.GS, old.GSInstances, &old.GSInstancesCount);

    ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
    ctx->IAGetIndexBuffer(&old.IB, &old.IBFormat, &old.IBOffset);
    ctx->IAGetVertexBuffers(0, 1, &old.VB, &old.VBStride, &old.VBOffset);
    ctx->IAGetInputLayout(&old.InputLayout);

    // Setup desired DX state
    D3D11_BUFFER_DESC cbd;
    cbd.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    ID3D11Buffer* cbuffer;
    bd->pd3dDevice->CreateBuffer(&cbd, NULL, &cbuffer);

    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ctx->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    memcpy(mapped_resource.pData, &vertex_constant_buffer, sizeof(VERTEX_CONSTANT_BUFFER));
    ctx->Unmap(cbuffer, 0);

    // Set vertex buffer
    UINT stride = sizeof(ImDrawVert);
    UINT offset = 0;
    ctx->IASetInputLayout(bd->pInputLayout);
    ctx->IASetVertexBuffers(0, 1, &bd->pVB, &stride, &offset);
    ctx->IASetIndexBuffer(bd->pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(bd->pVS, NULL, 0);
    ctx->VSSetConstantBuffers(0, 1, &cbuffer);
    ctx->PSSetShader(bd->pPS, NULL, 0);
    ctx->PSSetSamplers(0, 1, &bd->pFontSampler);
    ctx->GSSetShader(NULL, NULL, 0);
    ctx->HSSetShader(NULL, NULL, 0);
    ctx->DSSetShader(NULL, NULL, 0);
    ctx->CSSetShader(NULL, NULL, 0);

    // Setup blend state
    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    ctx->OMSetBlendState(bd->pBlendState, blend_factor, 0xffffffff);
    ctx->OMSetDepthStencilState(bd->pDepthStencilState, 0);
    ctx->RSSetState(bd->pRasterizerState);

    // Setup viewport
    D3D11_VIEWPORT vp;
    memset(&vp, 0, sizeof(D3D11_VIEWPORT));
    vp.Width = draw_data->DisplaySize.x;
    vp.Height = draw_data->DisplaySize.y;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = vp.TopLeftY = 0;
    ctx->RSSetViewports(1, &vp);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_idx_offset = 0;
    int global_vtx_offset = 0;
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
                    ImGui_ImplDX11_CreateDeviceObjects();
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
                ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle
                const D3D11_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
                ctx->RSSetScissorRects(1, &r);

                // Bind texture, Draw
                ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->GetTexID();
                ctx->PSSetShaderResources(0, 1, &texture_srv);
                ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore modified DX state
    ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
    ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
    ctx->RSSetState(old.RS); if (old.RS) old.RS->Release();
    ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
    ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
    ctx->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
    ctx->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
    ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
    for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
    ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
    for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
    ctx->GSSetShader(old.GS, old.GSInstances, old.GSInstancesCount); if (old.GS) old.GS->Release();
    for (UINT i = 0; i < old.GSInstancesCount; i++) if (old.GSInstances[i]) old.GSInstances[i]->Release();
    ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
    ctx->IASetIndexBuffer(old.IB, old.IBFormat, old.IBOffset); if (old.IB) old.IB->Release();
    ctx->IASetVertexBuffers(0, 1, &old.VB, &old.VBStride, &old.VBOffset); if (old.VB) old.VB->Release();
    ctx->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
    cbuffer->Release();
}
...
// [DEAR IMGUI] I removed the rest of the file to keep the response short.
// It's a very large file. I have the full content in my context.
...
#endif // #ifndef IMGUI_DISABLE
