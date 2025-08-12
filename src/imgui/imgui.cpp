// dear imgui, v1.90.8 WIP
// (main source code)

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
// Tip: To quickly find the documentation for a function, search for the function
// name with a trailing parenthesis, e.g. 'search: MyFunction('.
//-----------------------------------------------------------------------------

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

// System includes
#include <ctype.h>      // toupper
#include <stdio.h>      // vsnprintf, sscanf, printf
#if defined(_WIN32)
#include <windows.h>    // FindFirstFileA, FindNextFileA
#endif

// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

#if !defined(IMGUI_DISABLE_OBSOLETE_FUNCTIONS) && defined(IMGUI_DISABLE_TEST_WINDOWS) && !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#define IMGUI_DISABLE_DEMO_WINDOWS
#endif

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
static void ShowDemoWindow(bool* p_open);
#endif

#if !defined(IMGUI_DISABLE_METRICS_WINDOW)
static void ShowMetricsWindow(bool* p_open);
#endif

#if !defined(IMGUI_DISABLE_DEBUG_TOOLS)
static void ShowDebugLogWindow(bool* p_open);
static void ShowIDStackToolWindow(bool* p_open);
static void ShowAboutWindow(bool* p_open);
#endif

#if !defined(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
namespace ImGui
{
    // OBSOLETED in 1.89 (from September 2022)
    IMGUI_API void          SetItemDefaultFocus();
    IMGUI_API bool          IsItemHovered(ImGuiHoveredFlags flags);
    // OBSOLETED in 1.88 (from August 2022)
    IMGUI_API bool          IsAnyWindowFocused();
    // OBSOLETED in 1.86 (from October 2021)
    IMGUI_API bool          IsWindowContentHoverable(ImGuiWindow* window, ImGuiHoveredFlags flags);
    // OBSOLETED in 1.84 (from August 2021)
    IMGUI_API void          TreeAdvanceToLabelPos();
    // OBSOLETED in 1.82 (from April 2021)
    IMGUI_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback = NULL, void* custom_callback_data = NULL);
    // OBSOLETED in 1.81 (from February 2021)
    IMGUI_API void          SetNextTreeNodeOpen(bool is_open, ImGuiCond cond = 0);
    // OBSOLETED in 1.80 (from January 2021)
    IMGUI_API void          Value(const char* prefix, bool b);
    IMGUI_API void          Value(const char* prefix, int v);
    IMGUI_API void          Value(const char* prefix, unsigned int v);
    IMGUI_API void          Value(const char* prefix, float v, const char* float_format = NULL);
    // OBSOLETED in 1.79 (from October 2020)
    IMGUI_API void          Indent();
    IMGUI_API void          Unindent();
    // OBSOLETED in 1.78 (from August 2020)
    IMGUI_API void          LabelText(const char* label, const char* fmt, ...) IM_FMTARGS(2);
    // OBSOLETED in 1.76 (from June 2020)
    IMGUI_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    IMGUI_API void          PopClipRect();
    // OBSOLETED in 1.75 (from May 2020)
    IMGUI_API bool          IsWindowAppearing();
    // OBSOLETED in 1.73 (from April 2020)
    IMGUI_API bool          GetContentRegionAvail(ImVec2* p_out);
    // OBSOLETED in 1.72 (from March 2020)
    IMGUI_API ImDrawList*   GetOverlayDrawList();
    // OBSOLETED in 1.71 (from February 2020)
    IMGUI_API bool          IsAnyItemActive();
    // OBSOLETED in 1.70 (from January 2020)
    IMGUI_API bool          IsAnyItemFocused();
    // OBSOLETED in 1.69 (from January 2019)
    IMGUI_API void          ShowFontSelector(const char* label);
    // OBSOLETED in 1.67 (from October 2018)
    IMGUI_API void          SetNextWindowContentSize(const ImVec2& size);
    // OBSOLETED in 1.66 (from September 2018)
    IMGUI_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);
    // OBSOLETED in 1.64 (from August 2018)
    IMGUI_API void          BeginGroup();
    IMGUI_API void          EndGroup();
    // OBSOLETED in 1.61 (from March 2018)
    IMGUI_API bool          GetWindowContentRegionMin(ImVec2* p_out);
    IMGUI_API bool          GetWindowContentRegionMax(ImVec2* p_out);
    // OBSOLETED in 1.60 (from February 2018)
    IMGUI_API void          ListBoxHeader(const char* label, int items_count, int height_in_items = -1);
    IMGUI_API void          ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0));
    IMGUI_API void          ListBoxFooter();
    // OBSOLETED in 1.53 (from December 2017)
    IMGUI_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);
    IMGUI_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);
}
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

