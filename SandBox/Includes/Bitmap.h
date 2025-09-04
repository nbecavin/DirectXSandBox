#ifndef __BITMAP_HH__
#define __BITMAP_HH__

#include <BitmapFlags.h>

class Bitmap : public GraphObject
{
protected:
	U8 *	Datas;
	U32		DataSize;
	U32		Sx,Sy,Sz;
	BitmapFormat		Format;
	U32		Mips;
	U32		MultisampleType;
	U32		Type;
	U32		Flags;
	U64		HwResId;

public:
	Bitmap() : Flags(BM_SRGB), Datas(NULL), DataSize(0), Sx(0), Sy(0), Sz(1), Format(BM_R8G8B8A8_UNORM), Mips(1), MultisampleType(BM_MSAA_NONE), Type(BM_TYPE_2D), HwResId(BM_INVALIDHWRESID) {}

	inline	void	SetData(void* pixels, U32 bytesize) {
		Datas = (U8*)pixels; DataSize = bytesize;
	}

	inline	void	SetSize(U32 _Sx,U32 _Sy,U32 _Sz=1);
	inline	void	SetFormat(BitmapFormat _Format) { Format=_Format; }
	inline	void	SetType(U32 _Type) { Type=_Type; }
	inline	void	EnableFlags(U32 _Flags) {Flags|=(_Flags);}
	inline	void	DisableFlags(U32 _Flags) {Flags&=(~(_Flags));}

	inline	U32		GetFlags() const { return Flags; }
	inline	U8 *	GetDatas() const { return Datas; }
	inline	U32		GetDataSize() const { return DataSize; }
	inline	U32		GetSx() const { return Sx; }
	inline	U32		GetSy() const { return Sy; }
	inline	U32		GetSz() const { return Sz; }
	inline	BitmapFormat GetFormat() const { return Format; }
	inline	U32		GetMips() const { return Mips; }
	inline	U32		GetType() const { return Type; }
	inline	U32		GetMultisampleType() const { return MultisampleType; }

	inline	void	BinHwResId(U64 _Id) { HwResId=_Id; }
	inline	U64		GetBinHwResId() const { return HwResId; }

public:
	bool	LoadDDS(const char* filename);
};

#include <Bitmap.inl.h>

#endif //__BITMAP_HH__
