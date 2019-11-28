#include "Graphics/DX12/DX12RenderingCore.h"
#include "Graphics/DX12/DX12RenderFactory.h"

DX12RenderingCore::DX12RenderingCore()
{
	_adapter = DX12RenderFactory::GetAdapter (false);
	_device = DX12RenderFactory::CreateDevice (_adapter);
}
