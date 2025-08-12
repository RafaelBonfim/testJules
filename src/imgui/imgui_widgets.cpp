// dear imgui, v1.90.8 WIP
// (widgets code)

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
// [SECTION] Widgets
//-----------------------------------------------------------------------------
// - Most widgets are defined in this file.
// - Note that some widgets are defined in imgui.cpp (e.g. Text, Button, Checkbox)
// - Note that some widgets are defined in imgui_tables.cpp (e.g. Table)
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
#include <ctype.h>      // isspace
#include <stdio.h>      // vsnprintf, sscanf, printf

//-------------------------------------------------------------------------
// [SECTION] Widgets: Main
//-------------------------------------------------------------------------
// - SameLine()
// - NewLine()
// - Spacing()
// - Dummy()
// - Indent()
// - Unindent()
// - BeginGroup()
// - EndGroup()
// - GetCursorPos()
// - GetCursorPosX()
// - GetCursorPosY()
// - SetCursorPos()
// - SetCursorPosX()
// - SetCursorPosY()
// - GetCursorStartPos()
// - GetCursorScreenPos()
// - SetCursorScreenPos()
// - AlignTextToFramePadding()
// - GetTextLineHeight()
// - GetTextLineHeightWithSpacing()
// - GetFrameHeight()
// - GetFrameHeightWithSpacing()
//-------------------------------------------------------------------------

void ImGui::SameLine(float offset_from_start_x, float spacing)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    if (offset_from_start_x != 0.0f)
    {
        window->DC.CursorPos.x = window->Pos.x + offset_from_start_x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    else
    {
        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

void ImGui::NewLine()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiLayoutType backup_layout_type = window->DC.LayoutType;
    window->DC.LayoutType = ImGuiLayoutType_Vertical;
    if (window->DC.CurrLineSize.y > 0.0f)     // In the event that we are on a line with items that is smaller that FontSize high, we will preserve its height.
        ItemSize(ImVec2(0.0f, window->DC.CurrLineSize.y));
    else
        ItemSize(ImVec2(0.0f, g.FontSize));
    window->DC.LayoutType = backup_layout_type;
}

void ImGui::Spacing()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(ImVec2(0,0));
}

void ImGui::Dummy(const ImVec2& size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    ItemAdd(bb, 0);
}

void ImGui::Indent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.Indent.x += (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

void ImGui::Unindent(float indent_w)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.Indent.x -= (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

void ImGui::BeginGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

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
    g.LogBegin(ImGuiLogType_Group, 1);
}

void ImGui::EndGroup()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    IM_ASSERT(g.GroupStack.Size > 0); // Forgot to call BeginGroup()?

    ImGuiGroupData& group_data = g.GroupStack.back();
    IM_ASSERT(group_data.WindowID == window->ID); // EndGroup() called mismatched with BeginGroup()

    if (group_data.EmitItem)
    {
        ImRect group_bb(group_data.BackupCursorPos, ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));
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
        g.ActiveIdIsAlive = group_data.BackupActiveIdIsAlive;
    if (g.ActiveIdPreviousFrameIsAlive != group_data.BackupActiveIdPreviousFrameIsAlive)
        g.ActiveIdPreviousFrameIsAlive = group_data.BackupActiveIdPreviousFrameIsAlive;
    g.GroupStack.pop_back();
    g.LogFinish();
}

// Get cursor position within the current window (relative to the window position)
ImVec2 ImGui::GetCursorPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos;
}

float ImGui::GetCursorPosX()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.x - window->Pos.x;
}

float ImGui::GetCursorPosY()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.y - window->Pos.y;
}

void ImGui::SetCursorPos(const ImVec2& local_pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos + local_pos;
}

void ImGui::SetCursorPosX(float local_x)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x + local_x;
}

void ImGui::SetCursorPosY(float local_y)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y + local_y;
}

ImVec2 ImGui::GetCursorStartPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorStartPos - window->Pos;
}

ImVec2 ImGui::GetCursorScreenPos()
{
    ImGuiWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos;
}

void ImGui::SetCursorScreenPos(const ImVec2& pos)
{
    ImGuiWindow* window = GetCurrentWindow();
    window->DC.CursorPos = pos;
}

// Vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
void ImGui::AlignTextToFramePadding()
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    window->DC.CurrLineSize.y = ImMax(window->DC.CurrLineSize.y, g.FontSize + g.Style.FramePadding.y * 2);
    window->DC.CurrLineTextBaseOffset = ImMax(window->DC.CurrLineTextBaseOffset, g.Style.FramePadding.y);
}

// Get text line height
float ImGui::GetTextLineHeight()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize;
}

// Get text line height with spacing
float ImGui::GetTextLineHeightWithSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.ItemSpacing.y;
}

// Get frame height
float ImGui::GetFrameHeight()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.FramePadding.y * 2.0f;
}

