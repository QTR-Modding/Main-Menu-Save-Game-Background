#pragma once
#include "SKSEMenuFramework.h"
#include "Configuration.h"

namespace UI {
    void Register();
    namespace Config {

        void Render();
        void __stdcall Render();
        void __stdcall PostProcess();
        void __stdcall Debug();
    }
};