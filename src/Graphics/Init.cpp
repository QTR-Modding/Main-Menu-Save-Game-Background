#include "Init.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "TextureSaver.h"
#include "ScreenCapture.h"
#include "PostProcessor.h"
#include "Renderer.h"

void Init::Main() {
    const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
        return;
    }

    auto data = renderer->GetRuntimeData();

    auto swapChain = reinterpret_cast<IDXGISwapChain*>(data.renderWindows[0].swapChain);
    if (!swapChain) {
        logger::error("couldn't find swapChain");
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
        logger::error("IDXGISwapChain::GetDesc failed.");
        return;
    }

    auto device = reinterpret_cast<ID3D11Device*>(data.forwarder);
    auto context = reinterpret_cast<ID3D11DeviceContext*>(data.context);

    TextureSaver::Install(swapChain, device, context);
    ScreenCapture::Install(swapChain, device, context);
    Renderer::Install(swapChain, device, context);
    PostProcessor::Install(swapChain, device, context);
    SKSE::log::info("Initializing ImGui...");

    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
        SKSE::log::error("ImGui initialization failed (Win32)");
        return;
    }

    if (!ImGui_ImplDX11_Init(device, context)) {
        SKSE::log::error("ImGui initialization failed (DX11)");
        return;
    }

    SKSE::log::info("ImGui initialized.");

    io.Fonts->Build();
}