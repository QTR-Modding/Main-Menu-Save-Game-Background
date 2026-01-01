#pragma once
namespace Hooks {

        class LoadGameHook {
    public:
        static void Install();

    private:
        static int32_t thunk(RE::BSWin32SaveDataSystemUtility* a1, char* fileName, void* a3);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    class SaveGameHook {
    public:
        static void Install();

    private:
        static char* thunk(RE::BGSSaveLoadManager* a1, void* a2, char* a3, void* a4, int32_t a5);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    class LoadingScreenHook {
    public:
        static void Install();

    private:
        static int64_t thunk(int64_t a1, uint32_t a2);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct CreateD3DAndSwapChain {
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    struct RenderUIHook {
        static int64_t thunk(int64_t gMenuManager);
        static inline REL::Relocation<decltype(thunk)> originalFunction;
        static void Install();
    };

    void Install();
    void DataLoaded();
}
