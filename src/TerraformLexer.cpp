// TerraformLexer.cpp - Terraform/HCL Lexer Implementation

#include "TerraformLexer.h"
#include "Scintilla.h"
#include <cstring>
#include <cctype>

// Lexer identifier - must be unique
#define SCLEX_TERRAFORM 200

TerraformLexer::TerraformLexer() {
    InitKeywords();
}

TerraformLexer::~TerraformLexer() {
}

void TerraformLexer::InitKeywords() {
    // Block types (main Terraform keywords)
    const char* keywordList[] = {
        "terraform", "provider", "resource", "data", "module", "variable",
        "output", "locals", "moved", "import", "removed", "check"
    };
    for (const char* kw : keywordList) {
        keywords.insert(kw);
    }

    // Data types
    const char* typeList[] = {
        "string", "number", "bool", "list", "map", "set", "object", "tuple",
        "any", "optional", "dynamic"
    };
    for (const char* t : typeList) {
        types.insert(t);
    }

    // Built-in functions
    const char* functionList[] = {
        // Numeric Functions
        "abs", "ceil", "floor", "log", "max", "min", "parseint", "pow", "signum",
        // String Functions
        "chomp", "format", "formatlist", "indent", "join", "lower", "regex",
        "regexall", "replace", "split", "strrev", "substr", "title", "trim",
        "trimprefix", "trimsuffix", "trimspace", "upper", "startswith", "endswith",
        // Collection Functions
        "alltrue", "anytrue", "chunklist", "coalesce", "coalescelist", "compact",
        "concat", "contains", "distinct", "element", "flatten", "index", "keys",
        "length", "list", "lookup", "map", "matchkeys", "merge", "one", "range",
        "reverse", "setintersection", "setproduct", "setsubtract", "setunion",
        "slice", "sort", "sum", "transpose", "values", "zipmap",
        // Encoding Functions
        "base64decode", "base64encode", "base64gzip", "csvdecode", "jsondecode",
        "jsonencode", "textdecodebase64", "textencodebase64", "urlencode",
        "yamldecode", "yamlencode",
        // Filesystem Functions
        "abspath", "dirname", "pathexpand", "basename", "file", "fileexists",
        "fileset", "filebase64", "templatefile", "filemd5", "filesha1",
        "filesha256", "filesha512",
        // Date and Time Functions
        "formatdate", "timeadd", "timecmp", "timestamp", "plantimestamp",
        // Hash and Crypto Functions
        "base64sha256", "base64sha512", "bcrypt", "md5", "rsadecrypt", "sha1",
        "sha256", "sha512", "uuid", "uuidv5",
        // IP Network Functions
        "cidrhost", "cidrnetmask", "cidrsubnet", "cidrsubnets", "cidrcontains",
        // Type Conversion Functions
        "can", "issensitive", "nonsensitive", "sensitive", "tobool", "tolist",
        "tomap", "tonumber", "toset", "tostring", "try", "type",
        // Terraform-specific
        "templatefile", "path", "self", "count", "each", "terraform"
    };
    for (const char* f : functionList) {
        builtinFunctions.insert(f);
    }

    // Meta-arguments
    const char* metaArgList[] = {
        "count", "for_each", "depends_on", "provider", "lifecycle",
        "provisioner", "connection", "precondition", "postcondition",
        "create_before_destroy", "prevent_destroy", "ignore_changes",
        "replace_triggered_by"
    };
    for (const char* m : metaArgList) {
        metaArguments.insert(m);
    }

    // Provisioner types
    const char* provisionerList[] = {
        "file", "local-exec", "remote-exec"
    };
    for (const char* p : provisionerList) {
        provisioners.insert(p);
    }
}

bool TerraformLexer::IsKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

bool TerraformLexer::IsType(const std::string& word) const {
    return types.find(word) != types.end();
}

bool TerraformLexer::IsBuiltinFunction(const std::string& word) const {
    return builtinFunctions.find(word) != builtinFunctions.end();
}

