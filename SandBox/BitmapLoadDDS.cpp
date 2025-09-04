#include <Bitmap.h>

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

static const char* GetErrorString(tinyddsloader::Result result)
{
	switch (result)
	{
	case tinyddsloader::Result::Success:return "Success";
	case tinyddsloader::Result::ErrorFileOpen: return "ErrorFileOpen";
	case tinyddsloader::Result::ErrorRead: return "ErrorRead";
	case tinyddsloader::Result::ErrorMagicWord: return "ErrorMagicWord";
	case tinyddsloader::Result::ErrorSize: return "ErrorSize";
	case tinyddsloader::Result::ErrorVerify: return "ErrorVerify";
	case tinyddsloader::Result::ErrorNotSupported: return "ErrorNotSupported";
	case tinyddsloader::Result::ErrorInvalidData: return "ErrorInvalidData";
	default:
		return "Unknwon";
	};
}

bool Bitmap::LoadDDS(const char* filename)
{
	tinyddsloader::DDSFile dds;

	fs::File fp;
	U32 fileLen;

	if (fp.Open(filename,fs::OPEN_READ)!=fs::ERROR_NONE)
	{
		MESSAGE("Bitmap::LoadDDS -> failed to open file %s",filename);
		return false;
	}

	fileLen = fp.GetSize();

	U8* buffer = new U8[fileLen];
	U32 readSize = fp.Read(buffer,1,fileLen);
	if ( readSize !=  fileLen )
	{
		delete [] buffer;
		return false;
	}

	fp.Close();

	tinyddsloader::Result loadResult = dds.Load( buffer, readSize );
	if(loadResult!= tinyddsloader::Success)
	{
		MESSAGE("Bitmap::LoadDDS -> failed to read DDS %s with error %s", filename, GetErrorString(loadResult));
		delete [] buffer;
		return false;
	}
	
	// Create Bitmap
	Sx = dds.GetWidth();
	Sy = dds.GetHeight();
	Sz = dds.GetDepth();
	Mips = 1;// dds.GetMipCount();
	Type = ((dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Texture2D) && (dds.GetArraySize() == 6)) ? BM_TYPE_CUBE : BM_TYPE_2D;

	// compute total size
	DataSize = 0;
	
	for (uint32_t arrayIdx = 0; arrayIdx < dds.GetArraySize(); arrayIdx++) {
		const auto* imageData = dds.GetImageData(0, arrayIdx);
		DataSize = imageData->m_memSlicePitch * dds.GetArraySize();
		//for (uint32_t mipIdx = 0; mipIdx < dds.GetMipCount(); mipIdx++) {
		//	const auto* imageData = dds.GetImageData(mipIdx, arrayIdx);
		//	DataSize += imageData->m_memPitch

		//	std::cout << "Array[" << arrayIdx << "] "
		//		<< "Mip[" << mipIdx << "]: "
		//		<< "(" << imageData->m_width << ", "
		//		<< imageData->m_height << ", " << imageData->m_depth
		//		<< ")\n";
		//}
	}

//	DataSize = dds.mBytelen;

	Datas = (U8*)malloc(DataSize*8);
	const auto* imageData = dds.GetImageData(0, 0);
	memcpy(Datas, (U8*)imageData->m_mem, imageData->m_memSlicePitch);
	delete[] buffer;

	switch (dds.GetFormat())
	{
	case tinyddsloader::DDSFile::DXGIFormat::BC1_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
	case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
		Format = BM_BC1_UNORM;
		break;
	case tinyddsloader::DDSFile::DXGIFormat::BC7_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
	case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
		Format = BM_BC7_UNORM;
		break;
	//case DDS_RGBA:		Format = BM_FORMAT_ABGR8888; break;
	//case DDS_RGB:
	//	Format = BM_FORMAT_ABGR8888; break;
	//case DDS_DXT1:		Format = BM_FORMAT_DXT1; break;
	//	//case DDS_DXT3:		Format = BM_DXT3; break;
	//case DDS_DXT5:		Format = BM_FORMAT_DXT5; break;
		//case DDS_R16F:		Format = BM_R16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
		//case DDS_R32F:		Format = BM_R32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
		//case DDS_G16R16F:	Format = BM_R16G16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
		//case DDS_G32R32F:	Format = BM_R32G32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
		//case DDS_RGBA16F:	Format = BM_R16G16B16A16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
		//case DDS_RGBA32F:	Format = BM_R32G32B32A32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	default:
		MESSAGE("LoadDDS Unknown bitmap format(%d)\n%s", dds.GetFormat(), filename);
		free(Datas);
		Datas = NULL;
		return false;
	}
	return true;
}
