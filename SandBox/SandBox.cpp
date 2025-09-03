#include "SandBox.h"
#if defined(_PC)
#include <WinMain.h>
#endif
#if defined(_ANDROID)
#include <AndroidMain.h>
#endif
#include <Renderer.h>
#include <InputManager.h>
#include <Sky.h>
#include <Terrain.h>
#include <Proc.h>
#include <Mesh.h>
#include <CameraFree.h>

#include <SceneImporter.h>

sys::Globals gData;

void sys::Globals::Init()
{
	m_bExitStatus = FALSE;
}

void sys::Globals::Exit()
{
	m_bExitStatus = TRUE;
}

Bool sys::Globals::GetExitStatus()
{
	return m_bExitStatus;
}

void sys::Init()
{
	gData.Init();

	InitTimer();
	LOWLEVELINIT();
}

void sys::Shut()
{
	LOWLEVELSHUT();
}

void sys::EndFrame()
{
	LOWLEVELENDFRAME();
}

void sys::MainLoop()
{
	float StartTime = GetAbsoluteTime();
	gData.DeltaTime = 0.016f;

MESSAGE("Entering mainloop");

//super temp
	gData.Rdr->SetCamera(new Camera());

	Camera * cam = gData.Rdr->GetCamera();
	//cam->SetWorldPosition(Vec4f(0.f,2.f,-10.f,1.f));
	cam->SetWorldPosition(Vec4f(11.5, -4.2f, 0.349f, 1.f));
	//cam->SetWorldTarget(Vec4f(0.f,0.f,0.f,1.f));
	cam->SetWorldTarget(Vec4f(10.5f, -4.07f, 0.362f, 1.f));

	RegisterGraphObject(new Sky());
	//RegisterGraphObject(new Terrain());
	//RegisterGraphObject(new Proc(sphere));
	//RegisterGraphObject(new Proc(torus));

MESSAGE("Load some mesh");

	//
	SceneImporter imp;
//	imp.LoadScene(std::string("..\\GameDB\\assets\\Bistro_v5_2\\BistroInterior.fbx"));
	imp.LoadScene(std::string("..\\GameDB\\assets\\bistro\\bistro.gltf"));
//	imp.LoadScene(std::string("..\\GameDB\\assets\\sponza\\sponza.gltf"));

	Mesh * pMesh;
	pMesh = new Mesh();
	//pMesh->Load("assets\\models\\powerplant.sdkmesh");
	//pMesh->Load("assets\\models\\MicroscopeCity\\scanner.sdkmesh");
	//pMesh->Load("assets\\models\\MicroscopeCity\\column.sdkmesh");
	//pMesh->Load("assets\\models\\Dwarf\\dwarf.sdkmesh");
	pMesh->Load("assets\\models\\sponza\\SponzaNoFlag.sdkmesh");
	pMesh->SetScale(0.01f);
	pMesh->SetWorldPosition(60.5f*0.01f,-128.f*0.01f,-5.f*0.01f);
	RegisterGraphObject(pMesh);

	Mesh * pMesh2;
	pMesh2 = new Mesh();
	//pMesh2->Load("assets\\models\\TorusKnot\\tk.sdkmesh");
	pMesh2->Load("assets\\models\\sponza\\flag.sdkmesh");
	pMesh2->SetScale(0.01f);
	pMesh->SetWorldPosition(-24.f*0.01f,-71.f*0.01f,9.6f*0.01f);
	RegisterGraphObject(pMesh2);

	CameraFree * pCameraScript = new CameraFree;
	RegisterScriptObject(pCameraScript);

//

	while(gData.GetExitStatus()==FALSE)
	{
		float dTime = GetDeltaTime();
		gData.Rdr->BeginFrame(dTime);

		// Get new input from external device
		gData.Input->Update(dTime);

		UpdateEditorMenu();

		// Update script
		for(U32 i=0;i<gData.m_ScriptObjectDA.GetSize();i++)
		{
			ScriptObject * it = gData.m_ScriptObjectDA[i];
			it->Update(dTime);
		}

		// Update Actors and modifiers
		for(U32 i=0;i<gData.m_GraphObjectDA.GetSize();i++)
		{
			GraphObject * it = gData.m_GraphObjectDA[i];
			it->Update(dTime);
		}

		// Submit to display
		gData.Rdr->MainLoop();

//		MESSAGE( "FPS %f\n", 1.f/GetDeltaTime() );

		sys::EndFrame();

		float CurrentTime = GetAbsoluteTime();
		gData.DeltaTime = CurrentTime - StartTime;
//MESSAGE("In mainloop %f %f", gData.DeltaTime, CurrentTime);
		gData.DeltaTime = max(gData.DeltaTime,0.0001f);
		StartTime = CurrentTime;
	}

MESSAGE("Exit mainloop");
}

void sys::RegisterGraphObject(GraphObject * object)
{
	gData.m_GraphObjectDA.Add(object);
}

void sys::RegisterScriptObject(ScriptObject * object)
{
	gData.m_ScriptObjectDA.Add(object);
}

float sys::GetDeltaTime()
{
	return gData.DeltaTime;
}

void sys::UpdateEditorMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}



	// show hierarchy
	{
		if (ImGui::Begin("Hierarchy"))
		{
			for (int i = 0; i < gData.m_GraphObjectDA.GetSize(); i++)
			{
				ImGui::PushID(i);
				auto it = gData.m_GraphObjectDA[i];
				//it.

				if (ImGui::TreeNode(std::string(it->GetName()).c_str()))
				{
					ImGui::Text("schbeul");
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
		ImGui::End();
	}
}