bool TerraformLexer::IsMetaArgument(const std::string& word) const {
    return metaArguments.find(word) != metaArguments.end();
}

bool TerraformLexer::IsProvisioner(const std::string& word) const {
    return provisioners.find(word) != provisioners.end();
}

bool TerraformLexer::IsIdentifierStart(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool TerraformLexer::IsIdentifierChar(char ch) {
    return IsIdentifierStart(ch) || (ch >= '0' && ch <= '9') || ch == '-';
}

bool TerraformLexer::IsDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool TerraformLexer::IsHexDigit(char ch) {
    return IsDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool TerraformLexer::IsOperator(char ch) {
    return ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
           ch == '%' || ch == '!' || ch == '<' || ch == '>' || ch == '&' ||
           ch == '|' || ch == '?' || ch == ':' || ch == '.' || ch == ',';
}

void TerraformLexer::ColouriseDoc(Sci_Position startPos, Sci_Position length, int initStyle, Scintilla::IDocument *pAccess) {
    Sci_Position endPos = startPos + length;

    // Get the document buffer
    const char* buffer = pAccess->BufferPointer();
    Sci_Position docLength = pAccess->Length();

    pAccess->StartStyling(startPos);

    int state = initStyle;
    Sci_Position i = startPos;

    std::string currentWord;
    std::string heredocDelimiter;
    int interpolationDepth = 0;
    int bracketDepth = 0;
    bool atLineStart = true;
    bool expectingBlockName = false;

    // Adjust state for continuation
    if (startPos > 0) {
        Sci_Position line = pAccess->LineFromPosition(startPos);
        if (line > 0) {
            int lineState = pAccess->GetLineState(line - 1);
            interpolationDepth = lineState & 0xFF;
            bracketDepth = (lineState >> 8) & 0xFF;
        }
    }

    while (i < endPos && i < docLength) {
        char ch = buffer[i];
        char chNext = (i + 1 < docLength) ? buffer[i + 1] : '\0';
        char chNext2 = (i + 2 < docLength) ? buffer[i + 2] : '\0';

        switch (state) {
            case SCE_TF_DEFAULT:
                if (ch == '#' || (ch == '/' && chNext == '/')) {
                    // Line comment
                    state = SCE_TF_COMMENT_LINE;
                    pAccess->SetStyleFor(1, SCE_TF_COMMENT_LINE);
                    i++;
                }
                else if (ch == '/' && chNext == '*') {
                    // Block comment
                    state = SCE_TF_COMMENT_BLOCK;
                    pAccess->SetStyleFor(2, SCE_TF_COMMENT_BLOCK);
                    i += 2;
                }
                else if (ch == '"') {
                    // Check for heredoc
                    if (chNext == '<' && chNext2 == '<') {
                        // Heredoc: <<EOF or <<-EOF
                        state = SCE_TF_HEREDOC;
                        Sci_Position heredocStart = i;
                        i += 2;
                        if (i < docLength && buffer[i] == '-') i++;

                        // Get delimiter
                        heredocDelimiter.clear();
                        while (i < docLength && IsIdentifierChar(buffer[i])) {
                            heredocDelimiter += buffer[i];
                            i++;
                        }
                        pAccess->SetStyleFor(static_cast<Sci_Position>(i - heredocStart), SCE_TF_HEREDOC);
                    }
                    else {
                        // Regular string
                        state = SCE_TF_STRING;
                        pAccess->SetStyleFor(1, SCE_TF_STRING);
                        i++;
                    }
                }
                else if (ch == '<' && chNext == '<') {
                    // Heredoc without leading quote
                    state = SCE_TF_HEREDOC;
                    Sci_Position heredocStart = i;
                    i += 2;
                    if (i < docLength && buffer[i] == '-') i++;

                    heredocDelimiter.clear();
                    while (i < docLength && IsIdentifierChar(buffer[i])) {
                        heredocDelimiter += buffer[i];
                        i++;
                    }
                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - heredocStart), SCE_TF_HEREDOC);
                }
                else if (IsIdentifierStart(ch)) {
                    // Identifier or keyword
                    currentWord.clear();
                    Sci_Position wordStart = i;
                    while (i < docLength && IsIdentifierChar(buffer[i])) {
                        currentWord += buffer[i];
                        i++;
                    }

                    int style = SCE_TF_IDENTIFIER;

                    if (currentWord == "true" || currentWord == "false") {
                        style = SCE_TF_BOOL;
                    }
                    else if (currentWord == "null") {
                        style = SCE_TF_NULL;
                    }
                    else if (IsKeyword(currentWord)) {
                        style = SCE_TF_KEYWORD;
                        expectingBlockName = true;
                    }
                    else if (IsType(currentWord)) {
                        style = SCE_TF_TYPE;
                    }
                    else if (IsMetaArgument(currentWord)) {
                        style = SCE_TF_META_ARGUMENT;
                    }
                    else if (IsBuiltinFunction(currentWord)) {
                        // Check if followed by (
                        Sci_Position lookAhead = i;
                        while (lookAhead < docLength && (buffer[lookAhead] == ' ' || buffer[lookAhead] == '\t')) {
                            lookAhead++;
                        }
                        if (lookAhead < docLength && buffer[lookAhead] == '(') {
                            style = SCE_TF_BUILTIN_FUNCTION;
                        }
                    }
                    else if (currentWord == "var" || currentWord == "local" ||
                             currentWord == "data" || currentWord == "module" ||
                             currentWord == "each" || currentWord == "count" ||
                             currentWord == "self" || currentWord == "path") {
                        // Check if followed by .
                        if (i < docLength && buffer[i] == '.') {
                            style = SCE_TF_VARIABLE_REF;
                        }
                    }

                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - wordStart), static_cast<char>(style));
                }
                else if (IsDigit(ch) || (ch == '-' && IsDigit(chNext))) {
                    // Number
                    Sci_Position numStart = i;
                    if (ch == '-') i++;

                    // Check for hex
                    if (i + 1 < docLength && buffer[i] == '0' && (buffer[i+1] == 'x' || buffer[i+1] == 'X')) {
                        i += 2;
                        while (i < docLength && IsHexDigit(buffer[i])) i++;
                    }
                    else {
                        while (i < docLength && IsDigit(buffer[i])) i++;
                        // Decimal part
                        if (i < docLength && buffer[i] == '.') {
                            i++;
                            while (i < docLength && IsDigit(buffer[i])) i++;
                        }
                        // Exponent
                        if (i < docLength && (buffer[i] == 'e' || buffer[i] == 'E')) {
                            i++;
                            if (i < docLength && (buffer[i] == '+' || buffer[i] == '-')) i++;
                            while (i < docLength && IsDigit(buffer[i])) i++;
                        }
                    }
                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - numStart), SCE_TF_NUMBER);
                }
                else if (ch == '$' && chNext == '{') {
                    // Interpolation start
                    state = SCE_TF_INTERPOLATION;
                    interpolationDepth++;
                    pAccess->SetStyleFor(2, SCE_TF_INTERPOLATION);
                    i += 2;
                }
                else if (ch == '%' && chNext == '{') {
                    // Directive start
                    state = SCE_TF_INTERPOLATION;
                    interpolationDepth++;
                    pAccess->SetStyleFor(2, SCE_TF_INTERPOLATION);
                    i += 2;
                }
                else if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '(' || ch == ')') {
                    pAccess->SetStyleFor(1, SCE_TF_BRACKET);
                    if (ch == '{') bracketDepth++;
                    else if (ch == '}') {
                        bracketDepth--;
                        if (interpolationDepth > 0 && bracketDepth < interpolationDepth) {
                            interpolationDepth--;
                        }
                    }
                    i++;
                }
                else if (IsOperator(ch)) {
                    // Handle multi-character operators
                    Sci_Position opStart = i;
                    if ((ch == '=' && chNext == '=') ||
                        (ch == '!' && chNext == '=') ||
                        (ch == '<' && chNext == '=') ||
                        (ch == '>' && chNext == '=') ||
                        (ch == '&' && chNext == '&') ||
                        (ch == '|' && chNext == '|') ||
                        (ch == '=' && chNext == '>') ||
                        (ch == '-' && chNext == '>') ||
                        (ch == '.' && chNext == '.') ) {
                        i += 2;
                        if (ch == '.' && chNext == '.' && i < docLength && buffer[i] == '.') {
                            i++; // ...
                        }
                    }
                    else {
                        i++;
                    }
                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - opStart), SCE_TF_OPERATOR);
                }
                else if (ch == '\n' || ch == '\r') {
                    pAccess->SetStyleFor(1, SCE_TF_DEFAULT);
                    i++;
                    atLineStart = true;
                    expectingBlockName = false;
                }
                else {
                    pAccess->SetStyleFor(1, SCE_TF_DEFAULT);
                    i++;
                }
                break;

            case SCE_TF_COMMENT_LINE:
                if (ch == '\n' || ch == '\r') {
                    state = SCE_TF_DEFAULT;
                    pAccess->SetStyleFor(1, SCE_TF_DEFAULT);
                    i++;
                }
                else {
                    pAccess->SetStyleFor(1, SCE_TF_COMMENT_LINE);
                    i++;
                }
                break;

            case SCE_TF_COMMENT_BLOCK:
                if (ch == '*' && chNext == '/') {
                    pAccess->SetStyleFor(2, SCE_TF_COMMENT_BLOCK);
                    i += 2;
                    state = SCE_TF_DEFAULT;
                }
                else {
                    pAccess->SetStyleFor(1, SCE_TF_COMMENT_BLOCK);
                    i++;
                }
                break;

            case SCE_TF_STRING:
                if (ch == '\\' && chNext != '\0') {
                    // Escape sequence
                    pAccess->SetStyleFor(2, SCE_TF_ESCAPE_CHAR);
                    i += 2;
                }
                else if (ch == '$' && chNext == '{') {
                    // Interpolation in string
                    pAccess->SetStyleFor(2, SCE_TF_INTERPOLATION);
                    interpolationDepth++;
                    i += 2;
                    // Stay in string state but track interpolation
                }
                else if (ch == '%' && chNext == '{') {
                    // Directive in string
                    pAccess->SetStyleFor(2, SCE_TF_INTERPOLATION);
                    interpolationDepth++;
                    i += 2;
                }
                else if (ch == '"') {
                    pAccess->SetStyleFor(1, SCE_TF_STRING);
                    i++;
                    state = SCE_TF_DEFAULT;
                }
                else if (ch == '\n' || ch == '\r') {
                    // Unterminated string
                    pAccess->SetStyleFor(1, SCE_TF_STRING);
                    i++;
                    state = SCE_TF_DEFAULT;
                }
                else {
                    pAccess->SetStyleFor(1, SCE_TF_STRING);
                    i++;
                }
                break;

            case SCE_TF_HEREDOC:
                {
                    // Look for end delimiter
                    bool foundEnd = false;
                    if (atLineStart || ch == '\n') {
                        if (ch == '\n') {
                            pAccess->SetStyleFor(1, SCE_TF_HEREDOC);
                            i++;
                        }
                        // Skip leading whitespace (for <<-)
                        Sci_Position linePos = i;
                        while (linePos < docLength && (buffer[linePos] == ' ' || buffer[linePos] == '\t')) {
                            linePos++;
                        }

                        // Check for delimiter
                        if (!heredocDelimiter.empty()) {
                            bool match = true;
                            for (size_t d = 0; d < heredocDelimiter.length() && match; d++) {
                                if (linePos + d >= static_cast<size_t>(docLength) || buffer[linePos + d] != heredocDelimiter[d]) {
                                    match = false;
                                }
                            }
                            if (match) {
                                // Check that delimiter is followed by newline or end
                                Sci_Position afterDelim = linePos + static_cast<Sci_Position>(heredocDelimiter.length());
                                if (afterDelim >= docLength || buffer[afterDelim] == '\n' || buffer[afterDelim] == '\r') {
                                    // Found end - style the whitespace and delimiter
                                    if (linePos > i) {
                                        pAccess->SetStyleFor(static_cast<Sci_Position>(linePos - i), SCE_TF_HEREDOC);
                                    }
                                    i = linePos;
                                    pAccess->SetStyleFor(static_cast<Sci_Position>(heredocDelimiter.length()), SCE_TF_HEREDOC);
                                    i += static_cast<Sci_Position>(heredocDelimiter.length());
                                    state = SCE_TF_DEFAULT;
                                    heredocDelimiter.clear();
                                    foundEnd = true;
                                }
                            }
                        }
                    }

                    if (!foundEnd) {
                        if (ch == '\n') {
                            atLineStart = true;
                        }
                        else {
                            pAccess->SetStyleFor(1, SCE_TF_HEREDOC);
                            i++;
                            atLineStart = false;
                        }
                    }
                }
                break;

            case SCE_TF_INTERPOLATION:
                if (ch == '}') {
                    pAccess->SetStyleFor(1, SCE_TF_INTERPOLATION);
                    i++;
                    interpolationDepth--;
                    if (interpolationDepth <= 0) {
                        interpolationDepth = 0;
                        state = SCE_TF_DEFAULT;
                    }
                }
                else if (ch == '"') {
                    // String inside interpolation
                    pAccess->SetStyleFor(1, SCE_TF_STRING);
                    i++;
                    // Handle nested string
                    while (i < docLength && buffer[i] != '"') {
                        if (buffer[i] == '\\' && i + 1 < docLength) {
                            pAccess->SetStyleFor(2, SCE_TF_ESCAPE_CHAR);
                            i += 2;
                        }
                        else {
                            pAccess->SetStyleFor(1, SCE_TF_STRING);
                            i++;
                        }
                    }
                    if (i < docLength) {
                        pAccess->SetStyleFor(1, SCE_TF_STRING);
                        i++;
                    }
                }
                else if (IsIdentifierStart(ch)) {
                    currentWord.clear();
                    Sci_Position wordStart = i;
                    while (i < docLength && IsIdentifierChar(buffer[i])) {
                        currentWord += buffer[i];
                        i++;
                    }

                    int style = SCE_TF_IDENTIFIER;
                    if (IsBuiltinFunction(currentWord)) {
                        Sci_Position lookAhead = i;
                        while (lookAhead < docLength && (buffer[lookAhead] == ' ' || buffer[lookAhead] == '\t')) {
                            lookAhead++;
                        }
                        if (lookAhead < docLength && buffer[lookAhead] == '(') {
                            style = SCE_TF_BUILTIN_FUNCTION;
                        }
                    }
                    else if (currentWord == "var" || currentWord == "local" ||
                             currentWord == "data" || currentWord == "module" ||
                             currentWord == "each" || currentWord == "count") {
                        style = SCE_TF_VARIABLE_REF;
                    }
                    else if (currentWord == "true" || currentWord == "false") {
                        style = SCE_TF_BOOL;
                    }
                    else if (currentWord == "null") {
                        style = SCE_TF_NULL;
                    }
                    else if (currentWord == "for" || currentWord == "in" ||
                             currentWord == "if" || currentWord == "else" ||
                             currentWord == "endif" || currentWord == "endfor") {
                        style = SCE_TF_KEYWORD;
                    }

                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - wordStart), static_cast<char>(style));
                }
                else if (IsDigit(ch)) {
                    Sci_Position numStart = i;
                    while (i < docLength && (IsDigit(buffer[i]) || buffer[i] == '.')) i++;
                    pAccess->SetStyleFor(static_cast<Sci_Position>(i - numStart), SCE_TF_NUMBER);
                }
                else if (ch == '{') {
                    interpolationDepth++;
                    pAccess->SetStyleFor(1, SCE_TF_BRACKET);
                    i++;
                }
                else if (IsOperator(ch)) {
                    pAccess->SetStyleFor(1, SCE_TF_OPERATOR);
                    i++;
                }
                else if (ch == '(' || ch == ')' || ch == '[' || ch == ']') {
                    pAccess->SetStyleFor(1, SCE_TF_BRACKET);
                    i++;
                }
                else {
                    pAccess->SetStyleFor(1, SCE_TF_INTERPOLATION);
                    i++;
                }
                break;

            default:
                state = SCE_TF_DEFAULT;
                pAccess->SetStyleFor(1, SCE_TF_DEFAULT);
                i++;
                break;
        }

        if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t') {
            atLineStart = false;
        }
    }

    // Save line state
    if (i > 0) {
        Sci_Position line = pAccess->LineFromPosition(i - 1);
        int lineState = (interpolationDepth & 0xFF) | ((bracketDepth & 0xFF) << 8);
        pAccess->SetLineState(line, lineState);
    }
}