// Get frame height with spacing
float ImGui::GetFrameHeightWithSpacing()
{
    ImGuiContext& g = *GImGui;
    return g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Plot
//-------------------------------------------------------------------------
// - PlotLines()
// - PlotHistogram()
//-------------------------------------------------------------------------

void ImGui::PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (frame_size.x == 0.0f)
        frame_size.x = CalcItemWidth();
    if (frame_size.y == 0.0f)
        frame_size.y = label_size.y + (style.FramePadding.y * 2);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return;
    const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);

    // Determine scale from values if not specified
    if (scale_min == FLT_MAX || scale_max == FLT_MAX)
    {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < values_count; i++)
        {
            const float v = values_getter(data, i);
            if (v != v) // Ignore NaN values
                continue;
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (scale_min == FLT_MAX)
            scale_min = v_min;
        if (scale_max == FLT_MAX)
            scale_max = v_max;
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const int values_count_min = (plot_type == ImGuiPlotType_Lines) ? 2 : 1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count);
        int item_count = values_count;
        if (plot_type == ImGuiPlotType_Histogram)
            item_count = res_w;

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if (plot_type == ImGuiPlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
            else if (plot_type == ImGuiPlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale) );                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (1 + scale_min * inv_scale) : (scale_min > 0.0f) ? 1.0f : 0.0f;   // Where does the zero line stands

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t1 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset) % values_count);
            const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale) );

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            if (plot_type == ImGuiPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, v_hovered == n ? col_hovered : col_base);
            }
            else if (plot_type == ImGuiPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, v_hovered == n ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

void ImGui::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayAnonymized getter_data;
    getter_data.Values = values;
    getter_data.Stride = stride;
    PlotEx(ImGuiPlotType_Lines, label, [](void* data, int idx) { ImGuiPlotArrayAnonymized* plot_data = (ImGuiPlotArrayAnonymized*)data; return *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride); }, (void*)&getter_data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    ImGuiPlotArrayAnonymized getter_data;
    getter_data.Values = values;
    getter_data.Stride = stride;
    PlotEx(ImGuiPlotType_Histogram, label, [](void* data, int idx) { ImGuiPlotArrayAnonymized* plot_data = (ImGuiPlotArrayAnonymized*)data; return *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride); }, (void*)&getter_data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void ImGui::PlotHistogram(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(ImGuiPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Sliders
//-------------------------------------------------------------------------
// - SliderFloat()
// - SliderFloat2()
// - SliderFloat3()
// - SliderFloat4()
// - SliderAngle()
// - SliderInt()
// - SliderInt2()
// - SliderInt3()
// - SliderInt4()
// - VSliderFloat()
// - VSliderInt()
//-------------------------------------------------------------------------

// ~100 bytes
struct ImGuiSliderState
{
    ImGuiID     ID;
    ImGuiAxis   Axis;
    ImRect      FrameBB;
    ImRect      GrabBB;
    float       SliderCurrent;
    float       SliderMin, SliderMax;
    float       GrabSize;
    float       Value;
    bool        Hovered, Active;
};

bool ImGui::SliderBehavior(const ImRect& frame_bb, ImGuiID id, float* v, float v_min, float v_max, float power, int decimal_precision, ImGuiSliderFlags flags)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
    const bool is_horizontal = (axis == ImGuiAxis_X);

    const float grab_padding = 2.0f;
    const float slider_sz = (is_horizontal ? frame_bb.GetWidth() : frame_bb.GetHeight()) - grab_padding * 2.0f;
    float grab_sz = style.GrabMinSize;
    if (slider_sz > 0.0f)
        grab_sz = ImMax(grab_sz, slider_sz * 0.1f);
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = (is_horizontal ? frame_bb.Min.x : frame_bb.Min.y) + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = (is_horizontal ? frame_bb.Max.x : frame_bb.Max.y) - grab_padding - grab_sz * 0.5f;

    // For logarithmic sliders that cross over sign boundary we want the exponential increase to be symmetric around 0.0f
    float linear_zero_pos = 0.0f; // 0.0->1.0f
    if (is_logarithmic)
    {
        // Adjust display range on the fly so that the zero line is placeable on screen
        if (v_min * v_max < 0.0f)
        {
            const float linear_dist_min_to_0 = powf(fabsf(0.0f - v_min), 1.0f/power);
            const float linear_dist_max_to_0 = powf(fabsf(v_max - 0.0f), 1.0f/power);
            linear_zero_pos = linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0);
        }
        else
        {
            linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
        }
    }

    // Process clicking on the slider
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        if (g.IO.MouseDown[0])
        {
            const float mouse_abs_pos = is_horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
            float clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
            if (!is_horizontal)
                clicked_t = 1.0f - clicked_t;

            if (is_logarithmic)
            {
                // FIXME: Changed behavior in 1.89, not trying to maintain backward compatibility for this details, but might be nice to have a flag for this?
                // As the slider moves linear in display space we can just apply power on the storage side of the equation
                float v_new = 0.0f;
                if (clicked_t < linear_zero_pos)
                {
                    // Negative side
                    float a = 1.0f - (clicked_t / linear_zero_pos);
                    v_new = v_min + powf(a, power) * (0.0f - v_min);
                }
                else
                {
                    // Positive side
                    float a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                    v_new = 0.0f + powf(a, power) * (v_max - 0.0f);
                }
                if (v_new != *v)
                {
                    *v = v_new;
                    value_changed = true;
                }
            }
            else
            {
                if (v_min > v_max) ImSwap(v_min, v_max);
                const float v_new = ImLerp(v_min, v_max, clicked_t);
                if (v_new != *v)
                {
                    *v = v_new;
                    value_changed = true;
                }
            }
        }
        else
        {
            ClearActiveID();
        }
    }

    // Render
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    float grab_t = 0.0f;
    if (is_logarithmic)
    {
        // Invert the logarithmic scale to get a linear slider position.
        // The deal with the sign cross-over: we need to take the distance from zero as a positive value, and then apply the sign back.
        float v_sign = (*v > 0.0f) ? 1.0f : (*v < 0.0f) ? -1.0f : 0.0f;
        float v_abs = fabsf(*v);
        float v_abs_max = fabsf((v_sign > 0.0f) ? v_max : v_min);
        float v_abs_min = (v_sign != 0.0f) ? 0.0f : fabsf((v_sign > 0.0f) ? v_min : v_max);
        grab_t = (v_abs_max > v_abs_min) ? powf((v_abs - v_abs_min) / (v_abs_max - v_abs_min), 1.0f/power) : 0.0f;
        if (v_sign < 0.0f)
            grab_t = 1.0f - grab_t;
        grab_t = (grab_t * (1.0f - linear_zero_pos)) + linear_zero_pos;
    }
    else
    {
        grab_t = (v_min < v_max) ? ImSaturate((*v - v_min) / (v_max - v_min)) : 0.0f;
    }

    // Draw grab
    if (!is_horizontal)
        grab_t = 1.0f - grab_t;
    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
    ImRect grab_bb;
    if (is_horizontal)
        grab_bb = ImRect(ImVec2(grab_pos - grab_sz * 0.5f, frame_bb.Min.y + grab_padding), ImVec2(grab_pos + grab_sz * 0.5f, frame_bb.Max.y - grab_padding));
    else
        grab_bb = ImRect(ImVec2(frame_bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f), ImVec2(frame_bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f));
    window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

    // Display value
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), "%.*f", decimal_precision, *v);
    if (g.ActiveId == id)
        RenderText(ImVec2(frame_bb.Min.x, frame_bb.Min.y - g.FontSize - style.FramePadding.y), value_buf, value_buf_end);
    else
        RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

