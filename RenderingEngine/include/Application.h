#pragma once
#include <minwindef.h>
#include "Window.h"

class Application
{
	HINSTANCE _hInstance;
	Window* _window;
	
public:
	Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

	void init();
	
	int main();
};