void TerraformLexer::FoldDoc(Sci_Position startPos, Sci_Position length, int, Scintilla::IDocument *pAccess) {
    Sci_Position endPos = startPos + length;
    const char* buffer = pAccess->BufferPointer();
    Sci_Position docLength = pAccess->Length();

    Sci_Position lineCurrent = pAccess->LineFromPosition(startPos);
    int levelCurrent = SC_FOLDLEVELBASE;

    if (lineCurrent > 0) {
        levelCurrent = pAccess->GetLevel(lineCurrent - 1) >> 16;
    }

    int levelNext = levelCurrent;
    Sci_Position lineStart = pAccess->LineStart(lineCurrent);

    for (Sci_Position i = startPos; i < endPos && i < docLength; ) {
        char ch = buffer[i];

        if (ch == '{' || ch == '[') {
            levelNext++;
        }
        else if (ch == '}' || ch == ']') {
            levelNext--;
        }

        if (ch == '\n' || i == endPos - 1 || i == docLength - 1) {
            int lev = levelCurrent | (levelNext << 16);
            if (levelNext > levelCurrent) {
                lev |= SC_FOLDLEVELHEADERFLAG;
            }
            if (lev != pAccess->GetLevel(lineCurrent)) {
                pAccess->SetLevel(lineCurrent, lev);
            }
            lineCurrent++;
            lineStart = pAccess->LineStart(lineCurrent);
            levelCurrent = levelNext;
        }
        i++;
    }
}

