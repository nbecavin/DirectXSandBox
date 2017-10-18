
void AssertFonc(char * condition)
{
#if defined(_PC)
#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#else
	OutputDebugString(condition);
	DebugBreak();
#endif
#endif
}

void MessageFonc(char * fmt,...)
{
	if(!fmt)
		return;
	char msg[2048];
	va_list args;
	va_start(args,fmt);
	vsprintf(msg,fmt,args);
	strcat(msg,"\n");
#if defined(_PC)
	WCHAR wmsg[2048];
	MultiByteToWideChar(CP_ACP,0,msg,-1,wmsg,2048);
	OutputDebugStringW(wmsg);
#else
	__android_log_print(ANDROID_LOG_DEBUG, "SandBoxDroidNativeLib", msg);
#endif
}
