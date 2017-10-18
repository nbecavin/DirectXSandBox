#include <CameraFree.h>
#include <Camera.h>
#include <Renderer.h>
#include <InputManager.h>

CameraFree::CameraFree()
{
}

void CameraFree::Update(F32 dTime)
{
	Camera * cam = gData.Rdr->GetCamera();

	sys::InputManager * input = gData.Input;

	float forward = input->GetAction(0,INPUT_ACTION_MOVE_FORWARD);
	float backward = input->GetAction(0,INPUT_ACTION_MOVE_BACKWARD);
	float left = input->GetAction(0,INPUT_ACTION_MOVE_LEFT);
	float right = input->GetAction(0,INPUT_ACTION_MOVE_RIGHT);
	float up = input->GetAction(0,INPUT_ACTION_MOVE_UP);
	float down = input->GetAction(0,INPUT_ACTION_MOVE_DOWN);

	float x = left-right;
	float y = up-down;
	float z = forward-backward;

	float speed = dTime*4.5f;

	Mat4x4 viewMatrix = cam->GetViewMatrix();
	
	Vec4f campos = cam->GetWorldPosition();
	Vec4f camdir = cam->GetWorldTarget()-cam->GetWorldPosition();
	camdir.Normalize();

	Vec4f camleft,camup;
	Vec4f upvec(0.0,1.f,0.f,1.f);
	Vec4::Cross(&camleft,&upvec,&camdir);
	Vec4::Cross(&camup,&camleft,&camdir);

	// Translations
	campos += camleft*speed*x;
	campos += camup*speed*y;
	campos += camdir*speed*z;

	//
	float speedangle = dTime*1.f;
	float rotateLeft = input->GetAction(0,INPUT_ACTION_RSTICK_LEFT);
	float rotateRight = input->GetAction(0,INPUT_ACTION_RSTICK_RIGHT);
	float rotateUp = input->GetAction(0,INPUT_ACTION_RSTICK_UP);
	float rotateDown = input->GetAction(0,INPUT_ACTION_RSTICK_DOWN);
	float yangle = rotateRight*speedangle - rotateLeft*speedangle;
	float xangle = rotateUp*speedangle - rotateDown*speedangle;

	// Rotations
#ifdef _PC
	XMVECTOR qrotx = XMQuaternionRotationAxis(*reinterpret_cast<XMVECTOR*>(&camup),yangle);
	XMVECTOR qroty = XMQuaternionRotationAxis(*reinterpret_cast<XMVECTOR*>(&camleft),xangle);
	XMMATRIX mrotx = XMMatrixRotationQuaternion(qrotx);
	XMMATRIX mroty = XMMatrixRotationQuaternion(qroty);

	XMVECTOR _camdir = XMVector4Transform(*reinterpret_cast<XMVECTOR*>(&camdir),mrotx);
	_camdir = XMVector4Transform(_camdir,mroty);
	camdir = *reinterpret_cast<Vec4f*>(&_camdir);

	cam->SetWorldPosition(campos);
	cam->SetWorldTarget(campos+camdir);
#endif
}
