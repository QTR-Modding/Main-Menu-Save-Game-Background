#pragma once
#include "dxgi.h"
#include <d3d11.h>
#include <wrl/client.h>

class Graphics {
    static inline IDXGISwapChain* swapChain = nullptr;
    static inline ID3D11Device* device = nullptr;
    static inline ID3D11DeviceContext* context = nullptr;

    static inline ID3D11ShaderResourceView* oldFrame = nullptr;
    static inline ID3D11ShaderResourceView* oldPostProcessedFrame = nullptr;
    static bool ApplyGaussianBlur(ID3D11Texture2D* inputTex, ID3D11Texture2D** outputTex);
public:
    static void Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context);
    static void FetchTextureBuffer();
    static bool SaveCurrentFrameAsDDS(const wchar_t* filename);
    static void UpdatePostProcessedFrame();
    static ID3D11ShaderResourceView* GetPostProcessedFrame();
};