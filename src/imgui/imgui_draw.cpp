// dear imgui, v1.90.8 WIP
// (drawing and font handling)

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
// [SECTION] STB libraries implementation
//-----------------------------------------------------------------------------

// Compile and use stb_rect_pack.h and stb_truetype.h
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#include "imstb_rectpack.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "imstb_truetype.h"

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------

void ImGui::StyleColorsDark(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
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
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
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
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
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
// [SECTION] ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    Font = NULL;
    FontSize = 0.0f;
    CurveTessellationTol = 1.25f;
    CircleSegmentMaxError = 1.60f;
    ClipRectFullscreen = ImVec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);
    InitialFlags = ImDrawListFlags_None;

    // Look up texture atlas for white pixel.
    if (GImGui && GImGui->IO.Fonts && GImGui->IO.Fonts->TexID)
    {
        ImFontAtlas* atlas = GImGui->IO.Fonts;
        if (atlas->TexUvWhitePixel.x < 0.0f)
            atlas->GetTexDataAsRGBA32(NULL, NULL, NULL); // Need to build font texture
        TexUvWhitePixel = atlas->TexUvWhitePixel;
    }
    else
    {
        TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    }
}

void ImDrawListSharedData::SetCircleTessellationMaxError(float max_error)
{
    if (CircleSegmentMaxError == max_error)
        return;
    IM_ASSERT(max_error > 0.0f);
    CircleSegmentMaxError = max_error;
}

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = GImGui->DrawListSharedData.InitialFlags;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _CmdHeader.ClipRect = GImGui->DrawListSharedData.ClipRectFullscreen;
    _CmdHeader.TextureId = NULL;
    _CmdHeader.VtxOffset = 0;
    _Splitter.Clear();
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _Splitter.ClearFreeMemory();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList)(GImGui->DrawListSharedData);
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

// Using macros because C++ is a terrible language, we want guaranteed inlining without side-effects.
#define IM_DRAWLIST_PRIM_RECT(a, b, col)            do { VtxWritePtr[0].pos = (a);             VtxWritePtr[0].uv = GImGui->DrawListSharedData.TexUvWhitePixel; VtxWritePtr[0].col = (col); \
                                                         VtxWritePtr[1].pos = ImVec2((b).x, (a).y); VtxWritePtr[1].uv = GImGui->DrawListSharedData.TexUvWhitePixel; VtxWritePtr[1].col = (col); \
                                                         VtxWritePtr[2].pos = (b);             VtxWritePtr[2].uv = GImGui->DrawListSharedData.TexUvWhitePixel; VtxWritePtr[2].col = (col); \
                                                         VtxWritePtr[3].pos = ImVec2((a).x, (b).y); VtxWritePtr[3].uv = GImGui->DrawListSharedData.TexUvWhitePixel; VtxWritePtr[3].col = (col); \
                                                         VtxWritePtr += 4;                                                                                \
                                                         IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2); \
                                                         IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3); \
                                                         IdxWritePtr += 6;                                                                                \
                                                         _VtxCurrentIdx += 4; } while (0)

#define IM_DRAWLIST_PRIM_RECT_UV(a, b, uv_a, uv_b, col) do { VtxWritePtr[0].pos = (a);             VtxWritePtr[0].uv = (uv_a); VtxWritePtr[0].col = (col); \
                                                              VtxWritePtr[1].pos = ImVec2((b).x, (a).y); VtxWritePtr[1].uv = ImVec2((uv_b).x, (uv_a).y); VtxWritePtr[1].col = (col); \
                                                              VtxWritePtr[2].pos = (b);             VtxWritePtr[2].uv = (uv_b); VtxWritePtr[2].col = (col); \
                                                              VtxWritePtr[3].pos = ImVec2((a).x, (b).y); VtxWritePtr[3].uv = ImVec2((uv_a).x, (uv_b).y); VtxWritePtr[3].col = (col); \
                                                              VtxWritePtr += 4;                                                                                   \
                                                              IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2); \
                                                              IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3); \
                                                              IdxWritePtr += 6;                                                                                   \
                                                              _VtxCurrentIdx += 4; } while (0)

