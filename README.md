# SyntaxView

SyntaxView is a plugin for Open Salamander that enables viewing text files with syntax highlighting. It uses TextMate grammars and themes for highlighting.


# Basic Directory Description

The plugin fully prepared for installation and use is located in the `dist/syntaxview-plugin/` directory.
The manual containing installation and usage instructions is located in the `dist/syntaxview-plugin/README.md` file.

The source code is located in the `src/` directory.
The implementation of the plugin is located in the `src/plugins/syntax-viewer` directory.
The implementation of the TextMate module is located in the `src/msvc-textmate` directory.

# Structure of the Repository

```text
SyntaxView/
│
├── dist/                             # Distribution version
│ 	├── syntaxview-plugin/			  # Complete plugin ready for use (64-bit version)
│	│	├── README.md         		  # Directory and file description, installation and usage manual (EN)
│	│	├── README_sk.md      		  # Slovak version of README.md
│	│	└── ...
│	└── syntaxview-plugin-x86/		  # Complete plugin ready for use (32-bit version)
│
│
├── src/							  # Complete source code of the project
│   ├── msvc-textmate/                # TextMate module
│   │   ├── build-msvc/				  # Visual Studio files for compilation
│   │   │   ├── textmate.sln          # Solution for the TextMate module
│   │   │   ├── textmate.vcxproj	  # Project for the TextMate parser
│   │   │   ├── onigmolib.vcxproj	  # Project for the Onigmo library
│   │   │   └── ...
│   │   │
│   │   ├── libs/					  # Libraries used by the TextMate module
│   │   │   ├── jsoncpp/			  # Source code of the JsonCpp library
│   │   │   ├── Onigmo/				  # Source code of the Onigmo library
│   │   │   ├── tinyxml2/			  # Source code of the tinyxml2 library
│   │   │   └── tm-parser/			  # Source code of the TextMate parser
│   │   └── ...
│   │
│   └── plugins/                      # Plugin
│       ├── shared/                   # Shared dependencies from the Open Salamander SDK
│       └── syntax-viewer/            # Syntax View plugin
│           ├── vcxproj/        
│           │   ├── syntaxview.sln	  # Solution for the SyntaxView plugin (main project)
│           │   └── ...				  # Other projects belonging to the solution
│           │
│           ├── *.cpp, *.h			  		# Source code of the plugin
│           ├── *.rc, *.rc2, *.rh, *.rh2	# Plugin resources
│           └── ...
│
└── README.md						  # Description of the submitted project
```

# Plugin Compilation

To compile the plugin, it is necessary to open the `src/plugins/syntax-viewer/vcxproj/syntaxview.sln` solution file and use `Build` to first compile the `textmate` project and then the `syntaxview` project. For easier navigation, all other projects that are part of the plugin are also visible in this solution.

The compilation result is copied to the `src/plugins/syntax-viewer/plugin-files/` directory.

To avoid the need for manual solution reconfiguration, the `src` directory must be placed at the path `C:\Projects\SyntaxView\src\`. Due to the generation of the solution from the CMake configuration, it is not possible to correctly relativize the project paths.

# Description of Plugin Source Files

Source files from the `src/plugins/syntax-view/` directory:

| File                              | Purpose / Description                               |
| --------------------------------- | --------------------------------------------------- |
| `dialogs.cpp`, `dialogs.h`        | implementation of plugin dialog windows             |
| `find.cpp`, `find.h`              | functionality of the search dialog                  |
| `helper.cpp`, `helper.h`          | helper utilities and shared functions               |
| `searcher.cpp`, `searcher.h`      | searching and processing of search results          |
| `syntaxview.cpp`, `syntaxview.h`  | main part of the plugin interface, registry storage |
| `textmodel.cpp`, `textmodel.h`    | internal data/text model of the displayed file      |
| `viewer.cpp`                      | content rendering and display, UI implementation    |
| `syntaxview.rc`, `syntaxview.rc2` | plugin resource configuration                       |
| `syntaxview.rh`, `syntaxview.rh2` | resource definitions                                |
| `version.h2`                      | plugin version definition                           |

# Authorship of Implementation Parts

## src/msvc-textmate/

The implementation in the `msvc-textmate/` directory is based on the original GitHub project `icedman/nvim-textmate`. Modifications to the retained source code in this part consisted of several dozen lines of code.

The following changes were made:

* removal of parts related to the Neovim editor
* preservation of the original CMake configuration and libraries
* creation of new Visual Studio solution and project files
* implementation of multiple logical fixes and implementation adjustments

Changes in the used libraries:

| Component   | Scope of modifications                           |
| ----------- | ------------------------------------------------ |
| `tm-parser` | parser fixes -- processing and internal behavior |
| `jsoncpp`   | no changes made                                  |
| `tinyxml2`  | no changes made                                  |
| `onigmo`    | minor modifications, mainly for UTF-16 support   |

## src/plugins/

The `shared/` directory is part of the Open Salamander SDK, and no changes were made to its files.

The `syntax-viewer/` directory contains the implementation of the plugin, which was originally based on the `demoview` plugin template from the Open Salamander SDK. Most of the original files were significantly modified and extended.


# Legal Information

SyntaxView is open-source software licensed under GPLv3. Some individual files and libraries use different but compatible licenses.

# Third-party Components

This project uses third-party libraries and data sources:

## Libraries

-   JSONCpp -- MIT License
-   Onigmo -- BSD-style license
-   nvim-textmate (icedman) -- license compatible with GPLv3
-   tinyxml2 -- Zlib licence

## Grammars and Themes

This project includes TextMate grammars and themes from the following source:

-   Source: https://github.com/shikijs/textmate-grammars-themes
-   License: Distributed under their respective licenses (MIT, Apache 2.0, BSD-style, etc.)