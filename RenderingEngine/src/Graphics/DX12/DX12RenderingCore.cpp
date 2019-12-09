#include "Graphics/DX12/DX12RenderingCore.h"
#include "Graphics/DX12/DX12RenderFactory.h"

DX12RenderingCore::DX12RenderingCore()
{
	_adapter = DX12RenderFactory::GetAdapter (false);
	_device = DX12RenderFactory::CreateDevice (_adapter);
	_commandQueue = DX12RenderFactory::CreateCommandQueue (_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	_swapChain = DX12RenderFactory::CreateSwapChain (nullptr, _commandQueue, width, height, numFrames);
	_currentBackBufferIndex = _swapChain->GetCurrentBackBufferIndex ();
	_descriptorHeap = DX12RenderFactory::CreateDescriptorHeap (_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numFrames);
	_descriptorSize = _device->GetDescriptorHandleIncrementSize (D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void DX12RenderingCore::render()
{
	auto commandAllocator = g_CommandAllocators[g_CurrentBackBufferIndex];
	auto backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];

	commandAllocator->Reset ();
	g_CommandList->Reset (commandAllocator.Get (), nullptr);

	// Clear the render target.
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition (
			backBuffer.Get (),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		g_CommandList->ResourceBarrier (1, &barrier);

		FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv (g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart (),
			g_CurrentBackBufferIndex, g_RTVDescriptorSize);

		g_CommandList->ClearRenderTargetView (rtv, clearColor, 0, nullptr);
	}

	// Present
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition (
			backBuffer.Get (),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		g_CommandList->ResourceBarrier (1, &barrier);

		ThrowIfFailed (g_CommandList->Close ());

		ID3D12CommandList* const commandLists[] = {
			_commandList.Get ()
		};
		_commandQueue->ExecuteCommandLists (_countof (commandLists), commandLists);

		g_FrameFenceValues[g_CurrentBackBufferIndex] = Signal (g_CommandQueue, g_Fence, g_FenceValue);

		UINT syncInterval = g_VSync ? 1 : 0;
		UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed (g_SwapChain->Present (syncInterval, presentFlags));

		g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex ();

		WaitForFenceValue (g_Fence, g_FrameFenceValues[g_CurrentBackBufferIndex], g_FenceEvent);
	}
}
