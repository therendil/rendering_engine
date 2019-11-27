#include "Win32Window.h"


Win32Window::~Win32Window()
{
}

void Win32Window::update()
{
}

void Win32Window::render()
{
}

LRESULT CALLBACK Win32Window::WndProcImpl (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//TODO: SetWindowLongPtr (hwnd, GWLP_USERDATA, (LONG_PTR)this);
	if (_isInitialized)
	{
		switch (message)
		{
		case WM_PAINT:
			update ();
			render ();
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bool alt = (::GetAsyncKeyState (VK_MENU) & 0x8000) != 0;

			switch (wParam)
			{
			case 'V':
				_vSync = !_vSync;
				break;
			case VK_ESCAPE:
				::PostQuitMessage (0);
				break;
			case VK_RETURN:
				if (alt)
				{
			case VK_F11:
				setFullscreen (!_fullscreen);
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
			::GetClientRect (g_hWnd, &clientRect);

			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;

			Resize (width, height);
		}
		break;
		case WM_DESTROY:
			::PostQuitMessage (0);
			break;
		default:
			return ::DefWindowProcW (hwnd, message, wParam, lParam);
		}
	}
	else
	{
		return ::DefWindowProcW (hwnd, message, wParam, lParam);
	}

	return 0;
}


LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32Window* window = (Win32Window*)(GetWindowLongPtr (hwnd, GWLP_USERDATA));
	if (window) return window->WndProcImpl (hwnd, message, wParam, lParam);
	return DefWindowProc (hwnd, message, wParam, lParam);
}
