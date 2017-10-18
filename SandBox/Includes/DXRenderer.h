#ifndef __DXRENDERER_HH__
#define __DXRENDERER_HH__

#define GET_RDR_INSTANCE()	((sys::DXRenderer*)gData.Rdr)

#if defined(_PCDX11) || defined(_PCDX12)
	#include <DX11Renderer.h>
#endif

#endif //__DXRENDERER_HH__
