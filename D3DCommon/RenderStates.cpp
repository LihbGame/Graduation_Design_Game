#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS    = 0;
ID3D11RasterizerState* RenderStates::CullBackRS = 0;

ID3D11DepthStencilState* RenderStates::LessEqualsDSS = 0;

ID3D11BlendState* RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState* RenderStates::TransparentBS = 0;
ID3D11BlendState* RenderStates::WriteAlphaBS = 0;
ID3D11BlendState* RenderStates::WriteRgbaBS = 0;
void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	//
	// NoCullRS
	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	//
	// CullBackRS
	//
	D3D11_RASTERIZER_DESC CullBackDesc;
	ZeroMemory(&CullBackDesc, sizeof(D3D11_RASTERIZER_DESC));
	CullBackDesc.FillMode = D3D11_FILL_SOLID;
	CullBackDesc.CullMode = D3D11_CULL_FRONT;
	CullBackDesc.FrontCounterClockwise = false;
	CullBackDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&CullBackDesc, &CullBackRS));

	//
	// LessEqualsDSS
	//
	D3D11_DEPTH_STENCIL_DESC equalsDesc;
	ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	equalsDesc.DepthEnable = true;
	equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equalsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	HR(device->CreateDepthStencilState(&equalsDesc, &LessEqualsDSS));



	//
	// AlphaToCoverageBS
	//

	D3D11_BLEND_DESC alphaToCoverageDesc = {0};
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = {0};
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));

	//WriteAlphaBS
	D3D11_BLEND_DESC WriteAlphaDesc = { 0 };
	/*WriteAlphaDesc.RenderTarget[0].BlendEnable = true;
	WriteAlphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	WriteAlphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	WriteAlphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	WriteAlphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	WriteAlphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	WriteAlphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;*/
	WriteAlphaDesc.RenderTarget[0].RenderTargetWriteMask= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	HR(device->CreateBlendState(&WriteAlphaDesc, &WriteAlphaBS));

	//WriteRgbBS
	D3D11_BLEND_DESC WriteRgbaBSDesc = { 0 };
	WriteRgbaBSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&WriteRgbaBSDesc, &WriteRgbaBS));
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(WireframeRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(CullBackRS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
	ReleaseCOM(LessEqualsDSS);
	ReleaseCOM(WriteAlphaBS);
	ReleaseCOM(WriteRgbaBS);
}