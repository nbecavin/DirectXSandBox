#pragma once
#ifndef __WINMAIN_HH__
#define __WINMAIN_HH__

namespace sys { namespace pc {
	
	void LowLevelInit();
	void LowLevelShut();
	void LowLevelEndFrame();

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
	//Windows::m_CoreWindow
	ref class Direct3DView sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		Direct3DView();
	
		// IFrameworkView Methods.
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
		virtual void Load(Platform::String^ entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	protected:
		// Event Handlers.
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnLogicalDpiChanged(Platform::Object^ sender);
		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnResuming(Platform::Object^ sender, Platform::Object^ args);
		void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
		void OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
		void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);

	private:
		bool m_windowClosed;
		bool m_windowVisible;
	};
	ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
	{
	public:
		virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
		virtual Windows::UI::Core::CoreWindow^ GetWindow() { return m_hWindow.Get(); }
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window) { m_hWindow=window; }
	private:
		Platform::Agile<Windows::UI::Core::CoreWindow> m_hWindow;
	};

	extern Direct3DApplicationSource^ m_App;

#endif

	extern HWND			hWnd;
	extern HINSTANCE	hInstance;

}; };

#define LOWLEVELINIT()		sys::pc::LowLevelInit()
#define LOWLEVELSHUT()		sys::pc::LowLevelShut()
#define LOWLEVELENDFRAME()	sys::pc::LowLevelEndFrame()

#endif //__WINMAIN_HH__
