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

ShaderKernel* ShaderMap::ComputeHistogramCS = nullptr;
ShaderKernel* ShaderMap::TonemappingPS = nullptr;

ShaderKernel* ShaderMap::RaytracingDebugRGS = nullptr;
ShaderKernel* ShaderMap::RaytracingDebugInlineRGS = nullptr;
ShaderKernel* ShaderMap::PathTracerRGS = nullptr;

namespace sys {

	ShaderKernel* textRTXDI;

	void Renderer::InitShaders()
	{
		// Base mesh shaders
		ShaderMap::BasePassPS = GetHAL()->CreateKernel("shaders/base_pass.ps.hlsl", "ForwardMain", SHADER_TYPE_PIXEL);
		ShaderMap::GBufferPassPS = GetHAL()->CreateKernel("shaders/base_pass.ps.hlsl", "GbufferMain", SHADER_TYPE_PIXEL);
		ShaderMap::BaseMeshVS = GetHAL()->CreateKernel("shaders/vertex_factory.vs.hlsl", "MeshFactory", SHADER_TYPE_VERTEX);

		// Register 2D shaders
		ShaderMap::SimpleVertexFactoryVS = GetHAL()->CreateKernel("shaders/simple_vertex_factory.vs.hlsl", "vh2d_base", SHADER_TYPE_VERTEX);
		ShaderMap::ScreenVertexVS = GetHAL()->CreateKernel("shaders/simple_vertex_factory.vs.hlsl", "vh_screenvertex", SHADER_TYPE_VERTEX);
		
		// ImGUI
		ShaderMap::ImGuiVS = GetHAL()->CreateKernel("shaders/ImGui.hlsl", "VsMain", SHADER_TYPE_VERTEX);
		ShaderMap::ImGuiPS = GetHAL()->CreateKernel("shaders/ImGui.hlsl", "PsMain", SHADER_TYPE_PIXEL);

		// Post process
		ShaderMap::ComputeHistogramCS = GetHAL()->CreateKernel("shaders/auto_exposure.cs.hlsl", "ComputeHistogram", SHADER_TYPE_COMPUTE);
		ShaderMap::TonemappingPS = GetHAL()->CreateKernel("shaders/post_process.ps.hlsl", "Tonemapping", SHADER_TYPE_PIXEL);
		//ShaderMap::ColorGradingPS = GetHAL()->CreateKernel("shaders/post_process.ps.hlsl", "ph_colorgrading", SHADER_TYPE_PIXEL);
		//ShaderMap::PassThroughPS = GetHAL()->CreateKernel("shaders/post_process.ps.hlsl", "ph_passthrough", SHADER_TYPE_PIXEL);

		//textRTXDI = GetHAL()->CreateKernel("Shaders/RTXDI-Integration/LightingPasses/Presampling/PresampleReGIR.hlsl", "main", SHADER_TYPE_COMPUTE);

		// Raygen
		ShaderMap::RaytracingDebugRGS = GetHAL()->CreateKernel("shaders/raytracing_debug.raygen.hlsl", "main", SHADER_TYPE_RAYGEN);
		ShaderMap::RaytracingDebugInlineRGS = GetHAL()->CreateKernel("shaders/raytracing_debug.raygen.hlsl", "main", SHADER_TYPE_COMPUTE); //inline RT
		//ShaderMap::PathTracerRGS = GetHAL()->CreateKernel("shaders/pathtracer.raygen.hlsl", "main", SHADER_TYPE_RAYGEN);
	}

};
