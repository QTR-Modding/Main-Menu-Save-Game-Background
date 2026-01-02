#pragma once
#include "SKSEMenuFramework.h"
#include "Configuration.h"
#define TEXTURE_PATH ".\\DATA\\textures\\interface\\objects\\mainmenuwallpaper.dds"

namespace UI {
    void Register();
    namespace Config {
        void __stdcall Hud();
        void __stdcall Render();
        void __stdcall PostProcess();
    }
};