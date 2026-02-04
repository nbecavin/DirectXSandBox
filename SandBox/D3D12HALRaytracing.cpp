#include <D3D12HAL.h>
#include <RaytracingScene.h>
#include <GpuScene.h>

AccelerationStructure* D3D12HAL::CreateAccelerationStructure(U32 Size)
{
	D3D12AccelerationStructure* newResource = new D3D12AccelerationStructure(Size);

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

void D3D12HAL::BuildBLAS()
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC Desc = {};

	Desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	Desc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

	D3D12_RAYTRACING_GEOMETRY_DESC GeometryDesc[64]; //this is a maximum	
	Desc.Inputs.pGeometryDescs = GeometryDesc;

	int NbSubMesh = 0;
	for (int i = 0; i < NbSubMesh; i++)
	{
		auto& Desc = GeometryDesc[i];
		Desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		Desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		Desc.Triangles.Transform3x4 = 0;
		Desc.Triangles.IndexBuffer = 0;
		Desc.Triangles.IndexCount = 0;
		Desc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
		Desc.Triangles.VertexBuffer.StartAddress = 0;
		Desc.Triangles.VertexBuffer.StrideInBytes = 0;
		Desc.Triangles.VertexCount = 0;
		Desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	}

	Desc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO Info = {};
	m_Device->GetRaytracingAccelerationStructurePrebuildInfo(&Desc.Inputs, &Info);

	// Allocate scratch buffer
	//scene->GetOrCreateScratchBuffer(Info.ScratchDataSizeInBytes + Info.UpdateScratchDataSizeInBytes);

	// Allocate AS
	//scene->GetOrCreateTLAS(Info.ResultDataMaxSizeInBytes);
	
	Desc.DestAccelerationStructureData = 0;
	Desc.ScratchAccelerationStructureData = 0;

	m_CommandList->BuildRaytracingAccelerationStructure(&Desc, 0, nullptr);
}

void D3D12HAL::BuildTLAS()
{
	RaytracingScene* scene = gData.Scene->GetRaytracingScene();

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS Inputs = {};
	Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;	

	// Alloc base buffer
	const sys::InstanceDataDA& instances = gData.Scene->GetInstance();

	Buffer* instanceStore = scene->GetOrCreateInstanceBuffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instances.GetSize());
	ID3D12Resource* dataStore = instanceStore->AsPtr<D3D12Buffer>()->res;

	D3D12_RAYTRACING_INSTANCE_DESC* lockedData;
	dataStore->Map(0, nullptr, (void**)&lockedData);

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
		D3D12_RAYTRACING_INSTANCE_DESC& it = lockedData[i];
		ConvertMatrix(it, instance.instanceCst.WorldMatrix);
		it.InstanceID = i;
		it.InstanceMask = 0x1;
		it.InstanceContributionToHitGroupIndex = i;
		it.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		it.AccelerationStructure = 0; //BLAS
	}

	dataStore->Unmap(0, nullptr);

	Inputs.InstanceDescs = instanceStore->AsPtr<D3D12Buffer>()->res->GetGPUVirtualAddress();
	Inputs.NumDescs = instances.GetSize();

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO Info = {}; 
	m_Device->GetRaytracingAccelerationStructurePrebuildInfo(&Inputs, &Info);

	// Allocate scratch buffer
	scene->GetOrCreateScratchBuffer(Info.ScratchDataSizeInBytes + Info.UpdateScratchDataSizeInBytes);

	// Allocate AS
	scene->GetOrCreateTLAS(Info.ResultDataMaxSizeInBytes);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC Desc = {};
	Desc.Inputs = Inputs;
	Desc.DestAccelerationStructureData = scene->m_TLAS->AsPtr<D3D12AccelerationStructure>()->res->GetGPUVirtualAddress();
	Desc.ScratchAccelerationStructureData = scene->m_ScratchBuffer->AsPtr<D3D12Buffer>()->res->GetGPUVirtualAddress();

	m_CommandList->BuildRaytracingAccelerationStructure(&Desc, 0, nullptr);

	scene->Advance();
}
