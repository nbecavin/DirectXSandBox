/*
 * AndroidMain.h
 *
 *  Created on: 6 mars 2012
 *      Author: Nicolas
 */

#ifndef ANDROIDMAIN_H_
#define ANDROIDMAIN_H_

namespace sys { namespace android {

	void LowLevelInit();
	void LowLevelShut();
	void LowLevelEndFrame();

//	extern HWND			hWnd;
//	extern HINSTANCE	hInstance;
	extern bool			m_uiVisible;

	Bool ResumeGraphics();
	void PauseGraphics();

}; };

#define LOWLEVELINIT()		sys::android::LowLevelInit()
#define LOWLEVELSHUT()		sys::android::LowLevelShut()
#define LOWLEVELENDFRAME()	sys::android::LowLevelEndFrame()

#endif /* ANDROIDMAIN_H_ */
