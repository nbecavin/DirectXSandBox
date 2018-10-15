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

	
};

#endif //__D3D12_INTEROP_H__
