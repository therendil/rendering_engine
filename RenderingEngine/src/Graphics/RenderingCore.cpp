#include "Graphics/RenderingCore.h"
#include "Graphics/DX12/DX12RenderingCore.h"

IRenderingCore* IRenderingCore::CreateRenderer(RHI_TYPE type)
{
	switch (type)
	{
	case RHI_TYPE::DX12:
		return new DX12RenderingCore ();
	case RHI_TYPE::VULKAN:
	case RHI_TYPE::OPENGL:
	default:
		return nullptr;
	}
}