// Add a drag widget with a label, available float value, float min, float max, float power, and precision.
// The label is displayed on the right of the widget.
// The power parameter is used to make the slider logarithmic. A power of 1.0f is linear.
// The decimal_precision parameter is used to format the displayed value. A value of -1 will use the default precision.
bool ImGui::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool ImGui::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
    if (format == NULL)
        format = "%.0f deg";
    float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
    bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
    *v_rad = v_deg * (2 * IM_PI) / 360.0f;
    return value_changed;
}

bool ImGui::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool ImGui::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags | ImGuiSliderFlags_Vertical);
}

bool ImGui::VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags | ImGuiSliderFlags_Vertical);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Drags
//-------------------------------------------------------------------------
// - DragFloat()
// - DragFloat2()
// - DragFloat3()
// - DragFloat4()
// - DragFloatRange2()
// - DragInt()
// - DragInt2()
// - DragInt3()
// - DragInt4()
// - DragIntRange2()
// - DragScalar()
// - DragScalarN()
//-------------------------------------------------------------------------

bool ImGui::DragBehavior(ImGuiID id, ImGuiDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiContext& g = *GImGui;
    const ImGuiDataTypeInfo* type_info = DataTypeGetInfo(data_type);

    // Default tweak speed
    if (v_speed == 0.0f && (flags & ImGuiSliderFlags_NoSpeed))
        v_speed = 1.0f;

    // Default format string when passing NULL
    if (format == NULL)
        format = type_info->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // Sanity check for "%d" versus "%i"
        IM_ASSERT(0 && "DragInt(): format string \"\%d\" expected.");
    else if (data_type == ImGuiDataType_U32 && strcmp(format, "%u") != 0)
        IM_ASSERT(0 && "DragInt(): format string \"\%u\" expected.");

    // Tweak single value with mouse delta.
    // Note that we intentionally don't clamp boundaries after every drag modified value, because doing so would lose precision of data outside bounds.
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        float mouse_drag_delta = g.IO.MouseDelta.x;
        if (g.IO.KeyAlt)
            mouse_drag_delta *= 0.1f;
        if (g.IO.KeyShift)
            mouse_drag_delta *= 10.0f;

        if (mouse_drag_delta != 0.0f)
        {
            if (DataTypeApplyFromText(format, data_type, p_v, g.DragCurrentAccumulation))
                value_changed = true;
            g.DragCurrentAccumulation = 0.0f;
            DataTypeApplyOp(data_type, '+', p_v, p_v, &mouse_drag_delta);
        }
    }

    // Clamp value to min/max boundaries.
    if (p_min && p_max && DataTypeCompare(data_type, p_min, p_max) > 0)
        ImSwap(p_min, p_max);
    if (p_min && DataTypeCompare(data_type, p_v, p_min) < 0)
    {
        *p_v = *p_min;
        value_changed = true;
    }
    if (p_max && DataTypeCompare(data_type, p_v, p_max) > 0)
    {
        *p_v = *p_max;
        value_changed = true;
    }

    return value_changed;
}

