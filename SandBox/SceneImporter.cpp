/* assimp include files. These three are usually needed. */
#include <SceneImporter.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <Mesh.h>
#include <Bitmap.h>
#include <Material.h>

#pragma comment(lib, "../Tools/assimp/lib/x64/assimp-vc143-mt.lib")

void SceneImporter::LoadScene(std::string& filepath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate | aiProcess_SortByPType |
		aiProcess_GenBoundingBoxes |
		aiProcess_PreTransformVertices
	);

	if (pScene)
	{
		OutputDebugString("Succeeded loading\n");

		for (int i = 0; i < pScene->mNumMaterials; i++)
		{
			aiMaterial* pAiMaterial = pScene->mMaterials[i];
			for (int t = 0; t < pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE); t++)
			{
				aiString tex;
				pAiMaterial->GetTexture(aiTextureType_DIFFUSE, t, &tex, nullptr, nullptr, nullptr, nullptr, nullptr);
				OutputDebugString(tex.C_Str());
				OutputDebugString("tex\n");
			}
		}

		for (int i = 0; i< pScene->mNumMeshes; i++)
		{
			aiMesh* pAiMesh = pScene->mMeshes[i];

			// Add mesh to the global scene
			Mesh* pMesh = new Mesh();
			pMesh->LoadFromAiMesh(pAiMesh);
			pMesh->SetWorldPosition(0, 0, 0);
			sys::RegisterGraphObject(pMesh);
		}
	}
	else
	{
		OutputDebugString("Failed\n");
	}
}

void Mesh::LoadFromAiMesh(aiMesh* importMesh)
{
	SetName(std::string(importMesh->mName.C_Str()));

	// One subset per mesh
	MeshSubSet* pItem = SubSetsDA.Add();

	// Create IBs - 32 bits
	U32 numIndices = importMesh->mNumFaces * 3;
	U32 sizeBytes = numIndices * 4;
	U32* pIndices = new U32[numIndices];
	{
		//fill
		U32* pWrite = pIndices;
		for (int i = 0; i < importMesh->mNumFaces; i++)
		{
			auto& it = importMesh->mFaces[i];
			pWrite[0] = it.mIndices[0];
			pWrite[1] = it.mIndices[1];
			pWrite[2] = it.mIndices[2];
			pWrite += 3;
		}
		pItem->IB = gData.Rdr->CreateIndexBuffer(sizeBytes, 0, FMT_IDX_32, pIndices);

		pItem->IndexStart = (U32)0;
		pItem->IndexCount = (U32)numIndices; //Trilist
		pItem->IndexType = FMT_IDX_32;
	}

	struct MeshVertex
	{
		float pos[3];
		float nrm[3];
		float uv[2];
	};

	// Create VBs
	U32 vertexStride = sizeof(MeshVertex);
	U32 vertexByteSize = vertexStride * importMesh->mNumVertices;
	{
		pItem->VertexStart = (U32)0;
		pItem->VertexCount = (U32)importMesh->mNumVertices;
		pItem->PrimType = PRIM_TRIANGLELIST;

		// Create VBs
		MeshVertex* pVertices = new MeshVertex[pItem->VertexCount];
		for (int i = 0; i < importMesh->mNumVertices; i++)
		{
			MeshVertex& pVtx = pVertices[i];

			pVtx.pos[0] = importMesh->mVertices[i].x;
			pVtx.pos[1] = importMesh->mVertices[i].y;
			pVtx.pos[2] = importMesh->mVertices[i].z;

			pVtx.nrm[0] = importMesh->mNormals[i].x;
			pVtx.nrm[1] = importMesh->mNormals[i].y;
			pVtx.nrm[2] = importMesh->mNormals[i].z;

			pVtx.uv[0] = importMesh->mTextureCoords[0][i].x;
			pVtx.uv[1] = importMesh->mTextureCoords[0][i].y;
		}
		//	UINT byteSize = (UINT)m_pVertexBufferArray[i].SizeBytes;
		pItem->VB = gData.Rdr->CreateVertexBuffer(vertexByteSize, 0, pVertices);
	}

	// Vertex Declaration
	{
		VertexElement DeclDesc[32];
		VertexElement* pElt = DeclDesc;

		// POSITION0
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, pos);
		pElt->Type = DECL_FMT_FLOAT3;
		pElt->Semantic = DECL_POSITION + 0;
		pElt++;
		// NORMAL
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, nrm);
		pElt->Type = DECL_FMT_FLOAT3;
		pElt->Semantic = DECL_NORMAL;
		pElt++;
		// TEXCOORD0
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, uv);
		pElt->Type = DECL_FMT_FLOAT2;
		pElt->Semantic = DECL_TEXCOORD0 + 0;
		pElt++;
		//
		pElt[0].END();

		pItem->Decl = gData.Rdr->CreateVertexDecl(DeclDesc, SHADER_VS_BASE_MESH);
		pItem->Stride = vertexStride;
		pItem->MtlId = 0;// pSubSet->MaterialID;
	}

	// dummy material
	{
		const std::string aliaspath = "..\\GameDB";
		Material* pMat = MtlDA.Add();

		std::string albedo, normal;
		Bitmap* bm;

		auto LoadBitmap = [&](std::string& tex, MaterialStage stage)
			{
				Bitmap* bm = nullptr;
				asset::Cache& asset = asset::Cache::GetInstance();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP, tex.c_str(), (GraphObject**)&bm);
				if (bNew)
				{
					bm->LoadDDS(tex.c_str());
					gData.Rdr->CreateTexture(bm);
				}
				pMat->SetBitmap(bm, stage);
			};

		LoadBitmap(std::string("..\\GameDB\\assets\\models\\sponza\\spnza_bricks_a_diff.dds"), MTL_STAGE_DIFFUSE);
		//LoadBitmap(std::string("..\\GameDB\\assets\\models\\sponza\\spnza_bricks_a_normal.dds"), MTL_STAGE_NORMAL);
		LoadBitmap(std::string("..\\GameDB\\assets\\models\\sponza\\spnza_bricks_a_spec.DDS"), MTL_STAGE_SPEC);
	}
}