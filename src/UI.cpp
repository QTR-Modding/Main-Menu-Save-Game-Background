#include "UI.h"
#include "Configuration.h"
#include "Translations.h"

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Config", Config::Render);
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

