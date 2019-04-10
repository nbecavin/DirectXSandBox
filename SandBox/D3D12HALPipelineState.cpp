#include <DXRenderer.h>
#include <D3D12HAL.h>

void D3D12HAL::SetDepthStencilState(DepthStencilDesc& Desc)
{
	D3D12_DEPTH_STENCIL_DESC ds;
	D3D12Interop::DepthStencilState(ds, Desc.desc);
	m_CurrentPSO.DepthStencilState = ds;
}

void D3D12HAL::SetRasterizerState(RasterizerDesc& Desc)
{
	D3D12_RASTERIZER_DESC rs;
	D3D12Interop::RasterizerState(rs, Desc.desc);
	rs.ForcedSampleCount = 0;
	m_CurrentPSO.RasterizerState = rs;
}

void D3D12HAL::SetShaderResource(U32 Slot, EShaderType Type, sys::TextureLink* View)
{
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_CurrentSRV[0][Slot] = View->m_D3D12SRVcpu;
	}
	else if (Type == SHADER_TYPE_VERTEX)
	{
		m_CurrentSRV[1][Slot] = View->m_D3D12SRVcpu;
	}
}

void D3D12HAL::SetConstantBuffer(U32 Slot, EShaderType Type, ConstantBuffer* CBV)
{
	D3D12ConstantBuffer* cbv = (D3D12ConstantBuffer*)CBV;
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_CurrentCBV[0][Slot] = cbv->GetCPUDescriptorHandle();
	}
	else if (Type == SHADER_TYPE_VERTEX)
	{
		m_CurrentCBV[1][Slot] = cbv->GetCPUDescriptorHandle();
	}
}

void D3D12HAL::SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler)
{
}

void D3D12HAL::SetBlendState(BlendDesc& Blend)
{
	D3D12Interop::BlendState(m_CurrentPSO.BlendState, Blend.desc);
	m_CurrentPSO.SampleMask = 0xffffffff;
}

void D3D12HAL::SetViewports(D3D11_VIEWPORT& Vp)
{
	m_CommandList->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT*>(&Vp));
	D3D12_RECT rect;
	rect.left = Vp.TopLeftX;
	rect.top = Vp.TopLeftY;
	rect.right = Vp.TopLeftX + Vp.Width;
	rect.bottom = Vp.TopLeftY + Vp.Height;
	m_CommandList->RSSetScissorRects(1, &rect);
}

void D3D12HAL::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	// Update descriptor table
	{
		U32 start = m_SRVDynamicHeap.AllocateSlot(SRV_CBV_DESCRIPTOR_TABLE_OFFSET * 2);
		U32 offset = start;

		//pixel - SRV
		for (int i = 0; i < MAX_SRVS; i++, offset++)
		{
			if (m_CurrentSRV[0][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SRVDynamicHeap.GetCPUSlotHandle(offset),
					m_CurrentSRV[0][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			}
		}
		//pixel - CBV
		for (int i = 0; i < MAX_CBS; i++, offset++)
		{
			if (m_CurrentCBV[0][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SRVDynamicHeap.GetCPUSlotHandle(offset),
					m_CurrentCBV[0][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			}
		}
		//vertex - SRV
		for (int i = 0; i < MAX_SRVS; i++, offset++)
		{
			if (m_CurrentSRV[1][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SRVDynamicHeap.GetCPUSlotHandle(offset),
					m_CurrentSRV[1][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			}
		}
		//vertex - CBS
		for (int i = 0; i < MAX_CBS; i++, offset++)
		{
			if (m_CurrentCBV[1][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SRVDynamicHeap.GetCPUSlotHandle(offset),
					m_CurrentCBV[1][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			}
		}
		
		offset = start + 0;
		m_CommandList->SetGraphicsRootDescriptorTable(0, m_SRVDynamicHeap.GetGPUSlotHandle(offset)); //Pixel SRV
		offset = start + MAX_SRVS;
		m_CommandList->SetGraphicsRootDescriptorTable(1, m_SRVDynamicHeap.GetGPUSlotHandle(offset)); //Pixel CBV
		//m_CommandList->SetGraphicsRootDescriptorTable(2, cbv->GetGPUDescriptorHandle()); //Pixel Samplers
		offset = start + SRV_CBV_DESCRIPTOR_TABLE_OFFSET;
		m_CommandList->SetGraphicsRootDescriptorTable(3, m_SRVDynamicHeap.GetGPUSlotHandle(offset)); //Vertex SRV
		offset = start + SRV_CBV_DESCRIPTOR_TABLE_OFFSET + MAX_SRVS;
		m_CommandList->SetGraphicsRootDescriptorTable(4, m_SRVDynamicHeap.GetGPUSlotHandle(offset)); //Vertex CBV
		//m_CommandList->SetGraphicsRootDescriptorTable(5, cbv->GetGPUDescriptorHandle()); //Vertex Samplers
	}

	ID3D12PipelineState* PSO;
	m_CurrentPSO.pRootSignature = m_RootSignature.Get();
	GetDevice()->CreateGraphicsPipelineState(&m_CurrentPSO, IID_PPV_ARGS(&PSO));
	PSO->SetName(L"test");
	m_CommandList->SetPipelineState(PSO);
	m_CommandList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}
