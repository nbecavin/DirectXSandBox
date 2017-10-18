
	struct VS_Output
	{
		float4 position : POSITION;
		float4 color : COLOR0;
	};

	VS_Output vh_2d_base( in const float4 pos : POSITION,
						  in const float3 normal : NORMAL )
	{
		VS_Output Out;
		Out.position = pos;
		Out.position.w = 1.f;
		
		// Basic directional lighting
		float3 DLIGHT_DIR = normalize(float3(1,1,1));
		float3 DLIGHT_COLOR = float3( 0.8, 1, 0.9 );
		
		float3 n = normalize(normal)*2-1;
		float lDotN = saturate( dot( DLIGHT_DIR, n ) );
		float light = lDotN*DLIGHT_COLOR;
		
		Out.color.xyz = light;
		Out.color.w = 1;
		
		return Out;
	}
