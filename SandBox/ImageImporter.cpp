#include <ImageImporter.h>
#include <DirectXTex.h>
#include <filesystem>
#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>
#include <Bitmap.h>

using namespace DirectX;

//
// WIC (PNG, JPEG, etc...) Loader
//

bool ImageImporter::LoadFromWIC(const std::filesystem::path& path, Bitmap* texture)
{
	fs::File fp;
	
	if (fp.OpenInMemory(path.string().c_str(), fs::OPEN_READ) != fs::ERROR_NONE)
	{
		MESSAGE("ImageImporter::LoadFromWIC -> failed to open file %s", path.string().c_str());
		return false;
	}

    ScratchImage image;
    WIC_FLAGS flags = WIC_FLAGS_NONE;

    TexMetadata metadata;
    HRESULT hr = LoadFromWICMemory((const std::byte*)fp.GetBlob().ptr, fp.GetBlob().size, flags, &metadata, image);

	// Create Bitmap
	texture->SetSize(metadata.width, metadata.height, metadata.depth);
	texture->SetMipLevels(metadata.mipLevels);
	//texture->SetType(
	//	((dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Texture2D) && (dds.GetArraySize() == 6)) ? BM_TYPE_CUBE : BM_TYPE_2D);
	texture->SetType(BM_TYPE_2D);

    return false;
}

//
// DDS Loader
//

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

bool ImageImporter::LoadFromDDS(const std::filesystem::path& path, Bitmap* texture)
{
	fs::File fp;
	if (fp.OpenInMemory(path.string().c_str(), fs::OPEN_READ) != fs::ERROR_NONE)
	{
		MESSAGE("ImageImporter::LoadFromDDS -> failed to open file %s", path.string().c_str());
		return false;
	}

	tinyddsloader::DDSFile dds;
	tinyddsloader::Result loadResult = dds.Load(fp.GetBlob().ptr, fp.GetBlob().size);
	if (loadResult != tinyddsloader::Success)
	{
		MESSAGE("ImageImporter::LoadFromDDS -> failed to read DDS %s with error %s", path.string().c_str(), GetErrorString(loadResult));
		return false;
	}

	// Create Bitmap
	texture->SetSize(dds.GetWidth(), dds.GetHeight(), dds.GetDepth());
	texture->SetMipLevels(dds.GetMipCount());
	texture->SetType(((dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Texture2D) && (dds.GetArraySize() == 6)) ? BM_TYPE_CUBE : BM_TYPE_2D);

	// Compute total size
	U32 dataSize = 0;
	for (uint32_t arrayIdx = 0; arrayIdx < dds.GetArraySize(); arrayIdx++)
	{
		for (uint32_t mipIdx = 0; mipIdx < dds.GetMipCount(); mipIdx++)
		{
			const auto* imageData = dds.GetImageData(mipIdx, arrayIdx);
			//DataSize += imageData->m_memPitch * imageData->m_height;
			dataSize += imageData->m_memSlicePitch;
		}
	}

	// Allocate
	U8* data = (U8*)malloc(dataSize);
	texture->SetData(data, dataSize);	

	// Copy data
	void* writePtr = data;
	for (uint32_t arrayIdx = 0; arrayIdx < dds.GetArraySize(); arrayIdx++)
	{
		for (uint32_t mipIdx = 0; mipIdx < dds.GetMipCount(); mipIdx++)
		{
			const auto* imageData = dds.GetImageData(mipIdx, arrayIdx);
			memcpy(writePtr, (U8*)imageData->m_mem, imageData->m_memSlicePitch);
			writePtr = (char*)writePtr + imageData->m_memSlicePitch;
		}
	}

	switch (dds.GetFormat())
	{
	case tinyddsloader::DDSFile::DXGIFormat::BC1_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
	case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
		texture->SetFormat(BM_BC1_UNORM);
		break;
	case tinyddsloader::DDSFile::DXGIFormat::BC3_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
	case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
		texture->SetFormat(BM_BC3_UNORM);
		break;
	case tinyddsloader::DDSFile::DXGIFormat::BC5_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
		texture->SetFormat(BM_BC5_UNORM);
		break;
	case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm:
		texture->SetFormat(BM_BC5_UNORM);
		break;
	case tinyddsloader::DDSFile::DXGIFormat::BC7_Typeless:
	case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
	case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
		texture->SetFormat(BM_BC7_UNORM);
		break;
		//BC6H_Typeless = 94,
		//BC6H_UF16 = 95,
		//BC6H_SF16 = 96,

		//case DDS_RGBA:		Format = BM_FORMAT_ABGR8888; break;
		//case DDS_RGB:
		//	Format = BM_FORMAT_ABGR8888; break;
			//case DDS_R16F:		Format = BM_R16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
			//case DDS_R32F:		Format = BM_R32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
			//case DDS_G16R16F:	Format = BM_R16G16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
			//case DDS_G32R32F:	Format = BM_R32G32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
			//case DDS_RGBA16F:	Format = BM_R16G16B16A16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
			//case DDS_RGBA32F:	Format = BM_R32G32B32A32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	default:
		MESSAGE("ImageImporter::LoadFromDDS Unknown bitmap format(%d)\n%s", dds.GetFormat(), path.string().c_str());
		texture->FreeData();
		return false;
	}
	return true;
}
