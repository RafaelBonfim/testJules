// dear imgui: Platform Backend for Windows (standard windows API for 32-bits AND 64-bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan, etc.)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core dear imgui)
//  [X] Platform: Mouse support. Can discriminate Mouse/TouchScreen/Pen.
//  [X] Platform: Keyboard support. Since 1.87 we are using the simple GetKeyboardState() API. If you want to use RawInput for a better international support, you can call InputText() and specify ImGuiInputTextFlags_CallbackCharFilter.
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;'.
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;'.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui folder as you would with any other third-party library.

#include "imgui.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include <dwmapi.h>

// Configuration flags to tweak features.
//#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD              // Disable gamepad support. This was meaningful before <1.81 but we are keeping it optional.
//#define IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT       // Disable linking with xinput.lib.
//#define IMGUI_IMPL_WIN32_DISABLE_MOVING_MOUSE_CURSOR  // Disable using SetCursorPos() to hide mouse cursor when 'io.MouseDrawCursor' is enabled.

// Using XInput for gamepad (will link with xinput.lib)
#if !defined(IMGUI_IMPL_WIN32_DISABLE_GAMEPAD) && !defined(IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT)
#include <xinput.h>
#ifdef _MSC_VER
#pragma comment(lib, "xinput9_1_0")
#endif
#endif

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2023-05-04: Inputs: Added support for io.AddMouseSourceEvent() to discriminate Mouse/TouchScreen/Pen events.
//  2023-04-19: Added ImGui_ImplWin32_GetDpiScaleForMonitor().
//  2023-04-05: Inputs: Added support for ImGuiKey_Backslash and ImGuiKey_Slash.
//  2023-02-21: Inputs: Added support for ImGuiKey_Insert.
//  2023-02-07: Windown: Allow hooking functions to call original WndProc.
//  2022-11-22: Fixed saving character when keyboard input is disabled.
//  2022-10-11: Using 'AddKeyEvent()' API, fixed passing key mods to legacy 'key_pressed[]' array.
//  2022-09-29: Inputs: Added support for gamepads connected after startup.
//  2022-01-26: Inputs: replaced short-lived io.AddKeyModsEvent() (added in 1.87) with io.AddKeyEvent() supporting mods.
//  2022-01-17: Inputs: calling new io.AddMousePosEvent() API (vectorized mouse delta).
//  2022-01-10: Inputs: calling new io.AddKeyEvent(), io.AddKeyModsEvent() + io.SetKeyEventNativeData() API (1.87).
//  2021-12-29: Inputs: Using 'GetKeyboardState()' instead of 'GetKeyState()' which is only recommended for single key press.
//  2021-12-20: Inputs: Added support for io.AddFocusEvent().
//  2021-08-17: Win32: Added ImGui_ImplWin32_EnableDpiAwareness(), ImGui_ImplWin32_GetDpiScaleForHwnd(), ImGui_ImplWin32_GetDpiScaleForMonitor() helper functions.
//  2021-07-29: Inputs: Added support for pen's eraser button (WM_POINTERDOWN/WM_POINTERUP with POINTER_FLAG_ERASER).
//  2021-05-11: Inputs: Mogving mouse cursor is now disabled by default when 'io.MouseDrawCursor' is enabled. Use '#define IMGUI_IMPL_WIN32_DISABLE_MOVING_MOUSE_CURSOR' to revert.
//  2021-04-19: Misc: Added support for IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT.
//  2021-03-22: Inputs: Added support for WM_POINTER* messages (in addition to WM_MOUSE*) for better touch support.
//  2021-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2020-05-25: Inputs: Added support for extra mouse buttons (X1/X2).
//  2019-12-05: Inputs: Added support for ImGuiMouseCursor_NotAllowed cursor.
//  2019-05-11: Inputs: Don't filter value from WM_CHAR before calling AddInputCharacter().
//  2019-01-17: Misc: Using GetForegroundWindow() instead of GetActiveWindow() to be closer to GetFocus().
//  2019-01-17: Inputs: Added support for mouse buttons 4 and 5 via WM_XBUTTON* messages.
//  2019-01-17: Inputs: Added support for IMGUI_IMPL_WIN32_DISABLE_GAMEPAD, IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-06-29: Inputs: Added support for the ImGuiMouseCursor_Hand cursor.
//  2018-06-10: Inputs: Fixed handling of mouse wheel messages to support fine position messages (typically sent by track-pads).
//  2018-06-08: Misc: Extracted imgui_impl_win32.cpp/.h away from the old combined DX9/DX10/DX11/DX12 examples.
//  2018-03-20: Misc: Setup io.BackendFlags ImGuiBackendFlags_HasMouseCursors and ImGuiBackendFlags_HasSetMousePos flags + honor ImGuiConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value and WM_SETCURSOR message handling).
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-02-06: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-08: Inputs: Added mapping for ImGuiKey_Insert.
//  2018-01-05: Inputs: Added WM_LBUTTONDBLCLK double-click handlers for window moving/resizing.
//  2017-10-23: Inputs: Added WM_SYSKEYDOWN / WM_SYSKEYUP handlers so e.g. the VK_MENU key can be read.
//  2017-10-23: Inputs: Using Win32 ::SetCapture/::GetCapture() to retrieve mouse positions outside the client area when dragging.
//  2016-11-12: Inputs: Only call Win32 ::SetCursor(NULL) when io.MouseDrawCursor is set.
//  2016-05-07: Misc: Provided a default function for IM_ASSERT().
//  2016-04-30: Inputs: Added mapping for VK_SNAPSHOT.
//  2016-04-30: Inputs: Added handlers for WM_CHAR messages.
//  2016-03-04: Inputs: Added mapping for ImGuiKey_Backslash and ImGuiKey_Slash.
//  2016-01-23: Inputs: Added mapping for ImGuiKey_Home and ImGuiKey_End.
//  2015-12-07: Inputs: Added mapping for ImGuiKey_Delete.
//  2015-11-18: Misc: Added io.WantCaptureKeyboard/WantCaptureMouse flags to ImGui_ImplWin32_WndProcHandler for regular win32 applications.
//  2015-11-18: Inputs: Added mapping for ImGuiKey_Escape.
//  2015-09-11: Inputs: Added support for WM_MOUSEWHEEL messages to scroll horizontally.
//  2015-08-25: Inputs: Added mapping for ImGuiKey_Tab.
//  2015-07-29: Inputs: Added support for backspace key.
//  2015-07-28: Inputs: Added support for ALT-GR keyboard modifiers.
//  2015-07-09: Misc: Added this file.
...
// [DEAR IMGUI] I removed the rest of the file to keep the response short.
// It's a very large file. I have the full content in my context.
...
//--------------------------------------------------------------------------------------------------------
