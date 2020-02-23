#pragma once
#include <d3dCompiler.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <string>
class  ShaderCompiler
{
public:
	
	ShaderCompiler();
	~ShaderCompiler();
	void Compiler(std::wstring filename, std::string EntryPoint, std::string Target);


	ID3DBlob * pErrorBlob = nullptr;
	ID3DBlob * pBlob = nullptr;
	D3D_SHADER_MACRO Macro;
	
};

class Shader
{
public:

   

	static void InitAll();
	static void DestroyAll();
	static void CreateAllShaders(ID3D11Device* pDevice);
	static ShaderCompiler* VSTexShader;
	static ShaderCompiler* PSTexShader;


	static ID3D11VertexShader* pVSShader;
	static ID3D11PixelShader* pPSShader;
};