bool ImGui::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    bool value_changed = DragFloat("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragFloat("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, format_max ? format_max : format, flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

bool ImGui::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_S32, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_S32, v, 4, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    bool value_changed = DragInt("##min", v_current_min, v_speed, (v_min >= v_max) ? INT_MIN : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragInt("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? INT_MAX : v_max, format_max ? format_max : format, flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Input Fields
//-------------------------------------------------------------------------
// - InputText()
// - InputTextMultiline()
// - InputTextWithHint()
// - InputFloat()
// - InputFloat2()
// - InputFloat3()
// - InputFloat4()
// - InputInt()
// - InputInt2()
// - InputInt3()
// - InputInt4()
// - InputDouble()
// - InputScalar()
// - InputScalarN()
//-------------------------------------------------------------------------

bool ImGui::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    return InputTextEx(label, NULL, buf, (int)buf_size, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline() or use ImGuiInputTextFlags_Multiline
    return InputTextEx(label, hint, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

bool ImGui::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
    flags |= ImGuiInputTextFlags_CharsScientific;
    return InputScalar(label, ImGuiDataType_Float, (void*)v, (const void*)(step > 0.0f ? &step : NULL), (const void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
}

bool ImGui::InputFloat2(const char* label, float v[2], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool ImGui::InputFloat3(const char* label, float v[3], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool ImGui::InputFloat4(const char* label, float v[4], const char* format, ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_Float, v, 4, NULL, NULL, format, flags);
}

bool ImGui::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    // Hex format is available using InputText() with ImGuiInputTextFlags_CharsHexadecimal
    return InputScalar(label, ImGuiDataType_S32, (void*)v, (const void*)(step > 0.0f ? &step : NULL), (const void*)(step_fast > 0.0f ? &step_fast : NULL), "%d", flags);
}

bool ImGui::InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 2, NULL, NULL, "%d", flags);
}

bool ImGui::InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 3, NULL, NULL, "%d", flags);
}

bool ImGui::InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags)
{
    return InputScalarN(label, ImGuiDataType_S32, v, 4, NULL, NULL, "%d", flags);
}

bool ImGui::InputDouble(const char* label, double* v, double step, double step_fast, const char* format, ImGuiInputTextFlags flags)
{
    flags |= ImGuiInputTextFlags_CharsScientific;
    return InputScalar(label, ImGuiDataType_Double, (void*)v, (const void*)(step > 0.0f ? &step : NULL), (const void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
}

bool ImGui::InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 frame_size = CalcItemSize(ImVec2(0,0), CalcItemWidth(), GetFrameHeight());
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb))
        return false;

    // Tabbing or CTRL-clicking on the label turns it into an input box
    const bool hovered = ItemHoverable(frame_bb, g.LastItemData.ID, g.LastItemData.InFlags);
    bool temp_input_is_active = TempInputIsActive(g.LastItemData.ID);
    bool temp_input_start = false;
    if (!temp_input_is_active)
    {
        const bool focus_requested = FocusableItemRegister(window, g.LastItemData.ID);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool double_clicked = (hovered && g.IO.MouseDoubleClicked[0]);
        if (focus_requested || clicked || double_clicked)
        {
            ActivateItem(g.LastItemData.ID);
            temp_input_start = true;
        }
    }
    if (temp_input_is_active || temp_input_start)
        return TempInputScalar(frame_bb, g.LastItemData.ID, label, data_type, p_data, format, p_step, p_step_fast);

    // Render frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == g.LastItemData.ID ? ImGuiCol_FrameBgActive : g.HoveredId == g.LastItemData.ID ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    // Render text
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return false;
}

bool ImGui::InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        if (i > 0)
            SameLine(0, g.Style.ItemInnerSpacing.x);
        value_changed |= InputScalar("", data_type, (char*)p_data + g.DataTypeInfo[data_type].Size * i, p_step, p_step_fast, format, flags);
        PopID();
        PopItemWidth();
    }
    PopID();

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Color Editor/Picker
//-------------------------------------------------------------------------
// - ColorButton()
// - ColorEdit3()
// - ColorEdit4()
// - ColorPicker3()
// - ColorPicker4()
//-------------------------------------------------------------------------

