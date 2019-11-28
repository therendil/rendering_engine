#include "Application.h"
#include <shellapi.h> // For CommandLineToArgvW

Application::Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) : _hInstance(hInstance)
{
}

void Application::init()
{
	auto args = ParseCommandLineArgs ();
	_window = new Window(_hInstance, L"RenderingEngine", std::get<0>(args), std::get<1> (args));
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

auto Application::ParseCommandLineArgs()
{
	{
		int argc;
		wchar_t** argv = ::CommandLineToArgvW (::GetCommandLineW (), &argc);

		int width = 800;
		int height = 600;
		bool useWarp = false;

		for (size_t i = 0; i < argc; ++i)
		{
			if (::wcscmp (argv[i], L"-w") == 0 || ::wcscmp (argv[i], L"--width") == 0)
			{
				width = ::wcstol (argv[++i], nullptr, 10);
			}
			if (::wcscmp (argv[i], L"-h") == 0 || ::wcscmp (argv[i], L"--height") == 0)
			{
				height = ::wcstol (argv[++i], nullptr, 10);
			}
			if (::wcscmp (argv[i], L"-warp") == 0 || ::wcscmp (argv[i], L"--warp") == 0)
			{
				useWarp = true;
			}
		}

		// Free memory allocated by CommandLineToArgvW
		::LocalFree (argv);

		return std::make_tuple (width, height, useWarp);
	}
}
