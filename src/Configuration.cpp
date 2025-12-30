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

    if (j.contains("EnableOnAutoSave")) {
        EnableOnAutoSave = j["EnableOnAutoSave"];
        logger::info("EnableOnAutoSave: {}", EnableOnAutoSave);
    }
    if (j.contains("EnableOnQuickSave")) {
        EnableOnQuickSave = j["EnableOnQuickSave"];
        logger::info("EnableOnQuickSave: {}", EnableOnQuickSave);
    }
    if (j.contains("EnableOnManualSave")) {
        EnableOnManualSave = j["EnableOnManualSave"];
        logger::info("EnableOnManualSave: {}", EnableOnManualSave);
    }
    if (j.contains("EnableOnOtherSave")) {
        EnableOnOtherSave = j["EnableOnOtherSave"];
        logger::info("EnableOnOtherSave: {}", EnableOnOtherSave);
    }
}


void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["ScreenPoolingIntervalInMilliseconds"] = ScreenPoolingIntervalInMilliseconds;
    j["EnableOnAutoSave"] = EnableOnAutoSave;
    j["EnableOnQuickSave"] = EnableOnQuickSave;
    j["EnableOnManualSave"] = EnableOnManualSave;
    j["EnableOnOtherSave"] = EnableOnOtherSave;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
