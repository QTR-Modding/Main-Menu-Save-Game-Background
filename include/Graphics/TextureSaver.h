#pragma once
#include "dxgi.h"
#include <d3d11.h>
#include <wrl/client.h>

namespace TextureSaver {
    void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    void SaveResourceAsDDS(ID3D11ShaderResourceView* source, std::vector<std::wstring> filenames);
};