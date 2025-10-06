

#define HDR_MAX 50.0f

#define HISTOGRAM_GROUP_SIZE 16
#define HISTOGRAM_SIZE 128
groupshared uint lds_Histogram[HISTOGRAM_SIZE];


[numthreads(HISTOGRAM_GROUP_SIZE, HISTOGRAM_GROUP_SIZE, 1)]
void ComputeHistogram(uint2 did : SV_DispatchThreadID, uint gtid : SV_GroupIndex)
{
    // Initialize the local histogram data
    lds_Histogram[gtid] = 0;
    GroupMemoryBarrierWithGroupSync();

}