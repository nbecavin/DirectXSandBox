// Copyright (c) Asobo Studio, All rights reserved. www.asobostudio.com
#include <DirectoryWatcher.h>

#if	defined(_DIRECTORYWATCHER_Z)
//#include <WinThread_Z.h>
//#include <FilesystemUtils_Z.h>

DirectoryWatcher_Z::DirectoryWatcher_Z()
{
	pBuffer=NULL; 
	mBufferSize=16*1024;
	mStopNow=bNeedRestart=FALSE;
	mStartedHandle=INVALID_HANDLE_VALUE;
	mDirHandle=INVALID_HANDLE_VALUE;
	pThis=this;
	mLastModifiedAbsoluteTime=0.f;
}

DirectoryWatcher_Z::~DirectoryWatcher_Z()
{
	Stop();
}


Bool  DirectoryWatcher_Z::IsStarted()
{
	return mThread.IsInited();
}

U32 CallDirWatcherThreadUpdate(void *pUserData);

Bool DirectoryWatcher_Z::Start(Bool bWatchOnlyAddedFiles)
{
	if (mDirName.IsEmpty())
	{
		Error("DirectoryWatcher_Z directory not set or previously set directory was invalid.");
		return FALSE;
	}

	mWatchOnlyAddedFiles=bWatchOnlyAddedFiles;

	mErrorTxt.Flush();
	if(mDirHandle!=INVALID_HANDLE_VALUE)
	{
		Error("DirectoryWatcher_Z already done on %s",mDirName.Get());
		return FALSE;
	}

	mStopNow=FALSE;
	mModifiedHM.Clear();
	memset(&mOverlapped,0,sizeof(mOverlapped));
	pBuffer=(BYTE *)AllocAlign_Z(mBufferSize,128);

	ThreadParam_Z thp;
	thp.Name = "ACE: DirectoryWatcher_Z";
	thp.ThreadProc = &CallDirWatcherThreadUpdate;
	thp.StackSize = 0x40000;
	thp.UserData = this;
	bThreadLoop=TRUE;
	if(!mThread.IsInited())
		mThread.Init(thp);

	mStartedHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	mThread.Start();
	WaitForSingleObject(mStartedHandle, INFINITE);
	CloseHandle(mStartedHandle);
	mStartedHandle=INVALID_HANDLE_VALUE;

	OUTPUT_Z("ThreadID %d DirectoryWatcher_Z on Folder %s",mThread.GetThreadId(),mDirName.Get());
	return	TRUE;
}