// Data
static ImGuiContext*    GImGui = NULL;

//-----------------------------------------------------------------------------
// [SECTION] Context creation and access
//-----------------------------------------------------------------------------

// DLL users:
// - Heaps and globals are not shared across DLL boundaries!
// - You will need to call SetCurrentContext() + SetAllocatorFunctions() for each static/DLL boundary you are calling from.
// - Same applies for creating a context on one side of a DLL boundary and passing it to the other side.
// - If you are passing arguments from one side to the other, you are passing pointers to memory allocated on one side.
//   The other side will not be able to free/reallocate them.
//   This is a problem if you are passing ImGuiContext* from one side to the other and destroying it on the other side.
//   This is also a problem if you are passing strings from one side to the other and they are freed on the other side.
//   To solve this, you can:
//   - Use a custom allocator that is shared across DLL boundaries.
//   - Pass a function pointer to a deallocator function to the other side.
//   - Use a memory allocation scheme that is shared across DLL boundaries (e.g. VirtualAlloc/VirtualFree on Windows).
//   - Use a custom string class that is shared across DLL boundaries.
//   - Use a custom vector class that is shared across DLL boundaries.
//   - etc.

IMGUI_API ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas)
{
    ImGuiContext* ctx = IM_NEW(ImGuiContext)(shared_font_atlas);
    if (GImGui == NULL)
        SetCurrentContext(ctx);
    Initialize(ctx);
    return ctx;
}

IMGUI_API void DestroyContext(ImGuiContext* ctx)
{
    if (ctx == NULL)
        ctx = GImGui;
    Shutdown(ctx);
    if (GImGui == ctx)
        SetCurrentContext(NULL);
    IM_DELETE(ctx);
}

IMGUI_API ImGuiContext* GetCurrentContext()
{
    return GImGui;
}

IMGUI_API void SetCurrentContext(ImGuiContext* ctx)
{
#ifdef IMGUI_SET_CURRENT_CONTEXT_FUNC
    IMGUI_SET_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-local storage
#else
    GImGui = ctx;
#endif
}

//-----------------------------------------------------------------------------
// [SECTION] Main
//-----------------------------------------------------------------------------

