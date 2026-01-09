#pragma once

namespace Renderer {
    void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    void Render(ID3D11ShaderResourceView* texture);
};