Bool DirectoryWatcher_Z::EnablePrivilege(LPCTSTR pszPrivName, Bool fEnable) 
{    
	HANDLE hToken;    
	if (!OpenProcessToken(GetCurrentProcess(), 		
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 	
	{
		DirectoryWatcher_Z::Error("DirectoryWatcher_Z Failed OpenProcessToken %s", pszPrivName);
		return FALSE;
	}
	TOKEN_PRIVILEGES tp = { 1 };        
	if(!LookupPrivilegeValue(NULL, pszPrivName,  &tp.Privileges[0].Luid) )
	{
		CloseHandle(hToken);
		DirectoryWatcher_Z::Error("DirectoryWatcher_Z Failed LookupPrivilegeValue %s", pszPrivName);
		return FALSE;
	}
	tp.Privileges[0].Attributes = fEnable ?  SE_PRIVILEGE_ENABLED : 0;
	if(!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
	{
		CloseHandle(hToken);
		DirectoryWatcher_Z::Error("DirectoryWatcher_Z Failed LookupPrivilegeValue %s", pszPrivName);
		return FALSE;
	}
	CloseHandle(hToken);
	return (GetLastError() == ERROR_SUCCESS);
}

Bool DirectoryWatcher_Z::Stop()
{
	if(INVALID_HANDLE_VALUE==mDirHandle)
		return FALSE;
	MESSAGE("Stopping DirectoryWatcher_Z for %s ", mDirName.Get());

	if (mThread.IsInited())
	{
		MESSAGE("DirectoryWatcher_Z QueueUserAPC(CallDirWatcherThreadTerminate) for %s on thread %u", mDirName.Get(), Thread_Z::GetThreadId());
		HANDLE threadHandle = mThread.GetThreadObject()->threadHandle;
		// Ask the thread to wake up through APC
		if (0 == ::QueueUserAPC(DirectoryWatcher_Z::CallDirWatcherThreadTerminate, threadHandle, (ULONG_PTR)this))
		{
			OUTPUT_Z("DirectoryWatcher_Z QueueUserAPC(CallDirWatcherThreadTerminate) for %s on thread %u - Failed with %lu", mDirName.Get(), Thread_Z::GetThreadId(), GetLastError());
			// Since we couldn't add the user APC, ask the thread to exit
			bThreadLoop=FALSE;
			bNeedRestart=FALSE;

			// This is the error path, something went wrong, so give the thread 30ms to exit or terminate it
			const S64 microSecondsToJoin = 1000 * 30; // 30ms
			if (mThread.Join(microSecondsToJoin) != THREAD_WAIT_SIGNALED)
			{
				TerminateThread(threadHandle, 1);
			}
		}
		// If the APC was sent, this will join + close the thread
		mThread.Shut();
	}
	mErrorTxt.Flush();
	mModifiedHM.Clear();

	bNeedRestart = FALSE;	//the CallDirWatcherThreadTerminate is a kind of abort that cause the error ERROR_OPERATION_ABORTED on which we set the need restart so need to set to false in case it ask a restart
	mStopNow = FALSE;
	return TRUE;
}

Bool DirectoryWatcher_Z::ResolveRestart()
{
	if(IsStarted() && bNeedRestart)
	{
		MESSAGE_Z("DirectoryWatcher_Z Restarting as dwErrorCode returned (%s)",mDirName.Get());
		OUTPUT_Z("DirectoryWatcher_Z Restarting as dwErrorCode returned (%s)",mDirName.Get());
		
		Sleep_Z(1000);
		Stop();

		if (mRestartedCount >= sMaxRestartCount)
			return FALSE;
		++mRestartedCount;

		if(!Start(mWatchOnlyAddedFiles))
			bNeedRestart=TRUE;
	
		return TRUE;
	}
	return FALSE;
}

void CALLBACK DirectoryWatcher_Z::CallDirWatcherThreadTerminate(__in ULONG_PTR Parameter)
{
	DirectoryWatcher_Z	*pDirWatcher=(DirectoryWatcher_Z *)Parameter;
	OUTPUT_Z("CallDirWatcherThreadTerminate() for %s on thread %u", pDirWatcher->mDirName.Get(), Thread_Z::GetThreadId());
	pDirWatcher->bThreadLoop = FALSE;
	pDirWatcher->bNeedRestart = FALSE;
	CancelIo(pDirWatcher->mDirHandle);
	CloseHandle(pDirWatcher->mDirHandle);
	pDirWatcher->mDirHandle=INVALID_HANDLE_VALUE;
}

Bool DirectoryWatcher_Z::SecurityWrite(const Char *pFolderName,Bool bOutputSecurityDescriptor)
{
	//SecurityDescriptor
	DWORD size = 0;
	SECURITY_INFORMATION	sinfo=
								GROUP_SECURITY_INFORMATION
							|	DACL_SECURITY_INFORMATION
							;

	GetFileSecurity(pFolderName, sinfo, 0, 0, &size);
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_INSUFFICIENT_BUFFER)
		{
			Error("DirectoryWatcher_Z Failed to retrieve security descriptor (error %d)", dwError);
			return FALSE;
		}
	}

	PSECURITY_DESCRIPTOR	sd=Alloc_Z(size);
	if(!GetFileSecurity(pFolderName,
				sinfo,
				sd,
				size,
				&size))
	{
		DWORD dwError = GetLastError();
		Error("DirectoryWatcher_Z Failed to retrieve security descriptor (error %d)",dwError);
		Free_Z(sd);
		return FALSE;
	}
	// Getting DACL from SecurityDescriptor
	PACL acl=nullptr;
	BOOL bDaclPresent, bDaclDefaulted;
	Bool	bSuccess =GetSecurityDescriptorDacl(sd,&bDaclPresent, &acl, &bDaclDefaulted);
	// Check if we successfully retrieved DACL
	if (!bSuccess || !acl)
	{
		Error("DirectoryWatcher_Z Failed to retrieve DACL from security descriptor (error %d)", GetLastError());
		Free_Z(sd);
		return FALSE;
	}
	if(!bDaclPresent)
	{
		Free_Z(sd);
		return TRUE;
	}
	/* Get information about the ACL (ie number of ACEs) */
	ACL_SIZE_INFORMATION acl_size_info;
	if (! GetAclInformation(acl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation))
	{
		DWORD dwError = GetLastError();
		Error("DirectoryWatcher_Z Error getting DACL size information: %d\n", dwError);
		Free_Z(sd);
		Free_Z(acl);
		return FALSE;
	}
	int	maskAllowed=0;
	int	maskDenied=0;
	for (USHORT i = 0; i < acl_size_info.AceCount; i++)
	{
		union
		{
			LPVOID				pVoidAce;
			ACE_HEADER			*pAce;
			ACCESS_ALLOWED_ACE	*pAceAllowed;
			ACCESS_DENIED_ACE	*pAceDenied;
		}U;
		bSuccess = GetAce(acl, i, &U.pVoidAce);
		if (!bSuccess)
		{
			DWORD dwError = GetLastError();
			continue;
		}
		int	mask=0;
		char *type=NULL;
		SID *sid=NULL;
		if(U.pAce->AceType==ACCESS_ALLOWED_ACE_TYPE)
		{
			mask=U.pAceAllowed->Mask;
			maskAllowed|=mask;
			type="Allow";
			sid = (SID *) &U.pAceAllowed->SidStart;

		}
		else if(U.pAce->AceType==ACCESS_DENIED_ACE_TYPE)
		{
			mask=U.pAceAllowed->Mask;
			maskDenied|=mask;
			type="Deny";
			sid = (SID *) &U.pAceDenied->SidStart;
		}
		if (! IsValidSid(sid))
			continue;
		SID_NAME_USE snu;
		char *name = 0;
		char *domain = 0;
		unsigned long namesize = 0;
		unsigned long domainsize = 0;

		LookupAccountSid(0, sid, name, &namesize, domain, &domainsize, &snu);
		DWORD ret = GetLastError();
		if (ret != ERROR_INSUFFICIENT_BUFFER) 
		{
			Error("DirectoryWatcher_Z Warning failed getting size of name and domain: %d", ret);
			continue;
		}
		name = (char *) Alloc_Z(namesize);
		domain = (char *) Alloc_Z(domainsize);
		// Perform the actual lookup */
		if (! LookupAccountSid(0, sid, name, &namesize, domain, &domainsize, &snu)) 
		{
			Error("DirectoryWatcher_Z Warning Error looking up account name: %d", GetLastError());
			Free_Z(name);
			Free_Z(domain);
			continue;
		}
		if (*domain && bOutputSecurityDescriptor)
			OUTPUT_Z("%s\\%s", *domain?domain:"",name);

		Free_Z(name);
		Free_Z(domain);

		if(bOutputSecurityDescriptor)
		{
			if (mask & FILE_READ_DATA) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_READ_DATA", type);
			if (mask & FILE_WRITE_DATA) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_WRITE_DATA", type);
			if (mask & FILE_APPEND_DATA) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_APPEND_DATA", type);
			if (mask & FILE_READ_EA) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_READ_EA", type);
			if (mask & FILE_WRITE_EA) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_WRITE_EA", type);
			if (mask & FILE_EXECUTE) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_EXECUTE", type);
			if (mask & FILE_READ_ATTRIBUTES) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_READ_ATTRIBUTES", type);
			if (mask & FILE_WRITE_ATTRIBUTES) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_WRITE_ATTRIBUTES", type);
			if (mask & FILE_DELETE_CHILD) OUTPUT_Z("DirectoryWatcher_Z \t%s\tFILE_DELETE", type);
			if (mask & DELETE) OUTPUT_Z("DirectoryWatcher_Z \t%s\tDELETE", type);
			if (mask & READ_CONTROL) OUTPUT_Z("DirectoryWatcher_Z \t%s\tREAD_CONTROL", type);
			if (mask & WRITE_DAC) OUTPUT_Z("DirectoryWatcher_Z \t%s\tWRITE_DAC", type);
			if (mask & WRITE_OWNER) OUTPUT_Z("DirectoryWatcher_Z \t%s\tWRITE_OWNER", type);
			if (mask & SYNCHRONIZE) OUTPUT_Z("DirectoryWatcher_Z \t%s\tSYNCHRONIZE", type);
		}
	} 
	maskAllowed&=~maskDenied;
	if(! (maskAllowed& FILE_WRITE_ATTRIBUTES))
	{
		//Error("DirectoryWatcher_Z Warning Error looking up account name: %d", GetLastError());
		return FALSE;
	}
	
	return TRUE;
}
void DirectoryWatcher_Z::Error(const Char *pTxt,...)
{
	String_Z<1024>	String;
	va_list Marker;						// Initialize variable arguments
	va_start( Marker, pTxt );
	String.Vsprintf(pTxt,Marker); 
	va_end( Marker );
	U32	PrevLen=mErrorTxt.GetSize();
	U32 StrLen=(U32)strlen(String);
	if(!StrLen)	return;
	StrLen++;
	mErrorTxt.SetSize(PrevLen ? (PrevLen-1+1+StrLen) : StrLen);
	mErrorTxt.Minimize();
	if(PrevLen)
	{	
		PrevLen--;
		mErrorTxt[PrevLen++]='\n';
	}
	memcpy((void*)(mErrorTxt.GetArrayPtr()+PrevLen),String,StrLen);
}

constexpr void UpdateFileActionFlags(DirectoryWatcher_Z::EFileActionFlags& currentActionFlags, DWORD windowsFileAction)
{
	using mActionFlags = DirectoryWatcher_Z::EFileActionFlags;
	switch (windowsFileAction)
	{
		case FILE_ACTION_ADDED:
			currentActionFlags &= ~(mActionFlags::Removed);
			currentActionFlags |= mActionFlags::Added;
			break;

		case FILE_ACTION_REMOVED:
			currentActionFlags &= ~(mActionFlags::Added);
			currentActionFlags |= mActionFlags::Removed;
			break;
		case FILE_ACTION_MODIFIED:
			currentActionFlags |= mActionFlags::Modified;
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			currentActionFlags |= mActionFlags::Renamed | mActionFlags::Removed;
			currentActionFlags &= ~(mActionFlags::Added);
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			currentActionFlags |= mActionFlags::Renamed | mActionFlags::Added;
			currentActionFlags &= ~(mActionFlags::Removed);
			break;
		default:
			EXCEPTIONC_Z(false, "Unknown file action!");
	}
}

void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	LPOVERLAPPED	pOverlapped=lpOverlapped;
	pOverlapped++;
	DirectoryWatcher_Z	**ppWatchThis=reinterpret_cast<DirectoryWatcher_Z**>(pOverlapped);
	DirectoryWatcher_Z	*pWatch=*ppWatchThis;
	if(dwErrorCode != ERROR_SUCCESS)
	{
		switch(dwErrorCode)
		{
			case	ERROR_NETNAME_DELETED:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) ERROR_NETNAME_DELETED",dwErrorCode);
				break;
			case	ERROR_NETWORK_UNREACHABLE:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) ERROR_NETWORK_UNREACHABLE",dwErrorCode);
				break;
			case	ERROR_DEV_NOT_EXIST:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) ERROR_NETWORK_UNREACHABLE",dwErrorCode);
				break;
			case	ERROR_SEM_TIMEOUT:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) ERROR_SEM_TIMEOUT",dwErrorCode);
				break;
			case	ERROR_OPERATION_ABORTED:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) ERROR_OPERATION_ABORTED", dwErrorCode);
				break;
			default:
				OUTPUT_Z("DirectoryWatcher_Z::Error (%d) unknown",dwErrorCode);
				break;
		}
		pWatch->bThreadLoop=FALSE;
		pWatch->bNeedRestart=TRUE;
	}
	else if (dwNumberOfBytesTransfered && dwErrorCode == ERROR_SUCCESS)
	{
		PFILE_NOTIFY_INFORMATION pNotify;
		size_t offset = 0;
		DWORD	NextEntryOffset=0;
		do
		{
			pNotify = (PFILE_NOTIFY_INFORMATION) (pWatch->pBuffer+offset);
			NextEntryOffset=pNotify->NextEntryOffset;
			offset += NextEntryOffset;

			TCHAR szFile[ARRAY_CHAR_MAX];
			int count = WideCharToMultiByte(CP_UTF8, 0, pNotify->FileName,
				pNotify->FileNameLength / sizeof(WCHAR),
				szFile, ARRAY_CHAR_MAX - 1, NULL, NULL);
			szFile[count] = TEXT('\0');

			String_Z<> strFullFileName;
			strFullFileName.StrCpy(pWatch->mDirName.Get());
			strFullFileName.StrCat("\\");
			int	PathLen=strFullFileName.StrLen();
			int	i=0;
			for(i=0;(PathLen+i<ARRAY_CHAR_MAX-1) && szFile[i] != '\0';i++)
			{
				strFullFileName[PathLen+i] = (CHAR)szFile[i];
			}
			strFullFileName[PathLen+i]=0;
			PathLen+=i;
			for(--PathLen;PathLen>=0;PathLen--)
			{
				if(strFullFileName[PathLen]=='\\'||strFullFileName[PathLen]=='/')
					break;	//a way to remove directory
				else if(strFullFileName[PathLen]=='.')
				{ //require a file name with extension

					String_Z<> relativeFilePath = szFile;
					relativeFilePath.MakeUp();

					{// scope for resource guard
						SharedResourceGuard_Z	guard(pWatch->mMutex);
						pWatch->mLastNotifyCounter++;

						if (pWatch->mWatchOnlyAddedFiles)
						{
							//only watching files added/removed using DirectoryWather_Z::AddFile(..) DirectoryWather_Z::RemoveFile(..)
							const	auto	*pResult = pWatch->mCrcFileNameHT.Search(Name_Z::GetID(relativeFilePath));
							if (pResult)
							{
								pWatch->mLastModifiedAbsoluteTime = GetDAbsoluteTime();

								Bool		bInserted;
								DirectoryWatcher_Z::ModifiedFileData& fileData = pWatch->mModifiedHM.Insert(relativeFilePath, &bInserted);
								if (bInserted) // newly modified file, set initial variables
								{
									fileData.Ref = pResult->Ref;
								}
								fileData.NotifyCounter = pWatch->mLastNotifyCounter;               // Always update counter & date
								UpdateFileActionFlags(fileData.FileActionFlags, pNotify->Action); // always update flags
								OUTPUT_Z("DirectoryWatcher_Z::%d Modified Added File %s", pNotify->Action, strFullFileName.Get());
							}
						}
						else
						{
							//otherwise all files from folder
							pWatch->mLastModifiedAbsoluteTime = GetAbsoluteTime();

							DirectoryWatcher_Z::ModifiedFileData& fileData = pWatch->mModifiedHM.Insert(relativeFilePath);
							fileData.NotifyCounter = pWatch->mLastNotifyCounter;	//Always update counter & date
							UpdateFileActionFlags(fileData.FileActionFlags, pNotify->Action); // always update flags
							OUTPUT_Z("DirectoryWatcher_Z::%d Modified File %s", pNotify->Action, strFullFileName.Get());
						}
					}
					break;
				}
			}
		} while (NextEntryOffset != 0);
	}

	if (!pWatch->mStopNow 
		&& pWatch->mDirHandle != INVALID_HANDLE_VALUE // It is required because multiple APCs might be processed during a single SleepEx
													  // This means we could enter CallDirWatcherThreadTerminate and then WatchCallback, in which case the handle will be invalid and must not be used
		)
	{
	//	OUTPUT_Z("RefreshWatch() for %s on thread %u", pWatch->mDirName.Get(), Thread_Z::GetThreadId());
		if(!pWatch->RefreshWatch())
		{
			pWatch->bThreadLoop=FALSE;
		}
	}
}

