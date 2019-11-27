#pragma once
#include "Window.h"
#include <Windows.h>

class Win32Window : public IWindow
{
	LRESULT CALLBACK WndProcImpl (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	
public:
	~Win32Window () override;
	void update () override;
	void render () override;
	
	static LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	
};
