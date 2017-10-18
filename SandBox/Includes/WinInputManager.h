#ifndef __WININPUTMANAGER_HH__
#define __WININPUTMANAGER_HH__

#include <InputManager.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>

namespace sys
{

	class WinInputManager : public InputManager
	{
	public:
		WinInputManager();
		~WinInputManager();

		virtual void Init();
		virtual void Shut();
		virtual void Update(float dTime);

		virtual float GetAction(int inputid,int actionid);

	protected:
		// DirectInput
		IDirectInput8 *	m_DirectInput;
		IDirectInputDevice8 * m_DeviceMouse;
		IDirectInputDevice8 * m_DeviceKeyboard;

		// Xinput
		XINPUT_STATE	m_XInputPads[XUSER_MAX_COUNT];
	};

};

#endif //__WININPUTMANAGER_HH__
