// dear imgui, v1.90.8 WIP
// (tables and columns code)

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
// [SECTION] Tables
//-----------------------------------------------------------------------------
// - This is a transitionary file, all functions will end up in imgui.cpp
// - We are using "tr" instead of "row" because of the frequent collision with the word "row" in C/C++.
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
// [SECTION] Tables: Main code
//-------------------------------------------------------------------------
// - ImGui::BeginTable()
// - ImGui::EndTable()
// - ImGui::TableNextRow()
// - ImGui::TableNextColumn()
// - ImGui::TableSetColumnIndex()
//-------------------------------------------------------------------------

// Configuration
static const int TABLE_MAX_COLUMNS = 64;

// Helper
static inline int TableGetColumnIdx(ImGuiTable* table, int n) { return (table->Flags & ImGuiTableFlags_Reorderable) ? table->DisplayOrderToIndex[n] : n; }

// Helper to get the column by its DisplayOrder
static inline ImGuiTableColumn* TableGetColumn(ImGuiTable* table, int n) { return &table->Columns[TableGetColumnIdx(table, n)]; }

// Helper to get the column by its IndexWithinEnabledSet
static inline ImGuiTableColumn* TableGetColumnByEnabledIndex(ImGuiTable* table, int n) { return &table->Columns[table->EnabledColumns[n]]; }

// Helper to get the column by its UserID
static inline ImGuiTableColumn* TableGetColumnByUserID(ImGuiTable* table, ImGuiID id)
{
    for (int i = 0; i < table->ColumnsCount; i++)
        if (table->Columns[i].UserID == id)
            return &table->Columns[i];
    return NULL;
}

// Helper to get the column by its name
static inline ImGuiTableColumn* TableGetColumnByName(ImGuiTable* table, const char* name)
{
    ImGuiID id = ImHashStr(name);
    return TableGetColumnByUserID(table, id);
}

// Find the column index in the DisplayOrder array that matches the given column index in the Columns array.
// This is used to find the column's display order from its storage order.
static int TableGetColumnDisplayOrder(ImGuiTable* table, int column_idx)
{
    for (int i = 0; i < table->ColumnsCount; i++)
        if (table->DisplayOrderToIndex[i] == column_idx)
            return i;
    return -1;
}

// Find the column's storage index from its display order.
static int TableGetColumnStorageIndex(ImGuiTable* table, int display_order)
{
    return table->DisplayOrderToIndex[display_order];
}

// Find the column's index in the EnabledColumns array.
static int TableGetColumnEnabledIndex(ImGuiTable* table, int column_idx)
{
    for (int i = 0; i < table->EnabledColumnsCount; i++)
        if (table->EnabledColumns[i] == column_idx)
            return i;
    return -1;
}

// Get the column's display order from its enabled index.
static int TableGetColumnDisplayOrderFromEnabledIndex(ImGuiTable* table, int enabled_idx)
{
    return TableGetColumnDisplayOrder(table, table->EnabledColumns[enabled_idx]);
}

// Get the column's enabled index from its display order.
static int TableGetColumnEnabledIndexFromDisplayOrder(ImGuiTable* table, int display_order)
{
    return TableGetColumnEnabledIndex(table, table->DisplayOrderToIndex[display_order]);
}

// Get the column's storage index from its enabled index.
static int TableGetColumnStorageIndexFromEnabledIndex(ImGuiTable* table, int enabled_idx)
{
    return table->EnabledColumns[enabled_idx];
}

// Get the column's enabled index from its storage index.
static int TableGetColumnEnabledIndexFromStorageIndex(ImGuiTable* table, int storage_idx)
{
    for (int i = 0; i < table->EnabledColumnsCount; i++)
        if (table->EnabledColumns[i] == storage_idx)
            return i;
    return -1;
}

// Get the column's display order from its storage index.
static int TableGetColumnDisplayOrderFromStorageIndex(ImGuiTable* table, int storage_idx)
{
    return TableGetColumnDisplayOrder(table, storage_idx);
}

// Get the column's storage index from its display order.
static int TableGetColumnStorageIndexFromDisplayOrder(ImGuiTable* table, int display_order)
{
    return table->DisplayOrderToIndex[display_order];
}

// Get the column's enabled index from its UserID.
static int TableGetColumnEnabledIndexFromUserID(ImGuiTable* table, ImGuiID id)
{
    for (int i = 0; i < table->EnabledColumnsCount; i++)
        if (table->Columns[table->EnabledColumns[i]].UserID == id)
            return i;
    return -1;
}

