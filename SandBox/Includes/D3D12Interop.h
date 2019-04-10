#pragma once
#ifndef __D3D12_INTEROP_H__
#define __D3D12_INTEROP_H__

namespace D3D12Interop
{
	inline void BlendState(D3D12_BLEND_DESC& dest, const D3D11_BLEND_DESC& source)
	{
		dest.AlphaToCoverageEnable = source.AlphaToCoverageEnable;
		dest.IndependentBlendEnable = source.IndependentBlendEnable;
		for (int i = 0; i < _countof(source.RenderTarget); i++)
		{
			D3D12_RENDER_TARGET_BLEND_DESC& dstRT = dest.RenderTarget[i];
			const D3D11_RENDER_TARGET_BLEND_DESC& srcRT = source.RenderTarget[i];
			dstRT.BlendEnable = srcRT.BlendEnable;
			dstRT.LogicOpEnable = false;
			dstRT.SrcBlend = (D3D12_BLEND)srcRT.SrcBlend;
			dstRT.DestBlend = (D3D12_BLEND)srcRT.DestBlend;
			dstRT.BlendOp = (D3D12_BLEND_OP)srcRT.BlendOp;
			dstRT.SrcBlendAlpha = (D3D12_BLEND)srcRT.SrcBlendAlpha;
			dstRT.DestBlendAlpha = (D3D12_BLEND)srcRT.DestBlendAlpha;
			dstRT.BlendOpAlpha = (D3D12_BLEND_OP)srcRT.BlendOpAlpha;
			dstRT.LogicOp = D3D12_LOGIC_OP_CLEAR;
			dstRT.RenderTargetWriteMask = srcRT.RenderTargetWriteMask;
		}
	}

	inline void RasterizerState(D3D12_RASTERIZER_DESC& dest, const D3D11_RASTERIZER_DESC& source)
	{
		dest.AntialiasedLineEnable = FALSE;
		dest.CullMode = (D3D12_CULL_MODE)source.CullMode;
		dest.DepthBias = source.DepthBias;
		dest.DepthBiasClamp = source.DepthBiasClamp;
		dest.DepthClipEnable = source.DepthClipEnable;
		dest.FillMode = (D3D12_FILL_MODE)source.FillMode;
		dest.FrontCounterClockwise = source.FrontCounterClockwise;
		dest.MultisampleEnable = source.MultisampleEnable;
		dest.SlopeScaledDepthBias = source.SlopeScaledDepthBias;
		dest.ForcedSampleCount = 0;
		dest.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	inline void DepthStencilState(D3D12_DEPTH_STENCIL_DESC& dest, const D3D11_DEPTH_STENCIL_DESC& source)
	{
		dest.DepthEnable = source.DepthEnable;
		dest.DepthWriteMask = (D3D12_DEPTH_WRITE_MASK)source.DepthWriteMask;
		dest.DepthFunc = (D3D12_COMPARISON_FUNC)source.DepthFunc;
		dest.StencilEnable = source.StencilEnable;
		dest.StencilReadMask = source.StencilReadMask;
		dest.StencilWriteMask = source.StencilWriteMask;
		dest.FrontFace.StencilFailOp = (D3D12_STENCIL_OP)source.FrontFace.StencilFailOp;
		dest.FrontFace.StencilDepthFailOp = (D3D12_STENCIL_OP)source.FrontFace.StencilDepthFailOp;
		dest.FrontFace.StencilPassOp = (D3D12_STENCIL_OP)source.FrontFace.StencilPassOp;
		dest.FrontFace.StencilFunc = (D3D12_COMPARISON_FUNC)source.FrontFace.StencilFunc;
		dest.BackFace.StencilFailOp = (D3D12_STENCIL_OP)source.BackFace.StencilFailOp;
		dest.BackFace.StencilDepthFailOp = (D3D12_STENCIL_OP)source.BackFace.StencilDepthFailOp;
		dest.BackFace.StencilPassOp = (D3D12_STENCIL_OP)source.BackFace.StencilPassOp;
		dest.BackFace.StencilFunc = (D3D12_COMPARISON_FUNC)source.BackFace.StencilFunc;
	}
	
};

#endif //__D3D12_INTEROP_H__
