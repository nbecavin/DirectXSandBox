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
		void GatherDrawObjects();
		void UpdateBuffers();

		RaytracingScene* GetRaytracingScene() {
			return m_RaytracingScene.get();
		}

		void SetActiveMaterial(Material* Mat);
		void DrawScene();

	protected:
		MaterialDA		m_MaterialDA;
		std::shared_ptr<Bitmap> m_MaterialStorage;

		InstanceDataDA m_Instances;
		DynArray<ConstantBuffer*, 256> m_InstanceData; //Naive construct

		std::unique_ptr<RaytracingScene> m_RaytracingScene;
	};

}

