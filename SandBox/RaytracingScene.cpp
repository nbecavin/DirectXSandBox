#include <RaytracingScene.h>
#include <Bitmap.h>
#include <Renderer.h>
#include <RenderHAL.h>

// ----------------------------------------------------------------------

RaytracingGeometry::RaytracingGeometry(RaytracingGeometryDesc& _initializer)
{
	m_Desc = _initializer;
}

void RaytracingGeometry::GetOrCreateScratchBuffer(U32 byteSize)
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	if (m_ScratchBuffer == nullptr)
		m_ScratchBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_None);
	else if (m_ScratchBuffer->GetSize() < byteSize)
	{
		// fo delete
		m_ScratchBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_None);
	}
}

void RaytracingGeometry::GetOrCreateAS(U32 byteSize)
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	if (m_AS == nullptr)
		m_AS = hal->CreateAccelerationStructure(byteSize);
	else if (m_AS->GetSize() < byteSize)
	{
		// fo delete
		m_AS = hal->CreateAccelerationStructure(byteSize);
	}
}

// ----------------------------------------------------------------------

RaytracingScene::RaytracingScene()
{
}

RaytracingScene::~RaytracingScene()
{
}

void RaytracingScene::Init()
{
}

void RaytracingScene::Shut()
{

}

void RaytracingScene::GetOrCreateScratchBuffer(U32 byteSize)
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	if (m_ScratchBuffer == nullptr)
		m_ScratchBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_None);
	else if (m_ScratchBuffer->GetSize() < byteSize)
	{
		// fo delete
		m_ScratchBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_None);
	}
}

void RaytracingScene::GetOrCreateTLAS(U32 byteSize)
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	if (m_TLAS == nullptr)
		m_TLAS = hal->CreateAccelerationStructure(byteSize);
	else if (m_TLAS->GetSize() < byteSize)
	{
		// fo delete
		m_TLAS = hal->CreateAccelerationStructure(byteSize);
	}
}

Buffer* RaytracingScene::GetOrCreateInstanceBuffer(U32 byteSize)
{
	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	auto currentBuffer = m_InstanceDesc[m_CurrentBufferIndex];

	if (currentBuffer == nullptr)
		currentBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_Write);
	else if (currentBuffer->GetSize() < byteSize)
	{
		// fo delete
		currentBuffer = hal->CreateBuffer(byteSize, Buffer::CpuAccess_Write);
	}
	return currentBuffer;
}
