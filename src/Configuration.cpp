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
}
