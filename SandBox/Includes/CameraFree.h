#ifndef __CAMERAFREE_HH__
#define __CAMERAFREE_HH__

#include <ScriptObject.h>

class CameraFree : public ScriptObject
{
public:
	CameraFree();

	virtual void Update(F32 dTime);

protected:
};

#endif //__CAMERAFREE_HH__
