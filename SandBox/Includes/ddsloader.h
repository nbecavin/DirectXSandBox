#ifndef __DDS_LOADER_H__
#define __DDS_LOADER_H__

namespace SD
{

	struct TexParam
	{
		S32				width;			//
		S32				height;			//
		S32				format;			//
		S32				mipmapLevel;	//
		Bool			isCubeMap;		//
	};

	// surface description flags
	#define gMaxMipmaps				16
	#define gMaxTextures			16

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	enum ComponentType
	{
		COMPONENT_TYPE_BYTE = 0,
		COMPONENT_TYPE_UNSIGNED_BYTE,
		COMPONENT_TYPE_SHORT,
		COMPONENT_TYPE_UNSIGNED_SHORT,
		COMPONENT_TYPE_INT,
		COMPONENT_TYPE_UNSIGNED_INT,
		COMPONENT_TYPE_HALF_FLOAT,
		COMPONENT_TYPE_FLOAT
	};

	enum TextureType
	{
		TEXTURE_TYPE_NONE,
		TEXTURE_TYPE_FLAT,    // 1D, 2D, and rectangle textures
		TEXTURE_TYPE_3D,
		TEXTURE_TYPE_CUBEMAP
	};
		
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct DdsImage
	{
		U8*				pixels[gMaxMipmaps];		// the mip map images 
	} DdsImage;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	class DdsTexture
	{
	public:
		Bool			mIsCompressed;	//
		ComponentType	mComponentType;	//
		U32				mFormat;		//
		U32				mInternalFormat;
		U32				mWidth;			//
		U32				mHeight;		//
		U32				mMips;			//
		U32				mSurfaces;		// ( 1 = a texture 6 = a cube map)
		U32				mBytelen;

		DdsImage		mImage[6];		//

		DdsTexture();
		~DdsTexture();

		//
		void	dispose();

		//
		Bool	load( const void* data, bool swaphdr=false );

		//
		Bool	isCubeMap() const {  return (mSurfaces == 6);  }

	};

	//
	extern Bool loadDdsTexture(
		U32& texture, TexParam& param, const char* filename,
		Bool videoMemPool = true);

	//
	extern Bool loadDdsTextureFromMemory(
		U32& texture, TexParam& param, const void* data,
		Bool videoMemPool = true);

	//
	extern Bool loadDdsTexture(
		U32& texture, const char* filename,
		Bool videoMemPool = true);

	//
	extern Bool loadDdsTextureFromMemory(
		U32& texture, const void* data,
		Bool videoMemPool = true);


} // namespace SD

enum DdsImageFormat {
	DDS_DXT1,
	DDS_DXT3,
	DDS_DXT5,

	DDS_R,
	DDS_GR,
	DDS_RGBA,
	DDS_BGRA,
	DDS_RGB,
	DDS_BGR,

	DDS_R16F,
	DDS_R32F,

	DDS_G16R16F,
	DDS_G32R32F,

	DDS_RGBA16,
	DDS_RGBA16F,
	DDS_RGBA32F,

	DDS_RGB16,
	DDS_RGB16F,
	DDS_RGB32F,

	DDS_ALPHA,
	DDS_ALPHA8,
	DDS_ALPHA16,
	DDS_ALPHA32F,
	DDS_ALPHA16F,

	DDS_LUMINANCE,
	DDS_LUMINANCE8,
	DDS_LUMINANCE16,
	DDS_LUMINANCE32F,
	DDS_LUMINANCE16F,

	DDS_LUMINANCE_ALPHA,
	DDS_LUMINANCE16_ALPHA16,
	DDS_LUMINANCE32F_ALPHA32F,
	DDS_LUMINANCE16F_ALPHA16F
};

enum DdsTextureType {
	DDS_TEXTURE_2D,
	DDS_TEXTURE_CUBE
};

#endif // __DDS_LOADER_H__
