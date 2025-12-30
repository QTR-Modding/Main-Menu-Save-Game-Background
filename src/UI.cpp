#include "UI.h"
#include "Configuration.h"
void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }
    SKSEMenuFramework::SetSection(MOD_NAME);
    SKSEMenuFramework::AddSectionItem("Config", Config::Render);
}


void __stdcall UI::Config::Render() {
    ImGuiMCP::Text("When should the menu image be updated");

    if (ImGuiMCPComponents::ToggleButton("Enable on auto save", &Configuration::EnableOnAutoSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton("Enable on quick save", &Configuration::EnableOnQuickSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton("Enable on manual save", &Configuration::EnableOnManualSave)) {
        Configuration::Save();
    }
    if (ImGuiMCPComponents::ToggleButton("Enable on other save", &Configuration::EnableOnOtherSave)) {
        Configuration::Save();
    }
}

