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
const char* gaussianBlurCS_H = R"(
cbuffer BlurCB : register(b0)
{
    float2 texelSize;
    float radius;
    float brightness;
    float saturation;
    float contrast;
    float rMultiply;
    float gMultiply;
    float bMultiply;
};

Texture2D inputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16,16,1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 sum = float4(0,0,0,0);
    float sigma = radius / 2.0f;
    float twoSigmaSq = 2.0f * sigma * sigma;
    float piSigma = 3.14159265 * twoSigmaSq;
    float weightSum = 0.0f;

    int r = int(radius);

    for(int y=-r; y<=r; y++)
    {
        for(int x=-r; x<=r; x++)
        {
            float2 offset = float2(x,y) * texelSize;
            float weight = exp(-(x*x + y*y)/twoSigmaSq) / piSigma;
            sum += inputTex.Load(int3(DTid.xy + int2(x,y),0)) * weight;
            weightSum += weight;
        }
    }

    float4 blurred = sum / weightSum;

    blurred.rgb *= brightness;

    blurred.rgb = ((blurred.rgb - 0.5f) * contrast) + 0.5f;

    float lum = dot(blurred.rgb, float3(0.299, 0.587, 0.114));
    blurred.rgb = lerp(float3(lum, lum, lum), blurred.rgb, saturation);

    blurred.r *= rMultiply;
    blurred.g *= gMultiply;
    blurred.b *= bMultiply;

    outputTex[DTid.xy] = blurred;
}


)";

bool Graphics::ApplyGaussianBlur(ID3D11Texture2D* inputTex, ID3D11Texture2D** outputTex) {
    D3D11_TEXTURE2D_DESC desc;
    inputTex->GetDesc(&desc);

    D3D11_TEXTURE2D_DESC blurDesc = desc;
    blurDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    blurDesc.CPUAccessFlags = 0;
    blurDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> blurTex;
    device->CreateTexture2D(&blurDesc, nullptr, &blurTex);

    ComPtr<ID3D11ShaderResourceView> srvInput;
    device->CreateShaderResourceView(inputTex, nullptr, &srvInput);

    ComPtr<ID3D11UnorderedAccessView> uavOutput;
    device->CreateUnorderedAccessView(blurTex.Get(), nullptr, &uavOutput);

    ComPtr<ID3D11ComputeShader> blurCS;
    ComPtr<ID3DBlob> csBlob;
    D3DCompile(gaussianBlurCS_H, strlen(gaussianBlurCS_H), nullptr, nullptr, nullptr, "main", "cs_5_0", 0, 0, &csBlob, nullptr);
    device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &blurCS);

    context->CSSetShader(blurCS.Get(), nullptr, 0);
    ID3D11ShaderResourceView* srvs[] = {srvInput.Get()};
    context->CSSetShaderResources(0, 1, srvs);
    ID3D11UnorderedAccessView* uavs[] = {uavOutput.Get()};
    context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

    struct BlurCB {
        DirectX::XMFLOAT2 texelSize;
        float radius;
        float brightness;
        float saturation;
        float contrast;
        float rMultiply;
        float gMultiply;
        float bMultiply;
        float padding[3];
    } cbData;
    cbData.texelSize = DirectX::XMFLOAT2(1.0f / desc.Width, 1.0f / desc.Height);
    cbData.radius = Configuration::BlurRadius;
    cbData.brightness = Configuration::Brightness;
    cbData.saturation = Configuration::Saturation;
    cbData.contrast = Configuration::Contrast;
    cbData.rMultiply = Configuration::RMultiply;
    cbData.gMultiply = Configuration::BMultiply;
    cbData.bMultiply= Configuration::GMultiply;

    ComPtr<ID3D11Buffer> cb;
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(cbData);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {&cbData, 0, 0};
    device->CreateBuffer(&bd, &initData, &cb);

    context->CSSetConstantBuffers(0, 1, cb.GetAddressOf());
    context->Dispatch((desc.Width + 15) / 16, (desc.Height + 15) / 16, 1);

    context->CSSetShader(nullptr, nullptr, 0);
    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    context->CSSetShaderResources(0, 1, nullSRV);
    ID3D11UnorderedAccessView* nullUAV[1] = {nullptr};
    context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

    *outputTex = blurTex.Detach();
    return true;
}
ID3D11ShaderResourceView* Graphics::GetCurrentFrameSRV() {
    std::shared_lock lock(mutex);
    if (!oldFrame) return nullptr;

    ID3D11Resource* resource = nullptr;
    oldFrame->GetResource(&resource);
    if (!resource) return nullptr;

    ID3D11Texture2D* tex2D = nullptr;
    HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex2D));
    resource->Release();
    if (FAILED(hr)) return nullptr;

    ID3D11Texture2D* finalTex = tex2D;
    if (Configuration::PostProcess) {
        ID3D11Texture2D* blurredTex = nullptr;
        if (ApplyGaussianBlur(tex2D, &blurredTex)) {
            finalTex = blurredTex;
        }
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

    if (Configuration::PostProcess && finalTex != tex2D) finalTex->Release();
    tex2D->Release();

    if (FAILED(hr)) return nullptr;
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
    if (Configuration::PostProcess) {
        ID3D11Texture2D* blurredTex = nullptr;
        if (ApplyGaussianBlur(tex2D, &blurredTex)) {
            finalTex = blurredTex;
        }
    }

    DirectX::ScratchImage image;
    DirectX::CaptureTexture(device, context, finalTex, image);

    bool result = SUCCEEDED(DirectX::SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, filename));

    if (Configuration::PostProcess && finalTex != tex2D) finalTex->Release();
    resource->Release();
    return result;
}
