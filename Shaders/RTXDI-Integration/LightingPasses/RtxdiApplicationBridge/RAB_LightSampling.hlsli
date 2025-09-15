#ifndef RAB_LIGHT_SAMPLING_HLSLI
#define RAB_LIGHT_SAMPLING_HLSLI

#include "RAB_RayPayload.hlsli"

float2 RAB_GetEnvironmentMapRandXYFromDir(float3 worldDir)
{
    float2 uv = directionToEquirectUV(worldDir); 
    uv.x -= g_Const.sceneConstants.environmentRotation;
    uv = frac(uv);
    return uv;
}

// Computes the probability of a particular direction being sampled from the environment map
// relative to all the other possible directions, based on the environment map pdf texture.
float RAB_EvaluateEnvironmentMapSamplingPdf(float3 L)
{
    if (!g_Const.restirDI.initialSamplingParams.environmentMapImportanceSampling)
        return 1.0;

    float2 uv = RAB_GetEnvironmentMapRandXYFromDir(L);

    uint2 pdfTextureSize = g_Const.environmentPdfTextureSize.xy;
    uint2 texelPosition = uint2(pdfTextureSize * uv);
    float texelValue = t_EnvironmentPdfTexture[texelPosition].r;
    
    int lastMipLevel = max(0, int(floor(log2(max(pdfTextureSize.x, pdfTextureSize.y)))));
    float averageValue = t_EnvironmentPdfTexture.mips[lastMipLevel][uint2(0, 0)].x;
    
    // The single texel in the last mip level is effectively the average of all texels in mip 0,
    // padded to a square shape with zeros. So, in case the PDF texture has a 2:1 aspect ratio,
    // that texel's value is only half of the true average of the rectangular input texture.
    // Compensate for that by assuming that the input texture is square.
    float sum = averageValue * square(1u << lastMipLevel);

    return texelValue / sum;
}

// Evaluates pdf for a particular light
float RAB_EvaluateLocalLightSourcePdf(uint lightIndex)
{
    uint2 pdfTextureSize = g_Const.localLightPdfTextureSize.xy;
    uint2 texelPosition = RTXDI_LinearIndexToZCurve(lightIndex);
    float texelValue = t_LocalLightPdfTexture[texelPosition].r;

    int lastMipLevel = max(0, int(floor(log2(max(pdfTextureSize.x, pdfTextureSize.y)))));
    float averageValue = t_LocalLightPdfTexture.mips[lastMipLevel][uint2(0, 0)].x;

    // See the comment at 'sum' in RAB_EvaluateEnvironmentMapSamplingPdf.
    // The same texture shape considerations apply to local lights.
    float sum = averageValue * square(1u << lastMipLevel);

    return texelValue / sum;
}

float3 RAB_GetReflectedRadianceForSurface(float3 incomingRadianceLocation, float3 incomingRadiance, RAB_Surface surface)
{
    float3 L = normalize(incomingRadianceLocation - surface.worldPos);
    float3 N = surface.normal;
    float3 V = surface.viewDir;

    if (dot(L, surface.geoNormal) <= 0)
        return 0;

    float d = Lambert(N, -L);
    float3 s;
    if (surface.material.roughness == 0)
        s = 0;
    else
        s = GGX_times_NdotL(V, L, N, max(surface.material.roughness, kMinRoughness), surface.material.specularF0);

    return incomingRadiance * (d * surface.material.diffuseAlbedo + s);
}

float RAB_GetReflectedLuminanceForSurface(float3 incomingRadianceLocation, float3 incomingRadiance, RAB_Surface surface)
{
    return RTXDI_Luminance(RAB_GetReflectedRadianceForSurface(incomingRadianceLocation, incomingRadiance, surface));
}

// Computes the weight of the given light samples when the given surface is
// shaded using that light sample. Exact or approximate BRDF evaluation can be
// used to compute the weight. ReSTIR will converge to a correct lighting result
// even if all samples have a fixed weight of 1.0, but that will be very noisy.
// Scaling of the weights can be arbitrary, as long as it's consistent
// between all lights and surfaces.
float RAB_GetLightSampleTargetPdfForSurface(RAB_LightSample lightSample, RAB_Surface surface)
{
    if (lightSample.solidAnglePdf <= 0)
        return 0;
    
    return RAB_GetReflectedLuminanceForSurface(lightSample.position, lightSample.radiance, surface) / lightSample.solidAnglePdf;
}

// Computes the weight of the given GI sample when the given surface is shaded using that GI sample.
float RAB_GetGISampleTargetPdfForSurface(float3 samplePosition, float3 sampleRadiance, RAB_Surface surface)
{
    float3 reflectedRadiance = RAB_GetReflectedRadianceForSurface(samplePosition, sampleRadiance, surface);

    return RTXDI_Luminance(reflectedRadiance);
}

