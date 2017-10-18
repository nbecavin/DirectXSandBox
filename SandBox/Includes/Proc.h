#ifndef __PROC_H__
#define __PROC_H__

#include <GraphObject.h>
#include <Renderer.h>
#include <Material.h>

enum ProcType {
	sphere,
	torus,
};

class Proc : public GraphObject
{
public:
	Proc();
	Proc(ProcType _type_);
	virtual ~Proc();
 
	virtual void Update(F32 dTime);
	virtual void Draw();

protected:
	VertexBuffer		*	m_VB;
	IndexBuffer			*	m_IB;
	VertexDeclaration	*	m_VertexDecl;

	Material				m_Mat;

	ProcType				m_Type;
};

#endif //__TERRAIN_H__
