// PluginDefinition.h - Terraform/HCL Syntax Highlighting Plugin for Notepad++

#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include "PluginInterface.h"

//-------------------------------------//
//-- PLUGIN NAME                     --//
//-------------------------------------//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Terraform Syntax");

//-----------------------------------------------//
//-- PLUGIN COMMAND NUMBER                     --//
//-----------------------------------------------//
const int nbFunc = 3;

//
// Plugin initialization
//
void pluginInit(HANDLE hModule);

//
// Plugin cleanup
//
void pluginCleanUp();

//
// Initialize plugin commands
//
void commandMenuInit();

//
// Clean up plugin commands
//
void commandMenuCleanUp();

//
// Set command function
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);

//
// Plugin command functions
//
void applyTerraformLexer();
void showAbout();
void showStyleConfig();

//
// Register the Terraform lexer with Scintilla
//
void registerTerraformLexer();

//
// Apply default Terraform styles
//
void applyDefaultStyles(HWND scintilla);

#endif //PLUGINDEFINITION_H
