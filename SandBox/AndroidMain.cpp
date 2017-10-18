#if defined(_ANDROID)

#include <GLES2/gl2.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <jni.h>
#include <nv_shader/nv_shader.h>
#include <nv_time/nv_time.h>
#include <nv_thread/nv_thread.h>
#include <nv_event/nv_event.h>
#include <nv_math/nv_math.h>
#include <nv_math/nv_matrix.h>
#include <nv_debug.h>

#include <AndroidMain.h>
#include <AndroidRenderer.h>
#include <AndroidInputManager.h>

bool sys::android::m_uiVisible = false;

void sys::android::LowLevelInit()
{
	// Instantiate managers
	gData.Rdr = (Renderer*) new AndroidRenderer;
	gData.Rdr->Init();
	gData.Input = (InputManager*) new AndroidInputManager;
	gData.Input->Init();
}

void sys::android::LowLevelShut()
{
	PauseGraphics();
	gData.Input->Shut();
	delete gData.Input;
	gData.Rdr->Shut();
	delete gData.Rdr;
}

Bool sys::android::ResumeGraphics()
{
	return ((AndroidRenderer*)gData.Rdr)->Resume();
}

void sys::android::PauseGraphics()
{
	((AndroidRenderer*)gData.Rdr)->Pause();
}

// Add any initialization that requires the app Java classes
// to be accessible (such as nv_shader_init, NvAPKInit, etc,
// as listed in the docs)
int32_t NVEventAppInit(int32_t argc, char** argv)
{
	nv_shader_init();
	return 0;
}

int32_t NVEventAppMain(int32_t argc, char** argv)
{
    sys::android::m_uiVisible = false;

	MESSAGE( "Calling app init" );
	sys::Init();
	MESSAGE( "App entering main loop" );
	sys::MainLoop();
	MESSAGE( "App shut" );
	sys::Shut();

	return 0;
}

void sys::android::LowLevelEndFrame()
{
	// Pump NvEvent
	NVEvent ev;
	while (NVEventGetNextEvent(&ev, m_uiVisible ? 0 : NV_EVENT_WAIT_FOREVER))
	{
		switch (ev.m_type)
		{
		case NV_EVENT_WINDOW_SIZE:
			MESSAGE("Window size event: %d x %d", ev.m_data.m_size.m_w, ev.m_data.m_size.m_h);
			gData.Rdr->SetSize(ev.m_data.m_size.m_w,ev.m_data.m_size.m_h);
			break;
		case NV_EVENT_KEY:
			MESSAGE("Key");
			break;
		case NV_EVENT_CHAR:
			MESSAGE("Char event: 0x%02x", ev.m_data.m_char.m_unichar);
			//!TODO!keyDownEvent(ev.m_data.m_char.m_unichar);
			break;
		case NV_EVENT_TOUCH:
			//!TODO!inputEvent((int)(ev.m_data.m_touch.m_x),(int)(ev.m_data.m_touch.m_y));
			MESSAGE("Touch event %d %d", (int)(ev.m_data.m_touch.m_x),(int)(ev.m_data.m_touch.m_y) );
			break;
		case NV_EVENT_RESUME:
		{
			MESSAGE("Resume event");
			if(sys::android::ResumeGraphics())
			{
				MESSAGE("graphics init successfull");
				m_uiVisible = true;
			}
			else
			{
				MESSAGE("graphics init failed, quitting");
				gData.Exit();
			}
			break;
		}
		case NV_EVENT_PAUSE:
		{
			MESSAGE("Pause event");
			// the app must invoke NVEventGetNextEvent in order to
			// release the java thread after handling pause. Failure
			// to do so will get the app killed by the OS quickly.
			sys::android::PauseGraphics();
			m_uiVisible = false;
			continue;
		}
		case NV_EVENT_QUIT:
			MESSAGE("Quit event");
			gData.Exit();
			break;
		default:
			MESSAGE("UNKNOWN event (0x%x)", ev.m_type);
			break;
		};
	}
}

#endif //_ANDROID
