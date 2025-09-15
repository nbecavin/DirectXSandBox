#include <DXRenderer.h>
#include <D3D12HAL.h>

#include <functional>
#include <unordered_map>
#include <cstring>

// ============ Helpers generiques ============

inline void hash_combine(std::size_t& seed, std::size_t h) {
	seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

inline std::size_t hash_memory(const void* data, size_t size) {
	const unsigned char* p = static_cast<const unsigned char*>(data);
	std::size_t h = 1469598103934665603ull; // FNV offset
	for (size_t i = 0; i < size; ++i) {
		h ^= static_cast<std::size_t>(p[i]);
		h *= 1099511628211ull; // FNV prime
	}
	return h;
}

// Hash d'une structure trivially copyable (ex: D3D12_RASTERIZER_DESC)
template <typename T>
inline std::size_t hash_struct(const T& s) {
	return hash_memory(&s, sizeof(T));
}

// Comparaison memoire brute (OK pour structs sans pointeurs)
template <typename T>
inline bool equal_struct(const T& a, const T& b) {
	return std::memcmp(&a, &b, sizeof(T)) == 0;
}

// ============ Hash & Equal pour PSO ============

struct PipelineDescHash {
	std::size_t operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) const noexcept {
		std::size_t seed = 0;

		// RootSignature -> on hash juste le pointeur (optionnel : GUID unique ?)
		hash_combine(seed, std::hash<void*>{}(desc.pRootSignature));

		// Shaders (contenu, pas pointeur)
		if (desc.VS.pShaderBytecode)
			hash_combine(seed, hash_memory(desc.VS.pShaderBytecode, desc.VS.BytecodeLength));
		if (desc.PS.pShaderBytecode)
			hash_combine(seed, hash_memory(desc.PS.pShaderBytecode, desc.PS.BytecodeLength));
		if (desc.DS.pShaderBytecode)
			hash_combine(seed, hash_memory(desc.DS.pShaderBytecode, desc.DS.BytecodeLength));
		if (desc.HS.pShaderBytecode)
			hash_combine(seed, hash_memory(desc.HS.pShaderBytecode, desc.HS.BytecodeLength));
		if (desc.GS.pShaderBytecode)
			hash_combine(seed, hash_memory(desc.GS.pShaderBytecode, desc.GS.BytecodeLength));

		// Fixed function state
		hash_combine(seed, hash_struct(desc.BlendState));
		hash_combine(seed, hash_struct(desc.RasterizerState));
		hash_combine(seed, hash_struct(desc.DepthStencilState));
		hash_combine(seed, hash_struct(desc.SampleDesc));

		// Input layout
		for (UINT i = 0; i < desc.InputLayout.NumElements; i++) {
			hash_combine(seed, hash_struct(desc.InputLayout.pInputElementDescs[i]));
		}

		// IA / RT formats
		hash_combine(seed, std::hash<D3D12_PRIMITIVE_TOPOLOGY_TYPE>{}(desc.PrimitiveTopologyType));
		for (UINT i = 0; i < desc.NumRenderTargets; i++) {
			hash_combine(seed, std::hash<DXGI_FORMAT>{}(desc.RTVFormats[i]));
		}
		hash_combine(seed, std::hash<DXGI_FORMAT>{}(desc.DSVFormat));

		// Misc
		hash_combine(seed, std::hash<UINT>{}(desc.SampleMask));
		hash_combine(seed, std::hash<UINT>{}(desc.NodeMask));
		//hash_combine(seed, std::hash<D3D12_CACHED_PIPELINE_STATE>{}(desc.CachedPSO));
		hash_combine(seed, std::hash<D3D12_PIPELINE_STATE_FLAGS>{}(desc.Flags));

		return seed;
	}
};

struct PipelineDescEqual {
	bool operator()(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& a,
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& b) const noexcept {
		// RootSignature (même pointeur attendu ici)
		if (a.pRootSignature != b.pRootSignature) return false;

		// Shaders
		auto cmp_shader = [](auto& A, auto& B) {
			if (A.BytecodeLength != B.BytecodeLength) return false;
			if (A.BytecodeLength == 0) return true;
			return std::memcmp(A.pShaderBytecode, B.pShaderBytecode, A.BytecodeLength) == 0;
			};
		if (!cmp_shader(a.VS, b.VS)) return false;
		if (!cmp_shader(a.PS, b.PS)) return false;
		if (!cmp_shader(a.DS, b.DS)) return false;
		if (!cmp_shader(a.HS, b.HS)) return false;
		if (!cmp_shader(a.GS, b.GS)) return false;

		// Fixed function
		if (!equal_struct(a.BlendState, b.BlendState)) return false;
		if (!equal_struct(a.RasterizerState, b.RasterizerState)) return false;
		if (!equal_struct(a.DepthStencilState, b.DepthStencilState)) return false;
		if (!equal_struct(a.SampleDesc, b.SampleDesc)) return false;

		// InputLayout
		if (a.InputLayout.NumElements != b.InputLayout.NumElements) return false;
		for (UINT i = 0; i < a.InputLayout.NumElements; i++) {
			if (!equal_struct(a.InputLayout.pInputElementDescs[i], b.InputLayout.pInputElementDescs[i]))
				return false;
		}

		// IA / RT
		if (a.PrimitiveTopologyType != b.PrimitiveTopologyType) return false;
		if (a.NumRenderTargets != b.NumRenderTargets) return false;
		for (UINT i = 0; i < a.NumRenderTargets; i++) {
			if (a.RTVFormats[i] != b.RTVFormats[i]) return false;
		}
		if (a.DSVFormat != b.DSVFormat) return false;

		// Misc
		if (a.SampleMask != b.SampleMask) return false;
		if (a.NodeMask != b.NodeMask) return false;
		if (a.Flags != b.Flags) return false;

		return true;
	}
};

// ============ Alias pour le cache ============
using PipelineCache = std::unordered_map<
	D3D12_GRAPHICS_PIPELINE_STATE_DESC,
	ComPtr<ID3D12PipelineState>,
	PipelineDescHash,
	PipelineDescEqual
>;
std::vector<ComPtr<ID3D12PipelineState>> m_PSOStore;

PipelineCache m_GraphicPSOCache;

ID3D12PipelineState* GetOrCreatePipeline(ID3D12Device* device,
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
	auto it = m_GraphicPSOCache.find(desc);
	if (it != m_GraphicPSOCache.end()) {
		return it->second.Get(); // déjà en cache
	}

	ID3D12PipelineState* pipeline = nullptr;
	HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
	if (SUCCEEDED(hr)) {
		m_GraphicPSOCache[desc] = pipeline;
	}
	return pipeline;
}

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

void D3D12HAL::SetScissorRect(U32 left, U32 right, U32 top, U32 bottom)
{
	D3D12_RECT rect;
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;
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

	m_CurrentPSO.pRootSignature = m_RootSignature.Get();

	ComPtr<ID3D12PipelineState> PSO;
	PSO = GetOrCreatePipeline(GetDevice(), m_CurrentPSO);
	m_CommandList->SetPipelineState(PSO.Get());
	m_CommandList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}
