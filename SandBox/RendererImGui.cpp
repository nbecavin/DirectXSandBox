#include <Renderer.h>
#include <Bitmap.h>
using namespace sys;

void Renderer::InitImGUI()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	//ImGui_ImplWin32_Init(hwnd);
	//ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Setup style
	ImGui::StyleColorsDark();

	// Build and load the texture atlas into a texture
	// (In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
	int width, height, bpp;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp);

	// At this point you've got the texture data and you need to upload that your your graphic system:
	// After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
	// This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ below for details about ImTextureID.
	//MyTexture* texture = MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)

	Bitmap* bmap = new Bitmap();
	bmap->SetFormat(BM_FORMAT_ABGR8888);
	bmap->SetSize(width, height);
	bmap->SetType(BM_TYPE_2D);
	bmap->SetData(pixels, width*height*bpp);	
	CreateTexture(bmap);
	io.Fonts->TexID = (void*)bmap;
}

void Renderer::DrawImGUI()
{
	bool showDemo = true;
	ImGui::ShowDemoWindow(&showDemo);
	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData->Valid)
	{
		OutputDebugString("lkml");
	}

	//ID3D11DeviceContext* ctx = g_pd3dDeviceContext;

	//// Create and grow vertex/index buffers if needed
	//if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	//{
	//	if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
	//	g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
	//	D3D11_BUFFER_DESC desc;
	//	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	//	desc.Usage = D3D11_USAGE_DYNAMIC;
	//	desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
	//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	desc.MiscFlags = 0;
	//	if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
	//		return;
	//}
	//if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	//{
	//	if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
	//	g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
	//	D3D11_BUFFER_DESC desc;
	//	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	//	desc.Usage = D3D11_USAGE_DYNAMIC;
	//	desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
	//	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pIB) < 0)
	//		return;
	//}

	//// Copy and convert all vertices into a single contiguous buffer
	//D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	//if (ctx->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
	//	return;
	//if (ctx->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
	//	return;
	//ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	//ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
	//for (int n = 0; n < draw_data->CmdListsCount; n++)
	//{
	//	const ImDrawList* cmd_list = draw_data->CmdLists[n];
	//	memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
	//	memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
	//	vtx_dst += cmd_list->VtxBuffer.Size;
	//	idx_dst += cmd_list->IdxBuffer.Size;
	//}
	//ctx->Unmap(g_pVB, 0);
	//ctx->Unmap(g_pIB, 0);

	//// Setup orthographic projection matrix into our constant buffer
	//// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). 
	//{
	//	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	//	if (ctx->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
	//		return;
	//	VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
	//	float L = draw_data->DisplayPos.x;
	//	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	//	float T = draw_data->DisplayPos.y;
	//	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	//	float mvp[4][4] =
	//	{
	//		{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
	//		{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
	//		{ 0.0f,         0.0f,           0.5f,       0.0f },
	//		{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
	//	};
	//	memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
	//	ctx->Unmap(g_pVertexConstantBuffer, 0);
	//}

	//// Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
	//struct BACKUP_DX11_STATE
	//{
	//	UINT                        ScissorRectsCount, ViewportsCount;
	//	D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	//	D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	//	ID3D11RasterizerState*      RS;
	//	ID3D11BlendState*           BlendState;
	//	FLOAT                       BlendFactor[4];
	//	UINT                        SampleMask;
	//	UINT                        StencilRef;
	//	ID3D11DepthStencilState*    DepthStencilState;
	//	ID3D11ShaderResourceView*   PSShaderResource;
	//	ID3D11SamplerState*         PSSampler;
	//	ID3D11PixelShader*          PS;
	//	ID3D11VertexShader*         VS;
	//	UINT                        PSInstancesCount, VSInstancesCount;
	//	ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
	//	D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
	//	ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
	//	UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
	//	DXGI_FORMAT                 IndexBufferFormat;
	//	ID3D11InputLayout*          InputLayout;
	//};
	//BACKUP_DX11_STATE old;
	//old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	//ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
	//ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
	//ctx->RSGetState(&old.RS);
	//ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
	//ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
	//ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
	//ctx->PSGetSamplers(0, 1, &old.PSSampler);
	//old.PSInstancesCount = old.VSInstancesCount = 256;
	//ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
	//ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
	//ctx->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
	//ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
	//ctx->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
	//ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
	//ctx->IAGetInputLayout(&old.InputLayout);

	//// Setup viewport
	//D3D11_VIEWPORT vp;
	//memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	//vp.Width = draw_data->DisplaySize.x;
	//vp.Height = draw_data->DisplaySize.y;
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//vp.TopLeftX = vp.TopLeftY = 0;
	//ctx->RSSetViewports(1, &vp);

	//// Bind shader and vertex buffers
	//unsigned int stride = sizeof(ImDrawVert);
	//unsigned int offset = 0;
	//ctx->IASetInputLayout(g_pInputLayout);
	//ctx->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);
	//ctx->IASetIndexBuffer(g_pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	//ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ctx->VSSetShader(g_pVertexShader, NULL, 0);
	//ctx->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
	//ctx->PSSetShader(g_pPixelShader, NULL, 0);
	//ctx->PSSetSamplers(0, 1, &g_pFontSampler);

	//// Setup render state
	//const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	//ctx->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
	//ctx->OMSetDepthStencilState(g_pDepthStencilState, 0);
	//ctx->RSSetState(g_pRasterizerState);

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	ImVec2 pos = drawData->DisplayPos;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				// User callback (registered via ImDrawList::AddCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Apply scissor/clipping rectangle
				const D3D11_RECT r = { (LONG)(pcmd->ClipRect.x - pos.x), (LONG)(pcmd->ClipRect.y - pos.y), (LONG)(pcmd->ClipRect.z - pos.x), (LONG)(pcmd->ClipRect.w - pos.y) };
				//ctx->RSSetScissorRects(1, &r);

				// Bind texture, Draw
				Bitmap* bmap = static_cast<Bitmap*>(pcmd->TextureId);
				//ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;
				//ctx->PSSetShaderResources(0, 1, &texture_srv);
				//ctx->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

	//// Restore modified DX state
	//ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
	//ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
	//ctx->RSSetState(old.RS); if (old.RS) old.RS->Release();
	//ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
	//ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
	//ctx->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
	//ctx->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
	//ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
	//for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
	//ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
	//ctx->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
	//for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
	//ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
	//ctx->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	//ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	//ctx->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();

}
