#include "Renderer.h"
#include <d3dcompiler.h>

namespace Renderer {
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11BlendState* blendState = nullptr;
    ID3D11SamplerState* samplerState = nullptr;

    void InitShaders();

    struct Vertex {
        float x, y, z;
        float u, v;
    };

    const char* vertexShaderSource = R"(
    struct VS_INPUT {
        float3 pos : POSITION;
        float2 uv : TEXCOORD0;
    };

    struct VS_OUTPUT {
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD0;
    };

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.pos = float4(input.pos, 1.0f);
        output.uv = input.uv;
        return output;
    }
    )";

    const char* pixelShaderSource = R"(
    Texture2D tex : register(t0);
    SamplerState samp : register(s0);

    struct PS_INPUT {
        float4 pos : SV_POSITION;
        float2 uv : TEXCOORD0;
    };

    float4 main(PS_INPUT input) : SV_TARGET {
        return tex.Sample(samp, input.uv);
    }
    )";
}

void Renderer::InitShaders() {
    HRESULT hr;

    float depth = 1.f;
    Vertex vertices[] = {
        {-1.0f, 1.0f, depth, 0.0f, 0.0f}, {1.0f, 1.0f, depth, 1.0f, 0.0f}, {-1.0f, -1.0f, depth, 0.0f, 1.0f}, {1.0f, 1.0f, depth, 1.0f, 0.0f}, {1.0f, -1.0f, depth, 1.0f, 1.0f}, {-1.0f, -1.0f, depth, 0.0f, 1.0f},
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    hr = device->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr)) return;

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    hr = D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        if (errorBlob) errorBlob->Release();
        return;
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        vsBlob->Release();
        return;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr)) return;

    ID3DBlob* psBlob = nullptr;
    hr = D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        if (errorBlob) errorBlob->Release();
        return;
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr)) return;

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = device->CreateBlendState(&blendDesc, &blendState);
    if (FAILED(hr)) return;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampDesc, &samplerState);
    if (FAILED(hr)) return;

    return;
}

void Renderer::Render(ID3D11ShaderResourceView* texture) {
    ID3D11RasterizerState* oldRS = nullptr;
    ID3D11DepthStencilState* oldDS = nullptr;
    UINT oldStencilRef = 0;
    ID3D11BlendState* oldBS = nullptr;
    FLOAT oldBlendFactor[4];
    UINT oldSampleMask = 0;
    ID3D11Buffer* oldVB = nullptr;
    UINT oldStride = 0;
    UINT oldOffset = 0;
    ID3D11VertexShader* oldVS = nullptr;
    ID3D11PixelShader* oldPS = nullptr;
    ID3D11InputLayout* oldIL = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY oldTopology;
    ID3D11ShaderResourceView* oldSRV[1] = {nullptr};
    ID3D11SamplerState* oldSampler[1] = {nullptr};

    context->RSGetState(&oldRS);
    context->OMGetDepthStencilState(&oldDS, &oldStencilRef);
    context->OMGetBlendState(&oldBS, oldBlendFactor, &oldSampleMask);
    context->IAGetVertexBuffers(0, 1, &oldVB, &oldStride, &oldOffset);
    context->VSGetShader(&oldVS, nullptr, nullptr);
    context->PSGetShader(&oldPS, nullptr, nullptr);
    context->IAGetInputLayout(&oldIL);
    context->IAGetPrimitiveTopology(&oldTopology);
    context->PSGetShaderResources(0, 1, oldSRV);
    context->PSGetSamplers(0, 1, oldSampler);

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.DepthClipEnable = TRUE;

    ID3D11RasterizerState* rs = nullptr;
    device->CreateRasterizerState(&rsDesc, &rs);
    context->RSSetState(rs);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = FALSE;

    ID3D11DepthStencilState* ds = nullptr;
    device->CreateDepthStencilState(&dsDesc, &ds);
    context->OMSetDepthStencilState(ds, oldStencilRef);

    float blendFactor[4] = {1, 1, 1, 1};
    context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
    context->IASetInputLayout(inputLayout);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->PSSetShaderResources(0, 1, &texture);
    context->PSSetSamplers(0, 1, &samplerState);

    context->Draw(6, 0);

    ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
    context->PSSetShaderResources(0, 1, nullSRV);

    context->RSSetState(oldRS);
    context->OMSetDepthStencilState(oldDS, oldStencilRef);
    context->OMSetBlendState(oldBS, oldBlendFactor, oldSampleMask);
    context->IASetVertexBuffers(0, 1, &oldVB, &oldStride, &oldOffset);
    context->VSSetShader(oldVS, nullptr, 0);
    context->PSSetShader(oldPS, nullptr, 0);
    context->IASetInputLayout(oldIL);
    context->IASetPrimitiveTopology(oldTopology);
    context->PSSetShaderResources(0, 1, oldSRV);
    context->PSSetSamplers(0, 1, oldSampler);

    if (rs) rs->Release();
    if (ds) ds->Release();
}

void Renderer::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    Renderer::swapChain = swapChain;
    Renderer::device = device;
    Renderer::context = context;
    InitShaders();
}