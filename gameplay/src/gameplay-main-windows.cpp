#ifndef GP_NO_PLATFORM
#ifdef WIN32

#include "gameplay.h"

using namespace vk;

#ifndef _WINDOWS_
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif


//VulkanExample *vulkanExample;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Game* game = Game::getInstance();
	if (game != NULL)
	{
		game->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

/**
* Main entry point.
*/
extern "C" int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	Game* game = Game::getInstance();
	//game->setupWindow(hInstance, WndProc);
	Platform* platform = Platform::create(game);
	game->InitVulkanExample(true);	
	game->initSwapchain();
	game->prepare();
	//game->renderLoop();
	int result = platform->enterMessagePump();
	game->UnInitVulkanExample();
	delete platform;
	return 0;
}


/**
 * Main entry point.
 */
extern "C" int APIENTRY WinMainTest(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	Game* game = Game::getInstance();
	Platform* platform = Platform::create(game);
	GP_ASSERT(platform);
	int result = platform->enterMessagePump();
	delete platform;
	return result;
}

#endif
#endif
