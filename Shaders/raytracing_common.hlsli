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

float Interpolate(float vertices[3], float3 bary)
{
	return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

float2 Interpolate(float2 vertices[3], float3 bary)
{
	return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

float3 Interpolate(float3 vertices[3], float3 bary)
{
	return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

float4 Interpolate(float4 vertices[3], float3 bary)
{
	return vertices[0] * bary[0] + vertices[1] * bary[1] + vertices[2] * bary[2];
}

//
// Inline Raytracing

template<typename TPayload, typename TCallbackHandler>
void TraceRayInline(RaytracingAccelerationStructure TLAS, uint flags, RayDesc ray, inout TPayload pay, TCallbackHandler callbacks)
{
	RayQuery<RAY_FLAG_NONE, RAYQUERY_FLAG_NONE> q;
	q.TraceRayInline(TLAS, flags, -1, ray);
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

//
//

struct GeometrySampler
{
	float3 position;
	float2 texcoord0;
	float3 shading_normal;
	float3 geometry_normal;
};

GeometrySampler GetGeometrySample(HitAttributes h)
{
	GeometrySampler gs = (GeometrySampler) 0;
	
	float3 b = GetBarycentrics(h.Barycentrics);
	
	uint3 indices = 0;
	
	float3 vtx_position[3];
	vtx_position[0] = float3(1, 0, 1);
	vtx_position[1] = float3(0, 1, 0);
	vtx_position[2] = float3(0, 0, 1);
	gs.position = Interpolate(vtx_position, b);
	
	float3 vtx_texcoord0[3];
	vtx_texcoord0[0] = 0;
	vtx_texcoord0[1] = 0;
	vtx_texcoord0[2] = 0;
	gs.texcoord0 = Interpolate(vtx_texcoord0, b);
	
	return gs;
}

#endif //__RAYTRACING_COMMON_HH__
