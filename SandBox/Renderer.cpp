#include <Renderer.h>
#include <ShaderConstants.h>

namespace sys {

	int Renderer::Init()
	{
		return 0;
	}

	VertexBuffer*	Renderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas)
	{
		return NULL;
	}

	IndexBuffer*	Renderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas)
	{
		return NULL;
	}

	VertexDeclaration*	Renderer::CreateVertexDecl(VertexElement * _Decl,U32 _ShaderUID)
	{
		return NULL;
	}

	//
	void Renderer::PushWorldMatrix(Mat4x4* _m)
	{
		GlobalConstant* c = GetGlobalConstantData();
		Mat4x4 * _mat = (Mat4x4*)(c->v+8);
		_mat[0] = *_m;
	}

	void Renderer::UpdateGlobalConstant()
	{
		GlobalConstant * c;
		m_GlobalConstant->Lock(0, 0, (void**)&c);
		c[0] = m_GlobalConstantData;
		m_GlobalConstant->Unlock();
	}

	void Renderer::SetGlobalConstant(EShaderType Type)
	{
		SetConstantBuffer(0, Type, m_GlobalConstant);
	}

	void Renderer::BeginFrame(float dTime)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = dTime;              // set the time elapsed since the previous frame (in seconds)
		io.DisplaySize.x = SizeX;             // set the current display width
		io.DisplaySize.y = SizeY;             // set the current display height here
		ImGui::NewFrame();
	}

};
