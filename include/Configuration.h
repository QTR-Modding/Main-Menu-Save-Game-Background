#pragma once
namespace Configuration {
    inline int ScreenPoolingIntervalInMilliseconds = 1000;
    inline bool UpdateOnAutoSave = false;
    inline bool UpdateOnQuickSave = true;
    inline bool UpdateOnManualSave = true;
    inline bool UpdateOnOtherSave = true;

    inline bool PostProcess = true;
    inline float BlurRadius = 1.0f;
    inline float Brightness = 1.0f;
    inline float Saturation = 1.0f;
    inline float Contrast = 1.0f;
    inline float RMultiply = 1.0f;
    inline float GMultiply = 1.0f;
    inline float BMultiply = 1.0f;
    void Load();
    void Save();
};