// The 'master' NewFrame() is in charge of calling all the individual NewFrame() functions.
// This is so we can have a single entry point for the user to call.
void ImGui::NewFrame()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);
    IM_ASSERT(!g.WithinFrameScope && "Forgot to call EndFrame() or Render() at the end of the previous frame?");

    // Call NewFrame() on all windows
    for (int i = 0; i < g.Windows.Size; i++)
        g.Windows[i]->NewFrame();

    // Call NewFrame() on all viewports
    for (int i = 0; i < g.Viewports.Size; i++)
        g.Viewports[i]->NewFrame();

    // Call NewFrame() on all tables
    for (int i = 0; i < g.Tables.Size; i++)
        if (g.Tables.GetByIndex(i)->LastFrameActive != -1)
            g.Tables.GetByIndex(i)->NewFrame();

    // Call NewFrame() on all tab bars
    for (int i = 0; i < g.TabBars.Size; i++)
        if (g.TabBars.GetByIndex(i)->LastFrameVisible != -1)
            g.TabBars.GetByIndex(i)->NewFrame();

    // Call NewFrame() on all popups
    for (int i = 0; i < g.OpenPopupStack.Size; i++)
        g.OpenPopupStack[i].NewFrame();

    // Call NewFrame() on the current drag/drop payload
    if (g.DragDropActive)
        g.DragDropPayload.NewFrame();

    // Call NewFrame() on the current docking context
    if (g.DockContext)
        g.DockContext->NewFrame();

    // Initialize
    g.Time += g.IO.DeltaTime;
    g.WithinFrameScope = true;
    g.FrameCount += 1;
    g.TooltipOverrideCount = 0;
    g.WindowsActiveCount = 0;
    g.MenusIdSubmittedThisFrame.clear();

    // Check and assert for various common programming mistakes
    IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f && "Invalid DisplaySize value!");
    IM_ASSERT(g.IO.DeltaTime >= 0.0f && "Invalid DeltaTime value! See FAQ.");
    IM_ASSERT(g.IO.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");
    IM_ASSERT(g.Style.CurveTessellationTol > 0.0f && "Invalid CurveTessellationTol value!");
    IM_ASSERT(g.Style.CircleTessellationMaxError > 0.0f && "Invalid CircleTessellationMaxError value!");
    IM_ASSERT(g.Style.Alpha >= 0.0f && g.Style.Alpha <= 1.0f && "Invalid GlobalAlpha value!");
    IM_ASSERT(g.Style.WindowMinSize.x >= 1.0f && g.Style.WindowMinSize.y >= 1.0f && "Invalid WindowMinSize value!");

    // Update input tracking
    UpdateInputEvents(g.IO.WantUpdateEvents);

    // Load settings if this is the first frame
    if (g.FrameCount == 1)
    {
        if (g.IO.IniFilename)
            LoadIniSettingsFromDisk(g.IO.IniFilename);
        CallContextHooks(&g, ImGuiContextHookType_NewFramePre);
    }

    // Create a dummy window to hold popups, tooltips, etc.
    // This is required because we can't have a window that is both a popup and a child window.
    if (!g.BackgroundDrawList.VtxBuffer.empty())
        g.BackgroundDrawList.Clear();
    if (!g.ForegroundDrawList.VtxBuffer.empty())
        g.ForegroundDrawList.Clear();
    g.ForegroundDrawList.PushClipRectFullScreen();
    g.BackgroundDrawList.PushClipRectFullScreen();

    // Calculate frame-rate for the user, as a purely luxurious feature
    g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
    g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
    g.FramerateSecPerFrameCount = ImMin(g.FramerateSecPerFrameCount + 1, IM_ARRAYSIZE(g.FramerateSecPerFrame));
    g.FramerateSecPerFrameAccum = 0.0f;
    for (int i = 0; i < g.FramerateSecPerFrameCount; i++)
        g.FramerateSecPerFrameAccum += g.FramerateSecPerFrame[i];

    // Handle user-provided input ownership requested before NewFrame()
    if (g.WantCaptureMouseNextFrame != -1) { g.IO.WantCaptureMouse = g.WantCaptureMouseNextFrame; g.WantCaptureMouseNextFrame = -1; }
    if (g.WantCaptureKeyboardNextFrame != -1) { g.IO.WantCaptureKeyboard = g.WantCaptureKeyboardNextFrame; g.WantCaptureKeyboardNextFrame = -1; }

    // Handle user-provided text input requested before NewFrame()
    if (g.WantTextInputNextFrame != -1) { g.IO.WantTextInput = g.WantTextInputNextFrame; g.WantTextInputNextFrame = -1; }

    // Begin frame
    g.IO.MouseWheel = g.IO.MouseWheelH = 0.0f;
    g.IO.InputQueueCharacters.resize(0);
    g.IO.InputQueueSurrogate = 0;

    // Update mouse capture status
    UpdateHoveredWindowAndCaptureFlags();

    // Update keyboard capture status
    g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (g.NavWindow && g.NavWindow->HasFocus) || (g.OpenPopupStack.Size > 0);

    // Update text input status
    g.IO.WantTextInput = (g.ActiveId != 0 && g.InputTextState.ID == g.ActiveId);

    // Start frame
    if (g.FrameCountEnded != g.FrameCount)
        EndFrame();
    Begin("##Default");
}

// This is the main entry point to all rendering.
// Call after NewFrame() and before Render().
// You can submit rendering primitives into the draw lists below.
void ImGui::Render()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);
    IM_ASSERT(g.WithinFrameScope && "Forgot to call NewFrame()?");

    // End frame
    EndFrame();

    // Gather draw data
    g.DrawDataBuilder.Clear();
    if (!g.BackgroundDrawList.VtxBuffer.empty())
        g.DrawDataBuilder.AddDrawList(&g.BackgroundDrawList);
    if (!g.ForegroundDrawList.VtxBuffer.empty())
        g.DrawDataBuilder.AddDrawList(&g.ForegroundDrawList);

    // Gather draw data from all windows
    for (int i = 0; i < g.Windows.Size; i++)
    {
        ImGuiWindow* window = g.Windows[i];
        if (window->IsVisible() && !window->IsCollapsed())
            g.DrawDataBuilder.AddDrawList(window->DrawList);
    }

    // Render
    if (g.DrawDataBuilder.GetDrawListCount() > 0)
    {
        // Platform dependent render function
        if (g.IO.RenderDrawListsFn != NULL)
            g.IO.RenderDrawListsFn(&g.DrawData);
    }
}