#define IM_DRAWLIST_PRIM_QUAD_UV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col) do { VtxWritePtr[0].pos = (a); VtxWritePtr[0].uv = (uv_a); VtxWritePtr[0].col = (col); \
                                                                                 VtxWritePtr[1].pos = (b); VtxWritePtr[1].uv = (uv_b); VtxWritePtr[1].col = (col); \
                                                                                 VtxWritePtr[2].pos = (c); VtxWritePtr[2].uv = (uv_c); VtxWritePtr[2].col = (col); \
                                                                                 VtxWritePtr[3].pos = (d); VtxWritePtr[3].uv = (uv_d); VtxWritePtr[3].col = (col); \
                                                                                 VtxWritePtr += 4;                                                                          \
                                                                                 IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2); \
                                                                                 IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3); \
                                                                                 IdxWritePtr += 6;                                                                          \
                                                                                 _VtxCurrentIdx += 4; } while (0)

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We need to extend this path filling code to support caps.
void ImDrawList::AddPolyline(const ImVec2* points, int points_count, ImU32 col, ImDrawFlags flags, float thickness)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = GImGui->DrawListSharedData.TexUvWhitePixel;

    int count = points_count;
    if (!(flags & ImDrawFlags_Closed))
        count = points_count - 1;

    const bool thick_line = thickness > 1.0f;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? count * 10 : count * 8;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2));
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            ImVec2 d = points[i2] - points[i1];
            d = ImVec2(d.y, -d.x);
            float d_len = sqrtf(d.x*d.x + d.y*d.y);
            if (d_len > 0.0f)
                d *= 1.0f / d_len;
            temp_normals[i1] = d;
        }
        if (!(flags & ImDrawFlags_Closed))
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        if (!thick_line)
        {
            if (!(flags & ImDrawFlags_Closed))
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the triangles into a single big triangle strip.
            unsigned int vtx_idx = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                const unsigned int idx_base = vtx_idx;

                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dm_len_sqr = dm.x*dm.x + dm.y*dm.y;
                if (dm_len_sqr < 1e-6f)
                    dm_len_sqr = 1.0f;
                dm *= 1.0f / sqrtf(dm_len_sqr);
                dm *= AA_SIZE;

                temp_points[i2 * 2 + 0] = points[i2] + dm;
                temp_points[i2 * 2 + 1] = points[i2] - dm;

                VtxWritePtr[0].pos = points[i1]; VtxWritePtr[0].uv = uv; VtxWritePtr[0].col = col;
                VtxWritePtr[1].pos = points[i2]; VtxWritePtr[1].uv = uv; VtxWritePtr[1].col = col;
                VtxWritePtr[2].pos = temp_points[i1 * 2 + 0]; VtxWritePtr[2].uv = uv; VtxWritePtr[2].col = col_trans;
                VtxWritePtr[3].pos = temp_points[i1 * 2 + 1]; VtxWritePtr[3].uv = uv; VtxWritePtr[3].col = col_trans;
                VtxWritePtr[4].pos = temp_points[i2 * 2 + 0]; VtxWritePtr[4].uv = uv; VtxWritePtr[4].col = col_trans;
                VtxWritePtr[5].pos = temp_points[i2 * 2 + 1]; VtxWritePtr[5].uv = uv; VtxWritePtr[5].col = col_trans;
                VtxWritePtr += 6;

                IdxWritePtr[0] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[1] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[2] = (ImDrawIdx)(idx_base + 1);
                IdxWritePtr[3] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[4] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[5] = (ImDrawIdx)(idx_base + 4);
                IdxWritePtr[6] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[7] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[8] = (ImDrawIdx)(idx_base + 3);
                IdxWritePtr[9] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[10] = (ImDrawIdx)(idx_base + 3); IdxWritePtr[11] = (ImDrawIdx)(idx_base + 5);
                IdxWritePtr += 12;

                vtx_idx += 6;
            }
            _VtxCurrentIdx = vtx_idx;
        }
        else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!(flags & ImDrawFlags_Closed))
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
                temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the triangles into a single big triangle strip.
            unsigned int vtx_idx = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                const unsigned int idx_base = vtx_idx;

                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dm_len_sqr = dm.x*dm.x + dm.y*dm.y;
                if (dm_len_sqr < 1e-6f)
                    dm_len_sqr = 1.0f;
                dm *= 1.0f / sqrtf(dm_len_sqr);
                const ImVec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
                const ImVec2 dm_in = dm * half_inner_thickness;
                temp_points[i2 * 4 + 0] = points[i2] + dm_out;
                temp_points[i2 * 4 + 1] = points[i2] + dm_in;
                temp_points[i2 * 4 + 2] = points[i2] - dm_in;
                temp_points[i2 * 4 + 3] = points[i2] - dm_out;

                VtxWritePtr[0].pos = temp_points[i1 * 4 + 0]; VtxWritePtr[0].uv = uv; VtxWritePtr[0].col = col_trans;
                VtxWritePtr[1].pos = temp_points[i1 * 4 + 1]; VtxWritePtr[1].uv = uv; VtxWritePtr[1].col = col;
                VtxWritePtr[2].pos = temp_points[i1 * 4 + 2]; VtxWritePtr[2].uv = uv; VtxWritePtr[2].col = col;
                VtxWritePtr[3].pos = temp_points[i1 * 4 + 3]; VtxWritePtr[3].uv = uv; VtxWritePtr[3].col = col_trans;
                VtxWritePtr[4].pos = temp_points[i2 * 4 + 0]; VtxWritePtr[4].uv = uv; VtxWritePtr[4].col = col_trans;
                VtxWritePtr[5].pos = temp_points[i2 * 4 + 1]; VtxWritePtr[5].uv = uv; VtxWritePtr[5].col = col;
                VtxWritePtr[6].pos = temp_points[i2 * 4 + 2]; VtxWritePtr[6].uv = uv; VtxWritePtr[6].col = col;
                VtxWritePtr[7].pos = temp_points[i2 * 4 + 3]; VtxWritePtr[7].uv = uv; VtxWritePtr[7].col = col_trans;
                VtxWritePtr += 8;

                IdxWritePtr[0] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[1] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[2] = (ImDrawIdx)(idx_base + 5);
                IdxWritePtr[3] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[4] = (ImDrawIdx)(idx_base + 5); IdxWritePtr[5] = (ImDrawIdx)(idx_base + 4);
                IdxWritePtr[6] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[7] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[8] = (ImDrawIdx)(idx_base + 6);
                IdxWritePtr[9] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[10] = (ImDrawIdx)(idx_base + 6); IdxWritePtr[11] = (ImDrawIdx)(idx_base + 5);
                IdxWritePtr[12] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[13] = (ImDrawIdx)(idx_base + 3); IdxWritePtr[14] = (ImDrawIdx)(idx_base + 7);
                IdxWritePtr[15] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[16] = (ImDrawIdx)(idx_base + 7); IdxWritePtr[17] = (ImDrawIdx)(idx_base + 6);
                IdxWritePtr += 18;

                vtx_idx += 8;
            }
            _VtxCurrentIdx = vtx_idx;
        }
    }
    else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count * 6;
        const int vtx_count = count * 4;
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];

            ImVec2 d = p2 - p1;
            d = ImVec2(d.y, -d.x);
            float d_len = sqrtf(d.x*d.x + d.y*d.y);
            if (d_len > 0.0f)
                d *= (thickness * 0.5f) / d_len;

            const unsigned int idx_base = _VtxCurrentIdx;
            VtxWritePtr[0].pos = p1 + d; VtxWritePtr[0].uv = uv; VtxWritePtr[0].col = col;
            VtxWritePtr[1].pos = p2 + d; VtxWritePtr[1].uv = uv; VtxWritePtr[1].col = col;
            VtxWritePtr[2].pos = p2 - d; VtxWritePtr[2].uv = uv; VtxWritePtr[2].col = col;
            VtxWritePtr[3].pos = p1 - d; VtxWritePtr[3].uv = uv; VtxWritePtr[3].col = col;
            VtxWritePtr += 4;

            IdxWritePtr[0] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[1] = (ImDrawIdx)(idx_base + 1); IdxWritePtr[2] = (ImDrawIdx)(idx_base + 2);
            IdxWritePtr[3] = (ImDrawIdx)(idx_base + 0); IdxWritePtr[4] = (ImDrawIdx)(idx_base + 2); IdxWritePtr[5] = (ImDrawIdx)(idx_base + 3);
            IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

void ImDrawList::AddConvexPolyFilled(const ImVec2* points, int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = GImGui->DrawListSharedData.TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = points_count * 2;
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 d = p1 - p0;
            d = ImVec2(d.y, -d.x);
            float d_len = sqrtf(d.x*d.x + d.y*d.y);
            if (d_len > 0.0f)
                d *= 1.0f / d_len;
            temp_normals[i0] = d;
        }

        unsigned int vtx_idx = _VtxCurrentIdx;
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 dm = (temp_normals[i0] + temp_normals[i1]) * 0.5f;
            float dm_len_sqr = dm.x*dm.x + dm.y*dm.y;
            if (dm_len_sqr < 1e-6f)
                dm_len_sqr = 1.0f;
            dm *= 1.0f / sqrtf(dm_len_sqr);
            dm *= AA_SIZE * 0.5f;

            VtxWritePtr[0].pos = (p1 - dm); VtxWritePtr[0].uv = uv; VtxWritePtr[0].col = col;        // Inner
            VtxWritePtr[1].pos = (p1 + dm); VtxWritePtr[1].uv = uv; VtxWritePtr[1].col = col_trans;  // Outer
            VtxWritePtr += 2;

            // Add indexes for border
            IdxWritePtr[0] = (ImDrawIdx)(vtx_idx + (i1 << 1)); IdxWritePtr[1] = (ImDrawIdx)(vtx_idx + (i0 << 1)); IdxWritePtr[2] = (ImDrawIdx)(vtx_idx + (i0 << 1) + 1);
            IdxWritePtr[3] = (ImDrawIdx)(vtx_idx + (i1 << 1)); IdxWritePtr[4] = (ImDrawIdx)(vtx_idx + (i0 << 1) + 1); IdxWritePtr[5] = (ImDrawIdx)(vtx_idx + (i1 << 1) + 1);
            IdxWritePtr += 6;
        }
        _VtxCurrentIdx += points_count * 2;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            VtxWritePtr[0].pos = points[i];
            VtxWritePtr[0].uv = uv;
            VtxWritePtr[0].col = col;
            VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx);
            IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1);
            IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            IdxWritePtr += 3;
        }
        _VtxCurrentIdx += vtx_count;
    }
}

