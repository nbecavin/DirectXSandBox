#include <Renderer.h>
#include <Bitmap.h>
#include <WinMain.h>
using namespace sys;

void Renderer::InitImGUI()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	//ImGui_ImplWin32_Init(hwnd);
	//ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	//io.ImeWindowHandle = sys::pc::hWnd;

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
	/*
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.SetKeyEventNativeData(ImGuiKey_Home, VK_HOME);
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Space] = VK_SPACE;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';
	*/

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
	//bool showDemo = true;
	//ImGui::ShowDemoWindow(&showDemo);

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
	//	D3D12_MAPPED_SUBRESOURCE mapped_resource;
	//	if (ctx->Map(g_pVertexConstantBuffer, 0, D3D12_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
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
	SamplerDesc fontSampler;
	fontSampler.desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	fontSampler.desc.AddressU = fontSampler.desc.AddressV = fontSampler.desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	fontSampler.desc.MaxAnisotropy = 1;
	fontSampler.desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	fontSampler.desc.MaxLOD = D3D12_FLOAT32_MAX;
	SetSampler(0, SHADER_TYPE_PIXEL, fontSampler);

	//// Setup render state
	BlendDesc blend;
	blend.desc.RenderTarget[0].BlendEnable = TRUE;
	blend.desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blend.desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	SetBlendState(blend);
	
	DepthStencilDesc ds;
	ds.desc.DepthEnable = FALSE;
	SetDepthStencilState(ds);
	
	RasterizerDesc rs;
	rs.desc.CullMode = D3D12_CULL_MODE_NONE;
	SetRasterizerState(rs);

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
				const D3D12_RECT r = { (LONG)(pcmd->ClipRect.x - pos.x), (LONG)(pcmd->ClipRect.y - pos.y), (LONG)(pcmd->ClipRect.z - pos.x), (LONG)(pcmd->ClipRect.w - pos.y) };
				//ctx->RSSetScissorRects(1, &r);

				// Bind texture, Draw
				Bitmap* bmap = reinterpret_cast<Bitmap*>(pcmd->GetTexID());
				SetShaderResource(0, SHADER_TYPE_PIXEL, bmap);
				DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}
