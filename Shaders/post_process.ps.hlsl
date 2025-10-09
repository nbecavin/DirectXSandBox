#include "sh_const.h"
#include "sh_math.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);

float4 Tonemapping(ScreenVertexVsOutput i) : SV_Target
{
	float3 vSample = t0.Sample(s0, i.tex).rgb;
	
	// Gamma correction : Linear -> sRGB
	vSample.rgb = LinearToSRGB(vSample, false);

	return float4(vSample,1);
}
