	#include "sh_const.h"

	VS_Output vh_base( in const float4 pos : POSITION,
					   in const float3 normal : NORMAL,
					   in const float2 uv : TEXCOORD0 )
	{
		VS_Output Out;
		
		float4 opos,wpos;
		wpos = mul( pos, WORLD_MATRIX );
		opos = mul( wpos, viewMatrix );//VIEW_MATRIX );
		opos = mul( opos, projMatrix );//PROJ_MATRIX );
		Out.position = opos;
		
		float3 lnormal;
		lnormal.xy = normal.xy;
		lnormal.z  = 1 - sqrt( normal.x*normal.x + normal.y*normal.y );

		float3 n = lnormal;//mul( lnormal, WORLD_MATRIX );
		Out.normal = n;
		
		Out.color.xyz = 1;
		Out.color.w = 1;
		
		Out.uv = uv;

		Out.eyevec.xyz = EYE_WORLD-wpos.xyz;
		
		return Out;
	}