// End frame, render and update inputs.
void ImGui::EndFrame()
{
    ImGuiContext& g = *GImGui;
    IM_ASSERT(g.Initialized);
    IM_ASSERT(g.WithinFrameScope && "Forgot to call NewFrame()?");

    // Don't process EndFrame() multiple times.
    if (g.FrameCountEnded == g.FrameCount)
        return;

    // Notify OS when our Input Method Editor cursor has moved.
    if (g.IO.ImeSetInputScreenPosFn && (g.InputTextState.CursorPosPrev.x != g.InputTextState.CursorPos.x || g.InputTextState.CursorPosPrev.y != g.InputTextState.CursorPos.y))
        g.IO.ImeSetInputScreenPosFn((int)g.InputTextState.CursorPos.x, (int)g.InputTextState.CursorPos.y);

    // Hide implicit/fallback "Debug" window if it hasn't been used
    if (g.FrameCount > 0 && !g.IO.WantSaveIniSettings)
        if (ImGuiWindow* window = FindWindowByName("Debug##Default"))
            if (!window->WriteAccessed)
                window->Hidden = true;

    // Update the next frame capture states.
    g.IO.WantCaptureMouseUnlessPopupClose = (g.HoveredWindow != NULL && !IsPopupOpen(0, ImGuiPopupFlags_AnyPopupLevel)) || (g.ActiveId != 0);

    // Call EndFrame() on all windows
    for (int i = 0; i < g.Windows.Size; i++)
        g.Windows[i]->EndFrame();

    // Call EndFrame() on all viewports
    for (int i = 0; i < g.Viewports.Size; i++)
        g.Viewports[i]->EndFrame();

    // Call EndFrame() on all tables
    for (int i = 0; i < g.Tables.Size; i++)
        if (g.Tables.GetByIndex(i)->LastFrameActive != -1)
            g.Tables.GetByIndex(i)->EndFrame();

    // Call EndFrame() on all tab bars
    for (int i = 0; i < g.TabBars.Size; i++)
        if (g.TabBars.GetByIndex(i)->LastFrameVisible != -1)
            g.TabBars.GetByIndex(i)->EndFrame();

    // Call EndFrame() on all popups
    for (int i = 0; i < g.OpenPopupStack.Size; i++)
        g.OpenPopupStack[i].EndFrame();

    // Call EndFrame() on the current drag/drop payload
    if (g.DragDropActive)
        g.DragDropPayload.EndFrame();

    // Call EndFrame() on the current docking context
    if (g.DockContext)
        g.DockContext->EndFrame();

    // End of frame
    g.WithinFrameScope = false;
    g.FrameCountEnded = g.FrameCount;

    // Save settings if requested
    if (g.IO.WantSaveIniSettings)
    {
        if (g.IO.IniFilename != NULL)
            SaveIniSettingsToDisk(g.IO.IniFilename);
        g.IO.WantSaveIniSettings = false;
    }

    // Garbage collection
    if (g.GcCompactAll)
        GCCompact();
    g.GcCompactAll = false;
}

//-----------------------------------------------------------------------------
// [SECTION] IO
//-----------------------------------------------------------------------------

void ImGui::UpdateInputEvents(bool trickle_fast_inputs)
{
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;

    // Trickle down fast inputs (e.g. mouse moves and mouse wheel) to windows.
    // The other inputs are processed on a per-window basis.
    if (trickle_fast_inputs)
    {
        if (io.MousePos.x != io.MousePosPrev.x || io.MousePos.y != io.MousePosPrev.y)
            for (int i = 0; i < g.Windows.Size; i++)
                if (g.Windows[i]->IsActiveAndVisible())
                    g.Windows[i]->OnMouseMove(io.MousePos);
        if (io.MouseWheel != 0.0f || io.MouseWheelH != 0.0f)
            for (int i = 0; i < g.Windows.Size; i++)
                if (g.Windows[i]->IsActiveAndVisible())
                    g.Windows[i]->OnMouseWheel(io.MouseWheel, io.MouseWheelH);
    }

    // Process other inputs
    for (int i = 0; i < g.Windows.Size; i++)
        if (g.Windows[i]->IsActiveAndVisible())
            g.Windows[i]->UpdateInput();
}

//-----------------------------------------------------------------------------
// [SECTION] Window management
//-----------------------------------------------------------------------------

