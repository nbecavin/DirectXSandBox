#if defined(_PC)

#include <DX11Renderer.h>
#include <WinMain.h>
#include <D3D12HAL.h>
#include <D3D12HALBuffers.h>
#include <Material.h>
#include <Bitmap.h>
#include <unordered_set>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#define MessageBox(i,j,k,l)		OutputDebugStringA(j)
#endif

#pragma comment(lib,"../Tools/dxc/lib/x64/dxcompiler.lib")

namespace sys {

	VertexBuffer* DXRenderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void* _Datas)
	{
		VertexBuffer* ret = GetHAL().CreateVertexBuffer(_Size, _Usage, _Datas);
		if (ret)
		{
			m_VertexBufferDA.Add(ret);
		}
		return ret;
	}

	IndexBuffer* DXRenderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void* _Datas)
	{
		IndexBuffer* ret = GetHAL().CreateIndexBuffer(_Size, _Usage, _Fmt, _Datas);
		if (ret)
		{
			m_IndexBufferDA.Add(ret);
		}
		return ret;
	}

	const char* GetShaderProfile(U32 type)
	{
		switch (type)
		{
		case SHADER_TYPE_VERTEX:	return "vs_6_8";
		case SHADER_TYPE_PIXEL:		return "ps_6_8";
		case SHADER_TYPE_COMPUTE:	return "cs_6_8";
		case SHADER_TYPE_MESH:		return "ms_6_8";
		case SHADER_TYPE_RAYGEN:	return "lib_6_8";
		};
	}

	class CustomIncludeHandler : public IDxcIncludeHandler
	{
		ComPtr<IDxcUtils> m_pUtils;
	public:
		CustomIncludeHandler(const ComPtr<IDxcUtils>& _pUtils) : m_pUtils(_pUtils) {};
		HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
		{
			//sprintf(path, "..\\Shaders\\%s", src);
			ComPtr<IDxcBlobEncoding> pEncoding;
			std::wstring path = std::wstring(L"..//Shaders").append(pFilename);
			if (IncludedFiles.find(path) != IncludedFiles.end())
			{
				// Return empty string blob if this file has been included before
				static const char nullStr[] = " ";
				m_pUtils->CreateBlobFromPinned(nullStr, ARRAYSIZE(nullStr), DXC_CP_ACP, pEncoding.GetAddressOf());
				*ppIncludeSource = pEncoding.Detach();
				return S_OK;
			}

			HRESULT hr = m_pUtils->LoadFile(pFilename, nullptr, pEncoding.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				IncludedFiles.insert(path);
				*ppIncludeSource = pEncoding.Detach();
			}
			return hr;
		}

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override { return E_NOINTERFACE; }
		ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
		ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

		std::unordered_set<std::wstring> IncludedFiles;
	};

	void DXRenderer::RegisterShaderFromSourceFile(U32 _ShaderUID,const char* src,const char* epoint)
	{
		HRESULT hr;

		char profile[32]="";
		char path[1024];
		WCHAR wstr[256][256];
		int wstr_id = 0;

		auto ToWSTR = [&wstr,&wstr_id](const char* str) -> LPWSTR
			{
				MultiByteToWideChar(CP_ACP, 0, str, -1, wstr[wstr_id], 1024);
				return wstr[wstr_id++];
			};

		sprintf(path,"..\\Shaders\\%s",src);
	
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));

		strcpy(profile, GetShaderProfile(Type));

		std::vector<LPWSTR> arguments;

		// -E for the entry point (eg. 'main')
		arguments.push_back(L"-E");
		arguments.push_back(ToWSTR(epoint));
		arguments.push_back(L"-T");
		arguments.push_back(ToWSTR(GetShaderProfile(Type)));
		arguments.push_back(L"-I ../Shaders");

		WCHAR wpath[2049];
		MultiByteToWideChar(CP_ACP,0,path,-1,wpath,2048);

		ComPtr<IDxcCompiler3> pCompiler;
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(pCompiler.GetAddressOf()));

		ComPtr<IDxcUtils> pUtils;
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(pUtils.GetAddressOf()));

		UINT32 code_page = CP_UTF8;

		ComPtr<IDxcBlobEncoding> pSource;
		pUtils->LoadFile(wpath, &code_page, pSource.GetAddressOf());

		DxcBuffer sourceBuffer;
		sourceBuffer.Ptr = pSource->GetBufferPointer();
		sourceBuffer.Size = pSource->GetBufferSize();
		sourceBuffer.Encoding = 0;

		CustomIncludeHandler includeHandler(pUtils);

		ComPtr<IDxcResult> pResults;
		pCompiler->Compile(&sourceBuffer, (LPCWSTR*)arguments.data(), arguments.size(), &includeHandler, IID_PPV_ARGS(&pResults));

		ComPtr<IDxcBlobUtf8> pErrors;
		pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), nullptr);
		pResults->GetStatus(&hr);

		ComPtr<ID3DBlob> pCode;

		if (hr != S_OK && pErrors && pErrors->GetStringLength() > 0)
		{
			OutputDebugString((char*)pErrors->GetBufferPointer());
			MessageBox(NULL, (char*)pErrors->GetBufferPointer(), "Shader Error", MB_OK);
		}
		else
		{
			pResults->GetResult((IDxcBlob**)pCode.GetAddressOf());
			GetHAL().CreateShaderResource(pCode.Get(), Type, SID);
			pCode.Detach(); //detach from resource (ouais ca leak)
		}
	}

	ID3DBlob * DXRenderer::GetShaderBlob(U32 _ShaderUID)
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));
		return GetHAL().GetShaderBlob(Type, SID);
	}

	void DXRenderer::CreateTexture(Bitmap * _Bm)
	{
		GetHAL().CreateTexture(_Bm);
	}

};

#endif //_PC