// Get the column's UserID from its enabled index.
static ImGuiID TableGetColumnUserIDFromEnabledIndex(ImGuiTable* table, int enabled_idx)
{
    return table->Columns[table->EnabledColumns[enabled_idx]].UserID;
}

// Get the column's display order from its UserID.
static int TableGetColumnDisplayOrderFromUserID(ImGuiTable* table, ImGuiID id)
{
    for (int i = 0; i < table->ColumnsCount; i++)
        if (table->Columns[table->DisplayOrderToIndex[i]].UserID == id)
            return i;
    return -1;
}

// Get the column's UserID from its display order.
static ImGuiID TableGetColumnUserIDFromDisplayOrder(ImGuiTable* table, int display_order)
{
    return table->Columns[table->DisplayOrderToIndex[display_order]].UserID;
}

// Get the column's storage index from its UserID.
static int TableGetColumnStorageIndexFromUserID(ImGuiTable* table, ImGuiID id)
{
    for (int i = 0; i < table->ColumnsCount; i++)
        if (table->Columns[i].UserID == id)
            return i;
    return -1;
}

// Get the column's UserID from its storage index.
static ImGuiID TableGetColumnUserIDFromStorageIndex(ImGuiTable* table, int storage_idx)
{
    return table->Columns[storage_idx].UserID;
}

// Get the column's name from its storage index.
const char* ImGui::TableGetColumnName(int column_n)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return NULL;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return NULL;
    return table->Columns[column_n].Name;
}

// Get the column's name from its display order.
const char* ImGui::TableGetColumnName(ImGuiTable* table, int column_n)
{
    if (!table)
        return NULL;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return NULL;
    return table->Columns[TableGetColumnIdx(table, column_n)].Name;
}

// Get the column's flags from its storage index.
ImGuiTableColumnFlags ImGui::TableGetColumnFlags(int column_n)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return ImGuiTableColumnFlags_None;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return ImGuiTableColumnFlags_None;
    return table->Columns[column_n].Flags;
}

// Get the column's flags from its display order.
ImGuiTableColumnFlags ImGui::TableGetColumnFlags(ImGuiTable* table, int column_n)
{
    if (!table)
        return ImGuiTableColumnFlags_None;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return ImGuiTableColumnFlags_None;
    return table->Columns[TableGetColumnIdx(table, column_n)].Flags;
}

// Get the column's index from its name.
int ImGui::TableGetColumnIndex(const char* name)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, ImHashStr(name));
}

// Get the column's index from its name.
int ImGui::TableGetColumnIndex(ImGuiTable* table, const char* name)
{
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, ImHashStr(name));
}

// Get the column's index from its UserID.
int ImGui::TableGetColumnIndex(ImGuiID id)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, id);
}

// Get the column's index from its UserID.
int ImGui::TableGetColumnIndex(ImGuiTable* table, ImGuiID id)
{
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, id);
}

// Get the number of columns in the current table.
int ImGui::TableGetColumnCount()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    return table ? table->ColumnsCount : 0;
}

// Get the number of columns in the given table.
int ImGui::TableGetColumnCount(ImGuiTable* table)
{
    return table ? table->ColumnsCount : 0;
}

// Get the current column index.
int ImGui::TableGetColumnIndex()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return -1;
    return table->CurrentColumn;
}

// Get the current row index.
int ImGui::TableGetRowIndex()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return -1;
    return table->CurrentRow;
}

// Get the sort specs for the current table.
const ImGuiTableSortSpecs* ImGui::TableGetSortSpecs()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return NULL;
    return &table->SortSpecs;
}

// Get the sort specs for the given table.
const ImGuiTableSortSpecs* ImGui::TableGetSortSpecs(ImGuiTable* table)
{
    if (!table)
        return NULL;
    return &table->SortSpecs;
}

// Set the sort specs for the given table.
void ImGui::TableSetSortSpecs(ImGuiTable* table, const ImGuiTableSortSpecs* sort_specs, bool sort_direction_is_ascending)
{
    if (!table)
        return;
    table->SortSpecs = *sort_specs;
    table->SortSpecs.SortDirection = sort_direction_is_ascending ? ImGuiSortDirection_Ascending : ImGuiSortDirection_Descending;
    table->SortSpecsDirty = true;
}

// Set the sort specs for the current table.
void ImGui::TableSetSortSpecs(const ImGuiTableSortSpecs* sort_specs, bool sort_direction_is_ascending)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;
    TableSetSortSpecs(table, sort_specs, sort_direction_is_ascending);
}

