#ifndef __ANDROIDFILE_HH__
#define __ANDROIDFILE_HH__

#include <SandBoxFile.h>

namespace fs {

	class AndroidFileHandler : public FileHandler
	{
	public:
		AndroidFileHandler();
		virtual FileError	Open(const char* _filename, FileFlags _flags);
		virtual void		Close();
		virtual U32			Read(void* _buffer,U32 _eltsize,U32 _eltcount);
		virtual void		Write();
		virtual void		SetPos(FileSeek _seek);
		virtual U32			GetPos();
		virtual U32			GetSize();

		void GetRealFileName(const char* _filename,char* _realfilename);

		FILE * fp;
	};

	extern FileHandler * NewFileHandler();

};

#endif //__WINFILE_HH__
