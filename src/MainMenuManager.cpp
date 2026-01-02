#include "MainMenuManager.h"
#include "Graphics.h"
#include "Menu.h"
#include "Configuration.h"

using namespace std::chrono;
auto lastTime = steady_clock::now() - milliseconds(Configuration::ScreenPoolingIntervalInMilliseconds);

void MainMenuManager::OnRenderUI() {
    if (!Menu::IsOpen()) {
        auto now = steady_clock::now();
        if (duration_cast<milliseconds>(now - lastTime).count() >= Configuration::ScreenPoolingIntervalInMilliseconds) {
            lastTime = now;
        }
        Graphics::FetchTextureBuffer();
    }
}

void MainMenuManager::OnSaveGame() {
    Graphics::SaveCurrentFrameAsDDS(L".\\Data\\textures\\interface\\objects\\mainmenuwallpaper.dds", true);
}