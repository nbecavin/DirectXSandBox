#if defined(COMPUTE_SHADER)
#	define USE_INLINE_RAYTRACING 1
#else
#	define USE_INLINE_RAYTRACING 0
#endif

#if !defined(USE_INLINE_RAYTRACING)
#define USE_INLINE_RAYTRACING 1
#endif

#if __INTELLISENSE__
#define USE_INLINE_RAYTRACING 1
#endif

#include "sh_const.h"
#include "raytracing_common.hlsli"
#include "gbuffer.hlsli"

#define TMIN_EPSILON	10e-4
#define TMAX_INF		10e9

RaytracingAccelerationStructure TLAS;

RWTexture2D<float> Output : register(u0);

struct RayPayload
{
	float HitT;
	float3 color;
};

void OnAnyHit(HitAttributes h, inout RayPayload pay)
{
}

void OnClosestHit(HitAttributes h, inout RayPayload pay)
{
	GeometrySampler gs = GetGeometrySample(h);
	pay.HitT = h.HitT;
	pay.color = gs.position;
}

void OnMiss(inout RayPayload pay)
{
	pay.HitT = -1;
	pay.color = float3(1, 0, 0);
}

static DefaultCallbackHandler callbacks;

#if USE_INLINE_RAYTRACING

[numthreads(8, 8, 1)]
void main(uint2 did : SV_DispatchThreadID)
{
	float shadow = 1.0;

	float2 uvViewport = (float2) did.xy / float2(1920, 1080); //DispatchRaysDimensions();
	float2 ndcViewport = UVToNDC(uvViewport);

	float3 viewDir = normalize(float3(mul(float4(ndcViewport, 0, 1), Camera.invProjMatrix).xy, 1.f));
	//float3 viewDir = normalize(mul(float4(ndcViewport, 0, 1), Camera.invProjMatrix).xyz);
	float3 worldDir = normalize(mul(viewDir, Camera.invViewMatrix));

	GBuffer gbuffer;
	gbuffer.Unpack(uvViewport);
	
	RayDesc ray;
	ray.TMin = 0;
	ray.TMax = TMAX_INF;
	ray.Origin = Camera.eyeWorld;
	ray.Direction = worldDir;

	RayPayload pay = (RayPayload)0;
	TraceRayInline(TLAS, RAY_FLAG_NONE, ray, pay, callbacks);
	
	if (pay.HitT > 0)
	{		
		ray.TMin = 0.01;
		ray.Origin = ray.Origin + ray.Direction * (pay.HitT - TMIN_EPSILON);
		ray.Direction = DLIGHT_DIR;
		
		pay = (RayPayload)0;
		TraceRayInline(TLAS, RAY_FLAG_NONE, ray, pay, callbacks);
		
		shadow = (pay.HitT > 0) ? 0 : 1;
	}

	Output[did] = shadow;
}

#else

[shader("miss")]\
void Miss(inout RayPayload pay)
{
	callbacks.OnMissCallback(pay);
}

[shader("anyhit")]\
void BaseAHS(inout RayPayload pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
}

[shader("closesthit")]\
void BaseCHS(inout RayPayload pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
	pay.HitT = RayTCurrent();
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
