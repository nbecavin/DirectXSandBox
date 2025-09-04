#ifndef __MESH_HH__
#define __MESH_HH__

#include <GraphObject.h>
#include <Renderer.h>

class aiMesh;
class aiMaterial;

class Mesh : public GraphObject
{
public:
	Mesh();
	virtual ~Mesh();

	virtual void Update(F32 dTime);
	virtual void Draw();
	virtual void DrawGBuffer();

public:
	void Load(const char * fname);
	void LoadFromAiMesh(std::filesystem::path directory, aiMesh* importMesh, aiMaterial* importMaterial);

	struct MeshSubSet {
		VertexBuffer *	VB;
		IndexBuffer *	IB;
		U32				MtlId;
		Mat4x4			Pos;
		U32				Stride;

		U32				IndexStart;
		U32				IndexCount;
		U32				IndexType;
		U32				VertexStart;
		U32				VertexCount;
		PrimitiveType	PrimType;

		VertexDeclaration * Decl;
	};
	DynArray<MeshSubSet,8>	SubSetsDA;
	DynArray<Material,8>	MtlDA;
};

#endif //__MESH_HH__
