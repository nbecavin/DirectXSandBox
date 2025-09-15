#include <Renderer.h>

ShaderKernel* ShaderMap::BasePassPS = nullptr;
ShaderKernel* ShaderMap::GBufferPassPS = nullptr;
ShaderKernel* ShaderMap::BaseMeshVS = nullptr;
ShaderKernel* ShaderMap::SimpleVertexFactoryVS = nullptr;
ShaderKernel* ShaderMap::ScreenVertexVS = nullptr;
ShaderKernel* ShaderMap::ColorGradingPS = nullptr;
ShaderKernel* ShaderMap::PassThroughPS = nullptr;
ShaderKernel* ShaderMap::ImGuiVS = nullptr;
ShaderKernel* ShaderMap::ImGuiPS = nullptr;

namespace sys {

	ShaderKernel* textRTXDI;

	void Renderer::InitShaders()
	{
		// Base mesh shaders
		ShaderMap::BasePassPS = CreateKernel("shaders/base_pass.ps.hlsl", "ForwardMain", SHADER_TYPE_PIXEL);
		ShaderMap::GBufferPassPS = CreateKernel("shaders/base_pass.ps.hlsl", "GbufferMain", SHADER_TYPE_PIXEL);
		ShaderMap::BaseMeshVS = CreateKernel("shaders/vertex_factory.vs.hlsl", "MeshFactory", SHADER_TYPE_VERTEX);

		// Register 2D shaders
		ShaderMap::SimpleVertexFactoryVS = CreateKernel("shaders/simple_vertex_factory.vs.hlsl", "vh2d_base", SHADER_TYPE_VERTEX);
		ShaderMap::ScreenVertexVS = CreateKernel("shaders/simple_vertex_factory.vs.hlsl", "vh_screenvertex", SHADER_TYPE_VERTEX);
		
		ShaderMap::ColorGradingPS = CreateKernel("shaders/post_process.ps.hlsl", "ph_colorgrading", SHADER_TYPE_PIXEL);
		ShaderMap::PassThroughPS = CreateKernel("shaders/post_process.ps.hlsl", "ph_passthrough", SHADER_TYPE_PIXEL);

		// ImGUI
		ShaderMap::ImGuiVS = CreateKernel("shaders/ImGui.hlsl", "VsMain", SHADER_TYPE_VERTEX);
		ShaderMap::ImGuiPS = CreateKernel("shaders/ImGui.hlsl", "PsMain", SHADER_TYPE_PIXEL);

		//textRTXDI = CreateKernel("Shaders/RTXDI-Integration/LightingPasses/Presampling/PresampleReGIR.hlsl", "main", SHADER_TYPE_COMPUTE);
		// Register compute shaders
		//RegisterShaderFromSourceFile( SHADER_CS_TEST, "compute_test.csh","ch_main");
	}

};
