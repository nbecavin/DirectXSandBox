#ifndef __MATERIAL_HH__
#define __MATERIAL_HH__

class Bitmap;

enum MaterialStage
{
	MTL_STAGE_DIFFUSE,
	MTL_STAGE_NORMAL,
	MTL_STAGE_SPEC
};

class Material
{
protected:
	Bitmap * m_BitmapStage[16];

public:
	Material();

	void SetBitmap(Bitmap* Bmap,U32 StageId=0);
	Bitmap * GetBitmap(U32 StageId=0) const { return m_BitmapStage[StageId]; }
};

#endif //__MATERIAL_HH__
