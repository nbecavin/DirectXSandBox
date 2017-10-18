#include <Bitmap.h>
#include <ddsloader.h>

bool Bitmap::LoadDDS(const char* filename)
{
	SD::DdsTexture dds;

	fs::File fp;
	U32 fileLen;

	if (fp.Open(filename,fs::OPEN_READ)!=fs::ERROR_NONE)
	{
		MESSAGE("failed to load %s\n",filename);
		return false;
	}

	fileLen = fp.GetSize();

	U8* buffer = new U8[fileLen];
	U32 readSize = fp.Read(buffer,1,fileLen);
	if ( readSize !=  fileLen )
	{
		MESSAGE("faild to read %s\n", filename );
		delete [] buffer;
		return false;
	}

	fp.Close();

	Bool loadResult = dds.load( buffer );
	if(!loadResult)
	{
		MESSAGE("faild to read %s\n", filename);
		delete [] buffer;
		return false;
	}
	
	// Create Bitmap
	Datas = (U8*)malloc(dds.mBytelen);
	memcpy(Datas,(U8*)dds.mImage[0].pixels[0],dds.mBytelen);
	//BITMAP_DATA_FREE(Datas);
	delete [] buffer;

	Sx = dds.mWidth;
	Sy = dds.mHeight;
	//Sz = dds.mDepth;
	DataSize = dds.mBytelen;
	Mips = dds.mMips;
	Type = dds.mSurfaces==6?BM_TYPE_CUBE:BM_TYPE_2D;

	switch(dds.mInternalFormat)
	{
	case DDS_RGBA:		Format = BM_FORMAT_ABGR8888; break;
	case DDS_RGB:
		Format = BM_FORMAT_ABGR8888; break;
	case DDS_DXT1:		Format = BM_FORMAT_DXT1; break;
	//case DDS_DXT3:		Format = BM_DXT3; break;
	case DDS_DXT5:		Format = BM_FORMAT_DXT5; break;
	//case DDS_R16F:		Format = BM_R16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	//case DDS_R32F:		Format = BM_R32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	//case DDS_G16R16F:	Format = BM_R16G16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	//case DDS_G32R32F:	Format = BM_R32G32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	//case DDS_RGBA16F:	Format = BM_R16G16B16A16F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	//case DDS_RGBA32F:	Format = BM_R32G32B32A32F; Bm->EnableFlag(FL_BITMAP_CONVERTED); break;
	default:
		MESSAGE("LoadDDS Unknown bitmap format(%d)\n%s",dds.mInternalFormat,filename);
		free(Datas);
		Datas = NULL;
		return false;
	}

	return true;
}
