#include <DdsLoader.h>

namespace SD
{

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct PixelFormatFlag
	{
		static const U32	ALPHA_PIXELS	= 0x00000001;
		static const U32	ALPHA			= 0x00000002;
		static const U32	FOUR_CC			= 0x00000004;
		static const U32	RGB				= 0x00000040;
		static const U32	RGBA			= 0x00000041;
		static const U32	LUMINANCE		= 0x00020000;
		static const U32	LUMINANCE_ALPHA	= 0x00020001;
	} PixelFormatFlag;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct Caps1Flag
	{
		static const U32	COMPLEX			= 0x00000008;
		static const U32	TEXTURE			= 0x00001000;
		static const U32	MIPMAP			= 0x00400000;
	} Caps1Flag;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct Caps2Flag
	{
		static const U32	CUBEMAP				= 0x00000200l;
		static const U32	CUBEMAP_POSITIVE_X	= 0x00000400l;
		static const U32	CUBEMAP_NEGATIVE_X	= 0x00000800l;
		static const U32	CUBEMAP_POSITIVE_Y	= 0x00001000l;
		static const U32	CUBEMAP_NEGATIVE_Y	= 0x00002000l;
		static const U32	CUBEMAP_POSITIVE_Z	= 0x00004000l;
		static const U32	CUBEMAP_NEGATIVE_Z	= 0x00008000l;
		static const U32	CUBEMAP_ALLFACES	= 0x0000FC00l;
		static const U32	VOLUME				= 0x00200000l;
	} Caps2Flag;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct FourCC
	{
		static const U32	A16B16G16R16	= 36;

		// s10e5 formats (16-bits per channel)
		static const U32	R16F			= 111;
		static const U32	G16R16F			= 112;
		static const U32	A16B16G16R16F	= 113;

		// IEEE s23e8 formats (32-bits per channel)
		static const U32	R32F			= 114;
		static const U32	G32R32F			= 115;
		static const U32	A32B32G32R32F	= 116;
		
		static const U32	DXT1			= 0x31545844;
		static const U32	DXT3			= 0x33545844;
		static const U32	DXT5			= 0x35545844;
	} FourCC;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct DdsPixelFormat
	{
		U32 dwSize;
		U32 dwFlags;
		U32 dwFourCC;
		U32 dwRGBBitCount;
		U32 dwRBitMask;
		U32 dwGBitMask;
		U32 dwBBitMask;
		U32 dwABitMask;
	} DdsPixelFormat;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	typedef struct DdsHeader
	{
		U32 dwSize;
		U32 dwFlags;
		U32 dwHeight;
		U32 dwWidth;
		U32 dwPitchOrLinearSize;
		U32 dwDepth;
		U32 dwMipMapCount;
		U32 dwReserved1[11];
		DdsPixelFormat ddpfPixelFormat;
		U32 dwCaps1;
		U32 dwCaps2;
		U32 dwReserved2[3];
	} DdsHeader;

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	S32 getPixelComponentSize( ComponentType component )
	{
		S32 size;
		switch ( component )
		{
		case COMPONENT_TYPE_BYTE:
		case COMPONENT_TYPE_UNSIGNED_BYTE:
			size = 1;
			break;
		case COMPONENT_TYPE_SHORT:
		case COMPONENT_TYPE_UNSIGNED_SHORT:
			size = 2;
			break;
		case COMPONENT_TYPE_INT:
		case COMPONENT_TYPE_UNSIGNED_INT:
			size = 4;
			break;
		case COMPONENT_TYPE_HALF_FLOAT:
			size = 2;
			break;
		case COMPONENT_TYPE_FLOAT:
			size = 4;
			break;
		default:
			fprintf( stderr, "unknown component type\n");
			size = 0;
		}
		return size;
	}

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	S32 getImageSize( U32 w, U32 h, U32 format, ComponentType component )
	{
		S32 componentSize = getPixelComponentSize( component );

		switch ( format )
		{
		// DXTC
		case DDS_DXT1:
			return ((w+3)/4)* ((h+3)/4)* 8;   
		case DDS_DXT3:
		case DDS_DXT5:
			return ((w+3)/4)*((h+3)/4)* 16;   

		// 1 component
		case DDS_R:
		case DDS_ALPHA:
		case DDS_LUMINANCE:
			return w*h*componentSize;

		// 2 components
		case DDS_GR:
		case DDS_LUMINANCE_ALPHA:
			return w*h*componentSize*2;

		// 3 components
		case DDS_RGB:
		case DDS_BGR:
			return w*h*componentSize*3;

		// 4 components
		case DDS_RGBA:
		case DDS_BGRA:
			return w*h*componentSize*4;

		default:
			fprintf( stderr, "unable to determine image size\n");
			return false;
		}
	}

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	Bool ImageSpec( U32& format, ComponentType& type, const DdsHeader& ddsh )
	{
		if ( ddsh.ddpfPixelFormat.dwFlags & PixelFormatFlag::FOUR_CC )	// its a compressed texture
		{
			switch( ddsh.ddpfPixelFormat.dwFourCC )
			{
			case FourCC::DXT1:
				format = DDS_DXT1;
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
				break;
			case FourCC::DXT3:
				format = DDS_DXT3;
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
				break;
			case FourCC::DXT5:
				format = DDS_DXT5;
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
				break;
			case FourCC::A16B16G16R16:
				format = DDS_RGBA;
				type = COMPONENT_TYPE_UNSIGNED_SHORT;
				break;
			case FourCC::A16B16G16R16F:
				format = DDS_RGBA;
				type = COMPONENT_TYPE_HALF_FLOAT;
				break;
			case FourCC::R32F:
				format = DDS_R;
				type = COMPONENT_TYPE_FLOAT;
				break;
			case FourCC::G32R32F:
				format = DDS_GR;
				type = COMPONENT_TYPE_FLOAT;
				break;
			case FourCC::A32B32G32R32F:
				format = DDS_RGBA;
				type = COMPONENT_TYPE_FLOAT;
				break;
			default:
				fprintf( stderr, "Uses a compressed texture of unsupported type\n");
				return false;
			}
		}
		else if ( ddsh.ddpfPixelFormat.dwFlags == PixelFormatFlag::RGBA )
		{
			if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 32 )
			{
				if ( ddsh.ddpfPixelFormat.dwRBitMask == 0x000000ff )
					format = DDS_RGBA;
				else
					format = DDS_BGRA; 
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else
			{
				fprintf( stderr, "unsupported pixel format type\n");
				return false;
			}
		}
		else if ( ddsh.ddpfPixelFormat.dwFlags == PixelFormatFlag::RGB )
		{
			if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 32 )
			{
				if ( ddsh.ddpfPixelFormat.dwRBitMask == 0x000000ff )
					format = DDS_RGBA;
				else
					format = DDS_BGRA; 
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 24 )
			{
				if ( ddsh.ddpfPixelFormat.dwRBitMask == 0x000000ff )
					format = DDS_RGB;
				else
					format = DDS_BGR;
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else
			{
				fprintf( stderr, "unsupported pixel format type\n");
				return false;
			}
		}
		else if ( ddsh.ddpfPixelFormat.dwFlags == PixelFormatFlag::ALPHA )
		{
			if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 8 &&
				 ddsh.ddpfPixelFormat.dwABitMask == 0x000000ff )
			{
				format = DDS_ALPHA; 
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else
			{
				fprintf( stderr, "unsupported pixel format type\n");
				return false;
			}
		}
		else if ( ddsh.ddpfPixelFormat.dwFlags == PixelFormatFlag::LUMINANCE_ALPHA )
		{
			if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 16 )
			{
				format = DDS_LUMINANCE_ALPHA; 
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else
			{
				fprintf( stderr, "unsupported pixel format type\n");
				return false;
			}
		}
		else if ( ddsh.ddpfPixelFormat.dwFlags == PixelFormatFlag::LUMINANCE )
		{
			format = DDS_LUMINANCE;
			if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 16 )
			{
				type = COMPONENT_TYPE_UNSIGNED_SHORT;
			}
			else if ( ddsh.ddpfPixelFormat.dwRGBBitCount == 8 )
			{
				type = COMPONENT_TYPE_UNSIGNED_BYTE;
			}
			else
			{
				fprintf( stderr, "unsupported pixel format type\n");
				return false;
			}
		}
		else 
		{
			fprintf( stderr, "Uses a texture of unsupported type\n");
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	Bool getInternalFormat( U32& internalFormat, U32 format, U32 type )
	{
		switch ( format )
		{
		case DDS_DXT1:
			internalFormat = DDS_DXT1;
			break;
		case DDS_DXT3:
			internalFormat = DDS_DXT3;
			break;
		case DDS_DXT5:
			internalFormat = DDS_DXT5;
			break;
		case DDS_R:
			switch ( type )
			{
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_R32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_R16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_GR:
			switch ( type )
			{
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_G32R32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_G16R16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_RGBA:
		case DDS_BGRA:
			switch ( type )
			{
			case COMPONENT_TYPE_BYTE:
			case COMPONENT_TYPE_UNSIGNED_BYTE:
				internalFormat = DDS_RGBA;
				break;
			case COMPONENT_TYPE_SHORT:
			case COMPONENT_TYPE_UNSIGNED_SHORT:
				internalFormat = DDS_RGBA16;
				break;
			case COMPONENT_TYPE_INT:
			case COMPONENT_TYPE_UNSIGNED_INT:
				internalFormat = DDS_RGBA;
				break;
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_RGBA32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_RGBA16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_RGB:
		case DDS_BGR:
			switch ( type )
			{
			case COMPONENT_TYPE_BYTE:
			case COMPONENT_TYPE_UNSIGNED_BYTE:
				internalFormat = DDS_RGB;
				break;
			case COMPONENT_TYPE_SHORT:
			case COMPONENT_TYPE_UNSIGNED_SHORT:
				internalFormat = DDS_RGB16;
				break;
			case COMPONENT_TYPE_INT:
			case COMPONENT_TYPE_UNSIGNED_INT:
				internalFormat = DDS_RGB;
				break;
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_RGB32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_RGB16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_ALPHA:
			switch ( type )
			{
			case COMPONENT_TYPE_BYTE:
			case COMPONENT_TYPE_UNSIGNED_BYTE:
				internalFormat = DDS_ALPHA8;
				break;
			case COMPONENT_TYPE_SHORT:
			case COMPONENT_TYPE_UNSIGNED_SHORT:
				internalFormat = DDS_ALPHA16;
				break;
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_ALPHA32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_ALPHA16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_LUMINANCE:
			switch ( type )
			{
			case COMPONENT_TYPE_BYTE:
			case COMPONENT_TYPE_UNSIGNED_BYTE:
				internalFormat = DDS_LUMINANCE8;
				break;
			case COMPONENT_TYPE_SHORT:
			case COMPONENT_TYPE_UNSIGNED_SHORT:
				internalFormat = DDS_LUMINANCE16;
				break;
			case COMPONENT_TYPE_INT:
			case COMPONENT_TYPE_UNSIGNED_INT:
				internalFormat = DDS_LUMINANCE;
				break;
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_LUMINANCE32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_LUMINANCE16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		case DDS_LUMINANCE_ALPHA:
			switch ( type )
			{
//			case COMPONENT_TYPE_BYTE:
//			case COMPONENT_TYPE_UNSIGNED_BYTE:
//				internalFormat = GL_LUMINANCE8_ALPHA8;
//				break;
			case COMPONENT_TYPE_SHORT:
			case COMPONENT_TYPE_UNSIGNED_SHORT:
				internalFormat = DDS_LUMINANCE16_ALPHA16;
				break;
			case COMPONENT_TYPE_FLOAT:
				internalFormat = DDS_LUMINANCE32F_ALPHA32F;
				break;
			case COMPONENT_TYPE_HALF_FLOAT:
				internalFormat = DDS_LUMINANCE16F_ALPHA16F;
				break;
			default:
				fprintf( stderr, "unsupported component type\n");
				return false;
			}
			break;
		default:
			fprintf( stderr, "unsupported format\n");
			return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------------
	DdsTexture::DdsTexture()
	{
		memset( this, 0, sizeof(DdsTexture) );
	}

	//-----------------------------------------------------------------------------
	DdsTexture::~DdsTexture()
	{
		dispose();
	}

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	void	DdsTexture::dispose()
	{
		memset( this, 0, sizeof(DdsTexture) );
	}

	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	Bool	DdsTexture::load( const void* data, bool swaphdr )
	{
		U8 *buff;
		buff = (U8*)data;

		// read in file marker, make sure its a DDS file
		if ( strncmp( (char*)buff, "DDS ", 4 ) != 0 )
		{
			MESSAGE("This is not a dds file");
			return false;
		}
		buff += 4;

		DdsHeader *ddsh;
		ddsh = (DdsHeader*)buff;
		buff += sizeof(DdsHeader);

		if(swaphdr)
		{
			U32* t = (U32*)ddsh;
			for ( U32 i = 0; i<sizeof(DdsHeader) / 4; i++ )
			{
				U32 v = *(t+i);
				*(t + i) = ((v & 0xff) << 24) | ((v & 0xff00) << 8) | ((v & 0xff0000) >> 8)  | ((v & 0xff000000) >> 24);
			}
		}

		TextureType textureType = TEXTURE_TYPE_FLAT;
		if ( ddsh->dwCaps2 & Caps2Flag::CUBEMAP ) textureType = TEXTURE_TYPE_CUBEMAP;

		// check if image is a volume texture
		if ( (ddsh->dwCaps2 & Caps2Flag::VOLUME) && (ddsh->dwDepth > 0) )
		{
			MESSAGE("This is a volume texture \n" );
			return false;
		}

		// get the format of the image
		U32 format;
		ComponentType componentType;

		// get the texture format and number of color channels
		Bool specTest = ImageSpec( format, componentType, *ddsh );
		if ( !specTest )
			return false;

		mFormat			= format;
		mComponentType	= componentType;
		mHeight			= ddsh->dwHeight;
		mWidth			= ddsh->dwWidth;

		if ( ddsh->dwMipMapCount == 0 ) ddsh->dwMipMapCount++;

		mMips			= ddsh->dwMipMapCount;

		mSurfaces		= 1;
		if ( textureType == TEXTURE_TYPE_CUBEMAP ) mSurfaces = 6;

		mIsCompressed	= ( DDS_DXT1 == format ||
							DDS_DXT3 == format ||
							DDS_DXT5 == format );
	
		// get pointers to the pixel data
		mBytelen = 0;
		U32 numComponents = getPixelComponentSize( componentType );
		for ( U32 i = 0; i < mSurfaces; i++ )
		{
			U32 width = ddsh->dwWidth;
			U32 height = ddsh->dwHeight;

			for ( U32 j = 0; j < mMips; j++ )
			{
				U32 size = getImageSize( width, height, format, componentType );
				mBytelen += size;
				mImage[i].pixels[j] = buff;

				buff += size;
				width /= 2;
				height /= 2;
			}
		}

		if ( !getInternalFormat( mInternalFormat, mFormat, mComponentType ) )
			return false;

		MESSAGE( "dds surfaces %d mips %d w %d h%d f0x%X t%d", mSurfaces, mMips, mWidth, mHeight, mFormat, mComponentType );
		return true;
	}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
Bool loadDdsTexture(
	U32& texture, TexParam& param, const char* filename,
	Bool videoMemPool)
{
	fs::File fp;
	U32 fileLen;

	if (fp.Open(filename,fs::OPEN_READ)!=fs::ERROR_NONE)
	{
		fprintf( stderr, "failed to load %s\n",filename);
		return false;
	}

	fileLen = fp.GetSize();

	U8 *buffer = new U8[fileLen];
	U32 readSize = fp.Read(buffer,1,fileLen);
	if ( readSize !=  fileLen )
	{
		fprintf( stderr, "faild to read %s\n", filename );
		delete [] buffer;
		fp.Close();
		return false;
	}

	fp.Close();

	Bool roadTest = loadDdsTextureFromMemory( texture, param, buffer, videoMemPool);
	
	delete [] buffer;
	return roadTest;
}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
Bool loadDdsTextureFromMemory(
	U32& texture, TexParam& param, const void* data,
	Bool videoMemPool)
{
	DdsTexture dds;
	Bool loadResult = dds.load( data );
	if ( !loadResult )
		return false;
	
	U32 interlanFormat;
	if ( !getInternalFormat( interlanFormat, dds.mFormat, dds.mComponentType ) )
		return false;

	param.format = interlanFormat;
	param.width = dds.mWidth;
	param.height = dds.mHeight;
	param.mipmapLevel = dds.mMips;
	param.isCubeMap = dds.isCubeMap();

	return true;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
Bool loadDdsTexture(
	U32& texture, const char* filename,
	Bool videoMemPool)
{
	TexParam param;
	return loadDdsTexture( texture, param, filename, videoMemPool);
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
Bool loadDdsTextureFromMemory(
	U32& texture, const void* data,
	Bool videoMemPool)
{
	TexParam param;
	return loadDdsTextureFromMemory( texture, param, data, videoMemPool);
}

} // namespace SD