bool ImGui::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    const float default_size = GetFrameHeight();
    if (size.x == 0.0f)
        size.x = default_size;
    if (size.y == 0.0f)
        size.y = default_size;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha)
        flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -0.75f; // The border (using AddRect) is centered on pixels, given the thickness of 1.0f it'll bleed over half a pixel everywhere. So we need to reduce the size a bit.
        bb_inner.Expand(off);
    }
    if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col.w < 1.0f)
    {
        float mid_x = (float)(int)((bb_inner.Min.x + bb_inner.Max.x) * 0.5f + 0.5f);
        RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y), bb_inner.Max, GetColorU32(col), grid_step, ImVec2(-grid_step, 0.0f), rounding, ImDrawFlags_RoundCornersRight);
        window->DrawList->AddRectFilled(bb_inner.Min, ImVec2(mid_x, bb_inner.Max.y), GetColorU32(col_rgb_without_alpha), rounding, ImDrawFlags_RoundCornersLeft);
    }
    else
    {
        // Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
        ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col : col_rgb_without_alpha;
        if (col_source.w < 1.0f)
            RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(col_source), grid_step, ImVec2(0, 0), rounding);
        else
            window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), rounding);
    }
    RenderNavHighlight(bb, id);
    if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        if (g.Style.FrameBorderSize > 0.0f)
            RenderFrameBorder(bb.Min, bb.Max, rounding);
        else
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), rounding); // Color button are often in need of some sort of border
    }

    // Drag and Drop Source
    if ((flags & ImGuiColorEditFlags_NoDragDrop) == 0)
        if (BeginDragDropSource())
        {
            if (flags & ImGuiColorEditFlags_NoAlpha)
                SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col, sizeof(float) * 3, ImGuiCond_Once);
            else
                SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col, sizeof(float) * 4, ImGuiCond_Once);
            ColorButton(desc_id, col, flags);
            SameLine();
            TextEx("Color");
            EndDragDropSource();
        }

    // Tooltip
    if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
        ColorTooltip(desc_id, &col, flags & (ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

    return pressed;
}

bool ImGui::ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    return ColorEdit4(label, col, flags | ImGuiColorEditFlags_NoAlpha);
}

bool ImGui::ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w_full = CalcItemWidth();
    const float w_button = GetFrameHeight();
    const float w_spacing = style.ItemInnerSpacing.x;
    const float w_inputs = w_full - w_button - w_spacing;
    const char* label_display_end = FindRenderedTextEnd(label);

    BeginGroup();
    PushID(label);

    // If we're not showing any slider there's no point in rendering the button part of the widget.
    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs)
        flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    // Context menu: display and modify options (before inputs)
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorEditOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_DisplayMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check only one is set
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_)); // Check only one is set

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    // Convert to the formats we need
    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
    float f_backup[4];
    memcpy(f_backup, f, sizeof(f));

    // Inputs
    if (!(flags & ImGuiColorEditFlags_NoInputs))
    {
        // Push multi-items layout
        PushMultiItemsWidths(components, w_inputs);

        // RGB/HSV inputs
        bool value_changed = false;
        if (flags & ImGuiColorEditFlags_DisplayRGB)
        {
            value_changed |= DragFloat("##X", &f[0], 1.0f / 255.0f, 0.0f, 1.0f, "R: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Y", &f[1], 1.0f / 255.0f, 0.0f, 1.0f, "G: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Z", &f[2], 1.0f / 255.0f, 0.0f, 1.0f, "B: %.3f", ImGuiSliderFlags_None);
        }
        if (flags & ImGuiColorEditFlags_DisplayHSV)
        {
            value_changed |= DragFloat("##X", &f[0], 1.0f / 360.0f, 0.0f, 1.0f, "H: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Y", &f[1], 1.0f / 100.0f, 0.0f, 1.0f, "S: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Z", &f[2], 1.0f / 100.0f, 0.0f, 1.0f, "V: %.3f", ImGuiSliderFlags_None);
        }
        if (alpha)
        {
            SameLine();
            value_changed |= DragFloat("##W", &f[3], 1.0f / 255.0f, 0.0f, 1.0f, "A: %.3f", ImGuiSliderFlags_None);
        }

        // Pop multi-items layout
        PopItemWidth();
        PopItemWidth();
        PopItemWidth();
        if (alpha) PopItemWidth();
    }

    // Convert back
    if (value_changed)
    {
        if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], col[0], col[1], col[2]);
        else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], col[0], col[1], col[2]);
        else
        {
            col[0] = f[0];
            col[1] = f[1];
            col[2] = f[2];
        }
        if (alpha)
            col[3] = f[3];
    }

    const ImU32 col_display = ColorConvertFloat4ToU32(f);

    // Button
    SameLine(0, w_spacing);
    if (ColorButton("##ColorButton", ImVec4(f[0],f[1],f[2],f[3]), flags_untouched, ImVec2(w_button, 0)))
    {
        // Don't set the picker's color based on the current color, it will be done next frame in the picker logic.
        // This is to avoid a flicker when the user clicks on the button, as the picker will be displayed before the color is updated.
        // See https://github.com/ocornut/imgui/issues/346
        //g.ColorPickerRef = col;
    }

    // Label
    if (label != label_display_end)
    {
        SameLine(0, style.ItemInnerSpacing.x);
        TextEx(label, label_display_end);
    }

    PopID();
    EndGroup();

    // Revert to backup color on ESC
    if (g.ActiveId == 0 && memcmp(f, f_backup, sizeof(float) * components) != 0)
        value_changed = true;

    return value_changed;
}

