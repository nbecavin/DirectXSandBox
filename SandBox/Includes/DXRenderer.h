#ifndef __DXRENDERER_HH__
#define __DXRENDERER_HH__

#define GET_RDR_INSTANCE()	((sys::DXRenderer*)gData.Rdr)

#if defined(_PCDX9)
	#include <DX9Renderer.h>
#elif defined(_PCDX11)
	#include <DX11Renderer.h>
#elif defined(_PCDX12)
	#include <DX12Renderer.h>
#endif

#endif //__DXRENDERER_HH__