void RAB_GetLightDirDistance(RAB_Surface surface, RAB_LightSample lightSample,
    out float3 o_lightDir,
    out float o_lightDistance)
{
    if (lightSample.lightType == PolymorphicLightType::kEnvironment)
    {
        o_lightDir = -lightSample.normal;
        o_lightDistance = DISTANT_LIGHT_DISTANCE;
    }
    else
    {
        float3 toLight = lightSample.position - surface.worldPos;
        o_lightDistance = length(toLight);
        o_lightDir = toLight / o_lightDistance;
    }
}

// Forward declare the SDK function that's used in RAB_AreMaterialsSimilar
bool RTXDI_CompareRelativeDifference(float reference, float candidate, float threshold);

float3 GetEnvironmentRadiance(float3 direction)
{
    if (!g_Const.sceneConstants.enableEnvironmentMap)
        return 0;

    Texture2D environmentLatLongMap = t_BindlessTextures[g_Const.sceneConstants.environmentMapTextureIndex];

    float2 uv = directionToEquirectUV(direction);
    uv.x -= g_Const.sceneConstants.environmentRotation;

    float3 environmentRadiance = environmentLatLongMap.SampleLevel(s_EnvironmentSampler, uv, 0).rgb;
    environmentRadiance *= g_Const.sceneConstants.environmentScale;

    return environmentRadiance;
}

bool IsComplexSurface(int2 pixelPosition, RAB_Surface surface)
{
    // Use a simple classification of surfaces into simple and complex based on the roughness.
    // The PostprocessGBuffer pass modifies the surface roughness and writes the DenoiserNormalRoughness
    // channel where the roughness is preserved. The roughness stored in the regular G-buffer is modified
    // based on the surface curvature around the current pixel. If the surface is curved, roughness increases.
    // Detect that increase here and disable permutation sampling based on a threshold.
    // Other classification methods can be employed for better quality.
    float originalRoughness = t_DenoiserNormalRoughness[pixelPosition].a;
    return originalRoughness < (surface.material.roughness * g_Const.restirDI.temporalResamplingParams.permutationSamplingThreshold);
}

uint getLightIndex(uint instanceID, uint geometryIndex, uint primitiveIndex)
{
    uint lightIndex = RTXDI_InvalidLightIndex;
    InstanceData hitInstance = t_InstanceData[instanceID];
    uint geometryInstanceIndex = hitInstance.firstGeometryInstanceIndex + geometryIndex;
    lightIndex = t_GeometryInstanceToLight[geometryInstanceIndex];
    if (lightIndex != RTXDI_InvalidLightIndex)
      lightIndex += primitiveIndex;
    return lightIndex;
}

// Return true if anything was hit. If false, RTXDI will do environment map sampling
// o_lightIndex: If hit, must be a valid light index for RAB_LoadLightInfo, if no local light was hit, must be RTXDI_InvalidLightIndex
// randXY: The randXY that corresponds to the hit location and is the same used for RAB_SamplePolymorphicLight
bool RAB_TraceRayForLocalLight(float3 origin, float3 direction, float tMin, float tMax,
    out uint o_lightIndex, out float2 o_randXY)
{
    o_lightIndex = RTXDI_InvalidLightIndex;
    o_randXY = 0;

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = tMin;
    ray.TMax = tMax;

    float2 hitUV;
    bool hitAnything;
#if USE_RAY_QUERY
    RayQuery<RAY_FLAG_CULL_NON_OPAQUE | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES> rayQuery;
    rayQuery.TraceRayInline(SceneBVH, RAY_FLAG_NONE, INSTANCE_MASK_OPAQUE, ray);
    rayQuery.Proceed();

    hitAnything = rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT;
    if (hitAnything)
    {
        o_lightIndex = getLightIndex(rayQuery.CommittedInstanceID(), rayQuery.CommittedGeometryIndex(), rayQuery.CommittedPrimitiveIndex());
        hitUV = rayQuery.CommittedTriangleBarycentrics();
    }
#else
    RayPayload payload = (RayPayload)0;
    payload.instanceID = ~0u;
    payload.throughput = 1.0;

    TraceRay(SceneBVH, RAY_FLAG_CULL_NON_OPAQUE | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, INSTANCE_MASK_OPAQUE, 0, 0, 0, ray, payload);
    hitAnything = payload.instanceID != ~0u;
    if (hitAnything)
    {
        o_lightIndex = getLightIndex(payload.instanceID, payload.geometryIndex, payload.primitiveIndex);
        hitUV = payload.barycentrics;
    }
#endif

    if (o_lightIndex != RTXDI_InvalidLightIndex)
    {
        o_randXY = randomFromBarycentric(hitUVToBarycentric(hitUV));
    }

    return hitAnything;
}

#endif // RAB_LIGHT_SAMPLING_HLSLI