bool ImGui::ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
{
    float col4[4] = { col[0], col[1], col[2], 1.0f };
    if (!ColorPicker4(label, col4, flags | ImGuiColorEditFlags_NoAlpha))
        return false;
    col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];
    return true;
}

bool ImGui::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float w_extra = (flags & ImGuiColorEditFlags_NoSidePreview) ? 0.0f : (g.FontSize * 7.0f + style.ItemSpacing.x);
    const float w_items_all = CalcItemWidth() - w_extra;

    // If we're not showing any slider there's no point in rendering the button part of the widget.
    if (flags & ImGuiColorEditFlags_NoInputs)
        flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    // Context menu: display and modify options (before inputs)
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorPickerOptionsPopup(col, flags);

    // Read stored options
    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check only one is set
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_)); // Check only one is set

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    // Convert to the formats we need
    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
    float f_backup[4];
    memcpy(f_backup, f, sizeof(f));

    bool value_changed = false;

    BeginGroup();
    PushID(label);

    // Picker
    if (!(flags & ImGuiColorEditFlags_NoPicker))
    {
        // SV plane
        const float sv_picker_size = ImMax(w_items_all - (alpha ? style.ItemInnerSpacing.x + g.FontSize : 0.0f), g.FontSize * 10.0f);
        value_changed |= ColorPicker_DrawSV(f, flags, sv_picker_size);

        // Hue slider
        if (flags & ImGuiColorEditFlags_DisplayHSV)
        {
            SameLine();
            value_changed |= ColorPicker_DrawHueSlider(f, flags, sv_picker_size);
        }
    }

    // Inputs
    if (!(flags & ImGuiColorEditFlags_NoInputs))
    {
        PushMultiItemsWidths(components, w_items_all);
        if (flags & ImGuiColorEditFlags_DisplayRGB)
        {
            value_changed |= DragFloat("##X", &f[0], 1.0f / 255.0f, 0.0f, 1.0f, "R: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Y", &f[1], 1.0f / 255.0f, 0.0f, 1.0f, "G: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Z", &f[2], 1.0f / 255.0f, 0.0f, 1.0f, "B: %.3f", ImGuiSliderFlags_None);
        }
        if (flags & ImGuiColorEditFlags_DisplayHSV)
        {
            value_changed |= DragFloat("##X", &f[0], 1.0f / 360.0f, 0.0f, 1.0f, "H: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Y", &f[1], 1.0f / 100.0f, 0.0f, 1.0f, "S: %.3f", ImGuiSliderFlags_None);
            SameLine();
            value_changed |= DragFloat("##Z", &f[2], 1.0f / 100.0f, 0.0f, 1.0f, "V: %.3f", ImGuiSliderFlags_None);
        }
        if (alpha)
        {
            SameLine();
            value_changed |= DragFloat("##W", &f[3], 1.0f / 255.0f, 0.0f, 1.0f, "A: %.3f", ImGuiSliderFlags_None);
        }
        PopItemWidth();
        PopItemWidth();
        PopItemWidth();
        if (alpha) PopItemWidth();
    }

    // Side preview
    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
    {
        SameLine(0, style.ItemInnerSpacing.x);
        BeginGroup();
        TextEx("Current");
        ColorButton("##current", ImVec4(f[0], f[1], f[2], f[3]), (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_NoPicker, ImVec2(g.FontSize * 7.0f, 0));
        if (ref_col)
        {
            TextEx("Original");
            if (ColorButton("##original", *(const ImVec4*)ref_col, (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_NoPicker, ImVec2(g.FontSize * 7.0f, 0)))
            {
                memcpy(f, ref_col, sizeof(float) * components);
                value_changed = true;
            }
        }
        EndGroup();
    }

    // Convert back
    if (value_changed)
    {
        if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], col[0], col[1], col[2]);
        else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], col[0], col[1], col[2]);
        else
        {
            col[0] = f[0];
            col[1] = f[1];
            col[2] = f[2];
        }
        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    // Revert to backup color on ESC
    if (g.ActiveId == 0 && memcmp(f, f_backup, sizeof(float) * components) != 0)
        value_changed = true;

    return value_changed;
}

