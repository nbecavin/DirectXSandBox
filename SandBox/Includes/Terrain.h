#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <GraphObject.h>
#include <Renderer.h>
#include <Material.h>

class Terrain : public GraphObject
{
public:
	Terrain();
	virtual ~Terrain();
 
	virtual void Update(F32 dTime);
	virtual void Draw();
	virtual void DrawGBuffer();

protected:
	int	GridSizeX,GridSizeY;

	VertexBuffer		*	m_VB;
	IndexBuffer			*	m_IB;
	VertexDeclaration	*	m_VertexDecl;

	Material				m_Mat;

};

#endif //__TERRAIN_H__