static void UpdateHoveredWindowAndCaptureFlags()
{
    ImGuiContext& g = *GImGui;

    // Find the window that is hovered by the mouse
    // - Child windows can be hovered even if their parent is not.
    // - We should not hover a window that is generally not interactable.
    g.HoveredWindow = NULL;
    g.HoveredWindowUnderMovingWindow = NULL;
    ImGuiWindow* hovered_window = NULL;
    if (g.IO.MousePos.x >= 0.0f && g.IO.MousePos.y >= 0.0f)
    {
        for (int i = g.Windows.Size - 1; i >= 0; i--)
        {
            ImGuiWindow* window = g.Windows[i];
            if (!window->IsActiveAndVisible())
                continue;
            if (window->Flags & ImGuiWindowFlags_NoMouseInputs)
                continue;

            // Using the clipping rectangle of the window-rendering-only-that-is-a-child-of-the-window-that-is-hovered is not a good idea.
            // It would prevent us from hovering a child window when the mouse is over the parent's title bar.
            ImRect bb(window->Pos, window->Pos + window->Size);
            if (bb.Contains(g.IO.MousePos))
            {
                hovered_window = window;
                break;
            }
        }
    }

    // Update the hovered window
    if (g.HoveredWindow != hovered_window)
    {
        if (g.HoveredWindow)
            g.HoveredWindow->OnMouseLeave();
        g.HoveredWindow = hovered_window;
        if (g.HoveredWindow)
            g.HoveredWindow->OnMouseEnter();
    }

    // Update the hovered window under a moving window
    if (g.MovingWindow)
    {
        g.HoveredWindowUnderMovingWindow = NULL;
        for (int i = g.Windows.Size - 1; i >= 0; i--)
        {
            ImGuiWindow* window = g.Windows[i];
            if (window == g.MovingWindow)
                continue;
            if (!window->IsActiveAndVisible())
                continue;
            if (window->Flags & ImGuiWindowFlags_NoMouseInputs)
                continue;
            ImRect bb(window->Pos, window->Pos + window->Size);
            if (bb.Contains(g.IO.MousePos))
            {
                g.HoveredWindowUnderMovingWindow = window;
                break;
            }
        }
    }

    // Update the mouse capture status
    g.IO.WantCaptureMouse = (g.HoveredWindow != NULL) || (g.ActiveId != 0);
}

//-----------------------------------------------------------------------------
// [SECTION] Style
//-----------------------------------------------------------------------------

const ImGuiStyle& ImGui::GetStyle()
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    return GImGui->Style;
}

ImGuiStyle& ImGui::GetStyle()
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    return GImGui->Style;
}

void ImGui::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGui::StyleColorsClassic(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void ImGui::StyleColorsLight(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.90f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers
//-----------------------------------------------------------------------------

// Helper: Find window given name, optionally focus on it
ImGuiWindow* ImGui::FindWindowByName(const char* name)
{
    ImGuiContext& g = *GImGui;
    ImGuiID id = ImHashStr(name);
    return g.WindowsById.GetVal(id);
}

//-----------------------------------------------------------------------------
// [SECTION] Demo, Debug, Information
//-----------------------------------------------------------------------------

// Demo window logic is here so we can have a single file containing all the demo code.
// It is called by ShowDemoWindow() which is in imgui_demo.cpp.
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ImGui::ShowDemoWindow(bool* p_open)
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    ShowDemoWindow(p_open);
}
#endif

#if !defined(IMGUI_DISABLE_METRICS_WINDOW)
// Metrics window logic is here so we can have a single file containing all the metrics code.
// It is called by ShowMetricsWindow() which is in imgui_metrics.cpp.
void ImGui::ShowMetricsWindow(bool* p_open)
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    ShowMetricsWindow(p_open);
}
#endif

#if !defined(IMGUI_DISABLE_DEBUG_TOOLS)
// Debug log window logic is here so we can have a single file containing all the debug log code.
// It is called by ShowDebugLogWindow() which is in imgui_debug_tools.cpp.
void ImGui::ShowDebugLogWindow(bool* p_open)
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    ShowDebugLogWindow(p_open);
}

// ID Stack Tool window logic is here so we can have a single file containing all the id stack tool code.
// It is called by ShowIDStackToolWindow() which is in imgui_debug_tools.cpp.
void ImGui::ShowIDStackToolWindow(bool* p_open)
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    ShowIDStackToolWindow(p_open);
}

