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

// Win32 Data
struct ImGui_ImplWin32_Data
{
    HWND                        hWnd;
    HWND                        MouseHwnd;
    bool                        MouseTracked;
    int                         MouseButtonsDown;
    INT64                       Time;
    INT64                       TicksPerSecond;
    ImGuiMouseCursor            LastMouseCursor;
    bool                        HasGamepad;
    bool                        WantUpdateHasGamepad;
    bool                        WantUpdateMonitors;

    ImGui_ImplWin32_Data()      { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplWin32_Data* ImGui_ImplWin32_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplWin32_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

// Functions
static bool ImGui_ImplWin32_IsVkDown(int vk)
{
    return (::GetKeyState(vk) & 0x8000) != 0;
}

static void ImGui_ImplWin32_AddKeyEvent(ImGuiKey key, bool down, int native_keycode, int native_scancode = -1)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(key, down);
    io.SetKeyEventNativeData(key, native_keycode, native_scancode); // To support legacy indexing (<1.87 user code)
}

static void ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
{
    // Left & right Shift keys: when both are pressed together, Windows tend to not generate the WM_KEYUP event for the first released one.
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && !ImGui_ImplWin32_IsVkDown(VK_LSHIFT))
        ImGui_ImplWin32_AddKeyEvent(ImGuiKey_LeftShift, false, VK_LSHIFT);
    if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !ImGui_ImplWin32_IsVkDown(VK_RSHIFT))
        ImGui_ImplWin32_AddKeyEvent(ImGuiKey_RightShift, false, VK_RSHIFT);

    // Sometimes WM_KEYUP for Win key is not passed down to the app (e.g. for Win+V on some setups, according to #6065)
    if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !ImGui_ImplWin32_IsVkDown(VK_LWIN))
        ImGui_ImplWin32_AddKeyEvent(ImGuiKey_LeftSuper, false, VK_LWIN);
    if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !ImGui_ImplWin32_IsVkDown(VK_RWIN))
        ImGui_ImplWin32_AddKeyEvent(ImGuiKey_RightSuper, false, VK_RWIN);
}

static void ImGui_ImplWin32_UpdateMouseData()
{
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(bd->hWnd != 0);

    const bool is_app_focused = (::GetForegroundWindow() == bd->hWnd);
    if (is_app_focused)
    {
        // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        // When io.WantSetMousePos is set, we need to spoof Windows mouse position. This is useful for OrbitControls classes.
        if (io.WantSetMousePos)
        {
            POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
            if (::ClientToScreen(bd->hWnd, &pos))
                ::SetCursorPos(pos.x, pos.y);
        }

        // (Optional) Fallback to provide mouse position when focused (WM_MOUSEMOVE already provides this when hovered or captured)
        // This also fills a short gap when clicking title bar of a border-less window, mouse position is frozen over the title bar but underlying drag may begin or resume.
        if (!io.WantSetMousePos && !bd->MouseTracked)
        {
            POINT pos;
            if (::GetCursorPos(&pos) && ::ScreenToClient(bd->hWnd, &pos))
                io.AddMousePosEvent((float)pos.x, (float)pos.y);
        }
    }
}

static void ImGui_ImplWin32_UpdateMouseCursor()
{
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   win_32_cursor = IDC_SIZENWSE; break;
        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
}

