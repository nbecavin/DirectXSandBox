// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#define D3D12_AGILITYSDK_VERSION	717

#if defined(_PC)
//#	include "targetver.h"
#	define WIN32_LEAN_AND_MEAN             // Exclure les en-t�tes Windows rarement utilis�s
#	define _CRT_SECURE_NO_WARNINGS
#	define NOMINMAX
#	include <windows.h>
#	include <algorithm>
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
#	include <dxgi1_6.h>
//#	include <d3d11_4.h>
#	include <d3d12.h>
#	include <d3dx12.h>
#	include <DirectXMath.h>
#	include <dxcapi.h>
#	include <../../Tools/dxc/inc/dxcapi.h>
#	include <d3d12shader.h>
	using namespace DirectX;
#	define __CDECL	__cdecl
#	include <pix3.h>
#endif

#include <filesystem>

#include <SandBoxTypes.h>
#include <SandBoxDebug.h>
#include <SandBoxMemory.h>
#include <SandBoxFile.h>
#include <VectorLib.h>
#include "SandBox.h"
