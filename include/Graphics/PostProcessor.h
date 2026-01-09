#pragma once
#include "dxgi.h"
#include <d3d11.h>
#include <wrl/client.h>

namespace PostProcessor {
    void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    ID3D11ShaderResourceView* PostProcessResource(ID3D11ShaderResourceView* source);
};