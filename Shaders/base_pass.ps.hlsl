#include "sh_const.h"
	
/*Texture2D    tex0 : register( t0 );
SamplerState g_samLinear : register( s0 );*/
	
Texture2D sDiffuse  : register(t0);
SamplerState sSampler : register(s0);
Texture2D sNormal   : register(t1);
Texture2D sSpecular : register(t2);
Texture2D sSSAO		: register(t7);

float4 ForwardMain( const in VS_Output i ) : SV_TARGET
{
	// Basic directional lighting
	float3 DLIGHT_DIR = normalize(float3(1,1,0));
	float3 DLIGHT_COLOR = float3( 0.8, 1, 0.9 );
	float3 DLIGHT_AMBIENT = float3( 0.1,0.1,0.2 );
	
	// Diffuse term
	float NoL = saturate( dot( DLIGHT_DIR, i.normal ) );
	float light = NoL * DLIGHT_COLOR+DLIGHT_AMBIENT;

	// Specular term
	float3 ref = reflect( -normalize(i.eyevec), normalize(i.normal) );
	float  EyeDotR = saturate(dot(normalize(DLIGHT_DIR),ref));

	float spec = saturate( pow(EyeDotR, 96 ) );
	light += spec*4;
		
	float3 normal = sNormal.Sample(sSampler, i.uv).xyz*0.5 + 0.5;
	//return light*i.color*sDiffuse.Sample(sSampler, i.uv);
	return i.color*sDiffuse.Sample(sSampler, i.uv);
}
	
float4 GbufferMain( const in VS_Output i ) : SV_TARGET
{
	float albedo_alpha = sDiffuse.Sample(sSampler, i.uv).a;
	float3 normal = sNormal.Sample(sSampler, i.uv).xyz*0.5+0.5;//i.normal;

	// get normal in world space
	return float4(normal,albedo_alpha);
}
