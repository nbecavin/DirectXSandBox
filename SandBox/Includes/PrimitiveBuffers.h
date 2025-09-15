#ifndef __PRIMITIVE_BUFFERS_HH__
#define __PRIMITIVE_BUFFERS_HH__

#define FMT_IDX_16	0
#define FMT_IDX_32	1

enum PrimitiveType
{
	PRIM_TRIANGLELIST = 0,
	PRIM_TRIANGLESTRIP,
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
	virtual void Create(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas) = 0;
	virtual bool IsInited() = 0;
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap MapFlags = WriteNoOverwrite) = 0;
	virtual void Unlock() = 0;
	virtual U32 GetSize() { return Size; }
protected:
	U32 Size;
};

typedef Buffer VertexBuffer;
typedef Buffer IndexBuffer;
typedef Buffer ConstantBuffer;

struct VertexElement
{
	U32		StreamId;
	U32		Offset;
	S32		Type;
	U32		Semantic;
	inline void END() { StreamId=0; Offset=0; Type=-1; Semantic=0; }
};
enum {
	DECL_FMT_FLOAT4 = 0,
	DECL_FMT_FLOAT3,
	DECL_FMT_FLOAT2,
	DECL_FMT_FLOAT1,

	DECL_FMT_COLOR,
    DECL_FMT_UBYTE4,
    DECL_FMT_SHORT2,
    DECL_FMT_SHORT4,
	DECL_FMT_UBYTE4N,
    DECL_FMT_SHORT2N,
    DECL_FMT_SHORT4N,
    DECL_FMT_USHORT2N,
    DECL_FMT_USHORT4N,

	DECL_FMT_UDEC3,
	DECL_FMT_DEC3N,
    DECL_FMT_FLOAT16_2,
    DECL_FMT_FLOAT16_4
};
enum {
	DECL_POSITION = 0,
	DECL_COLOR0,
	DECL_COLOR1,
	DECL_NORMAL,
	DECL_TANGENT,
	DECL_BINORMAL,
	DECL_TEXCOORD0,
	DECL_TEXCOORD1,
	DECL_TEXCOORD2,
	DECL_TEXCOORD3,
	DECL_TEXCOORD4,
	DECL_TEXCOORD5,
	DECL_TEXCOORD6,
	DECL_TEXCOORD7,
	DECL_TEXCOORD8,
};
#define DECL_END()	{ 0, 0, -1, 0 }

class VertexDeclaration {
public:
	virtual void Create(VertexElement* Decl) = 0;
};

class Shader {
public:
};

#endif //__PRIMITIVE_BUFFERS_HH__
