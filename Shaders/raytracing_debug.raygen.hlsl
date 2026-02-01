#if defined(COMPUTE_SHADER)
#	define USE_INLINE_RAYTRACING 1
#else
#	define USE_INLINE_RAYTRACING 0
#endif

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

	RayDesc ray;
	ray.TMin = 0;
	ray.TMax = 1000000;
	ray.Direction = float3(0,1,0);
	ray.Origin = float3(0,0,0);

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
	switch(q.CommittedStatus())
    {
	case COMMITTED_TRIANGLE_HIT: //Closest
		color = float3(0,1,0);
		break;
	case COMMITTED_NOTHING: //Miss
		color = float3(1,0,0);
		break;
	}

	Output[did] = float4(color, 1);
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