Bool DirectoryWatcher_Z::RefreshWatch(Bool _clear)
{
	Bool	bRet=ReadDirectoryChangesW(
		mDirHandle, pBuffer, mBufferSize, TRUE,
		mNotifyFilter, NULL, &mOverlapped, _clear ? 0 : WatchCallback) != 0;
	return bRet;
}


U32 CallDirWatcherThreadUpdate(void *pUserData)
{
	DirectoryWatcher_Z	*pDirWatcher=(DirectoryWatcher_Z *)pUserData;

#if 0
	LPCTSTR arPrivelegeNames[]	=	
		{	
			SE_BACKUP_NAME, //	these two are required forzz the FILE_FLAG_BACKUP_SEMANTICS flag used in the call to 
			SE_RESTORE_NAME,//  CreateFile() to open the directory handle for ReadDirectoryChangesW
			SE_CHANGE_NOTIFY_NAME //just to make sure...it's on by default for all users.
			//<others here as needed>
		};
	for(int i = 0; i < sizeof(arPrivelegeNames) / sizeof(arPrivelegeNames[0]); ++i)
		if( !pDirWatcher->EnablePrivilege(arPrivelegeNames[i], TRUE) )
		{
			pDirWatcher->Error("DirectoryWatcher_Z notifications may not work as intended due to insufficient access rights/process privileges.");
			return FALSE;
		}
#endif

	const auto platformDir = FileSystem_Z::PlatformPath::ConvertFromUTF8(pDirWatcher->mDirName.Get());
	pDirWatcher->mDirHandle = CreateFileW(platformDir.GetCStr(), FILE_LIST_DIRECTORY|FILE_READ_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, 
			OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

	if (pDirWatcher->mStartedHandle != INVALID_HANDLE_VALUE)
	{
		SetEvent(pDirWatcher->mStartedHandle);
	}

	if (pDirWatcher->mDirHandle == INVALID_HANDLE_VALUE)
	{
		pDirWatcher->Error("Unable to open directory %s",pDirWatcher->mDirName.Get());
	}
	else
	{

		pDirWatcher->mOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		pDirWatcher->mOverlapped.Pointer=pDirWatcher;
		pDirWatcher->mNotifyFilter =	FILE_NOTIFY_CHANGE_LAST_WRITE
										| FILE_NOTIFY_CHANGE_SIZE 
										| FILE_NOTIFY_CHANGE_FILE_NAME
										| FILE_NOTIFY_CHANGE_ATTRIBUTES
										;
										
		if (!pDirWatcher->RefreshWatch())
			pDirWatcher->bThreadLoop=FALSE;

		while(pDirWatcher->mDirHandle!=INVALID_HANDLE_VALUE && pDirWatcher->bThreadLoop)
		{
			MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
			if (pDirWatcher->bThreadLoop)
				SleepEx( INFINITE, true );
		}
	}

	pDirWatcher->mStopNow = TRUE;
	
	CancelIo(pDirWatcher->mDirHandle);
	
	pDirWatcher->RefreshWatch(TRUE);
	if (!HasOverlappedIoCompleted(&pDirWatcher->mOverlapped))
		SleepEx(5, TRUE);

	CloseHandle(pDirWatcher->mOverlapped.hEvent);
	return	FALSE;
}
#endif

/*
Bool DirectoryWatcher_Z::TryMakeRelativePath(const Char* fileName, String_Z<>& relativePath)
{
	if (mDirName.IsEmpty()) return FALSE; // the directory to watch has to be set before this function does anything useful
	GetRelativeFromPath(mDirName, fileName, relativePath);
	if (relativePath.StrniCmp(".\\",2)!=0)	
		return FALSE;
	relativePath.StrRemove(".\\"); // '.\\' was added by GetRelativeFromPath
	relativePath.MakeUp();
	return TRUE;
}
*/

Bool DirectoryWatcher_Z::SetWatchedDirectory(const char* pDirectory)
{
#if 0
	EXCEPTION_Z(!IsStarted());
	EXCEPTION_Z(!strchr(pDirectory,'/'));	//Use "\" Please
	String_Z<ARRAY_CHAR_MAX>	DirPath;
	DirPath.StrCpy(pDirectory);
	SanitizePath(DirPath);
	for(int PathLen=DirPath.StrLen();PathLen && DirPath[PathLen-1]=='\\';PathLen--)
		DirPath[PathLen-1]=0;

#ifdef _DIRECTORYWATCHER_Z
	if(!File_Z::DoFolderExists(DirPath.Get()))
	{
		Error("DirectoryWatcher_Z Failed folder doesnt exist %s",DirPath.Get());
		return FALSE;
	}
	if(!SecurityWrite(DirPath.Get(),FALSE))
	{
		Error("DirectoryWatcher_Z Failed folder security is not writable %s",DirPath.Get());
		return FALSE;
	}
#endif

	mModifiedHM.Clear();
	mDirName=DirPath;
#endif

	return TRUE;
}

#if 0

Bool DirectoryWatcher_Z::AddFile(const Char *pFileName,const Function_Z<S64(void)>& createRefOnAddSucceed)
{
	String_Z<> relativePath;
	if (!TryMakeRelativePath(pFileName, relativePath))
	{
		OUTPUT_Z("DirectoryWatcher_Z: Trying to add a file that is not relative to the watched directory. Path: %s Dir: %s", pFileName, mDirName.Get());
		return FALSE;
	}
	SharedResourceGuard_Z	guard(mMutex);
	NameID_ZHashT_Z<S64>	elt(Name_Z::GetID(relativePath.Get()));
	if (mCrcFileNameHT.Search(elt))
	{
		OUTPUT_Z("DirectoryWatcher_Z: Trying to add a file already added Path: %s Dir: %s", pFileName, mDirName.Get());
		return FALSE;
	}
	elt.Ref = createRefOnAddSucceed();
	mCrcFileNameHT.Insert(elt);
	return TRUE;
}
Bool DirectoryWatcher_Z::AddFile(const Char *pFileName)
{
	Function_Z<S64(void)> createRefOnAddSucceed =[&](void)->S64
	{
		return -1;
	};
	return AddFile(pFileName, createRefOnAddSucceed);
}

Bool DirectoryWatcher_Z::RemoveFile(const Char *pFileName)
{
	String_Z<> relativePath;
	if (!TryMakeRelativePath(pFileName, relativePath))
	{
		OUTPUT_Z("DirectoryWatcher_Z: Trying to remove a file that is not relative to the watched directory. Path: %s Dir: %s", pFileName, mDirName.Get());
		return FALSE;
	}

	RemoveModifiedFile(pFileName);

	SharedResourceGuard_Z	guard(mMutex);
	NameID_ZHashT_Z<S64>	elt(Name_Z::GetID(relativePath.Get()));
	return mCrcFileNameHT.Suppress(elt);
}

S64	DirectoryWatcher_Z::GetFileRef(const Char *pFileName)
{
	String_Z<> relativePath;
	if (!TryMakeRelativePath(pFileName, relativePath))
	{
		OUTPUT_Z("DirectoryWatcher_Z: Trying to get a file ref for a file that is not relative to the watched directory. Path: %s Dir: %s", pFileName, mDirName.Get());
		return -1;
	}

	NameID_ZHashT_Z<S64>	elt(Name_Z::GetID(relativePath.Get()));
	SharedResourceGuard_Z	guard(mMutex);
	const	auto	*pResult=mCrcFileNameHT.Search(elt);
	return	pResult?pResult->Ref:-1;
}

Bool DirectoryWatcher_Z::GetModifiedFiles(ModifiedFileDataHM &HT,const Bool bFlushModifiedFiles,const Double WaitSilenceSecond)
{
	if(WaitSilenceSecond && (GetDAbsoluteTime()-mLastModifiedAbsoluteTime)<WaitSilenceSecond)
		return FALSE;
	SharedResourceGuard_Z	guard(mMutex);
	HT=mModifiedHM;
	if(bFlushModifiedFiles)	
		mModifiedHM.Clear();
	return TRUE;
}
Bool DirectoryWatcher_Z::RemoveModifiedFile(const Char *pFileName)
{
	String_Z<> relativePath;
	if (!TryMakeRelativePath(pFileName, relativePath))
	{
		OUTPUT_Z("DirectoryWatcher_Z: Trying to remove a modified file that is not relative to the watched directory. Path: %s Dir: %s", pFileName, mDirName.Get());
		return FALSE;
	}
	SharedResourceGuard_Z	guard(mMutex);
	const auto it = mModifiedHM.Find(relativePath);
	if (it == mModifiedHM.end())
		return FALSE;
	else mModifiedHM.Remove(it);
	return TRUE;
}

#endif
