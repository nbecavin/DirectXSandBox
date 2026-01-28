#include <Mesh.h>

Mesh::Mesh()
{
	m_Node.SetIdentity();
}

Mesh::~Mesh()
{
}

void Mesh::Update(F32 dTime)
{
	// Rotate 180° per sec
	static float angle = 0;
	float angle_inc = dTime*2.f/6.28f;
	angle += angle_inc;
//	*reinterpret_cast<XMMATRIX*>(&m_Node) = XMMatrixRotationY(angle);
}
