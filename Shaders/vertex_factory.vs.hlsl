#include "sh_const.h"
#include "sh_math.h"

VS_Output MeshFactory(
	in const float4 pos : POSITION,
	in const float3 tangent : TANGENT,
	in const float3 binormal : BINORMAL,
	in const float3 normal : NORMAL,
	in const float2 uv : TEXCOORD0 )
{
	VS_Output Out;
		
	float4 opos,wpos;
	wpos = mul( pos, WORLD_MATRIX );
	opos = mul( wpos, Camera.viewMatrix );
	opos = mul( opos, Camera.projMatrix );
	Out.position = opos;
		
	float3 lnormal;
	lnormal.xy = normal.xy;
	lnormal.z  = 1 - sqrt( normal.x*normal.x + normal.y*normal.y );

	float3 n = lnormal;//mul( lnormal, WORLD_MATRIX );
	//Out.normal = n;

	Out.tbn = MakeTBN(tangent, binormal, normal);

	Out.color.xyz = 1;
	Out.color.w = 1;
		
	Out.uv = uv;

	Out.eyevec.xyz = Camera.eyeWorld - wpos.xyz;
		
	return Out;
}
