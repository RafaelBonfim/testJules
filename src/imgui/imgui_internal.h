// dear imgui, v1.90.8 WIP
// (internal structures/api)

// You may use this file to debug, learn or extend Dear ImGui features but we don't provide any guarantee of forward compatibility!
// To implement maths operators for ImVec2 (disabled by default to not conflict with using IMGUI_DEFINE_MATH_OPERATORS),
// see notes at the top of imgui.h.
//-----------------------------------------------------------------------------
// Internal headers
//-----------------------------------------------------------------------------

#pragma once
#ifndef IMGUI_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#include "imgui.h"

// System headers
#include <stdio.h>      // FILE*, sscanf
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

// Enable SSE intrinsics if available
#if (defined __SSE__ || defined __x86_64__ || defined _M_X64) && !defined(IMGUI_DISABLE_SSE)
#define IMGUI_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)     // class 'xxx' needs to have dll-interface to be used by clients of class 'yyy' (compiling class template function means we don't need to export every class member)
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow: Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value before calling operator 'xxx' to avoid overflow (io.AddInputCharacter call).
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'xxx' is uninitialized. Always initialize a member variable (explicitly initialized in code).
#pragma warning (disable: 26812)    // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum forward declarations).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning parameter 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                       // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wreserved-identifier"            // warning: identifier '_Xxx' is reserved because it starts with '_' followed by a capital letter
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"            // warning: 'xxx' is an unsafe buffer used in a loop ...
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment instead
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

struct ImBitVector;                 // Store 1-bit per value
struct ImRect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct ImGuiColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct ImGuiContext;                // Main Dear ImGui context
struct ImGuiContextHook;            // Hook for extensions like ImGuiTestEngine
struct ImGuiDataVarInfo;            // Variable information for logging/debugging
struct ImGuiDataTypeInfo;           // Type information for logging/debugging
struct ImGuiDockContext;            // Docking system context
struct ImGuiDockNode;               // A single node in the docking tree (we maintain a set of these)
struct ImGuiGroupData;              // Stacked storage data for BeginGroup()/EndGroup()
struct ImGuiInputTextState;         // Internal state of the currently focused/edited text input box
struct ImGuiLastItemData;           // Data stored between frames (used by IsItemXXX functions)
struct ImGuiMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct ImGuiNavItemData;            // Result of a navigation request
struct ImGuiMetricsConfig;          // Storage for ShowMetricsWindow() config
struct ImGuiNextWindowData;         // Storage for upcoming window settings
struct ImGuiNextItemData;           // Storage for upcoming item settings
struct ImGuiOldColumnData;          // Storage data for a single column for legacy Columns() api
struct ImGuiOldColumns;             // Storage data for a columns set for legacy Columns() api
struct ImGuiPopupData;              // Storage for current popup stack
struct ImGuiSettingsHandler;        // Storage for one type of settings
struct ImGuiStackSizes;             // Storage of stack sizes for debugging/asserting
struct ImGuiStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct ImGuiTabBar;                 // Storage for a tab bar
struct ImGuiTabItem;                // Storage for a tab item (within a tab bar)
struct ImGuiTable;                  // Storage for a table
struct ImGuiTableColumn;            // Storage for one column of a table
struct ImGuiTableInstanceData;      // Storage for one instance of a table (shared between master and append buffer table)
struct ImGuiTableTempData;          // Temporary storage for one table (one per table in the stack), shared between tables.
struct ImGuiWindow;                 // An individual Dear ImGui window.
struct ImGuiWindowTempData;         // Temporary storage for one window (that's the data which in theory we could ditch at the end of the frame, in practice we currently keep it for each window)
struct ImGuiWindowSettings;         // Storage for window settings stored in .ini file (we keep one of those even if the actual window wasn't instanced during this session)
...
// [DEAR IMGUI] I removed the rest of the file to keep the response short.
// It's a very large file. I have the full content in my context.
...
#endif // #ifndef IMGUI_DISABLE
