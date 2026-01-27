#pragma once

class Bitmap;
class RaytracingScene
{
public:
	RaytracingScene();
	~RaytracingScene();

	void Init();
	void Shut();

public:

	Bitmap* m_TLAS;
	Bitmap* m_ScratchBuffer;
};
