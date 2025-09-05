/* assimp include files. These three are usually needed. */
#include <SceneImporter.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <filesystem>

#include <Mesh.h>
#include <Bitmap.h>
#include <Material.h>

#pragma comment(lib, "../Tools/assimp/lib/x64/assimp-vc143-mt.lib")

std::string str_toupper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		// static_cast<int(*)(int)>(std::toupper)         // wrong
		// [](int c){ return std::toupper(c); }           // wrong
		// [](char c){ return std::toupper(c); }          // wrong
		[](unsigned char c) { return std::toupper(c); } // correct
	);
	return s;
}

void SceneImporter::LoadScene(std::string& filepath)
{
	std::string with_alias = "..\\GameDB\\" + filepath;
	std::filesystem::path directory(with_alias);

	std::string extension = str_toupper(directory.extension().string());
	if (extension.compare(".SDKMESH")==0)
	{
		Mesh* pMesh;
		pMesh = new Mesh();
		pMesh->Load(filepath.c_str());
		pMesh->SetScale(0.01f);
		pMesh->SetWorldPosition(60.5f * 0.01f, -128.f * 0.01f, -5.f * 0.01f);
		sys::RegisterGraphObject(pMesh);
	}
	else //ASSIMP
	{
		directory.remove_filename();

		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(with_alias,
			aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_GenUVCoords |
			aiProcess_TransformUVCoords |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate | aiProcess_SortByPType |
			aiProcess_GenBoundingBoxes |
			aiProcess_PreTransformVertices
		);

		if (pScene)
		{
			/*for (int i = 0; i < pScene->mNumMaterials; i++)
			{
				aiMaterial* pAiMaterial = pScene->mMaterials[i];
				for (int t = 0; t < pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE); t++)
				{
					aiString tex;
					{
						pAiMaterial->GetTexture(aiTextureType_DIFFUSE, t, &tex, nullptr, nullptr, nullptr, nullptr, nullptr);
						OutputDebugString(tex.C_Str());
						OutputDebugString("\n");
					}
					{
						pAiMaterial->GetTexture(aiTextureType_NORMALS, t, &tex, nullptr, nullptr, nullptr, nullptr, nullptr);
						OutputDebugString(tex.C_Str());
						OutputDebugString("\n");
					}
				}
			}*/

			for (int i = 0; i < pScene->mNumMeshes; i++)
			{
				aiMesh* pAiMesh = pScene->mMeshes[i];
				aiMaterial* pAiMaterial = pScene->mMaterials[pAiMesh->mMaterialIndex];

				// Add mesh to the global scene
				Mesh* pMesh = new Mesh();
				pMesh->LoadFromAiMesh(directory, pAiMesh, pAiMaterial);
				pMesh->SetWorldPosition(0, 0, 0);
				sys::RegisterGraphObject(pMesh);
			}

			if (pScene->HasCameras())
			{
				MESSAGE("Scene has %d cameras", pScene->mNumCameras);
				for (int i = 0; i < pScene->mNumCameras; i++)
				{
					auto pCamera = pScene->mCameras[i];
					MESSAGE("Camera %d %s", i, pCamera->mName.C_Str());
				}
			}

			MESSAGE("Succeeded loading %s", filepath.c_str());
		}
		else
		{
			MESSAGE("Failed loading %s", filepath.c_str());
		}
	}
}

void Mesh::LoadFromAiMesh(std::filesystem::path directory, aiMesh* importMesh, aiMaterial* importMaterial)
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
		float TBN[3][3];
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

			pVtx.TBN[0][0] = importMesh->mTangents[i].x;
			pVtx.TBN[0][1] = importMesh->mTangents[i].y;
			pVtx.TBN[0][2] = importMesh->mTangents[i].z;
			pVtx.TBN[1][0] = importMesh->mBitangents[i].x;
			pVtx.TBN[1][1] = importMesh->mBitangents[i].y;
			pVtx.TBN[1][2] = importMesh->mBitangents[i].z;
			pVtx.TBN[2][0] = importMesh->mNormals[i].x;
			pVtx.TBN[2][1] = importMesh->mNormals[i].y;
			pVtx.TBN[2][2] = importMesh->mNormals[i].z;

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
		// TANGENT
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, TBN[0]);
		pElt->Type = DECL_FMT_FLOAT3;
		pElt->Semantic = DECL_TANGENT;
		pElt++;
		// BINORMAL
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, TBN[1]);
		pElt->Type = DECL_FMT_FLOAT3;
		pElt->Semantic = DECL_BINORMAL;
		pElt++;
		// NORMAL
		pElt->StreamId = 0;
		pElt->Offset = offsetof(MeshVertex, TBN[2]);
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

		auto LoadBitmap = [&](MaterialStage stage)
			{
				std::string tex;

				aiTextureType type = aiTextureType_NONE;
				switch (stage)
				{
				case MTL_STAGE_ALBEDO:		type = aiTextureType_DIFFUSE; break;
				case MTL_STAGE_NORMAL:		type = aiTextureType_NORMALS; break;
				case MTL_STAGE_ROUGHNESS:	type = aiTextureType_SPECULAR; break;
				default:
					return;
				};

				aiString aitex;
				importMaterial->GetTexture(type, 0, &aitex, nullptr, nullptr, nullptr, nullptr, nullptr);

				std::filesystem::path path(directory);
				path.append(aitex.C_Str());
				path.replace_extension(std::string("dds"));

				Bitmap* bm = nullptr;
				asset::Cache& asset = asset::Cache::GetInstance();
				tex = path.string();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP, tex.c_str(), (GraphObject**)&bm);
				if (bNew)
				{
					if (bm->LoadDDS(tex.c_str()))
					{
						gData.Rdr->CreateTexture(bm);
						pMat->SetBitmap(bm, stage);
					}					
				}				
			};

		LoadBitmap(MTL_STAGE_ALBEDO);
		LoadBitmap(MTL_STAGE_NORMAL);
		LoadBitmap(MTL_STAGE_ROUGHNESS);

		auto LoadBitmapFromPath = [&](std::string& tex, MaterialStage stage)
			{
				Bitmap* bm = nullptr;
				asset::Cache& asset = asset::Cache::GetInstance();
				Bool bNew = asset.LoadAsset(asset::Type::BITMAP, tex.c_str(), (GraphObject**)&bm);
				if (bNew)
				{
					if (bm->LoadDDS(tex.c_str()))
					{
						gData.Rdr->CreateTexture(bm);
						pMat->SetBitmap(bm, stage);
					}
				}				
			};

		// dummy texture
		if (!pMat->GetBitmap(MTL_STAGE_ALBEDO))
		{
			LoadBitmapFromPath(std::string("..\\GameDB\\assets\\models\\sponza\\spnza_bricks_a_diff.dds"), MTL_STAGE_ALBEDO);
		}
		if (!pMat->GetBitmap(MTL_STAGE_ROUGHNESS))
		{
			LoadBitmapFromPath(std::string("..\\GameDB\\assets\\models\\sponza\\spnza_bricks_a_diff.dds"), MTL_STAGE_ROUGHNESS);
		}
	}
}