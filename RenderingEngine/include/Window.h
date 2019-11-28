#pragma once
#include "Window.h"
#include <Windows.h>
#include <cstdint>

#ifdef CreateWindow
#undef CreateWindow
#endif

class Window
{
	bool _fullscreen;
	bool _vSync;
	bool _isInitialized;
	int _width;
	int _height;
	const wchar_t* _windowClassName;
	HWND _hWnd;
	LRESULT CALLBACK WndProcImpl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	Window(HINSTANCE hInstance, const wchar_t* windowClassName);
	~Window();
	
	void update();
	void render();
	void setFullscreen(bool on);
	void resize(int w, int h);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND CreateWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, uint32_t width, uint32_t height);
	static void RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName);
};
