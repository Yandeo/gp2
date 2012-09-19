#pragma once

#include "Win32Window.h"
#include <D3D10.h>
#include <D3DX10.h>

class CGameApplication
{
public:
	//This is the Game Application calss constructor, It will create an instance of the class
	CGameApplication(void);
	//This deconstructor we deallocate all the resources, the D3D10 objects we call the release function and we also delete the window
	~CGameApplication(void);

	bool init();
	bool run();

private:
	bool initGame();
	bool initGraphics();
	bool initWindow();

	void render();
	void update();

private:
	ID3D10Device * m_pD3D10Device;
	IDXGISwapChain * m_pSwapChain;
	ID3D10RenderTargetView * m_pRenderTargetView;
	CWin32Window * m_pWindow;
	ID3D10Buffer* m_pVertexBuffer;
	ID3D10InputLayout* m_pVertextLayout;
	ID3D10Effect* m_pEffect;
	ID3D10EffectTechnique* m_pTechnique;
};