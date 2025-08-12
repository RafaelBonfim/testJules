// dear imgui, v1.90.8 WIP
// (demo code)

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

// System includes
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, qsort, atoi
#if defined(_MSC_VER)
#include <crtdbg.h>         // _ASSERTE
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow: Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value before calling operator 'xxx' to avoid overflow (io.AddInputCharacter call).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning parameter 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"        // warning: 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy->paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning: cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wformat-security"                // warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"          // warning: declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has a destructor, it's unfortunate. but we don't have a better choice.
#pragma clang diagnostic ignored "-Wunused-macros"                  // warning: macro is not used                                   // we define snprintf/vsnprintf on Windows so they are available, but might not be used.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                       // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this warning.
#pragma clang diagnostic ignored "-Wreserved-id-macro"              // warning: macro name is a reserved identifier
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"        // warning: variable 'init_ref' is used uninitialized whenever 'if' condition is true
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"          // warning: format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wmisleading-indentation"   // [__GNUC__ >= 6] warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_t);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppFullscreen(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleAppDocuments(bool* p_open);

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon font.
// You can also use HelpMarker(const char*) as a more readable equivalent.
void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window
//-----------------------------------------------------------------------------
// - ShowDemoWindow()
// - ShowExampleAppMainMenuBar()
// - ShowExampleAppConsole()
// - ShowExampleAppLog()
// - ShowExampleAppLayout()
// - ShowExampleAppPropertyEditor()
// - ShowExampleAppLongText()
// - ShowExampleAppAutoResize()
// - ShowExampleAppConstrainedResize()
// - ShowExampleAppSimpleOverlay()
// - ShowExampleAppFullscreen()
// - ShowExampleAppWindowTitles()
// - ShowExampleAppCustomRendering()
// - ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Demonstrate most Dear ImGui features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does.
// You may then search for keywords in the code when you are interested by a specific feature.
void ImGui::ShowDemoWindow(bool* p_open)
{
    // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
    // Most functions would normally assert because the font atlas is not built.
    IM_ASSERT(ImGui::GetIO().Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_fullscreen = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;
    static bool show_app_documents = false;

    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_fullscreen)          ShowExampleAppFullscreen(&show_app_fullscreen);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);
    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);

    // Dear ImGui Apps (accessible from the "Tools" menu)
    static bool show_app_metrics = false;
    static bool show_app_debug_log = false;
    static bool show_app_id_stack_tool = false;
    static bool show_app_about = false;
    static bool show_app_style_editor = false;

    if (show_app_metrics)             ImGui::ShowMetricsWindow(&show_app_metrics);
    if (show_app_debug_log)           ImGui::ShowDebugLogWindow(&show_app_debug_log);
    if (show_app_id_stack_tool)       ImGui::ShowIDStackToolWindow(&show_app_id_stack_tool);
    if (show_app_about)               ImGui::ShowAboutWindow(&show_app_about);
    if (show_app_style_editor)
    {
        ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool unsaved_document = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Dear ImGui Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
    // e.g. Use 2/3 of the space for widgets and 1/3 for labels (right aligned)
    //ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
    // e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ShowExampleAppMainMenuBar();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            ImGui::MenuItem("Console", NULL, &show_app_console);
            ImGui::MenuItem("Log", NULL, &show_app_log);
            ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
            ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
            ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            ImGui::MenuItem("Fullscreen window", NULL, &show_app_fullscreen);
            ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            ImGui::MenuItem("Documents", NULL, &show_app_documents);
            ImGui::EndMenu();
        }
        //if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
        if (ImGui::BeginMenu("Tools"))
        {
#ifndef IMGUI_DISABLE_METRICS_WINDOW
            ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
#endif
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
            ImGui::MenuItem("Debug Log", NULL, &show_app_debug_log);
            ImGui::MenuItem("ID Stack Tool", NULL, &show_app_id_stack_tool);
#endif
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("dear imgui says hello! (%s)", IMGUI_VERSION);
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::Text("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more compact examples.");
        ImGui::BulletText("Wait, this is a TypeScript codebase!");
        ImGui::Separator();

        ImGui::Text("PROGRAMMER GUIDE:");
        ImGui::BulletText(
            "See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!\n"
            "See comments in imgui.cpp.\n"
            "See example applications in the examples/ folder.\n"
            "Read the FAQ at http://www.dearimgui.org/faq/\n"
            "Set 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' for keyboard controls.\n"
            "Set 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad' for gamepad controls.\n");
        ImGui::Separator();

        ImGui::Text("USER GUIDE:");
        ImGui::ShowUserGuide();
    }

    if (ImGui::CollapsingHeader("Configuration"))
    {
        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::TreeNode("Configuration##2"))
        {
            ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard",    &io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
            ImGui::SameLine(); HelpMarker("Enable keyboard navigation.\n");
            ImGui::CheckboxFlags("io.ConfigFlags: NavEnableGamepad",     &io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
            ImGui::SameLine(); HelpMarker("Enable gamepad navigation.\n");
            ImGui::Separator();

            ImGui::CheckboxFlags("io.ConfigFlags: NavNoCaptureKeyboard", &io.ConfigFlags, ImGuiConfigFlags_NavNoCaptureKeyboard);
            ImGui::SameLine(); HelpMarker("Don't capture keyboard when navigating.\n");
            ImGui::CheckboxFlags("io.ConfigFlags: NoMouse",              &io.ConfigFlags, ImGuiConfigFlags_NoMouse);
            ImGui::SameLine(); HelpMarker("Make mouse invisible and block mouse interaction.\n");
            ImGui::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange",  &io.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange);
            ImGui::SameLine(); HelpMarker("Don't change mouse cursor icon.\n");
            ImGui::Separator();

            ImGui::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            ImGui::SameLine(); HelpMarker("Enable blinking cursor in text field.");
            ImGui::Checkbox("io.ConfigDragClickToInputText", &io.ConfigDragClickToInputText);
            ImGui::SameLine(); HelpMarker("Enable turning DragXXX widgets into text input with a simple click-drag (without holding Alt).");
            ImGui::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            ImGui::SameLine(); HelpMarker("Enable resizing windows from their edges and corners.");
            ImGui::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            ImGui::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            ImGui::SameLine(); HelpMarker("Instruct Dear ImGui to render a mouse cursor for you. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable hardware cursor but switch to software cursor when resizing or dragging a window).");
            ImGui::Text("Also see style.FramePadding for borders around items.");

            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Backend Flags"))
        {
            HelpMarker(
                "Those flags are set by the back-ends (imgui_impl_xxx files) to specify their capabilities.\n"
                "Here we expose them as read-only fields to avoid breaking interactions with your back-end.");

            // Make a local copy to avoid modifying the back-end flags.
            ImGuiBackendFlags backend_flags = io.BackendFlags;
            ImGui::CheckboxFlags("io.BackendFlags: HasGamepad",           &backend_flags, ImGuiBackendFlags_HasGamepad);
            ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors",      &backend_flags, ImGuiBackendFlags_HasMouseCursors);
            ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos",       &backend_flags, ImGuiBackendFlags_HasSetMousePos);
            ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &backend_flags, ImGuiBackendFlags_RendererHasVtxOffset);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Style"))
        {
            HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
            ImGui::ShowStyleEditor();
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNode("Capture/Logging"))
        {
            HelpMarker(
                "The logging API redirects all text output so you can easily capture the content of "
                "a window or a block. Tree nodes can be automatically expanded.\n"
                "Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            ImGui::LogButtons();

            HelpMarker("You can also call ImGui::LogText() to output directly to the log without a visual output.");
            if (ImGui::Button("Copy \"Hello, world!\" to clipboard"))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("Hello, world!");
                ImGui::LogFinish();
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Window options"))
    {
        if (ImGui::BeginTable("split", 3))
        {
            ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &no_titlebar);
            ImGui::TableNextColumn(); ImGui::Checkbox("No scrollbar", &no_scrollbar);
            ImGui::TableNextColumn(); ImGui::Checkbox("No menu", &no_menu);
            ImGui::TableNextColumn(); ImGui::Checkbox("No move", &no_move);
            ImGui::TableNextColumn(); ImGui::Checkbox("No resize", &no_resize);
            ImGui::TableNextColumn(); ImGui::Checkbox("No collapse", &no_collapse);
            ImGui::TableNextColumn(); ImGui::Checkbox("No close", &no_close);
            ImGui::TableNextColumn(); ImGui::Checkbox("No nav", &no_nav);
            ImGui::TableNextColumn(); ImGui::Checkbox("No background", &no_background);
            ImGui::TableNextColumn(); ImGui::Checkbox("No bring to front", &no_bring_to_front);
            ImGui::TableNextColumn(); ImGui::Checkbox("Unsaved document", &unsaved_document);
            ImGui::EndTable();
        }
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowColumns();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    ImGui::End();
}

static void ShowDemoWindowWidgets()
{
    if (!ImGui::CollapsingHeader("Widgets"))
        return;

    static float f = 0.0f;
    static int counter = 0;
    static char str0[128] = "Hello, world!";
    static char str1[128] = "";
    static int item_current = 1;
    static int item_current_2 = 1;
    const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
    static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
    static bool check = true;
    static int e = 0;

    if (ImGui::TreeNode("Basic"))
    {
        ImGui::PushID("basic");
        ImGui::Text("This is some basic text.");
        ImGui::Checkbox("checkbox", &check);
        ImGui::SameLine();
        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);
        ImGui::PopID();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Combo"))
    {
        // Expose items as a string separated by zeroes.
        const char* items_separated_by_zeros = "Apple\0Banana\0Cherry\0Kiwi\0Mango\0Orange\0Pineapple\0Strawberry\0Watermelon\0";
        if (ImGui::Combo("combo 1", &item_current, items_separated_by_zeros)) {}
        if (ImGui::Combo("combo 2 (read-only)", &item_current_2, items, IM_ARRAYSIZE(items), 4)) {}
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Inputs & Sliders"))
    {
        ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
        ImGui::InputTextWithHint("input text (with hint)", "enter text here", str1, IM_ARRAYSIZE(str1));
        ImGui::InputFloat("input float", &f, 0.01f, 1.0f, "%.3f");
        ImGui::SliderFloat("slider float", &f, 0.0f, 1.0f, "ratio = %.3f");
        ImGui::SliderFloat("slider float (log)", &f, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
        ImGui::ColorEdit3("color 1", (float*)&color);
        ImGui::ColorEdit4("color 2", (float*)&color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Multi-line Text Input"))
    {
        static char text[1024 * 16] =
            "/*\n"
            " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
            " the hexadecimal encoding of an undocumented\n"
            " Intel x86 processor instruction that freezes a\n"
            " computer, discovered in 1997.\n"
            "*/\n\n"
            "// A simple helper function to load a file into memory\n"
            "bool LoadFile(const char* filename, char** data, int* size)\n"
            "{\n"
            "    FILE* file = fopen(filename, \"rb\");\n"
            "    if (!file) return false;\n"
            "    fseek(file, 0, SEEK_END);\n"
            "    *size = ftell(file);\n"
            "    fseek(file, 0, SEEK_SET);\n"
            "    *data = (char*)malloc(*size + 1);\n"
            "    fread(*data, 1, *size, file);\n"
            "    (*data)[*size] = 0;\n"
            "    fclose(file);\n"
            "    return true;\n"
            "}\n";
        ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);
        ImGui::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!ImGui::CollapsingHeader("Layout & Scrolling"))
        return;

    if (ImGui::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        ImGui::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        ImGui::Checkbox("Disable Menu", &disable_menu);

        // Child 1: no border, enable horizontal scrollbar
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            ImGui::BeginChild("Child1", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
                ImGui::Text("%04d: scrollable region", i);
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Child 2: with border
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
            if (disable_menu)
                window_flags |= ImGuiWindowFlags_MenuBar;
            ImGui::BeginChild("Child2", ImVec2(0, 260), true, window_flags);
            if (!disable_menu && ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    ImGui::MenuItem("dummy menu");
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::Text("Columns");
            ImGui::Separator();

            // Create a dummy table of 2 columns
            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 100; i++)
                {
                    char buf[32];
                    sprintf(buf, "%03d", i);
                    ImGui::TableNextColumn(); ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                    ImGui::TableNextColumn(); ImGui::Text("Some contents");
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Widgets Width"))
    {
        // Use PushItemWidth() to set the width of the upcoming widgets.
        // Use SameLine() to position widgets on the same line.
        // Use GetContentRegionAvail().x to query the available space.
        static float f = 0.0f;
        ImGui::Text("PushItemWidth(100)");
        ImGui::SameLine(); HelpMarker("Fixed width.");
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("float##1", &f);
        ImGui::PopItemWidth();

        ImGui::Text("PushItemWidth(GetContentRegionAvail().x * 0.5f)");
        ImGui::SameLine(); HelpMarker("Half of available width.");
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::DragFloat("float##2", &f);
        ImGui::PopItemWidth();

        ImGui::Text("PushItemWidth(-100)");
        ImGui::SameLine(); HelpMarker("Align to right edge minus 100.");
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("float##3", &f);
        ImGui::PopItemWidth();

        ImGui::Text("PushItemWidth(-1)");
        ImGui::SameLine(); HelpMarker("Align to right edge.");
        ImGui::PushItemWidth(-1);
        ImGui::DragFloat("float##4", &f);
        ImGui::PopItemWidth();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Basic Horizontal Layout"))
    {
        ImGui::TextWrapped("(Use ImGui::SameLine() to place items on the same line. Use ImGui::NewLine() to move to a new line.)");

        // Text
        ImGui::Text("Same line: ");
        ImGui::SameLine();
        ImGui::Text("1,");
        ImGui::SameLine();
        ImGui::Text("2,");
        ImGui::SameLine();
        ImGui::Text("3");

        // Button
        ImGui::Button("A");
        ImGui::SameLine();
        ImGui::Button("B");
        ImGui::SameLine();
        ImGui::Button("C");

        // Input
        static char str[32] = "input";
        ImGui::InputText("input 1", str, 32);
        ImGui::SameLine();
        ImGui::Text("label");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        ImGui::Checkbox("My", &c1);
        ImGui::SameLine();
        ImGui::Checkbox("Tailor", &c2);
        ImGui::SameLine();
        ImGui::Checkbox("is", &c3);
        ImGui::SameLine();
        ImGui::Checkbox("rich", &c4);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Groups"))
    {
        HelpMarker(
            "BeginGroup()/EndGroup() allows you to vertically align items.\n"
            "They are particularly useful when adding labels to controls.");

        // Create a fake color just for the demo
        static ImVec4 col = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);

        ImGui::BeginGroup();
        {
            ImGui::Text("First");
            ImGui::ColorButton("color", col, ImGuiColorEditFlags_NoTooltip);
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            ImGui::Text("Second");
            ImGui::Button("Button");
        }
        ImGui::EndGroup();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text Baseline Alignment"))
    {
        HelpMarker(
            "The baseline of a text is the line on which most letters 'sit'.\n"
            "You can use SetNextItemBaselineOffset() to vertically align widgets that are not text-based.");

        ImGui::Text("Text");
        ImGui::SameLine();
        ImGui::SetNextItemBaselineOffset(ImGui::GetTextLineHeight() * 0.5f);
        ImGui::Button("Button");
        ImGui::SameLine();
        HelpMarker("This button is vertically aligned with the text baseline.");

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Scrolling"))
    {
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given position.");

        static int track_item = 50;
        static bool enable_track = true;
        static bool enable_auto_scroll = true;
        ImGui::Checkbox("Enable Track", &enable_track);
        ImGui::Checkbox("Enable Auto-Scroll", &enable_auto_scroll);
        ImGui::SliderInt("Track Item", &track_item, 0, 99);
        if (enable_track)
        {
            float scroll_y = ImGui::GetScrollY();
            float item_y = ImGui::GetCursorPosY();
            ImGui::Text("ScrollY: %.2f, ItemY: %.2f", scroll_y, item_y);
        }

        if (ImGui::Button("Scroll to item"))
        {
            ImGui::SetScrollHereY(track_item / 100.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Scroll to top"))
        {
            ImGui::SetScrollY(0.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Scroll to bottom"))
        {
            ImGui::SetScrollY(ImGui::GetScrollMaxY());
        }

        ImGui::BeginChild("scrolling", ImVec2(0, 150), true);
        for (int i = 0; i < 100; i++)
        {
            if (enable_track && i == track_item)
            {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", i);
                if (enable_auto_scroll)
                    ImGui::SetScrollHereY(0.5f);
            }
            else
            {
                ImGui::Text("Item %d", i);
            }
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Horizontal Scrolling"))
    {
        HelpMarker("Horizontal scrolling can be enabled on a per-window basis (see 'Configuration->Window options').");
        if (ImGui::Button("2000 pixels wide"))
        {
            ImGui::SetNextWindowContentSize(ImVec2(2000, 0));
            ImGui::Begin("2000 pixels wide", NULL, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("Hello, world!");
            ImGui::End();
        }
        ImGui::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!ImGui::CollapsingHeader("Popups & Modal windows"))
        return;

    if (ImGui::TreeNode("Popups"))
    {
        ImGui::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup
        if (ImGui::Button("Select.."))
            ImGui::OpenPopup("my_select_popup");
        ImGui::SameLine();
        ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (ImGui::BeginPopup("my_select_popup"))
        {
            ImGui::SeparatorText("Aquarium");
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (ImGui::Selectable(names[i]))
                    selected_fish = i;
            ImGui::EndPopup();
        }

        // More advanced popup with checkbox handling
        if (ImGui::Button("Toggle.."))
            ImGui::OpenPopup("my_toggle_popup");
        if (ImGui::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                ImGui::MenuItem(names[i], "", &toggles[i]);
            if (ImGui::BeginMenu("Sub-menu"))
            {
                ImGui::MenuItem("Click me");
                ImGui::EndMenu();
            }

            ImGui::Separator();
            ImGui::Text("Tooltip here");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("I am a tooltip over a popup");

            if (ImGui::Button("Stacked Popup"))
                ImGui::OpenPopup("another popup");
            if (ImGui::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    ImGui::MenuItem(names[i], "", &toggles[i]);
                if (ImGui::BeginMenu("Sub-menu"))
                {
                    ImGui::MenuItem("Click me");
                    if (ImGui::Button("Stacked Popup"))
                        ImGui::OpenPopup("another popup");
                    if (ImGui::BeginPopup("another popup"))
                    {
                        ImGui::Text("I am the last one here.");
                        ImGui::EndPopup();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Context menus"))
    {
        HelpMarker("\"Context\" functions are simple helpers to associate a Popup to a given Item or Window identifier.");

        // BeginContextPopup() is a helper to declare a context menu that will be displayed over the last submitted item.
        // It is a shortcut for:
        //   if (IsItemHovered() && IsMouseClicked(1))
        //       OpenPopup("context");
        //   if (BeginPopup("context")) { ... }
        static float value = 0.5f;
        ImGui::Text("Value = %.3f (<-- right-click this)", value);
        ImGui::BeginContextPopup("context");
        if (ImGui::MenuItem("Set to 0.0f")) value = 0.0f;
        if (ImGui::MenuItem("Set to 0.5f")) value = 0.5f;
        if (ImGui::MenuItem("Set to 1.0f")) value = 1.0f;
        ImGui::EndPopup();

        // We can also use BeginContextPopup() on a window.
        // This will create a popup that is displayed over the window itself.
        ImGui::BeginContextPopup("window_context", ImGuiPopupFlags_MouseButtonRight);
        if (ImGui::MenuItem("Clear")) { value = 0.5f; }
        ImGui::EndPopup();

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Modals"))
    {
        HelpMarker("Modal windows are like popups but disabling interaction with the underlying main window. They stay in the center of the viewport and have a dimming background.");

        if (ImGui::Button("Delete.."))
            ImGui::OpenPopup("Delete?");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
            ImGui::Separator();

            static bool dont_ask_me_next_time = false;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Menus inside a regular window"))
    {
        ImGui::TextWrapped("Below is a demonstration of menus embedded in a regular window. Menus are also supported in popups and modals.");
        static bool has_close_button = true;
        ImGui::Checkbox("Show close button", &has_close_button);

        // This is a simple menu that is created within a regular window.
        // It is not a popup, so it will not be closed when clicking outside.
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                if (has_close_button && ImGui::MenuItem("Close", "Ctrl+W")) { *p_open = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::TreePop();
    }
}

static void ShowDemoWindowColumns()
{
    if (!ImGui::CollapsingHeader("Columns"))
        return;

    ImGui::PushID("Columns");

    // Basic columns
    if (ImGui::TreeNode("Basic"))
    {
        ImGui::Text("Without border:");
        ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
        ImGui::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (ImGui::Selectable(label)) {}
            //if (ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f))) {}
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Text("With border:");
        ImGui::Columns(4, "mycolumns"); // 4-ways, with border
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("Name"); ImGui::NextColumn();
        ImGui::Text("Path"); ImGui::NextColumn();
        ImGui::Text("Flags"); ImGui::NextColumn();
        ImGui::Separator();
        const char* names[] = { "One", "Two", "Three" };
        const char* paths[] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                selected = i;
            ImGui::NextColumn();
            ImGui::Text(names[i]); ImGui::NextColumn();
            ImGui::Text(paths[i]); ImGui::NextColumn();
            ImGui::Text("...."); ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Mixed items"))
    {
        ImGui::Columns(3, "mixed");
        ImGui::Separator();

        ImGui::Text("Hello");
        ImGui::Button("Banana");
        ImGui::NextColumn();

        ImGui::Text("ImGui");
        ImGui::Button("Apple");
        static float foo = 1.0f;
        ImGui::InputFloat("red", &foo, 0.05f, 0, "%.2f");
        ImGui::Text("An extra line here.");
        ImGui::NextColumn();

        ImGui::Text("Get this party started!");
        static float bar = 1.0f;
        ImGui::InputFloat("blue", &bar, 0.05f, 0, "%.2f");
        ImGui::NextColumn();

        if (ImGui::CollapsingHeader("Category A")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category B")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        if (ImGui::CollapsingHeader("Category C")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::TreePop();
    }

    // Scrolling columns
    if (ImGui::TreeNode("Scrolling"))
    {
        ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 7 + 30), true);
        ImGui::Columns(3);
        static bool selection[10] = {};
        for (int i = 0; i < 10; i++)
        {
            char label[32];
            sprintf(label, "Item %d", i);
            if (ImGui::Selectable(label, &selection[i])) {}
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::EndChild();
        ImGui::TreePop();
    }

    ImGui::PopID();
}

static void ShowDemoWindowMisc()
{
    if (ImGui::CollapsingHeader("Filtering"))
    {
        static ImGuiTextFilter filter;
        ImGui::Text("Filter usage:\n"
            "  \"\"         display all lines\n"
            "  \"xxx\"      display lines containing \"xxx\"\n"
            "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
            "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                ImGui::BulletText("%s", lines[i]);
    }

    if (ImGui::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        ImGui::Text("WantTextInput: %d", io.WantTextInput);
        ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
        ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        if (ImGui::TreeNode("Mouse State"))
        {
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse pos: <INVALID>");
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
            ImGui::Text("Mouse wheel H: %.1f", io.MouseWheelH);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Keyboard, Gamepad & Navigation State"))
        {
            ImGui::Text("Keys down:");      for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (ImGui::IsKeyDown(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
            ImGui::Text("Keys pressed:");   for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (ImGui::IsKeyPressed(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
            ImGui::Text("Keys released:");  for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (ImGui::IsKeyReleased(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
            ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            ImGui::Text("NavInputs down:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputs[i]); }
            ImGui::Text("NavInputs pressed:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { ImGui::SameLine(); ImGui::Text("[%d]", i); }
            ImGui::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

            ImGui::Button("Hovering me sets the\nkeyboard focus here");
            if (ImGui::IsItemHovered())
                ImGui::SetKeyboardFocusHere();

            ImGui::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar
//-----------------------------------------------------------------------------
// - ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------
static void ShowExampleAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::BeginMenu("Open Recent"))
            {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                if (ImGui::BeginMenu("More.."))
                {
                    ImGui::MenuItem("Hello");
                    ImGui::MenuItem("Sailor");
                    if (ImGui::BeginMenu("Recurse.."))
                    {
                        ShowExampleAppMainMenuBar();
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As..")) {}

            ImGui::Separator();

            if (ImGui::BeginMenu("Options"))
            {
                static bool enabled = true;
                ImGui::MenuItem("Enabled", "", &enabled);
                ImGui::BeginChild("child", ImVec2(0, 60), true);
                for (int i = 0; i < 10; i++)
                    ImGui::Text("Scrolling Text %d", i);
                ImGui::EndChild();
                static float f = 0.5f;
                static int n = 0;
                ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                ImGui::InputFloat("Input", &f, 0.1f);
                ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Colors"))
            {
                float sz = ImGui::GetTextLineHeight();
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
                    ImVec2 p = ImGui::GetCursorScreenPos();
                    ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                    ImGui::Dummy(ImVec2(sz, sz));
                    ImGui::SameLine();
                    ImGui::MenuItem(name);
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Quit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Console
//-----------------------------------------------------------------------------
// - ShowExampleAppConsole()
//-----------------------------------------------------------------------------
static void ShowExampleAppConsole(bool* p_open)
{
    static char                            input_buf[256];
    static ImVector<char*>                 items;
    static ImVector<const char*>           commands;
    static ImVector<char*>                 history;
    static int                             history_pos = -1; // -1: new line, 0..history.Size-1 browsing history.
    static ImGuiTextFilter                 filter;
    static bool                            auto_scroll = true;
    static bool                            scroll_to_bottom = false;

    // TODO: display items starting from the bottom

    if (!ImGui::Begin("Example: Console", p_open))
    {
        ImGui::End();
        return;
    }

    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            *p_open = false;
        ImGui::EndPopup();
    }

    ImGui::TextWrapped(
        "This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help.");

    if (ImGui::SmallButton("Add Debug Text"))  { items.push_back(strdup("Not a command")); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add Debug Error")) { items.push_back(strdup("[error] Something went wrong")); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear"))           { items.clear(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); items.push_back(strdup("Spam")); }

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) items.clear();
        ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or add custom widgets.
    // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too slow and may require user-side clipping
    // to only process visible items. The clipper will automatically measure the height of your first item and then
    // assume all same height items.
    // If you already have a constant height item what you can do is just add a dummy item of zero size before
    // your clipper, and then call the clipper with a constant height. That would make the clipper not have
    // to guess the height of your item.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < items.Size; i++)
    {
        const char* item = items[i];
        if (!filter.PassFilter(item))
            continue;

        // Normally you would store more information in your item than just a string.
        // (e.g. make Items[] an array of structure, store color/type etc.)
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
        else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    if (scroll_to_bottom || (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    scroll_to_bottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", input_buf, IM_ARRAYSIZE(input_buf), input_text_flags, [](ImGuiInputTextCallbackData* data) {
        //AddLog("cursor: %d, selection_start: %d, selection_end: %d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        return 0;
    }, (void*)this))
    {
        char* s = input_buf;
        Strtrim(s);
        if (s[0])
            ExecCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

#endif // #ifndef IMGUI_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Example App: Log
//-----------------------------------------------------------------------------
// - ShowExampleAppLog()
//-----------------------------------------------------------------------------
// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
static void ShowExampleAppLog(bool* p_open)
{
    static ImGuiTextBuffer log;
    static bool            auto_scroll = true;

    if (!ImGui::Begin("Example: Log", p_open))
    {
        ImGui::End();
        return;
    }

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    static ImGuiTextFilter filter;
    filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        log.clear();
    if (copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char* buf = log.begin();
    const char* buf_end = log.end();
    if (filter.IsActive())
    {
        // In this example we don't use the clipper when filtering.
        // The clipper is meant to avoid processing items outside of the visible area.
        // When filtering we need to process all items to find the filtered ones.
        // FIXME-NAV: We need some form of reverse lookup table for clipper to work with filtering.
        for (int line_no = 0; line_no < log.size(); line_no++)
        {
            const char* line_start = buf;
            const char* line_end = (line_no + 1 < log.size()) ? (log.begin() + log.size()) : buf_end;
            if (filter.PassFilter(line_start, line_end))
                ImGui::TextUnformatted(line_start, line_end);
            buf = line_end;
        }
    }
    else
    {
        // The clipper requires a known constant item height.
        // If you have items of different height, you need to specify a height override on a per-item basis.
        // Here our lines are all the same height, so we can do this easily.
        ImGuiListClipper clipper;
        clipper.Begin(log.size());
        while (clipper.Step())
        {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
            {
                const char* line_start = buf;
                const char* line_end = (line_no + 1 < log.size()) ? (log.begin() + log.size()) : buf_end;
                ImGui::TextUnformatted(line_start, line_end);
                buf = line_end;
            }
        }
        clipper.End();
    }
    ImGui::PopStyleVar();

    if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout
//-----------------------------------------------------------------------------
// - ShowExampleAppLayout()
//-----------------------------------------------------------------------------
static void ShowExampleAppLayout(bool* p_open)
{
    if (!ImGui::Begin("Example: Simple layout", p_open, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Left
    static int selected = 0;
    ImGui::BeginChild("left pane", ImVec2(150, 0), true);
    for (int i = 0; i < 100; i++)
    {
        char label[128];
        sprintf(label, "MyObject %d", i);
        if (ImGui::Selectable(label, selected == i))
            selected = i;
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // Right
    ImGui::BeginGroup();
    ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
    ImGui::Text("MyObject: %d", selected);
    ImGui::Separator();
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Description"))
        {
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details"))
        {
            ImGui::Text("ID: 0123456789");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
    if (ImGui::Button("Revert")) {}
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::EndGroup();

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor
//-----------------------------------------------------------------------------
// - ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    if (!ImGui::Begin("Example: Property editor", p_open))
    {
        ImGui::End();
        return;
    }

    HelpMarker(
        "This example shows how you can use common widgets to create a simple property editor.\n"
        "The left column is for labels, the right column is for values.\n"
        "See 'Demo->Layout->Widgets Width' for details.");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
    {
        // Create a static array of properties that we can edit.
        static struct {
            const char* Name;
            int         Type; // 0=bool, 1=int, 2=float, 3=color
            void*       Ptr;
            float       Step, Min, Max;
        } properties[] = {
            { "Position",   1, (void*)0, 1.0f, 0.0f, 100.0f },
            { "Rotation",   2, (void*)4, 0.1f, -180.0f, 180.0f },
            { "Scale",      2, (void*)8, 0.1f, 0.0f, 10.0f },
            { "Color",      3, (void*)12, 0.0f, 0.0f, 1.0f },
            { "Animate",    0, (void*)16, 0.0f, 0.0f, 0.0f },
        };
        static float values[5] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f };

        for (int i = 0; i < IM_ARRAYSIZE(properties); i++)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(properties[i].Name);
            ImGui::TableSetColumnIndex(1);

            ImGui::PushID(i);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (properties[i].Type == 0)
                ImGui::Checkbox("##value", (bool*)&values[i]);
            else if (properties[i].Type == 1)
                ImGui::DragInt("##value", (int*)&values[i], properties[i].Step, (int)properties[i].Min, (int)properties[i].Max);
            else if (properties[i].Type == 2)
                ImGui::DragFloat("##value", &values[i], properties[i].Step, properties[i].Min, properties[i].Max);
            else if (properties[i].Type == 3)
                ImGui::ColorEdit4("##value", &values[i]);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text
//-----------------------------------------------------------------------------
// - ShowExampleAppLongText()
//-----------------------------------------------------------------------------
static void ShowExampleAppLongText(bool* p_open)
{
    if (!ImGui::Begin("Example: Long text display", p_open))
    {
        ImGui::End();
        return;
    }

    static ImGuiTextBuffer log;
    static int             lines = 0;
    ImGui::Text("Printing unusually long amount of text.");
    ImGui::SameLine();
    if (ImGui::Button("Print"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
        lines += 1000;
    }
    ImGui::BeginChild("Log");
    ImGui::TextUnformatted(log.begin(), log.end());
    ImGui::EndChild();

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize
//-----------------------------------------------------------------------------
// - ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!ImGui::Begin("Example: Auto-resizing window", p_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    static int lines = 10;
    ImGui::TextUnformatted(
        "Window will resize every-frame to the size of its content.\n"
        "Note that you probably don't want to query the window size to\n"
        "output your content because that would create a feedback loop.");
    ImGui::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        ImGui::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize
//-----------------------------------------------------------------------------
// - ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints
    {
        // Helper functions to demonstrate programmatic constraints
        static void Square(ImGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = IM_MAX(data->DesiredSize.x, data->DesiredSize.y); }
        static void Step(ImGuiSizeCallbackData* data)   { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    const char* test_desc[] =
    {
        "Resize vertical only",
        "Resize horizontal only",
        "Width > 100, Height > 100",
        "Width > 400, Height > 400",
        "Custom: Always Square",
        "Custom: Always Step 100",
    };

    for (int i = 0; i < 6; i++)
    {
        ImGuiCond cond = (i < 2) ? ImGuiCond_Always : ImGuiCond_Once;
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));      // Vertical only
        if (i == 1) ImGui::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));      // Horizontal only
        if (i == 2) ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
        if (i == 3) ImGui::SetNextWindowSizeConstraints(ImVec2(400, 400), ImVec2(FLT_MAX, FLT_MAX)); // Width > 400, Height > 400
        if (i == 4) ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);          // Always Square
        if (i == 5) ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Always Step 100

        ImGui::SetNextWindowSize(ImVec2(200, 100), cond);
        char buf[128];
        sprintf(buf, "Example: %s", test_desc[i]);
        if (ImGui::Begin(buf, p_open))
        {
            ImGui::Text("Description:\n%s", test_desc[i]);
        }
        ImGui::End();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay
//-----------------------------------------------------------------------------
// - ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    static int location = 0;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
    {
        ImGui::Text("dear imgui says hello! (%s)", IMGUI_VERSION);
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center",       NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left",     NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right",    NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Fullscreen window
//-----------------------------------------------------------------------------
// - ShowExampleAppFullscreen()
//-----------------------------------------------------------------------------
static void ShowExampleAppFullscreen(bool* p_open)
{
    static bool use_work_area = true;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    if (!ImGui::Begin("Example: Fullscreen window", p_open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Use work area instead of main area", &use_work_area);
    ImGui::SameLine();
    HelpMarker(
        "Main Area = entire viewport.\n"
        "Work Area = entire viewport minus sections used by the main menu bars, task bars etc.\n\n"
        "Enable the main menu bar in this demo to see the difference.");

    ImGui::Separator();
    ImGui::Text("This window cannot be moved or resized. You can close it with the 'X' button.");

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles
//-----------------------------------------------------------------------------
// - ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------
static void ShowExampleAppWindowTitles(bool*)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;

    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display title and ID.

    // Using "##" to display same title but have unique identifier.
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Same title as another window##1");
    ImGui::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Same title as another window##2");
    ImGui::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    ImGui::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], ImGui::GetFrameCount());
    ImGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 300), ImGuiCond_FirstUseEver);
    ImGui::Begin(buf);
    ImGui::Text("This window has a changing title.");
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering
//-----------------------------------------------------------------------------
// - ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------
static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!ImGui::Begin("Example: Custom rendering", p_open))
    {
        ImGui::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
    // Define IMGUI_DEFINE_MATH_OPERATORS in imconfig.h to get vector operators.
    // For example, ImVec2 operator+(const ImVec2&, const ImVec2&) is defined if you define IMGUI_DEFINE_MATH_OPERATORS.
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Primitives
    ImGui::Text("Primitives");
    static float sz = 36.0f;
    static float thickness = 4.0f;
    static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
    ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
    ImGui::ColorEdit3("Color", &col.x);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImU32 col32 = ImColor(col);
    float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
    for (int n = 0; n < 2; n++)
    {
        float th = (n == 0) ? 1.0f : thickness;
        draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col32, th); y += spacing;
        draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 0.0f, 0, th); y += sz + spacing;
        draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32); y += sz + spacing;
        draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), ImColor(0, 0, 0), ImColor(255, 0, 0), ImColor(255, 255, 0), ImColor(0, 255, 0)); y += sz + spacing;
        draw_list->AddTriangle(ImVec2(x, y + sz), ImVec2(x + sz / 2, y), ImVec2(x + sz, y + sz), col32, th); y += sz + spacing;
        draw_list->AddTriangleFilled(ImVec2(x, y + sz), ImVec2(x + sz / 2, y), ImVec2(x + sz, y + sz), col32); y += sz + spacing;
        draw_list->AddCircle(ImVec2(x + sz / 2, y + sz / 2), sz / 2, col32, 0, th); y += sz + spacing;
        draw_list->AddCircleFilled(ImVec2(x + sz / 2, y + sz / 2), sz / 2, col32); y += sz + spacing;
        draw_list->AddNgon(ImVec2(x + sz / 2, y + sz / 2), sz / 2, col32, 4, th); y += sz + spacing;
        draw_list->AddNgonFilled(ImVec2(x + sz / 2, y + sz / 2), sz / 2, col32, 4); y += sz + spacing;
        draw_list->AddText(ImVec2(x, y), col32, "Hello"); y += ImGui::GetTextLineHeight();
        draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 2, ImVec2(x, y), col32, "Hello"); y += ImGui::GetFontSize() * 2;
        x += sz + spacing;
        y = p.y + 4.0f;
    }
    ImGui::Dummy(ImVec2((sz + spacing) * 2, (sz + spacing) * 8));
    ImGui::Separator();

    // Canvas
    ImGui::Text("Canvas");
    static ImVector<ImVec2> points;
    static bool adding_line = false;
    ImGui::Checkbox("Add Line", &adding_line);
    if (adding_line)
    {
        if (ImGui::Button("Clear")) points.clear();
        if (points.Size >= 2)
            ImGui::SameLine();
        if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); }
    }
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw border and background color
    ImGuiIO& io = ImGui::GetIO();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

    // This will catch our interactions
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x, canvas_p0.y); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Add first and second point
    if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        points.push_back(mouse_pos_in_canvas);
        points.push_back(mouse_pos_in_canvas);
    }
    if (adding_line)
    {
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (points.Size == 0 || points.back().x != mouse_pos_in_canvas.x || points.back().y != mouse_pos_in_canvas.y)
                points.push_back(mouse_pos_in_canvas);
        }
        if (is_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            adding_line = false;
        }
    }

    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something specific.
    const float mouse_threshold_for_pan = -1.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
    {
        for (int i = 0; i < points.Size; i++)
            points[i] = ImVec2(points[i].x + io.MouseDelta.x, points[i].y + io.MouseDelta.y);
    }

    // Draw all points
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    for (int i = 0; i < points.Size; i += 2)
        draw_list->AddLine(ImVec2(origin.x + points[i].x, origin.y + points[i].y), ImVec2(origin.x + points[i + 1].x, origin.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
    draw_list->PopClipRect();

    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents
//-----------------------------------------------------------------------------
// - ShowExampleAppDocuments()
//-----------------------------------------------------------------------------
// This is a contrived example to demonstrate using the Docking-specific functions.
// It is not useful in itself, but it can be used as a starting point for more complex applications.
static void ShowExampleAppDocuments(bool* p_open)
{
    // Options
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        ShowDockingDisabledMessage();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth sorting.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
            if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: NoBringToFrontOnFocus",  "", (dockspace_flags & ImGuiDockNodeFlags_NoBringToFrontOnFocus) != 0))  { dockspace_flags ^= ImGuiDockNodeFlags_NoBringToFrontOnFocus; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            ImGui::Separator();

            if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                *p_open = false;
            ImGui::EndMenu();
        }
        HelpMarker(
            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!\n"
            "- Drag from window title bar or their tab to dock/undock.\n"
            "- Drag from window menu button (upper-left button) to undock an entire node (all windows).\n"
            "- Hold SHIFT to disable docking prediction when dragging a window.\n"
            "- Use the \"DockSpace\" menu to create dockspaces.\n"
            "This demo app has nothing to do with it!"
        );

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

static void ShowDockingDisabledMessage()
{
    ImGui::Text("Docking is disabled!");
    ImGui::Text("Please enable it in your io.ConfigFlags with:");
    ImGui::Text("  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;");
}

#endif // #ifndef IMGUI_DISABLE_DEMO_WINDOWS
