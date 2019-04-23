#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera
{
public:
	Camera();

	void	SetWorldPosition(const Vec4f& worldPos) { m_WorldPos=worldPos; }
	void	SetWorldTarget(const Vec4f& targetPos)  { m_WorldTarget=targetPos; }
	void	Update();

	Mat4x4	GetViewMatrix() const { return m_Node; }
	//Mat4x4	GetInverseViewMatrix() const { return m_Node; }

	Vec4f	GetWorldPosition() const { return m_WorldPos; }
	Vec4f	GetWorldTarget() const { return m_WorldTarget; }
	
	void	DebugDraw();

private:
	Mat4x4				m_Node;
	Vec4f				m_WorldPos;
	Vec4f				m_WorldTarget;
};

#endif //__CAMERA_H__
