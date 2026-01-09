#include "UI.h"
#include "Configuration.h"
#include "Translations.h"
#include "Background.h"
#include "Fade.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem(Translations::Get("MCP.Config"), Config::Render);
    SKSEMenuFramework::AddSectionItem(Translations::Get("MCP.PostProcess"), Config::PostProcess);
    #ifndef NDEBUG
        SKSEMenuFramework::AddSectionItem("Debug", Config::Debug);
    #endif 
}

void __stdcall UI::Config::Render() {
    ImGuiMCP::Text(Translations::Get("MCP.UpdateDescription"));

    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.UpdateOnAutoSave"), &Configuration::UpdateOnAutoSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.UpdateOnQuickSave"), &Configuration::UpdateOnQuickSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.UpdateOnManualSave"), &Configuration::UpdateOnManualSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.UpdateOnOtherSave"), &Configuration::UpdateOnOtherSave)) {
        Configuration::Save();
    }
    ImGuiMCP::Text(Translations::Get("MCP.LoadingScreen"));
    if (ImGuiMCPComponents::ToggleButton(Translations::Get("MCP.ReplaceLoadingScreenMesh"), &Configuration::ReplaceLoadingScreenMesh)) {
        Configuration::Save();
    }
}

void AddImage(ID3D11ShaderResourceView* texture, float width) {
    if (texture) {
        D3D11_TEXTURE2D_DESC desc;
        ID3D11Resource* resource = nullptr;
        texture->GetResource(&resource);
        auto tex2D = static_cast<ID3D11Texture2D*>(resource);
        tex2D->GetDesc(&desc);

        float aspect = static_cast<float>(desc.Height) / desc.Width;
        float screenWidth = ImGuiMCP::GetIO()->DisplaySize.x;
        float height = width * aspect;

        ImGuiMCP::Image(texture, ImGuiMCP::ImVec2(width, height));
        resource->Release();
    }
}

void __stdcall UI::Config::PostProcess() {
    ImGuiMCP::Text(Translations::Get("MCP.OutputImageEffects"));

    {
        if (ImGuiMCP::Button(std::format("{}##BlurRadiusClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::BlurRadius = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##BlurRadiusReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::BlurRadius = 12.5f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.BlurRadius"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##BlurRadius", &Configuration::BlurRadius)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##VignetteStrengthClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::VignetteStrength = 0.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##VignetteStrengthReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::VignetteStrength = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.VignetteStrength"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##VignetteStrength", &Configuration::VignetteStrength)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##BrightnessClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::Brightness = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##BrightnessReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::Brightness = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.Brightness"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##Brightness", &Configuration::Brightness)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##ContrastClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::Contrast = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##ContrastReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::Contrast = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.Contrast"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##Contrast", &Configuration::Contrast)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##SaturationClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::Saturation = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##SaturationReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::Saturation = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.Saturation"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##Saturation", &Configuration::Saturation)) {
            Configuration::Save();
        }
    }

    if (ImGuiMCP::Button("Update")) {
        Background::Apply();
    }


    auto screenWidth = ImGuiMCP::GetIO()->DisplaySize.x;
    AddImage(Background::GetBackgroundImage(), screenWidth / 2);

}

void __stdcall UI::Config::Debug() {
    AddImage(Fade::GetFadeFrame(), 640);
}