int SCI_METHOD TerraformLexer::Version() const {
    return Scintilla::dvRelease5;
}

void SCI_METHOD TerraformLexer::Release() {
    delete this;
}

const char * SCI_METHOD TerraformLexer::PropertyNames() {
    return "";
}

int SCI_METHOD TerraformLexer::PropertyType(const char *) {
    return 0;
}

const char * SCI_METHOD TerraformLexer::DescribeProperty(const char *) {
    return "";
}

Sci_Position SCI_METHOD TerraformLexer::PropertySet(const char *, const char *) {
    return 0;
}

const char * SCI_METHOD TerraformLexer::DescribeWordListSets() {
    return "Keywords Types Functions MetaArguments";
}

Sci_Position SCI_METHOD TerraformLexer::WordListSet(int n, const char *wl) {
    if (wl == nullptr) return 0;

    std::set<std::string>* targetSet = nullptr;
    switch (n) {
        case 0: targetSet = &keywords; break;
        case 1: targetSet = &types; break;
        case 2: targetSet = &builtinFunctions; break;
        case 3: targetSet = &metaArguments; break;
        default: return 0;
    }

    targetSet->clear();
    std::string word;
    while (*wl) {
        if (*wl == ' ' || *wl == '\n' || *wl == '\r' || *wl == '\t') {
            if (!word.empty()) {
                targetSet->insert(word);
                word.clear();
            }
        }
        else {
            word += *wl;
        }
        wl++;
    }
    if (!word.empty()) {
        targetSet->insert(word);
    }

    return 0;
}

