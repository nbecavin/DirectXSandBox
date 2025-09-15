#ifndef RAB_RAY_PAYLOAD_HLSLI
#define RAB_RAY_PAYLOAD_HLSLI

struct RayPayload
{
    float3 throughput;
    float committedRayT;
    uint instanceID;
    uint geometryIndex;
    uint primitiveIndex;
    bool frontFace;
    float2 barycentrics;
};

#endif // RAB_RAY_PAYLOAD_HLSLI