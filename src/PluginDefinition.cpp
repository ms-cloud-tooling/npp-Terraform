// PluginDefinition.cpp - Terraform/HCL Syntax Highlighting Plugin Implementation

#include "PluginDefinition.h"
#include "TerraformLexer.h"
#include "menuCmdID.h"

// Plugin data
FuncItem funcItem[nbFunc];
NppData nppData;
HANDLE hModule;
bool lexerRegistered = false;

// Terraform Style Colors (can be customized)
struct TerraformStyle {
    int style;
    COLORREF foreground;
    COLORREF background;
    bool bold;
    bool italic;
};

static TerraformStyle defaultStyles[] = {
    { SCE_TF_DEFAULT,          RGB(0, 0, 0),       RGB(255, 255, 255), false, false },
    { SCE_TF_COMMENT_LINE,     RGB(0, 128, 0),     RGB(255, 255, 255), false, true  },
    { SCE_TF_COMMENT_BLOCK,    RGB(0, 128, 0),     RGB(255, 255, 255), false, true  },
    { SCE_TF_STRING,           RGB(163, 21, 21),   RGB(255, 255, 255), false, false },
    { SCE_TF_HEREDOC,          RGB(163, 21, 21),   RGB(255, 250, 240), false, false },
    { SCE_TF_NUMBER,           RGB(255, 128, 0),   RGB(255, 255, 255), false, false },
    { SCE_TF_KEYWORD,          RGB(0, 0, 255),     RGB(255, 255, 255), true,  false },
    { SCE_TF_ATTRIBUTE,        RGB(128, 0, 128),   RGB(255, 255, 255), false, false },
    { SCE_TF_IDENTIFIER,       RGB(0, 0, 0),       RGB(255, 255, 255), false, false },
    { SCE_TF_OPERATOR,         RGB(0, 0, 0),       RGB(255, 255, 255), false, false },
    { SCE_TF_BRACKET,          RGB(128, 0, 0),     RGB(255, 255, 255), true,  false },
    { SCE_TF_TYPE,             RGB(43, 145, 175),  RGB(255, 255, 255), false, false },
    { SCE_TF_BUILTIN_FUNCTION, RGB(136, 0, 136),   RGB(255, 255, 255), false, false },
    { SCE_TF_INTERPOLATION,    RGB(255, 69, 0),    RGB(255, 255, 240), true,  false },
    { SCE_TF_VARIABLE_REF,     RGB(0, 128, 128),   RGB(255, 255, 255), false, false },
    { SCE_TF_BOOL,             RGB(0, 0, 255),     RGB(255, 255, 255), false, false },
    { SCE_TF_NULL,             RGB(128, 128, 128), RGB(255, 255, 255), false, true  },
    { SCE_TF_BLOCK_TYPE,       RGB(0, 0, 255),     RGB(255, 255, 255), true,  false },
    { SCE_TF_META_ARGUMENT,    RGB(139, 0, 139),   RGB(255, 255, 255), false, false },
    { SCE_TF_PROVISIONER,      RGB(75, 0, 130),    RGB(255, 255, 255), false, false },
    { SCE_TF_ESCAPE_CHAR,      RGB(255, 0, 128),   RGB(255, 255, 255), true,  false },
};

void pluginInit(HANDLE hMod) {
    hModule = hMod;
}

void pluginCleanUp() {
}

void commandMenuInit() {
    setCommand(0, TEXT("Apply Terraform Syntax"), applyTerraformLexer, NULL, false);
    setCommand(1, TEXT("Configure Styles..."), showStyleConfig, NULL, false);
    setCommand(2, TEXT("About"), showAbout, NULL, false);
}

void commandMenuCleanUp() {
}

bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) {
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

void registerTerraformLexer() {
    if (lexerRegistered) return;

    // Get current Scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1) return;

    HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    // Create lexer instance and set it
    Scintilla::ILexer5* lexer = TerraformLexer::LexerFactory();
    ::SendMessage(curScintilla, SCI_SETILEXER, 0, (LPARAM)lexer);

    lexerRegistered = true;
}

void applyDefaultStyles(HWND scintilla) {
    // Set default style first
    ::SendMessage(scintilla, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(0, 0, 0));
    ::SendMessage(scintilla, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(255, 255, 255));
    ::SendMessage(scintilla, SCI_STYLECLEARALL, 0, 0);

    // Apply Terraform styles
    for (const auto& style : defaultStyles) {
        ::SendMessage(scintilla, SCI_STYLESETFORE, style.style, style.foreground);
        ::SendMessage(scintilla, SCI_STYLESETBACK, style.style, style.background);
        ::SendMessage(scintilla, SCI_STYLESETBOLD, style.style, style.bold ? 1 : 0);
        ::SendMessage(scintilla, SCI_STYLESETITALIC, style.style, style.italic ? 1 : 0);
    }

    // Set keywords for Terraform
    const char* keywords = "terraform provider resource data module variable output locals moved import removed check";
    ::SendMessage(scintilla, SCI_SETKEYWORDS, 0, (LPARAM)keywords);

    const char* types = "string number bool list map set object tuple any optional dynamic";
    ::SendMessage(scintilla, SCI_SETKEYWORDS, 1, (LPARAM)types);

    // Enable folding
    ::SendMessage(scintilla, SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
    ::SendMessage(scintilla, SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");

    // Request re-lex
    Sci_Position docLen = static_cast<Sci_Position>(::SendMessage(scintilla, SCI_GETLENGTH, 0, 0));
    ::SendMessage(scintilla, SCI_COLOURISE, 0, docLen);
}

void applyTerraformLexer() {
    // Get current Scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1) return;

    HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    // Create and set lexer
    Scintilla::ILexer5* lexer = TerraformLexer::LexerFactory();
    ::SendMessage(curScintilla, SCI_SETILEXER, 0, (LPARAM)lexer);

    // Apply styles
    applyDefaultStyles(curScintilla);

    ::MessageBox(nppData._nppHandle,
        TEXT("Terraform/HCL syntax highlighting has been applied to the current document."),
        TEXT("Terraform Syntax"),
        MB_OK | MB_ICONINFORMATION);
}

void showStyleConfig() {
    ::MessageBox(nppData._nppHandle,
        TEXT("Style Configuration:\n\n")
        TEXT("To customize colors, modify the defaultStyles array in PluginDefinition.cpp and rebuild the plugin.\n\n")
        TEXT("Styles available:\n")
        TEXT("- Default\n")
        TEXT("- Comments (line and block)\n")
        TEXT("- Strings and Heredocs\n")
        TEXT("- Numbers\n")
        TEXT("- Keywords (resource, variable, etc.)\n")
        TEXT("- Types (string, number, bool, etc.)\n")
        TEXT("- Built-in Functions\n")
        TEXT("- Interpolations (${...})\n")
        TEXT("- Variable References (var.xxx)\n")
        TEXT("- Operators and Brackets"),
        TEXT("Style Configuration"),
        MB_OK | MB_ICONINFORMATION);
}

void showAbout() {
    ::MessageBox(nppData._nppHandle,
        TEXT("Terraform/HCL Syntax Highlighting Plugin\n")
        TEXT("Version 1.0.0\n\n")
        TEXT("Features:\n")
        TEXT("- Full Terraform/HCL syntax highlighting\n")
        TEXT("- Support for all block types\n")
        TEXT("- Built-in function recognition\n")
        TEXT("- String interpolation highlighting\n")
        TEXT("- Heredoc support\n")
        TEXT("- Code folding\n\n")
        TEXT("Supported file extensions: .tf, .tfvars, .hcl\n\n")
        TEXT("For issues and updates, visit the GitHub repository."),
        TEXT("About Terraform Syntax"),
        MB_OK | MB_ICONINFORMATION);
}
