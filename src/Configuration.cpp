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
    if (j.contains("ScreenPoolingIntervalInMilliseconds")) {
        ScreenPoolingIntervalInMilliseconds = j["ScreenPoolingIntervalInMilliseconds"];
        logger::info("ScreenPoolingIntervalInMilliseconds: {}", ScreenPoolingIntervalInMilliseconds);
    }

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
    if (j.contains("PostProcess")) {
        PostProcess = j["PostProcess"];
        logger::info("PostProcess: {}", PostProcess);
    }
    if (j.contains("BlurRadius")) {
        BlurRadius = j["BlurRadius"];
        logger::info("BlurRadius: {}", BlurRadius);
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
    if (j.contains("RMultiply")) {
        RMultiply = j["RMultiply"];
        logger::info("RMultiply: {}", RMultiply);
    }
    if (j.contains("GMultiply")) {
        GMultiply = j["GMultiply"];
        logger::info("GMultiply: {}", GMultiply);
    }
    if (j.contains("BMultiply")) {
        BMultiply = j["BMultiply"];
        logger::info("BMultiply: {}", BMultiply);
    }
}

void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["ScreenPoolingIntervalInMilliseconds"] = ScreenPoolingIntervalInMilliseconds;
    j["UpdateOnAutoSave"] = UpdateOnAutoSave;
    j["UpdateOnQuickSave"] = UpdateOnQuickSave;
    j["UpdateOnManualSave"] = UpdateOnManualSave;
    j["UpdateOnOtherSave"] = UpdateOnOtherSave;
    j["PostProcess"] = PostProcess;
    j["BlurRadius"] = BlurRadius;
    j["Brightness"] = Brightness;
    j["Saturation"] = Saturation;
    j["Contrast"] = Contrast;
    j["RMultiply"] = RMultiply;
    j["GMultiply"] = GMultiply;
    j["BMultiply"] = BMultiply;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
