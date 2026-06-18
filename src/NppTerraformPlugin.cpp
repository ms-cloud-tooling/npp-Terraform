// NppTerraformPlugin.cpp - Main entry point for the Terraform Syntax Plugin

#include "PluginDefinition.h"
#include "TerraformLexer.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reasonForCall, LPVOID /*lpReserved*/) {
    try {
        switch (reasonForCall) {
            case DLL_PROCESS_ATTACH:
                pluginInit(hModule);
                break;

            case DLL_PROCESS_DETACH:
                pluginCleanUp();
                break;

            case DLL_THREAD_ATTACH:
                break;

            case DLL_THREAD_DETACH:
                break;
        }
    }
    catch (...) {
        return FALSE;
    }

    return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData) {
    nppData = notpadPlusData;
    commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR* getName() {
    return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int *nbF) {
    *nbF = nbFunc;
    return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode) {
    switch (notifyCode->nmhdr.code) {
        case NPPN_SHUTDOWN:
            commandMenuCleanUp();
            break;

        case NPPN_BUFFERACTIVATED:
        case NPPN_FILEOPENED:
            {
                // Check if file is a Terraform file
                TCHAR filePath[MAX_PATH];
                ::SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, MAX_PATH, (LPARAM)filePath);

                // Check extension
                TCHAR* ext = wcsrchr(filePath, L'.');
                if (ext != nullptr) {
                    if (_wcsicmp(ext, L".tf") == 0 ||
                        _wcsicmp(ext, L".tfvars") == 0 ||
                        _wcsicmp(ext, L".hcl") == 0) {
                        // Apply Terraform lexer automatically
                        int which = -1;
                        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
                        if (which != -1) {
                            HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

                            // Create and set lexer
                            Scintilla::ILexer5* lexer = TerraformLexer::LexerFactory();
                            ::SendMessage(curScintilla, SCI_SETILEXER, 0, (LPARAM)lexer);

                            // Apply styles
                            applyDefaultStyles(curScintilla);
                        }
                    }
                }
            }
            break;

        default:
            break;
    }
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {
    return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode() {
    return TRUE;
}
#endif

// Export the lexer factory for direct lexer registration
extern "C" __declspec(dllexport) Scintilla::ILexer5* CreateLexer(const char* name) {
    if (name != nullptr && strcmp(name, "terraform") == 0) {
        return TerraformLexer::LexerFactory();
    }
    return nullptr;
}

extern "C" __declspec(dllexport) const char* GetLexerName(unsigned int index) {
    if (index == 0) {
        return "terraform";
    }
    return nullptr;
}

extern "C" __declspec(dllexport) int GetLexerCount() {
    return 1;
}

extern "C" __declspec(dllexport) void GetLexerStatusText(unsigned int index, TCHAR* desc, int maxLen) {
    if (index == 0 && desc != nullptr && maxLen > 0) {
        wcsncpy_s(desc, maxLen, L"Terraform/HCL", maxLen - 1);
    }
}
