#include "UI.h"
#include "Configuration.h"
#include "Translations.h"
#include "Graphics.h"
void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Config", Config::Render);
    SKSEMenuFramework::AddSectionItem("PostProcess", Config::PostProcess);
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
        if (ImGuiMCP::Button(std::format("{}##RMultiplyClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::RMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##RMultiplyReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::RMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.RMultiply"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##RMultiply", &Configuration::RMultiply)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##GMultiplyClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::GMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##GMultiplyReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::GMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.GMultiply"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##GMultiply", &Configuration::GMultiply)) {
            Configuration::Save();
        }
    }

    {
        if (ImGuiMCP::Button(std::format("{}##BMultiplyClear", Translations::Get("MCP.Clear")).c_str())) {
            Configuration::BMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button(std::format("{}##BMultiplyReset", Translations::Get("MCP.Reset")).c_str())) {
            Configuration::BMultiply = 1.0f;
            Configuration::Save();
        }
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(Translations::Get("MCP.BMultiply"));
        ImGuiMCP::SameLine();
        if (ImGuiMCP::InputFloat("##BMultiply", &Configuration::BMultiply)) {
            Configuration::Save();
        }
    }

    if (ImGuiMCP::Button("Preview")) {
        Graphics::UpdatePostProcessedFrame();
    }
    auto texture = Graphics::GetPostProcessedFrame();
    if (texture) {
        D3D11_TEXTURE2D_DESC desc;
        ID3D11Resource* resource = nullptr;
        texture->GetResource(&resource);
        auto tex2D = static_cast<ID3D11Texture2D*>(resource);
        tex2D->GetDesc(&desc);

        float aspect = static_cast<float>(desc.Height) / desc.Width;
        float screenWidth = ImGuiMCP::GetIO()->DisplaySize.x;
        float width = screenWidth / 3.0f;
        float height = width * aspect;

        ImGuiMCP::Image(texture, ImGuiMCP::ImVec2(width, height));
        resource->Release();
    }
}