// About window logic is here so we can have a single file containing all the about code.
// It is called by ShowAboutWindow() which is in imgui_debug_tools.cpp.
void ImGui::ShowAboutWindow(bool* p_open)
{
    IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() or ImGui::SetCurrentContext()?");
    ShowAboutWindow(p_open);
}
#endif // #if !defined(IMGUI_DISABLE_DEBUG_TOOLS)

//-----------------------------------------------------------------------------
// [SECTION] Obsolete functions
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// OBSOLETED in 1.89 (from September 2022)
void ImGui::SetItemDefaultFocus()
{
    ImGuiContext& g = *GImGui;
    if (g.CurrentWindow->DC.LastItemId != 0)
        g.NavJustMovedToId = g.CurrentWindow->DC.LastItemId;
}

// OBSOLETED in 1.88 (from August 2022)
bool ImGui::IsAnyWindowFocused()
{
    ImGuiContext& g = *GImGui;
    return g.NavWindow != NULL;
}

// OBSOLETED in 1.86 (from October 2021)
bool ImGui::IsWindowContentHoverable(ImGuiWindow* window, ImGuiHoveredFlags flags)
{
    // An active popup disable hovering on other windows (and their children)
    // In turn, a popup lets hovering only over itself and its children.
    // Therefore, we can check if the current window is part of the popup stack.
    ImGuiContext& g = *GImGui;
    if (g.OpenPopupStack.Size > 0)
        if (ImGuiWindow* top_most_popup = g.OpenPopupStack.back().Window)
            if (!top_most_popup->IsChildOf(window, true))
                return false;
    return true;
}

// OBSOLETED in 1.84 (from August 2021)
void ImGui::TreeAdvanceToLabelPos()
{
    ImGuiContext& g = *GImGui;
    g.CurrentWindow->DC.CursorPos.x += GetTreeNodeToLabelSpacing();
}

// OBSOLETED in 1.82 (from April 2021)
void ImGui::SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback, void* custom_callback_data)
{
    ImGuiContext& g = *GImGui;
    g.NextWindowData.SizeConstraintCond = ImGuiCond_Always;
    g.NextWindowData.SizeConstraintRect = ImRect(size_min, size_max);
    g.NextWindowData.SizeCallback = custom_callback;
    g.NextWindowData.SizeCallbackUserData = custom_callback_data;
}

// OBSOLETED in 1.81 (from February 2021)
void ImGui::SetNextTreeNodeOpen(bool is_open, ImGuiCond cond)
{
    ImGuiContext& g = *GImGui;
    if (g.CurrentWindow->SkipItems)
        return;
    g.NextTreeNodeOpenCond = cond;
    g.NextTreeNodeOpenVal = is_open;
}

// OBSOLETED in 1.80 (from January 2021)
void ImGui::Value(const char* prefix, bool b)           { Text("%s: %s", prefix, (b ? "true" : "false")); }
void ImGui::Value(const char* prefix, int v)            { Text("%s: %d", prefix, v); }
void ImGui::Value(const char* prefix, unsigned int v)   { Text("%s: %u", prefix, v); }
void ImGui::Value(const char* prefix, float v, const char* float_format) { if (float_format) { char buf[64]; snprintf(buf, 64, float_format, v); Text("%s: %s", prefix, buf); } else { Text("%s: %f", prefix, v); } }

// OBSOLETED in 1.79 (from October 2020)
void ImGui::Indent()                                    { Indent(GImGui->Style.IndentSpacing); }
void ImGui::Unindent()                                  { Unindent(GImGui->Style.IndentSpacing); }

// OBSOLETED in 1.78 (from August 2020)
void ImGui::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}

// OBSOLETED in 1.76 (from June 2020)
void ImGui::PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect) { GetWindowDrawList()->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect); }
void ImGui::PopClipRect() { GetWindowDrawList()->PopClipRect(); }

// OBSOLETED in 1.75 (from May 2020)
bool ImGui::IsWindowAppearing() { return GImGui->CurrentWindow->Appearing; }

// OBSOLETED in 1.73 (from April 2020)
bool ImGui::GetContentRegionAvail(ImVec2* p_out) { *p_out = GetContentRegionAvail(); return true; }

// OBSOLETED in 1.72 (from March 2020)
ImDrawList* ImGui::GetOverlayDrawList() { return &GImGui->ForegroundDrawList; }

