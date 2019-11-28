#include "Window.h"
#include <cassert>
#include <algorithm>
#include "GraphicsAPI.h"

Window::Window (HINSTANCE hInstance, const wchar_t* windowClassName, int width, int height) : _windowClassName (windowClassName), _width(width), _height(height)
{
	RegisterWindowClass (hInstance, windowClassName);
	_hWnd = CreateWindow (windowClassName, hInstance, windowClassName, _width, _height);
	_vSync = GraphicsAPI::CheckTearingSupport ();

	::GetWindowRect (_hWnd, &_windowRect);
}

Window::~Window()
{
}

void Window::update()
{
}

void Window::render()
{
}

void Window::setFullscreen(bool on)
{
}

void Window::resize(int w, int h)
{
}

void Window::show() const
{
	::ShowWindow(_hWnd, SW_SHOW);
}

LRESULT CALLBACK Window::WndProcImpl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//TODO: SetWindowLongPtr (hwnd, GWLP_USERDATA, (LONG_PTR)this);
	if (_isInitialized)
	{
		switch (message)
		{
		case WM_PAINT:
			update();
			render();
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case 'V':
				_vSync = !_vSync;
				break;
			case VK_ESCAPE:
				::PostQuitMessage(0);
				break;
			case VK_RETURN:
				if (alt)
				{
			case VK_F11:
				setFullscreen(!_fullscreen);
				}
				break;
			}
		}
		break;
		// The default window procedure will play a system notification sound 
		// when pressing the Alt+Enter keyboard combination if this message is 
		// not handled.
		case WM_SYSCHAR:
			break;
		case WM_SIZE:
		{
			RECT clientRect = {};
			::GetClientRect(_hWnd, &clientRect);

			const int width = clientRect.right - clientRect.left;
			const int height = clientRect.bottom - clientRect.top;

			resize(width, height);
		}
		break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			break;
		default:
			return ::DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else
	{
		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* window = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (window)
		return window->WndProcImpl(hwnd, message, wParam, lParam);

	return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND Window::CreateWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, uint32_t width, uint32_t height)
{
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

	HWND hWnd = ::CreateWindowExW(
		NULL,
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInst,
		nullptr
	);

	assert(hWnd && "Failed to create window");

	return hWnd;
}

void Window::RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName)
{
	// Register a window class for creating our render window with.
	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = ::LoadIcon(hInst, NULL); //  MAKEINTRESOURCE(APPLICATION_ICON));
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = ::LoadIcon(hInst, NULL); //  MAKEINTRESOURCE(APPLICATION_ICON));

	static HRESULT hr = ::RegisterClassExW(&windowClass);
	assert(SUCCEEDED(hr));
}
