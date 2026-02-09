#if defined(_PC)

#include <RenderHAL.h>
#include <Material.h>
#include <Bitmap.h>
#include "SandBox.h"
#include <WinMain.h>
#include <..\..\Shaders\ShaderRegs.h>

namespace sys
{
	/// Load and return the newest pix version, newest than minVersion.
	/// The version has to be formatted as "xxxx.xx" with x being a number as it will be compared with string comp
	HMODULE LoadWinPixWithMinVersion(const wchar_t* minVersion)
	{
		HMODULE libHandle{};

		if (GetModuleHandleExW(0, L"WinPixGpuCapturer.dll", &libHandle))
		{
			return libHandle;
		}

		LPWSTR programFilesPath = nullptr;
		if (FAILED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath)))
		{
			CoTaskMemFree(programFilesPath);
			return nullptr;
		}

		wchar_t pixSearchPath[MAX_PATH];

		if (FAILED(StringCchCopyW(pixSearchPath, MAX_PATH, programFilesPath)))
		{
			CoTaskMemFree(programFilesPath);
			return nullptr;
		}
		CoTaskMemFree(programFilesPath);

		if (FAILED(StringCchCatW(pixSearchPath, MAX_PATH, L"\\Microsoft PIX\\*")))
			return nullptr;

		WIN32_FIND_DATAW findData;
		bool foundPixInstallation = false;
		wchar_t newestVersionFound[MAX_PATH];
		wchar_t output[MAX_PATH];
		wchar_t possibleOutput[MAX_PATH];

		HANDLE hFind = FindFirstFileW(pixSearchPath, &findData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
					(findData.cFileName[0] != '.'))
				{
					if ((!foundPixInstallation || wcscmp(newestVersionFound, findData.cFileName) <= 0) && wcscmp(findData.cFileName, minVersion) >= 0)
					{
						// length - 1 to get rid of the wildcard character in the search path
						if (FAILED(StringCchCopyNW(possibleOutput, MAX_PATH, pixSearchPath, wcslen(pixSearchPath) - 1)))
							return nullptr;

						if (FAILED(StringCchCatW(possibleOutput, MAX_PATH, findData.cFileName)))
							return nullptr;

						if (FAILED(StringCchCatW(possibleOutput, MAX_PATH, L"\\WinPixGpuCapturer.dll")))
							return nullptr;

						DWORD result = GetFileAttributesW(possibleOutput);

						if (result != INVALID_FILE_ATTRIBUTES && !(result & FILE_ATTRIBUTE_DIRECTORY))
						{
							foundPixInstallation = true;
							if (FAILED(StringCchCopyW(newestVersionFound, _countof(newestVersionFound), findData.cFileName)))
								return nullptr;
							if (FAILED(StringCchCopyW(output, _countof(possibleOutput), possibleOutput)))
								return nullptr;
						}
					}
				}
			} while (FindNextFileW(hFind, &findData) != 0);
		}

		FindClose(hFind);

		if (!foundPixInstallation)
		{
			SetLastError(ERROR_FILE_NOT_FOUND);
			return nullptr;
		}

		return LoadLibraryW(output);

	}

	void RenderHAL::Init(int sizeX, int sizeY, sys::Renderer* owner)
	{
		SizeX = sizeX;
		SizeY = sizeY;
		m_FrameIndex = 0;

		//m_PixModule = LoadWinPixWithMinVersion(L"2208.10");
		if (m_PixModule) {
			MESSAGE("PIXGpuCapturer Loaded");
			PIXSetHUDOptions(PIX_HUD_SHOW_ON_NO_WINDOWS);
		}
		else
			MESSAGE("Impossible to load PIXGpuCapturer. Either you don't have PIX installed OR you don't have a version greater than 2208.10");

		//// Fill in a buffer description.
		//D3D12_BUFFER_DESC cbDesc;
		//cbDesc.ByteWidth = VS_CONSTANT_BUFFER_SIZE;
		//cbDesc.Usage = D3D12_USAGE_DYNAMIC;
		//cbDesc.BindFlags = D3D12_BIND_CONSTANT_BUFFER;
		//cbDesc.CPUAccessFlags = D3D12_CPU_ACCESS_WRITE;
		//cbDesc.MiscFlags = 0;
		//cbDesc.StructureByteStride = 0;
		//hr = GetDevice()->CreateBuffer( &cbDesc, NULL, &m_VSConstant );

		m_DefaultRS.desc.AntialiasedLineEnable = FALSE;
		m_DefaultRS.desc.CullMode = D3D12_CULL_MODE_NONE;
		m_DefaultRS.desc.DepthBias = 0.f;
		m_DefaultRS.desc.DepthBiasClamp = 0.f;
		m_DefaultRS.desc.DepthClipEnable = TRUE;
		m_DefaultRS.desc.FillMode = D3D12_FILL_MODE_SOLID;
		m_DefaultRS.desc.FrontCounterClockwise = FALSE;
		m_DefaultRS.desc.MultisampleEnable = FALSE;
		m_DefaultRS.desc.SlopeScaledDepthBias = 0.f;

		m_DefaultDS.desc.DepthEnable = TRUE;
		m_DefaultDS.desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		m_DefaultDS.desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		m_DefaultDS.desc.StencilEnable = FALSE;
		m_DefaultDS.desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		m_DefaultDS.desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		m_DefaultDS.desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_DefaultDS.desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		m_DSS_NoZWrite.desc.DepthEnable = FALSE;
		m_DSS_NoZWrite.desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		m_DSS_NoZWrite.desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		m_DefaultSS.desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_DefaultSS.desc.AddressU = m_DefaultSS.desc.AddressV = m_DefaultSS.desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_DefaultSS.desc.MaxAnisotropy = 1;
		m_DefaultSS.desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_DefaultSS.desc.MaxLOD = D3D12_FLOAT32_MAX;

		m_NoBilinearSS.desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		m_NoBilinearSS.desc.AddressU = m_NoBilinearSS.desc.AddressV = m_NoBilinearSS.desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		m_NoBilinearSS.desc.MaxAnisotropy = 1;
		m_NoBilinearSS.desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		m_NoBilinearSS.desc.MaxLOD = D3D12_FLOAT32_MAX;

		InitShaders();
		InitStaticDatas();
	}

	struct SCREEN_VERTEX
	{
		Vec4f	pos;
		Vec2f	tex;
	};

	bool RenderHAL::InitStaticDatas()
	{
		// Fullscreen quad for post processing
		{
			// Create a screen quad for render to texture operations
			SCREEN_VERTEX svQuad[4];
			svQuad[0].pos.Set(-1.0f, 1.0f, 0.5f, 1.0f);
			svQuad[0].tex.Set(0.0f, 0.0f);
			svQuad[1].pos.Set(1.0f, 1.0f, 0.5f, 1.0f);
			svQuad[1].tex.Set(1.0f, 0.0f);
			svQuad[2].pos.Set(-1.0f, -1.0f, 0.5f, 1.0f);
			svQuad[2].tex.Set(0.0f, 1.0f);
			svQuad[3].pos.Set(1.0f, -1.0f, 0.5f, 1.0f);
			svQuad[3].tex.Set(1.0f, 1.0f);

			// Generate runtime buffers
			m_FullscreenQuadVB = CreateVertexBuffer(sizeof(svQuad), sizeof(SCREEN_VERTEX), 0, NULL);

			SCREEN_VERTEX* pVtxData;
			if (m_FullscreenQuadVB && m_FullscreenQuadVB->Lock(0, 0, (void**)&pVtxData) == true)
			{
				memcpy(pVtxData, svQuad, sizeof(svQuad));
				m_FullscreenQuadVB->Unlock();
			}

			VertexElement DeclDesc[] =
			{
				{ 0, 0, DECL_FMT_FLOAT4, DECL_POSITION },
				{ 0, 16, DECL_FMT_FLOAT2, DECL_TEXCOORD0 },
				DECL_END()
			};

			m_ScreenVertexDecl = CreateVertexDecl(DeclDesc);
		}

		return TRUE;
	}

	void RenderHAL::Shut()
	{
		ReleaseAllResources();
		if (m_PixModule)
			FreeLibrary(m_PixModule);
	}

};

#endif //_PC
