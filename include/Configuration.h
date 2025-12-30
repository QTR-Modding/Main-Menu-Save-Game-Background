#pragma once
namespace Configuration {
    inline int ScreenPoolingIntervalInMilliseconds = 1000;
    inline bool EnableOnAutoSave = false;
    inline bool EnableOnQuickSave = true;
    inline bool EnableOnManualSave = true;
    inline bool EnableOnOtherSave = true;
    void Load();
    void Save();
};