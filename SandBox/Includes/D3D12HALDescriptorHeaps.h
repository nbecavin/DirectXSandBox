#pragma once
#ifndef __D3D12_DESCRIPTORHEAPS_HH__
#define __D3D12_DESCRIPTORHEAPS_HH__

class D3D12DescriptorHeap
{
public:
	void Init(D3D12_DESCRIPTOR_HEAP_DESC& heapDesc, const char * name);

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSlotHandle(U32 _Slot) const { return{ m_CPUBase.ptr + _Slot * m_DescriptorSize }; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSlotHandle(U32 _Slot) const { return{ m_GPUBase.ptr + _Slot * m_DescriptorSize }; }

	void SetSlot(U32 _Slot)
	{
		m_CurrentDescriptorOffset = _Slot;
	}

	U32 AllocateSlot(U32 _Num)
	{
		U32 slot;

		// Recycle ...
		U32 endDescriptor = m_CurrentDescriptorOffset + _Num;
		if (endDescriptor >= m_Desc.NumDescriptors)
		{
			slot = 0;
			m_CurrentDescriptorOffset = _Num;
		}
		else
		{
			slot = m_CurrentDescriptorOffset;
			m_CurrentDescriptorOffset += _Num;
		}

		return slot;
	}

	U32 GetCurrentSlot() { return m_CurrentDescriptorOffset; }

	UINT GetDescriptorSize() const { return m_DescriptorSize; }
	ID3D12DescriptorHeap* Get() { return m_Heap.Get(); }
	D3D12_DESCRIPTOR_HEAP_DESC GetDesc() const { return m_Desc; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
	D3D12_DESCRIPTOR_HEAP_DESC m_Desc;

	D3D12_CPU_DESCRIPTOR_HANDLE	m_CPUBase = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_GPUBase = {};

	UINT	m_DescriptorSize = 0;
	UINT	m_CurrentDescriptorOffset = 0;
};

#endif //__D3D12_DESCRIPTORHEAPS_HH__
