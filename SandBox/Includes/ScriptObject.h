#ifndef __SCRIPTOBJECT_HH__
#define __SCRIPTOBJECT_HH__

#include <DynArray.h>

class ScriptObject
{
public:
	ScriptObject();

	virtual void Update(F32 dTime);

protected:
};

typedef DynArray<ScriptObject*,16> ScriptObjectDA;

#endif //__SCRIPTOBJECT_HH__
