#pragma once

#include <Material.h>
#include <Bitmap.h>
#include <Renderer.h>

#include <GpuSceneInstance.h>

class RaytracingScene;
namespace sys
{

	class GpuScene
	{
	public:
		GpuScene();
		virtual ~GpuScene();

		void Init();
		void Shut();

		void RegisterMaterial(Material* material);

		const MaterialDA& GetMaterialDA() const { return m_MaterialDA; }

		const InstanceDataDA& GetInstance() const { return m_Instances; }

		void BuilRaytracing();
		void GatherDrawObjects();
		void UpdateBuffers();

		RaytracingScene* GetRaytracingScene() {
			return m_RaytracingScene.get();
		}

		void SetActiveMaterial(Material* Mat);
		void DrawScene();

	protected:
		MaterialDA		m_MaterialDA;

		static const U32 sMaterialMaxCount = 16000;
		std::shared_ptr<Buffer> m_MaterialStorage;
		U64 m_MaterialStorageWriteOffset = 0;

		InstanceDataDA m_Instances;
		DynArray<ConstantBuffer*, 256> m_InstanceData; //Naive construct

		std::unique_ptr<RaytracingScene> m_RaytracingScene;
	};

}

