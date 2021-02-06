#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"
#include "inireader/IniReader.h"

int currLangId;
int menuLang;
int menuJap;
const char* gameLanguages[] = { "english", "french", "german", "italian", "spanish", "russian", "japanese" };

signed int GetUserLanguage() {
    return currLangId;
}

void Init() {
    std::unique_ptr<ScopedUnprotect::Unprotect> Protect = ScopedUnprotect::UnprotectSectionOrFullModule(GetModuleHandle(nullptr), ".text");

    using namespace std;
    using namespace Memory;
    using namespace hook;

    CIniReader iniReader("");
    currLangId = iniReader.ReadInteger("MAIN", "GameLanguageID", 0);

    void* getGameLanguage[] = {
        get_pattern("E8 ? ? 30 00 83 3D ? ? ? ? 00 8B 0D ? ? ? ? 89 0D ? ? ? ? A3"),
        get_pattern("E8 ? ? 2E 00 ? ? 05 75 34 C7 44 ? ? ? ? 8F 3D 8B ? ? 10 89 44"),
        get_pattern("E8 ? ? FF FF 80 3D ? C2 ? ? 00 A3 ? ? ? ? A3 ? ? ? ? C6 05 ? 0C"),
        get_pattern("E8 ? ? FF FF 8B F8 ? ? ? ? 00 00 8B ? ? ? ? 03 00 80 3D ? C2"),
        get_pattern("E8 ? ? FF FF EB 05 A1 ? ? ? ? 80 BE 63 02 00 00 00 C6 86 ? 02 00"),
    };

    for (void* pattern : getGameLanguage) {
        InjectHook(pattern, GetUserLanguage, PATCH_CALL);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Init, 0, 0, 0);
    }
    return TRUE;
}
