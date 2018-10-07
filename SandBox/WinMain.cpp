#if defined(_PC)

// SandBox.cpp : définit le point d'entrée pour l'application.
//

#include "stdafx.h"
#include "resource.h"
#include "SandBox.h"
#include <WinMain.h>
#include <DXRenderer.h>
#include <WinInputManager.h>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
	using namespace Windows::ApplicationModel;
	using namespace Windows::ApplicationModel::Core;
	using namespace Windows::ApplicationModel::Activation;
	using namespace Windows::UI::Core;
	using namespace Windows::System;
	using namespace Windows::Foundation;
	using namespace Windows::Graphics::Display;
	using namespace concurrency;
#endif

#define MAX_LOADSTRING 100

// Variables globales :
TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre
TCHAR szWindowClass[MAX_LOADSTRING];			// le nom de la classe de fenêtre principale

// Pré-déclarations des fonctions incluses dans ce module de code :
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

namespace sys { namespace pc {
	HINSTANCE	hInstance;
	int			nCmdShow;
	HWND		hWnd;
#if WINAPI_FAMILY==WINAPI_FAMILY_APP
	Direct3DApplicationSource^ m_App;
#endif
}; };

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	sys::pc::m_App = ref new sys::pc::Direct3DApplicationSource();
	CoreApplication::Run(sys::pc::m_App);
	return 0;
}
#else
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	sys::pc::hInstance = hInstance;
	sys::pc::nCmdShow = nCmdShow;

	sys::Init();
	sys::MainLoop();
	sys::Shut();

	return 0;
}
#endif

//
//  FONCTION : MyRegisterClass()
//
//  BUT : inscrit la classe de fenêtre.
//
//  COMMENTAIRES :
//
//    Cette fonction et son utilisation sont nécessaires uniquement si vous souhaitez que ce code
//    soit compatible avec les systèmes Win32 avant la fonction 'RegisterClassEx'
//    qui a été ajoutée à Windows 95. Il est important d'appeler cette fonction
//    afin que l'application dispose des petites icônes correctes qui lui sont
//    associées.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
#if WINAPI_FAMILY!=WINAPI_FAMILY_APP
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SANDBOX));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= "";//MAKEINTRESOURCE(IDC_SANDBOX);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
#else
	return 0;
#endif
}


//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   BUT : enregistre le handle de l'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
#if WINAPI_FAMILY!=WINAPI_FAMILY_APP
	DWORD ws = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	sys::pc::hWnd = CreateWindow(szWindowClass, szTitle, ws, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (!sys::pc::hWnd)
		return FALSE;
	ShowWindow(sys::pc::hWnd, nCmdShow);
	UpdateWindow(sys::pc::hWnd);
#endif
	return TRUE;
}

#if WINAPI_FAMILY!=WINAPI_FAMILY_APP

static bool ImGui_ImplWin32_UpdateMouseCursor();
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  BUT :  traite les messages pour la fenêtre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fenêtre principale
//  WM_DESTROY	- génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Analyse les sélections de menu :
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(sys::pc::hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			//DestroyWindow(hWnd);
			gData.Exit();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO : ajoutez ici le code de dessin...
		EndPaint(hWnd, &ps);
		break;
	//case WM_SIZE:
	//	break;
	case WM_DESTROY:
		gData.Exit();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Process Win32 mouse/keyboard inputs. 
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinations when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() == NULL)
		return 0;

	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	{
		int button = 0;
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
		if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
			::SetCapture(hwnd);
		io.MouseDown[button] = true;
		return 0;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		int button = 0;
		if (msg == WM_LBUTTONUP) button = 0;
		if (msg == WM_RBUTTONUP) button = 1;
		if (msg == WM_MBUTTONUP) button = 2;
		io.MouseDown[button] = false;
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
			::ReleaseCapture();
		return 0;
	}
	case WM_MOUSEWHEEL:
		io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_MOUSEHWHEEL:
		io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return 0;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return 0;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
			return 1;
		return 0;
	}
	return 0;
}

