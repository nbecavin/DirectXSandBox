#ifndef __RENDERER_PIPELINE_STATE_H__
#define __RENDERER_PIPELINE_STATE_H__

struct DepthStencilDesc
{
	D3D11_DEPTH_STENCIL_DESC desc;
	DepthStencilDesc()
	{
		CD3D11_DEFAULT def;
		CD3D11_DEPTH_STENCIL_DESC _desc(def);
		desc = _desc;
	}
};

struct RasterizerDesc
{
	D3D11_RASTERIZER_DESC desc;
	RasterizerDesc()
	{
		CD3D11_DEFAULT def;
		CD3D11_RASTERIZER_DESC _desc(def);
		desc = _desc;
	}
};

struct BlendDesc
{
	D3D11_BLEND_DESC desc;
	BlendDesc()
	{
		CD3D11_DEFAULT def;
		CD3D11_BLEND_DESC _desc(def);
		desc = _desc;
	}
};

struct SamplerDesc
{
	D3D11_SAMPLER_DESC desc;
	SamplerDesc()
	{
		ZeroMemory(&desc, sizeof(desc));
	}
};

#endif //__RENDERER_PIPELINE_STATE_H__
