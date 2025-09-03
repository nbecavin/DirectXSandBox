#ifndef __GRAPHOBJECT_HH__
#define __GRAPHOBJECT_HH__

#include <DynArray.h>

class GraphObject
{
public:
	GraphObject();

	virtual void SetName(std::string& _name) final { m_Name = _name; }
	virtual const std::string& GetName() final { return m_Name; }

	virtual void Update(F32 dTime);
	virtual void Draw();
	virtual void DrawGBuffer();

			void SetScale(float scale);
			void SetWorldPosition(float x,float y,float z);

protected:
	// Geometry space position
	Mat4x4				m_Node;
	std::string			m_Name;
};

typedef DynArray<GraphObject*,16> GraphObjectDA;

#endif //__GRAPHOBJECT_HH__
