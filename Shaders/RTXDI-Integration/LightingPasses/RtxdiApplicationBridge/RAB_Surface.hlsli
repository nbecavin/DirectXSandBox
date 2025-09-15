#ifndef RTXDI_RAB_SURFACE_HLSLI
#define RTXDI_RAB_SURFACE_HLSLI

#include "../../GBufferHelpers.hlsli"
#include "../../SceneGeometry.hlsli"
#include "../../ShaderParameters.h"

#include "RAB_RandomSamplerState.hlsli"
#include "RAB_Material.hlsli"

static const bool kSpecularOnly = false;

struct RAB_Surface
{
    float3 worldPos;
    float3 viewDir;
    float3 normal;
    float3 geoNormal;
    float viewDepth;
    float diffuseProbability;
    RAB_Material material;
};

RAB_Surface RAB_EmptySurface()
{
    RAB_Surface surface = (RAB_Surface)0;
    surface.viewDepth = BACKGROUND_DEPTH;
    return surface;
}

// Checks if the given surface is valid, see RAB_GetGBufferSurface.
bool RAB_IsSurfaceValid(RAB_Surface surface)
{
    return surface.viewDepth != BACKGROUND_DEPTH;
}

// Returns the world position of the given surface
float3 RAB_GetSurfaceWorldPos(RAB_Surface surface)
{
    return surface.worldPos;
}

RAB_Material RAB_GetMaterial(RAB_Surface surface)
{
    return surface.material;
}

// Returns the world shading normal of the given surface
float3 RAB_GetSurfaceNormal(RAB_Surface surface)
{
    return surface.normal;
}

// Returns the linear depth of the given surface.
// It doesn't have to be linear depth in a strict sense (i.e. viewPos.z),
// and can be distance to the camera or primary path length instead.
// Just make sure that the motion vectors' .z component follows the same logic.
float RAB_GetSurfaceLinearDepth(RAB_Surface surface)
{
    return surface.viewDepth;
}

float getSurfaceDiffuseProbability(RAB_Surface surface)
{
    float diffuseWeight = calcLuminance(surface.material.diffuseAlbedo);
    float specularWeight = calcLuminance(Schlick_Fresnel(surface.material.specularF0, dot(surface.viewDir, surface.normal)));
    float sumWeights = diffuseWeight + specularWeight;
    return sumWeights < 1e-7f ? 1.f : (diffuseWeight / sumWeights);
}

RAB_Surface GetGBufferSurface(
    int2 pixelPosition, 
    bool previousFrame,
    PlanarViewConstants view, 
    Texture2D<float> depthTexture, 
    Texture2D<uint> normalsTexture, 
    Texture2D<uint> geoNormalsTexture)
{
    RAB_Surface surface = RAB_EmptySurface();

    if (any(pixelPosition >= view.viewportSize))
        return surface;

    surface.viewDepth = depthTexture[pixelPosition];

    if(surface.viewDepth == BACKGROUND_DEPTH)
        return surface;

    surface.material = RAB_GetGBufferMaterial(pixelPosition, previousFrame);

    surface.geoNormal = octToNdirUnorm32(geoNormalsTexture[pixelPosition]);
    surface.normal = octToNdirUnorm32(normalsTexture[pixelPosition]);
    surface.worldPos = viewDepthToWorldPos(view, pixelPosition, surface.viewDepth);
    surface.viewDir = normalize(view.cameraDirectionOrPosition.xyz - surface.worldPos);
    surface.diffuseProbability = getSurfaceDiffuseProbability(surface);

    return surface;
}


// Reads the G-buffer, either the current one or the previous one, and returns a surface.
// If the provided pixel position is outside of the viewport bounds, the surface
// should indicate that it's invalid when RAB_IsSurfaceValid is called on it.
RAB_Surface RAB_GetGBufferSurface(int2 pixelPosition, bool previousFrame)
{
    if(previousFrame)
    {
        return GetGBufferSurface(
            pixelPosition,
            previousFrame,
            g_Const.prevView,
            t_PrevGBufferDepth, 
            t_PrevGBufferNormals, 
            t_PrevGBufferGeoNormals);
    }
    else
    {
        return GetGBufferSurface(
            pixelPosition, 
            previousFrame,
            g_Const.view, 
            t_GBufferDepth, 
            t_GBufferNormals, 
            t_GBufferGeoNormals);
    }
}

float3 worldToTangent(RAB_Surface surface, float3 w)
{
    // reconstruct tangent frame based off worldspace normal
    // this is ok for isotropic BRDFs
    // for anisotropic BRDFs, we need a user defined tangent
    float3 tangent;
    float3 bitangent;
    ConstructONB(surface.normal, tangent, bitangent);

    return float3(dot(bitangent, w), dot(tangent, w), dot(surface.normal, w));
}

float3 tangentToWorld(RAB_Surface surface, float3 h)
{
    // reconstruct tangent frame based off worldspace normal
    // this is ok for isotropic BRDFs
    // for anisotropic BRDFs, we need a user defined tangent
    float3 tangent;
    float3 bitangent;
    ConstructONB(surface.normal, tangent, bitangent);

    return bitangent * h.x + tangent * h.y + surface.normal * h.z;
}

bool RAB_GetSurfaceBrdfSample(RAB_Surface surface, inout RAB_RandomSamplerState rng, out float3 dir)
{
    float3 rand;
    rand.x = RAB_GetNextRandom(rng);
    rand.y = RAB_GetNextRandom(rng);
    rand.z = RAB_GetNextRandom(rng);
    if (rand.x < surface.diffuseProbability)
    {
        if (kSpecularOnly)
            return false;

        float pdf;
        float3 h = SampleCosHemisphere(rand.yz, pdf);
        dir = tangentToWorld(surface, h);
    }
    else
    {
        float3 Ve = normalize(worldToTangent(surface, surface.viewDir));
        float3 h = ImportanceSampleGGX_VNDF(rand.yz, max(surface.material.roughness, kMinRoughness), Ve, 1.0);
        h = normalize(h);
        dir = reflect(-surface.viewDir, tangentToWorld(surface, h));
    }

    return dot(surface.normal, dir) > 0.f;
}

float RAB_GetSurfaceBrdfPdf(RAB_Surface surface, float3 dir)
{
    float cosTheta = saturate(dot(surface.normal, dir));
    float diffusePdf = kSpecularOnly ? 0.f : (cosTheta / M_PI);
    float specularPdf = ImportanceSampleGGX_VNDF_PDF(max(surface.material.roughness, kMinRoughness), surface.normal, surface.viewDir, dir);
    float pdf = cosTheta > 0.f ? lerp(specularPdf, diffusePdf, surface.diffuseProbability) : 0.f;
    return pdf;
}

#endif // RTXDI_RAB_SURFACE_HLSLI