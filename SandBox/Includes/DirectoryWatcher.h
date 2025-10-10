// Copyright (c) Asobo Studio, All rights reserved. www.asobostudio.com
#pragma once
#ifndef	_DIRECTORY_WATCHER_Z_H
#define	_DIRECTORY_WATCHER_Z_H

#include <DynArray.h>
//#include <HashTable_Z.h>
//#include <Thread_Z.h>
//#include <Mutex_Z.h>
//#include <HashMap_Z.h>

class DirectoryWatcher_Z
{
public:
	//! \brief File action flags. Any operations on the file will set the respective flags, with some exceptions.
	//
	// - Any operations on a file will set the respective flags. 
	// - Most, if not all, file operations will typically be accompanied by Modified.
	// - 'Added' and Removed are exclusive (only the most recent action of the two is kept).
	// - `Renamed` will be set together with an `Added` or a `Removed` flag, indicating whether it's the new or the old file name respectively. This is because a file rename action is composed of two separate actions in the windows API: FILE_ACTION_RENAMED_OLD_NAME and FILE_ACTION_RENAMED_NEW_NAME.
	// - If a file name changes, but e.g. only from upper case to lower case, you will only see Renamed and Added, not Removed (as they are exclusive, and the new file name is the most recent action)
	//
	enum class EFileActionFlags : U32
	{
		Added		= 1 << 1,//FILE_ACTION_ADDED,
		Removed		= 1 << 2,//FILE_ACTION_REMOVED,
		Modified	= 1 << 3,//FILE_ACTION_MODIFIED,

		Renamed		= 1 << 4
	};
	DECLARE_INLINE_ENUM_FLAG_OPERATORS_IN_CLASS_Z(EFileActionFlags);
	
	// The path of the file is the key in the hashmap.
	class ModifiedFileData
	{
	public:
		S32					NotifyCounter=0; //!< Gu's mysterious variable copy-pasted from Hash_Z
		EFileActionFlags	FileActionFlags=EFileActionFlags(0);
		S64					Ref=-1; //!< "Legacy". For AddFile which takes a ref for external array indexing or file name crc
	};

//	typedef HashMap_Z<String_Z<>, ModifiedFileData> ModifiedFileDataHM;

private:
	OVERLAPPED					mOverlapped;
	DirectoryWatcher_Z	*		pThis; // Sneakily used, see WatchCallback or ask Gu
	HANDLE						mStartedHandle;
	HANDLE						mDirHandle;
	BYTE				*		pBuffer;
	DWORD						mBufferSize;
	LPARAM						lParam;
	DWORD						mNotifyFilter;
	Bool						mWatchOnlyAddedFiles,mStopNow,bNeedRestart;
	U32							mRestartedCount = 0;
	static constexpr	U32		sMaxRestartCount = 5;
//	Thread_Z					mThread;
	Bool						bThreadLoop;
	U8DA						mErrorTxt;

				Bool			RefreshWatch(Bool _clear=FALSE);
				Bool			EnablePrivilege(LPCTSTR pszPrivName, Bool fEnable);
				Bool			SecurityWrite(const char *pDirectory,Bool bOutputSecurityDescriptor);

		friend	U32				CallDirWatcherThreadUpdate(void* userdata);//Thread Proc
		friend	void CALLBACK	WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
				void			Error(const char *,...);
		static	void CALLBACK	CallDirWatcherThreadTerminate( __in ULONG_PTR Parameter);
		friend	void CALLBACK	CallDirWatcherThreadTerminate( __in ULONG_PTR Parameter);
public:									
	DirectoryWatcher_Z();
	virtual	~DirectoryWatcher_Z();
			Bool			Start(Bool bWatchOnlyAddedFiles=TRUE); //!< bWatchOnlyAddedFiles means that only files that were added with AddFile will be added to the modified file list.
			Bool			Stop();
			Bool			ResolveRestart();
			Bool			IsStarted();
	inline const char *		GetDirectory()const							{return mDirName.c_str(); }
	inline const char *		GetError()const								{return (const char *)mErrorTxt.GetArrayPtr(); }
	inline	S32				GetLastNotifyCounter() const				{return mLastNotifyCounter;}
public:
			Bool			SetWatchedDirectory(const char* pDirectory); //!< Set the directory to watch and clears the list of files to watch that were previously added.

/*
			Bool			AddFile(const char *pFileName);
			Bool			AddFile(const char *pFileName,const Function_Z<S64(void)>& createRefOnAddSucceed);
			Bool			RemoveFile(const char *pFileName);
			S64				GetFileRef(const char *pFileName);
			Bool			RemoveModifiedFile(const char *pFileName);
			Bool			TryMakeRelativePath(const char* pFileName, String_Z<>& relativePath);
			Bool			GetModifiedFiles(ModifiedFileDataHM &HM,const Bool bFlushModifiedFiles=TRUE,const Double WaitSilenceSecond=4.f);
*/

private:
	std::string					mDirName;
	double						mLastModifiedAbsoluteTime;
	S32							mLastNotifyCounter = 0;//global counter incremented each time a FileActionFlag happen, it doesn't matter if we listen to a known list of files
//	SharedResource_Z			mMutex;
//	HashTableBase_Z<NameID_ZHashT_Z<S64>>	mCrcFileNameHT;

	//! A hashmap storing a pair of file path and modification data. The key is the path to the file, relative to the watched directory.
//	ModifiedFileDataHM			mModifiedHM;
};

#endif