#pragma once

class D3D12Shader : public Shader
{
public:
	D3D12_SHADER_BYTECODE m_ByteCode;
	U64 m_Hash;
	ID3DBlob* m_Blob;
	ComPtr<ID3D12ShaderReflection> m_Reflection;
};
