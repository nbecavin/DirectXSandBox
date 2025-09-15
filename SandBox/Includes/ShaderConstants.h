#ifndef __SHADER_CONSTANT__
#define __SHADER_CONSTANT__

#include <VectorLib.h>

struct GlobalConstant
{
	Mat4x4	WorldMatrix;
	Mat4x4	InvWorldMatrix;
};

//
// cb_camera
struct CameraConstant
{
	Mat4x4	ViewMatrix;
	Mat4x4	ProjMatrix;
	Mat4x4	InvProjMatrix;
	Mat4x4	InvViewProjMatrix;
	Vec4f	EyeWorld;
	Vec4f   dummy[3];
};

struct PostProcessConstant
{
	Vec4f	gPostProcess[16];
	Vec4f	gSampleOffsetWeight[16];
};

#endif //__SHADER_CONSTANT__
