#include "Fade.h"
#include "imgui.h"
#include "ScreenCapture.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

namespace Fade {
    ScreenCapture fadeFrame;

    double GetDouble(RE::GFxValue parent, const char* name) {
        RE::GFxValue result;

        if (!parent.GetMember(name, &result)) {
            return 0;
        }

        return result.GetNumber();
    }

    void SetDouble(RE::GFxValue parent, const char* name, double value) {
        parent.SetMember(name, RE::GFxValue(value)); 
    }

    void MofiyFadeMenu() {
        auto ui = RE::UI::GetSingleton();
        auto faderMenu = ui->GetMenu<RE::FaderMenu>();

        RE::GFxValue _root;
        RE::GFxValue mc_FaderMenu;

        if (!faderMenu->uiMovie->GetVariable(&_root, "_root")) {
            return;
        }

        if (!_root.GetMember("mc_FaderMenu", &mc_FaderMenu)) {
            return;
        }

        SetDouble(mc_FaderMenu, "fFadeDuration", 0.5);
        SetDouble(mc_FaderMenu, "fMinNumSeconds", 0);
        SetDouble(_root, "_alpha", 0);
    }

    void RenderFade() {
        auto fade = GetFadeAmmount();

        if (fade <= 0) {
            return;
        }

        MofiyFadeMenu();

        auto image = GetFadeFrame();
        if (!image) {
            return;
        }

        float g_deltaTime = *(float*)REL::RelocationID(523660, 410199).address();

        float screenW = ImGui::GetIO().DisplaySize.x;
        float screenH = ImGui::GetIO().DisplaySize.y;

        ID3D11Resource* res = nullptr;
        image->GetResource(&res);

        ID3D11Texture2D* tex = nullptr;
        res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex);

        D3D11_TEXTURE2D_DESC desc;
        tex->GetDesc(&desc);

        res->Release();
        tex->Release();

        float imageW = float(desc.Width);
        float imageH = float(desc.Height);

        float screenAspect = screenW / screenH;
        float imageAspect = imageW / imageH;

        float drawW;
        float drawH;

        if (imageAspect > screenAspect) {
            drawH = screenH;
            drawW = screenH * imageAspect;
        } else {
            drawW = screenW;
            drawH = screenW / imageAspect;
        }

        float x = (screenW - drawW) * 0.5f;
        float y = (screenH - drawH) * 0.5f;

        auto list = ImGui::GetForegroundDrawList();
        ImU32 color = IM_COL32(255, 255, 255, (int)(fade / 100 * 255));
        list->AddImage(image, ImVec2{x, y}, ImVec2{x + drawW, y + drawH}, ImVec2{0, 0}, ImVec2{1, 1}, color);
    }
}

void Fade::FetchFadeFrame() {
    auto fade = Fade::GetFadeAmmount();

    if (fade > 0) {
        return;
    }

    fadeFrame.FetchTextureBuffer();
}

ID3D11ShaderResourceView* Fade::GetFadeFrame() { return fadeFrame.GetTextureBuffer(); }

float Fade::GetFadeAmmount() {
    auto ui = RE::UI::GetSingleton();

    if (!ui->IsMenuOpen(RE::FaderMenu::MENU_NAME)) {
        return 0;
    }

    auto faderMenu = ui->GetMenu<RE::FaderMenu>();

    RE::GFxValue _root;
    RE::GFxValue mc_FaderMenu;
    RE::GFxValue mc_FadeRect;

    if (!faderMenu->uiMovie->GetVariable(&_root, "_root")) {
        return 0;
    }

    if (!_root.GetMember("mc_FaderMenu", &mc_FaderMenu)) {
        return 0;
    }

    if (!mc_FaderMenu.GetMember("mc_FadeRect", &mc_FadeRect)) {
        return 0;
    }

    if (GetDouble(mc_FaderMenu, "fFadeDuration") == 0) {
        return 0;
    }

    return GetDouble(mc_FadeRect, "_alpha");
}

void Fade::Render() {
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
    RenderFade();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}