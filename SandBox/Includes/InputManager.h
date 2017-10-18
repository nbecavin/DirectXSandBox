#ifndef __INPUTMANAGER_HH__
#define __INPUTMANAGER_HH__

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

	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		virtual void Init();
		virtual void Shut();
		virtual void Update(float dTime);

		virtual float GetAction(int inputid,int actionid) { return 0.f; }
	};

};

#endif //__INPUTMANAGER_HH__
