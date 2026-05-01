#pragma once
#ifndef __RAYTRACING_COMMON_HH__
#define __RAYTRACING_COMMON_HH__

struct HitAttributes
{
	float HitT;
	float2 Barycentrics;
};

struct DefaultCallbackHandler
{
	template<typename TRayPayload>
	void OnAnyHitCallback(HitAttributes h, inout TRayPayload pay)
	{
		OnAnyHit(h, pay);
	};
	
	template<typename TRayPayload>
	void OnClosestHitCallback(HitAttributes h, inout TRayPayload pay)
	{
		OnClosestHit(h, pay);
	};
	
	template<typename TRayPayload>
	void OnMissCallback(TRayPayload pay)
	{
		OnMiss(pay);
	}
};

float3 GetBarycentrics(float2 b)
{
	return float3(1 - b.x - b.y, b.x, b.y);
}

//
// Inline Raytracing

template<typename TPayload, typename TCallbackHandler>
void TraceRayInline(RaytracingAccelerationStructure TLAS, RayDesc ray, inout TPayload pay, TCallbackHandler callbacks)
{
	RayQuery<RAY_FLAG_NONE, RAYQUERY_FLAG_NONE> q;
	q.TraceRayInline(TLAS, RAY_FLAG_NONE, -1, ray);
	while (q.Proceed())
	{
		if (q.CandidateType() == CANDIDATE_NON_OPAQUE_TRIANGLE)
		{
			HitAttributes h;
			h.HitT = q.CandidateTriangleRayT();
			h.Barycentrics = q.CandidateTriangleBarycentrics();		
			callbacks.OnAnyHitCallback(h, pay);
			q.CommitNonOpaqueTriangleHit(); //Trivially accept for now
		}
	}
	if (q.CommittedStatus() == COMMITTED_TRIANGLE_HIT) //Closest
	{
		HitAttributes h;
		h.HitT = q.CommittedRayT();
		h.Barycentrics = q.CommittedTriangleBarycentrics();
		callbacks.OnClosestHitCallback(h, pay);
	}
	else //COMMITTED_NOTHING //Miss
	{
		callbacks.OnMissCallback(pay);
	}
}

#endif //__RAYTRACING_COMMON_HH__
