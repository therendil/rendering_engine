#pragma once

class IWindow
{
protected:
	bool _fullscreen;
	bool _vSync;
	bool _isInitialized;
public:
	virtual ~IWindow () = default;
	virtual void update () = 0;
	virtual void render () = 0;
	void setFullscreen (bool on);
};

inline void IWindow::setFullscreen(bool on)
{
}