void SCI_METHOD TerraformLexer::Lex(Sci_Position startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) {
    ColouriseDoc(startPos, lengthDoc, initStyle, pAccess);
}

void SCI_METHOD TerraformLexer::Fold(Sci_Position startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) {
    FoldDoc(startPos, lengthDoc, initStyle, pAccess);
}

void * SCI_METHOD TerraformLexer::PrivateCall(int, void *) {
    return nullptr;
}

int SCI_METHOD TerraformLexer::LineEndTypesSupported() {
    return 0;
}

int SCI_METHOD TerraformLexer::AllocateSubStyles(int, int) {
    return -1;
}

int SCI_METHOD TerraformLexer::SubStylesStart(int) {
    return -1;
}

int SCI_METHOD TerraformLexer::SubStylesLength(int) {
    return 0;
}

int SCI_METHOD TerraformLexer::StyleFromSubStyle(int subStyle) {
    return subStyle;
}

int SCI_METHOD TerraformLexer::PrimaryStyleFromStyle(int style) {
    return style;
}

void SCI_METHOD TerraformLexer::FreeSubStyles() {
}

void SCI_METHOD TerraformLexer::SetIdentifiers(int, const char *) {
}

int SCI_METHOD TerraformLexer::DistanceToSecondaryStyles() {
    return 0;
}

