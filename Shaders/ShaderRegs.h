#pragma once

//
// Defines shader static allocation map (minimize constant transfer)
//

enum EVisualize
{
	EVIZ_LIT,
	EVIZ_PATHTRACER,
	EVIZ_SHOW_NORMAL,
	EVIZ_SHOW_ROUGHNESS,
	EVIZ_SHOW_METAL,
	EVIZ_SHOW_RAYTRACING_DEBUG
};

struct GlobalParameters
{
	U32 Visualize;
};
