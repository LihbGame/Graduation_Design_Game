#pragma once

#include <Noesis_pch.h>
//#include <NsRender/D3D11RenderDeviceApi.h>
#include "D3D11RenderDevice.h"
class GameGUI
{
public:
	GameGUI();
	~GameGUI();
	void Init(ID3D11DeviceContext* context,int g_Width,int g_Height);
	void Render();
	void PreRender();
	void Update(float dt);
	void SetupKeyMap();
	Noesis::Key* GetKeyMap();
	Noesis::Ptr<Noesis::IView> GetXamlView();
private:
	// This is the main view that displays the content
	Noesis::Ptr<Noesis::IView> g_XamlView;
	// Map translation from windows keycodes to Noesis enums
	Noesis::Key g_KeyMap[256];
	

	Noesis::Ptr<Noesis::FrameworkElement> xaml;
	
};

