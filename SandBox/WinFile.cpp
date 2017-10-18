#if defined(_PC)

#include <WinFile.h>

using namespace fs;

FileHandler * fs::NewFileHandler() { return new WinFileHandler(); }

WinFileHandler::WinFileHandler()
{
	fp = NULL;
}

FileError WinFileHandler::Open(const char* _filename, FileFlags _flags)
{
	char realfilename[1024];

#if WINAPI_FAMILY_ONE_PARTITION( WINAPI_FAMILY, WINAPI_PARTITION_APP )
	Windows::Storage::StorageFolder ^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	Platform::String ^ localPath = localFolder->Path;

	const WCHAR * __p = localPath->Data();
	WideCharToMultiByte(CP_ACP,0,__p,-1,realfilename,1023,NULL,FALSE);
	
	//strcpy(realfilename,"ms-appdata:///local/");
	//strcpy(realfilename,(char*)localPath);
	strcat(realfilename,"\\");
	strcat(realfilename,_filename);
#else
	strcpy(realfilename,_filename);
#endif

	char mode[16];
	if(_flags&OPEN_READ)
		strcpy(mode,"rb");
	fp = fopen(realfilename,mode);
	if(fp)
		return ERROR_NONE;
	else
	{
		MESSAGE("Can't open file %s", _filename);
		return ERROR_OPEN;
	}
}

void WinFileHandler::Close()
{
	if(fp)
	{
		fclose(fp);
		fp=NULL;
	}
}

U32 WinFileHandler::Read(void* _buffer,U32 _eltsize,U32 _eltcount)
{
	if(fp)
		return fread(_buffer,_eltsize,_eltcount,fp);
	else
		return 0;
}

void WinFileHandler::Write()
{
}

void WinFileHandler::SetPos(FileSeek _seek)
{
	//fseek(fp,0,SEEK_END);
}

U32 WinFileHandler::GetPos()
{
	if(fp)
		return ftell(fp);
	else
		return 0;
}

U32 WinFileHandler::GetSize()
{
	if(!fp) return 0;
	int curpos = fseek(fp,0,SEEK_END);
	int filesize = ftell(fp);
	fseek(fp,curpos,SEEK_SET);
	return filesize;
}

#endif //_PC
