#include "ShaderCompiler.h"
#include "d3dUtil.h"
#include <d3d11.h>
#include <string>
 ShaderCompiler* Shader::VSTexShader=nullptr;
 ShaderCompiler* Shader::PSTexShader=nullptr;


ID3D11VertexShader* Shader::pVSShader=nullptr;
ID3D11PixelShader* Shader::pPSShader=nullptr;


ShaderCompiler::ShaderCompiler()
{

	



}

ShaderCompiler::~ShaderCompiler()
{
	ReleaseCOM(pErrorBlob);
	ReleaseCOM(pBlob);
}

void ShaderCompiler::Compiler(std::wstring filename, std::string EntryPoint, std::string Target)
{
	HRESULT hr = S_OK;

	
	//compiler  shader
	hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr,"main",Target.c_str(),NULL, NULL, &pBlob, &pErrorBlob);
	if (pErrorBlob != nullptr)
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
}

void Shader::InitAll()
{
	VSTexShader = new ShaderCompiler();
	PSTexShader = new ShaderCompiler();
	
	VSTexShader->Compiler(L"VertexShader.hlsl", "VS", "vs_5_0");
	PSTexShader->Compiler(L"PixelShader.hlsl", "PS", "ps_5_0");
}

void Shader::DestroyAll()
{
	SafeDelete(VSTexShader);
	SafeDelete(PSTexShader);
	ReleaseCOM(pVSShader);
	ReleaseCOM(pPSShader);
}

void Shader::CreateAllShaders(ID3D11Device* pDevice)
{
	pDevice->CreateVertexShader(VSTexShader->pBlob->GetBufferPointer(), VSTexShader->pBlob->GetBufferSize(),
		nullptr, &pVSShader);

	pDevice->CreatePixelShader(PSTexShader->pBlob->GetBufferPointer(), PSTexShader->pBlob->GetBufferSize(),
		nullptr, &pPSShader);
}
