#ifndef __BITMAP_FLAGS_HH__
#define __BITMAP_FLAGS_HH__

enum {
	BM_FORMAT_ABGR8888,
	BM_FORMAT_DXT1,
	BM_FORMAT_DXT5,
	BM_FORMAT_R32F
};

enum {
	BM_TYPE_2D		= (1<<0),
	BM_TYPE_3D		= (1<<1),
	BM_TYPE_CUBE	= (1<<2),

	BM_TYPE_RT		= (1<<7)
};

enum {
	BM_MSAA_NONE,
	BM_MSAA_2X,
	BM_MSAA_4X,
	BM_MSAA_8X,
	BM_MSAA_16X
};

#define BM_SRGB		(1<<0)

#define BM_INVALIDHWRESID	(-1)

#endif //__BITMAP_FLAGS_HH__
