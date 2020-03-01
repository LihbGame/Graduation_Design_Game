#ifndef D3DAPP_H
#define D3DAPP_H

#include "d3dUtil.h"
#include "GameTimer.h"
#include <string>
#include "GameGUI.h"


//fbx model tansform informaition struct

struct Model_Tansform_Info
{
	int Model_Instance_Num;
	//如果是动态模型使用以下两个属性
	std::vector<XMMATRIX> Mat_tansform_Rot_Scal;
	std::vector<XMMATRIX> Mat_tansform_Translation;

	std::vector<XMMATRIX> Mat_World;
};


class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();
	
	static D3DApp* Get();
	ID3D11Device* GetD3DDevice();
	ID3D11DeviceContext* GetD3DDeviceContext();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;
	
	int Run();
 
	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.

	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0; 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

protected:
	static D3DApp* gd3dApp;
	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mFullScreen;
	bool      mResizing;
	UINT      m4xMsaaQuality;
	
	GameTimer mTimer;

	


	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriverType;
	int mClientWidth;
	int mClientHeight;
	bool mEnable4xMsaa;

	//GUI
	GameGUI* m_pGameGUI;
	Noesis::Ptr<Noesis::IView> g_XamlView;
	Noesis::Key* g_KeyMap;
};


//----------------------------------------------------------------
inline D3DApp* D3DApp::Get()
{
	return gd3dApp;
}
//----------------------------------------------------------------
inline ID3D11Device* D3DApp::GetD3DDevice()
{
	return md3dDevice;
}
//----------------------------------------------------------------
inline ID3D11DeviceContext* D3DApp::GetD3DDeviceContext()
{
	return md3dImmediateContext;
}


#endif // D3DAPP_H