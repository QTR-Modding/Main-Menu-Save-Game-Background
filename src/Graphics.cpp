#include "Graphics.h"
#include "Configuration.h"
#include <DirectXTex.h>
#include <shared_mutex>
#include <d3dcompiler.h>
std::shared_mutex mutex;
using Microsoft::WRL::ComPtr;

void Graphics::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    Graphics::swapChain = swapChain;
    Graphics::device = device;
    Graphics::context = context;
    InitShaders();

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

bool Graphics::InitShaders() {
    Microsoft::WRL::ComPtr<ID3DBlob> csBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(L".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &csBlob, &errorBlob);

    if (FAILED(hr)) return false;

    hr = device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &gaussianBlurCS);

    return SUCCEEDED(hr);
}

bool Graphics::PostProcessTexture(ID3D11Texture2D* inputTex, ID3D11Texture2D** outputTex) {
    if (!inputTex || !outputTex || !gaussianBlurCS) return false;

    D3D11_TEXTURE2D_DESC inputDesc;
    inputTex->GetDesc(&inputDesc);

    // Calculate 16:9 output dimensions that fully contain the input
    UINT outputWidth, outputHeight;
    float inputAspect = (float)inputDesc.Width / (float)inputDesc.Height;
    float targetAspect = 16.0f / 9.0f;

    if (inputAspect > targetAspect) {
        // Input is wider than 16:9, match input width
        outputWidth = inputDesc.Width;
        outputHeight = (UINT)((float)inputDesc.Width / targetAspect + 0.5f);
    } else {
        // Input is taller/equal to 16:9, match input height
        outputHeight = inputDesc.Height;
        outputWidth = (UINT)((float)inputDesc.Height * targetAspect + 0.5f);
    }

    // Create 16:9 output texture
    D3D11_TEXTURE2D_DESC outputDesc = inputDesc;
    outputDesc.Width = outputWidth;
    outputDesc.Height = outputHeight;
    outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    outputDesc.CPUAccessFlags = 0;
    outputDesc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> blurTex;
    if (FAILED(device->CreateTexture2D(&outputDesc, nullptr, &blurTex))) return false;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvInput;
    if (FAILED(device->CreateShaderResourceView(inputTex, nullptr, &srvInput))) return false;

    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uavOutput;
    if (FAILED(device->CreateUnorderedAccessView(blurTex.Get(), nullptr, &uavOutput))) return false;

    struct BlurCB {
        DirectX::XMFLOAT2 texelSize;
        float radius;
        float brightness;

        float saturation;
        float contrast;
        float rMultiply;
        float gMultiply;

        float bMultiply;
        UINT inputWidth;
        UINT inputHeight;
        UINT outputWidth;

        UINT outputHeight;
        INT offsetX;
        INT offsetY;
        float padding;
    } cbData;

    cbData.texelSize = DirectX::XMFLOAT2(1.0f / inputDesc.Width, 1.0f / inputDesc.Height);
    cbData.radius = Configuration::BlurRadius;
    cbData.brightness = Configuration::Brightness;
    cbData.saturation = Configuration::Saturation;
    cbData.contrast = Configuration::Contrast;
    cbData.rMultiply = Configuration::RMultiply;
    cbData.gMultiply = Configuration::GMultiply;
    cbData.bMultiply = Configuration::BMultiply;
    cbData.inputWidth = inputDesc.Width;
    cbData.inputHeight = inputDesc.Height;
    cbData.outputWidth = outputWidth;
    cbData.outputHeight = outputHeight;
    cbData.offsetX = (INT)((outputWidth - inputDesc.Width) / 2);
    cbData.offsetY = (INT)((outputHeight - inputDesc.Height) / 2);
    cbData.padding = 0.0f;

    Microsoft::WRL::ComPtr<ID3D11Buffer> cb;
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(BlurCB);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &cbData;
    if (FAILED(device->CreateBuffer(&bd, &initData, &cb))) return false;

    context->CSSetShader(gaussianBlurCS.Get(), nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = {srvInput.Get()};
    context->CSSetShaderResources(0, 1, srvs);
    ID3D11UnorderedAccessView* uavs[] = {uavOutput.Get()};
    context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);
    ID3D11Buffer* cbs[] = {cb.Get()};
    context->CSSetConstantBuffers(0, 1, cbs);

    context->Dispatch((outputWidth + 15) / 16, (outputHeight + 15) / 16, 1);

    ID3D11ShaderResourceView* nullSRV[] = {nullptr};
    ID3D11UnorderedAccessView* nullUAV[] = {nullptr};
    ID3D11Buffer* nullCB[] = {nullptr};
    context->CSSetShader(nullptr, nullptr, 0);
    context->CSSetShaderResources(0, 1, nullSRV);
    context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
    context->CSSetConstantBuffers(0, 1, nullCB);

    *outputTex = blurTex.Detach();
    return true;
}

void Graphics::UpdatePostProcessedFrame() {
    std::shared_lock lock(mutex);
    if (!oldFrame) return;

    ID3D11Resource* resource = nullptr;
    oldFrame->GetResource(&resource);
    if (!resource) return;

    ID3D11Texture2D* tex2D = nullptr;
    HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex2D));
    resource->Release();
    if (FAILED(hr)) return;

    ID3D11Texture2D* finalTex = tex2D;

    ID3D11Texture2D* postProcessedTex = nullptr;
    if (PostProcessTexture(tex2D, &postProcessedTex)) {
        finalTex = postProcessedTex;
    }

    if (oldPostProcessedFrame != nullptr) {
        oldPostProcessedFrame->Release();
    }
    oldPostProcessedFrame = nullptr;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(finalTex, &srvDesc, &oldPostProcessedFrame);

    if (finalTex != tex2D) finalTex->Release();
    tex2D->Release();

    if (FAILED(hr)) return;
}
ID3D11ShaderResourceView* Graphics::GetPostProcessedFrame() { 
    return oldPostProcessedFrame;

}
bool Graphics::SaveCurrentFrameAsDDS(const wchar_t* filename) {

    if (!oldFrame) return false;

    std::filesystem::path path(filename);
    if (path.has_parent_path()) std::filesystem::create_directories(path.parent_path());

    std::shared_lock lock(mutex);

    ID3D11Resource* resource = nullptr;
    oldFrame->GetResource(&resource);
    auto tex2D = static_cast<ID3D11Texture2D*>(resource);

    ID3D11Texture2D* finalTex = tex2D;
    ID3D11Texture2D* postProcessedTex = nullptr;
    if (PostProcessTexture(tex2D, &postProcessedTex)) {
        finalTex = postProcessedTex;
    }

    DirectX::ScratchImage image;
    HRESULT captureResult = DirectX::CaptureTexture(device, context, finalTex, image);

    bool result = false;
    if (SUCCEEDED(captureResult)) {
        result = SUCCEEDED(DirectX::SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, filename));
    }

    if (finalTex != tex2D) finalTex->Release();
    resource->Release();
    return result;
}
