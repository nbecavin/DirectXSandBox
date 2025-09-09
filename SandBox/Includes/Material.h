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

	Vec4f m_Diffuse;
	float m_Roughness;

public:
	Material();

	void SetBitmap(Bitmap* Bmap,U32 StageId=0);
	Bitmap * GetBitmap(U32 StageId=0) const { return m_BitmapStage[StageId]; }

	Vec4f GetDiffuse() { return m_Diffuse; }
	float GetRoughness() { return m_Roughness; }

	void LoadFromAiMaterial(std::filesystem::path directory, aiMaterial* importMaterial);
};

typedef DynArray<Material*, 16> MaterialDA;

#endif //__MATERIAL_HH__