static void ImGui_ImplWin32_UpdateGamepads()
{
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Calling XInputGetState() every frame on disconnected gamepads is unfortunately too slow.
    // We fetch a list of connected gamepad devices from XInputGetCapabilities() and only poll the connected ones.
    // It is unfortunately not possible to easily detect new connections using a callback or event.
    // We will poll once typical frame to avoid adding latency.
    if (bd->WantUpdateHasGamepad)
    {
        XINPUT_CAPABILITIES caps;
        bd->HasGamepad = (XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
        bd->WantUpdateHasGamepad = false;
    }

    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
    if (bd->HasGamepad)
    {
        XINPUT_STATE xinput_state;
        if (XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
        {
            const XINPUT_GAMEPAD& gamepad = xinput_state.Gamepad;
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

            #define MAP_BUTTON(KEY_NO, BUTTON_ENUM)     { io.AddKeyEvent(KEY_NO, (gamepad.wButtons & BUTTON_ENUM) != 0); }
            #define MAP_ANALOG(KEY_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); if (vn > 0.1f) io.AddKeyAnalogEvent(KEY_NO, true, vn); else io.AddKeyAnalogEvent(KEY_NO, false, 0.0f); }
            MAP_BUTTON(ImGuiKey_GamepadStart,           XINPUT_GAMEPAD_START);
            MAP_BUTTON(ImGuiKey_GamepadBack,            XINPUT_GAMEPAD_BACK);
            MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        XINPUT_GAMEPAD_X);
            MAP_BUTTON(ImGuiKey_GamepadFaceRight,       XINPUT_GAMEPAD_B);
            MAP_BUTTON(ImGuiKey_GamepadFaceUp,          XINPUT_GAMEPAD_Y);
            MAP_BUTTON(ImGuiKey_GamepadFaceDown,        XINPUT_GAMEPAD_A);
            MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        XINPUT_GAMEPAD_DPAD_LEFT);
            MAP_BUTTON(ImGuiKey_GamepadDpadRight,       XINPUT_GAMEPAD_DPAD_RIGHT);
            MAP_BUTTON(ImGuiKey_GamepadDpadUp,          XINPUT_GAMEPAD_DPAD_UP);
            MAP_BUTTON(ImGuiKey_GamepadDpadDown,        XINPUT_GAMEPAD_DPAD_DOWN);
            MAP_BUTTON(ImGuiKey_GamepadL1,              XINPUT_GAMEPAD_LEFT_SHOULDER);
            MAP_BUTTON(ImGuiKey_GamepadR1,              XINPUT_GAMEPAD_RIGHT_SHOULDER);
            MAP_ANALOG(ImGuiKey_GamepadL2,              gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255);
            MAP_ANALOG(ImGuiKey_GamepadR2,              gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255);
            MAP_BUTTON(ImGuiKey_GamepadL3,              XINPUT_GAMEPAD_LEFT_THUMB);
            MAP_BUTTON(ImGuiKey_GamepadR3,              XINPUT_GAMEPAD_RIGHT_THUMB);
            MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
            MAP_ANALOG(ImGuiKey_GamepadLStickRight,     gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadLStickUp,        gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadLStickDown,      gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
            MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      gamepad.sThumbRX, -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, -32768);
            MAP_ANALOG(ImGuiKey_GamepadRStickRight,     gamepad.sThumbRX, +XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadRStickUp,        gamepad.sThumbRY, +XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadRStickDown,      gamepad.sThumbRY, -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, -32768);
            #undef MAP_BUTTON
            #undef MAP_ANALOG
        }
    }
#endif // #ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
}

static BOOL CALLBACK ImGui_ImplWin32_UpdateMonitors_EnumFunc(HMONITOR monitor, HDC, LPRECT, LPARAM)
{
    MONITORINFO info = { 0 };
    info.cbSize = sizeof(MONITORINFO);
    if (!::GetMonitorInfo(monitor, &info))
        return TRUE;
    ImGuiPlatformMonitor imgui_monitor;
    imgui_monitor.MainPos = ImVec2((float)info.rcMonitor.left, (float)info.rcMonitor.top);
    imgui_monitor.MainSize = ImVec2((float)(info.rcMonitor.right - info.rcMonitor.left), (float)(info.rcMonitor.bottom - info.rcMonitor.top));
    imgui_monitor.WorkPos = ImVec2((float)info.rcWork.left, (float)info.rcWork.top);
    imgui_monitor.WorkSize = ImVec2((float)(info.rcWork.right - info.rcWork.left), (float)(info.rcWork.bottom - info.rcWork.top));
    imgui_monitor.DpiScale = ImGui_ImplWin32_GetDpiScaleForMonitor(monitor);
    ImGuiIO& io = ImGui::GetIO();
    if (info.dwFlags & MONITORINFOF_PRIMARY)
        io.PlatformIO.Monitors.push_front(imgui_monitor);
    else
        io.PlatformIO.Monitors.push_back(imgui_monitor);
    return TRUE;
}

