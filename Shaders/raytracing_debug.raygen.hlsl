#if defined(COMPUTE_SHADER)
#	define USE_INLINE_RAYTRACING 1
#else
#	define USE_INLINE_RAYTRACING 0
#endif

#include "sh_const.h"

#define TMIN_EPSILON	 10e-4
#define TMAX_INF	 10e9

RaytracingAccelerationStructure TLAS;

RWTexture2D<float4> Output : register(u0);

struct RayPayload
{
	float HitT;
};

#if USE_INLINE_RAYTRACING

[numthreads(8, 8, 1)]
void main(uint2 did : SV_DispatchThreadID)
{
	float3 color = Output[did].xyz;

	color *= float3(1,0,1);

	float2 uvViewport = (float2)did.xy / float2(1920,1080);//DispatchRaysDimensions();
	float2 ndcViewport = UVToNDC(uvViewport);

	float3 viewDir = normalize(float3(mul(float4(ndcViewport, 0, 1), Camera.invProjMatrix).xy, 1.f));
	//float3 viewDir = normalize(mul(float4(ndcViewport, 0, 1), Camera.invProjMatrix).xyz);
	float3 worldDir = mul(viewDir, Camera.invViewMatrix);

	RayDesc ray;
	ray.TMin = 0;
	ray.TMax = TMAX_INF;
	ray.Origin = Camera.eyeWorld;
	ray.Direction = worldDir;

//ray.Direction = float3(0,1,0);
//ray.Origin = float3(0,0,0);

	RayQuery<RAY_FLAG_NONE, RAYQUERY_FLAG_NONE> q;
	q.TraceRayInline(TLAS, RAY_FLAG_NONE, -1, ray);
	while(q.Proceed())
    {
        switch(q.CandidateType())
        {
		case CANDIDATE_NON_OPAQUE_TRIANGLE:
			q.CommitNonOpaqueTriangleHit(); //Trivially accept for now
			break;
		}
	}
	if(q.CommittedStatus()==COMMITTED_TRIANGLE_HIT) //Closest
	{
		float2 b = q.CommittedTriangleBarycentrics();
		color = float3(1 - b.x - b.y, b.x, b.y);
	}
	else //COMMITTED_NOTHING //Miss
	{
		color = float3(1,0,0);
	}

	//color = worldDir;

	//Output[did] = float4(color, 1);
}

#else

[shader("miss")]\
void Miss(inout RayPayload Pay)
{
}

[shader("anyhit")]\
void BaseAHS(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
}

[shader("closesthit")]\
void BaseCHS(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
	Pay.HitT = RayTCurrent();
}

[shader("raygeneration")]
void main()
{
	uint2 did = DispatchRaysIndex().xy;
	
	float3 color = 0;
	
	RayPayload pay = (RayPayload)0;

	RayDesc ray;
	ray.TMin = 0;
	ray.TMax = 1000000;
	ray.Direction = float3(0, 1, 0);
	ray.Origin = float3(0, 0, 0);
	
	TraceRay(TLAS, RAY_FLAG_NONE, -1, 0, 1, 0, ray, pay);
	
	Output[did] = float4(color, 1);
}

#endif
