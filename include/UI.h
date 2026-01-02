#pragma once
#include "SKSEMenuFramework.h"
#include "Configuration.h"
namespace UI {
    void Register();
    namespace Config {
        void __stdcall Hud();
        void __stdcall Render();
        void __stdcall PostProcess();
    }
};