#include "TextureMgr.h"

TextureMgr::TextureMgr() : md3dDevice(0)
{
}

TextureMgr::~TextureMgr()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
}

void TextureMgr::Init(ID3D11Device* device)
{
	md3dDevice = device;
}

ID3D11ShaderResourceView* TextureMgr::CreateTexture(std::wstring filename)
{
	ID3D11ShaderResourceView* srv = 0;
	ID3D11Resource* texResource = nullptr;

	// Does it already exist?
	if( mTextureSRV.find(filename) != mTextureSRV.end() )
	{
		srv = mTextureSRV[filename];
	}
	else
	{
		//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, filename.c_str(), 0, 0, &srv, 0 ));
		HR(DirectX::CreateDDSTextureFromFile(md3dDevice, filename.c_str(), &texResource, &srv));

		mTextureSRV[filename] = srv;
	}
	ReleaseCOM(texResource);
	return srv;
}
 
