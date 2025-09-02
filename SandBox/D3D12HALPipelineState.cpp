#include <DXRenderer.h>
#include <D3D12HAL.h>

void D3D12HAL::SetDepthStencilState(DepthStencilDesc& Desc)
{
	m_CurrentPSO.DepthStencilState = Desc.desc;
}

void D3D12HAL::SetRasterizerState(RasterizerDesc& Desc)
{
	m_CurrentPSO.RasterizerState = Desc.desc;
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
	D3D12SamplerState& state = m_SamplerStateCache.Get(Sampler);
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_CurrentSampler[0][Slot] = state.GetCPUDescriptorHandle();
	}
	else if (Type == SHADER_TYPE_VERTEX)
	{
		m_CurrentSampler[1][Slot] = state.GetCPUDescriptorHandle();
	}
}

void D3D12HAL::SetBlendState(BlendDesc& Blend)
{
	m_CurrentPSO.BlendState = Blend.desc;
	m_CurrentPSO.SampleMask = 0xffffffff;
}

void D3D12HAL::SetViewports(D3D12_VIEWPORT& Vp)
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
		//pixel - SRV
		{
			U32 slot = m_SRVDynamicHeap.AllocateSlot(MAX_SRVS);
			for (int i = 0; i < MAX_SRVS; i++)
			{
				if (m_CurrentSRV[0][i].ptr != 0)
				{
					m_Device->CopyDescriptorsSimple(1,
						m_SRVDynamicHeap.GetCPUSlotHandle(slot + i),
						m_CurrentSRV[0][i],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
			m_CommandList->SetGraphicsRootDescriptorTable(0, m_SRVDynamicHeap.GetGPUSlotHandle(slot));
		}
		//pixel - CBV
		{
			U32 slot = m_SRVDynamicHeap.AllocateSlot(MAX_SRVS);
			for (int i = 0; i < MAX_CBS; i++)
			{
				if (m_CurrentCBV[0][i].ptr != 0)
				{
					m_Device->CopyDescriptorsSimple(1,
						m_SRVDynamicHeap.GetCPUSlotHandle(slot + i),
						m_CurrentCBV[0][i],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				}
			}
			m_CommandList->SetGraphicsRootDescriptorTable(1, m_SRVDynamicHeap.GetGPUSlotHandle(slot));
		}
		//vertex - SRV
		{
			U32 slot = m_SRVDynamicHeap.AllocateSlot(MAX_SRVS);
			for (int i = 0; i < MAX_SRVS; i++)
			{
				if (m_CurrentSRV[1][i].ptr != 0)
				{
					m_Device->CopyDescriptorsSimple(1,
						m_SRVDynamicHeap.GetCPUSlotHandle(slot + i),
						m_CurrentSRV[1][i],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				}
			}
			m_CommandList->SetGraphicsRootDescriptorTable(3, m_SRVDynamicHeap.GetGPUSlotHandle(slot));
		}
		//vertex - CBS
		{
			U32 slot = m_SRVDynamicHeap.AllocateSlot(MAX_SRVS);
			for (int i = 0; i < MAX_CBS; i++)
			{
				if (m_CurrentCBV[1][i].ptr != 0)
				{
					m_Device->CopyDescriptorsSimple(1,
						m_SRVDynamicHeap.GetCPUSlotHandle(slot + i),
						m_CurrentCBV[1][i],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
			m_CommandList->SetGraphicsRootDescriptorTable(4, m_SRVDynamicHeap.GetGPUSlotHandle(slot));
		}
	}

	{
		U32 start = m_SamplerDynamicHeap.AllocateSlot(MAX_SAMPLERS * 2);
		U32 offset = start;

		//Sampler
		for (int i = 0; i < MAX_SAMPLERS; i++, offset++)
		{
			if (m_CurrentSampler[0][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SamplerDynamicHeap.GetCPUSlotHandle(offset),
					m_CurrentSampler[0][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			}
			if (m_CurrentSampler[1][i].ptr != 0)
			{
				m_Device->CopyDescriptorsSimple(1,
					m_SamplerDynamicHeap.GetCPUSlotHandle(offset+MAX_SAMPLERS),
					m_CurrentSampler[1][i],
					D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			}
		}

		offset = start + 0;
		m_CommandList->SetGraphicsRootDescriptorTable(2, m_SamplerDynamicHeap.GetGPUSlotHandle(offset)); //Pixel Samplers
		offset = start + MAX_SAMPLERS;
		m_CommandList->SetGraphicsRootDescriptorTable(5, m_SamplerDynamicHeap.GetGPUSlotHandle(offset)); //Vertex Samplers		
	}

	ID3D12PipelineState* PSO = nullptr;
	m_CurrentPSO.pRootSignature = m_RootSignature.Get();
	GetDevice()->CreateGraphicsPipelineState(&m_CurrentPSO, IID_PPV_ARGS(&PSO));
	PSO->SetName(L"test");
	m_CommandList->SetPipelineState(PSO);
	m_CommandList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}
