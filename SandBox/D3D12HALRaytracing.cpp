#include <DXRenderer.h>
#include <D3D12HAL.h>

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