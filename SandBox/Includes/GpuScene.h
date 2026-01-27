#pragma once

#include <Material.h>
#include <Bitmap.h>

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

		void UpdateBuffers();

	protected:
		MaterialDA		m_MaterialDA;



		std::shared_ptr<Bitmap> m_MaterialStorage;

	};

}

