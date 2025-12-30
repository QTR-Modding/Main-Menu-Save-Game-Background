#pragma once
namespace Configuration {
    inline int ScreenPoolingIntervalInMilliseconds = 1000;
    inline bool UpdateOnAutoSave = false;
    inline bool UpdateOnQuickSave = true;
    inline bool UpdateOnManualSave = true;
    inline bool UpdateOnOtherSave = true;
    void Load();
    void Save();
};