// Begin a table.
bool ImGui::BeginTable(const char* str_id, int column, ImGuiTableFlags flags, const ImVec2& outer_size, float inner_width)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    IM_ASSERT(column > 0 && column <= TABLE_MAX_COLUMNS);

    // Find existing table
    ImGuiID id = window->GetID(str_id);
    ImGuiTable* table = g.Tables.GetByKey(id);
    if (!table)
    {
        table = IM_NEW(ImGuiTable)();
        table->ID = id;
        g.Tables.SetByKey(id, table);
    }
    const int instance_no = table->MemoryCompacted ? 0 : table->InstanceCurrent;
    const ImGuiID instance_id = id + instance_no;
    const int table_last_frame_active = table->LastFrameActive;
    const int table_instance_last_frame_active = (instance_no < table->Instances.Size) ? table->Instances[instance_no].LastFrameActive : -1;

    // If the instance is not active, we can't use it.
    if (table_instance_last_frame_active != -1 && table_instance_last_frame_active != g.FrameCount - 1)
        table = NULL;

    // If the table is not active, we can't use it.
    if (table && table_last_frame_active != -1 && table_last_frame_active != g.FrameCount - 1)
        table = NULL;

    // Create a new table if we don't have one
    if (!table)
    {
        table = IM_NEW(ImGuiTable)();
        table->ID = id;
        g.Tables.SetByKey(id, table);
    }
    table->LastFrameActive = g.FrameCount;
    if (instance_no >= table->Instances.Size)
        table->Instances.resize(instance_no + 1);
    table->Instances[instance_no].LastFrameActive = g.FrameCount;

    // Set the current table
    g.CurrentTable = table;
    table->CurrentColumn = -1;
    table->CurrentRow = -1;
    table->Flags = flags;
    table->OuterWindow = table->InnerWindow = window;
    table->OuterRect = window->WorkRect;
    table->InnerWidth = inner_width;

    // Set the number of columns
    if (column > 0)
        TableSetColumnCount(table, column);

    // Set the outer size
    if (outer_size.x > 0.0f)
        table->OuterRect.Max.x = table->OuterRect.Min.x + outer_size.x;
    if (outer_size.y > 0.0f)
        table->OuterRect.Max.y = table->OuterRect.Min.y + outer_size.y;

    // Push a new table on the stack
    g.CurrentTableStack.push_back(table);

    // Begin the inner window
    if (flags & ImGuiTableFlags_NoHostExtendX)
        table->InnerWindow = window;
    else
    {
        // We need to create a child window to host our table.
        // This is so that we can scroll horizontally without affecting the parent window.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
        if (flags & ImGuiTableFlags_ScrollX)
            window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        char child_id[64];
        sprintf(child_id, "##table_%08x", id);
        BeginChild(child_id, ImVec2(0,0), false, window_flags);
        table->InnerWindow = g.CurrentWindow;
        table->InnerWindow->ScrollMax.y = 0.0f; // Disable vertical scrolling
    }

    // Initialize the columns
    if (table->ColumnsCount > 0)
        TableSetupColumns(table);

    return true;
}

// End a table.
void ImGui::EndTable()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Pop the table from the stack
    g.CurrentTableStack.pop_back();
    g.CurrentTable = g.CurrentTableStack.empty() ? NULL : g.CurrentTableStack.back();

    // End the inner window
    if (table->InnerWindow != table->OuterWindow)
        EndChild();

    // Reset the table
    table->CurrentColumn = -1;
    table->CurrentRow = -1;
}

// Go to the next row.
void ImGui::TableNextRow(ImGuiTableRowFlags row_flags, float min_row_height)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Go to the next row
    table->CurrentRow++;
    table->RowFlags = row_flags;
    table->RowMinHeight = min_row_height;

    // Reset the column index
    table->CurrentColumn = -1;
}

// Go to the next column.
bool ImGui::TableNextColumn()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return false;

    // Go to the next column
    table->CurrentColumn++;
    if (table->CurrentColumn >= table->ColumnsCount)
    {
        table->CurrentColumn = 0;
        TableNextRow();
    }

    // Set the cursor position
    ImGuiTableColumn* column = TableGetColumn(table, table->CurrentColumn);
    ImGui::SetCursorPosX(column->MinX - table->InnerWindow->Scroll.x);
    return true;
}

// Set the current column index.
bool ImGui::TableSetColumnIndex(int column_n)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return false;

    // Set the column index
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return false;
    table->CurrentColumn = column_n;

    // Set the cursor position
    ImGuiTableColumn* column = TableGetColumn(table, table->CurrentColumn);
    ImGui::SetCursorPosX(column->MinX - table->InnerWindow->Scroll.x);
    return true;
}

