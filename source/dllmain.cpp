#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"
#include "inireader/IniReader.h"

int currLangId;
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
        get_pattern("E8 E2 EE 30 00"),
        get_pattern("E8 0A A9 2E 00"),
        get_pattern("E8 9E 73 FF FF"),
        get_pattern("E8 9D E5 FF FF"),
        get_pattern("E8 A1 1F FF FF"),
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
