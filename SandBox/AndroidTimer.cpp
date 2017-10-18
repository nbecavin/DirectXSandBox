#if defined(_ANDROID)

#include <nv_time/nv_time.h>

//
// Timer functions
//

namespace sys {

	void InitTimer()
	{
		//Get time
		if(nvValidTimeExtension())
			MESSAGE("============ TIME EXTENSION ============");
		else
			MESSAGE("!!!!!!!!!!!! NO TIME EXTENSION !!!!!!!!!!!!");

		//double base_time = (double)(nvGetSystemTime()) / 1000.0;
	}

	double GetDAbsoluteTime()
	{
		double current_time = (double)(nvGetSystemTime()) / 1000.0;
		return current_time;
	}

	float GetAbsoluteTime()
	{
		return static_cast<float>(GetDAbsoluteTime());
	}

};

#endif //_ANDROID