//-------------------------------------------------------------------------
// [SECTION] Tables: Headers
//-------------------------------------------------------------------------
// - ImGui::TableSetupColumn()
// - ImGui::TableSetupScrollFreeze()
// - ImGui::TableHeadersRow()
// - ImGui::TableHeader()
//-------------------------------------------------------------------------

// Setup a column.
void ImGui::TableSetupColumn(const char* label, ImGuiTableColumnFlags flags, float init_width_or_weight, ImGuiID user_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Add a new column
    if (table->ColumnsCount >= TABLE_MAX_COLUMNS)
        return;
    ImGuiTableColumn* column = &table->Columns[table->ColumnsCount];
    column->Name = label;
    column->Flags = flags;
    column->InitWidthOrWeight = init_width_or_weight;
    column->UserID = user_id;
    table->ColumnsCount++;
}

// Setup the number of columns to freeze.
void ImGui::TableSetupScrollFreeze(int cols, int rows)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    table->FreezeColumnsCount = (cols > 0) ? ImMin(cols, table->ColumnsCount) : 0;
    table->FreezeRowsCount = (rows > 0) ? rows : 0;
}

// Display the table headers.
void ImGui::TableHeadersRow()
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Display the headers
    for (int i = 0; i < table->ColumnsCount; i++)
    {
        if (TableNextColumn())
            TableHeader(TableGetColumnName(table, i));
    }
}

// Display a table header.
void ImGui::TableHeader(const char* label)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Get the column
    ImGuiTableColumn* column = TableGetColumn(table, table->CurrentColumn);

    // Display the header
    if (ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
    {
        // Sort the table
        if (column->Flags & ImGuiTableColumnFlags_NoSort)
            return;
        if (table->SortSpecs.ColumnUserID == column->UserID)
            table->SortSpecs.SortDirection = (table->SortSpecs.SortDirection == ImGuiSortDirection_Ascending) ? ImGuiSortDirection_Descending : ImGuiSortDirection_Ascending;
        else
        {
            table->SortSpecs.ColumnUserID = column->UserID;
            table->SortSpecs.SortDirection = ImGuiSortDirection_Ascending;
        }
        table->SortSpecsDirty = true;
    }

    // Display the sort arrow
    if (table->SortSpecs.ColumnUserID == column->UserID)
    {
        ImGui::SameLine();
        ImGui::Text(table->SortSpecs.SortDirection == ImGuiSortDirection_Ascending ? " /\\" : " \\/");
    }
}

//-------------------------------------------------------------------------
// [SECTION] Tables: Columns manipulation
//-------------------------------------------------------------------------
// - ImGui::TableGetColumnCount()
// - ImGui::TableGetColumnName()
// - ImGui::TableGetColumnFlags()
// - ImGui::TableGetColumnIndex()
// - ImGui::TableSetColumnEnabled()
// - ImGui::TableGetColumnWidth()
// - ImGui::TableSetColumnWidth()
//-------------------------------------------------------------------------

// Get the number of columns in the current table.
int ImGui::TableGetColumnCount(ImGuiTable* table)
{
    return table ? table->ColumnsCount : 0;
}

// Get the name of a column.
const char* ImGui::TableGetColumnName(ImGuiTable* table, int column_n)
{
    if (!table)
        return NULL;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return NULL;
    return table->Columns[TableGetColumnIdx(table, column_n)].Name;
}

// Get the flags of a column.
ImGuiTableColumnFlags ImGui::TableGetColumnFlags(ImGuiTable* table, int column_n)
{
    if (!table)
        return ImGuiTableColumnFlags_None;
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return ImGuiTableColumnFlags_None;
    return table->Columns[TableGetColumnIdx(table, column_n)].Flags;
}

// Get the index of a column from its name.
int ImGui::TableGetColumnIndex(ImGuiTable* table, const char* name)
{
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, ImHashStr(name));
}

// Get the index of a column from its UserID.
int ImGui::TableGetColumnIndex(ImGuiTable* table, ImGuiID id)
{
    if (!table)
        return -1;
    return TableGetColumnDisplayOrderFromUserID(table, id);
}

// Enable or disable a column.
void ImGui::TableSetColumnEnabled(int column_n, bool enabled)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Get the column
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return;
    ImGuiTableColumn* column = TableGetColumn(table, column_n);

    // Enable or disable the column
    if (enabled)
        column->Flags &= ~ImGuiTableColumnFlags_Disabled;
    else
        column->Flags |= ImGuiTableColumnFlags_Disabled;
}

// Get the width of a column.
float ImGui::TableGetColumnWidth(int column_n)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return 0.0f;

    // Get the column
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return 0.0f;
    ImGuiTableColumn* column = TableGetColumn(table, column_n);
    return column->WidthRequest;
}

