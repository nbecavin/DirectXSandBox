#include "sh_const.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
	
half4 ph_colorgrading(ScreenVertexVsOutput i) : SV_Target
{
	half3 vSample = t0.Sample(s0, i.tex);
	
	// Gamma correction : Linear -> sRGB
	vSample.rgb = pow(vSample.rgb,1.f/2.2f);

	return half4(vSample,1);
}

half4 ph_passthrough(ScreenVertexVsOutput i) : SV_Target
{
	return t0.Sample(s0, i.tex);
}
