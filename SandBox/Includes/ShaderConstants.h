#ifndef __SHADER_CONSTANT__
#define __SHADER_CONSTANT__

#include <VectorLib.h>

struct MatConstant
{
	Mat4x4	m;
};

//
// cb_camera
struct CameraConstant
{
	Mat4x4	ViewMatrix;
	Mat4x4	ProjMatrix;
	Mat4x4	InvProjMatrix;
	Mat4x4	InvViewProjMatrix;
	static void * CreateHwRes();
};

struct PostProcessConstant
{
	Vec4f	gPostProcess[16];
	Vec4f	gSampleOffsetWeight[16];
	static void * CreateHwRes();
};

#endif //__SHADER_CONSTANT__
