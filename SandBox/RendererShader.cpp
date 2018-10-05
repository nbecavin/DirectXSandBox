#include <Renderer.h>

namespace sys {

	void Renderer::InitShaders()
	{
		RegisterShaderFromSourceFile( SHADER_PS_PIXEL_LIT, "pixel.psh", "ph_base" );
		RegisterShaderFromSourceFile( SHADER_PS_PIXEL_GBUFFER, "pixel.psh", "ph_gbuffer" );
		RegisterShaderFromSourceFile( SHADER_VS_BASE_MESH, "vertex_factory.vsh", "vh_base" );

		// Register 2D shaders
		RegisterShaderFromSourceFile( SHADER_VS_BASE_2D,				"base.vsh", "vh2d_base" );
		RegisterShaderFromSourceFile( SHADER_VS_BASE_SCREENVERTEX,		"base.vsh", "vh_screenvertex");
		RegisterShaderFromSourceFile( SHADER_PS_POSTPROC_COLORGRADING,	"post_process.psh", "ph_colorgrading");
		RegisterShaderFromSourceFile( SHADER_PS_POSTPROC_FXAA,			"post_process.psh", "ph_fxaa");
		RegisterShaderFromSourceFile( SHADER_PS_POSTPROC_PASSTHROUGH,	"post_process.psh", "ph_passthrough");
		RegisterShaderFromSourceFile( SHADER_PS_SSAO,					"ssao.psh", "ph_ambientocclusion");
		RegisterShaderFromSourceFile( SHADER_PS_LINEAR_DEPTH,			"ssao.psh", "ph_ambientocclusion_linearzbuf");

		RegisterShaderFromSourceFile(SHADER_VS_IMGUI, "ImGui.shader", "VsMain");
		RegisterShaderFromSourceFile(SHADER_PS_IMGUI, "ImGui.shader", "PsMain");

		// Register compute shaders
		RegisterShaderFromSourceFile( SHADER_CS_TEST, "compute_test.csh","ch_main");
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
 