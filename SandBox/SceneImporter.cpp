/* assimp include files. These three are usually needed. */
#include <SceneImporter.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <Mesh.h>

#pragma comment(lib, "../Tools/assimp/lib/x64/assimp-vc143-mt.lib")

void SceneImporter::LoadScene(std::string& filepath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenBoundingBoxes
	);

	if (pScene)
	{
		OutputDebugString("Succeeded loading\n");

		for (int i = 0; i< pScene->mNumMeshes; i++)
		{
			aiMesh* pAiMesh = pScene->mMeshes[i];
			//pAiMesh->mFaces->mIndices

			// Add mesh to the global scene
			Mesh* pMesh = new Mesh();
			pMesh->SetName(std::string(pAiMesh->mName.C_Str()));
			pMesh->SetWorldPosition(0, 0, 0);
			sys::RegisterGraphObject(pMesh);

//			pMesh2->Load("assets\\models\\sponza\\flag.sdkmesh");

		}
	}
	else
	{
		OutputDebugString("Failed\n");
	}
}
