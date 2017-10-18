#ifndef __WINFILE_HH__
#define __WINFILE_HH__

#include <SandBoxFile.h>

namespace fs {

	class WinFileHandler : public FileHandler
	{
	public:
		WinFileHandler();
		virtual FileError	Open(const char* _filename, FileFlags _flags);
		virtual void		Close();
		virtual U32			Read(void* _buffer,U32 _eltsize,U32 _eltcount);
		virtual void		Write();
		virtual void		SetPos(FileSeek _seek);
		virtual U32			GetPos();
		virtual U32			GetSize();

		FILE * fp;
	};

	extern FileHandler * NewFileHandler();

};

#endif //__WINFILE_HH__
