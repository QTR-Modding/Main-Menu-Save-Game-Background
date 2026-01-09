#include "TextureSaver.h"
#include <DirectXTex.h>

namespace TextureSaver {
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
}

void TextureSaver::Install(IDXGISwapChain* swapChain, ID3D11Device* device, ID3D11DeviceContext* context) {
    TextureSaver::swapChain = swapChain;
    TextureSaver::device = device;
    TextureSaver::context = context;
}

void TextureSaver::SaveResourceAsDDS(ID3D11ShaderResourceView* source, std::vector<std::wstring> filenames) {

    if (!source) return;

    ID3D11Resource* resource = nullptr;
    source->GetResource(&resource);
    auto tex2D = static_cast<ID3D11Texture2D*>(resource);

    DirectX::ScratchImage image;
    HRESULT captureResult = DirectX::CaptureTexture(device, context, tex2D, image);

    if (!SUCCEEDED(captureResult)) {
        return;
    }

    for (auto filename : filenames) {

        std::filesystem::path path(filename);

        if (!path.has_parent_path()) {
            continue;
        }

        std::filesystem::create_directories(path.parent_path());
        
        SUCCEEDED(DirectX::SaveToDDSFile(*image.GetImage(0, 0, 0), DirectX::DDS_FLAGS_NONE, filename.c_str()));
    }

    resource->Release();
}
