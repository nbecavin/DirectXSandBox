	#include "sh_const.h"
	
	void vh2d_base(in const float4 pos : SV_Position )
	{
	}

	ScreenVertexVsOutput vh_screenvertex(in const ScreenVertexVsInput i)
	{
		ScreenVertexVsOutput o;
		o.pos.xy = i.pos.xy;// *USER(0).xy + USER(0).zw;
		o.pos.zw = i.pos.zw;
		o.tex = i.tex;
		return o;
	}