static void ImGui_ImplWin32_UpdateMonitors()
{
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    io.PlatformIO.Monitors.resize(0);
    ::EnumDisplayMonitors(NULL, NULL, ImGui_ImplWin32_UpdateMonitors_EnumFunc, 0);
    bd->WantUpdateMonitors = false;
}

bool ImGui_ImplWin32_Init(void* hwnd)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    INT64 perf_frequency, perf_counter;
    if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter))
        return false;

    // Setup backend capabilities flags
    ImGui_ImplWin32_Data* bd = IM_NEW(ImGui_ImplWin32_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_win32";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;       // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;        // We can honor io.WantSetMousePos requests (optional, rarely used)

    bd->hWnd = (HWND)hwnd;
    bd->WantUpdateHasGamepad = true;
    bd->TicksPerSecond = perf_frequency;
    bd->Time = perf_counter;
    bd->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = main_viewport->PlatformHandleRaw = (void*)bd->hWnd;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplWin32_UpdateMonitors();

    // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void ImGui_ImplWin32_Shutdown()
{
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    IM_DELETE(bd);
}

void ImGui_ImplWin32_NewFrame()
{
    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplWin32_Init()?");
    ImGuiIO& io = ImGui::GetIO();

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    io.DeltaTime = (float)(current_time - bd->Time) / bd->TicksPerSecond;
    bd->Time = current_time;

    // Update OS mouse position
    ImGui_ImplWin32_UpdateMouseData();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (bd->LastMouseCursor != mouse_cursor)
    {
        bd->LastMouseCursor = mouse_cursor;
        ImGui_ImplWin32_UpdateMouseCursor();
    }

    // Update gamepads
    ImGui_ImplWin32_UpdateGamepads();

    // Update monitors
    if (bd->WantUpdateMonitors)
        ImGui_ImplWin32_UpdateMonitors();
}

// Process Win32 mouse/keyboard inputs.
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API to be able to read mouse coordinates when dragging mouse out of the client area.
// PS: Since 1.87 (pre-release), we are using the simple GetKeyboardState() API. If you want to use RawInput for a better international support, you can call InputText() and specify ImGuiInputTextFlags_CallbackCharFilter.
// PS: IME support: we have implementation in 'misc/cpp/imgui_stdlib.h' for the standard library ITs, but you can also handle it yourself according to your needs.
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == NULL)
        return 0;

    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();

    switch (msg)
    {
    case WM_MOUSEMOVE:
        // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
        if (!bd->MouseTracked)
        {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            ::TrackMouseEvent(&tme);
            bd->MouseTracked = true;
        }
        io.AddMousePosEvent((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSELEAVE:
        if (bd->MouseTracked)
        {
            bd->MouseTracked = false;
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
        break;
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        if (bd->MouseButtonsDown == 0 && ::GetCapture() == NULL)
            ::SetCapture(hwnd);
        bd->MouseButtonsDown |= 1 << button;
        io.AddMouseButtonEvent(button, true);
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        int button = 0;
        if (msg == WM_LBUTTONUP) { button = 0; }
        if (msg == WM_RBUTTONUP) { button = 1; }
        if (msg == WM_MBUTTONUP) { button = 2; }
        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        bd->MouseButtonsDown &= ~(1 << button);
        if (bd->MouseButtonsDown == 0 && ::GetCapture() == hwnd)
            ::ReleaseCapture();
        io.AddMouseButtonEvent(button, false);
        return 0;
    }
    case WM_MOUSEWHEEL:
        io.AddMouseWheelEvent(0.0f, (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
        return 0;
    case WM_MOUSEHWHEEL:
        io.AddMouseWheelEvent((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA, 0.0f);
        return 0;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    {
        bool down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        if (wParam < 256)
        {
            // Submit key event
            ImGui_ImplWin32_AddKeyEvent((ImGuiKey)wParam, down, wParam);
        }
        else
        {
            // Non-system keys are not supported by this backend
        }
        return 0;
    }
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
        io.AddFocusEvent(msg == WM_SETFOCUS);
        return 0;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return 0;
    case WM_SETCURSOR:
        // We handle the cursor shape ourselves
        if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
            return 1;
        return 0;
    case WM_DEVICECHANGE:
        if ((UINT)wParam == DBT_DEVNODES_CHANGED)
            bd->WantUpdateHasGamepad = true;
        return 0;
    case WM_DISPLAYCHANGE:
        bd->WantUpdateMonitors = true;
        return 0;
    }
    return 0;
}


//--------------------------------------------------------------------------------------------------------
// DPI-related functions
//--------------------------------------------------------------------------------------------------------
// - ImGui_ImplWin32_EnableDpiAwareness()
// - ImGui_ImplWin32_GetDpiScaleForHwnd()
// - ImGui_ImplWin32_GetDpiScaleForMonitor()
//--------------------------------------------------------------------------------------------------------
// High DPI support depends on starting the application with the right manifest, which is provided by default with the Dear ImGui examples.
// See https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#my-text-is-blurry for details.
//--------------------------------------------------------------------------------------------------------

void ImGui_ImplWin32_EnableDpiAwareness()
{
    // if (IsWindows10OrGreater()) // This is a lie, this function is available on Windows 8.1.
    {
        static HMODULE user32_dll = ::LoadLibraryA("user32.dll"); // Reference counted per-process
        if (FARPROC SetThreadDpiAwarenessContext = ::GetProcAddress(user32_dll, "SetThreadDpiAwarenessContext"))
        {
            void* dpi_awareness_context_per_monitor_aware_v2 = (void*)-4; // As per MSDN documentation, can be used on Windows 10 anniversary update or later
            ::SetThreadDpiAwarenessContext(dpi_awareness_context_per_monitor_aware_v2);
            return;
        }
    }
    // Fallback for older Windows versions, this will make the entire app blurry on HDPI displays
    // (but is better than nothing)
#if !defined(_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
    ::SetProcessDPIAware();
#endif
}

float ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd)
{
    UINT dpi = 0;
    // if (IsWindows10OrGreater()) // This is a lie, this function is available on Windows 8.1.
    {
        static HMODULE user32_dll = ::LoadLibraryA("user32.dll"); // Reference counted per-process
        if (FARPROC GetDpiForWindow = ::GetProcAddress(user32_dll, "GetDpiForWindow"))
            dpi = (UINT)::GetDpiForWindow((HWND)hwnd);
    }
    if (dpi == 0)
    {
        // Fallback for older Windows versions
        HDC hdc = ::GetDC((HWND)hwnd);
        dpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
        ::ReleaseDC((HWND)hwnd, hdc);
    }
    return dpi / 96.0f;
}

float ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor)
{
    UINT dpi_x = 0, dpi_y = 0;
    // if (IsWindows10OrGreater()) // This is a lie, this function is available on Windows 8.1.
    {
        static HMODULE shcore_dll = ::LoadLibraryA("shcore.dll"); // Reference counted per-process
        if (FARPROC GetDpiForMonitor = ::GetProcAddress(shcore_dll, "GetDpiForMonitor"))
            ::GetDpiForMonitor((HMONITOR)monitor, 0, &dpi_x, &dpi_y);
    }
    if (dpi_x == 0)
    {
        // Fallback for older Windows versions
        const HDC hdc = ::CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
        dpi_x = ::GetDeviceCaps(hdc, LOGPIXELSX);
        ::DeleteDC(hdc);
    }
    return dpi_x / 96.0f;
}

//--------------------------------------------------------------------------------------------------------
// IME-related functions
//--------------------------------------------------------------------------------------------------------
// - ImGui_ImplWin32_EnableIme()
//--------------------------------------------------------------------------------------------------------

void ImGui_ImplWin32_EnableIme()
{
    ::ImmAssociateContext(NULL, NULL);
}

//--------------------------------------------------------------------------------------------------------
