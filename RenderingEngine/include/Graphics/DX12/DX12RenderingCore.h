#pragma once
#include "Graphics/RenderingCore.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>

class DX12RenderingCore : public IRenderingCore
{
	Microsoft::WRL::ComPtr<IDXGIAdapter4> _adapter;
	Microsoft::WRL::ComPtr<ID3D12Device2> _device;

public:
	DX12RenderingCore ();
};