const char * SCI_METHOD TerraformLexer::GetSubStyleBases() {
    return "";
}

int SCI_METHOD TerraformLexer::NamedStyles() {
    return 21;
}

const char * SCI_METHOD TerraformLexer::NameOfStyle(int style) {
    switch (style) {
        case SCE_TF_DEFAULT: return "Default";
        case SCE_TF_COMMENT_LINE: return "Line Comment";
        case SCE_TF_COMMENT_BLOCK: return "Block Comment";
        case SCE_TF_STRING: return "String";
        case SCE_TF_HEREDOC: return "Heredoc";
        case SCE_TF_NUMBER: return "Number";
        case SCE_TF_KEYWORD: return "Keyword";
        case SCE_TF_ATTRIBUTE: return "Attribute";
        case SCE_TF_IDENTIFIER: return "Identifier";
        case SCE_TF_OPERATOR: return "Operator";
        case SCE_TF_BRACKET: return "Bracket";
        case SCE_TF_TYPE: return "Type";
        case SCE_TF_BUILTIN_FUNCTION: return "Built-in Function";
        case SCE_TF_INTERPOLATION: return "Interpolation";
        case SCE_TF_VARIABLE_REF: return "Variable Reference";
        case SCE_TF_BOOL: return "Boolean";
        case SCE_TF_NULL: return "Null";
        case SCE_TF_BLOCK_TYPE: return "Block Type";
        case SCE_TF_META_ARGUMENT: return "Meta-argument";
        case SCE_TF_PROVISIONER: return "Provisioner";
        case SCE_TF_ESCAPE_CHAR: return "Escape Character";
        default: return "Unknown";
    }
}