void ImDrawList::PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments)
{
    if (radius == 0.0f)
    {
        _Path.push_back(center);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full circle you don't need the last point because it will connect back to the first one.
    // But if you are trying to draw an arc, you need the last point!
    _Path.reserve(_Path.Size + num_segments + 1);
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(ImVec2(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
    }
}

void ImDrawList::PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius == 0.0f || a_min_of_12 > a_max_of_12)
    {
        _Path.push_back(center);
        return;
    }

    // Scale normals up to represent circles of radius 1, render actual radius in Vtx shader
    static ImVec2 circle_vtx[12];
    static bool circle_vtx_builds = false;
    if (!circle_vtx_builds)
    {
        for (int i = 0; i < 12; i++)
        {
            const float a = ((float)i / 12.0f) * 2.0f * IM_PI;
            circle_vtx[i].x = cosf(a);
            circle_vtx[i].y = sinf(a);
        }
        circle_vtx_builds = true;
    }

    _Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
    for (int a = a_min_of_12; a <= a_max_of_12; a++)
        _Path.push_back(ImVec2(center.x + circle_vtx[a % 12].x * radius, center.y + circle_vtx[a % 12].y * radius));
}

void ImDrawList::PathBezierCubicCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
        num_segments = 32; // Auto-tessellated
    _Path.reserve(_Path.Size + num_segments);
    for (int i = 1; i <= num_segments; i++)
    {
        float t = (float)i / (float)num_segments;
        float u = 1.0f - t;
        float w1 = u*u*u;
        float w2 = 3*u*u*t;
        float w3 = 3*u*t*t;
        float w4 = t*t*t;
        _Path.push_back(ImVec2(w1*p1.x + w2*p2.x + w3*p3.x + w4*p4.x, w1*p1.y + w2*p2.y + w3*p3.y + w4*p4.y));
    }
}

void ImDrawList::PathBezierQuadraticCurveTo(const ImVec2& p2, const ImVec2& p3, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
        num_segments = 32; // Auto-tessellated
    _Path.reserve(_Path.Size + num_segments);
    for (int i = 1; i <= num_segments; i++)
    {
        float t = (float)i / (float)num_segments;
        float u = 1.0f - t;
        float w1 = u*u;
        float w2 = 2*u*t;
        float w3 = t*t;
        _Path.push_back(ImVec2(w1*p1.x + w2*p2.x + w3*p3.x, w1*p1.y + w2*p2.y + w3*p3.y));
    }
}

void ImDrawList::PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding, ImDrawFlags flags)
{
    rounding = ImMin(rounding, fabsf(rect_max.x - rect_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, fabsf(rect_max.y - rect_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);

    if (rounding <= 0.0f || (flags & ImDrawFlags_RoundCornersNone) == ImDrawFlags_RoundCornersNone)
    {
        PathLineTo(rect_min);
        PathLineTo(ImVec2(rect_max.x, rect_min.y));
        PathLineTo(rect_max);
        PathLineTo(ImVec2(rect_min.x, rect_max.y));
    }
    else
    {
        const float r = rounding;
        if (flags & ImDrawFlags_RoundCornersTopLeft)     PathArcToFast(ImVec2(rect_min.x + r, rect_min.y + r), r, 6, 9);
        else PathLineTo(rect_min);
        if (flags & ImDrawFlags_RoundCornersTopRight)    PathArcToFast(ImVec2(rect_max.x - r, rect_min.y + r), r, 9, 12);
        else PathLineTo(ImVec2(rect_max.x, rect_min.y));
        if (flags & ImDrawFlags_RoundCornersBottomRight) PathArcToFast(ImVec2(rect_max.x - r, rect_max.y - r), r, 0, 3);
        else PathLineTo(rect_max);
        if (flags & ImDrawFlags_RoundCornersBottomLeft)  PathArcToFast(ImVec2(rect_min.x + r, rect_max.y - r), r, 3, 6);
        else PathLineTo(ImVec2(rect_min.x, rect_max.y));
    }
}

void ImDrawList::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1 + ImVec2(0.5f, 0.5f));
    PathLineTo(p2 + ImVec2(0.5f, 0.5f));
    PathStroke(col, 0, thickness);
}

// p_min = upper-left, p_max = lower-right
void ImDrawList::AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.50f, 0.50f), rounding, flags);
    else
        PathRect(p_min + ImVec2(0.50f, 0.50f), p_max - ImVec2(0.49f, 0.49f), rounding, flags); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(p_min, p_max, rounding, flags);
        PathFillConvex(col);
    }
    else
    {
        PrimReserve(6, 4);
        PrimRect(p_min, p_max, col);
    }
}

// p_min = upper-left, p_max = lower-right
void ImDrawList::AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = GImGui->DrawListSharedData.TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteVtx(p_min, uv, col_upr_left);
    PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    PrimWriteVtx(p_max, uv, col_bot_right);
    PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
    PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 4); PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 3); PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 2);
    PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 4); PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 2); PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx - 1);
}

void ImDrawList::AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathLineTo(p4);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(p1);
    PathLineTo(p2);
    PathLineTo(p3);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
        num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, GImGui->Style.CircleTessellationMaxError);

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);

    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || radius <= 0.0f)
        return;

    // Obtain segment count
    if (num_segments <= 0)
        num_segments = IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, GImGui->Style.CircleTessellationMaxError);

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    num_segments = ImClamp(num_segments, 3, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);

    const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

void ImDrawList::AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;
    PathArcTo(center, radius - 0.5f, 0.0f, 2.0f * IM_PI, num_segments - 1);
    PathStroke(col, ImDrawFlags_Closed, thickness);
}

void ImDrawList::AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;
    PathArcTo(center, radius, 0.0f, 2.0f * IM_PI, num_segments - 1);
    PathFillConvex(col);
}