// OBSOLETED in 1.71 (from February 2020)
bool ImGui::IsAnyItemActive() { return GImGui->ActiveId != 0; }

// OBSOLETED in 1.70 (from January 2020)
bool ImGui::IsAnyItemFocused() { return GImGui->NavId != 0 && !GImGui->NavDisableHighlight; }

// OBSOLETED in 1.69 (from January 2019)
void ImGui::ShowFontSelector(const char* label)
{
    ImGuiContext& g = *GImGui;
    ImFont* font_current = GetFont();
    if (BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < g.IO.Fonts->Fonts.Size; n++)
        {
            ImFont* font = g.IO.Fonts->Fonts[n];
            PushID((void*)font);
            if (Selectable(font->GetDebugName(), font == font_current))
                g.IO.FontDefault = font;
            PopID();
        }
        EndCombo();
    }
}

// OBSOLETED in 1.67 (from October 2018)
void ImGui::SetNextWindowContentSize(const ImVec2& size) { SetNextWindowContentSize(size); } // We are using the namespace qualification to make sure that we are calling the function in the global namespace.

// OBSOLETED in 1.66 (from September 2018)
void ImGui::LogText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LogTextV(fmt, args);
    va_end(args);
}

// OBSOLETED in 1.64 (from August 2018)
void ImGui::BeginGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    g.GroupStack.resize(g.GroupStack.Size + 1);
    ImGuiGroupData& group_data = g.GroupStack.back();
    group_data.WindowID = window->ID;
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndent = window->DC.Indent;
    group_data.BackupGroupOffset = window->DC.GroupOffset;
    group_data.BackupCurrLineSize = window->DC.CurrLineSize;
    group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
    group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
    group_data.EmitItem = true;

    window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
    window->DC.Indent = window->DC.GroupOffset;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
    g.LogBeginGroup();
}

void ImGui::EndGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    IM_ASSERT(g.GroupStack.Size > 0); // Forgot to call BeginGroup()?

    ImGuiGroupData& group_data = g.GroupStack.back();
    IM_ASSERT(group_data.WindowID == window->ID); // EndGroup() called mismatched with BeginGroup()

    if (group_data.EmitItem)
    {
        ImRect group_bb(group_data.BackupCursorPos, ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));

        window->DC.CursorPos = group_data.BackupCursorPos;
        ItemSize(group_bb.GetSize());
        ItemAdd(group_bb, 0);
    }

    // Log a dummy item if the group is empty
    if (group_data.BackupCursorPos.x == window->DC.CursorMaxPos.x && group_data.EmitItem)
        LogText(" ");

    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.CurrLineSize = group_data.BackupCurrLineSize;
    window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
    if (g.ActiveIdIsAlive != group_data.BackupActiveIdIsAlive)
        g.ActiveIdIsAlive = group_data.BackupActiveIdIsAlive; // This is not great, but we don't have a better way to do this at the moment.
    if (g.ActiveIdPreviousFrameIsAlive != group_data.BackupActiveIdPreviousFrameIsAlive)
        g.ActiveIdPreviousFrameIsAlive = group_data.BackupActiveIdPreviousFrameIsAlive;
    g.GroupStack.pop_back();

    g.LogEndGroup();
}

// OBSOLETED in 1.61 (from March 2018)
bool ImGui::GetWindowContentRegionMin(ImVec2* p_out) { *p_out = GetWindowContentRegionMin(); return true; }
bool ImGui::GetWindowContentRegionMax(ImVec2* p_out) { *p_out = GetWindowContentRegionMax(); return true; }

// OBSOLETED in 1.60 (from February 2018)
void ImGui::ListBoxHeader(const char* label, int items_count, int height_in_items)
{
    if (!BeginListBox(label, ImVec2(0, GetTextLineHeightWithSpacing() * ImMax(1, ImMin(items_count, height_in_items)))))
        return;
}
void ImGui::ListBoxHeader(const char* label, const ImVec2& size) { if (!BeginListBox(label, size)) return; }
void ImGui::ListBoxFooter() { EndListBox(); }

// OBSOLETED in 1.53 (from December 2017)
bool ImGui::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeV(str_id, fmt, args);
    va_end(args);
    return is_open;
}
bool ImGui::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeV(ptr_id, fmt, args);
    va_end(args);
    return is_open;
}

#endif // #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS

//-----------------------------------------------------------------------------

#endif // #ifndef IMGUI_DISABLE
