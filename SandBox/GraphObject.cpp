#include <GraphObject.h>

GraphObject::GraphObject()
{
	SetName(std::string("Unknown"));
}

void GraphObject::SetScale(float scale)
{
	m_Node.m[0][0] = scale;
	m_Node.m[1][1] = scale;
	m_Node.m[2][2] = scale;
}

void GraphObject::SetWorldPosition(float x,float y,float z)
{
	m_Node.m[0][3] = x;
	m_Node.m[1][3] = y;
	m_Node.m[2][3] = z;
}

void GraphObject::SetMatrix(const Mat4x4& m)
{
	m_Node = m;
}

void GraphObject::Update(F32 dTime)
{
}

void GraphObject::Draw()
{
}

void GraphObject::DrawGBuffer()
{

	double test = std::max(1.0, 2.0);
}
