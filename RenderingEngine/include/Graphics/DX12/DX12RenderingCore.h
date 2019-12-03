#pragma once
#include "Graphics/RenderingCore.h"
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class DX12RenderingCore : public IRenderingCore
{
	Microsoft::WRL::ComPtr<IDXGIAdapter4> _adapter;
	Microsoft::WRL::ComPtr<ID3D12Device2> _device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapChain;
	UINT _currentBackBufferIndex;
	uint32_t numFrames;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _descriptorHeap;
	UINT _descriptorSize;
public:
	DX12RenderingCore ();
	void render () override;
};
