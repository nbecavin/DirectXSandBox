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

	//// Create and grow vertex/index buffers if needed
	if (!m_ImGuiVB || (m_ImGuiVB->GetSize() / sizeof(ImDrawVert)) < drawData->TotalVtxCount)
	{
		//if (m_ImGuiVB) DeleteVertexBuffer(m_ImGuiVB);
		U32 byteWidth = (drawData->TotalVtxCount + 5000) * sizeof(ImDrawVert);
		m_ImGuiVB = CreateVertexBuffer(byteWidth, 0, nullptr);
	}
	if (!m_ImGuiIB || (m_ImGuiIB->GetSize() / 2) < drawData->TotalIdxCount)
	{
		//if (m_ImGuiIB) DeleteIndexBuffer(m_ImGuiIB);
		U32 byteWidth = (drawData->TotalIdxCount + 10000) * 2;
		m_ImGuiIB = CreateIndexBuffer(byteWidth, 0, FMT_IDX_16, nullptr);
	}

	void* vtxResource;
	void* idxResource;
	m_ImGuiVB->Lock(0, 0, &vtxResource, Buffer::WriteDiscard);
	m_ImGuiIB->Lock(0, 0, &idxResource, Buffer::WriteDiscard);
	ImDrawVert* vtx_dst = (ImDrawVert*)vtxResource;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idxResource;
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	m_ImGuiVB->Unlock();
	m_ImGuiIB->Unlock();

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