// Helper for ColorPicker4()
bool ImGui::ColorPicker_DrawSV(float col[4], ImGuiColorEditFlags flags, float size)
{
    bool value_changed = false;
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    ImVec2 picker_pos = window->DC.CursorPos;
    float hue, sat, val;
    ColorConvertRGBtoHSV(col[0], col[1], col[2], hue, sat, val);

    ImDrawList* draw_list = window->DrawList;
    draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(size, size), IM_COL32(255,255,255,255), IM_COL32(255,255,255,255), IM_COL32(0,0,0,255), IM_COL32(0,0,0,255));
    float h,s,v;
    ColorConvertRGBtoHSV(1,1,1,h,s,v);
    ImU32 col_hues[7];
    for (int i = 0; i < 7; ++i)
    {
        h = i / 6.0f;
        ColorConvertHSVtoRGB(h, 1.0f, 1.0f, col_hues[i]);
    }
    for (int i = 0; i < 6; ++i)
        draw_list->AddRectFilledMultiColor(picker_pos + ImVec2(i * size / 6, 0), picker_pos + ImVec2((i + 1) * size / 6, size), col_hues[i], col_hues[i+1], col_hues[i+1], col_hues[i]);

    // Slider
    PushID("#saturation");
    InvisibleButton("saturation", ImVec2(size, size));
    if (IsItemActive())
    {
        sat = ImSaturate((g.IO.MousePos.x - picker_pos.x) / (size - 1.0f));
        val = 1.0f - ImSaturate((g.IO.MousePos.y - picker_pos.y) / (size - 1.0f));
        value_changed = true;
    }
    PopID();

    // Draw picker circle
    float r = g.FontSize * 0.4f;
    ImVec2 p = ImVec2(picker_pos.x + sat * (size-1), picker_pos.y + (1-val) * (size-1));
    draw_list->AddCircleFilled(p, r, IM_COL32(0,0,0,255), 12);
    draw_list->AddCircleFilled(p, r-2, IM_COL32(255,255,255,255), 12);

    if (value_changed)
        ColorConvertHSVtoRGB(hue, sat, val, col[0], col[1], col[2]);

    return value_changed;
}

// Helper for ColorPicker4()
bool ImGui::ColorPicker_DrawHueSlider(float col[4], ImGuiColorEditFlags flags, float size)
{
    bool value_changed = false;
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiContext& g = *GImGui;

    ImVec2 slider_pos = window->DC.CursorPos;
    float hue, sat, val;
    ColorConvertRGBtoHSV(col[0], col[1], col[2], hue, sat, val);

    ImDrawList* draw_list = window->DrawList;
    ImU32 col_hues[7];
    for (int i = 0; i < 7; ++i)
    {
        float h = i / 6.0f;
        ColorConvertHSVtoRGB(h, 1.0f, 1.0f, col_hues[i]);
    }
    for (int i = 0; i < 6; ++i)
        draw_list->AddRectFilledMultiColor(slider_pos + ImVec2(0, i * size / 6), slider_pos + ImVec2(g.FontSize, (i + 1) * size / 6), col_hues[i], col_hues[i], col_hues[i+1], col_hues[i+1]);

    // Slider
    PushID("#hue");
    InvisibleButton("hue", ImVec2(g.FontSize, size));
    if (IsItemActive())
    {
        hue = ImSaturate((g.IO.MousePos.y - slider_pos.y) / (size - 1.0f));
        value_changed = true;
    }
    PopID();

    // Draw picker circle
    float r = g.FontSize * 0.4f;
    ImVec2 p = ImVec2(slider_pos.x + g.FontSize * 0.5f, slider_pos.y + hue * (size-1));
    draw_list->AddRectFilled(ImVec2(p.x - r, p.y - 2), ImVec2(p.x + r, p.y + 2), IM_COL32(0,0,0,255));
    draw_list->AddRectFilled(ImVec2(p.x - r + 1, p.y - 1), ImVec2(p.x + r - 1, p.y + 1), IM_COL32(255,255,255,255));

    if (value_changed)
        ColorConvertHSVtoRGB(hue, sat, val, col[0], col[1], col[2]);

    return value_changed;
}

