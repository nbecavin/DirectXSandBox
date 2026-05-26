#ifndef __INPUTMANAGER_HH__
#define __INPUTMANAGER_HH__

#include <SDL3/SDL.h>

enum
{
	INPUT_ACTION_MOVE_FORWARD,
	INPUT_ACTION_MOVE_BACKWARD,
	INPUT_ACTION_MOVE_LEFT,
	INPUT_ACTION_MOVE_RIGHT,
	INPUT_ACTION_MOVE_UP,
	INPUT_ACTION_MOVE_DOWN,
	INPUT_ACTION_RSTICK_LEFT,
	INPUT_ACTION_RSTICK_RIGHT,
	INPUT_ACTION_RSTICK_UP,
	INPUT_ACTION_RSTICK_DOWN
};

namespace sys
{
    class Gamepad {
    public:
        enum class Type {
            Unknown,
            Xbox,
            PlayStation,
            Nintendo
        };

        struct Stick { float x = 0.f; float y = 0.f; };
        struct Trigger { float value = 0.f; };

        struct Buttons {
            bool south = false;   // A (Xbox), Cross (PS)
            bool east = false;    // B (Xbox), Circle (PS)
            bool west = false;    // X (Xbox), Square (PS)
            bool north = false;   // Y (Xbox), Triangle (PS)

            bool l1 = false;
            bool r1 = false;
            bool l3 = false;
            bool r3 = false;

            bool start = false;   // Options / Menu
            bool back = false;    // Share / View
            bool guide = false;   // Xbox button / PS button
        };

        Gamepad() = default;

        // Move ctor
        Gamepad(Gamepad&& o) noexcept
            : controller(o.controller)
        {
            o.controller = nullptr;
        }

        // Move assign
        Gamepad& operator=(Gamepad&& o) noexcept
        {
            if (this != &o) {
                close();
                controller = o.controller;
                o.controller = nullptr;
            }
            return *this;
        }

        ~Gamepad() { close(); }

        bool open(SDL_JoystickID& id)
        {
            controller = SDL_OpenGamepad(id);
            if (!controller)
                return false;

            const char* name = SDL_GetGamepadName(controller);

            int count;
            SDL_GamepadBinding** bindings = SDL_GetGamepadBindings(controller, &count);

            joystick = SDL_GetGamepadJoystick(controller);
            haptic = SDL_OpenHapticFromJoystick(joystick);
            if (haptic) SDL_InitHapticRumble(haptic);

            detectType();
            return true;
        }

        void close()
        {
            if (haptic) { SDL_CloseHaptic(haptic); haptic = nullptr; }
            if (controller) { SDL_CloseGamepad(controller); controller = nullptr; }
        }

		const char* getTypeString() { return SDL_GetGamepadName(controller); }

        bool isConnected() const { return controller != nullptr; }

        void update();

        void update(const SDL_Event& e) {
            if (!controller) return;

            switch (e.type) {
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: handleAxis(e.gaxis); break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP: handleButton(e.gbutton); break;
            }
        }

        // Public state
        Stick leftStick, rightStick;
        Trigger leftTrigger, rightTrigger;
        Buttons buttons;
        Type type = Type::Unknown;

        // Features
        void rumble(float strength, uint32_t ms) {
            if (haptic) SDL_PlayHapticRumble(haptic, strength, ms);
        }

        void setLED(uint8_t r, uint8_t g, uint8_t b) {
            if (controller) SDL_SetGamepadLED(controller, r, g, b);
        }

    private:
        SDL_Gamepad* controller = nullptr;
        SDL_Joystick* joystick = nullptr;
        SDL_Haptic* haptic = nullptr;

        void detectType() {
            std::string name = SDL_GetGamepadName(controller);
            if (name.find("Xbox") != std::string::npos) type = Type::Xbox;
            else if (name.find("DualSense") != std::string::npos ||
                name.find("Wireless Controller") != std::string::npos)
                type = Type::PlayStation;
            else type = Type::Unknown;
        }

        static float norm(Sint16 v) {
            return (v >= 0 ? v / 32767.f : v / 32768.f);
        }

        void handleAxis(const SDL_GamepadAxisEvent& a) {
            float v = norm(a.value);

            switch (a.axis) {
            case SDL_GAMEPAD_AXIS_LEFTX: leftStick.x = v; break;
            case SDL_GAMEPAD_AXIS_LEFTY: leftStick.y = v; break;
            case SDL_GAMEPAD_AXIS_RIGHTX: rightStick.x = v; break;
            case SDL_GAMEPAD_AXIS_RIGHTY: rightStick.y = v; break;
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER: leftTrigger.value = a.value / 32767.f; break;
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER: rightTrigger.value = a.value / 32767.f; break;
            }
        }

        void handleButton(const SDL_GamepadButtonEvent& b) {
            bool pressed = b.down;

            switch (b.button) {
            case SDL_GAMEPAD_BUTTON_SOUTH: buttons.south = pressed; break;
            case SDL_GAMEPAD_BUTTON_EAST: buttons.east = pressed; break;
            case SDL_GAMEPAD_BUTTON_WEST: buttons.west = pressed; break;
            case SDL_GAMEPAD_BUTTON_NORTH: buttons.north = pressed; break;

            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER: buttons.l1 = pressed; break;
            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: buttons.r1 = pressed; break;
            case SDL_GAMEPAD_BUTTON_LEFT_STICK: buttons.l3 = pressed; break;
            case SDL_GAMEPAD_BUTTON_RIGHT_STICK: buttons.r3 = pressed; break;

            case SDL_GAMEPAD_BUTTON_START: buttons.start = pressed; break;
            case SDL_GAMEPAD_BUTTON_BACK: buttons.back = pressed; break;
            case SDL_GAMEPAD_BUTTON_GUIDE: buttons.guide = pressed; break;
            }
        }
    };

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		DynArray<Gamepad, 4> gamepads;

		virtual void Init();
		virtual void Shut();
		virtual void Update(float dTime);

        virtual float GetAction(int inputid, int actionid);
	};

};

#endif //__INPUTMANAGER_HH__
