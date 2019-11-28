#pragma once

#include "Graphics/RenderFactory.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <exception>
#include <d3d12.h>

inline void ThrowIfFailed (HRESULT hr)
{
	if (FAILED (hr))
	{
		// Set a breakpoint on this line to catch DirectX API errors
		throw std::exception ();
	}
}

class DX12RenderFactory : public IRenderFactory
{
	~DX12RenderFactory () override;
public:
	static bool CheckTearingSupport ();
	static Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter (bool useWarp);
	static Microsoft::WRL::ComPtr<ID3D12Device2> CreateDevice (Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
};
