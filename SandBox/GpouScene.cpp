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
	UpdateBuffers();
}

void GpuScene::Shut()
{
}

void GpuScene::RegisterMaterial(Material* material)
{
	U32 id = m_MaterialDA.Add(material);

	material->SetGPUDataOffset(m_MaterialStorageWriteOffset);
	m_MaterialStorageWriteOffset += sizeof(MaterialParameter);
	//EXCEPTION();
}

void GpuScene::UpdateBuffers()
{
	if (m_MaterialStorage.get() == nullptr)
	{
		U32 buffer_byte_size = sMaterialMaxCount * sizeof(MaterialParameter);
		m_MaterialStorage.reset(gData.Rdr->GetHAL()->CreateBuffer(buffer_byte_size, Buffer::CpuAccess_Write));
		m_MaterialStorageWriteOffset = 0;

		m_MaterialSRV.reset(gData.Rdr->GetHAL()->CreateShaderResourceView(m_MaterialStorage.get(), sMaterialMaxCount, sizeof(MaterialParameter)));
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

	// Update storage data
	U64 dataOffset = Mat->GetGPUDataOffset();
	MaterialParameter* param = (MaterialParameter *)m_MaterialStorage->Map(dataOffset);
	if (param)
	{
		param->albedo.Set(Mat->GetDiffuse().x, Mat->GetDiffuse().y, Mat->GetDiffuse().z);
		param->roughness = Mat->GetRoughness();
		param->metallic = Mat->GetMetallic();
		param->emission.Set(0, 0, 0);// Mat->GetEmissive();
		param->opacity = 1.f;
		param->ao = 1.f;

		auto BindSRV = [&HAL](Material* Mat, U32 Stage, U32 Slot, U32& BindlessSlot)
			{
				Bitmap* bm = NULL;
				bm = Mat->GetBitmap(Stage);
				if (bm && bm->GetBinHwResId() != BM_INVALIDHWRESID)
				{
					TextureLink* tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
					BindlessSlot = tex->m_BindlessSRVIndex;
					//HAL->SetShaderResource(0, SHADER_TYPE_PIXEL, bm);
				}
			};

		BindSRV(Mat, MTL_STAGE_ALBEDO, 0, param->albedo_map);
		BindSRV(Mat, MTL_STAGE_NORMAL, 1, param->normal_map);
		BindSRV(Mat, MTL_STAGE_ROUGHNESS, 2, param->roughness_map);
	}
}

void GpuScene::DrawScene(PassHint hint)
{
	auto* HAL = gData.Rdr->GetHAL();

	HAL->ProfileBeginEvent(0, "GpuScene Draw");

	HAL->SetShaderResource(16, SHADER_TYPE_VERTEX, m_MaterialSRV.get());
	HAL->SetShaderResource(16, SHADER_TYPE_PIXEL, m_MaterialSRV.get());

	// Grow material constant buffer array if needed
	//auto* mtl_cst = HAL->CreateConstantBuffer(sizeof(MaterialParameter) * 10000);

	for (int id = 0; id < m_Instances.GetSize(); id++)
	{
		InstanceData& instance = m_Instances[id];

		// Grow
		if( id >= m_InstanceData.GetSize())
		{
			auto* c0 = HAL->CreateConstantBuffer(sizeof(InstanceConstant));
			m_InstanceData.Add(c0);

			auto* c1 = HAL->CreateConstantBuffer(sizeof(InstanceIDConstant));
			m_InstanceIDData.Add(c1);
		}

		HAL->BindGraphicPipelineState(ShaderMap::BaseMeshVS, hint == PassHint::GBuffer ? ShaderMap::GBufferPassPS : ShaderMap::BasePassPS);

		HAL->SetConstantBuffer(1, SHADER_TYPE_VERTEX, m_InstanceData[id]);
		HAL->SetConstantBuffer(2, SHADER_TYPE_VERTEX, m_InstanceIDData[id]);
		HAL->SetConstantBuffer(2, SHADER_TYPE_PIXEL, m_InstanceIDData[id]);
		SetActiveMaterial(instance.shading.Mtl);

		// fill instance data
		{
			InstanceConstant* c;
			m_InstanceData[id]->Lock(0, 0, (void**)&c);
			c[0] = instance.instanceCst;
			m_InstanceData[id]->Unlock();
		}
		{
			InstanceIDConstant* c;
			m_InstanceIDData[id]->Lock(0, 0, (void**)&c);
			c[0].MaterialID = instance.shading.Mtl->GetGPUDataOffset() / sizeof(MaterialParameter);
			c[0].MatricesID = 0;
			m_InstanceIDData[id]->Unlock();
		}

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