const char * SCI_METHOD TerraformLexer::TagsOfStyle(int style) {
    switch (style) {
        case SCE_TF_COMMENT_LINE:
        case SCE_TF_COMMENT_BLOCK: return "comment";
        case SCE_TF_STRING:
        case SCE_TF_HEREDOC: return "literal string";
        case SCE_TF_NUMBER: return "literal number";
        case SCE_TF_KEYWORD:
        case SCE_TF_BLOCK_TYPE: return "keyword";
        case SCE_TF_TYPE: return "type";
        case SCE_TF_BUILTIN_FUNCTION: return "function";
        case SCE_TF_OPERATOR: return "operator";
        case SCE_TF_BOOL:
        case SCE_TF_NULL: return "literal";
        default: return "";
    }
}

const char * SCI_METHOD TerraformLexer::DescriptionOfStyle(int style) {
    return NameOfStyle(style);
}

const char * SCI_METHOD TerraformLexer::GetName() {
    return "terraform";
}

int SCI_METHOD TerraformLexer::GetIdentifier() {
    return SCLEX_TERRAFORM;
}

const char * SCI_METHOD TerraformLexer::PropertyGet(const char *) {
    return "";
}

Scintilla::ILexer5* TerraformLexer::LexerFactory() {
    return new TerraformLexer();
}
