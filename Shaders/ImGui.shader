#include "sh_const.h"

Texture2D<float4> t;
SamplerState s;

struct ImGuiVertex
{
	float2 position : POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

struct ImGuiVsPs
{
	float4 position : SV_Position;
	float4 color : VERTEXCOLOR;
	float2 uv : TEXCOORD0;
};

float4 PsMain(in ImGuiVsPs i) : SV_Target0
{
	float4 color = /*t.Sample(s, i.uv) **/ i.color;
	return float4(color);
}

void VsMain(in ImGuiVertex i, out ImGuiVsPs o)
{
	o.position.xy = i.position.xy / float2(1280,720);

	// UV to NDC...
	o.position.xy = o.position.xy * 2 - 1;
	o.position.y *= -1;
	o.position.z = 0;
	o.position.w = 1;

	o.color = i.color;
	o.uv = i.uv;
}
