#pragma once

class ScreenCapture {
    ID3D11Texture2D* cachedTexture = nullptr;
    ID3D11ShaderResourceView* cachedSRV = nullptr;
    D3D11_TEXTURE2D_DESC cachedDesc{};

    static inline IDXGISwapChain* swapChain = nullptr;
    static inline ID3D11Device* device = nullptr;
    static inline ID3D11DeviceContext* context = nullptr;

public:
    static void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    void FetchTextureBuffer();
    void Clear();
    ID3D11ShaderResourceView* GetTextureBuffer();
};