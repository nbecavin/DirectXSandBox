#pragma once

class Buffer;
class AccelerationStructure;
class RaytracingScene
{
public:
	RaytracingScene();
	~RaytracingScene();

	void Init();
	void Shut();

public:

	AccelerationStructure* m_TLAS;
	Buffer* m_ScratchBuffer;
};
