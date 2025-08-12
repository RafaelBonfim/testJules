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

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

IMGUI_IMPL_API bool     ImGui_ImplWin32_Init(void* hwnd);
IMGUI_IMPL_API void     ImGui_ImplWin32_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplWin32_NewFrame();

// Win32 message handler your application need to call.
// - You should ALT-TAB to your window or click minimize and restore to make sure window handles are update.
// - 'hwnd' is checked against the last known window handle to avoid passing events to windows that are not targeted by Dear ImGui.
// - 'wparam' is one of those Win32 types that is 32-bit on 32-bit systems and 64-bit on 64-bit systems.
//   We use 'WPARAM' instead of 'uintptr_t' to avoid soft-casting errors.
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// DPI-related functions (optional)
// - Call ImGui_ImplWin32_EnableDpiAwareness() before creating your window.
// - Call ImGui_ImplWin32_GetDpiScaleForHwnd() on window creation and every WM_DPICHANGED event.
// - Call ImGui_ImplWin32_GetDpiScaleForMonitor() to query DPI of a given monitor.
IMGUI_IMPL_API void     ImGui_ImplWin32_EnableDpiAwareness();
IMGUI_IMPL_API float    ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd);
IMGUI_IMPL_API float    ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor);

// IME-related functions (optional)
// - Call ImGui_ImplWin32_EnableIme() before creating your window.
IMGUI_IMPL_API void     ImGui_ImplWin32_EnableIme();
