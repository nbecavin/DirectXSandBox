#ifndef __SKY_HH__
#define __SKY_HH__

#include <GraphObject.h>
#include <Renderer.h>

class Sky : public GraphObject
{
public:
	Sky();
	virtual ~Sky();
 
	virtual void Update(F32 dTime);
	virtual void Draw();

protected:
	VertexBuffer		*	m_VB;
	IndexBuffer			*	m_IB;
	VertexDeclaration	*	m_VertexDecl;
};

#endif //__SKY_HH__
