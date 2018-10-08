#if defined(_PC)

#include <DX11Renderer.h>
#include <WinMain.h>
#if defined(_PCDX12)
#include <D3D12HAL.h>
#include <D3D12HALBuffers.h>
#elif defined(_PCDX11)
#include <D3D11HAL.h>
#include <D3D11HALBuffers.h>
#endif
#include <Material.h>
#include <Bitmap.h>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#define MessageBox(i,j,k,l)		OutputDebugStringA(j)
#endif

namespace sys {

	VertexBuffer*	DXRenderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void* _Datas)
	{
		DXVertexBuffer ** buffer = m_VertexBufferDA.Add();
		buffer[0] = new DXVertexBuffer();
		MESSAGE("Begin create VB");
		buffer[0]->Create(_Size,_Usage,0,_Datas);
		MESSAGE("End create VB");
		return (buffer[0]->IsInited()) ? buffer[0] : NULL;
	}

	IndexBuffer*	DXRenderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void* _Datas)
	{
		DXIndexBuffer ** buffer = m_IndexBufferDA.Add();
		buffer[0] = new DXIndexBuffer();
		MESSAGE("Begin create IB");
		buffer[0]->Create(_Size,_Usage,_Fmt,_Datas);
		MESSAGE("End create IB");
		return (buffer[0]->IsInited()) ? buffer[0] : NULL;
	}

	void DXRenderer::RegisterShaderFromSourceFile(U32 _ShaderUID,const char* src,const char* epoint)
	{
		HRESULT hr;

		char profile[32]="";
		char path[1024];

		sprintf(path,"..\\Shaders\\%s",src);
	
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));

		if(Type==SHADER_TYPE_VERTEX)
		{
			strcpy(profile,"vs_5_0");
		}
		if(Type==SHADER_TYPE_PIXEL)
		{
			strcpy(profile,"ps_5_0");
		}
		if(Type==SHADER_TYPE_COMPUTE)
		{
			strcpy(profile,"cs_5_0");
		}

		DWORD CpFlag = 0;//*D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		//CpFlag |= D3DCOMPILE_DEBUG;
	#endif
		//CpFlag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
		CpFlag |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
		CpFlag |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

		D3D_SHADER_MACRO mac[128]; U32 macnb=0;
		mac[macnb].Name = "_PCDX11";
		mac[macnb].Definition = "1";
		macnb++;
#ifdef _PCDX12
		mac[macnb].Name = "_PCDX12";
		mac[macnb].Definition = "1";
		macnb++;
#endif
		mac[macnb].Name = 0;
		mac[macnb].Definition = 0;
		macnb++;

		ID3DBlob * pError = NULL;
		ID3DBlob * pCode = NULL;

		WCHAR wpath[2049];
		MultiByteToWideChar(CP_ACP,0,path,-1,wpath,2048);
		hr = D3DCompileFromFile(wpath,mac,D3D_COMPILE_STANDARD_FILE_INCLUDE,epoint,profile,CpFlag,0,&pCode,&pError);
		if(hr!=S_OK)
		{
			if(hr==D3D11_ERROR_FILE_NOT_FOUND)
				MessageBox(NULL,"File not found","Shader Error",MB_OK);
			else
			{
				char str[256];
				sprintf(str,"HR=%d\n",hr);
				MessageBox(NULL,str,"Shader Error",MB_OK);
			}
			if(pError)
				MessageBox(NULL,(char*)pError->GetBufferPointer(),"Shader Error",MB_OK);
		}
		if( pError )
			pError->Release();

		if(pCode)
		{
			GetHAL().CreateShaderResource(pCode, Type, SID);
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

	ID3D11Buffer * CreateConstantBuffer(void* _DataPtr,U32 _DataSize)
	{
#if 0 //def _PCDX11
		ID3D11Buffer * pCstBuffer;

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = _DataSize;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = _DataPtr;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;
		((DXRenderer*)gData.Rdr)->GetDevice()->CreateBuffer(&desc,&initialData,&pCstBuffer);
		return pCstBuffer;
#else
		return NULL;
#endif
	}

};

//
// Constant buffers

void* CameraConstant::CreateHwRes()
{
	CameraConstant t;
	return (void*)sys::CreateConstantBuffer(&t,sizeof(t));
}

void* PostProcessConstant::CreateHwRes()
{
	PostProcessConstant t;
	return (void*)sys::CreateConstantBuffer(&t,sizeof(t));
}

#endif //_PC
