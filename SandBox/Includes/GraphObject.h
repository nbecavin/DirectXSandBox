#ifndef __GRAPHOBJECT_HH__
#define __GRAPHOBJECT_HH__

#include <DynArray.h>

struct NameID
{
public:
	std::string Str;
	U32 Hash;

	NameID(const std::string& _name)
	{
		Str = _name;
		//m_HashID = (U32)std::hash<std::string>(_name);
	}
};

class GraphObject
{
public:
	GraphObject();

	virtual void SetName(std::string& _name) final { m_Name = _name; }
	virtual const std::string& GetName() final { return m_Name.Str; }

	virtual void Update(F32 dTime);
	virtual void Draw();
	virtual void DrawGBuffer();

			void SetScale(float scale);
			void SetWorldPosition(float x,float y,float z);

protected:
	// Geometry space position
	Mat4x4				m_Node;
	NameID				m_Name = { "Unknown" };
};

typedef DynArray<GraphObject*,16> GraphObjectDA;

#endif //__GRAPHOBJECT_HH__
