#include <GpuScene.h>
#include <Renderer.h>

#include <../../Shaders/material_parameter.hlsli.h>

//
// collect drawcals

using namespace sys;

struct InstanceData
{

};

GpuScene::GpuScene()
{

}

GpuScene::~GpuScene()
{

}

void GpuScene::Init()
{
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
		gData.Rdr->GetHAL()->CreateBuffer();
	}
}

/*
void RenderHAL::PushMaterial(Material* Mat)
	{
		Bitmap* bm = NULL;
		bm = Mat->GetBitmap(MTL_STAGE_ALBEDO);
		if(bm)
		{
			SetShaderResource(0, SHADER_TYPE_PIXEL, bm);
		}
		bm = Mat->GetBitmap(MTL_STAGE_NORMAL);
		if(bm)
		{
			SetShaderResource(1, SHADER_TYPE_PIXEL, bm);
		}
		bm = Mat->GetBitmap(MTL_STAGE_ROUGHNESS);
		if(bm)
		{
			SetShaderResource(2, SHADER_TYPE_PIXEL, bm);
		}
	}
*/

