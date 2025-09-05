#pragma once
#ifndef __SH_MATH_HH__
#define __SH_MATH_HH__

static const float PI = 3.14159265359;
static const float TWO_PI = 6.28318530718;
static const float HALF_PI = 1.57079632679;

//
// Transformations
//

float3x3 MakeTBN(float3 tangent, float3 binormal, float3 normal)
{
	return transpose(float3x3(tangent, binormal, normal));
}

//
// Color space
//

// sRGB encoding constants
static const float SRGB_EPS = 0.0031308;  // linear threshold
static const float SRGB_A = 1.055;
static const float SRGB_B = 0.055;
static const float SRGB_PHI = 12.92;
static const float SRGB_GAMMA_INV = 1.0 / 2.4;

// Scalar encode: linear (0..1) -> sRGB (0..1)
float LinearToSRGB(float c, bool fast)
{
    c = saturate(c); // keep in [0,1] to avoid NaNs in pow
    if (fast)
        return pow(c, 1.f / 2.2f);
    else
        return (c <= SRGB_EPS) ? (SRGB_PHI * c) : (SRGB_A * pow(c, SRGB_GAMMA_INV) - SRGB_B);
}

// Vector overloads
float3 LinearToSRGB(float3 c, bool fast)
{
    return float3(
        LinearToSRGB(c.r, fast),
        LinearToSRGB(c.g, fast),
        LinearToSRGB(c.b, fast)
    );
}

float4 LinearToSRGB(float4 c, bool fast)
{
    return float4(LinearToSRGB(c.rgb, fast), c.a); // alpha stays linear
}

// sRGB <-> Linear helpers (useful if your textures are read as linear)
float SRGBToLinear(float c)
{
    return (c <= 0.04045) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
}
float2 SRGBToLinear(float2 c) { return float2(SRGBToLinear(c.r), SRGBToLinear(c.g)); }
float3 SRGBToLinear(float3 c) { return float3(SRGBToLinear(c.r), SRGBToLinear(c.g), SRGBToLinear(c.b)); }
float4 SRGBToLinear(float4 c) { return float4(SRGBToLinear(c.r), SRGBToLinear(c.g), SRGBToLinear(c.b), c.a); }

#endif //__SH_MATH_HH__
