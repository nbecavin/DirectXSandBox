
struct VSOutput
{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
};

VSOutput VSRenderFSQuad( in const float4 position : POSITION )
{
	VSOutput	Out;
	Out.position = position;
	Out.texcoord = 0;
	return Out;
}

float4 PSPostProcess( ) : COLOR0
{
	float4 c;
	c.x = 1;
	c.y = 0.5;
	c.z = 0.5;
	c.w = 1;
	return c;
}

technique PostProcess
{
    pass P0
    {
		VertexShader = compile vs_3_0 VSRenderFSQuad();
        PixelShader = compile ps_3_0 PSPostProcess();
    }
}
