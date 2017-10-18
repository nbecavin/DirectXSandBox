#include "StdAfx.h"
#include "FBXManager.h"

//fbxsdk-2012.2-mdd.lib;

CFBXManager::CFBXManager(void)
{
}

CFBXManager::~CFBXManager(void)
{
}

void CFBXManager::Init()
{
	/*
	m_SdkManager = KFbxSdkManager::Create();

    // Create the io settings object.
	KFbxIOSettings *ios = KFbxIOSettings::Create(m_SdkManager, IOSROOT);
	m_SdkManager->SetIOSettings(ios);
	
	// Create an importer using our sdk manager.
	KFbxImporter* lImporter = KFbxImporter::Create(m_SdkManager,"");
	
	const char* lFilename = "file.fbx";

	// Use the first argument as the filename for the importer.
	if(!lImporter->Initialize(lFilename, -1, m_SdkManager->GetIOSettings()))
	{
	//	printf("Call to KFbxImporter::Initialize() failed.\n");
	//	printf("Error returned: %s\n\n", lImporter->GetLastErrorString());
	//	exit(-1);
	}*/

}
