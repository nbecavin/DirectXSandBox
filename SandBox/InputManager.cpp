#include <InputManager.h>
#include <SDL3/SDL.h>

namespace sys
{

	InputManager::InputManager()
	{
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Init()
	{
		/*
		int numJoysticks = 0;
		SDL_JoystickID* j = SDL_GetJoysticks(&numJoysticks);
		for (int i = 0; i < numJoysticks; i++, j++)
		{
			Gamepad gamepad;
			if (gamepad.open(*j))
			{
				gamepads.Add(gamepad);
			}
		}*/

		SDL_SetEventEnabled(SDL_EVENT_GAMEPAD_ADDED, true);
		SDL_SetEventEnabled(SDL_EVENT_GAMEPAD_REMOVED, true);

		//SDL_Log("SDL runtime version: %s", SDL_GetVersionString());
	}

	void InputManager::Shut()
	{
	}

	void InputManager::Update(float dTime)
	{
		// refresh gamepad list here....
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_EVENT_GAMEPAD_ADDED)
			{
				Gamepad gamepad;
				if (gamepad.open(e.gdevice.which))
				{
					gamepads.Add(gamepad);
				}
			}
			else if (e.type == SDL_EVENT_GAMEPAD_REMOVED)
			{
			}
		}

		// Poll events
		SDL_UpdateGamepads();

		for(auto& gamepad : gamepads)
		{
			if (gamepad.isConnected())
			{
				gamepad.update();
			}			
		}
	}

	float InputManager::GetAction(int inputid, int actionid)
	{
		float value = 0.f;

		float	coef = 255.f;// / 32767.f;

		if (gamepads.GetSize())
		{
			Gamepad g = gamepads[0];
			value = g.leftStick.y; // Example: using left stick Y for forward/backward
			switch (actionid) {
			case INPUT_ACTION_MOVE_FORWARD:
				value = (float)Clamp((float)g.leftStick.y * coef, 0.f, 255.f);
				break;
			case INPUT_ACTION_MOVE_BACKWARD:
				value = -(float)Clamp((float)g.leftStick.y * coef, -255.f, 0.f);
				break;
			case INPUT_ACTION_MOVE_LEFT:
				value = -(float)Clamp((float)g.leftStick.x * coef, -255.f, 0.f);
				break;
			case INPUT_ACTION_MOVE_RIGHT:
				value = (float)Clamp((float)g.leftStick.x * coef, 0.f, 255.f);
				break;
			case INPUT_ACTION_RSTICK_LEFT:
				value = -(float)Clamp((float)g.rightStick.x * coef, -255.f, 0.f);
				break;
			case INPUT_ACTION_RSTICK_RIGHT:
				value = (float)Clamp((float)g.rightStick.x * coef, 0.f, 255.f);
				break;
			case INPUT_ACTION_RSTICK_DOWN:
				value = (float)Clamp((float)g.rightStick.y * coef, 0.f, 255.f);
				break;
			case INPUT_ACTION_RSTICK_UP:
				value = -(float)Clamp((float)g.rightStick.y * coef, -255.f, 0.f);
				break;
			case INPUT_ACTION_MOVE_UP:
				value = (float)g.rightTrigger.value;
				break;
			case INPUT_ACTION_MOVE_DOWN:
				value = (float)g.leftTrigger.value;
				break;
			};
		}

		// Remove dead zone
		value = Clamp(255.f * (value - 32.f) / (255.f - 32.f), 0.f, 255.f);

		return value / 255.f;
	}

	void Gamepad::update()
	{
		//void handleAxis(const SDL_GamepadAxisEvent & a) {
		leftStick.x = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX));
		leftStick.y = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY));
		rightStick.x = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTX));
		rightStick.y = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTY));
		leftTrigger.value = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
		rightTrigger.value = SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
	
		//void handleButton(const SDL_GamepadButtonEvent & b) {
		buttons.south =	SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_SOUTH);
		buttons.east =  SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_EAST);
		buttons.west = 	SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_WEST);
		buttons.north =	SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_NORTH);

		buttons.l1 = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
		buttons.r1 = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
		buttons.l3 = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_LEFT_STICK);
		buttons.r3 = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_RIGHT_STICK);

		buttons.start = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_START);
		buttons.back  = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_BACK);
		buttons.guide = SDL_GetGamepadButton(controller, SDL_GAMEPAD_BUTTON_GUIDE);
	}
};
