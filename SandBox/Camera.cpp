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

#include <imgui.h>

void Camera::DebugDraw()
{
	ImGui::Begin("CameraDebug", nullptr);
	char str[1024];
	sprintf(str, "Position %.3f %.3f %.3f", m_WorldPos.x, m_WorldPos.y, m_WorldPos.z);
	ImGui::Text(str);
	sprintf(str, "Target %.3f %.3f %.3f", m_WorldTarget.x, m_WorldTarget.y, m_WorldTarget.z);	
	ImGui::Text(str);
	ImGui::End();
}