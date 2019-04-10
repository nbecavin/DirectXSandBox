#pragma once
#ifndef __D3D12_SAMPLER_STATE_HH__
#define __D3D12_SAMPLER_STATE_HH__

class D3D12SamplerState
{
public:
	void Create(const SamplerDesc& Desc);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() {
		return m_CpuBase;
	}

	const SamplerDesc& GetDesc() const { return desc; }

private:
	SamplerDesc desc;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuBase;
};

class D3D12SamplerStateCache
{
public:
	D3D12SamplerState& Get(const SamplerDesc& Desc)
	{
		for (auto& it : m_SamplerStateDA)
		{
			if (it.GetDesc() == Desc)
			{
				return it;
			}	
		}
		D3D12SamplerState it;
		it.Create(Desc);
		m_SamplerStateDA.push_back(it);
		return it;
	}

private:
	std::vector<D3D12SamplerState> m_SamplerStateDA;
};

#endif //__D3D12_SAMPLER_STATE_HH__
