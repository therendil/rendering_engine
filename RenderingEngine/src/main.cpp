#include <Windows.h>
#include <WinBase.h>
#include "Application.h"

int CALLBACK wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	SetThreadDpiAwarenessContext (DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	Application app(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	
	return app.main();
}
