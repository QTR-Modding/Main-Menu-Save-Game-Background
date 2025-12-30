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
}

void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["ScreenPoolingIntervalInMilliseconds"] = ScreenPoolingIntervalInMilliseconds;
    j["UpdateOnAutoSave"] = UpdateOnAutoSave;
    j["UpdateOnQuickSave"] = UpdateOnQuickSave;
    j["UpdateOnManualSave"] = UpdateOnManualSave;
    j["UpdateOnOtherSave"] = UpdateOnOtherSave;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
