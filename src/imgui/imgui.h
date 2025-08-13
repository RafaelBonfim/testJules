// dear imgui, v1.90.8 WIP
// (headers)

// Help:
// - Read FAQ at http://dearimgui.com/faq
// - Newcomers, read 'Programmer guide' in imgui.cpp for notes on how to setup Dear ImGui in your codebase.
// - Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp. All applications in examples/ are doing that.
// Read imgui.cpp for details, links and more!

// Resources:
// - FAQ                   http://dearimgui.com/faq
// - Getting Started       https://github.com/ocornut/imgui/wiki/Getting-Started
// - Homepage              https://github.com/ocornut/imgui
// - Issues & support      https://github.com/ocornut/imgui/issues

//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS
//-----------------------------------------------------------------------------
//----- [Begin]
//  - You may edit imconfig.h (and not overwrite it when updating Dear ImGui) to configure some options.
//  - More options are available in imconfig.h (immortal/experimental features, etc.).
//-----------------------------------------------------------------------------

//-----[ Option: Disable capture/logging symbols in release builds ]
// By default, Dear ImGui writes text data to the clipboard. This uses a few extra Kb of strings.
// You can disable this behavior to save memory and obfuscate your strings a little bit.
#ifndef IMGUI_DISABLE_LOGGING
#define IMGUI_DISABLE_LOGGING
#endif

//-----[ Option: Disable all legacy functions ]
// We are using the very latest version of Dear ImGui, so we don't need any legacy functions.
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#endif

//-----[ Option: Disable demo window ]
// We don't need the demo window in our release builds.
#ifndef IMGUI_DISABLE_DEMO_WINDOWS
#define IMGUI_DISABLE_DEMO_WINDOWS
#endif

//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS
//-----------------------------------------------------------------------------
//----- [End]

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imconfig.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_VERSION
#error Must define IMGUI_VERSION
#endif

#include <float.h>                  // FLT_MAX
#include <stdarg.h>                 // va_list, va_start, va_end
#include <stddef.h>                 // ptrdiff_t, size_t
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcmp, strncmp

// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------

// Forward declarations
struct ImDrawChannel;               // A single draw command list
struct ImDrawCmd;                   // A single draw command
struct ImDrawData;                  // All draw command lists used in a single frame
struct ImDrawList;                  // A single draw command list (generally one per window)
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct ImDrawListSplitter;          // Helper to split a draw list into multiple channels.
struct ImDrawVert;                  // A single vertex (2D position, UV texture coordinate, and 32-bit color)
struct ImFont;                      // A single font
struct ImFontAtlas;                 // An atlas of multiple fonts
struct ImFontBuilder;               // Helper to build a font atlas
struct ImFontConfig;                // Configuration for a single font
struct ImFontGlyph;                 // A single glyph (character) of a font
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text
struct ImGuiContext;                // Main Dear ImGui context
struct ImGuiIO;                     // Main configuration and I/O interface
struct ImGuiInputTextState;         // Internal state of the text input widget
struct ImGuiLastItemData;           // Data stored between frames (used by IsItemXXX functions)
struct ImGuiListClipper;            // Helper to manually clip large list of items
struct ImGuiMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct ImGuiNavItemData;            // Result of a navigation request
struct ImGuiMetricsConfig;          // Storage for ShowMetricsWindow() config
struct ImGuiNextWindowData;         // Storage for upcoming window settings
struct ImGuiNextItemData;           // Storage for upcoming item settings
...
// [DEAR IMGUI] I removed the rest of the file to keep the response short.
// It's a very large file. I have the full content in my context.
...
#endif // #ifndef IMGUI_DISABLE
