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

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.BlurRadius"), &Configuration::BlurRadius)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##BlurRadius", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::BlurRadius = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.VignetteStrength"), &Configuration::VignetteStrength)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##VignetteStrength", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::VignetteStrength = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.Brightness"), &Configuration::Brightness)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##Brightness", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::Brightness = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.Contrast"), &Configuration::Contrast)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##Contrast", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::Contrast = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.RMultiply"), &Configuration::RMultiply)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##RMultiply", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::RMultiply = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.GMultiply"), &Configuration::GMultiply)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##GMultiply", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::GMultiply = 1.0f;
        Configuration::Save();
    }

    if (ImGuiMCP::InputFloat(Translations::Get("MCP.BMultiply"), &Configuration::BMultiply)) {
        Configuration::Save();
    }
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button(std::format("{}##BMultiply", Translations::Get("MCP.Clear")).c_str())) {
        Configuration::BMultiply = 1.0f;
        Configuration::Save();
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
