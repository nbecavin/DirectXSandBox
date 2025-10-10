
#if NON_PATH_TRACING_PASS || defined(__cplusplus) || (__SHADER_TARGET_MAJOR < 6 || __SHADER_TARGET_MINOR < 8)
#	define RAYPAYLOAD_QUALIFIER
#	define RAYPAYLOAD_FIELD_QUALIFIER
#else
#	define RAYPAYLOAD_QUALIFIER        [raypayload] 
#	define RAYPAYLOAD_FIELD_QUALIFIER  : read(caller, anyhit, closesthit, miss) : write(caller, anyhit, closesthit, miss)
#endif

//#define RAYGEN_ENTRYPOINT(Name) \
//		[shader("raygeneration")] \
//		void Name()
//
//#define CLOSEST_HIT_ENTRYPOINT(Name, Pay, Attribute) \
//		[shader("closesthit")] \
//		void Name(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
//
//#define ANY_HIT_ENTRYPOINT(Name, Pay, Attribute) \
//		[shader("anyhit")] \
//		void Name(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
//
//#define MISS_ENTRYPOINT(Name, Pay) \
//		[shader("miss")] \
//		void Name(inout RayPayload Pay)
//
//#define INTERSECTION_ENTRYPOINT(Name) \
//		[shader("intersection")] \
//		void Name()
//
//#define CALLABLE_ENTRYPOINT(Name, Pay) \
//		[shader("callable")] \
//		void Name(inout CallablePayload Pay)

struct RAYPAYLOAD_QUALIFIER RayPayload
{
	float tHit RAYPAYLOAD_FIELD_QUALIFIER;
};

[shader("miss")] \
void Miss(inout RayPayload Pay)
{
}

[shader("anyhit")] \
void BaseAHS(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
}

[shader("closesthit")] \
void BaseCHS(inout RayPayload Pay, in BuiltInTriangleIntersectionAttributes Attribute)
{
}

[shader("raygeneration")]
void main()
{

}
