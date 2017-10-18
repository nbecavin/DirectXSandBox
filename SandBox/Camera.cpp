#include <Camera.h>

Camera::Camera()
{
	m_WorldPos.Set(0.f,0.f,-1.f);
	m_WorldTarget.Set(0.f,0.f,0.f);
	Update();
}

void Camera::Update()
{
#ifdef _PC
	XMMATRIX m,proj,view;
	XMVECTOR eye = *(XMVECTOR*)&m_WorldPos;
	XMVECTOR at = *(XMVECTOR*)&m_WorldTarget;
	XMVECTOR up = XMVectorSet(0,1,0,0);
	m_Node = *reinterpret_cast<Mat4x4*>(&XMMatrixLookAtRH(eye,at,up));
#endif
}
