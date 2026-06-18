# Terraform/HCL Syntax Highlighting Plugin for Notepad++

A Notepad++ plugin that provides syntax highlighting for Terraform (`.tf`, `.tfvars`) and HCL (`.hcl`) files using the ILexer5 interface.

## Features

- Full Terraform/HCL syntax highlighting with:
  - Block types: `resource`, `variable`, `output`, `locals`, `module`, `data`, `provider`, `terraform`
  - Data types: `string`, `number`, `bool`, `list`, `map`, `set`, `object`, `tuple`
  - Built-in functions (100+ functions)
  - String interpolation (`${...}`)
  - Heredoc strings (`<<EOF`)
  - Comments (line `#`, `//` and block `/* */`)
  - Meta-arguments: `count`, `for_each`, `depends_on`, `lifecycle`
  - Operators and brackets
- Code folding support
- Automatic syntax detection for `.tf`, `.tfvars`, and `.hcl` files

## Building

### Prerequisites

- Visual Studio 2019 or later (with C++ Desktop Development workload)
- Windows SDK 10.0

### Build Steps

1. Open `vs.proj\TerraformSyntax.vcxproj` in Visual Studio
2. Select the desired configuration:
   - `Release|x64` for 64-bit Notepad++
   - `Release|Win32` for 32-bit Notepad++
   - `Release|ARM64` for ARM64 Notepad++
3. Build the solution (Ctrl+Shift+B)
4. The compiled DLL will be in `bin64\` (x64), `bin\` (x86), or `arm64\` (ARM64)

## Installation

1. Build the plugin or download a pre-built release
2. Copy `TerraformSyntax.dll` to your Notepad++ plugins directory:
   - For 64-bit: `%ProgramFiles%\Notepad++\plugins\TerraformSyntax\`
   - For 32-bit: `%ProgramFiles(x86)%\Notepad++\plugins\TerraformSyntax\`
3. Restart Notepad++

## Usage

The plugin automatically detects and applies syntax highlighting to files with extensions:
- `.tf` (Terraform configuration)
- `.tfvars` (Terraform variables)
- `.hcl` (HashiCorp Configuration Language)

You can also manually apply the syntax highlighting via:
**Plugins > Terraform Syntax > Apply Terraform Syntax**

## Styles

The plugin provides highlighting for:

| Style | Description | Default Color |
|-------|-------------|---------------|
| Default | Regular text | Black |
| Comment | Line and block comments | Green (italic) |
| String | String literals | Dark Red |
| Heredoc | Heredoc strings | Dark Red (light bg) |
| Number | Numeric literals | Orange |
| Keyword | Block types | Blue (bold) |
| Type | Data types | Teal |
| Function | Built-in functions | Purple |
| Interpolation | `${...}` | Orange-Red (bold) |
| Variable Ref | `var.xxx`, `local.xxx` | Cyan |
| Boolean | `true`, `false` | Blue |
| Null | `null` | Gray (italic) |
| Operator | Operators | Black |
| Bracket | Brackets | Dark Red (bold) |

## Project Structure

```
npp-Terraform-Syntax/
├── src/
│   ├── ILexer.h              # Scintilla ILexer5 interface
│   ├── TerraformLexer.h      # Lexer header
│   ├── TerraformLexer.cpp    # Lexer implementation
│   ├── PluginDefinition.h    # Plugin definition header
│   ├── PluginDefinition.cpp  # Plugin implementation
│   ├── NppTerraformPlugin.cpp# Plugin entry point
│   ├── PluginInterface.h     # Notepad++ plugin interface
│   ├── Notepad_plus_msgs.h   # Notepad++ messages
│   ├── Scintilla.h           # Scintilla definitions
│   ├── Sci_Position.h        # Scintilla position types
│   ├── menuCmdID.h           # Menu command IDs
│   └── TerraformSyntax.rc    # Resource file
└── vs.proj/
    └── TerraformSyntax.vcxproj # Visual Studio project
```

## License

This project is licensed under the GNU General Public License v3.0.

## Acknowledgments

- Based on the Notepad++ Plugin Template
- Uses Scintilla's ILexer5 interface for syntax highlighting