// Text is rendered via a string helper function because we need to parse UTF-8 codepoints.
// FIXME-OPT: This is suboptimal because we are calling GetCharacterRangeFromUTF8() on every call to AddText().
// Ideally we should cache the result of this conversion.
void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared data if not specified
    if (font == NULL)
        font = GImGui->DrawListSharedData.Font;
    if (font_size == 0.0f)
        font_size = GImGui->DrawListSharedData.FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID != NULL); // Use the dear imgui driver back-end drawing function that will call 'Build'
    IM_ASSERT(font->ContainerAtlas->TexID != (ImTextureID)(intptr_t)-1);

    ImVec4 clip_rect = _CmdHeader.ClipRect;
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(p_min, p_max, uv_min, uv_max, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1, const ImVec2& uv2, const ImVec2& uv3, const ImVec2& uv4, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (rounding <= 0.0f || (flags & ImDrawFlags_RoundCornersNone) == ImDrawFlags_RoundCornersNone)
    {
        AddImage(user_texture_id, p_min, p_max, uv_min, uv_max, col);
        return;
    }

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(p_min, p_max, rounding, flags);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    ImGui::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, p_min, p_max, uv_min, uv_max, true);

    if (push_texture_id)
        PopTextureID();
}
//-----------------------------------------------------------------------------
// [SECTION] ImDrawListSplitter
//-----------------------------------------------------------------------------
//- Details: https://github.com/ocornut/imgui/issues/2286
//- We are using this splitting logic to be able to render layers in any order.
//- Watermark in ShowDemoWindow() is using this to render on top of all other windows.
//-----------------------------------------------------------------------------

void ImDrawListSplitter::Clear()
{
    _Current = 0;
    _Count = 1;
    _Channels.resize(1);
    _Channels[0].CmdBuffer.resize(0);
    _Channels[0].IdxBuffer.resize(0);
}

void ImDrawListSplitter::ClearFreeMemory()
{
    for (int i = 0; i < _Channels.Size; i++)
    {
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

void ImDrawListSplitter::Split(ImDrawList* draw_list, int count)
{
    IM_ASSERT(_Current == 0);
    _Count = count;
    _Channels.resize(count);
    for (int i = 0; i < count; i++)
    {
        _Channels[i].CmdBuffer.resize(0);
        _Channels[i].IdxBuffer.resize(0);
    }
}

static void ImDrawList_SwapBuffers(ImDrawList* draw_list, ImVector<ImDrawCmd>* cmd_buffer, ImVector<ImDrawIdx>* idx_buffer, ImVector<ImDrawVert>* vtx_buffer)
{
    IM_ASSERT(draw_list->CmdBuffer.Size == 0 && draw_list->IdxBuffer.Size == 0 && draw_list->VtxBuffer.Size == 0);
    draw_list->CmdBuffer.swap(*cmd_buffer);
    draw_list->IdxBuffer.swap(*idx_buffer);
    draw_list->VtxBuffer.swap(*vtx_buffer);
}

void ImDrawListSplitter::Merge(ImDrawList* draw_list)
{
    if (_Count <= 1)
        return;

    // Note that we are NOT merging the VtxBuffer, because all channels have been writing to the same one.
    // So we can leave it as-is.
    int new_cmd_size = 0;
    int new_idx_size = 0;
    for (int i = 0; i < _Count; i++)
    {
        new_cmd_size += _Channels[i].CmdBuffer.Size;
        new_idx_size += _Channels[i].IdxBuffer.Size;
    }
    ImVector<ImDrawCmd> new_cmd_buffer;
    ImVector<ImDrawIdx> new_idx_buffer;
    new_cmd_buffer.resize(new_cmd_size);
    new_idx_buffer.resize(new_idx_size);

    ImDrawCmd* cmd_write = new_cmd_buffer.Data;
    ImDrawIdx* idx_write = new_idx_buffer.Data;
    for (int i = 0; i < _Count; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        memcpy(cmd_write, ch.CmdBuffer.Data, ch.CmdBuffer.Size * sizeof(ImDrawCmd));
        memcpy(idx_write, ch.IdxBuffer.Data, ch.IdxBuffer.Size * sizeof(ImDrawIdx));
        cmd_write += ch.CmdBuffer.Size;
        idx_write += ch.IdxBuffer.Size;
    }
    draw_list->CmdBuffer.swap(new_cmd_buffer);
    draw_list->IdxBuffer.swap(new_idx_buffer);

    // Invalidate the current command header to avoid an assert on the next PushClipRect()
    draw_list->_CmdHeader.VtxOffset = draw_list->VtxBuffer.Size;
}

void ImDrawListSplitter::SetCurrentChannel(ImDrawList* draw_list, int channel_idx)
{
    IM_ASSERT(channel_idx >= 0 && channel_idx < _Count);
    if (_Current == channel_idx)
        return;

    // Switch buffers
    ImDrawList_SwapBuffers(draw_list, &_Channels[_Current].CmdBuffer, &_Channels[_Current].IdxBuffer, &draw_list->VtxBuffer);
    _Current = channel_idx;
    ImDrawList_SwapBuffers(draw_list, &_Channels[_Current].CmdBuffer, &_Channels[_Current].IdxBuffer, &draw_list->VtxBuffer);

    // Invalidate the current command header to avoid an assert on the next PushClipRect()
    draw_list->_CmdHeader.VtxOffset = draw_list->VtxBuffer.Size;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

// Convert UTF-8 to 32-bit character, process single character input.
// A nearly-direct copy of pdh's utf8_decode() from http://www.w3.org/International/questions/qa-forms-utf-8.en.html
// We handle UTF-8 decoding error by skipping forward.
int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
    unsigned int c = (unsigned char)*in_text;
    if (c < 0x80)
    {
        *out_char = c;
        return 1;
    }
    if (c < 0xE0)
    {
        if (in_text + 1 < in_text_end)
        {
            unsigned int c2 = (unsigned char)in_text[1];
            if ((c2 & 0xC0) == 0x80)
            {
                *out_char = ((c & 0x1F) << 6) | (c2 & 0x3F);
                return 2;
            }
        }
    }
    if (c < 0xF0)
    {
        if (in_text + 2 < in_text_end)
        {
            unsigned int c2 = (unsigned char)in_text[1];
            unsigned int c3 = (unsigned char)in_text[2];
            if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80)
            {
                *out_char = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                return 3;
            }
        }
    }
    if (c < 0xF8)
    {
        if (in_text + 3 < in_text_end)
        {
            unsigned int c2 = (unsigned char)in_text[1];
            unsigned int c3 = (unsigned char)in_text[2];
            unsigned int c4 = (unsigned char)in_text[3];
            if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80 && (c4 & 0xC0) == 0x80)
            {
                *out_char = ((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
                return 4;
            }
        }
    }
    *out_char = 0xFFFD; // Replacement character
    return 1;
}

int ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end)
{
    int char_count = 0;
    while (in_text < in_text_end)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        char_count++;
    }
    return char_count;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
    if (c < 0x80)
    {
        buf[0] = (char)c;
        return 1;
    }
    if (c < 0x800)
    {
        if (buf_size < 2) return 0;
        buf[0] = (char)(0xC0 + (c >> 6));
        buf[1] = (char)(0x80 + (c & 0x3F));
        return 2;
    }
    if (c < 0x10000)
    {
        if (buf_size < 3) return 0;
        buf[0] = (char)(0xE0 + (c >> 12));
        buf[1] = (char)(0x80 + ((c >> 6) & 0x3F));
        buf[2] = (char)(0x80 + ((c ) & 0x3F));
        return 3;
    }
    if (c <= 0x10FFFF)
    {
        if (buf_size < 4) return 0;
        buf[0] = (char)(0xF0 + (c >> 18));
        buf[1] = (char)(0x80 + ((c >> 12) & 0x3F));
        buf[2] = (char)(0x80 + ((c >> 6) & 0x3F));
        buf[3] = (char)(0x80 + ((c ) & 0x3F));
        return 4;
    }
    // Invalid code point, the max unicode is 0x10FFFF
    return 0;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

ImFontAtlas::ImFontAtlas()
{
    Locked = false;
    Flags = ImFontAtlasFlags_None;
    TexID = (ImTextureID)(intptr_t)-1;
    TexDesiredWidth = 0;
    TexGlyphPadding = 1;

    TexReady = false;
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvScale = ImVec2(0.0f, 0.0f);
    TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    for (int n = 0; n < IM_ARRAYSIZE(CustomRects); n++)
        CustomRects[n].ID = 0;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot delete ImFontAtlas between NewFrame() and EndFrame/Render()! Please call ImGui::Shutdown() or ImGui::DestroyContext() to ensure this is not happening.");
    Clear();
}

void ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigDataCount > 0) // Invalidate.
            Fonts[i]->ConfigData = NULL;
    ConfigData.clear();
    CustomRects.clear();
}

void ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexReady = false;
}

void ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < Fonts.Size; i++)
        IM_DELETE(Fonts[i]);
    Fonts.clear();
    ClearTexData();
}

void ImFontAtlas::Clear()
{
    ClearInputData();
    ClearFonts();
}

bool ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    return ImFontAtlasBuildWithStbTruetype(this);
}

void ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build pixels texture if not already built
    if (!TexReady)
        Build();

    if (out_pixels)
        *out_pixels = TexPixelsAlpha8;
    if (out_width)
        *out_width = TexWidth;
    if (out_height)
        *out_height = TexHeight;
    if (out_bytes_per_pixel)
        *out_bytes_per_pixel = 1;
}

void ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is slow, it is needed to support color emoji and glyphs.
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels_alpha8;
        GetTexDataAsAlpha8(&pixels_alpha8, NULL, NULL);
        if (pixels_alpha8)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels_alpha8;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                *dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    if (out_pixels)
        *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width)
        *out_width = TexWidth;
    if (out_height)
        *out_height = TexHeight;
    if (out_bytes_per_pixel)
        *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use ImGui::GetIO().Fonts->AddFontDefault() to add the default font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
    }

    // Invalidate texture
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font is ProggyClean.ttf, at 13px
// See more font options and instructions here: https://github.com/ocornut/imgui/tree/master/misc/fonts
const ImWchar* ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean Hangul Compatibility Jamo
        0xAC00, 0xD7A3, // Korean Hangul Syllables
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesChineseFull()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuation, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width and Full-width Forms
        0x4E00, 0x9FAF, // CJK Unified Ideographs
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store a list of common Chinese characters for simplified Chinese.
    // Sourced from https://gist.github.com/e3e7/dd55d64804332467597a
    // Updated in 2021 to include a few more characters.
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuation, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width and Full-width Forms
        0x4E00, 0x9FAF, // CJK Unified Ideographs
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesJapanese()
{
    // Store a list of common Japanese characters.
    // Sourced from https://gist.github.com/e3e7/dd55d64804332467597a
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuation, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width and Full-width Forms
        0x4E00, 0x9FAF, // CJK Unified Ideographs
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesThai()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesVietnamese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin
        0x0102, 0x0103,
        0x0110, 0x0111,
        0x0128, 0x0129,
        0x0168, 0x0169,
        0x01A0, 0x01A1,
        0x01AF, 0x01B0,
        0x1EA0, 0x1EF9,
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    Scale = 1.0f;
    DisplayOffset = ImVec2(0.0f, 0.0f);
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    FallbackChar = (ImWchar)'?';
    EllipsisChar = (ImWchar)'...';
    EllipsisCharCount = 1;
    EllipsisWidth = 0.0f;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
    for (int i = 0; i < IM_ARRAYSIZE(Used4kPagesMap); i++)
        Used4kPagesMap[i] = 0;
}

ImFont::~ImFont()
{
    // Programmers over-zealousy to avoid leaking anything tends to lead to mistakes here.
    // - ContainerAtlas is owned by ImGui::GetIO().Fonts -> we don't free it.
    // - ConfigData is owned by ImFontAtlas and shared by all fonts loaded from the same file -> we don't free it.
    // - Glyph is owned by ImFontAtlas and shared by all fonts loaded from the same file -> we don't free it.
    // - If you need to add/remove fonts at runtime, you will need to do a full rebuild of the font atlas.
    //   (see https://github.com/ocornut/imgui/issues/1497)
}

void ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    Scale = 1.0f;
    DisplayOffset = ImVec2(0.0f, 0.0f);
    Glyphs.clear();
    IndexAdvanceX.clear();
    IndexLookup.clear();
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    DirtyLookupTables = true;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i < Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    // Build lookup table
    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.resize(max_codepoint + 1);
    IndexLookup.resize(max_codepoint + 1);
    for (int i = 0; i < max_codepoint + 1; i++)
    {
        IndexAdvanceX[i] = -1.0f;
        IndexLookup[i] = (ImWchar)-1;
    }
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;
    }

    // Create a fallback glyph if we can't find the one we want.
    // We are not using FallbackChar for this purpose, as it is specified by user and may not be in the font.
    FallbackGlyph = FindGlyph((ImWchar)'?', true);
    if (FallbackGlyph == NULL)
    {
        if (!Glyphs.empty())
            FallbackGlyph = &Glyphs[0]; // Any glyph will do
    }
    EllipsisWidth = 0.0f;
    if (EllipsisChar != (ImWchar)-1)
    {
        const ImFontGlyph* glyph = FindGlyph(EllipsisChar);
        if (glyph)
            EllipsisWidth = glyph->AdvanceX;
    }

    DirtyLookupTables = false;
}

bool ImFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last)
{
    unsigned int page_begin = (c_begin / 4096);
    unsigned int page_last = (c_last / 4096);
    for (unsigned int page_n = page_begin; page_n <= page_last; page_n++)
        if ((Used4kPagesMap[page_n >> 3] & (1 << (page_n & 7))))
            return false;
    return true;
}