static bool ImGui_ImplWin32_UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		::SetCursor(NULL);
	}
	else
	{
		// Show OS mouse cursor
		LPTSTR win32_cursor = IDC_ARROW;
		switch (imgui_cursor)
		{
		case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
		}
		::SetCursor(::LoadCursor(NULL, win32_cursor));
	}
	return true;
}

void ImGui_ImplWin32_UpdateMousePos()
{
	ImGuiIO& io = ImGui::GetIO();

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
	{
		POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		::ClientToScreen(sys::pc::hWnd, &pos);
		::SetCursorPos(pos.x, pos.y);
	}

	// Set mouse position
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	POINT pos;
	if (::GetActiveWindow() == sys::pc::hWnd && ::GetCursorPos(&pos))
		if (::ScreenToClient(sys::pc::hWnd, &pos))
			io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

#endif

void sys::pc::LowLevelInit()
{
#if WINAPI_FAMILY!=WINAPI_FAMILY_APP
	HACCEL hAccelTable;

	// Initialise les chaînes globales
	LoadString(sys::pc::hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(sys::pc::hInstance, IDC_SANDBOX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(sys::pc::hInstance);

	// Effectue l'initialisation de l'application :
	if (!InitInstance (sys::pc::hInstance, nCmdShow))
	{
		return ;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SANDBOX));
#endif

	// Instantiate managers
	gData.Rdr = (Renderer*) new DXRenderer;
	gData.Rdr->Init();
	gData.Input = (InputManager*) new WinInputManager;
	gData.Input->Init();
}

void sys::pc::LowLevelShut()
{
	gData.Input->Shut();
	delete gData.Input;
	gData.Rdr->Shut();
	delete gData.Rdr;
#if WINAPI_FAMILY!=WINAPI_FAMILY_APP
	DestroyWindow(sys::pc::hWnd);
#endif
}

void sys::pc::LowLevelEndFrame()
{
	MSG	msg;
	
#ifndef _MASTER
//	sys::Console->SetMemUsed( MemManager.GetMemUsed() );
#endif

	ImGui_ImplWin32_UpdateMousePos();

	// Empty the message queue for this window
#if WINAPI_FAMILY==WINAPI_FAMILY_APP
	CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
#else
	while( PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
#endif

	/*
	// Toggle Fullscreen/Windowed mode
	if(sys::InputMgr->ToggleFullscreen())
	{
		if(sys::Flags&FL_SYS_FULLSCREEN)
			sys::Rdr->SetWindowed();
		else
			sys::Rdr->SetFullscreen();
			//sys::Rdr->SetWindowed();
	}
	*/
}

#if 0 //WINAPI_FAMILY_ONE_PARTITION( WINAPI_FAMILY, WINAPI_PARTITION_APP )

using namespace sys::pc;

Direct3DView::Direct3DView() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

void Direct3DView::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Direct3DView::OnActivated);

	CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &Direct3DView::OnSuspending);

	CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &Direct3DView::OnResuming);

	//m_renderer = ref new CubeRenderer();
}

void Direct3DView::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &Direct3DView::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &Direct3DView::OnVisibilityChanged);

	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &Direct3DView::OnWindowClosed);

	window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Direct3DView::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Direct3DView::OnPointerMoved);

	sys::pc::m_App->SetWindow(window);
	//m_renderer->Initialize(CoreWindow::GetForCurrentThread());
}

void Direct3DView::Load(Platform::String^ entryPoint)
{
}

void Direct3DView::Run()
{
	sys::Init();
	sys::MainLoop();
	sys::Shut();

	//BasicTimer^ timer = ref new BasicTimer();

	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			//timer->Update();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			/*
			m_renderer->Update(timer->Total, timer->Delta);
			m_renderer->Render();
			m_renderer->Present(); // This call is synchronized to the display frame rate.
			*/
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void Direct3DView::Uninitialize()
{
}

void Direct3DView::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
//	m_renderer->UpdateForWindowSizeChange();
}

void Direct3DView::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void Direct3DView::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

void Direct3DView::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void Direct3DView::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	// Insert your code here.
}

void Direct3DView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void Direct3DView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		// Insert your code here.

		deferral->Complete();
	});
}
 
void Direct3DView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new Direct3DView();
}
#endif

#endif //_PC
