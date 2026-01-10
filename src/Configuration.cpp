#include "Configuration.h"
#include <nlohmann/json.hpp>

void Configuration::Load() {
    std::ifstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Config file not found");
        return;
    }
    nlohmann::json j;
    file >> j;

    if (j.contains("UpdateOnAutoSave")) {
        UpdateOnAutoSave = j["UpdateOnAutoSave"];
        logger::info("UpdateOnAutoSave: {}", UpdateOnAutoSave);
    }
    if (j.contains("UpdateOnQuickSave")) {
        UpdateOnQuickSave = j["UpdateOnQuickSave"];
        logger::info("UpdateOnQuickSave: {}", UpdateOnQuickSave);
    }
    if (j.contains("UpdateOnManualSave")) {
        UpdateOnManualSave = j["UpdateOnManualSave"];
        logger::info("UpdateOnManualSave: {}", UpdateOnManualSave);
    }
    if (j.contains("UpdateOnOtherSave")) {
        UpdateOnOtherSave = j["UpdateOnOtherSave"];
        logger::info("UpdateOnOtherSave: {}", UpdateOnOtherSave);
    }
    if (j.contains("BlurRadius")) {
        BlurRadius = j["BlurRadius"];
        logger::info("BlurRadius: {}", BlurRadius);
    }
    if (j.contains("VignetteStrength")) {
        VignetteStrength = j["VignetteStrength"];
        logger::info("VignetteStrength: {}", VignetteStrength);
    }
    if (j.contains("Brightness")) {
        Brightness = j["Brightness"];
        logger::info("Brightness: {}", Brightness);
    }
    if (j.contains("Saturation")) {
        Saturation = j["Saturation"];
        logger::info("Saturation: {}", Saturation);
    }
    if (j.contains("Contrast")) {
        Contrast = j["Contrast"];
        logger::info("Contrast: {}", Contrast);
    }
    if (j.contains("EnableOnLoadingScreens")) {
        EnableOnLoadingScreens = j["EnableOnLoadingScreens"];
        logger::info("EnableOnLoadingScreens: {}", EnableOnLoadingScreens);
    }
    if (j.contains("EnableOnMainMenu")) {
        EnableOnMainMenu = j["EnableOnMainMenu"];
        logger::info("EnableOnMainMenu: {}", EnableOnMainMenu);
    }
    if (j.contains("DisableLoadingScreenMesh")) {
        DisableLoadingScreenMesh = j["DisableLoadingScreenMesh"];
        logger::info("DisableLoadingScreenMesh: {}", DisableLoadingScreenMesh);
    }
}

void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["UpdateOnAutoSave"] = UpdateOnAutoSave;
    j["UpdateOnQuickSave"] = UpdateOnQuickSave;
    j["UpdateOnManualSave"] = UpdateOnManualSave;
    j["UpdateOnOtherSave"] = UpdateOnOtherSave;
    j["BlurRadius"] = BlurRadius;
    j["VignetteStrength"] = VignetteStrength;
    j["Brightness"] = Brightness;
    j["Saturation"] = Saturation;
    j["Contrast"] = Contrast;
    j["EnableOnLoadingScreens"] = EnableOnLoadingScreens;
    j["EnableOnMainMenu"] = EnableOnMainMenu;
    j["DisableLoadingScreenMesh"] = DisableLoadingScreenMesh;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
