#include "GameApplication.h"

struct Vertex
{
	D3DXVECTOR3 Pos;
};
CGameApplication::CGameApplication(void)
{
	m_pWindow-NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	m_pVertexBuffer=NULL;
}

CGameApplication::~CGameApplication(void)
{
	if (m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pVertexLayout)
		m_pVertexLayout->Release();

	if (m_pEffect)
		m_pEffect->Release();

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

	D3D10_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc( &techDesc );
	for ( UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0);
		m_pD3D10Device->Draw(3,0);
	}

	//flip the swap chain.
	m_pSwapChain->Present (0,0);
}

void CGameApplication::update()
{
	//This function is used to update the game state, AI, input devices and physics
}

bool CGameApplication::initGame()
{
	D3D10_BUFFER_DESC bd;
	// Describes how the buffer is written and read to and from. Default states that the resource will be written to and read by the GPU
	bd.Usage = D3D10_USAGE_DEFAULT;
	// The size of the buffer (3 Vertices)
	bd.ByteWidth = sizeof( Vertex ) * 3;
	//The type of buffer we are creating, this case - a combination of bind flags . and saying vertex buffer states that its a vertex buffer
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	// This specifies wither the CPU can access the buffer. 0 means no
	bd.CPUAccessFlags = 0;
	// This is to ask for any additional options, 0 means there are no additional options
	bd.MiscFlags = 0;

	// Defining 3 simple vertices 
	Vertex vertices[] =
	{
		D3DXVECTOR3(0.0f, 0.5f, 0.5f),
		D3DXVECTOR3(0.5f,-0.5f, 0.5f),
		D3DXVECTOR3(-0.5f,-0.5f, 0.5f),
	};

	// setting the pSysMem of the SUBRESOURCE_DATA to the vertices
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined (_DEBUG)
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	/* This large function call with 12 paramaters but we only get a few definitions 
	   the 1st paramater LPCSTR - the filename of the effect file
	   the 4th paramater LPCSTR - The shader profile we are using
	   the 5th paramater DWORD - the shader flags this allows it to collect debug info about the shader for example
	   the 7th paramater ID3D10Device* - A pointer to a valid device which will use this effect
	   the 10th paramater ID3D10Effect** - a pointer to a memory address of an effect object
	*/
	if(FAILED(D3DX10CreateEffectFromFile(TEXT("ScreenSpace.fx"),NULL,NULL,"fx_4_0",dwShaderFlags,0,m_pD3D10Device,NULL,NULL, &m_pEffect,NULL,NULL)))
	{
		MessageBox (NULL,TEXT("The FX file cannot be located, please run executable from the directory that contains the FX file."),TEXT("Error"), MB_OK);
		return false;
	}

	//We search the m_pEffect for the technique by the string name render
	m_pTechnique=m_pEffect->GetTechniqueByName("Render");

	//Creating the buffer how ever it takes in the following paramaters 
	/* D3D10_BUFFER_DESC* - A pointer to a buffer description
	   D3D10_SUBRESOURCE_DATA* - A pointer to the resource data
	   ID3D10Buffer** - Memory address of a pointer to a buffer
	*/
	if (FAILED(m_pD3D10Device->CreateBuffer( &bd, &InitData, &m_pVertexBuffer)))
		return false;

	/* Array of these input parameters as we can have many different elements of a vertex
	   1st parameter LPCSTR - a string which specifies the semantic that this element is bount to. this allows it to link up vertices from the buffer to the vertices passed into the vertex shader
	   2nd parameter UINT - Index of the semantic, again this is used to bind a vertex in the pipeline
	   3rd parameter DXGI_FORMAT - the format of that data, in this case 3 components with 32 bits per componont which are all floating point numbers
	   5th parameter UINT - the starting offset of the element, this will increase for the subsequent elements in the array.
	*/
	D3D10_INPUT_ELEMENT_DESC layout[]=
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
	};

	// calculating the size of the input array
	UINT numElements = sizeof ( layout ) / sizeof(D3D10_INPUT_ELEMENT_DESC);
	// retrieving the pass description
	D3D10_PASS_DESC PassDesc;
	//using this to bind the layout
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

	/* The call to createinputlayout will create the input layout using the follow parameters 
		 1st parameter D3D10_INPUT_ELEMENT_DESC* - an array of input element descriptions
		 2nd parameter UINT - The number of elements in the input elements array
		 3rd parameter const void* - a pointer to the complied shader code, this retrieced from the pass using the input signature variable of the Pass Description
		 4th parameter SIZE_T - the size of the above shader cod, this is retrieved for the pass description
		 5th parameter ID3D10InputerLayout** - a pointer to a memory address of the input layout object
	*/
	if(FAILED(m_pD3D10Device->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout)))
	{
		return false;
	}

	// telling the input assembler about the layout
	m_pD3D10Device->IASetInputLayout( m_pVertexLayout );
	//holding the size of one vertex
	UINT stride = sizeof( Vertex );
	// where the vertices will start in the buffer
	UINT offset = 0;

	/* The IASetVertexBuffer will bind one of many buffers to the input assembler to use, it needs the following parameters;
			UINT - The input slow to bind, 0 indicates the first slow. We can bind a buffer to different slots so we can change buffer as we render
			UINT - The number of buffers we are binding
			ID3D10Buffer** - a pointer to a memory address of buffer
			UINT* - an array of strides for the buffer
			UINT* - an array of offsets for the buffer
	*/
	m_pD3D10Device->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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


