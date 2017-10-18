#if defined(_PC)

#include "SandBox.h"

namespace sys {
	
	LONGLONG m_llQPFTicksPerSec;
	LONGLONG m_llBaseTime;
	
	double CDXUTTimer_GetAbsoluteTime()
	{
		LARGE_INTEGER qwTime = { 0 };
		QueryPerformanceCounter( &qwTime );
		double fTime = qwTime.QuadPart / ( double )m_llQPFTicksPerSec;
		return fTime;
	}

	void InitTimer()
	{
		// Use QueryPerformanceFrequency to get the frequency of the counter
		LARGE_INTEGER qwTicksPerSec = { 0 };
		QueryPerformanceFrequency( &qwTicksPerSec );
		m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

		m_llBaseTime = (LONGLONG)GetDAbsoluteTime();
	}

	double GetDAbsoluteTime()
	{
		return CDXUTTimer_GetAbsoluteTime()-m_llBaseTime;
	}

	float GetAbsoluteTime()
	{
		return static_cast<float>(GetDAbsoluteTime());
	}

};

#endif //_PC
