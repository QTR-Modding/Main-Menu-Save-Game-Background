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

    if (j.contains("UpdateAutoSave")) {
        UpdateOnAutoSave = j["UpdateAutoSave"];
        logger::info("UpdateAutoSave: {}", UpdateOnAutoSave);
    }
    if (j.contains("UpdateQuickSave")) {
        UpdateOnQuickSave = j["UpdateQuickSave"];
        logger::info("UpdateQuickSave: {}", UpdateOnQuickSave);
    }
    if (j.contains("UpdateManualSave")) {
        UpdateOnManualSave = j["UpdateManualSave"];
        logger::info("UpdateManualSave: {}", UpdateOnManualSave);
    }
    if (j.contains("UpdateOtherSave")) {
        UpdateOnOtherSave = j["UpdateOtherSave"];
        logger::info("UpdateOtherSave: {}", UpdateOnOtherSave);
    }
}


void Configuration::Save() {
    std::filesystem::create_directories(".\\Data\\SKSE\\Plugins");

    nlohmann::json j;
    j["ScreenPoolingIntervalInMilliseconds"] = ScreenPoolingIntervalInMilliseconds;
    j["UpdateAutoSave"] = UpdateOnAutoSave;
    j["UpdateQuickSave"] = UpdateOnQuickSave;
    j["UpdateManualSave"] = UpdateOnManualSave;
    j["UpdateOtherSave"] = UpdateOnOtherSave;

    std::ofstream file(".\\Data\\SKSE\\Plugins\\MainMenuSaveGameBackground.json");
    if (!file.is_open()) {
        logger::error("Failed to write config file");
        return;
    }

    file << j.dump(4);
}
