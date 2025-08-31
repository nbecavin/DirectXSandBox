#if defined(_PC)

#include <DX11Renderer.h>
#include <WinMain.h>
#include <D3D12HAL.h>
#include <D3D12HALBuffers.h>
#include <Material.h>
#include <Bitmap.h>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#define MessageBox(i,j,k,l)		OutputDebugStringA(j)
#endif

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
		mac[macnb].Name = "_PCDX12";
		mac[macnb].Definition = "1";
		macnb++;
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

};

#endif //_PC
