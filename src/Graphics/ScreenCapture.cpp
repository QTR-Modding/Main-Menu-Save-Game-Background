#include "ScreenCapture.h"
#include "dxgi.h"
#include <d3d11.h>

void ScreenCapture::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    ScreenCapture::swapChain = swapChain;
    ScreenCapture::device = device;
    ScreenCapture::context = context;
}

void ScreenCapture::FetchTextureBuffer() {
    if (!swapChain || !device || !context) {
        return;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
        return;
    }

    D3D11_TEXTURE2D_DESC desc;

    backBuffer->GetDesc(&desc);

    if (!cachedTexture || cachedDesc.Width != desc.Width || cachedDesc.Height != desc.Height || cachedDesc.Format != desc.Format) {
        if (cachedSRV) cachedSRV->Release();
        if (cachedTexture) cachedTexture->Release();

        cachedDesc = desc;
        cachedDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        cachedDesc.Usage = D3D11_USAGE_DEFAULT;
        cachedDesc.CPUAccessFlags = 0;
        cachedDesc.MiscFlags = 0;

        if (FAILED(device->CreateTexture2D(&cachedDesc, nullptr, &cachedTexture))) {
            backBuffer->Release();
            return;
        }

        if (FAILED(device->CreateShaderResourceView(cachedTexture, nullptr, &cachedSRV))) {
            cachedTexture->Release();
            cachedTexture = nullptr;
            backBuffer->Release();
            return;
        }
    }

    context->CopyResource(cachedTexture, backBuffer);
    backBuffer->Release();
}

ID3D11ShaderResourceView* ScreenCapture::GetTextureBuffer() { return cachedSRV; }
