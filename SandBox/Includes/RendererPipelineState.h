#ifndef __RENDERER_PIPELINE_STATE_H__
#define __RENDERER_PIPELINE_STATE_H__

struct DepthStencilDesc
{
	D3D12_DEPTH_STENCIL_DESC desc;
	DepthStencilDesc()
	{
		CD3DX12_DEFAULT def;
		CD3DX12_DEPTH_STENCIL_DESC _desc(def);
		desc = _desc;
	}
};

struct RasterizerDesc
{
	D3D12_RASTERIZER_DESC desc;
	RasterizerDesc()
	{
		CD3DX12_DEFAULT def;
		CD3DX12_RASTERIZER_DESC _desc(def);
		desc = _desc;
	}
};

struct BlendDesc
{
	D3D12_BLEND_DESC desc;
	BlendDesc()
	{
		CD3DX12_DEFAULT def;
		CD3DX12_BLEND_DESC _desc(def);
		desc = _desc;
	}
};

struct SamplerDesc
{
	D3D12_SAMPLER_DESC desc;
	SamplerDesc()
	{
		ZeroMemory(&desc, sizeof(desc));
	}
	bool operator ==(const SamplerDesc& _in) const
	{
		Bool bEqual =
			(desc.Filter == _in.desc.Filter) &&
			(desc.AddressU == _in.desc.AddressU) &&
			(desc.AddressV == _in.desc.AddressV) &&
			(desc.AddressW == _in.desc.AddressW) &&
			(desc.MipLODBias == _in.desc.MipLODBias) &&
			(desc.MaxAnisotropy == _in.desc.MaxAnisotropy) &&
			(desc.ComparisonFunc == _in.desc.ComparisonFunc) &&
			(desc.BorderColor[0] == _in.desc.BorderColor[0]) &&
			(desc.BorderColor[1] == _in.desc.BorderColor[1]) &&
			(desc.BorderColor[2] == _in.desc.BorderColor[2]) &&
			(desc.BorderColor[3] == _in.desc.BorderColor[3]) &&
			(desc.MinLOD == _in.desc.MinLOD) &&
			(desc.MaxLOD == _in.desc.MaxLOD);
		return bEqual;
	}
};

#endif //__RENDERER_PIPELINE_STATE_H__
