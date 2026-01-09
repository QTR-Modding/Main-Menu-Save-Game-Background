#include "Plugin.h"
#include "Hooks.h"
#include "Configuration.h"
#include "Translations.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    if (message->type == SKSE::MessagingInterface::kPostLoad) {
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    Translations::Install();
    UI::Register();
    Hooks::Install();
    Configuration::Load();
    return true;
}
