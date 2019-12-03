#pragma once

enum class RHI_TYPE
{
	DX12,
	VULKAN,
	OPENGL
};

class IRenderingCore
{
public:
	virtual ~IRenderingCore () = default;
	virtual void render () = 0;

	static IRenderingCore* CreateRenderer (RHI_TYPE type);
};