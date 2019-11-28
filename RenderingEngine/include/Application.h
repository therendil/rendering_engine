#pragma once
#include <minwindef.h>
#include "Window.h"
#include "Graphics/RenderingCore.h"


class Application
{
	HINSTANCE _hInstance;
	Window* _window;
	IRenderingCore* _renderingCore;
	
public:
	Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

	void init();
	
	int main();

private:
	static auto ParseCommandLineArgs ();

};
