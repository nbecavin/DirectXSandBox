#pragma once

class Buffer;
class AccelerationStructure;
class RaytracingScene
{
public:
	static constexpr const U32 NumBuffer = 4;

	RaytracingScene();
	~RaytracingScene();

	void Init();
	void Shut();

	void GetOrCreateScratchBuffer(U32 byteSize);
	void GetOrCreateTLAS(U32 byteSize);
	Buffer* GetOrCreateInstanceBuffer(U32 byteSize);

	void Advance() { m_CurrentBufferIndex = (m_CurrentBufferIndex + 1) % NumBuffer; }

public:
	AccelerationStructure* m_TLAS = nullptr;
	Buffer* m_ScratchBuffer = nullptr;
	Buffer* m_InstanceDesc[NumBuffer] = {};
	U32 m_CurrentBufferIndex = 0;
};
