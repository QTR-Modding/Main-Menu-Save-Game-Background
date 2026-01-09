#include "PostProcessor.h"
#include <d3dcompiler.h>
#include "Configuration.h"
#include "imgui.h"

namespace PostProcessor {
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> gaussianBlurCS;

    bool Init();

    bool PostProcessTexture(ID3D11Texture2D* inputTex, ID3D11Texture2D** outputTex);
}

void PostProcessor::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    PostProcessor::swapChain = swapChain;
    PostProcessor::device = device;
    PostProcessor::context = context;
    Init();
}

bool PostProcessor::Init() {
    Microsoft::WRL::ComPtr<ID3DBlob> csBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(L".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &csBlob, &errorBlob);

    if (FAILED(hr)) return false;

    hr = device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &gaussianBlurCS);

    return SUCCEEDED(hr);
}

bool PostProcessor::PostProcessTexture(ID3D11Texture2D* inputTex, ID3D11Texture2D** outputTex) {
    if (!inputTex || !outputTex || !gaussianBlurCS) return false;

    D3D11_TEXTURE2D_DESC inputDesc;
    inputTex->GetDesc(&inputDesc);

    D3D11_TEXTURE2D_DESC outputDesc = inputDesc;

    auto io = ImGui::GetIO();

    UINT outputWidth, outputHeight;
    float inputAspect = (float)inputDesc.Width / (float)inputDesc.Height;
    float targetAspect = io.DisplaySize.x / io.DisplaySize.y;

    if (inputAspect < targetAspect) {
        outputWidth = inputDesc.Width;
        outputHeight = (UINT)((float)inputDesc.Width / targetAspect + 0.5f);
    } else {
        outputHeight = inputDesc.Height;
        outputWidth = (UINT)((float)inputDesc.Height * targetAspect + 0.5f);
    }

    float offsetX = 0, offsetY = 0;

    float scaleX = (float)outputWidth / inputDesc.Width;
    float scaleY = (float)outputHeight / inputDesc.Height;

    if (scaleX > scaleY) {
        offsetY = (outputHeight / scaleX - inputDesc.Height) / 2.0f;
    } else {
        offsetX = (outputWidth / scaleY - inputDesc.Width) / 2.0f;
    }

    outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    outputDesc.CPUAccessFlags = 0;
    outputDesc.MiscFlags = 0;
    outputDesc.Width = outputWidth;
    outputDesc.Height = outputHeight;

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
        UINT inputWidth;
        UINT inputHeight;
        UINT outputWidth;
        UINT outputHeight;
        INT offsetX;
        INT offsetY;
        float vignetteStrength;
        float padding[3];
    } cbData;

    cbData.texelSize = DirectX::XMFLOAT2(1.0f / inputDesc.Width, 1.0f / inputDesc.Height);
    cbData.radius = Configuration::BlurRadius;
    cbData.brightness = Configuration::Brightness;
    cbData.saturation = Configuration::Saturation;
    cbData.contrast = Configuration::Contrast;
    cbData.inputWidth = inputDesc.Width;
    cbData.inputHeight = inputDesc.Height;
    cbData.outputWidth = outputWidth;
    cbData.outputHeight = outputHeight;
    cbData.offsetX = offsetX;
    cbData.offsetY = offsetY;
    cbData.vignetteStrength = Configuration::VignetteStrength;

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

ID3D11ShaderResourceView* PostProcessor::PostProcessResource(ID3D11ShaderResourceView* source) {
    ID3D11ShaderResourceView* target = nullptr;

    if (!source) return nullptr;

    ID3D11Resource* resource = nullptr;
    source->GetResource(&resource);
    if (!resource) return nullptr;

    ID3D11Texture2D* tex2D = nullptr;
    HRESULT hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tex2D));
    resource->Release();
    if (FAILED(hr)) return nullptr;

    ID3D11Texture2D* finalTex = tex2D;

    ID3D11Texture2D* postProcessedTex = nullptr;
    if (PostProcessTexture(tex2D, &postProcessedTex)) {
        finalTex = postProcessedTex;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(finalTex, &srvDesc, &target);

    if (finalTex != tex2D) finalTex->Release();
    tex2D->Release();

    if (FAILED(hr)) return nullptr;

    return target;
}