#pragma once

#include <ShaderConstants.h>

namespace sys
{

	struct InstanceData
	{
		InstanceConstant instanceCst;
		struct GeometryData
		{
			Buffer* AS;
			VertexBuffer* VB;
			IndexBuffer* IB;
			U32				IndexStart;
			U32				IndexCount;
			U32				IndexType;
			U32				VertexStart;
			U32				VertexStride;
			PrimitiveType	PrimType;
			VertexDeclaration* Decl = nullptr;
		} geometry;
		struct ShadingData
		{
			Material* Mtl;
		} shading;
	};

	using InstanceDataDA = DynArray<InstanceData, 256>;

}
