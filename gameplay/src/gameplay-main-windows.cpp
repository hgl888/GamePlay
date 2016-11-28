#ifndef GP_NO_PLATFORM
#ifdef WIN32

#include "gameplay.h"
#include "VulkanExample.h"

using namespace vk;

#ifndef _WINDOWS_
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif


VulkanExample *vulkanExample;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (vulkanExample != NULL)
	{
		vulkanExample->handleMessages(hWnd, uMsg, wParam, lParam);
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	vulkanExample = new VulkanExample();
	vulkanExample->setupWindow(hInstance, WndProc);
	vulkanExample->initSwapchain();
	vulkanExample->prepare();
	vulkanExample->renderLoop();
	delete(vulkanExample);
	return 0;
}

/**
 * Main entry point.
 */
//extern "C" int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
//{
//	Game* game = Game::getInstance();
//	Platform* platform = Platform::create(game);
//	GP_ASSERT(platform);
//	int result = platform->enterMessagePump();
//	delete platform;
//	return result;
//}

#endif
#endif
