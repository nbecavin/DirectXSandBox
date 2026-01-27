#pragma once

#include <ScriptObject.h>
#include <GraphObject.h>
class Material;
#include <AssetCache.h>

namespace sys
{
	// Some forward declaration
	class Renderer;
	class GpuScene;
	class InputManager;

	void Init();

	void UpdateEditorMenu();
	void MainLoop();
	void EndFrame();

	void Shut();

	void RegisterMaterial(Material* material);
	void RegisterGraphObject(GraphObject * object);
	void RegisterCameraObject(GraphObject* object);
	void RegisterScriptObject(ScriptObject * object);

	// Global pointers - Can be accessed from everywhere
	class Globals
	{
	public:
		InputManager* Input;
		Renderer* Rdr;
		std::unique_ptr<GpuScene> Scene;

		GraphObjectDA	m_CameraObjectDA;
		ScriptObjectDA	m_ScriptObjectDA;
		GraphObjectDA	m_GraphObjectDA;

		float			DeltaTime;

		void			Init();
		void			Exit();
		Bool			GetExitStatus();

	private:
		Bool			m_bExitStatus;
	};

	// Timer function
	void		InitTimer();
	double		GetDAbsoluteTime();
	float		GetAbsoluteTime();
	float		GetDeltaTime();

};

extern sys::Globals gData;
