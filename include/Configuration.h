#pragma once
namespace Configuration {
    inline bool UpdateOnAutoSave = false;
    inline bool UpdateOnQuickSave = true;
    inline bool UpdateOnManualSave = true;
    inline bool UpdateOnOtherSave = true;

    inline float BlurRadius = 1.0f;
    inline float VignetteStrength = 1.0f;
    inline float Brightness = 1.0f;
    inline float Saturation = 1.0f;
    inline float Contrast = 1.0f;
    inline float FadeDuration = 1.f;

    inline bool ReplaceLoadingScreenMesh = true;

    void Load();
    void Save();
};