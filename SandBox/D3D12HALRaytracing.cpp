#include <D3D12HAL.h>
#include <RaytracingScene.h>

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

void D3D12HAL::BuildTLAS()
{
	// Alloc base buffer
	//RaytracingScene* pScene = GetRaytracingScene();

	//for(int i=0; i<pScene.
	/*{
		D3D12_RAYTRACING_INSTANCE_DESC
		{
			FLOAT Transform[3][4];
			UINT InstanceID : 24;
			UINT InstanceMask : 8;
			UINT InstanceContributionToHitGroupIndex : 24;
			UINT Flags : 8;
			D3D12_GPU_VIRTUAL_ADDRESS AccelerationStructure;
		};

	}*/

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC Desc = {};
	Desc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	Desc.Inputs.InstanceDescs = 0;
	Desc.Inputs.NumDescs = 0;
	Desc.DestAccelerationStructureData = 0;
	Desc.ScratchAccelerationStructureData = 0;

	//m_CommandList->BuildRaytracingAccelerationStructure(&Desc, 0, nullptr);
}
