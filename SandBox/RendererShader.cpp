#include <Renderer.h>

namespace sys {

	void Renderer::InitShaders()
	{
		RegisterShaderFromSourceFile( SHADER_PS_PIXEL_LIT,		"base_pass.ps.hlsl", "ForwardMain" );
		RegisterShaderFromSourceFile( SHADER_PS_PIXEL_GBUFFER,	"base_pass.ps.hlsl", "GbufferMain" );
		RegisterShaderFromSourceFile( SHADER_VS_BASE_MESH,		"vertex_factory.vs.hlsl", "MeshFactory" );

		// Register 2D shaders
		RegisterShaderFromSourceFile( SHADER_VS_BASE_2D,				"simple_vertex_factory.vs.hlsl", "vh2d_base" );
		RegisterShaderFromSourceFile( SHADER_VS_BASE_SCREENVERTEX,		"simple_vertex_factory.vs.hlsl", "vh_screenvertex");
		RegisterShaderFromSourceFile( SHADER_PS_POSTPROC_COLORGRADING,	"post_process.ps.hlsl", "ph_colorgrading");
		RegisterShaderFromSourceFile( SHADER_PS_POSTPROC_PASSTHROUGH,	"post_process.ps.hlsl", "ph_passthrough");

		RegisterShaderFromSourceFile(SHADER_VS_IMGUI, "ImGui.hlsl", "VsMain");
		RegisterShaderFromSourceFile(SHADER_PS_IMGUI, "ImGui.hlsl", "PsMain");

		// Register compute shaders
		//RegisterShaderFromSourceFile( SHADER_CS_TEST, "compute_test.csh","ch_main");
	}

	void Renderer::PushShader( U32 _ShaderUID )
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		if(Type==SHADER_TYPE_VERTEX)
			int cul = 0;
		if(Type==SHADER_TYPE_PIXEL)
			int cul = 0;
	}

};
