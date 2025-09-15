#ifndef RAB_RT_SHADERS_HLSLI
#define RAB_RT_SHADERS_HLSLI

bool considerTransparentMaterial(uint instanceIndex, uint geometryIndex, uint triangleIndex, float2 rayBarycentrics, inout float3 throughput)
{
    GeometrySample gs = getGeometryFromHit(
        instanceIndex,
        geometryIndex,
        triangleIndex,
        rayBarycentrics,
        GeomAttr_TexCoord,
        t_InstanceData, t_GeometryData, t_MaterialConstants);
    
    MaterialSample ms = sampleGeometryMaterial(gs, 0, 0, 0,
        MatAttr_BaseColor | MatAttr_Transmission, s_MaterialSampler);

    bool alphaMask = ms.opacity >= gs.material.alphaCutoff;

    if (gs.material.domain == MaterialDomain_AlphaTested)
        return alphaMask;

    if (gs.material.domain == MaterialDomain_AlphaBlended)
    {
        throughput *= (1.0 - ms.opacity);
        return false;
    }

    if (gs.material.domain == MaterialDomain_Transmissive || 
        (gs.material.domain == MaterialDomain_TransmissiveAlphaTested && alphaMask) || 
        gs.material.domain == MaterialDomain_TransmissiveAlphaBlended)
    {
        throughput *= ms.transmission;

        if (ms.hasMetalRoughParams)
            throughput *= (1.0 - ms.metalness) * ms.baseColor;

        if (gs.material.domain == MaterialDomain_TransmissiveAlphaBlended)
            throughput *= (1.0 - ms.opacity);

        return all(throughput == 0);
    }

    return false;
}

#if !USE_RAY_QUERY
struct RayAttributes 
{
    float2 uv;
};

[shader("miss")]
void Miss(inout RayPayload payload : SV_RayPayload)
{
}

[shader("closesthit")]
void ClosestHit(inout RayPayload payload : SV_RayPayload, in RayAttributes attrib : SV_IntersectionAttributes)
{
    payload.committedRayT = RayTCurrent();
    payload.instanceID = InstanceID();
    payload.geometryIndex = GeometryIndex();
    payload.primitiveIndex = PrimitiveIndex();
    payload.frontFace = HitKind() == HIT_KIND_TRIANGLE_FRONT_FACE;
    payload.barycentrics = attrib.uv;
}

[shader("anyhit")]
void AnyHit(inout RayPayload payload : SV_RayPayload, in RayAttributes attrib : SV_IntersectionAttributes)
{
    if (!considerTransparentMaterial(InstanceID(), GeometryIndex(), PrimitiveIndex(), attrib.uv, payload.throughput))
        IgnoreHit();
}
#endif

#endif // RAB_RT_SHADERS_HLSLI