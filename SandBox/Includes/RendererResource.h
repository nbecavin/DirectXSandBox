#pragma once

class BufferDesc
{

};

class Buffer
{
public:
	enum ECpuAccess
	{
		CpuAccess_None = 0, // No CPU access, fastest path for GPU read & write memory.
		CpuAccess_Read,     // CPU can read but not write, GPU can copy to; ideal for CPU readbacks.
		CpuAccess_Write,    // CPU can write but not read, GPU can only read (ideally not too often); ideal for constants & uploads.

		CpuAccess_Count
	};

	enum EMap {
		Read,
		Write,
		ReadWrite,
		WriteDiscard,
		WriteNoOverwrite
	};

public:
	template <typename T>
	T* AsPtr() { return (T*)this; }

	Buffer(U32 _Size = 0, ECpuAccess _CpuAccess = CpuAccess_None)
	{
		CpuAccess = _CpuAccess;
		Size = _Size;
	}
	virtual U32 GetSize() { return Size; }
protected:
	U32 Size = 0;
	ECpuAccess CpuAccess;
};

class LockableBuffer : public Buffer
{
public:
	LockableBuffer() = default;
	virtual void Create(U32 _Size, U32 _Stride, U32 _Usage, U32 _Fmt, void* _Datas) = 0;
	virtual bool IsInited() = 0;
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void** pData, EMap MapFlags = WriteNoOverwrite) = 0;
	virtual void Unlock() = 0;
};

using ConstantBuffer = LockableBuffer;

class AccelerationStructure : public Buffer
{
public:
	AccelerationStructure(U32 _Size) : Buffer(_Size) {}
	// resource
	// view
};