void ImFont::SetGlyphVisible(ImWchar c, bool visible)
{
    if (ImFontGlyph* glyph = (ImFontGlyph*)(void*)FindGlyph((ImWchar)c))
        glyph->Visible = visible ? 1 : 0;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what is essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force progress by Gesundheits-batzen.
                    word_wrap_eol++;
            }

            if (s >= word_wrap_eol)
            {
                s = word_wrap_eol;
                IM_ASSERT(s > text_begin); // Should only happen on the second loop after a wrap.
                word_wrap_eol = NULL;
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
        }

        // Decode and calculate glyph width
        unsigned int c = (unsigned char)*s;
        const char* next_s;
        if (c < 0x80)
        {
            next_s = s + 1;
        }
        else
        {
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            s = next_s;
            continue;
        }
        if (c == '\r')
        {
            s = next_s;
            continue;
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX[(int)c] : FallbackAdvanceX) * scale;
        if (line_width + char_width >= max_width)
        {
            s = next_s; // We could define a max-width behavior where we render a dot instead of the character that passed the limit.
            break;
        }
        line_width += char_width;
        s = next_s;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured Unicode text wrapping
    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters on each iteration

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned char)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX[(int)c] : FallbackAdvanceX);
        if (ImCharIsBlankW(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }
            inside_word = true;
        }

        // Allow wrapping after punctuation.
        if (s > text && (s[0] == ',' || s[0] == '.' || s[0] == '!' || s[0] == '?'))
            word_end = next_s;

        if (line_width + word_width > wrap_width)
        {
            // Break within a word if necessary
            if (prev_word_end == NULL || (line_width + word_width) - wrap_width > wrap_width)
                return s;
            return prev_word_end;
        }

        s = next_s;
    }

    return text_end;
}

void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const
{
    const ImFontGlyph* glyph = FindGlyph(c);
    if (!glyph || !glyph->Visible)
        return;
    float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
    pos.x = floorf(pos.x + DisplayOffset.x);
    pos.y = floorf(pos.y + DisplayOffset.y);
    draw_list->PrimReserve(6, 4);
    draw_list->PrimRectUV(
        ImVec2(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale),
        ImVec2(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale),
        ImVec2(glyph->U0, glyph->V0),
        ImVec2(glyph->U1, glyph->V1),
        col);
}

void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // ImGui:: functions generally already provides a valid text_end, so this is here just for fallback.

    // Align to be pixel perfect
    pos.x = floorf(pos.x + DisplayOffset.x);
    pos.y = floorf(pos.y + DisplayOffset.y);
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    // Fast-forward to first visible line
    const char* s = text_begin;
    if (y + line_height < clip_rect.y && !word_wrap_enabled)
        while (y + line_height < clip_rect.y && s < text_end)
        {
            s = (const char*)memchr(s, '\n', text_end - s);
            s = s ? s + 1 : text_end;
            y += line_height;
        }

    // For large text, scan for the last visible line in order to avoid processing unused amount of text
    if (text_end - s > 10000 && !word_wrap_enabled)
    {
        const char* s_end = s;
        float y_end = y;
        while (y_end < clip_rect.w && s_end < text_end)
        {
            s_end = (const char*)memchr(s_end, '\n', text_end - s_end);
            s_end = s_end ? s_end + 1 : text_end;
            y_end += line_height;
        }
        text_end = s_end;
    }
    if (s == text_end)
        return;

    // Render
    const ImU32 col_trans = col & ~IM_COL32_A_MASK;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what is essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force progress by Gesundheits-batzen.
                    word_wrap_eol++;
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Stop rendering if we are outside the clipping rectangle
                if (y > clip_rect.w)
                    break;
                continue;
            }
        }

        // Decode and calculate glyph width
        unsigned int c = (unsigned char)*s;
        const char* next_s;
        if (c < 0x80)
        {
            next_s = s + 1;
        }
        else
        {
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c == '\n')
        {
            x = pos.x;
            y += line_height;

            // Stop rendering if we are outside the clipping rectangle
            if (y > clip_rect.w)
                break;
            s = next_s;
            continue;
        }
        if (c == '\r')
        {
            s = next_s;
            continue;
        }

        const ImFontGlyph* glyph = FindGlyph((ImWchar)c);
        if (glyph == NULL)
        {
            s = next_s;
            continue;
        }

        float char_width = glyph->AdvanceX * scale;
        if (glyph->Visible)
        {
            // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
            float x1 = x + glyph->X0 * scale;
            float x2 = x + glyph->X1 * scale;
            float y1 = y + glyph->Y0 * scale;
            float y2 = y + glyph->Y1 * scale;
            if (x1 <= clip_rect.z && x2 >= clip_rect.x)
            {
                // Render a character
                float u0 = glyph->U0;
                float v0 = glyph->V0;
                float u1 = glyph->U1;
                float v1 = glyph->V1;

                // CPU side clipping used for coarse clipping of large list of items.
                // Generally beneficial on mobile phones.
                if (cpu_fine_clip)
                {
                    if (x1 < clip_rect.x)
                    {
                        u0 = u0 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u1 - u0);
                        x1 = clip_rect.x;
                    }
                    if (y1 < clip_rect.y)
                    {
                        v0 = v0 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v1 - v0);
                        y1 = clip_rect.y;
                    }
                    if (x2 > clip_rect.z)
                    {
                        u1 = u0 + ((clip_rect.z - x1) / (x2 - x1)) * (u1 - u0);
                        x2 = clip_rect.z;
                    }
                    if (y2 > clip_rect.w)
                    {
                        v1 = v0 + ((clip_rect.w - y1) / (y2 - y1)) * (v1 - v0);
                        y2 = clip_rect.w;
                    }
                    if (y1 >= y2 || x1 >= x2)
                    {
                        x += char_width;
                        s = next_s;
                        continue;
                    }
                }

                // We are NOT calling PrimRectUV() here because we want to access VtxBuffer[] directly for performance.
                // This is a bit messy but is one of the oldest and most used function in the library, and used to be affecting performance heavily.
                // And we have a guarantee that the texture id is the same for all characters.
                {
                    draw_list->PrimReserve(6, 4);
                    draw_list->VtxBuffer[_VtxCurrentIdx+0].pos = ImVec2(x1, y1); draw_list->VtxBuffer[_VtxCurrentIdx+0].uv = ImVec2(u0, v0); draw_list->VtxBuffer[_VtxCurrentIdx+0].col = col;
                    draw_list->VtxBuffer[_VtxCurrentIdx+1].pos = ImVec2(x2, y1); draw_list->VtxBuffer[_VtxCurrentIdx+1].uv = ImVec2(u1, v0); draw_list->VtxBuffer[_VtxCurrentIdx+1].col = col;
                    draw_list->VtxBuffer[_VtxCurrentIdx+2].pos = ImVec2(x2, y2); draw_list->VtxBuffer[_VtxCurrentIdx+2].uv = ImVec2(u1, v1); draw_list->VtxBuffer[_VtxCurrentIdx+2].col = col;
                    draw_list->VtxBuffer[_VtxCurrentIdx+3].pos = ImVec2(x1, y2); draw_list->VtxBuffer[_VtxCurrentIdx+3].uv = ImVec2(u0, v1); draw_list->VtxBuffer[_VtxCurrentIdx+3].col = col;
                    draw_list->IdxBuffer[_IdxCurrentIdx+0] = (ImDrawIdx)_VtxCurrentIdx; draw_list->IdxBuffer[_IdxCurrentIdx+1] = (ImDrawIdx)(_VtxCurrentIdx+1); draw_list->IdxBuffer[_IdxCurrentIdx+2] = (ImDrawIdx)(_VtxCurrentIdx+2);
                    draw_list->IdxBuffer[_IdxCurrentIdx+3] = (ImDrawIdx)_VtxCurrentIdx; draw_list->IdxBuffer[_IdxCurrentIdx+4] = (ImDrawIdx)(_VtxCurrentIdx+2); draw_list->IdxBuffer[_IdxCurrentIdx+5] = (ImDrawIdx)(_VtxCurrentIdx+3);
                    draw_list->_VtxCurrentIdx += 4;
                    draw_list->_IdxCurrentIdx += 6;
                }
            }
        }
        x += char_width;
        s = next_s;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// This is the main build function.
