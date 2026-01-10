#include "Hooks.h"
#include "Background.h"
#include "Renderer.h"
#include "Fade.h"
#include "Init.h"

namespace Hooks {
    struct RenderMistMenu {
        static int64_t thunk(RE::NiCamera* a1, RE::BSShaderAccumulator* a2, int64_t a3) {
            auto result = originalFunction(a1, a2, a3);
            auto ui = RE::UI::GetSingleton();
            if (ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME)) {
                auto image = Background::GetBackgroundImage();
                Renderer::Render(image);
            }
            return result;
        }
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static inline void Install() {
            SKSE::AllocTrampoline(14);
            auto& trampoline = SKSE::GetTrampoline();
            originalFunction = trampoline.write_call<5>(REL::RelocationID(51855, 52727).address() + REL::Relocate(0x658, 0x659), thunk);
        }
    };

    struct SaveGameHook {
        static void Install() {
            SKSE::AllocTrampoline(14);
            auto& trampoline = SKSE::GetTrampoline();
            originalFunction = trampoline.write_call<5>(REL::RelocationID(34818, 35727).address() + REL::Relocate(0x112, 0x1ce), thunk);
        }
        static char* thunk(RE::BGSSaveLoadManager* manager, void* a2, char* fileName, void* a4, int32_t deviceId) {
            Background::OnSave(fileName, deviceId);
            return originalFunction(manager, a2, fileName, a4, deviceId);
        }
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct LoadGameHook {
        static errno_t thunk(RE::BSSaveDataSystemUtility* utility, const char* a_fileName, int64_t a3) {
            Background::OnLoad(a_fileName);
            return originalFunction(utility, a_fileName, a3);
        }
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static inline void Install() { originalFunction = REL::Relocation<std::uintptr_t>(RE::VTABLE_BSWin32SaveDataSystemUtility[0]).write_vfunc(0x11, thunk); }
    };

    struct CreateD3DAndSwapChain {
        static void thunk() {
            originalFunction();
            Init::Main();
        }
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install() {
            SKSE::AllocTrampoline(14);
            auto& trampoline = SKSE::GetTrampoline();
            const REL::Relocation<std::uintptr_t> target{REL::RelocationID(75595, 77226)};  // BSGraphics::InitD3D
            originalFunction = trampoline.write_call<5>(target.address() + REL::Relocate(0x9, 0x275), CreateD3DAndSwapChain::thunk);
        }
    };

    struct MainMenuProcessMessage {
        static RE::UI_MESSAGE_RESULTS thunk(RE::MainMenu* menu, RE::UIMessage& a_message) {
            if (a_message.type.get() == RE::UI_MESSAGE_TYPE::kShow) {
                Background::LoadLastSaveTexture();
            }
            return originalFunction(menu, a_message);
        }
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static inline void Install() { originalFunction = REL::Relocation<std::uintptr_t>(RE::VTABLE_MainMenu[0]).write_vfunc(0x4, thunk); }
    };

    struct RenderUIHook {

        static int64_t thunk1(int64_t gMenuManager) {
            Background::FetchGameFrame();
            auto result = originalFunction1(gMenuManager);
            Fade::Render();
            return result;
        }
        static int64_t thunk2(int64_t gMenuManager) {
            Fade::FetchFadeFrame();
            Background::FetchGameFrame();
            auto result = originalFunction2(gMenuManager);
            Fade::Render();
            return result;
        }
        static inline REL::Relocation<decltype(thunk1)> originalFunction1;
        static inline REL::Relocation<decltype(thunk2)> originalFunction2;
        static void Install() {
            auto& trampoline = SKSE::GetTrampoline();
            SKSE::AllocTrampoline(14);
            originalFunction1 = trampoline.write_call<5>(REL::RelocationID(35556, 36555).address() + REL::Relocate(0x3ab, 0x371), thunk1);
            SKSE::AllocTrampoline(14);
            originalFunction2 = trampoline.write_call<5>(REL::RelocationID(38085, 39039).address() + REL::Relocate(0x19a, 0x19a), thunk2);
        }
    };
}

void Hooks::Install() {
    RenderMistMenu::Install();

    SaveGameHook::Install();
    LoadGameHook::Install();

    CreateD3DAndSwapChain::Install();
    RenderUIHook::Install();

    MainMenuProcessMessage::Install();
}

