#include "Application.h"

Application::Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) : _hInstance(hInstance)
{
}

void Application::init()
{
	_window = new Window(_hInstance, L"RenderingEngine");
}

int Application::main()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	
	return 0;
}
