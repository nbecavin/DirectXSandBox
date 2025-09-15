#ifndef __RENDERER_SHADER_HH__
#define __RENDERER_SHADER_HH__

enum EShaderType
{
	SHADER_TYPE_VERTEX=1,
	SHADER_TYPE_PIXEL,
	SHADER_TYPE_MESH,
	SHADER_TYPE_COMPUTE,
	SHADER_TYPE_RAYGEN,

	SHADER_TYPE_BITS = (28),
	SHADER_TYPE_MASK = (0xf0000000)
};

class ShaderKernel
{
public:
	Shader* m_ShaderBlob = nullptr;
	EShaderType m_Type;
};

struct ShaderMap
{
	static ShaderKernel* BasePassPS;
	static ShaderKernel* GBufferPassPS;
	static ShaderKernel* BaseMeshVS;
	static ShaderKernel* SimpleVertexFactoryVS;
	static ShaderKernel* ScreenVertexVS;
	static ShaderKernel* ColorGradingPS;
	static ShaderKernel* PassThroughPS;
	static ShaderKernel* ImGuiVS;
	static ShaderKernel* ImGuiPS;
};

#endif //__RENDERER_SHADER_HH__
