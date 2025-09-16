#ifndef __SANDBOXFILE_HH__
#define __SANDBOXFILE_HH__

//
// File handle wrapper class

namespace fs {

	enum FileError {
		ERROR_NONE = 0,
		ERROR_OPEN = 1,
		ERROR_READ = 1
	};
	
	enum FileFlags {
		OPEN_READ = (1<<0),
		OPEN_WRITE = (1<<1)
	};

	enum FileSeek {
		FSEEK_SET = 0,
		FSEEK_CUR = 1,
		FSEEK_END = 2
	};

	class FileHandler;

	class File
	{
	public:
		File() : m_Hdl(NULL) {}
		virtual ~File() { Close(); }
		virtual FileError	Open(const char* _filename, FileFlags _flags);
		virtual void		Close();
		virtual U32			Read(void* _buffer,U32 _eltsize,U32 _eltcount);
		virtual void		Write();
		virtual void		SetPos(FileSeek _seek);
		virtual U32			GetPos();
		virtual U32			GetSize();

		struct Blob
		{
			U8* ptr = nullptr;
			size_t size = 0;

			void Alloc(size_t _size)
			{
				ptr = (U8*)malloc(_size);
				size = _size;
			}
			void Release()
			{
				if(ptr) free(ptr);
				size = 0;
				ptr = nullptr;
			}
		};

		virtual FileError	OpenInMemory(const char* _filename, FileFlags _flags);
		virtual Blob		GetBlob() { return m_Blob; }

	protected:
		FileHandler * m_Hdl;
		Blob m_Blob;
	};

	class FileHandler {
	public:
		virtual FileError	Open(const char* _filename, FileFlags _flags) = 0;
		virtual void		Close() = 0;
		virtual U32			Read(void* _buffer,U32 _eltsize,U32 _eltcount) = 0;
		virtual void		SetPos(FileSeek _seek) = 0;
		virtual U32			GetPos() = 0;
		virtual U32			GetSize() = 0;
	};

	extern FileHandler * NewFileHandler();

};

#endif //__SANDBOXFILE_HH__
