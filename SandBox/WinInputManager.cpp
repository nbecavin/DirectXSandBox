#if defined(_PC)

#include <WinMain.h>
#include <WinInputManager.h>

namespace sys
{

	WinInputManager::WinInputManager()
	{
	}

	WinInputManager::~WinInputManager()
	{
	}

	void WinInputManager::Init()
	{
		InputManager::Init();

		//
		// Create DirectInput
		HRESULT hr = DirectInput8Create(sys::pc::hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&m_DirectInput),NULL);
		//ASSERTC_T(!(FAILED(hr)),"Unable to create DirectInput interface");
	
		hr = m_DirectInput->CreateDevice(GUID_SysMouse,&m_DeviceMouse,NULL);
		m_DeviceMouse->SetDataFormat(&c_dfDIMouse2);
	}

	void WinInputManager::Shut()
	{
		m_DirectInput->Release();

		InputManager::Shut();
	}

	float WinInputManager::GetAction(int inputid,int actionid)
	{
		XINPUT_STATE * state = &m_XInputPads[inputid];
		float	coef = 255.f / 32767.f;
		float	value = 0.f;
		switch(actionid){
		case INPUT_ACTION_MOVE_FORWARD:
			value = (float)Clamp((float)state->Gamepad.sThumbLY*coef,0.f,255.f);
			break;
		case INPUT_ACTION_MOVE_BACKWARD:
			value = -(float)Clamp((float)state->Gamepad.sThumbLY*coef,-255.f,0.f);
			break;
		case INPUT_ACTION_MOVE_LEFT:
			value = -(float)Clamp((float)state->Gamepad.sThumbLX*coef,-255.f,0.f);
			break;
		case INPUT_ACTION_MOVE_RIGHT:
			value = (float)Clamp((float)state->Gamepad.sThumbLX*coef,0.f,255.f);
			break;
		case INPUT_ACTION_RSTICK_LEFT:
			value = -(float)Clamp((float)state->Gamepad.sThumbRX*coef,-255.f,0.f);
			break;
		case INPUT_ACTION_RSTICK_RIGHT:
			value = (float)Clamp((float)state->Gamepad.sThumbRX*coef,0.f,255.f);
			break;
		case INPUT_ACTION_RSTICK_DOWN:
			value = (float)Clamp((float)state->Gamepad.sThumbRY*coef,0.f,255.f);
			break;
		case INPUT_ACTION_RSTICK_UP:
			value = -(float)Clamp((float)state->Gamepad.sThumbRY*coef,-255.f,0.f);
			break;
		case INPUT_ACTION_MOVE_UP:
			value = (float)state->Gamepad.bRightTrigger;
			break;
		case INPUT_ACTION_MOVE_DOWN:
			value = (float)state->Gamepad.bLeftTrigger;
			break;
		};
	
		// Remove dead zone
		value = Clamp(255.f*(value-32.f)/(255.f-32.f),0.f,255.f);

		return value/255.f;
	}

	void WinInputManager::Update(float dTime)
	{
		// Update from mouse
		m_DeviceMouse->Acquire();
		DIMOUSESTATE2 lMouseState;
		m_DeviceMouse->GetDeviceState(sizeof(lMouseState),&lMouseState);
		
		for(int i=0;i<XUSER_MAX_COUNT;i++)
		{
			// Simply get the state of the controller from XInput.
			DWORD dwResult = XInputGetState( i, &m_XInputPads[i] );
			if(dwResult==ERROR_SUCCESS)
			{
				//OutputDebugString("kjlkj");
			}
			else
				ZeroMemory( &m_XInputPads[i], sizeof(XINPUT_STATE) );
		}

		/*char str[256];
		sprintf(str,"mouse XYZ(%d,%d,%d) BUTTON(%d,%d,%d,%d,%d,%d,%d,%d)\n",lMouseState.lX,lMouseState.lY,lMouseState.lZ,
				lMouseState.rgbButtons[0],lMouseState.rgbButtons[1],lMouseState.rgbButtons[2],lMouseState.rgbButtons[3],
				lMouseState.rgbButtons[4],lMouseState.rgbButtons[5],lMouseState.rgbButtons[6],lMouseState.rgbButtons[7]);
		OutputDebugString(str);*/
	}

};

#endif //_PC
