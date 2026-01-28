#pragma once

#include <Material.h>
#include <Bitmap.h>
#include <Renderer.h>

#include <GpuSceneInstance.h>

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

		void GatherDrawObjects();
		void UpdateBuffers();

		void SetActiveMaterial(Material* Mat);
		void DrawScene();

	protected:
		MaterialDA		m_MaterialDA;
		std::shared_ptr<Bitmap> m_MaterialStorage;

		InstanceDataDA m_Instances;
		DynArray<ConstantBuffer*, 256> m_InstanceData; //Naive construct

	};

}

