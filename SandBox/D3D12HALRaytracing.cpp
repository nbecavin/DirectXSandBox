#include <D3D12HAL.h>
#include <RaytracingScene.h>
#include <GpuScene.h>

AccelerationStructure* D3D12HAL::CreateAccelerationStructure(U32 Size)
{
	D3D12AccelerationStructure* newResource = new D3D12AccelerationStructure();

	CD3DX12_HEAP_PROPERTIES Heap(D3D12_HEAP_TYPE_DEFAULT);	
	CD3DX12_RESOURCE_DESC Desc = CD3DX12_RESOURCE_DESC::Buffer(Size, D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE, 0);

	m_Device->CreateCommittedResource(&Heap, D3D12_HEAP_FLAG_CREATE_NOT_ZEROED, &Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&newResource->res));

	/*
	U32 slot = m_SrvHeap.AllocateSlot(1);
	D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor = m_SrvHeap.GetCPUSlotHandle(slot);
	CD3DX12_SHADER_RESOURCE_VIEW_DESC ViewDesc = CD3DX12_SHADER_RESOURCE_VIEW_DESC::RaytracingAccelStruct(newResource->res->GetGPUVirtualAddress());
	m_Device->CreateShaderResourceView(newResource->res, &ViewDesc, DestDescriptor);
	*/

	return newResource;
}

void D3D12HAL::BuildAccelerationStructure()
{

	//m_Device->GetRaytracingAccelerationStructurePrebuildInfo();
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC Desc = {};
	Desc.DestAccelerationStructureData;
	Desc.ScratchAccelerationStructureData;
	//Desc.SourceAccelerationStructureData;
	Desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

	m_CommandList->BuildRaytracingAccelerationStructure(&Desc, 0, nullptr);
		//_In_  UINT NumPostbuildInfoDescs,
		//_In_reads_opt_(NumPostbuildInfoDescs)  const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC* pPostbuildInfoDescs) = 0;

}

using D3D12_RAYTRACING_INSTANCE_DESC_DA = DynArray<D3D12_RAYTRACING_INSTANCE_DESC, 1024>;
static D3D12_RAYTRACING_INSTANCE_DESC_DA m_TemporaryInstanceStore;

void D3D12HAL::BuildTLAS()
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS Inputs = {};
	Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;	

	// Alloc base buffer
	const sys::InstanceDataDA& instances = gData.Scene->GetInstance();
	if(m_TemporaryInstanceStore.GetSize() < instances.GetSize())
		m_TemporaryInstanceStore.SetSize(instances.GetSize()*2); //Grow with some margin

	for (int i = 0; i < instances.GetSize(); i++)
	{
		auto ConvertMatrix = [](D3D12_RAYTRACING_INSTANCE_DESC& it, const Mat4x4& m)
			{
				it.Transform[0][0] = m.m[0][0];
				it.Transform[0][1] = m.m[0][1];
				it.Transform[0][2] = m.m[0][2];
				it.Transform[0][3] = m.m[0][3];
				it.Transform[1][0] = m.m[1][0];
				it.Transform[1][1] = m.m[1][1];
				it.Transform[1][2] = m.m[1][2];
				it.Transform[1][3] = m.m[1][3];
				it.Transform[2][0] = m.m[2][0];
				it.Transform[2][1] = m.m[2][1];
				it.Transform[2][2] = m.m[2][2];
				it.Transform[2][3] = m.m[2][3];
			};

		const sys::InstanceData& instance = instances[i];
		D3D12_RAYTRACING_INSTANCE_DESC& it = m_TemporaryInstanceStore[i];
		ConvertMatrix(it, instance.instanceCst.WorldMatrix);
		it.InstanceID = i;
		it.InstanceMask = 0x1;
		it.InstanceContributionToHitGroupIndex = i;
		it.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		it.AccelerationStructure = 0; //BLAS
	}

	Inputs.InstanceDescs = 0;
	Inputs.NumDescs = 0;//instances.GetSize();

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO Info = {}; 
	m_Device->GetRaytracingAccelerationStructurePrebuildInfo(&Inputs, &Info);

	// Allocate scratch buffer
	Buffer* scratchBuffer = CreateBuffer();

	// Allocate AS
	AccelerationStructure* AS = CreateAccelerationStructure(Info.ResultDataMaxSizeInBytes);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC Desc = {};
	Desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	Desc.Inputs.InstanceDescs = 0;
	Desc.Inputs.NumDescs = 0;
	Desc.DestAccelerationStructureData = 0;
	Desc.ScratchAccelerationStructureData = 0;

	//m_CommandList->BuildRaytracingAccelerationStructure(&Desc, 0, nullptr);
}
