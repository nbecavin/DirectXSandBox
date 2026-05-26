#include <InputManager.h>
#include <SDL3/SDL.h>
#include <imgui.h>

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
		SDL_SetEventEnabled(SDL_EVENT_GAMEPAD_ADDED, true);
		SDL_SetEventEnabled(SDL_EVENT_GAMEPAD_REMOVED, true);
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
					gamepads.Emplace(std::move(gamepad));
				}
			}
			else if (e.type == SDL_EVENT_GAMEPAD_REMOVED)
			{
			}
		}

		// Poll events
		SDL_UpdateGamepads();

		bool bOpened = ImGui::Begin("Input Debug");
		
		for (auto& gamepad : gamepads)
		{
			if (gamepad.isConnected())
			{
				gamepad.update();

				if (bOpened)
				{
					ImGui::Text("Type = %s", gamepad.getTypeString());
					ImGui::Text("South %s", gamepad.buttons.south ? "Pressed" : "Released");
					ImGui::Text("East %s", gamepad.buttons.east ? "Pressed" : "Released");
					ImGui::Text("West %s", gamepad.buttons.west ? "Pressed" : "Released");
					ImGui::Text("North %s", gamepad.buttons.north ? "Pressed" : "Released");
					ImGui::Text("Left Stick: (%.2f, %.2f)", gamepad.leftStick.x, gamepad.leftStick.y);
					ImGui::Text("Right Stick: (%.2f, %.2f)", gamepad.rightStick.x, gamepad.rightStick.y);
					ImGui::Text("Left Trigger: %.2f", gamepad.leftTrigger.value);
					ImGui::Text("Right Trigger: %.2f", gamepad.rightTrigger.value);
					ImGui::Text("L1: %s", gamepad.buttons.l1 ? "Pressed" : "Released");
					ImGui::Text("R1: %s", gamepad.buttons.r1 ? "Pressed" : "Released");
					ImGui::Text("L3: %s", gamepad.buttons.l3 ? "Pressed" : "Released");
					ImGui::Text("R3: %s", gamepad.buttons.r3 ? "Pressed" : "Released");
					ImGui::Text("Start: %s", gamepad.buttons.start ? "Pressed" : "Released");
					ImGui::Text("Back: %s", gamepad.buttons.back ? "Pressed" : "Released");
					ImGui::Text("Guide: %s", gamepad.buttons.guide ? "Pressed" : "Released");
				}
			}
		}

		if(bOpened)
		{
			ImGui::End();
		}
	}

	float InputManager::GetAction(int inputid, int actionid)
	{
		float value = 0.f;
		float speed = 1.f;
		float deadZone = 0.25f;
		
		auto RemoveDeadZone = [](float v, float deadZone) -> float {
			v = Clamp(v, 0.f, 1.f);
			if (std::abs(v) < deadZone) return 0.f;
			return (v - deadZone) / (1.f - deadZone);
		};

		for(auto& g : gamepads)
		{
			switch (actionid) {
			case INPUT_ACTION_MOVE_FORWARD:
				value = RemoveDeadZone(-g.leftStick.y * speed, deadZone);
				break;
			case INPUT_ACTION_MOVE_BACKWARD:
				value = RemoveDeadZone(g.leftStick.y * speed, deadZone);
				break;
			case INPUT_ACTION_MOVE_LEFT:
				value = RemoveDeadZone(-g.leftStick.x * speed, deadZone);
				break;
			case INPUT_ACTION_MOVE_RIGHT:
				value = RemoveDeadZone(g.leftStick.x * speed, deadZone);
				break;
			case INPUT_ACTION_RSTICK_DOWN:
				value = RemoveDeadZone(-g.rightStick.y * speed, deadZone);
				break;
			case INPUT_ACTION_RSTICK_UP:
				value = RemoveDeadZone(g.rightStick.y * speed, deadZone);
				break;
			case INPUT_ACTION_RSTICK_LEFT:
				value = RemoveDeadZone(-g.rightStick.x * speed, deadZone);
				break;
			case INPUT_ACTION_RSTICK_RIGHT:
				value = RemoveDeadZone(g.rightStick.x * speed, deadZone);
				break;
			case INPUT_ACTION_MOVE_UP:
				value = g.rightTrigger.value;
				break;
			case INPUT_ACTION_MOVE_DOWN:
				value = g.leftTrigger.value;
				break;
			};
		}

		return value;
	}

	void Gamepad::update()
	{
		//void handleAxis(const SDL_GamepadAxisEvent & a) {
		leftStick.x = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTX));
		leftStick.y = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFTY));
		rightStick.x = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTX));
		rightStick.y = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHTY));
		leftTrigger.value = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
		rightTrigger.value = norm(SDL_GetGamepadAxis(controller, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));
	
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
