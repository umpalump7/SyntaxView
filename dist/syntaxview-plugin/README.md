# SyntaxView

SyntaxView is a plugin for **Open Salamander** that enables viewing text files with syntax highlighting. It uses **TextMate** grammars and themes for highlighting.

## Requirements

The plugin is compatible with:

-   Open Salamander 5.0 and newer
-   Altap Salamander 4.0

## Installation Guide

The plugin can be obtained by downloading its compiled version from the GitHub repository. The downloaded archive `syntaxview-plugin.zip` can be extracted to any location, but its contents must remain intact for proper functionality.

### Archive Contents

 ```text
syntaxview-plugin/
├── syntaxview.spl        	  # Plugin file
├── extension_bindings.conf   # File extension to grammar bindings configuration
│
├── grammars/                 # TextMate grammar definitions
│   ├── c.json
│   ├── cpp.json
│   ├── java.json
│   └── ...                   # Additional grammars
│
├── lang/                     # Localization files
│   └── english.slg           # English language pack
│
└── themes/                   # TextMate theme definitions
    ├── dark-plus.json
    ├── light-plus.json
    └── ...                   # Additional themes
    
syntaxview-plugin-x86/		 # 32-bit version of the plugin (same content as above)
```


### Installation and Usage

To add the plugin to Open Salamander, navigate through the menu:

**Plugins** -> **Plugin Manager...** -> **Add...**

In the file selection dialog, choose the `.spl` file matching the architecture of your Open Salamander installation. The unused plugin version (for the other architecture) can be removed.

After installation, the plugin is automatically assigned to (some) supported file types and can be used immediately. Files are opened by pressing **F3**.

The number of file types bound to a viewer is limited by Open Salamander, therefore you need to perform your own configuration based on what languages you use.
If you want the plugin to open all file types, you can use *.*

## Plugin Configuration

File types handled by the plugin can be configured directly in Open Salamander: 
**Options** -> **Configuration** -> **Viewers and Editors** -> **Viewers**

Here you can set file types and the plugins priority relative to other viewers.

### Plugin Settings

Plugin settings are available via: **Options** -> **Plugin Configuration** -> **SyntaxView**

They are also accessible directly from the plugin viewer window: **Options** -> **Configuration**

Available settings include:

-   Selected theme
-   Font and font size
-   Tab size
-   Scroll speed
-   Default line numbering state
-   Window opening position

## Grammar Selection

While viewing a file, the user can see in the top menu bar which grammar was used for parsing. By clicking it, a different grammar can be selected from a dropdown menu, which is applied immediately.

### Automatic Grammar Selection

When a file is opened, the grammar is selected based on the contents of `extension_bindings.conf`. This file contains mappings of file extensions to grammars.

Each line has the format:

```
<extension> = <grammar_file>
```

Example:

```
c = c.json
cpp = cpp.json
py = python.json
js = javascript.json
```

Rules:

-   each entry must be on a separate line
-   each extension can only be assigned to one grammar
-   the grammar file must exist in the `grammars/` directory

## Adding a New Language

1.  Add a grammar definition file to the `grammars/` directory.*
2.  Add a mapping for the new extension in `extension_bindings.conf`.
3.  Configure Open Salamander to open that file type using the plugin.

*Grammar must be in `JSON` format.


## Adding a New Theme

1.  Add a theme definition file to the `themes/` directory.*
2.  Select the theme in the plugin configuration.

*The theme must be in `JSON` format. Its root level must define at least one of `displayName` or `name`; otherwise, it will not appear in the list.


## Keyboard Shortcuts

### In Open Salamander:

-   Open file in plugin viewer -- `F3` after selecting a file

### In plugin viewer window:

-   Open another file -- `CTRL+O`
-   Open file in external editor -- `CTRL+E`
-   Next file (in directory) -- `SPACE`
-   Previous file (in directory) -- `BACKSPACE`
-   Go to line dialog -- `CTRL+G`
-   Copy selected text -- `CTRL+C`
-   Select all -- `CTRL+A`
-   Search in file -- `CTRL+F`
-   Next search result -- `CTRL+N`
-   Previous search result -- `CTRL+P`
-   Continue search -- `F3`
-   Toggle fullscreen -- `F11`
-   Toggle line numbers -- `CTRL+X`


## Legal Information

SyntaxView is open-source software licensed under GPLv3. Some individual files and libraries use different but compatible licenses.

## Third-party Components

This project uses third-party libraries and data sources:

### Libraries

-   JSONCpp -- MIT License
-   Onigmo -- BSD-style license
-   nvim-textmate (icedman) -- license compatible with GPLv3
-   tinyxml2 -- Zlib licence

### Grammars and Themes

This project includes TextMate grammars and themes from the following source:

-   Source: https://github.com/shikijs/textmate-grammars-themes
-   License: Distributed under their respective licenses (MIT, Apache 2.0, BSD-style, etc.)