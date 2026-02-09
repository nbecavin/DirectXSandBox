#include <GpuScene.h>
#include <Renderer.h>
#include <Mesh.h>

#include <../../Shaders/material_parameter.hlsli.h>

#include <GpuSceneInstance.h>
#include <RaytracingScene.h>

//
// collect drawcals

using namespace sys;

GpuScene::GpuScene()
{

}

GpuScene::~GpuScene()
{

}

void GpuScene::Init()
{
	m_RaytracingScene = std::make_unique<RaytracingScene>();
}

void GpuScene::Shut()
{
}

void GpuScene::RegisterMaterial(Material* material)
{
	U32 id = m_MaterialDA.Add(material);
}

void GpuScene::UpdateBuffers()
{
	if (m_MaterialStorage.get() == nullptr)
	{
		m_MaterialStorage = std::make_shared<Bitmap>();
		//m_MaterialStorage->
		//gData.Rdr->GetHAL()->CreateBuffer();
	}
}

void GpuScene::GatherDrawObjects()
{
	m_Instances.SetSize(0); //Empty

	auto* HAL = gData.Rdr->GetHAL();

	for (int i = 0; i < gData.m_GraphObjectDA.GetSize(); i++)
	{
		GraphObject* it = gData.m_GraphObjectDA[i];

		//NICOB - bon pour l'instant j'ai que des meshs, mais ça va chier plus tard
		{
			Mesh* mesh = (Mesh*)it;
			U32 SubSetCount = mesh->SubSetsDA.GetSize();
			for (int i = 0; i < SubSetCount; i++)
			{
				InstanceData* instance = m_Instances.Add();

				Mesh::MeshSubSet* pMesh = &mesh->SubSetsDA[i];

				instance->instanceCst.WorldMatrix = mesh->GetNode();
				instance->geometry.AS = pMesh->AS;
				instance->geometry.VB = pMesh->VB;
				instance->geometry.IB = pMesh->IB;
				instance->geometry.IndexStart = pMesh->IndexStart;
				instance->geometry.IndexCount = pMesh->IndexCount;
				instance->geometry.IndexType = pMesh->IndexType;
				instance->geometry.VertexStart = pMesh->VertexStart;
				instance->geometry.VertexStride = pMesh->Stride;
				instance->geometry.PrimType = pMesh->PrimType;
				instance->geometry.Decl = pMesh->Decl;
				if (mesh->MtlDA.GetSize())
					instance->shading.Mtl = &mesh->MtlDA[pMesh->MtlId];
				else
					instance->shading.Mtl = nullptr;
			}
		}
	}
}

void GpuScene::SetActiveMaterial(Material* Mat)
{
	if (Mat == nullptr)
		return;

	auto* HAL = gData.Rdr->GetHAL();

	Bitmap* bm = NULL;
	bm = Mat->GetBitmap(MTL_STAGE_ALBEDO);
	if (bm)
	{
		HAL->SetShaderResource(0, SHADER_TYPE_PIXEL, bm);
	}
	bm = Mat->GetBitmap(MTL_STAGE_NORMAL);
	if (bm)
	{
		HAL->SetShaderResource(1, SHADER_TYPE_PIXEL, bm);
	}
	bm = Mat->GetBitmap(MTL_STAGE_ROUGHNESS);
	if (bm)
	{
		HAL->SetShaderResource(2, SHADER_TYPE_PIXEL, bm);
	}
}

void GpuScene::DrawScene()
{
	auto* HAL = gData.Rdr->GetHAL();

	HAL->ProfileBeginEvent(0, "GpuScene Draw");

	for (int id = 0; id < m_Instances.GetSize(); id++)
	{
		InstanceData& instance = m_Instances[id];

		if( id >= m_InstanceData.GetSize())
		{
			auto* cst  = HAL->CreateConstantBuffer(sizeof(InstanceConstant));
			InstanceConstant* c;
			cst->Lock(0, 0, (void**)&c);
			c[0] = instance.instanceCst;
			cst->Unlock();
			m_InstanceData.Add(cst);
		}
		HAL->BindGraphicPipelineState(ShaderMap::BaseMeshVS, ShaderMap::BasePassPS);
		HAL->SetConstantBuffer(1, SHADER_TYPE_VERTEX, m_InstanceData[id]);

		SetActiveMaterial(instance.shading.Mtl);

		HAL->SetVertexDeclaration(instance.geometry.Decl);

		HAL->SetStreamSource(0, instance.geometry.VB, instance.geometry.VertexStart, instance.geometry.VertexStride);
		HAL->SetIndices(instance.geometry.IB, instance.geometry.IndexType);

		HAL->SetPrimitiveTopology(instance.geometry.PrimType);
		HAL->DrawIndexedInstanced(instance.geometry.IndexCount, 1, instance.geometry.IndexStart, instance.geometry.VertexStart);
	}

	HAL->ProfileEndEvent();
}

void GpuScene::BuilRaytracing()
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();

	// Build bottom level acceleration structure
	for (int id = 0; id < m_Instances.GetSize(); id++)
	{
		InstanceData& instance = m_Instances[id];
		hal->BuildBLAS(instance.geometry.AS);
	}

	// Build top level acceleration structure
	hal->BuildTLAS();
}
