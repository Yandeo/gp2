#include "GameApplication.h"

struct Vertex
{
	D3DXVECTOR3 Pos;
}

CGameApplication::CGameApplication(void)
{
	m_pWindow-NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
}

CGameApplication::~CGameApplication(void)
{
	if (m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();

	if(m_pSwapChain)
		m_pSwapChain->Release();

	if (m_pD3D10Device)
		m_pD3D10Device->Release();

	if (m_pWindow)
	{
		delete m_pWindow;
		m_pWindow=NULL;
	}
}

bool CGameApplication::init()
{
	//This is the function where we initialize our game.
	if(!initWindow())
		return false;

	if(!initGraphics())
		return false;

	if(!initGame())
		return false;

	return true;
}

bool CGameApplication::run()
{
	//this function's job is to continue to check for window messages until the window is closed and render the scene if there are no window messages.
	while(m_pWindow->running())
	{
		if(! m_pWindow->checkForWindowMessages())
		{
			update();
			render();
		}
	}
	return false;
}

void CGameApplication::render()
{
	// sets up a float array of colors (Red, Green, Blue and alpha) which has values from 0 to 1 for each component
	float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f };
	// uses the above color value and will clear the render target to that color
	m_pD3D10Device->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	//flip the swap chain.
	m_pSwapChain->Present (0,0);
}

void CGameApplication::update()
{
	//This function is used to update the game state, AI, input devices and physics
}

bool CGameApplication::initGame()
{
	return true;
}

bool CGameApplication::initGraphics()
{
	// In this function we initialize Direct3D10.
	RECT windowRect;
	//Retriving the windows attributes
	GetClientRect(m_pWindow->getHandleToWindow(),&windowRect);

	//Storing the width of the window in unsigned int called width
	UINT width=windowRect.right-windowRect.left;
	//Storing the height of the window in unsigned int called height
	UINT height=windowRect.bottom-windowRect.top;

	// Initilizing a unsigned int to hold the flags for device creation. These flags are often used to put the device into debug mode
	UINT createDeviceFlags=0;

#ifdef DEBUG
	//checking to see if we are in debug mode (preprocessir flag)
	createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG;
#endif
	// Initilzing a variable of type DXGI_SWAP_CHAIN_DESC calling it sd.
	//This variable holds all the options for creation of the Swap Chain.
	DXGI_SWAP_CHAIN_DESC sd;
	// Takes the size and memory address and sets the values to zero of the variable sd
	ZeroMemory( &sd, sizeof ( sd ) );
	
	//Checking to see if we are in full screen mode
	if (m_pWindow->isFullScreen())
		// Creates 2 bufferss (front and back)
		sd.BufferCount = 2;
	else
		// states that we are not in full screen mode and only need 1 buffer and the desktop
		sd.BufferCount=1;

	//asociates a window handle with the swap chain description
	sd.OutputWindow = m_pWindow->getHandleToWindow();
	//if in windowed mode then concert boolen into BOOL
	sd.Windowed = (BOOL)(!m_pWindow->isFullScreen());
	//this makes the buffers render targets (something we can draw to)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//This sets up the multisampling(antialsing) parameters for the swap chain.
	sd.SampleDesc.Count =1;
	sd.SampleDesc.Quality = 0;

	//setting the options for the underlying buffer
	//width
	sd.BufferDesc.Width = width;
	//height
	sd.BufferDesc.Height = height;
	// 8 bytes for each compnent (red,gree,blue and alpha)
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// refresh rate
	sd.BufferDesc.RefreshRate.Numerator= 60;
	sd.BufferDesc.RefreshRate.Denominator= 1;

	/*this will create the swap. This is surrounded by a if and FAILED . This checks if the function has failed
	   the paramaters it takes in are as soo ;
	   IDXGIAdpater* - a pointer to IDXGIAdpater is analogous to a physical graphics card - we have set it to NULL (the default adapter
	   D3D10_DRIVER_TYPE - type of driver flag, there are 2 types HARDWARE which is the hardware device and REFERENCE which is an reference driver which emulates most of D3D10 features.
	   HMODULE - a handle to a module (dynamic link library) which contains a software implementation of D3D10
	   UNIT - D3D10_CREATE_DEVICE_FLAG, this is optional. we use this to give addtional options when we create the device
	   UNIT - The version of D3D10 we are working with. this will ALWAYS be D3D10_SDK_VERSION
	   DXGI_SWAP_CHAIN_DESC* - a pointer to the swap chain descritption, we use the memory address operator (&) to get this pointer. Holds the options for the swap chain creation
	   IDXGISwapChain** - An address of a pointer to swap chain interface, this will initilize the IDXGISwapChain pointer
	   ID3D!0Device** - And address of a pointer to a D3D10 Device. This will inilize the ID3D10Device pointer*/
	if (FAILED(D3D10CreateDeviceAndSwapChain(NULL,D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_pD3D10Device)))
		return false;

	/* Associate a buffer for the swap chain with the render target view.
	   retrive this buffer using the GetBuffer function of the Swap Chain interface, in this case the buffer is going to be of type ID3D10Texture2D
	   The GetBuffer takes in paramaters as soo ;
	   UNIT - an indec of the buffer inside the Swap Chain. Zero will retrive the back buffer
	   REFIID - an id of the type of interface we are retricing for the swap chain. The _uuidof function will retrive the unique ID of the interface
	   void** - a pointer to an address of the buffer the reason this is void is beacause this function can take any type demending on the value of the REFIID
	*/
	ID3D10Texture2D *pBackBuffer;
	if ( FAILED (m_pSwapChain->GetBuffer(0,_uuidof(ID3D10Texture2D),(void**)&pBackBuffer)))
		return false;

	/* Creating a Render Target view, This view is the wayt to bind certain resources to the pipeline for rendering.
	   To CreateRenderTargetView we take in parameters as soo ; 
	   ID3D10Resource* - a pointer to a resource, a Texture2D interface inherits for this Resource interface so it can be passed as a paramater to this function
	   D3D10_RENDER_TARGET_VIEW_DESC* - A pointer to a structure which defines options for accessing parts of the render target such as sub areas of the resource
	   ID3D10RenderTargetView** - A pointer to an address of a render target view. After this call we should have a valid render target view*/
	if (FAILED(m_pD3D10Device->CreateRenderTargetView ( pBackBuffer,NULL, &m_pRenderTargetView)))
	{
		pBackBuffer->Release();
		return false;
	}
	pBackBuffer->Release();

	/*Binds an array of Render Targets to the Output Merger stage of the pipeline.
	  UINT - This values specifies the amount of render targets to bind to the pipeline
	  ID3D10RenderTargetView* - A pointer to an array of render targets
	  ID3D10DepthStencilView* - A pointer to a depth stencil this holds depth information of the scene.
	*/
	m_pD3D10Device->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	// Sets up a D3D10_VIEWPORT instance this is the same width as the window
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	// calling the RSSetViewports function to set the view port which is bound to the pipeline
	m_pD3D10Device->RSSetViewports (1, &vp );
	
	return true;
}

bool CGameApplication::initWindow()
{
	// This function we initalize a Win32 window
	//This code snippet alloactes a new instance of the Win32 window then calls the init function to actually create the window.
	m_pWindow=new CWin32Window();
	if(!m_pWindow->init(TEXT("Lab 1 Drawing a triangle"),800,640,false))
		return false;

	return true;
}


