#include "Graphics/DX12/DX12RenderFactory.h"
#include <d3d12.h>
using namespace Microsoft::WRL;

DX12RenderFactory::~DX12RenderFactory()
{
}



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

ComPtr<ID3D12Device2> DX12RenderFactory::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
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
