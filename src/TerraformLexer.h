// TerraformLexer.h - Terraform/HCL Lexer for Notepad++
// ILexer5 implementation for Terraform syntax highlighting

#pragma once

#include "ILexer.h"
#include <string>
#include <set>
#include <vector>

// Style definitions for Terraform/HCL
enum TerraformStyles {
    SCE_TF_DEFAULT = 0,
    SCE_TF_COMMENT_LINE = 1,
    SCE_TF_COMMENT_BLOCK = 2,
    SCE_TF_STRING = 3,
    SCE_TF_HEREDOC = 4,
    SCE_TF_NUMBER = 5,
    SCE_TF_KEYWORD = 6,           // resource, variable, output, etc.
    SCE_TF_ATTRIBUTE = 7,         // attribute names
    SCE_TF_IDENTIFIER = 8,
    SCE_TF_OPERATOR = 9,
    SCE_TF_BRACKET = 10,
    SCE_TF_TYPE = 11,             // string, number, bool, list, map, etc.
    SCE_TF_BUILTIN_FUNCTION = 12, // Built-in functions
    SCE_TF_INTERPOLATION = 13,    // ${...} expressions
    SCE_TF_VARIABLE_REF = 14,     // var.xxx, local.xxx references
    SCE_TF_BOOL = 15,             // true, false
    SCE_TF_NULL = 16,             // null
    SCE_TF_BLOCK_TYPE = 17,       // terraform, provider, data, module, locals
    SCE_TF_META_ARGUMENT = 18,    // count, for_each, depends_on, lifecycle, etc.
    SCE_TF_PROVISIONER = 19,      // provisioner types
    SCE_TF_ESCAPE_CHAR = 20,      // Escape sequences in strings
};

class TerraformLexer : public Scintilla::ILexer5 {
private:
    std::set<std::string> keywords;           // Block types
    std::set<std::string> types;              // Data types
    std::set<std::string> builtinFunctions;   // Built-in functions
    std::set<std::string> metaArguments;      // Meta-arguments
    std::set<std::string> provisioners;       // Provisioner types

    void InitKeywords();
    bool IsKeyword(const std::string& word) const;
    bool IsType(const std::string& word) const;
    bool IsBuiltinFunction(const std::string& word) const;
    bool IsMetaArgument(const std::string& word) const;
    bool IsProvisioner(const std::string& word) const;

    static bool IsIdentifierStart(char ch);
    static bool IsIdentifierChar(char ch);
    static bool IsDigit(char ch);
    static bool IsHexDigit(char ch);
    static bool IsOperator(char ch);

    void ColouriseDoc(Sci_Position startPos, Sci_Position length, int initStyle, Scintilla::IDocument *pAccess);
    void FoldDoc(Sci_Position startPos, Sci_Position length, int initStyle, Scintilla::IDocument *pAccess);

public:
    TerraformLexer();
    virtual ~TerraformLexer();

    // ILexer5 interface
    int SCI_METHOD Version() const override;
    void SCI_METHOD Release() override;
    const char * SCI_METHOD PropertyNames() override;
    int SCI_METHOD PropertyType(const char *name) override;
    const char * SCI_METHOD DescribeProperty(const char *name) override;
    Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;
    const char * SCI_METHOD DescribeWordListSets() override;
    Sci_Position SCI_METHOD WordListSet(int n, const char *wl) override;
    void SCI_METHOD Lex(Sci_Position startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
    void SCI_METHOD Fold(Sci_Position startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
    void * SCI_METHOD PrivateCall(int operation, void *pointer) override;
    int SCI_METHOD LineEndTypesSupported() override;
    int SCI_METHOD AllocateSubStyles(int styleBase, int numberStyles) override;
    int SCI_METHOD SubStylesStart(int styleBase) override;
    int SCI_METHOD SubStylesLength(int styleBase) override;
    int SCI_METHOD StyleFromSubStyle(int subStyle) override;
    int SCI_METHOD PrimaryStyleFromStyle(int style) override;
    void SCI_METHOD FreeSubStyles() override;
    void SCI_METHOD SetIdentifiers(int style, const char *identifiers) override;
    int SCI_METHOD DistanceToSecondaryStyles() override;
    const char * SCI_METHOD GetSubStyleBases() override;
    int SCI_METHOD NamedStyles() override;
    const char * SCI_METHOD NameOfStyle(int style) override;
    const char * SCI_METHOD TagsOfStyle(int style) override;
    const char * SCI_METHOD DescriptionOfStyle(int style) override;

    // ILexer5 specific
    const char * SCI_METHOD GetName() override;
    int SCI_METHOD GetIdentifier() override;
    const char * SCI_METHOD PropertyGet(const char *key) override;

    static Scintilla::ILexer5* LexerFactory();
};
