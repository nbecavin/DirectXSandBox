// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#if defined(_PC)
//#	include "targetver.h"
#	define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
#	define _CRT_SECURE_NO_WARNINGS
#	include <windows.h>
#	include <stdlib.h>
#	include <malloc.h>
#	include <memory.h>
#	include <stdio.h>
#	undef _PC
#		include <tchar.h>
#	undef _PC
#	define _PC
#	include <math.h>
#	include <combaseapi.h>
#	include <wrl/client.h>
#	include <ppl.h>
#	include <ppltasks.h>
#	include <dxgitype.h>
#	include <d3d11_3.h>
#   if defined(_PCDX12)
#		include <dxgi1_4.h>
#		include <d3d12.h>
//#		include <d3dx12.h>
#	endif
#	include <DirectXMath.h>
#	include <d3dcompiler.h>
	using namespace DirectX;
#	define __CDECL	__cdecl
#elif defined(_ANDROID)
#	include <stdlib.h>
#	include <malloc.h>
#	include <memory.h>
#	include <stdio.h>
#	include <jni.h>
#	include <string.h>
#	include <GLES2/gl2.h>
#	include <unistd.h>
#	include <math.h>
#	include <time.h>
#	include <nv_debug.h>
#	define __CDECL
#endif

#ifdef __cplusplus
#include <SandBoxTypes.h>
#include <SandBoxDebug.h>
#include <SandBoxMemory.h>
#include <SandBoxFile.h>
#include <VectorLib.h>
#include "SandBox.h"
#endif
