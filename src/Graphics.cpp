#include "Graphics.h"
#include <d3d11.h>
#include <DirectXTex.h>
#include <wincodec.h>
#include <shared_mutex>
std::shared_mutex mutex;
void Graphics::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    Graphics::swapChain = swapChain;
    Graphics::device = device;
    Graphics::context = context;

}
void Graphics::FetchTextureBuffer() {
    std::unique_lock lock(mutex);
    if (!swapChain || !device || !context) {
        return;
    }

    if (oldFrame) {
        oldFrame->Release();
        oldFrame = nullptr;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr) || !backBuffer) {
        return;
    }

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Texture2D* textureCopy = nullptr;
    hr = device->CreateTexture2D(&desc, nullptr, &textureCopy);
    if (FAILED(hr) || !textureCopy) {
        backBuffer->Release();
        return;
    }

    context->CopyResource(textureCopy, backBuffer);

    hr = device->CreateShaderResourceView(textureCopy, nullptr, &oldFrame);

    backBuffer->Release();
    textureCopy->Release();

    if (FAILED(hr)) {
        return;
    }
}
bool Graphics::SaveCurrentFrameAsDDS(const wchar_t* filename) {
    if (!oldFrame) {
        return false;
    }
    std::filesystem::path path(filename);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::shared_lock lock(mutex);
    auto srv = oldFrame;
    ID3D11Resource* resource = nullptr;
    srv->GetResource(&resource);

    DirectX::ScratchImage image;
    if (FAILED(DirectX::CaptureTexture(device, context, resource, image))) {
        resource->Release();
        return false;
    }

    bool result = SUCCEEDED(DirectX::SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, filename));

    resource->Release();
    return result;
}