#pragma once

//
// Defines shader static allocation map (minimize constant transfer)
//

enum EVisualize
{
	EVIZ_LIT = 0,
	EVIZ_SHOW_NORMAL = 1,
	EVIZ_SHOW_ROUGHNESS = 2,
	EVIZ_SHOW_METAL = 3
};

struct GlobalParameters
{
	U32 Visualize;
};
