#include "sh_const.h"
#include "sh_math.h"
#include "gbuffer.hlsli"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);

float4 main(ScreenVertexVsOutput i) : SV_Target
{
	float3 vSample = t0.Sample(s0, i.tex).rgb;
	
	GBuffer gbuffer;
	gbuffer.Unpack(i.tex);
	
	if (Global.Visualize == EVIZ_SHOW_ALBEDO)
		vSample.rgb = gbuffer.albedo;
	if (Global.Visualize == EVIZ_SHOW_NORMAL)
		vSample.rgb = gbuffer.shading_normal * 0.5 + 0.5;
	if (Global.Visualize == EVIZ_SHOW_ROUGHNESS)
		vSample.rgb = gbuffer.roughness.xxx;
	if (Global.Visualize == EVIZ_SHOW_METAL)
		vSample.rgb = gbuffer.metallic.xxx;

	// Gamma correction : Linear -> sRGB
	vSample.rgb = LinearToSRGB(vSample, false);
	
	return float4(vSample, 1);
}
