#ifndef __MATERIAL_HH__
#define __MATERIAL_HH__

class Bitmap;

enum MaterialStage
{
	MTL_STAGE_ALBEDO,
	MTL_STAGE_NORMAL,
	MTL_STAGE_ROUGHNESS,
	MTL_STAGE_METAL,
};

class aiMaterial;
class Material : public GraphObject
{
protected:
	Bitmap * m_BitmapStage[16];

	Vec4f m_Diffuse = Vec4f(0.5, 0.5, 0.5, 1);
	float m_Roughness = 0.5f;
	float m_Metallic = 0.f;
	float m_Emission = 0.f;

	U64 m_GPUStorageOffset = 0;

public:
	Material();

	void SetBitmap(Bitmap* Bmap,U32 StageId=0);
	Bitmap * GetBitmap(U32 StageId=0) const { return m_BitmapStage[StageId]; }

	Vec4f GetDiffuse() { return m_Diffuse; }
	float GetRoughness() { return m_Roughness; }
	float GetMetallic() { return m_Metallic; }
	float GetEmission() { return m_Emission; }

	void SetGPUDataOffset(U64 Offset) { m_GPUStorageOffset = Offset; }
	U64 GetGPUDataOffset() const { return m_GPUStorageOffset; }

	void LoadFromAiMaterial(std::filesystem::path directory, aiMaterial* importMaterial);
};

typedef DynArray<Material*, 16> MaterialDA;

#endif //__MATERIAL_HH__
