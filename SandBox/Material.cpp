#include <Material.h>

Material::Material()
{
	memset(m_BitmapStage,0,sizeof(m_BitmapStage));
}

void Material::SetBitmap(Bitmap* Bmap,U32 StageId)
{
	m_BitmapStage[StageId]=Bmap;
}
