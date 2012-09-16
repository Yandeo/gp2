//Are we on the windows platform?
#ifdef WIN32
//if so include the header for windows applications
#include <Windows.h>
#include "GameApplication.h"
#endif

#include "Win32Window.h"

//Check to see if we are on windows
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine,
					int nCmdShow)
#else
//we are on some other platform
int main(int argc, char **argv)
#endif
{
	// this is the entry point to the Game Application class.
	// allocating a pointer to the game application class
	CGameApplication *pApp=new CGameApplication();

	//Checking to see if it can be initialized
	if(!pApp->init())
	{
		//if it cant be we delete the pointer 
		if (pApp)
		{
			delete pApp;
			pApp=NULL;
			return 1;
		}
	}
	// if it can be we call the run function whiche acts like our game loop
	pApp->run();

	if (pApp)
	{
		delete pApp;
		pApp=NULL;
	}

	return 0;
}

