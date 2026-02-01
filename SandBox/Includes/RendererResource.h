#pragma once

class BufferDesc
{

};

class Buffer {
public:
	enum EMap {
		Read,
		Write,
		ReadWrite,
		WriteDiscard,
		WriteNoOverwrite
	};

public:
	virtual U32 GetSize() { return Size; }
protected:
	U32 Size;
};

class LockableBuffer : public Buffer
{
public:
	virtual void Create(U32 _Size, U32 _Usage, U32 _Fmt, void* _Datas) = 0;
	virtual bool IsInited() = 0;
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void** pData, EMap MapFlags = WriteNoOverwrite) = 0;
	virtual void Unlock() = 0;
};

using ConstantBuffer = LockableBuffer;

class AccelerationStructure : public Buffer
{
public:

	// resource
	// view
};
