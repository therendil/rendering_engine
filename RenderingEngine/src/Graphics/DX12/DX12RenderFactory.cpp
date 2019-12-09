#include "Graphics/DX12/DX12RenderFactory.h"
#include <d3d12.h>
#include <d3dx12.h>
using namespace Microsoft::WRL;

bool DX12RenderFactory::CheckTearingSupport ()
{
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	ComPtr<IDXGIFactory7> factory;
	if (SUCCEEDED (CreateDXGIFactory1 (IID_PPV_ARGS (&factory))))
	{
		return SUCCEEDED (factory->CheckFeatureSupport (DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof (allowTearing)));
	}

	return FALSE;
}

ComPtr<IDXGIAdapter4> DX12RenderFactory::GetAdapter (bool useWarp)
{
	ComPtr<IDXGIFactory7> factory;
	UINT createFactoryFlags = 0;

#ifdef _DEBUG
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed (CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)));

	ComPtr<IDXGIAdapter1> adapter1;
	ComPtr<IDXGIAdapter4> adapter4;

	if (useWarp)
	{
		ThrowIfFailed (factory->EnumWarpAdapter (IID_PPV_ARGS (&adapter1)));
		ThrowIfFailed (adapter1.As (&adapter4));
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; factory->EnumAdapters1 (i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			adapter1->GetDesc1 (&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED (D3D12CreateDevice (adapter1.Get (),
					D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				ThrowIfFailed (adapter1.As (&adapter4));
			}
		}
	}

	return adapter4;
}

ComPtr<ID3D12Device2> DX12RenderFactory::CreateDevice(ComPtr<IDXGIAdapter4> adapter)
{
	ComPtr<ID3D12Device6> d3d12Device;
	ThrowIfFailed (D3D12CreateDevice (adapter.Get (), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS (&d3d12Device)));

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;

	if (SUCCEEDED(d3d12Device.As(&infoQueue)))
	{
		infoQueue->SetBreakOnSeverity (D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		infoQueue->SetBreakOnSeverity (D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		infoQueue->SetBreakOnSeverity (D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

		D3D12_MESSAGE_ID denyIds[] = {
		   D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
		   D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
		   D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER newFilter = {};
		newFilter.DenyList.NumSeverities = _countof (severities);
		newFilter.DenyList.pSeverityList = severities;
		newFilter.DenyList.NumIDs = _countof (denyIds);
		newFilter.DenyList.pIDList = denyIds;

		ThrowIfFailed (infoQueue->PushStorageFilter (&newFilter));
	}
#endif

	return d3d12Device;
}

WRL::ComPtr<ID3D12CommandQueue> DX12RenderFactory::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
{
	ComPtr<ID3D12CommandQueue> commandQueue;

	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	ThrowIfFailed (device->CreateCommandQueue (&desc, IID_PPV_ARGS (&commandQueue)));

	return commandQueue;
}

WRL::ComPtr<IDXGISwapChain4> DX12RenderFactory::CreateSwapChain(HWND hWnd, WRL::ComPtr<ID3D12CommandQueue> commandQueue,
	uint32_t width, uint32_t height, uint32_t bufferCount)
{
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<IDXGIFactory4> factory;
	UINT createFactoryFlags = 0;
#ifdef _DEBUG
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed (CreateDXGIFactory2 (createFactoryFlags, IID_PPV_ARGS (&factory)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = CheckTearingSupport () ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed (factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1));

	ThrowIfFailed (factory->MakeWindowAssociation (hWnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed (swapChain1.As(&swapChain));

	return swapChain;
}

WRL::ComPtr<ID3D12DescriptorHeap> DX12RenderFactory::CreateDescriptorHeap(WRL::ComPtr<ID3D12Device2> device,
	D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	ThrowIfFailed (device->CreateDescriptorHeap (&desc, IID_PPV_ARGS (&descriptorHeap)));

	return descriptorHeap;
}

void DX12RenderFactory::UpdateRenderTargetViews(ComPtr<ID3D12Device2> device,
	ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap)
{
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize (D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	/*CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle (descriptorHeap->GetCPUDescriptorHandleForHeapStart ());

	for (int i = 0; i < numFrames; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed (swapChain->GetBuffer (i, IID_PPV_ARGS (&backBuffer)));

		device->CreateRenderTargetView (backBuffer.Get (), nullptr, rtvHandle);

		g_BackBuffers[i] = backBuffer;

		rtvHandle.Offset (rtvDescriptorSize);
	}*/
}
