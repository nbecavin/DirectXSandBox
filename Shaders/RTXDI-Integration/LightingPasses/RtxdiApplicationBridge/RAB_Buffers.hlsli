#ifndef RAB_BUFFER_HLSLI
#define RAB_BUFFER_HLSLI

// G-buffer resources
Texture2D<float> t_GBufferDepth : register(t0);
Texture2D<uint> t_GBufferNormals : register(t1);
Texture2D<uint> t_GBufferGeoNormals : register(t2);
Texture2D<uint> t_GBufferDiffuseAlbedo : register(t3);
Texture2D<uint> t_GBufferSpecularRough : register(t4);
Texture2D<float> t_PrevGBufferDepth : register(t5);
Texture2D<uint> t_PrevGBufferNormals : register(t6);
Texture2D<uint> t_PrevGBufferGeoNormals : register(t7);
Texture2D<uint> t_PrevGBufferDiffuseAlbedo : register(t8);
Texture2D<uint> t_PrevGBufferSpecularRough : register(t9);
Texture2D<float2> t_PrevRestirLuminance : register(t10);
Texture2D<float4> t_MotionVectors : register(t11);
Texture2D<float4> t_DenoiserNormalRoughness : register(t12);

// Scene resources
RaytracingAccelerationStructure SceneBVH : register(t30);
RaytracingAccelerationStructure PrevSceneBVH : register(t31);
StructuredBuffer<InstanceData> t_InstanceData : register(t32);
StructuredBuffer<GeometryData> t_GeometryData : register(t33);
StructuredBuffer<MaterialConstants> t_MaterialConstants : register(t34);

// RTXDI resources
StructuredBuffer<PolymorphicLightInfo> t_LightDataBuffer : register(t20);
Buffer<float2> t_NeighborOffsets : register(t21);
Buffer<uint> t_LightIndexMappingBuffer : register(t22);
Texture2D t_EnvironmentPdfTexture : register(t23);
Texture2D t_LocalLightPdfTexture : register(t24);
StructuredBuffer<uint> t_GeometryInstanceToLight : register(t25);

// Screen-sized UAVs
RWStructuredBuffer<RTXDI_PackedDIReservoir> u_LightReservoirs : register(u0);
RWTexture2D<float4> u_DiffuseLighting : register(u1);
RWTexture2D<float4> u_SpecularLighting : register(u2);
RWTexture2D<int2> u_TemporalSamplePositions : register(u3);
RWTexture2DArray<float4> u_Gradients : register(u4);
RWTexture2D<float2> u_RestirLuminance : register(u5);
RWStructuredBuffer<RTXDI_PackedGIReservoir> u_GIReservoirs : register(u6);

// RTXDI UAVs
RWBuffer<uint2> u_RisBuffer : register(u10);
RWBuffer<uint4> u_RisLightDataBuffer : register(u11);
RWBuffer<uint> u_RayCountBuffer : register(u12);
RWStructuredBuffer<SecondaryGBufferData> u_SecondaryGBuffer : register(u13);

// Other
ConstantBuffer<ResamplingConstants> g_Const : register(b0);
VK_PUSH_CONSTANT ConstantBuffer<PerPassConstants> g_PerPassConstants : register(b1);
SamplerState s_MaterialSampler : register(s0);
SamplerState s_EnvironmentSampler : register(s1);

#define RTXDI_RIS_BUFFER u_RisBuffer
#define RTXDI_LIGHT_RESERVOIR_BUFFER u_LightReservoirs
#define RTXDI_NEIGHBOR_OFFSETS_BUFFER t_NeighborOffsets
#define RTXDI_GI_RESERVOIR_BUFFER u_GIReservoirs

#define IES_SAMPLER s_EnvironmentSampler

// Translates the light index from the current frame to the previous frame (if currentToPrevious = true)
// or from the previous frame to the current frame (if currentToPrevious = false).
// Returns the new index, or a negative number if the light does not exist in the other frame.
int RAB_TranslateLightIndex(uint lightIndex, bool currentToPrevious)
{
    // In this implementation, the mapping buffer contains both forward and reverse mappings,
    // stored at different offsets, so we don't care about the currentToPrevious parameter.
    uint mappedIndexPlusOne = t_LightIndexMappingBuffer[lightIndex];

    // The mappings are stored offset by 1 to differentiate between valid and invalid mappings.
    // The buffer is cleared with zeros which indicate an invalid mapping.
    // Subtract that one to make this function return expected values.
    return int(mappedIndexPlusOne) - 1;
}

#endif // RAB_BUFFER_HLSLI