// It can be called from other code, but typically this is called by ImGui::GetIO().Fonts->Build().
bool ImFontAtlasBuildWithStbTruetype(ImFontAtlas* atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildRegisterDefaultCustomRects(atlas);

    atlas->TexID = (ImTextureID)(intptr_t)-1;
    atlas->TexReady = false;
    atlas->TexPixelsAlpha8 = NULL;
    atlas->TexPixelsRGBA32 = NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = ImVec2(0.0f, 0.0f);
    atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    atlas->ClearTexData();

    // 1. Pack all glyphs into a big texture rectangle
    stbrp_context pack_context;
    stbrp_node* pack_nodes;
    {
        // Create a list of all glyphs to pack
        ImVector<stbrp_rect> rects;
        rects.resize(0);
        for (int i = 0; i < atlas->ConfigData.Size; i++)
        {
            ImFontConfig& cfg = atlas->ConfigData[i];
            IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

            const int font_offset = stbtt_GetFontOffsetForIndex((const unsigned char*)cfg.FontData, cfg.FontDataSize, cfg.FontNo);
            if (font_offset == -1)
            {
                IM_ASSERT(0); // @td-ignore
                continue;
            }

            stbtt_fontinfo font_info;
            stbtt_InitFont(&font_info, (const unsigned char*)cfg.FontData, font_offset);

            // Using oversampling to get better looking results.
            const float oversample_h = (cfg.OversampleH > 0) ? (float)cfg.OversampleH : 1.0f;
            const float oversample_v = (cfg.OversampleV > 0) ? (float)cfg.OversampleV : 1.0f;
            const float font_scale = stbtt_ScaleForPixelHeight(&font_info, cfg.SizePixels * oversample_v);

            int ascent, descent, line_gap;
            stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
            cfg.DstFont->Ascent = floorf(ascent * font_scale + 0.5f);
            cfg.DstFont->Descent = floorf(descent * font_scale + 0.5f);

            for (const ImWchar* src_range = cfg.GlyphRanges; src_range[0] && src_range[1]; src_range += 2)
            {
                for (ImWchar c = src_range[0]; c <= src_range[1]; c++)
                {
                    if (cfg.MergeMode && cfg.DstFont->FindGlyph(c))
                        continue;

                    int g = stbtt_FindGlyphIndex(&font_info, c);
                    if (g == 0)
                        continue;

                    int x0, y0, x1, y1;
                    stbtt_GetGlyphBitmapBox(&font_info, g, font_scale * oversample_h, font_scale, &x0, &y0, &x1, &y1);
                    int w = x1 - x0;
                    int h = y1 - y0;
                    if (w > 0 && h > 0)
                    {
                        stbrp_rect r;
                        r.id = (int)c;
                        r.w = w + atlas->TexGlyphPadding;
                        r.h = h + atlas->TexGlyphPadding;
                        rects.push_back(r);
                    }
                }
            }
        }

        // Pack all rectangles into a single texture
        int tex_width = 0;
        int tex_height = 0;
        if (rects.Size > 0)
        {
            pack_nodes = (stbrp_node*)IM_ALLOC(rects.Size * sizeof(stbrp_node));
            stbrp_init_target(&pack_context, 1024, 1024, pack_nodes, rects.Size);
            stbrp_pack_rects(&pack_context, rects.Data, rects.Size);

            // Find the maximum width and height of the packed rectangles
            for (int i = 0; i < rects.Size; i++)
                if (rects[i].was_packed)
                {
                    tex_width = ImMax(tex_width, rects[i].x + rects[i].w);
                    tex_height = ImMax(tex_height, rects[i].y + rects[i].h);
                }
        }

        // Create the texture
        atlas->TexWidth = tex_width;
        atlas->TexHeight = tex_height;
        atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(tex_width * tex_height);
        memset(atlas->TexPixelsAlpha8, 0, tex_width * tex_height);
        atlas->TexUvScale = ImVec2(1.0f / tex_width, 1.0f / tex_height);

        // Render all glyphs to the texture
        for (int i = 0; i < rects.Size; i++)
        {
            if (rects[i].was_packed)
            {
                stbrp_rect* r = &rects[i];
                ImWchar c = (ImWchar)r->id;

                for (int j = 0; j < atlas->ConfigData.Size; j++)
                {
                    ImFontConfig& cfg = atlas->ConfigData[j];
                    if (cfg.DstFont->FindGlyph(c)) // Already has a glyph for this codepoint
                        continue;

                    const int font_offset = stbtt_GetFontOffsetForIndex((const unsigned char*)cfg.FontData, cfg.FontDataSize, cfg.FontNo);
                    if (font_offset == -1)
                        continue;

                    stbtt_fontinfo font_info;
                    stbtt_InitFont(&font_info, (const unsigned char*)cfg.FontData, font_offset);

                    const float oversample_h = (cfg.OversampleH > 0) ? (float)cfg.OversampleH : 1.0f;
                    const float oversample_v = (cfg.OversampleV > 0) ? (float)cfg.OversampleV : 1.0f;
                    const float font_scale = stbtt_ScaleForPixelHeight(&font_info, cfg.SizePixels * oversample_v);

                    int g = stbtt_FindGlyphIndex(&font_info, c);
                    if (g == 0)
                        continue;

                    int advance, lsb;
                    stbtt_GetGlyphHMetrics(&font_info, g, &advance, &lsb);

                    int x0, y0, x1, y1;
                    stbtt_GetGlyphBitmapBox(&font_info, g, font_scale * oversample_h, font_scale, &x0, &y0, &x1, &y1);

                    ImFontGlyph glyph;
                    glyph.Codepoint = c;
                    glyph.Visible = (g != 0);
                    glyph.X0 = (float)x0 / oversample_h;
                    glyph.Y0 = (float)y0 / oversample_v;
                    glyph.X1 = (float)x1 / oversample_h;
                    glyph.Y1 = (float)y1 / oversample_v;
                    glyph.U0 = (r->x + 0.5f) * atlas->TexUvScale.x;
                    glyph.V0 = (r->y + 0.5f) * atlas->TexUvScale.y;
                    glyph.U1 = (r->x + r->w - 0.5f) * atlas->TexUvScale.x;
                    glyph.V1 = (r->y + r->h - 0.5f) * atlas->TexUvScale.y;
                    glyph.AdvanceX = floorf(advance * font_scale + 0.5f);

                    cfg.DstFont->AddGlyph(&cfg, glyph);

                    unsigned char* dst_ptr = atlas->TexPixelsAlpha8 + r->y * atlas->TexWidth + r->x;
                    stbtt_MakeGlyphBitmap(&font_info, dst_ptr, r->w - atlas->TexGlyphPadding, r->h - atlas->TexGlyphPadding, atlas->TexWidth, font_scale * oversample_h, font_scale, g);

                    // Mark 4K page as used
                    const int page_n = c / 4096;
                    atlas->Used4kPagesMap[page_n >> 3] |= 1 << (page_n & 7);
                }
            }
        }
        IM_FREE(pack_nodes);
    }

    // 2. Store our custom rectangles
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlasCustomRect& r = atlas->CustomRects[i];
        if (r.ID == 0 || r.ID >= 0x10000)
        {
            IM_ASSERT(0); // ID needs to be lower than 0x10000
            continue;
        }

        stbrp_rect rect;
        rect.id = r.ID;
        rect.w = r.Width;
        rect.h = r.Height;
        stbrp_pack_rects(&pack_context, &rect, 1);
        if (rect.was_packed)
        {
            r.GlyphID = rect.id;
            r.GlyphAdvanceX = (float)rect.w;
            r.TexCoords[0] = ImVec2((float)rect.x / atlas->TexWidth, (float)rect.y / atlas->TexHeight);
            r.TexCoords[1] = ImVec2((float)(rect.x + rect.w) / atlas->TexWidth, (float)(rect.y + rect.h) / atlas->TexHeight);
            if (r.ID < 0x100)
            {
                unsigned char* dst_ptr = atlas->TexPixelsAlpha8 + rect.y * atlas->TexWidth + rect.x;
                for (int y = 0; y < r.Height; y++)
                    for (int x = 0; x < r.Width; x++)
                        dst_ptr[y * atlas->TexWidth + x] = r.Pixels[y * r.Width + x];
            }
        }
    }

    // 3. Build white pixel
    const int BORDER = 1;
    stbrp_rect r;
    r.id = 0;
    r.w = r.h = BORDER * 2;
    stbrp_pack_rects(&pack_context, &r, 1);
    if (r.was_packed)
    {
        atlas->TexUvWhitePixel = ImVec2((r.x + BORDER) * atlas->TexUvScale.x, (r.y + BORDER) * atlas->TexUvScale.y);
        unsigned char* dst_ptr = atlas->TexPixelsAlpha8 + r.y * atlas->TexWidth + r.x;
        for (int y = 0; y < BORDER * 2; y++)
            for (int x = 0; x < BORDER * 2; x++)
                dst_ptr[y * atlas->TexWidth + x] = 0xFF;
    }
    else
    {
        // We couldn't find a space for our white pixel, we need to do something.
        // This is very unlikely to happen, but we need to handle it.
        // We'll just use the top-left pixel of the texture as a white pixel.
        atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    }

    // 4. Build font lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();

    atlas->TexReady = true;
    return true;
}

void ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas* atlas)
{
    if (atlas->CustomRects.Size >= 1)
        return;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
        atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF*2+1, FONT_ATLAS_DEFAULT_TEX_DATA_H, FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS);
}

//-----------------------------------------------------------------------------
// ImFontAtlas::AddCustomRect()
//-----------------------------------------------------------------------------

int ImFontAtlas::AddCustomRectRegular(unsigned int id, int width, int height, const unsigned char* pixels)
{
    IM_ASSERT(id >= 0x100);
    IM_ASSERT(width > 0 && height > 0);
    IM_ASSERT(pixels != NULL);
    ImFontAtlasCustomRect r;
    r.ID = id;
    r.Width = width;
    r.Height = height;
    r.Pixels = pixels;
    r.GlyphID = 0;
    r.GlyphAdvanceX = 0.0f;
    r.TexCoords[0] = r.TexCoords[1] = ImVec2(0, 0);
    CustomRects.push_back(r);
    return CustomRects.Size - 1;
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset)
{
    IM_ASSERT(font != NULL);
    IM_ASSERT(id >= 0x100);
    IM_ASSERT(width > 0 && height > 0);
    ImFontAtlasCustomRect r;
    r.ID = id;
    r.Width = width;
    r.Height = height;
    r.Pixels = NULL;
    r.GlyphID = 0;
    r.GlyphAdvanceX = advance_x;
    r.TexCoords[0] = r.TexCoords[1] = ImVec2(0, 0);
    CustomRects.push_back(r);
    ImFontGlyph glyph;
    glyph.Codepoint = id;
    glyph.Visible = true;
    glyph.X0 = offset.x;
    glyph.Y0 = offset.y;
    glyph.X1 = offset.x + width;
    glyph.Y1 = offset.y + height;
    glyph.U0 = r.TexCoords[0].x;
    glyph.V0 = r.TexCoords[0].y;
    glyph.U1 = r.TexCoords[1].x;
    glyph.V1 = r.TexCoords[1].y;
    glyph.AdvanceX = advance_x;
    font->AddGlyph(NULL, glyph);
    return CustomRects.Size - 1;
}

const ImFontAtlasCustomRect* ImFontAtlas::GetCustomRectByIndex(int index) const
{
    IM_ASSERT(index >= 0 && index < CustomRects.Size);
    return &CustomRects[index];
}

void ImFontAtlas::CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->ID > 0 && rect->GlyphID != 0);
    *out_uv_min = rect->TexCoords[0];
    *out_uv_max = rect->TexCoords[1];
}

bool ImFontAtlas::GetMouseCursorTexData(ImGuiMouseCursor cursor, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2])
{
    if (cursor < 0 || cursor >= ImGuiMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(CustomRects[0].ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    ImFontAtlasCustomRect* r = &CustomRects[0];
    IM_ASSERT(r->GlyphID != 0);
    ImVec2 pos = FONT_ATLAS_DEFAULT_TEX_DATA_LOCATIONS[cursor];
    ImVec2 size = FONT_ATLAS_DEFAULT_TEX_DATA_SIZES[cursor];
    *out_size = size;
    *out_offset = pos;
    out_uv_border[0] = (r->TexCoords[0] + pos * TexUvScale);
    out_uv_border[1] = (r->TexCoords[0] + (pos + size) * TexUvScale);
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    out_uv_fill[0] = (r->TexCoords[0] + pos * TexUvScale);
    out_uv_fill[1] = (r->TexCoords[0] + (pos + size) * TexUvScale);
    return true;
}
