#ifndef RTXDI_RAB_RANDOM_SAMPLER_STATE_HLSLI
#define RTXDI_RAB_RANDOM_SAMPLER_STATE_HLSLI

#include "../../HelperFunctions.hlsli"

typedef RandomSamplerState RAB_RandomSamplerState;

// Initialized the random sampler for a given pixel or tile index.
// The pass parameter is provided to help generate different RNG sequences
// for different resampling passes, which is important for image quality.
// In general, a high quality RNG is critical to get good results from ReSTIR.
// A table-based blue noise RNG dose not provide enough entropy, for example.
RAB_RandomSamplerState RAB_InitRandomSampler(uint2 index, uint pass)
{
    return initRandomSampler(index, g_Const.frameIndex + pass * 13);
}

// Draws a random number X from the sampler, so that (0 <= X < 1).
float RAB_GetNextRandom(inout RAB_RandomSamplerState rng)
{
    return sampleUniformRng(rng);
}

#endif // RTXDI_RAB_RANDOM_SAMPLER_STATE_HLSLI