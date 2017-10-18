#ifndef __SANDBOXDEBUG_HH__
#define __SANDBOXDEBUG_HH__

#define ASSERT(_cond_,...)	if((_cond_)==FALSE)\
								AssertFonc(#_cond_);

void AssertFonc(char * condition);
void MessageFonc(char * fmt,...);

#define MESSAGE(...)	MessageFonc(__VA_ARGS__)

#endif //__SANDBOXDEBUG_HH__
