#ifndef __ANDROIDINPUTMANAGER_HH__
#define __ANDROIDINPUTMANAGER_HH__

#include <InputManager.h>

namespace sys
{

	class AndroidInputManager : public InputManager
	{
	public:
		AndroidInputManager();
		~AndroidInputManager();

		virtual void Init();
		virtual void Shut();
		virtual void Update(float dTime);

		virtual float GetAction(int inputid,int actionid);

	protected:
		// DirectInput
		//IDirectInput8 *	m_DirectInput;
		//IDirectInputDevice8 * m_DeviceMouse;
		//IDirectInputDevice8 * m_DeviceKeyboard;

		// Xinput
		//XINPUT_STATE	m_XInputPads[XUSER_MAX_COUNT];
	};

};

#endif //__ANDROIDINPUTMANAGER_HH__
