#include "Hooks.h"

#include <chrono>
#include <thread>
#include <iostream>
#include "Graphics.h"
#include "MainMenuManager.h"
#include "Configuration.h"
#include "SaveGame.h"
#include "File.h"
#include "Menu.h"
#include "UI.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#define QUICK_SAVE_DEVICE_ID 4
#define AUTO_SAVE_DEVICE_ID 3
#define MANUAL_SAVE_DEVICE_ID 2

#define LAST_SAVE_FILE_PATH ".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackgroundLastSave.txt"


bool isLoadingLastSave = false;

int32_t Hooks::LoadGameHook::thunk(RE::BSWin32SaveDataSystemUtility* util, char* fileName, void* unknown) {

    std::string contents;
    std::string loweredFileName = SaveGame::LowerTrimESS(fileName);
    if (File::ReadString(LAST_SAVE_FILE_PATH, contents) && contents == loweredFileName) {
        isLoadingLastSave = true;
    } else {
        isLoadingLastSave = false;
    }

    return originalFunction(util, fileName, unknown);
}

void Hooks::LoadGameHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(34677, 35600).address() + REL::Relocate(0xab, 0xab), thunk);
}


char* Hooks::SaveGameHook::thunk(RE::BGSSaveLoadManager* manager, void* a2, char* fileName, void* a4, int32_t deviceId) {

    if (deviceId == QUICK_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnQuickSave) {
            MainMenuManager::OnSaveGame();
            File::WriteString(LAST_SAVE_FILE_PATH, SaveGame::LowerTrimESS(fileName));
            SKSEMenuFramework::DisposeTexture(TEXTURE_PATH);
        }
    } else if (deviceId == AUTO_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnAutoSave) {
            MainMenuManager::OnSaveGame();
            File::WriteString(LAST_SAVE_FILE_PATH, SaveGame::LowerTrimESS(fileName));
            SKSEMenuFramework::DisposeTexture(TEXTURE_PATH);
        }
    } else if (deviceId == MANUAL_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnManualSave) {
            MainMenuManager::OnSaveGame();
            File::WriteString(LAST_SAVE_FILE_PATH, SaveGame::LowerTrimESS(fileName));
            SKSEMenuFramework::DisposeTexture(TEXTURE_PATH);
        }
    } else {
        if (Configuration::UpdateOnOtherSave) {
            MainMenuManager::OnSaveGame();
            File::WriteString(LAST_SAVE_FILE_PATH, SaveGame::LowerTrimESS(fileName));
            SKSEMenuFramework::DisposeTexture(TEXTURE_PATH);
        }
    }
    
    return originalFunction(manager, a2, fileName, a4, deviceId);
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
    LoadGameHook::Install();
    CreateD3DAndSwapChain::Install();
    RenderUIHook::Install();
    LoadingScreenHook::Install();
    AddMessageLoadingMenuHide::Install();
    DXGIPresentHook::install();
}

void Hooks::LoadingScreenHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(51048, 51929).address() + REL::Relocate(0x271, 0x176), thunk);
}

int64_t Hooks::LoadingScreenHook::thunk(int64_t a1, uint32_t a2) {
    auto result = originalFunction(a1, a2);
    auto dataLoader = RE::TESDataHandler::GetSingleton();
    if (!dataLoader) return result;
    auto dynamicLoadingScreenId = dataLoader->LookupFormID(0x800, "MainMenuSaveGameBackground.esp");
    auto dynamicLoadingScreen = RE::TESForm::LookupByID<RE::TESLoadScreen>(dynamicLoadingScreenId);
    if (!dynamicLoadingScreen) return result;
    const auto ui = RE::UI::GetSingleton();
    if (!ui) return result;
    auto loadingMenu = ui->GetMenu<RE::LoadingMenu>();
    if (!loadingMenu) return result;
    auto data = loadingMenu->GetRuntimeData();
    auto list = data.loadScreens;
    for (auto i = 0; i < list.size(); i++) {
        if (auto item = list[i]) {
            if (item->GetFormID() == dynamicLoadingScreenId) {
                if (result == dynamicLoadingScreenId) {
                    result = 0;
                }
                if (Configuration::ReplaceLoadingScreenMesh && isLoadingLastSave) {
                    auto original = data.loadScreens[result];
                    if (original) {
                        dynamicLoadingScreen->loadingText = original->loadingText;
                    }
                    return i;
                }
            }

        }
    }
    return result;
}

void Hooks::AddMessageLoadingMenuHide::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(13214, 13363).address() + REL::Relocate(0x211, 0x211), thunk);
}

void Hooks::AddMessageLoadingMenuHide::thunk(RE::UIMessageQueue* queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data) { 
    originalFunction(queue, a_menuName, a_type, a_data);
    if (Configuration::ReplaceLoadingScreenMesh && isLoadingLastSave) {
        MainMenuManager::OverlayAlpha = 1.0;
        RE::UIMessageQueue::GetSingleton()->AddMessage(RE::FaderMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
    }
    isLoadingLastSave = false;
}

void Hooks::DXGIPresentHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(REL::RelocationID(75461, 77246, 75461).address() + REL::Relocate(0x9, 0x9, 0x15), thunk);
}

void Hooks::DXGIPresentHook::thunk(std::uint32_t a_timer) {
    originalFunction(a_timer);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        // trick imgui into rendering at game's real resolution (ie. if upscaled with Display Tweaks)
        static const auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();

        auto& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(screenSize.width);
        io.DisplaySize.y = static_cast<float>(screenSize.height);
    }
    ImGui::NewFrame();
    UI::Config::Hud();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}