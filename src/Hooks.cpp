#include "Hooks.h"

#include <chrono>
#include <thread>
#include <iostream>
#include "Graphics.h"
#include "MainMenuManager.h"
char* Hooks::SaveGameHook::thunk(RE::BGSSaveLoadManager* manager, void* a2, char* fileName, void* a4, int32_t a5) {
    MainMenuManager::OnSaveGame();
    return originalFunction(manager, a2, fileName, a4, a5);
}

void Hooks::SaveGameHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(34818, 35727).address() + REL::Relocate(0x112, 0x1ce), thunk);
}

void Hooks::CreateD3DAndSwapChain::thunk() {
    originalFunction();

    if (const auto renderer = RE::BSGraphics::Renderer::GetSingleton()) {
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

        Graphics::Install(swapChain, device, context);
    }
}

void Hooks::CreateD3DAndSwapChain::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    const REL::Relocation<std::uintptr_t> target{REL::RelocationID(75595, 77226)};  // BSGraphics::InitD3D
    originalFunction = trampoline.write_call<5>(target.address() + REL::Relocate(0x9, 0x275), CreateD3DAndSwapChain::thunk);
}

int64_t Hooks::RenderUIHook::thunk(int64_t gMenuManager) { 
    MainMenuManager::OnRenderUI();
    return originalFunction(gMenuManager);
}

void Hooks::RenderUIHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(35556, 36555).address() + REL::Relocate(0x3ab, 0x371), thunk);
}

void Hooks::Install() {
    SaveGameHook::Install();
    CreateD3DAndSwapChain::Install();
    RenderUIHook::Install();
}