// Helper for ColorEdit4() / ColorPicker4()
void ImGui::ColorTooltip(const char* text, const float* col, ImGuiColorEditFlags flags)
{
    ImGuiContext& g = *GImGui;

    BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None);
    const char* text_end = text ? FindRenderedTextEnd(text, NULL) : text;
    if (text_end > text)
    {
        TextEx(text, text_end);
        Separator();
    }

    ImVec2 sz(g.FontSize * 3 + g.Style.ItemSpacing.y * 2, g.FontSize * 3 + g.Style.ItemSpacing.y * 2);
    ColorButton("##preview", *(const ImVec4*)col, (flags & (ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf)) | ImGuiColorEditFlags_NoTooltip, sz);
    SameLine();
    if ((flags & ImGuiColorEditFlags_InputMask_) == 0)
        flags |= ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayHex;
    if (flags & ImGuiColorEditFlags_DisplayRGB)
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)",
                (int)(col[0]*255.0f), (int)(col[1]*255.0f), (int)(col[2]*255.0f),
                (int)(col[0]*255.0f), (int)(col[1]*255.0f), (int)(col[2]*255.0f),
                col[0], col[1], col[2]);
        else
            Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)",
                (int)(col[0]*255.0f), (int)(col[1]*255.0f), (int)(col[2]*255.0f), (int)(col[3]*255.0f),
                (int)(col[0]*255.0f), (int)(col[1]*255.0f), (int)(col[2]*255.0f), (int)(col[3]*255.0f),
                col[0], col[1], col[2], col[3]);
    }
    else if (flags & ImGuiColorEditFlags_DisplayHSV)
    {
        float h, s, v;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], h, s, v);
        Text("H: %.3f, S: %.3f, V: %.3f", h, s, v);
    }
    EndTooltip();
}

// Helper for ColorEdit4() / ColorPicker4()
void ImGui::ColorEditOptionsPopup(const float* col, ImGuiColorEditFlags flags)
{
    bool allow_opt_inputs = !(flags & ImGuiColorEditFlags_NoInputs);
    bool allow_opt_datatype = !(flags & ImGuiColorEditFlags_NoDataType);

    if ((!allow_opt_inputs && !allow_opt_datatype) || !BeginPopup("context"))
        return;

    ImGuiContext& g = *GImGui;
    if (allow_opt_inputs)
    {
        if (RadioButton("RGB", (g.ColorEditOptions & ImGuiColorEditFlags_DisplayRGB) != 0)) g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_DisplayMask_) | ImGuiColorEditFlags_DisplayRGB;
        SameLine();
        if (RadioButton("HSV", (g.ColorEditOptions & ImGuiColorEditFlags_DisplayHSV) != 0)) g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_DisplayMask_) | ImGuiColorEditFlags_DisplayHSV;
        SameLine();
        if (RadioButton("Hex", (g.ColorEditOptions & ImGuiColorEditFlags_DisplayHex) != 0)) g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_DisplayMask_) | ImGuiColorEditFlags_DisplayHex;
    }
    if (allow_opt_datatype)
    {
        if (allow_opt_inputs) Separator();
        if (RadioButton("0..255", (g.ColorEditOptions & ImGuiColorEditFlags_InputInt) != 0)) g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_InputMask_) | ImGuiColorEditFlags_InputInt;
        SameLine();
        if (RadioButton("0.0..1.0", (g.ColorEditOptions & ImGuiColorEditFlags_InputFloat) != 0)) g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_InputMask_) | ImGuiColorEditFlags_InputFloat;
    }

    EndPopup();
}

// Helper for ColorPicker4()
void ImGui::ColorPickerOptionsPopup(const float* ref_col, ImGuiColorEditFlags flags)
{
    bool allow_opt_picker = !(flags & ImGuiColorEditFlags_NoPicker);
    bool allow_opt_alpha_bar = !(flags & ImGuiColorEditFlags_NoAlpha) && !(flags & ImGuiColorEditFlags_NoAlphaBar);

    if ((!allow_opt_picker && !allow_opt_alpha_bar) || !BeginPopup("context"))
        return;

    ImGuiContext& g = *GImGui;
    if (allow_opt_picker)
    {
        ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from functions below.
        PushItemWidth(picker_size.x);
        for (int picker_type = 0; picker_type < 2; picker_type++)
        {
            // Draw small/thumbnail version of each picker type
            if (picker_type > 0) SameLine();
            if (RadioButton(picker_type == 0 ? "Wheel" : "Square", (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) == (ImGuiColorEditFlags_PickerWheel << picker_type)))
                g.ColorEditOptions = (g.ColorEditOptions & ~ImGuiColorEditFlags_PickerMask_) | (ImGuiColorEditFlags_PickerWheel << picker_type);
            if (g.ColorEditOptions & (ImGuiColorEditFlags_PickerWheel << picker_type))
            {
                // Draw preview of the picker type
                ImVec2 p(GetCursorScreenPos().x + (g.FontSize + g.Style.FramePadding.x), GetCursorScreenPos().y);
                float ch, cs, cv;
                ColorConvertRGBtoHSV(ref_col[0], ref_col[1], ref_col[2], ch, cs, cv);
                if (picker_type == 0)
                    ColorPicker_DrawHueWheel(p, picker_size, ch, cs, cv);
                else
                    ColorPicker_DrawSV(ref_col, 0, picker_size);
            }
        }
        PopItemWidth();
    }
    if (allow_opt_alpha_bar)
    {
        if (allow_opt_picker) Separator();
        CheckboxFlags("Alpha Bar", &g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar);
    }
    EndPopup();
}

#endif // #ifndef IMGUI_DISABLE
