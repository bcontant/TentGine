#include "temp.h"
#include <D3D11.h>

std::vector<DisplayAdapter*> GetDisplayAdapters()
{
	std::vector<DisplayAdapter*> vAdapters;

	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vDXAdapters;
	IDXGIFactory* pFactory = NULL;

	HResult hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

	for (int i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		IDXGIOutput * pOutput;
		std::vector<IDXGIOutput*> vOutputs;
		for (int j = 0; pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND; j++)
		{
			DXGI_OUTPUT_DESC outputDesc;
			pOutput->GetDesc(&outputDesc);

			UINT modeCount;
			pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modeCount, NULL);

			DXGI_MODE_DESC* modeDesc = new DXGI_MODE_DESC[modeCount];
			pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modeCount, modeDesc);

			vOutputs.push_back(pOutput);
			++j;
		}

		vDXAdapters.push_back(pAdapter);
	}

	if (pFactory)
		pFactory->Release();

	return vAdapters;
}





