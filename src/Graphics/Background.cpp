#include "Background.h"
#include "TextureSaver.h"
#include "SKSEMenuFramework.h"
#include "SaveGame.h"
#include "Menu.h"
#include "ScreenCapture.h"
#include "PostProcessor.h"
#include "Configuration.h"

#include <shared_mutex> 

#define QUICK_SAVE_DEVICE_ID 4
#define AUTO_SAVE_DEVICE_ID 3
#define MANUAL_SAVE_DEVICE_ID 2

namespace Background {
    class TextureContainer {
        ID3D11ShaderResourceView* texture = nullptr;

    public:
        void SetTexture(ID3D11ShaderResourceView* newTexture) {
            ID3D11ShaderResourceView* old = texture;
            texture = newTexture;
            if (old) {
                old->Release();
            }
        }
        ID3D11ShaderResourceView* GetTexture() { return texture; }
    };

    class ExternalTextureContainer {
        ID3D11ShaderResourceView* texture = nullptr;
        std::string _path = "";
    public:
        void SetTexture(std::string path) {
            if (texture) {
                SKSEMenuFramework::DisposeTexture(path);
                texture = nullptr;
            }
            texture = reinterpret_cast<ID3D11ShaderResourceView*>(SKSEMenuFramework::LoadTexture(path));
            _path = path;
        }
        ID3D11ShaderResourceView* GetTexture() { return texture; }
    };

    std::shared_mutex mutex;
    ExternalTextureContainer rawBackgroundImage;
    TextureContainer backgroundImage;
    ScreenCapture currentGameFrame;

    void Save(const char* a_fileName) {
        std::unique_lock<std::shared_mutex> lock(mutex);

        std::vector<std::wstring> items;
        items.push_back(SaveGame::getDDSFullWPath(a_fileName).c_str());
        TextureSaver::SaveResourceAsDDS(currentGameFrame.GetTextureBuffer(), items);
    }
}

void Background::OnSave(char* fileName, int32_t deviceId) {
    if (deviceId == QUICK_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnQuickSave) {
            Save(fileName);
        }
    } else if (deviceId == AUTO_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnAutoSave) {
            Save(fileName);
        }
    } else if (deviceId == MANUAL_SAVE_DEVICE_ID) {
        if (Configuration::UpdateOnManualSave) {
            Save(fileName);
        }
    } else {
        if (Configuration::UpdateOnOtherSave) {
            Save(fileName);
        }
    }
}

ID3D11ShaderResourceView* Background::GetBackgroundImage() {
    return backgroundImage.GetTexture();
}

ID3D11ShaderResourceView* Background::GetCurrentFrame() {
    return currentGameFrame.GetTextureBuffer();
}

void Background::FetchGameFrame() { 
    if (Menu::IsOpen()) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(mutex);
    currentGameFrame.FetchTextureBuffer();
}

void Background::OnLoad(const char* a_fileName) {
    auto path = SaveGame::replaceESSWithDDS(a_fileName);
    rawBackgroundImage.SetTexture(path);
    auto texture = PostProcessor::PostProcessResource(rawBackgroundImage.GetTexture());
    backgroundImage.SetTexture(texture);
}

void Background::LoadLastSaveTexture() {
    auto manager = RE::BGSSaveLoadManager::GetSingleton();
    if (manager->lastFileName.length() == 1) {
        manager->PopulateSaveList();
    }
    auto path = SaveGame::GetFullPath(manager->lastFileName.c_str());
    Background::OnLoad(path.c_str());
}

void Background::Apply() {
    if (auto texture = rawBackgroundImage.GetTexture()) {
        backgroundImage.SetTexture(PostProcessor::PostProcessResource(texture));
    }
}

void Background::Clear() {
    backgroundImage.SetTexture(nullptr);
}