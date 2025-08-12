//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR IMGUI
// Runtime options (clipboard callbacks, enabling various features, etc.) can be set at runtime via the ImGuiIO structure in imgui.cpp.
//-----------------------------------------------------------------------------
// A) You may edit this file to compile Dear ImGui with custom options.
// B) or you can define the symbols below in your project/build system, which will override the settings in this file.
// C) or you can use cmake ../ -DIMGUI_OPTION_XXX=OFF to override options for cmake builds.
//-----------------------------------------------------------------------------

#pragma once

//-----[===============]
//-----[ PLATFORM/OS ]
//-----[===============]

// You can define your own platform/OS identifiers, as long as IMGUI_PLATFORM_NAME is defined and the code compiles.
// Note that IMGUI_PLATFORM_NAME is used to compile specific code sections, e.g. for console output.
//#define IMGUI_PLATFORM_NAME         "my_platform"
//#define IMGUI_PLATFORM_HAS_WIN32_IME    // Defined internally on Windows, but you can define it on other platforms if you have IME functions.

//-----[====================]
//-----[ COMPILER SETTINGS ]
//-----[====================]

// User-provided defines and settings can be automatically applied by including imconfig_user.h.
// As with all options in imconfig.h, you can set them from the build system instead.
//#define IMGUI_USER_CONFIG           "my_imgui_config.h"

//-----[================]
//-----[ MEMORY/ALLOC ]
//-----[================]

// All memory allocation calls use IM_ALLOC()/IM_FREE().
// By default, they are #define'd to malloc()/free() of <stdlib.h>.
// You can override them to use your own memory allocator.
// All allocations are made with IM_NEW() and IM_DELETE() which call IM_ALLOC()/IM_FREE() and placement new/delete.
// This is to allow IM_ALLOC() to return allocated memory that is not aligned for placement new (e.g. if you have a memory allocator that returns pointer+size, rather than just a pointer).
//#define IM_ALLOC(_SIZE)             malloc(_SIZE)
//#define IM_FREE(_PTR)               free(_PTR)
// #define IM_PLACEMENT_NEW(_PTR)      new(_PTR)

//-----[================]
//-----[ SECURITY ]
//-----[================]

// Define IMGUI_DISABLE_FILE_FUNCTIONS to disable FILE* functions such as ImGui::LoadIniSettingsFromDisk(), ImGui::SaveIniSettingsToDisk(), etc.
// As a corollary, this will also disable all ".ini" file functions.
//#define IMGUI_DISABLE_FILE_FUNCTIONS

// Define IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS to disable the default implementation of the file functions.
// You can then provide your own io.SetClipboardTextFn / io.GetClipboardTextFn pointers to your own functions.
// This is useful if you have custom file functions but want to use the default ".ini" file handling functionality.
//#define IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS

//-----[================]
//-----[ DEBUG/TESTING ]
//-----[================]

// Pass -DIMGUI_DEBUG_TEST_ENGINE to cmake to build with the test engine.
// This is automatically done by the build if the test engine is present.
//#ifndef IMGUI_DEBUG_TEST_ENGINE
//#define IMGUI_DEBUG_TEST_ENGINE
//#endif

//-----[ নাশ ]
//-----[ API ]
//-----[ নাশ ]

// Define IMGUI_API to specify linkage attributes for Dear ImGui functions.
// (e.g. "static" to build as a single file unit, or "__declspec(dllexport)" to export from a DLL).
// We define it by default to nothing so that it's convenient to compile as a single unit.
#ifndef IMGUI_API
#define IMGUI_API
#endif

//-----[=======================]
//-----[ FONT ATLAS / RASTER ]
//-----[=======================]

// By default, Dear ImGui uses stb_truetype to rasterize fonts.
// You can override the implementation by defining IMGUI_ENABLE_FREETYPE and linking with the freetype library.
// (This is NOT recommended, stb_truetype is much easier to integrate and needs no configuration).
//#define IMGUI_ENABLE_FREETYPE

//-----[====================]
//-----[ RENDERER BACKENDS ]
//-----[====================]

// Define IMGUI_IMPL_OPENGL_LOADER_CUSTOM to use your own OpenGL loader (e.g. glad, gl3w, etc.).
// You can use the IMGUI_IMPL_OPENGL_LOADER_DUMMY option to not use any loader.
//#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM         "my_opengl_loader.h"

//-----[==================]
//-----[ MISCELLANEOUS ]
//-----[==================]

// Define IMGUI_DISABLE_OBSOLETE_FUNCTIONS to remove functions that are marked obsolete in the code.
// You can still see the function declarations in the code if you want to update your code to use the newer API.
//#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// Define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS to disable the default implementation of clipboard functions for Windows.
// You can then provide your own io.SetClipboardTextFn / io.GetClipboardTextFn pointers to your own functions.
//#define IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS

// Define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS to disable the default implementation of IME functions for Windows.
// You can then provide your own io.ImeSetInputScreenPosFn / io.ImeWindowHandle pointers to your own functions.
//#define IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS

//-----[================]
//-----[ EXPERIMENTAL ]
//-----[================]

// Define IMGUI_HAS_VIEWPORT to enable the multi-viewport feature.
// See comments in imgui.cpp for details.
//#define IMGUI_HAS_VIEWPORT

// Define IMGUI_HAS_DOCK to enable the docking feature.
// See comments in imgui.cpp for details.
//#define IMGUI_HAS_DOCK

//-----[================]
//-----[ END OF FILE ]
//-----[================]