// Set the width of a column.
void ImGui::TableSetColumnWidth(int column_n, float width)
{
    ImGuiContext& g = *GImGui;
    ImGuiTable* table = g.CurrentTable;
    if (!table)
        return;

    // Get the column
    if (column_n < 0 || column_n >= table->ColumnsCount)
        return;
    ImGuiTableColumn* column = TableGetColumn(table, column_n);

    // Set the width
    column->WidthRequest = width;
}

//-------------------------------------------------------------------------
// [SECTION] Tables: Sorting
//-------------------------------------------------------------------------
// - ImGui::TableGetSortSpecs()
// - ImGui::TableSetSortSpecs()
//-------------------------------------------------------------------------

// Get the sort specs for the current table.
const ImGuiTableSortSpecs* ImGui::TableGetSortSpecs(ImGuiTable* table)
{
    if (!table)
        return NULL;
    return &table->SortSpecs;
}

// Set the sort specs for the given table.
void ImGui::TableSetSortSpecs(ImGuiTable* table, const ImGuiTableSortSpecs* sort_specs, bool sort_direction_is_ascending)
{
    if (!table)
        return;
    table->SortSpecs = *sort_specs;
    table->SortSpecs.SortDirection = sort_direction_is_ascending ? ImGuiSortDirection_Ascending : ImGuiSortDirection_Descending;
    table->SortSpecsDirty = true;
}

//-------------------------------------------------------------------------
// [SECTION] Tables: Internal
//-------------------------------------------------------------------------

// Set the number of columns in a table.
static void TableSetColumnCount(ImGuiTable* table, int column_count)
{
    if (table->ColumnsCount == column_count)
        return;
    table->ColumnsCount = column_count;
    table->Columns.resize(column_count);
    table->DisplayOrderToIndex.resize(column_count);
    for (int i = 0; i < column_count; i++)
        table->DisplayOrderToIndex[i] = i;
}

// Setup the columns of a table.
static void TableSetupColumns(ImGuiTable* table)
{
    // Reset the columns
    for (int i = 0; i < table->ColumnsCount; i++)
    {
        ImGuiTableColumn* column = &table->Columns[i];
        column->MinX = column->MaxX = 0.0f;
        column->WidthRequest = -1.0f;
        column->WidthGiven = 0.0f;
        column->DisplayOrder = i;
    }

    // Calculate the width of the columns
    float width_auto_sum = 0.0f;
    float width_given_sum = 0.0f;
    int   width_auto_count = 0;
    for (int i = 0; i < table->ColumnsCount; i++)
    {
        ImGuiTableColumn* column = &table->Columns[i];
        if (column->WidthRequest < 0.0f)
        {
            if (column->InitWidthOrWeight > 0.0f)
            {
                if (column->Flags & ImGuiTableColumnFlags_WidthStretch)
                    width_auto_sum += column->InitWidthOrWeight;
                else
                    width_given_sum += column->InitWidthOrWeight;
            }
            width_auto_count++;
        }
        else
            width_given_sum += column->WidthRequest;
    }

    // Calculate the width of the columns
    float width_total = table->InnerWidth > 0.0f ? table->InnerWidth : table->OuterRect.GetWidth();
    float width_remaining = width_total - width_given_sum;
    if (width_remaining > 0.0f && width_auto_count > 0)
    {
        float width_auto = width_remaining / width_auto_count;
        for (int i = 0; i < table->ColumnsCount; i++)
        {
            ImGuiTableColumn* column = &table->Columns[i];
            if (column->WidthRequest < 0.0f)
            {
                if (column->InitWidthOrWeight > 0.0f && (column->Flags & ImGuiTableColumnFlags_WidthStretch))
                    column->WidthRequest = width_auto * column->InitWidthOrWeight / width_auto_sum;
                else
                    column->WidthRequest = width_auto;
            }
        }
    }
    else
    {
        for (int i = 0; i < table->ColumnsCount; i++)
        {
            ImGuiTableColumn* column = &table->Columns[i];
            if (column->WidthRequest < 0.0f)
                column->WidthRequest = column->InitWidthOrWeight > 0.0f ? column->InitWidthOrWeight : 1.0f;
        }
    }

    // Calculate the position of the columns
    float x = table->OuterRect.Min.x;
    for (int i = 0; i < table->ColumnsCount; i++)
    {
        ImGuiTableColumn* column = TableGetColumn(table, i);
        column->MinX = x;
        column->MaxX = x + column->WidthRequest;
        x = column->MaxX;
    }
}

#endif // #ifndef IMGUI_DISABLE
