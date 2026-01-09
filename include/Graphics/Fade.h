#pragma once

namespace Fade {
    void FetchFadeFrame();
    ID3D11ShaderResourceView* GetFadeFrame();
    float GetFadeAmmount();
    void Render();
}