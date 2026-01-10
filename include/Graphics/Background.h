#pragma once

namespace Background {
    void OnSave(char* fileName, int32_t deviceId);
    void OnLoad(const char* a_fileName);

    ID3D11ShaderResourceView* GetBackgroundImage();
    ID3D11ShaderResourceView* GetCurrentFrame();
    void FetchGameFrame();
    void LoadLastSaveTexture();
    void Apply();
    void Clear();
}