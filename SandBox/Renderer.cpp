#include <Renderer.h>
#include <ShaderConstants.h>

namespace sys {

	int Renderer::Init()
	{
		m_GlobalParams = {};
		m_GlobalParams.Visualize = EVIZ_LIT;
		m_FrameIndex = 0;
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

	VertexDeclaration*	Renderer::CreateVertexDecl(VertexElement * _Decl)
	{
		return NULL;
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
