#if defined(_ANDROID)

#include <AndroidFile.h>

using namespace fs;

FileHandler * fs::NewFileHandler() { return new AndroidFileHandler(); }

AndroidFileHandler::AndroidFileHandler()
{
	fp = NULL;
}

FileError AndroidFileHandler::Open(const char* _filename, FileFlags _flags)
{
	char mode[16];
	char realfilename[1024];
	GetRealFileName(_filename,realfilename);

	if(_flags&OPEN_READ)
		strcpy(mode,"rb");
	fp = fopen(realfilename,mode);
	if(fp)
		return ERROR_NONE;
	else
	{
		MESSAGE("Can't open file %s", realfilename);
		return ERROR_OPEN;
	}
}

void AndroidFileHandler::Close()
{
	if(fp)
	{
		fclose(fp);
		fp=NULL;
	}
}

U32 AndroidFileHandler::Read(void* _buffer,U32 _eltsize,U32 _eltcount)
{
	if(fp)
		return fread(_buffer,_eltsize,_eltcount,fp);
	else
		return 0;
}

void AndroidFileHandler::Write()
{
}

void AndroidFileHandler::SetPos(FileSeek _seek)
{
	//fseek(fp,0,SEEK_END);
}

U32 AndroidFileHandler::GetPos()
{
	if(fp)
		return ftell(fp);
	else
		return 0;
}

U32 AndroidFileHandler::GetSize()
{
	if(!fp) return 0;
	int curpos = fseek(fp,0,SEEK_END);
	int filesize = ftell(fp);
	fseek(fp,curpos,SEEK_SET);
	return filesize;
}

void AndroidFileHandler::GetRealFileName(const char* _filename,char* _realfilename)
{
	char tmpstr[1024];
	strcpy(tmpstr,_filename);
	int strsize = strlen(tmpstr);
	for(int i=0;i<strsize;i++)
		if(tmpstr[i]=='\\')
			tmpstr[i]='/';
	sprintf(_realfilename,"/sdcard/SandBox/%s",tmpstr);
}

#endif //_ANDROID
