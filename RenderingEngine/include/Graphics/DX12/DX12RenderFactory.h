#pragma once

#include "Graphics/RenderFactory.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <exception>
#include <d3d12.h>
#include <cstdint>

constexpr void ThrowIfFailed (HRESULT hr)
{
	if (FAILED (hr))
	{
		// Set a breakpoint on this line to catch DirectX API errors
		throw std::exception ("HRESULT failed.");
	}
}

namespace WRL = Microsoft::WRL;

class DX12RenderFactory : public IRenderFactory
{
	~DX12RenderFactory () override = default;
public:
	static bool CheckTearingSupport ();
	static WRL::ComPtr<IDXGIAdapter4> GetAdapter (bool useWarp);
	static WRL::ComPtr<ID3D12Device2> CreateDevice (WRL::ComPtr<IDXGIAdapter4> adapter);
	static WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue (WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	static WRL::ComPtr<IDXGISwapChain4> CreateSwapChain (HWND hWnd, WRL::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, uint32_t bufferCount);
	static WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap (WRL::ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
	
};
