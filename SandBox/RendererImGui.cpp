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

	VertexElement DeclDesc[] =
	{
		{ 0, 0, DECL_FMT_FLOAT2, DECL_POSITION },
		{ 0, 8, DECL_FMT_FLOAT2, DECL_TEXCOORD0 },
		{ 0, 16, DECL_FMT_COLOR, DECL_COLOR0 },
		DECL_END()
	};

	m_ImGuiVertexDeclaration = gData.Rdr->CreateVertexDecl(DeclDesc, SHADER_VS_IMGUI);
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
	PushVertexDeclaration(m_ImGuiVertexDeclaration);
	PushStreamSource(0, m_ImGuiVB, 0, sizeof(ImDrawVert));
	PushIndices(m_ImGuiIB);
	//ctx->IASetIndexBuffer(g_pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

	SetPrimitiveTopology(PRIM_TRIANGLELIST);
	PushShader(SHADER_VS_IMGUI);
	PushShader(SHADER_PS_IMGUI);

	//ctx->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
	//ctx->PSSetSamplers(0, 1, &g_pFontSampler);

	//// Setup render state
	//const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	//ctx->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
	DSSetDefault();
	RSSetDefault();

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
				DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}