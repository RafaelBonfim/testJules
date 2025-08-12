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

// Use your programming IDE's "Go to definition" function on the names of the structures listed above.
// The structures are defined in the lower section of this file.

//-----------------------------------------------------------------------------
// [SECTION] Context pointer
// See implementation of this file for comments about using this pointer
//-----------------------------------------------------------------------------

#ifndef GImGui
extern IMGUI_API ImGuiContext* GImGui;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Internal API
// No guarantee of forward compatibility here!
//-----------------------------------------------------------------------------

namespace ImGui
{
    // Windows
    IMGUI_API ImGuiWindow*  FindWindowByID(ImGuiID id);
    IMGUI_API ImGuiWindow*  FindWindowByName(const char* name);
    IMGUI_API void          UpdateWindowParentAndRootLinks(ImGuiWindow* window, ImGuiWindowFlags flags, ImGuiWindow* parent_window);
    IMGUI_API ImVec2        CalcWindowNextAutoFitSize(ImGuiWindow* window);
    IMGUI_API bool          IsWindowChildOf(ImGuiWindow* window, ImGuiWindow* potential_parent, bool popup_hierarchy, bool dock_hierarchy);
    IMGUI_API bool          IsWindowWithinBeginStackOf(ImGuiWindow* window, ImGuiWindow* potential_parent);
    IMGUI_API bool          IsWindowAbove(ImGuiWindow* potential_above, ImGuiWindow* potential_below);
    IMGUI_API bool          IsWindowNavFocusable(ImGuiWindow* window);
    IMGUI_API void          SetWindowPos(ImGuiWindow* window, const ImVec2& pos, ImGuiCond cond = 0);
    IMGUI_API void          SetWindowSize(ImGuiWindow* window, const ImVec2& size, ImGuiCond cond = 0);
    IMGUI_API void          SetWindowCollapsed(ImGuiWindow* window, bool collapsed, ImGuiCond cond = 0);
    IMGUI_API void          SetWindowHitTestHole(ImGuiWindow* window, const ImVec2& pos, const ImVec2& size);
    IMGUI_API void          SetWindowHiddenAndSkipItems(ImGuiWindow* window, bool hidden);
    IMGUI_API void          SetWindowParentWindowForFocus(ImGuiWindow* window, ImGuiWindow* parent_window);
    inline void             SetWindowConditionAllowFlags(ImGuiWindow* window, ImGuiCond flags, bool enabled) { if (enabled) window->SetWindowConditionAllowFlags |= flags; else window->SetWindowConditionAllowFlags &= ~flags; }

    // Windows: Display Order and Focus
    IMGUI_API void          FocusWindow(ImGuiWindow* window, ImGuiFocusRequestFlags flags = 0);
    IMGUI_API void          FocusTopMostWindowUnderOne(ImGuiWindow* under_this_window, ImGuiWindow* ignore_window, ImGuiRoot* root, ImGuiFocusRequestFlags flags);
    IMGUI_API void          BringWindowToFocusFront(ImGuiWindow* window);
    IMGUI_API void          BringWindowToDisplayFront(ImGuiWindow* window);
    IMGUI_API void          BringWindowToDisplayBack(ImGuiWindow* window);
    IMGUI_API void          BringWindowToDisplayBehind(ImGuiWindow* window, ImGuiWindow* behind_window);
    IMGUI_API int           FindWindowDisplayIndex(ImGuiWindow* window);
    IMGUI_API ImGuiWindow*  FindBottomMostVisibleWindowWithinBeginStack(ImGuiWindow* window);

    // Fonts, drawing
    IMGUI_API void          SetCurrentFont(ImFont* font);
    inline ImFont*          GetDefaultFont() { ImGuiContext& g = *GImGui; return g.IO.FontDefault ? g.IO.FontDefault : g.Fonts->Fonts[0]; }
    inline ImDrawList*      GetForegroundDrawList(ImGuiWindow* window) { IMGUI_UNUSED(window); return GetForegroundDrawList(); } // This seemingly unnecessary wrapper is to save code size instead of mighty inlining
    IMGUI_API ImDrawList*   GetForegroundDrawList();

    // Init
    IMGUI_API void          Initialize(ImGuiContext* context);
    IMGUI_API void          Shutdown(ImGuiContext* context);    // This is called by DestroyContext()

    // NewFrame
    IMGUI_API void          UpdateInputEvents(bool trickle_fast_inputs);
    IMGUI_API void          UpdateHoveredWindowAndCaptureFlags();
    IMGUI_API void          StartMouseMovingWindow(ImGuiWindow* window);
    IMGUI_API void          UpdateMouseMovingWindowNewFrame();
    IMGUI_API void          UpdateMouseMovingWindowEndFrame();

    // Generic context hooks
    IMGUI_API ImGuiID       AddContextHook(ImGuiContext* context, const ImGuiContextHook* hook);
    IMGUI_API void          RemoveContextHook(ImGuiContext* context, ImGuiID hook_id);
    IMGUI_API void          CallContextHooks(ImGuiContext* context, ImGuiContextHookType type);

    // Viewports
    IMGUI_API void          SetCurrentViewport(ImGuiWindow* window, ImGuiViewport* viewport);

    // Settings
    IMGUI_API void                  MarkIniSettingsDirty();
    IMGUI_API void                  MarkIniSettingsDirty(ImGuiWindow* window);
    IMGUI_API void                  ClearIniSettings();
    IMGUI_API ImGuiWindowSettings*  CreateNewWindowSettings(const char* name);
    IMGUI_API ImGuiWindowSettings*  FindWindowSettingsByID(ImGuiID id);
    IMGUI_API ImGuiWindowSettings*  FindWindowSettingsByWindow(ImGuiWindow* window);
    IMGUI_API void                  RemoveWindowSettings(ImGuiID id);
    IMGUI_API ImGuiSettingsHandler* FindSettingsHandler(const char* type_name);

    // Basic Accessors
    inline ImGuiID          GetCurrentWindowID()            { ImGuiContext& g = *GImGui; return g.CurrentWindow ? g.CurrentWindow->ID : 0; }
    inline ImGuiID          GetActiveID()                   { ImGuiContext& g = *GImGui; return g.ActiveId; }
    inline ImGuiID          GetFocusID()                    { ImGuiContext& g = *GImGui; return g.NavId; }
    inline ImGuiID          GetFocusScopeID()               { ImGuiContext& g = *GImGui; return g.NavFocusScopeId; }
    inline ImGuiItemStatusFlags GetItemStatusFlags()        { ImGuiContext& g = *GImGui; return g.LastItemData.StatusFlags; }
    inline ImGuiItemFlags   GetItemFlags()                  { ImGuiContext& g = *GImGui; return g.LastItemData.InFlags; }
    inline ImGuiID          GetActiveIDPreviousFrame()      { ImGuiContext& g = *GImGui; return g.ActiveIdPreviousFrame; }
    inline bool             IsActiveIdUsingNavDir(ImGuiDir dir) { ImGuiContext& g = *GImGui; return (g.ActiveIdUsingNavDirMask & (1 << dir)) != 0; }
    inline bool             IsActiveIdUsingAllKeyboardKeys() { ImGuiContext& g = *GImGui; return g.ActiveIdUsingKeyInputMask == (ImU32)ImGuiKey_NamedKey_COUNT; }
    IMGUI_API void          SetActiveID(ImGuiID id, ImGuiWindow* window);
    IMGUI_API void          SetFocusID(ImGuiID id, ImGuiWindow* window);
    IMGUI_API void          ClearActiveID();
    IMGUI_API ImGuiID       GetHoveredID();
    IMGUI_API void          SetHoveredID(ImGuiID id);
    IMGUI_API void          KeepAliveID(ImGuiID id);
    IMGUI_API void          MarkItemEdited(ImGuiID id);
    IMGUI_API void          PushOverrideID(ImGuiID id);
    IMGUI_API ImGuiID       GetIDWithSeed(const char* str, const char* str_end, ImGuiID seed);
    IMGUI_API ImGuiID       GetIDWithSeed(int n, ImGuiID seed);

    // Basic Helpers for widget code
    IMGUI_API void          ItemSize(const ImVec2& size, float text_baseline_y = -1.0f);
    IMGUI_API void          ItemSize(const ImRect& bb, float text_baseline_y = -1.0f);
    IMGUI_API bool          ItemAdd(const ImRect& bb, ImGuiID id, const ImRect* nav_bb = NULL, ImGuiItemFlags extra_flags = 0);
    IMGUI_API bool          ItemHoverable(const ImRect& bb, ImGuiID id, ImGuiItemFlags item_flags);
    IMGUI_API bool          IsClippedEx(const ImRect& bb, ImGuiID id);
    IMGUI_API void          SetLastItemData(ImGuiID item_id, ImGuiItemFlags in_flags, ImGuiItemStatusFlags status_flags, const ImRect& item_rect);
    IMGUI_API ImVec2        CalcItemSize(ImVec2 size, float default_w, float default_h);
    IMGUI_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
    IMGUI_API void          PushMultiItemsWidths(int components, float w_full);
    IMGUI_API bool          IsItemToggledSelection();
    IMGUI_API ImVec2        GetContentRegionMaxAbs();
    IMGUI_API void          ShrinkWidths(ImGuiShrinkWidthItem* items, int count, float width_excess);

    // Parameter stacks
    IMGUI_API void          PushItemFlag(ImGuiItemFlags option, bool enabled);
    IMGUI_API void          PopItemFlag();
    IMGUI_API const ImGuiDataVarInfo* GetStyleVarInfo(ImGuiStyleVar idx);

    // Logging/Capture
    IMGUI_API void          LogBegin(ImGuiLogType type, int auto_open_depth);
    IMGUI_API void          LogToBuffer(int auto_open_depth = -1);
    IMGUI_API void          LogRenderedText(const ImVec2* ref_pos, const char* text, const char* text_end = NULL);
    IMGUI_API void          LogSetNextTextDecoration(const char* prefix, const char* suffix);

    // Popups, Modals, Tooltips
    IMGUI_API bool          BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    IMGUI_API void          OpenPopupEx(ImGuiID id, ImGuiPopupFlags popup_flags = ImGuiPopupFlags_None);
    IMGUI_API void          ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup);
    IMGUI_API void          ClosePopupsOverWindow(ImGuiWindow* ref_window, bool restore_focus_to_window_under_popup);
    IMGUI_API void          ClosePopupsExceptModals();
    IMGUI_API bool          IsPopupOpen(ImGuiID id, ImGuiPopupFlags popup_flags);
    IMGUI_API bool          BeginPopupEx(ImGuiID id, ImGuiWindowFlags extra_flags);
    IMGUI_API void          BeginTooltipEx(ImGuiTooltipFlags tooltip_flags, ImGuiWindowFlags extra_window_flags);
    IMGUI_API ImGuiWindow*  GetTopMostPopupModal();
    IMGUI_API ImVec2        FindBestWindowPosForPopup(ImGuiWindow* window);
    IMGUI_API ImVec2        FindBestWindowPosForPopup(const ImRect& ref_rect);

    // Menus
    IMGUI_API bool          BeginViewportSideBar(const char* name, ImGuiViewport* viewport, ImGuiDir dir, float size, ImGuiWindowFlags window_flags);
    IMGUI_API bool          BeginMenuEx(const char* label, const char* icon, bool enabled);
    IMGUI_API bool          MenuItemEx(const char* label, const char* icon, const char* shortcut = NULL, bool selected = false, bool enabled = true);

    // Columns (legacy)
    IMGUI_API void          BeginColumns(const char* str_id, int count, ImGuiOldColumnFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    IMGUI_API void          EndColumns();                                                             // close columns
    IMGUI_API void          PushColumnClipRect(int column_index);
    IMGUI_API void          PushColumnsBackground();
    IMGUI_API void          PopColumnsBackground();
    IMGUI_API ImGuiID       GetColumnsID(const char* str_id, int count);
    IMGUI_API ImGuiOldColumns* FindOrCreateColumns(ImGuiWindow* window, ImGuiID id);
    IMGUI_API float         GetColumnOffsetFromNorm(const ImGuiOldColumns* columns, float norm_x);
    IMGUI_API float         GetColumnNormFromOffset(const ImGuiOldColumns* columns, float offset);

    // Tables
    IMGUI_API bool          BeginTableEx(const char* name, ImGuiID id, int columns_count, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0, 0), float inner_width = 0.0f);
    IMGUI_API void          BeginTableNextRow(ImGuiTable* table);
    IMGUI_API bool          BeginTableNextColumn(ImGuiTable* table);
    IMGUI_API void          EndTable(ImGuiTable* table);
    IMGUI_API void          TableSetupColumn(ImGuiTable* table, const char* label, ImGuiTableColumnFlags flags, float init_width_or_weight, ImGuiID user_id);
    IMGUI_API void          TableSetupScrollFreeze(ImGuiTable* table, int cols, int rows);
    IMGUI_API void          TableHeadersRow(ImGuiTable* table);
    IMGUI_API void          TableHeader(ImGuiTable* table, const char* label);
    IMGUI_API void          TableSetColumnWidth(ImGuiTable* table, int column_n, float width);
    IMGUI_API void          TableSetColumnSortDirection(ImGuiTable* table, int column_n, ImGuiSortDirection sort_direction, bool append_to_sort_specs);
    IMGUI_API int           TableGetHoveredColumn(ImGuiTable* table);
    IMGUI_API float         TableGetHeaderRowHeight(ImGuiTable* table);
    IMGUI_API void          TablePushBackgroundChannel(ImGuiTable* table);
    IMGUI_API void          TablePopBackgroundChannel(ImGuiTable* table);
    IMGUI_API ImGuiTable*   GetCurrentTable();
    IMGUI_API ImGuiTable*   TableFindByID(ImGuiID id);
    IMGUI_API void          TableAngledHeadersRow(ImGuiTable* table);
    IMGUI_API void          TableBeginApplyRequests(ImGuiTable* table);

    // Tables: Internals
    inline      ImGuiTableInstanceData* GetTableInstanceData(ImGuiTable* table, int instance_no) { if (instance_no == 0) return &table->InstanceData; return table->InnerTable->InstanceData; }
    IMGUI_API   void                    TableGetInstanceData(ImGuiTable* table, int instance_no);
    IMGUI_API   void                    TableSortSpecsSanitize(ImGuiTable* table);
    IMGUI_API   void                    TableSortSpecsBuild(ImGuiTable* table);
    IMGUI_API   ImGuiSortDirection      TableGetColumnNextSortDirection(ImGuiTableColumn* column);
    IMGUI_API   void                    TableFixColumnWidths(ImGuiTable* table);
    IMGUI_API   void                    TableUpdateBorders(ImGuiTable* table);
    IMGUI_API   void                    TableUpdateColumnsWeightFromWidth(ImGuiTable* table);
    IMGUI_API   void                    TableSetColumnWidthAutoSingle(ImGuiTable* table, ImGuiTableColumn* column);
    IMGUI_API   void                    TableSetColumnWidthAutoAll(ImGuiTable* table);
    IMGUI_API   void                    TableUpdateLayout(ImGuiTable* table);
    IMGUI_API   bool                    TableBeginContextMenuPopup(ImGuiTable* table);

    // Tab Bars
    IMGUI_API bool          BeginTabBarEx(ImGuiTabBar* tab_bar, const ImRect& bb, ImGuiTabBarFlags flags);
    IMGUI_API ImGuiTabItem* TabBarFindTabByID(ImGuiTabBar* tab_bar, ImGuiID tab_id);
    IMGUI_API ImGuiTabItem* TabBarFindTabByOrder(ImGuiTabBar* tab_bar, int order);
    IMGUI_API ImGuiTabItem* TabBarGetCurrentTab(ImGuiTabBar* tab_bar);
    inline bool             TabBarWantMoveCheck(ImGuiTabBar* tab_bar) { return (tab_bar->Flags & ImGuiTabBarFlags_Reorderable) && !(tab_bar->Flags & ImGuiTabBarFlags_NoTabListScrollingButtons); }
    IMGUI_API ImGuiTabItem* TabBarInsertTab(ImGuiTabBar* tab_bar, ImGuiTabItemFlags flags, ImGuiWindow* window);
    IMGUI_API void          TabBarRemoveTab(ImGuiTabBar* tab_bar, ImGuiID tab_id);
    IMGUI_API void          TabBarCloseTab(ImGuiTabBar* tab_bar, ImGuiTabItem* tab);
    IMGUI_API void          TabBarQueueReorder(ImGuiTabBar* tab_bar, const ImGuiTabItem* tab, int offset);
    IMGUI_API void          TabBarQueueReorderFromMousePos(ImGuiTabBar* tab_bar, const ImGuiTabItem* tab, ImVec2 mouse_pos);
    IMGUI_API bool          TabBarProcessReorder(ImGuiTabBar* tab_bar);
    IMGUI_API bool          TabItemEx(ImGuiTabBar* tab_bar, const char* label, bool* p_open, ImGuiTabItemFlags flags, ImGuiWindow* docked_window);
    IMGUI_API ImVec2        TabItemCalcSize(const char* label, bool has_close_button);
    IMGUI_API void          TabItemBackground(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImU32 col);
    IMGUI_API void          TabItemLabelAndCloseButton(ImDrawList* draw_list, const ImRect& bb, ImGuiTabItemFlags flags, ImVec2 frame_padding, const char* label, ImGuiID tab_id, ImGuiID close_button_id, bool is_contents_visible, bool* out_just_closed, bool* out_text_clipped);

    // Docking
    IMGUI_API void          DockBuilderDockWindow(const char* window_name, ImGuiID node_id);
    IMGUI_API ImGuiDockNode*DockBuilderGetNode(ImGuiID node_id);
    IMGUI_API ImGuiDockNode*DockBuilderGetCentralNode(ImGuiID node_id);
    IMGUI_API ImGuiID       DockBuilderAddNode(ImGuiID node_id = 0, ImGuiDockNodeFlags flags = 0);
    IMGUI_API void          DockBuilderRemoveNode(ImGuiID node_id);
    IMGUI_API void          DockBuilderSetNodePos(ImGuiID node_id, ImVec2 pos);
    IMGUI_API void          DockBuilderSetNodeSize(ImGuiID node_id, ImVec2 size);
    IMGUI_API ImGuiID       DockBuilderSplitNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_at_dir, ImGuiID* out_id_at_opposite_dir);
    IMGUI_API void          DockBuilderCopyDockSpace(ImGuiID src_dockspace_id, ImGuiID dst_dockspace_id, ImVector<const char*>* in_window_names);
    IMGUI_API void          DockBuilderCopyNode(ImGuiID src_node_id, ImGuiID dst_node_id, ImVector<ImGuiID>* out_node_remap_pairs);
    IMGUI_API void          DockBuilderCopyWindowSettings(const char* src_name, const char* dst_name);
    IMGUI_API void          DockBuilderFinish(ImGuiID node_id);

    // Docking - Internals
    IMGUI_API ImGuiDockContext* GetDockContext();
    IMGUI_API void          DockContextInitialize(ImGuiContext* ctx);
    IMGUI_API void          DockContextShutdown(ImGuiContext* ctx);
    IMGUI_API void          DockContextClearNodes(ImGuiContext* ctx, ImGuiID root_id, bool clear_settings_links);
    IMGUI_API void          DockContextRebuildNodes(ImGuiContext* ctx);
    IMGUI_API void          DockContextUpdateUndockFromNodes(ImGuiContext* ctx);
    IMGUI_API void          DockContextUpdateDocking(ImGuiContext* ctx);
    IMGUI_API void          DockContextQueueDock(ImGuiContext* ctx, ImGuiWindow* target, ImGuiDockNode* target_node, ImGuiWindow* payload, ImGuiDir split_dir, float split_ratio, bool split_outer);
    IMGUI_API void          DockContextQueueUndockWindow(ImGuiContext* ctx, ImGuiWindow* window);
    IMGUI_API void          DockContextQueueUndockNode(ImGuiContext* ctx, ImGuiDockNode* node);
    IMGUI_API bool          DockContextCalcDropPosForDocking(ImGuiWindow* target, ImGuiDockNode* target_node, ImGuiWindow* payload, ImGuiDir split_dir, bool split_outer, ImVec2* out_pos);
    IMGUI_API bool          DockNodeGetRootNode(ImGuiDockNode* node, ImGuiDockNode** p_root_node);
    IMGUI_API ImGuiDockNode*DockNodeFindByID(ImGuiDockContext* dc, ImGuiID id);
    IMGUI_API void          DockNodeUpdate(ImGuiDockNode* node);
    IMGUI_API void          DockNodeUpdateForRoot(ImGuiDockNode* node);
    IMGUI_API void          DockNodeUpdateVisibleFlag(ImGuiDockNode* node);
    IMGUI_API void          DockNodeStartMouseMovingWindow(ImGuiDockNode* node, ImGuiWindow* window);
    IMGUI_API void          DockNodeMoveWindow(ImGuiDockNode* node, ImGuiWindow* window, ImVec2 pos);
    IMGUI_API void          DockNodeApplyPosSizeToWindows(ImGuiDockNode* node);
    IMGUI_API void          DockNodeAddWindow(ImGuiDockNode* node, ImGuiWindow* window, bool add_to_tab_bar);
    IMGUI_API void          DockNodeRemoveWindow(ImGuiDockNode* node, ImGuiWindow* window, ImGuiID save_dock_id, bool save_order);
    IMGUI_API int           DockNodeGetTabOrder(ImGuiWindow* window);
    IMGUI_API ImGuiID       DockNodeGetWindowMenuButtonId(const ImGuiDockNode* node);
    IMGUI_API ImGuiDockNode*DockNodeGetParentNode(ImGuiDockNode* node);
    IMGUI_API bool          DockNodeIsDropAllowed(ImGuiWindow* host_window, ImGuiWindow* payload_window);
    IMGUI_API void          DockNodeSetTabBar(ImGuiDockNode* node, ImGuiTabBar* tab_bar);
    IMGUI_API void          DockNodeSetWindowMenuButton(ImGuiDockNode* node, ImGuiButtonFlags flags);
    IMGUI_API ImGuiDockNode*DockNodeFindInfo(ImGuiWindow* window, ImGuiDockNode** p_node);

    // Drag and Drop
    IMGUI_API bool          BeginDragDropTargetCustom(const ImRect& bb, ImGuiID id);
    IMGUI_API void          ClearDragDrop();
    IMGUI_API bool          IsDragDropPayloadBeingAccepted();

    // Internal Columns API (this is going to be exposed after we figure out a user-facing API)
    IMGUI_API void          SetWindowClipRectBeforeSetChannel(ImGuiWindow* window, const ImRect& clip_rect);
    IMGUI_API void          BeginColumnsChildMenu();

    // Nav
    IMGUI_API void          NavInitWindow(ImGuiWindow* window, bool force_reinit);
    IMGUI_API void          NavInitRequest(ImGuiWindow* window, ImGuiNavRequest request);
    IMGUI_API bool          NavMoveRequestButNoResultYet();
    IMGUI_API void          NavMoveRequestSubmit(ImGuiDir move_dir, ImGuiDir clip_dir, ImGuiNavMoveFlags move_flags, ImGuiScrollFlags scroll_flags);
    IMGUI_API void          NavMoveRequestForward(ImGuiDir move_dir, ImGuiDir clip_dir, ImGuiNavMoveFlags move_flags, ImGuiScrollFlags scroll_flags);
    IMGUI_API void          NavMoveRequestResolveWithLastItem(ImGuiNavItemData* result);
    IMGUI_API void          NavMoveRequestResolveWithPastRect(const ImRect& rect_rel, ImGuiNavItemData* result);
    IMGUI_API void          NavMoveRequestCancel();
    IMGUI_API void          NavMoveRequestApplyResult();
    IMGUI_API void          NavMoveRequestTryWrapping(ImGuiWindow* window, ImGuiNavMoveFlags move_flags);
    IMGUI_API float         GetNavInputAmount(ImGuiNavInput n, ImGuiInputFlags mode);
    IMGUI_API ImVec2        GetNavInputAmount2d(ImGuiNavDirSourceFlags dir_sources, ImGuiInputFlags mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    IMGUI_API int           CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate);
    IMGUI_API void          ActivateItem(ImGuiID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    IMGUI_API void          SetNavID(ImGuiID id, int nav_layer, ImGuiID focus_scope_id, const ImRect& rect_rel);
    IMGUI_API void          SetNavFocusScope(ImGuiID focus_scope_id);
    IMGUI_API void          PushFocusScope(ImGuiID id);
    IMGUI_API void          PopFocusScope();
    inline ImGuiID          GetFocusScope() { ImGuiContext& g = *GImGui; return g.NavFocusScopeId; } // Focus scope is generally group ID.
    inline ImGuiID          GetNavFocusScope() { ImGuiContext& g = *GImGui; return g.NavFocusScopeId; }
    IMGUI_API ImGuiWindow*  NavRestoreLastChildNavWindow(ImGuiWindow* window);
    IMGUI_API void          NavRestoreLayer(ImGuiNavLayer layer);
    IMGUI_API void          NavRestoreHighlight();
    IMGUI_API bool          NavHasScroll();
    IMGUI_API void          NavUpdateCurrentWindowIsScrollPushableX();

    // Render
    IMGUI_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    IMGUI_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
    IMGUI_API void          RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    IMGUI_API void          RenderTextClippedEx(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    IMGUI_API void          RenderTextEllipsis(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, float clip_max_x, float ellipsis_max_x, const char* text, const char* text_end, const ImVec2* text_size_if_known);
    IMGUI_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    IMGUI_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
    IMGUI_API void          RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, ImDrawFlags flags = 0);
    IMGUI_API void          RenderNavHighlight(const ImRect& bb, ImGuiID id, ImGuiNavHighlightFlags flags = ImGuiNavHighlightFlags_TypeDefault);
    IMGUI_API const char*   FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.

    // Render helpers
    IMGUI_API void          RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale = 1.0f);
    IMGUI_API void          RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col);
    IMGUI_API void          RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz);
    IMGUI_API void          RenderMouseCursor(ImDrawList* draw_list, ImVec2 pos, float scale, ImGuiMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow);
    IMGUI_API void          RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, ImGuiDir direction, ImU32 col);
    IMGUI_API void          RenderArrowDockMenu(ImDrawList* draw_list, ImVec2 p_min, float sz, ImU32 col);
    IMGUI_API void          RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    IMGUI_API void          RenderRectFilledWithHole(ImDrawList* draw_list, const ImRect& outer, const ImRect& inner, ImU32 col, float rounding);

    // Widgets
    IMGUI_API void          TextEx(const char* text, const char* text_end = NULL, ImGuiTextFlags flags = 0);
    IMGUI_API bool          ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
    IMGUI_API bool          CloseButton(ImGuiID id, const ImVec2& pos);
    IMGUI_API bool          CollapseButton(ImGuiID id, const ImVec2& pos, ImGuiDockNode* dock_node);
    IMGUI_API bool          ArrowButtonEx(const char* str_id, ImGuiDir dir, ImVec2 size_arg, ImGuiButtonFlags flags = 0);
    IMGUI_API void          Scrollbar(ImGuiAxis axis);
    IMGUI_API bool          ScrollbarEx(const ImRect& bb, ImGuiID id, ImGuiAxis axis, ImS64* p_scroll_v, ImS64 avail_v, ImS64 contents_v, ImDrawFlags flags);
    IMGUI_API bool          ImageButtonEx(ImGuiID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
    IMGUI_API ImRect        GetWindowScrollbarRect(ImGuiWindow* window, ImGuiAxis axis);
    IMGUI_API ImGuiID       GetWindowScrollbarID(ImGuiWindow* window, ImGuiAxis axis);
    IMGUI_API ImGuiID       GetWindowResizeCornerID(ImGuiWindow* window, int n); // 0..3: corners
    IMGUI_API ImGuiID       GetWindowResizeBorderID(ImGuiWindow* window, ImGuiDir dir);

    // Widgets: InputText
    IMGUI_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool          TempInputText(const ImRect& bb, ImGuiID id, const char* label, char* buf, int buf_size, ImGuiInputTextFlags flags);
    IMGUI_API bool          TempInputScalar(const ImRect& bb, ImGuiID id, const char* label, ImGuiDataType data_type, void* p_data, const char* format, const void* p_clamp_min = NULL, const void* p_clamp_max = NULL);
    inline bool             TempInputIsActive(ImGuiID id)       { ImGuiContext& g = *GImGui; return (g.ActiveId == id && g.TempInputId == id); }
    inline ImGuiInputTextState* GetInputTextState(ImGuiID id)   { ImGuiContext& g = *GImGui; return (g.InputTextState.ID == id) ? &g.InputTextState : NULL; }

    // Widgets: ColorEdit
    IMGUI_API void          ColorTooltip(const char* text, const float* col, ImGuiColorEditFlags flags);
    IMGUI_API void          ColorEditOptionsPopup(const float* col, ImGuiColorEditFlags flags);
    IMGUI_API void          ColorPickerOptionsPopup(const float* ref_col, ImGuiColorEditFlags flags);

    // Widgets: Plot
    IMGUI_API void          PlotEx(ImGuiPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size);

    // Widgets: Sliders
    IMGUI_API bool          SliderBehavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb);
    IMGUI_API bool          SliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags);

    // Widgets: DragScalar, DragFloat, DragInt, etc.
    IMGUI_API bool          DragBehavior(ImGuiID id, ImGuiDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);
    IMGUI_API bool          DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags);

    // Widgets: Tree
    IMGUI_API bool          TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    IMGUI_API bool          TreeNodeBehaviorIsOpen(ImGuiID id, ImGuiTreeNodeFlags flags = 0);
    IMGUI_API void          TreePushOverrideID(ImGuiID id);

    // Text Filter
    IMGUI_API bool          PassFilter(const ImGuiTextFilter* filter, const char* text, const char* text_end = NULL);

    // Selections
    IMGUI_API void          SetNextItemSelectionUserData(ImGuiSelectionUserData selection_user_data); // Called before AddItem()

    // Data type helpers
    IMGUI_API const ImGuiDataTypeInfo*  DataTypeGetInfo(ImGuiDataType data_type);
    IMGUI_API int           DataTypeFormatString(char* buf, int buf_size, ImGuiDataType data_type, const void* p_data, const char* format);
    IMGUI_API void          DataTypeApplyOp(ImGuiDataType data_type, int op, void* output, const void* arg_1, const void* arg_2);
    IMGUI_API bool          DataTypeApplyFromText(const char* buf, ImGuiDataType data_type, void* p_data, const char* format);
    IMGUI_API int           DataTypeCompare(ImGuiDataType data_type, const void* arg_1, const void* arg_2);
    IMGUI_API bool          DataTypeClamp(ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max);

    // Input-level querying
    IMGUI_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);
    IMGUI_API bool          IsMouseDragPastThreshold(ImGuiMouseButton button, float lock_threshold = -1.0f);
    inline bool             IsKeyPressed(ImGuiKey key, bool repeat = true) { return IsKeyPressed(key, GetKeyOwner(key), repeat); }
    IMGUI_API bool          IsKeyPressed(ImGuiKey key, ImGuiID owner_id, bool repeat = true);
    IMGUI_API bool          IsKeyReleased(ImGuiKey key);
    IMGUI_API bool          IsKeyReleased(ImGuiKey key, ImGuiID owner_id);
    IMGUI_API bool          IsKeyDown(ImGuiKey key);
    IMGUI_API bool          IsKeyDown(ImGuiKey key, ImGuiID owner_id);
    IMGUI_API bool          IsMouseDown(ImGuiMouseButton button);
    IMGUI_API bool          IsMouseDown(ImGuiMouseButton button, ImGuiID owner_id);
    IMGUI_API bool          IsMouseClicked(ImGuiMouseButton button, bool repeat = false);
    IMGUI_API bool          IsMouseClicked(ImGuiMouseButton button, ImGuiID owner_id, bool repeat = false);
    IMGUI_API bool          IsMouseReleased(ImGuiMouseButton button);
    IMGUI_API bool          IsMouseDoubleClicked(ImGuiMouseButton button);
    IMGUI_API bool          IsMouseDoubleClicked(ImGuiMouseButton button, ImGuiID owner_id);
    IMGUI_API bool          IsMouseDragPastThreshold(ImGuiMouseButton button, float lock_threshold);
    IMGUI_API bool          IsMouseDragging(ImGuiMouseButton button, float lock_threshold = -1.0f);
    IMGUI_API ImVec2        GetMousePos();
    IMGUI_API ImVec2        GetMousePosDragDelta(ImGuiMouseButton button = 0, float lock_threshold = -1.0f);
    IMGUI_API void          ResetMouseDragDelta(ImGuiMouseButton button = 0);
    IMGUI_API ImGuiMouseCursor GetMouseCursor();
    IMGUI_API void          SetMouseCursor(ImGuiMouseCursor cursor_type);
    IMGUI_API void          SetNextFrameWantCaptureKeyboard(bool want_capture_keyboard);
    IMGUI_API void          SetNextFrameWantCaptureMouse(bool want_capture_mouse);
    IMGUI_API bool          IsAppFocused();

    // Clipboard Utilities
    IMGUI_API const char*   GetClipboardText();
    IMGUI_API void          SetClipboardText(const char* text);
    IMGUI_API void          LoadIniSettingsFromDisk(const char* ini_filename);
    IMGUI_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size = 0);
    IMGUI_API void          SaveIniSettingsToDisk(const char* ini_filename);
    IMGUI_API const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);

    // Debug Utilities
    IMGUI_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_vert, size_t sz_idx);
    IMGUI_API void          DebugFlashStyleColor(ImGuiCol idx);
    IMGUI_API void          DebugBreakClearData();
    IMGUI_API void          DebugBreakButton(const char* label, const char* description);
    IMGUI_API void          DebugBreakButtonTooltip(bool active, const char* description);
    IMGUI_API void          DebugNodeColumns(ImGuiOldColumns* columns);
    IMGUI_API void          DebugNodeDockNode(ImGuiDockNode* node, const char* label);
    IMGUI_API void          DebugNodeDrawList(ImGuiWindow* window, const ImDrawList* draw_list, const char* label);
    IMGUI_API void          DebugNodeStorage(ImGuiStorage* storage, const char* label);
    IMGUI_API void          DebugNodeTabBar(ImGuiTabBar* tab_bar, const char* label);
    IMGUI_API void          DebugNodeTable(ImGuiTable* table);
    IMGUI_API void          DebugNodeTableSettings(ImGuiTableSettings* settings);
    IMGUI_API void          DebugNodeWindow(ImGuiWindow* window, const char* label);
    IMGUI_API void          DebugNodeWindowSettings(ImGuiWindowSettings* settings);
    IMGUI_API void          DebugNodeWindowsList(ImVector<ImGuiWindow*>* windows, const char* label);
    IMGUI_API void          DebugNodeViewport(ImGuiViewport* viewport);
    IMGUI_API void          DebugRenderKeyboardPreview(ImDrawList* draw_list);
    IMGUI_API void          DebugRenderViewportRects(ImDrawList* draw_list);

    // Misc
    IMGUI_API ImVec2        CalcTextSizeA(float size, float max_width, float wrap_width, const char* text, const char* text_end = NULL, const char** remaining = NULL); // Not recommended to use directly
    IMGUI_API void          GetKeyMagnitude2d(ImVec2* out, ImGuiKey key_left, ImGuiKey key_right, ImGuiKey key_up, ImGuiKey key_down);
    IMGUI_API float         GetKeyMagnitude1d(ImGuiKey key_neg, ImGuiKey key_pos);
    IMGUI_API void          CalcTypematicRepeat(float t, float repeat_delay, float repeat_rate, int* out_count);
    IMGUI_API int           ParseFormatFindStart(const char* format);
    IMGUI_API int           ParseFormatFindEnd(const char* format);
    IMGUI_API const char*   ParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
    IMGUI_API int           ParseFormatPrecision(const char* format, int default_value);
    IMGUI_API bool          BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    IMGUI_API const char*   FindLatestDecoratedText(const char* text, const char* text_end);

    // Obsolete functions
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Obsoleted in v1.89, Dec 2022
    // 'GetContentRegionAvail()' is a better name for 'GetContentRegionAvail()'. This is EXACTLY the same function, just renamed.
    // Kept for backward compatibility, please replace with 'GetContentRegionAvail()'.
    static inline ImVec2    GetContentRegionAvail()                                 { return GetContentRegionAvail(); }
    // Obsoleted in v1.88, Aug 2022
    // This is EXACTLY the same function as 'GetStyleColorVec4()'. Kept for backward compatibility.
    static inline const ImVec4* GetStyleColorVec4(ImGuiCol idx)                     { return GetStyleColorVec4(idx); }
    // Obsoleted in v1.86, Oct 2021
    // Please use 'SetNextItemAllowOverlap()' instead. It is the same function, just a better name.
    static inline void      SetNextItemAllowOverlap()                               { SetNextItemAllowOverlap(); }
    // Obsoleted in v1.82, Apr 2021
    // This is EXACTLY the same function as IsKeyPressed(key, false). It was misleading because it had a name that suggested it would handle repeats, which it never did.
    static inline bool      IsKeyPressed(ImGuiKey key, bool repeat)                 { IM_ASSERT(repeat == false); return IsKeyPressed(key, false); }
    // Obsoleted in v1.82, Apr 2021
    // Was a misleading name for 'IsKeyDown(key)'.
    static inline bool      IsKeyDown(ImGuiKey key)                                 { return IsKeyDown(key); }
    // Obsoleted in v1.82, Apr 2021
    // Was a misleading name for 'IsKeyReleased(key)'.
    static inline bool      IsKeyReleased(ImGuiKey key)                             { return IsKeyReleased(key); }
    // Obsoleted in v1.82, Apr 2021
    // Please use 'GetMouseDragDelta(ImGuiMouseButton_Left)'.
    static inline ImVec2    GetMouseDragDelta(ImGuiMouseButton button, float lock_threshold) { return GetMouseDragDelta(button, lock_threshold); }
    // Obsoleted in v1.82, Apr 2021
    // Please use 'ResetMouseDragDelta(ImGuiMouseButton_Left)'.
    static inline void      ResetMouseDragDelta(ImGuiMouseButton button)            { ResetMouseDragDelta(button); }
    // Obsoleted in v1.80, Jan 2021
    // Please use 'GetTopMostPopupModal()'.
    static inline ImGuiWindow* GetFrontMostPopupModal()                             { return GetTopMostPopupModal(); }
    // Obsoleted in v1.79, Oct 2020
    // Please use 'IsItemToggledSelection()'. It was only available for a short time, so hopefully not many people are using it.
    static inline bool      IsItemToggledOpen()                                     { return IsItemToggledSelection(); }
    // Obsoleted in v1.78, Aug 2020
    // Please use 'CalcTextSize(text, NULL, false, -1.0f)'.
    static inline ImVec2    CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width) { return CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width); }
    // Obsoleted in v1.78, Aug 2020
    // Please use 'RenderText(pos, text, NULL, false)'.
    static inline void      RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash) { RenderText(pos, text, text_end, hide_text_after_hash); }
    // Obsoleted in v1.78, Aug 2020
    // Please use 'RenderTextWrapped(pos, text, NULL, wrap_width)'.
    static inline void      RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width) { RenderTextWrapped(pos, text, text_end, wrap_width); }
    // Obsoleted in v1.78, Aug 2020
    // Please use 'RenderTextClipped(pos_min, pos_max, text, NULL, &text_size, align, clip_rect)'.
    static inline void      RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align, const ImRect* clip_rect) { RenderTextClipped(pos_min, pos_max, text, text_end, text_size_if_known, align, clip_rect); }
    // Obsoleted in v1.76, Jun 2020
    static inline bool      IsAnyMouseDown()                                        { ImGuiContext& g = *GImGui; return g.IO.MouseDown[0] || g.IO.MouseDown[1] || g.IO.MouseDown[2] || g.IO.MouseDown[3] || g.IO.MouseDown[4]; }
    // Obsoleted in v1.72, Mar 2020
    // Please use 'GetBackgroundDrawList()'.
    static inline ImDrawList* GetWindowDrawList()                                   { ImGuiContext& g = *GImGui; return g.CurrentWindow->DrawList; }
    // Obsoleted in v1.72, Mar 2020
    // Please use 'GetBackgroundDrawList(GetMainViewport())'.
    static inline ImDrawList* GetOverlayDrawList()                                  { return GetForegroundDrawList(GetMainViewport()); }
    // Obsoleted in v1.71, Feb 2020
    // Please use 'BeginChild("##Child", size, true, 0)'.
    static inline bool      BeginChild(const char* str_id, const ImVec2& size, bool border, ImGuiWindowFlags extra_flags) { return BeginChildEx(str_id, 0, size, border, extra_flags); }
    static inline bool      BeginChild(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags extra_flags) { return BeginChildEx(NULL, id, size, border, extra_flags); }
    // Obsoleted in v1.69, Jan 2019
    // Please use 'SetNextWindowBgAlpha()'.
    static inline void      SetWindowBgAlpha(float alpha)                           { ImGuiContext& g = *GImGui; g.NextWindowData.BgAlphaVal = alpha; g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasBgAlpha; }
    // Obsoleted in v1.67, Oct 2018
    // Please use 'ShowMetricsWindow()'.
    static inline void      ShowTestWindow(bool* p_open = NULL)                     { return ShowMetricsWindow(p_open); }
    // Obsoleted in v1.67, Oct 2018
    // Please use 'IsPopupOpen(NULL, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel)'.
    static inline bool      IsAnyPopupOpen()                                        { ImGuiContext& g = *GImGui; return g.OpenPopupStack.Size > 0; }
    // Obsoleted in v1.64, Aug 2018
    // Please use 'OpenPopupContextItem()'.
    static inline void      OpenPopupOnItemClick(const char* str_id, ImGuiPopupFlags popup_flags) { OpenPopupOnItemClick(str_id, popup_flags); }
    // Obsoleted in v1.63, Aug 2018
    // Please use 'GetContentRegionMax() - GetCursorPos()'.
    static inline ImVec2    GetContentRegionAvail()                                 { ImGuiContext& g = *GImGui; return g.CurrentWindow->ContentRegionRect.GetSize() - g.CurrentWindow->DC.CursorPos; }
    // Obsoleted in v1.63, Aug 2018
    // Please use 'GetContentRegionMax().x - GetCursorPos().x'.
    static inline float     GetContentRegionAvailWidth()                            { return GetContentRegionAvail().x; }
    // Obsoleted in v1.60, Jun 2018
    // Please use 'GetMainViewport()->Pos'.
    static inline ImVec2    GetWindowPos()                                          { ImGuiContext& g = *GImGui; return g.CurrentWindow->Pos; }
    // Obsoleted in v1.60, Jun 2018
    // Please use 'GetMainViewport()->Size'.
    static inline ImVec2    GetWindowSize()                                         { ImGuiContext& g = *GImGui; return g.CurrentWindow->Size; }
    // Obsoleted in v1.60, Jun 2018
    // Please use 'GetMainViewport()->Size.x'.
    static inline float     GetWindowWidth()                                        { ImGuiContext& g = *GImGui; return g.CurrentWindow->Size.x; }
    // Obsoleted in v1.60, Jun 2018
    // Please use 'GetMainViewport()->Size.y'.
    static inline float     GetWindowHeight()                                       { ImGuiContext& g = *GImGui; return g.CurrentWindow->Size.y; }
    // Obsoleted in v1.53, Dec 2017
    // Please use 'SetNextWindowPos(pos, ImGuiCond_FirstUseEver)'.
    static inline void      SetNextWindowPosCenter(ImGuiCond cond = 0)              { ImGuiContext& g = *GImGui; g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos; g.NextWindowData.Pos = g.Viewports[0]->GetCenter(); g.NextWindowData.PosCond = cond ? cond : ImGuiCond_FirstUseEver; }
    // Obsoleted in v1.52, Nov 2017
    // Please use 'GetStyle().ItemSpacing'.
    static inline ImVec2    GetItemRectSpacing()                                    { ImGuiContext& g = *GImGui; return g.Style.ItemSpacing; }
    // Obsoleted in v1.50, Sep 2017
    // Please use 'IsMousePosValid()'.
    static inline bool      IsMousePosValid(const ImVec2* mouse_pos)                { if (mouse_pos == NULL) mouse_pos = &GetIO().MousePos; return mouse_pos->x >= 0.0f && mouse_pos->y >= 0.0f; }
    // Obsoleted in v1.50, Sep 2017
    // Please use 'GetTime()'.
    static inline float     GetTime()                                               { return (float)GetTime(); }
    // Obsoleted in v1.50, Sep 2017
    // Please use 'GetFrameCount()'.
    static inline int       GetFrameCount()                                         { return GetFrameCount(); }
#endif

} // namespace ImGui


//-----------------------------------------------------------------------------
// [SECTION] Generic helpers
//-----------------------------------------------------------------------------

// Free functions
IMGUI_API ImU32         ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
IMGUI_API ImU32         ImHashStr(const char* data, size_t data_size = 0, ImU32 seed = 0);

// Helper: ImBitArray
// Store 1-bit per value.
// Note that Resize() currently clears the whole vector.
template<int BIT_COUNT>
struct ImBitArray
{
    ImU32           Storage[(BIT_COUNT + 31) >> 5];
    ImBitArray()                                { Clear(); }
    void            Clear()                     { memset(Storage, 0, sizeof(Storage)); }
    void            SetBit(int n)               { IM_ASSERT(n >= 0 && n < BIT_COUNT); int W = n >> 5; int B = n & 31; Storage[W] |= (ImU32)1 << B; }
    void            ClearBit(int n)             { IM_ASSERT(n >= 0 && n < BIT_COUNT); int W = n >> 5; int B = n & 31; Storage[W] &= ~((ImU32)1 << B); }
    bool            TestBit(int n) const        { IM_ASSERT(n >= 0 && n < BIT_COUNT); int W = n >> 5; int B = n & 31; return (Storage[W] & ((ImU32)1 << B)) != 0; }
    void            SetBitRange(int n, int n2)  // Works on range [n..n2-1]
    {
        n2--;
        IM_ASSERT(n >= 0 && n < BIT_COUNT && n2 >= n && n2 < BIT_COUNT);
        if (n > n2)
            return;
        int W1 = n >> 5;
        int W2 = n2 >> 5;
        ImU32 B1 = (ImU32)1 << (n & 31);
        ImU32 B2 = (ImU32)1 << (n2 & 31);
        if (W1 == W2)
        {
            ImU32 mask = (B2 | (B2 - 1)) & ~(B1 - 1);
            Storage[W1] |= mask;
        }
        else
        {
            Storage[W1] |= ~(B1 - 1);
            for (int w = W1 + 1; w < W2; w++)
                Storage[w] = (ImU32)-1;
            Storage[W2] |= (B2 | (B2 - 1));
        }
    }
};

// Helper: ImBitVector
// Store 1-bit per value.
struct IMGUI_API ImBitVector
{
    ImVector<ImU32> Storage;
    void            Create(int n)               { Storage.resize((n + 31) >> 5); memset(Storage.Data, 0, (size_t)Storage.Size * sizeof(Storage.Data[0])); }
    void            Clear()                     { Storage.clear(); }
    bool            TestBit(int n) const        { IM_ASSERT(n < (Storage.Size << 5)); int W = n >> 5; int B = n & 31; return (Storage[W] & ((ImU32)1 << B)) != 0; }
    void            SetBit(int n)               { IM_ASSERT(n < (Storage.Size << 5)); int W = n >> 5; int B = n & 31; Storage[W] |= (ImU32)1 << B; }
    void            ClearBit(int n)             { IM_ASSERT(n < (Storage.Size << 5)); int W = n >> 5; int B = n & 31; Storage[W] &= ~((ImU32)1 << B); }
};

//-----------------------------------------------------------------------------
// [SECTION] ImDrawList support
//-----------------------------------------------------------------------------

// ImDrawList: Helper function to calculate a circle's segment count given its radius and a max error value.
// Estimation of number of circle segments based on error is derived using chosen value of 'max_error' (default is 0.30f)
// Number of segments = f(radius) = 2*PI/acos(1-error/radius)
// For max_error = 0.30f, we have relation: ~100 segments for radius=100.0f, 50 for radius=25.0f.
#define IM_ROUNDUP_TO_P2(v)     (((v) + 1) & ~1)    // A single bit weirdness in the result would be noticeable, so we round up to next multiple of 2.
inline int ImDrawList_CalcCircleAutoSegmentCount(float radius, float max_error)
{
    // Check for edge cases to avoid division by zero
    if (radius <= 0.0f || max_error <= 0.0f)
        return 4;

    // The formula is derived from the relation between the radius, the max error and the angle between segments.
    // The angle is acos(1 - max_error / radius).
    // Number of segments is 2 * PI / angle.
    // We use a lookup table for small radius values to avoid the floating point calculations and to ensure a minimum number of segments.
    if (radius <= 2.0f)
        return 4;
    if (radius <= 4.0f)
        return 8;

    const float inv_error = 1.0f / max_error;
    int count = (int)IM_ROUNDUP_TO_P2((int)(IM_PI * 2.0f * sqrtf(radius * inv_error)));
    return count > 256 ? 256 : count;
}

// ImDrawList: You can use the ImDrawList::PushClipRectFullScreen() to create a fullscreen clipping rectangle.
// This is generally useful to avoid situations where you have to manually calculate the clipping rectangle.
// This function is a shortcut to PushClipRect(GetMainViewport()->Pos, GetMainViewport()->Pos + GetMainViewport()->Size, false).
// However, it is better to use the ImDrawList API directly for more control.
inline void ImDrawList_PushClipRectFullScreen(ImDrawList* draw_list)
{
    ImGuiContext& g = *GImGui;
    ImGuiViewport* viewport = g.Viewports[0];
    draw_list->PushClipRect(viewport->Pos, viewport->Pos + viewport->Size, false);
}

// Data shared between all ImDrawList instances
// You can create multiple ImDrawList instances with their own buffers. You can use them to draw channels and merge them later.
IMGUI_API void ImDrawList_ResetForNewFrame(ImDrawList* draw_list);
IMGUI_API void ImDrawList_ClearFreeMemory(ImDrawList* draw_list);
IMGUI_API void ImDrawList_PopOverlappingDrawCmd(ImDrawList* draw_list);

struct IMGUI_API ImDrawListSharedData
{
    ImVec2          TexUvWhitePixel;            // UV of a single white pixel in the atlas texture (for drawing primitives)
    ImFont*         Font;                       // Current font for text rendering
    float           FontSize;                   // Current font size
    float           CurveTessellationTol;       // Tessellation tolerance for bezier curves
    float           CircleSegmentMaxError;      // Maximum error for circle segments
    ImVec4          ClipRectFullscreen;         // Fullscreen clipping rectangle
    ImDrawListFlags InitialFlags;               // Initial flags for all draw lists

    // Performance intentionally not measured on initial build, maybe in the future.
    // We could perhaps do it once, when the first ImDrawList is created.
    ImVector<ImVec4> TempBuffer;                // Temporary buffer for some calculations

    ImDrawListSharedData();
    void SetCircleTessellationMaxError(float max_error);
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];          // Global layers for secondary viewports
    ImVector<ImDrawList*>   DrawLists;
    ImDrawList*             OwnerData;
    int                     TotalVtxCount;
    int                     TotalIdxCount;

    ImDrawDataBuilder()     { Clear(); }
    void Clear()            { Layers[0].clear(); Layers[1].clear(); DrawLists.clear(); OwnerData = NULL; TotalVtxCount = TotalIdxCount = 0; }
    void ClearFreeMemory()  { for (int i = 0; i < 2; i++) for (int j = 0; j < Layers[i].Size; j++) IM_DELETE(Layers[i][j]); Clear(); }
    int  GetDrawListCount() const { return DrawLists.Size; }
    IMGUI_API void FlattenIntoSingleLayer();
};

//-----------------------------------------------------------------------------
// [SECTION] Main structures
//-----------------------------------------------------------------------------

struct ImGuiDataVarInfo
{
    ImGuiDataType   Type;
    ImU32           Count;      // 1 for most types, 2 for ImVec2, 4 for ImVec4, etc.
    ImU32           Offset;     // Offset in bytes from base ptr to variable.
    void* GetVarPtr(void* parent) const { return (void*)((unsigned char*)parent + Offset); }
};

struct ImGuiDataTypeInfo
{
    size_t          Size;       // Size in bytes
    const char*     Name;       // Short name of the type (e.g. "int", "float")
    const char*     PrintFmt;   // Format string for printing (e.g. "%d", "%.3f")
    const char*     ScanFmt;    // Format string for scanning (e.g. "%d", "%f")
};

// A color modifier to be stored on the color stack.
struct ImGuiColorMod
{
    ImGuiCol        Col;
    ImVec4          BackupValue;
};

// A style modifier to be stored on the style stack.
struct ImGuiStyleMod
{
    ImGuiStyleVar   VarIdx;
    union           { int BackupInt[2]; float BackupFloat[2]; };
    ImGuiStyleMod(ImGuiStyleVar idx, int v)     { VarIdx = idx; BackupInt[0] = v; }
    ImGuiStyleMod(ImGuiStyleVar idx, float v)   { VarIdx = idx; BackupFloat[0] = v; }
    ImGuiStyleMod(ImGuiStyleVar idx, ImVec2 v)  { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Stacked storage data for BeginGroup()/EndGroup()
struct ImGuiGroupData
{
    ImGuiID     WindowID;
    ImVec2      BackupCursorPos;
    ImVec2      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    ImVec2      BackupCurrLineSize;
    float       BackupCurrLineTextBaseOffset;
    ImGuiID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        BackupHoveredIdIsAlive;
    bool        EmitItem;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and not very good.
struct IMGUI_API ImGuiMenuColumns
{
    ImU32       TotalWidth;
    ImU32       NextTotalWidth;
    ImU16       Spacing;
    ImU16       OffsetIcon;     // Always zero for now
    ImU16       OffsetLabel;    // Offsets are locked in Update()
    ImU16       OffsetShortcut;
    ImU16       OffsetMark;
    ImU16       Widths[4];      // Width of:   Icon, Label, Shortcut, Mark  (accumulators for current frame)

    ImGuiMenuColumns() { Clear(); }
    void        Clear() { memset(this, 0, sizeof(*this)); }
    void        Update(float spacing, bool window_reappearing);
    float       DeclColumns(float w_icon, float w_label, float w_shortcut, float w_mark);
    void        CalcNextTotalWidth(bool update_offsets);
};

// Internal state of the currently focused/edited text input box
// For a given item ID, all fields vary over time.
struct IMGUI_API ImGuiInputTextState
{
    ImGuiID             ID;                     // widget id owning the text state
    int                 CurLenW, CurLenA;       // we need to maintain both wide-char and utf8 lengths... characters count vs bytes count
    ImVector<ImWchar>   TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>      InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8)
    bool                TextAIsValid;           // true when InitialTextA is valid
    int                 BufCapacityA;           // end-user buffer capacity
    float               ScrollX;                // horizontal scrolling/offset
    ImGuiStb::STB_TexteditState Stb;            // state for stb_textedit.h
    float               CursorAnim;             // timer for cursor blink
    bool                CursorFollow;           // set when we want scrolling to follow the cursor during the next frame.
    bool                SelectedAllMouseLock;   // after a double-click selection, generally we don't want to select all again on the next frame.
    bool                Edited;                 // edited status (most widgets read it but don't write to it so it needs to be selectably reset)
    ImGuiInputTextFlags UserFlags;              // copy of user flags
    ImGuiInputTextCallback  UserCallback;       // copy of user callback
    void*               UserCallbackData;       // copy of user callback data

    ImGuiInputTextState()                   { memset(this, 0, sizeof(*this)); }
    void        ClearText()                 { CurLenW = CurLenA = 0; TextW[0] = 0; InitialTextA[0] = 0; }
    void        ClearFreeMemory()           { TextW.clear(); InitialTextA.clear(); }
    int         GetUndoAvailCount() const   { return Stb.undostate.undo_point; }
    int         GetRedoAvailCount() const   { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void        OnKeyPressed(int key);

    // Static analysis features are not enabled by default in Dear ImGui, but you can enable them via imconfig.h
    // This is a helper to static analysis tools.
#ifdef IMGUI_ENABLE_STATIC_ANALYSIS
    static void StaticAnalysisDummy(ImGuiInputTextState* state) { state->ID = 0; state->CurLenW = 0; state->CurLenA = 0; state->TextW.push_back(0); state->InitialTextA.push_back(0); state->TextAIsValid = false; state->BufCapacityA = 0; state->ScrollX = 0; state->Stb = {}; state->CursorAnim = 0.0f; state->CursorFollow = false; state->SelectedAllMouseLock = false; state->Edited = false; state->UserFlags = 0; state->UserCallback = NULL; state->UserCallbackData = NULL; }
#endif
};

// Data saved in imgui.ini file
struct ImGuiWindowSettings
{
    ImGuiID     ID;
    ImVec2ih    Pos;
    ImVec2ih    Size;
    ImVec2ih    ViewportPos;
    ImGuiID     ViewportId;
    ImGuiID     DockId;
    ImGuiID     ClassId;        // ImGuiWindowClass*
    short       DockOrder;      // Order of the docked window within its parent dock node.
    bool        Collapsed;
    bool        WantApply;      // Set when loaded from .ini data.

    ImGuiWindowSettings()       { ID = 0; Pos = ImVec2ih(0, 0); Size = ImVec2ih(0, 0); ViewportId = 0; DockId = 0; ClassId = 0; DockOrder = -1; Collapsed = false; WantApply = false; }
    char* GetName()             { return (char*)(this + 1); }
};

struct ImGuiSettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    ImGuiID     TypeHash;       // == ImHashStr(TypeName)
    void*       (*ReadOpen)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLine)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*ApplyAll)(ImGuiContext* ctx, ImGuiSettingsHandler* handler);                               // Read: Called after parsing of all .ini data is finished
    void        (*WriteAll)(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void*       UserData;

    ImGuiSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

// Storage for current popup stack
struct ImGuiPopupData
{
    ImGuiID             PopupId;        // Set when Popup is open.
    ImGuiWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    ImGuiWindow*        SourceWindow;   // Set on OpenPopup()
    int                 OpenFrameCount; // Frame in which the popup was opened
    ImGuiID             OpenParentId;   // Don't open if our parent disappeared
    ImVec2              OpenPopupPos;   // Position at time of opening
    ImVec2              OpenMousePos;   // Similarly, mouse position at time of opening
};

struct ImGuiNavItemData
{
    ImGuiWindow*        Window;         // Target window
    ImGuiID             ID;             // Target item id
    ImGuiID             FocusScopeId;   // Target focus scope id
    ImRect              RectRel;        // Target item's rectangle relative to its window
    ImGuiItemFlags      InFlags;        // Target item's flags
    float               DistBox;        // Distance to the source bounding box
    float               DistCenter;     // Distance to the source center
    float               DistAxial;

    ImGuiNavItemData() { Clear(); }
    void Clear()       { Window = NULL; ID = FocusScopeId = 0; InFlags = 0; DistBox = DistCenter = DistAxial = FLT_MAX; }
};

struct ImGuiLastItemData
{
    ImGuiID                 ID;
    ImGuiItemFlags          InFlags;            // See ImGuiItemFlags_
    ImGuiItemStatusFlags    StatusFlags;        // See ImGuiItemStatusFlags_
    ImRect                  Rect;               // Full rectangle of last item
    ImRect                  NavRect;            // Navigation scoring rectangle (not displayed)
    ImRect                  DisplayRect;        // Display rectangle (only if wrapping has occurred)

    ImGuiLastItemData()     { Clear(); }
    void Clear()            { memset(this, 0, sizeof(*this)); }
};

struct ImGuiStackSizes
{
    short   SizeOfIDStack;
    short   SizeOfColorStack;
    short   SizeOfStyleVarStack;
    short   SizeOfFontStack;
    short   SizeOfFocusScopeStack;
    short   SizeOfGroupStack;
    short   SizeOfBeginPopupStack;

    ImGuiStackSizes() { memset(this, 0, sizeof(*this)); }
    void SetToCurrentState();
    void CompareWithCurrentState();
};

// Data stored per window that we could ditch at the end of the frame
// In practice we currently keep it for each window visible in the frame.
struct ImGuiWindowTempData
{
    // Layout
    ImVec2                  CursorPos;              // Current cursor position in window coordinates
    ImVec2                  CursorMaxPos;           // [Internal] Used to detect extra spacing required for content extending beyond the right most edge of a window
    ImVec2                  IdealMaxPos;            // [Internal] Used to compute ideal window size from contents, used by SetNextWindowSizeConstraints() and e.g. auto-fitting windows.
    ImVec2                  CurrLineSize;
    float                   CurrLineTextBaseOffset;
    ImVec2                  PrevLineSize;
    float                   PrevLineTextBaseOffset;
    ImS32                   TreeDepth;
    ImU32                   TreeJumpToParentOnPopMask; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31.. Could be turned into a ImBitVector if required.
    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by Indent(), decreased by Unindent())
    ImVec1                  ColumnsOffset;          // Current columns offset
    ImVec1                  GroupOffset;
    ImGuiID                 LastItemId;
    ImGuiItemStatusFlags    LastItemStatusFlags;
    ImRect                  LastItemRect;           // Full rectangle of last item
    ImRect                  LastItemDisplayRect;    // End-user display rectangle of last item (useful for using IsItemVisible() after a draw command)

    // Navigation
    ImGuiNavLayer           NavLayerCurrent;        // Current navigation layer (0 for base layer, 1 for menu layer, etc.)
    short                   NavLayerActiveMask;     // Which layers have been written to (result from previous frame)
    short                   NavLayerActiveMaskNext; // Which layers have been written to (buffer for current frame)
    ImGuiID                 NavFocusScopeIdCurrent; // Current focus scope ID
    bool                    NavIdIsAlive;           // When set to true, means the LastItemId is active and navigation should be seen as active for this item.
    bool                    NavHasScroll;           // Set when scrolling has been applied (after Begin)
    bool                    NavIsScrollPushableX;   // Set when we can push into a child window created with horizontal scrolling
    bool                    NavIsVisible;           // Set when the navigation cursor is visible (and provided to user through GetNavFocusPos())
    ImRect                  NavRect;                // Bounding box of current navigation focus scope (stored relative to window position)

    // Other
    ImGuiWindow*            ParentWindow;           // [Internal] Points to parent window.
    ImGuiMenuColumns        MenuColumns;            // Special layout storage for menu items
    int                     StateStorageSize;
    ImGuiStorage*           StateStorage;
    ImGuiOldColumns*        CurrentColumns;
    int                     CurrentTableIdx;
    ImGuiLayoutType         LayoutType;
    ImGuiLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()

    // For IsRectVisible()
    ImVec2                  ClipRectStack[IM_ARRAYSIZE(ImDrawList::UnusedA)];

    ImGuiWindowTempData()
    {
        memset(this, 0, sizeof(*this));
    }
};

// Main ImGui context
struct ImGuiContext
{
    bool                    Initialized;
    bool                    FontAtlasOwnedByContext;    // IO.Fonts-> is owned by the ImGuiContext and will be destructed along with it.
    ImGuiIO                 IO;
    ImGuiStyle              Style;
    ImFont*                 Font;                       // (Shortcut) == IO.Font -> Current font
    float                   FontSize;                   // (Shortcut) == IO.Font->FontSize -> Current font size, layout and draw calls scale with this.
    float                   FontBaseSize;               // (Shortcut) == IO.Font->FontSize * IO.FontGlobalScale -> Used to compute ideal window sizes.
    ImDrawListSharedData    DrawListSharedData;
    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    bool                    WithinFrameScope;           // Set by NewFrame(), cleared by EndFrame()
    bool                    WithinFrameScopePair;       // Valid between NewFrame() and EndFrame()
    bool                    WithinEndChild;             // Set within EndChild()
    bool                    GcCompactAll;               // Request full GC
    bool                    TestEngineHookItems;        // Will call test engine hooks: ImGuiTestEngineHook_ItemAdd(), ImGuiTestEngineHook_ItemInfo(), ImGuiTestEngineHook_Log()
    void*                   TestEngine;                 // Test engine user data

    // Windows state
    ImVector<ImGuiWindow*>  Windows;                    // Windows, sorted in display order, back to front
    ImVector<ImGuiWindow*>  WindowsFocusOrder;          // Root windows, sorted in focus order, back to front.
    ImVector<ImGuiWindow*>  WindowsTempSortBuffer;      // Temporary buffer used in EndFrame() to reorder windows so parents are kept before their children.
    ImVector<ImGuiWindow*>  CurrentWindowStack;
    ImGuiStorage            WindowsById;                // Map window ID -> ImGuiWindow*
    int                     WindowsActiveCount;         // Number of unique windows submitted by frame
    ImVec2                  WindowsHoverPadding;        // Padding around a window to be considered hovered (in pixels)
    ImGuiWindow*            CurrentWindow;              // Window being drawn into
    ImGuiWindow*            HoveredWindow;              // Window hovered while not moving window (this is updated without delay)
    ImGuiWindow*            HoveredWindowUnderMovingWindow; // Window hovered when moving a window.
    ImGuiWindow*            MovingWindow;               // Track the window we are moving
    ImGuiID                 MovingWindowId;             // Moving window unique identifier.
    ImVec2                  MovingWindowOffset;         // Rented by start of window moving, reset when moving is finished.
    ImGuiID                 MovingWindowDecayTimer;     // Time since window moving has stopped.

    // Item/widget state
    ImGuiID                 HoveredId;                  // Hovered widget
    ImGuiID                 HoveredIdPreviousFrame;
    bool                    HoveredIdAllowOverlap;
    bool                    HoveredIdIsActive;          // Hovered widget is active (not necessarily same as ActiveId==HoveredId)
    bool                    HoveredIdIsBlocked;         // Hovered widget is blocked by a modal popup.
    float                   HoveredIdTimer;             // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;    // Measure contiguous hovering time where the item has not been active
    ImGuiID                 ActiveId;                   // Active widget
    ImGuiID                 ActiveIdIsAlive;            // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;    // Set true on the frame an item become active
    bool                    ActiveIdAllowOverlap;       // Active widget allows another widget to steal active id (generally for overlapping widgets, but not exclusively)
    bool                    ActiveIdNoClearOnFocusLoss; // Disable losing active id if focus is lost.
    bool                    ActiveIdHasBeenPressedBefore; // Track whether the active id led to a press (this is to allow releasing on same frame as press)
    bool                    ActiveIdHasBeenEditedBefore;  // Was the value associated to the widget edited over the course of the Active state.
    bool                    ActiveIdHasBeenEditedThisFrame;
    ImU32                   ActiveIdUsingNavDirMask;    // Active widget will want to read those nav move requests (e.g. CTRL+TAB)
    ImU32                   ActiveIdUsingKeyInputMask;  // Active widget will want to read all keyboard keys continuously.
    ImVec2                  ActiveIdClickOffset;        // Click offset from upper-left corner of the item.
    ImGuiWindow*            ActiveIdWindow;
    ImGuiInputSource        ActiveIdSource;             // Activating source: mouse, nav, etc.
    ImGuiID                 ActiveIdPreviousFrame;
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEditedBefore;
    ImGuiID                 LastActiveId;               // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;          // Store the last non-zero ActiveId timer.

    // Next window/item data
    ImGuiNextWindowData     NextWindowData;             // Storage for SetNextWindow** functions
    ImGuiNextItemData       NextItemData;               // Storage for SetNextItem** functions

    // Shared stacks
    ImVector<ImGuiColorMod> ColorStack;                 // Stack for PushStyleColor()/PopStyleColor()
    ImVector<ImGuiStyleMod> StyleVarStack;              // Stack for PushStyleVar()/PopStyleVar()
    ImVector<ImFont*>       FontStack;                  // Stack for PushFont()/PopFont()
    ImVector<ImGuiID>       FocusScopeStack;            // Stack for PushFocusScope()/PopFocusScope()
    ImVector<ImGuiItemFlags>ItemFlagsStack;             // Stack for PushItemFlag()/PopItemFlag()
    ImVector<ImGuiGroupData>GroupStack;                 // Stack for BeginGroup()/EndGroup()
    ImVector<ImGuiPopupData>OpenPopupStack;             // Which popups are open (persistent)
    ImVector<ImGuiPopupData>BeginPopupStack;            // Which level of BeginPopup() we are in (reset every frame)
    ImVector<ImGuiViewport*>Viewports;                  // Active viewports (always at least one, the main viewport).

    // Gamepad/keyboard navigation
    ImGuiWindow*            NavWindow;                  // Focused window for navigation. Could be called 'FocusWindow'
    ImGuiID                 NavId;                      // Focused item for navigation
    ImGuiID                 NavFocusScopeId;            // Focus scope for navigation
    ImGuiID                 NavActivateId;              // ~~ (g.ActiveId == 0) && IsNavInputPressed(ImGuiNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    ImGuiID                 NavActivateDownId;          // ~~ IsNavInputDown(ImGuiNavInput_Activate) ? NavId : 0
    ImGuiID                 NavActivatePressedId;       // ~~ IsNavInputPressed(ImGuiNavInput_Activate) ? NavId : 0
    ImGuiID                 NavJustMovedToId;           // Just navigated to this id (result of a successfully MoveRequest).
    ImGuiID                 NavJustMovedToFocusScopeId; // Just navigated to this focus scope id (result of a successfully MoveRequest).
    ImGuiKeyModFlags        NavJustMovedToKeyMods;
    ImGuiID                 NavNextActivateId;          // Set by ActivateItem(), queued until next frame.
    ImGuiInputSource        NavInputSource;             // Keyboard or Gamepad mode?
    ImRect                  NavScoringRect;             // Rectangle used for scoring, in screen space. Based of window->DC.NavRefRectRel.
    int                     NavScoringCount;            // Metrics for debugging
    ImGuiNavLayer           NavLayer;                   // Current navigation layer (0 for base layer, 1 for menu layer, etc.)
    int                     NavIdTabCounter;            // == NavWindow->DC.FocusIdxTabCounter at time of NavId processing
    bool                    NavIdIsAlive;               // When set to true, means g.NavId corresponds to an item that has been seen and is currently active.
    bool                    NavMousePosDirty;           // When set to true, navigation logic will query mouse position.
    bool                    NavDisableHighlight;        // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why checking g.IO.NavVisible is necessary)
    bool                    NavDisableMouseHover;       // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is moved again.
    bool                    NavAnyRequest;              // ~~ NavMoveRequest || NavInitRequest
    bool                    NavInitRequest;             // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    ImGuiID                 NavInitResultId;
    ImRect                  NavInitResultRectRel;
    bool                    NavMoveSubmitted;           // Move request submitted, will process result on next frame.
    bool                    NavMoveScoringItems;        // Move request submitted, still scoring incoming items.
    bool                    NavMoveForwardToNextFrame;
    ImGuiNavMoveFlags       NavMoveFlags;
    ImGuiScrollFlags        NavMoveScrollFlags;
    ImGuiKeyModFlags        NavMoveKeyMods;
    ImGuiDir                NavMoveDir;                 // Direction of the move request (left/right/up/down), direction of the scroll request (forward/backward)
    ImGuiDir                NavMoveDirForDebug;
    ImGuiDir                NavMoveClipDir;             // FIXME-NAV: Describe the purpose of this better. Might want to rename?
    ImGuiNavItemData        NavMoveResultLocal;         // Best move request candidate within the current window.
    ImGuiNavItemData        NavMoveResultLocalVisible;  // Best move request candidate within the current window that is visible.
    ImGuiNavItemData        NavMoveResultOther;         // Best move request candidate within the current window's hierarchy (beyond current window).
    ImGuiNavItemData        NavTabbingResultFirst;      // First tabbing item in the current window.

    // Tabbing
    ImGuiWindow*            NavTabbingWindow;           // Tabbing starts from this window.
    int                     NavTabbingCounter;          // Tabbing index within NavTabbingWindow.
    ImGuiID                 NavTabbingId;               // ID of the item that is being tabbed to.
    bool                    NavTabbingDir;              // Direction of the tabbing.
    bool                    NavTabbingDirty;            // Set when tabbing needs to be re-evaluated.

    // Render
    ImDrawData              DrawData;                   // Main ImDrawData instance to pass to back-end
    ImDrawDataBuilder       DrawDataBuilder;
    float                   DimBgRatio;                 // 0.0..1.0 amount of dimming behind modals, etc.
    ImDrawList              BackgroundDrawList;         // For invisible expanding button overlapping all other items
    ImDrawList              ForegroundDrawList;         // For tooltips, popups, etc.
    ImGuiMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSource;       // Set when within a BeginDragDropSource block
    bool                    DragDropWithinTarget;       // Set when within a BeginDragDropTarget block
    ImGuiDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    ImGuiPayload            DragDropPayload;
    ImRect                  DragDropTargetRect;
    ImGuiID                 DragDropTargetId;
    ImGuiDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface; // Target item surface (we resolve overlapping targets by picking the one with smallest surface)
    ImGuiID                 DragDropAcceptIdCurr;       // Target item id (under mouse position)
    ImGuiID                 DragDropAcceptIdPrev;       // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;   // Last time a target expressed a desire to accept the payload.
    ImGuiID                 DragDropHoldJustPressedId;  // Set when holding a payload just made it appear.
    ImVector<unsigned char> DragDropPayloadBufHeap;     // We don't expose the ImVector<> directly
    unsigned char           DragDropPayloadBufLocal[16];

    // Docking
    ImGuiDockContext*       DockContext;

    // Tab bars
    ImVector<ImGuiTabBar>   TabBars;
    ImGuiTabBar*            CurrentTabBar;
    ImGuiTabBar*            NextTabBar;

    // Tables
    ImVector<ImGuiTable>    Tables;
    ImGuiTable*             CurrentTable;
    ImGuiTable*             NextTable;
    ImPool<ImGuiTable>      TablesLastTimeActive;       // Last used tables, maybe move to a central GC pool.

    // Temp storage
    ImVector<ImGuiWindow*>  CurrentWindowStackSnapshot; // Snapshot of CurrentWindowStack for IsWindowWithinBeginStackOf()
    ImVector<ImGuiOldColumns>   CurrentColumnsStack;
    ImVector<ImGuiTableTempData>CurrentTableStack;
    ImVector<float>         FloatStack;                 // Temporary array of floats, used by e.g. CalcWrapWidthForPos()
    ImVector<ImVec2>        PathStack;                  // Temporary array of ImVec2, used by e.g. RenderArrow()

    // Logging
    ImGuiLogType            LogType;
    FILE*                   LogFile;                    // If != NULL log to stdout/ file
    ImGuiTextBuffer         LogBuffer;                  // Accumulation buffer when log to clipboard. This is pointer so our GImGui static constructor doesn't call heap allocators.
    const char*             LogNextPrefix;
    const char*             LogNextSuffix;
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;    // Default expansion depth when entering a new node

    // Misc
    float                   FramerateSecPerFrame[60];   // Calculate estimate of framerate for user over the last 60 frames..
    int                     FramerateSecPerFrameIdx;
    int                     FramerateSecPerFrameCount;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;  // Explicitly set want_capture_mouse for next frame (-1: unset, 0: clear, 1: set)
    int                     WantCaptureKeyboardNextFrame;// Explicitly set want_capture_keyboard for next frame (-1: unset, 0: clear, 1: set)
    int                     WantTextInputNextFrame;
    char                    TempBuffer[1024*3+1];       // Temporary text buffer

    ImGuiContext(ImFontAtlas* shared_font_atlas = NULL) : BackgroundDrawList(&DrawListSharedData), ForegroundDrawList(&DrawListSharedData)
    {
        Initialized = false;
        FontAtlasOwnedByContext = shared_font_atlas == NULL;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        IO.Ctx = this;
        Style.Ctx = this;
        Time = 0.0;
        FrameCount = 0;
        FrameCountEnded = -1;
        FrameCountRendered = -1;
        WithinFrameScope = WithinFrameScopePair = WithinEndChild = false;
        GcCompactAll = false;
        TestEngineHookItems = false;
        TestEngine = NULL;

        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredWindowUnderMovingWindow = NULL;
        MovingWindow = NULL;
        MovingWindowId = 0;
        MovingWindowDecayTimer = 0.0f;

        HoveredId = HoveredIdPreviousFrame = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdIsActive = false;
        HoveredIdIsBlocked = false;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdNoClearOnFocusLoss = false;
        ActiveIdHasBeenPressedBefore = false;
        ActiveIdHasBeenEditedBefore = false;
        ActiveIdHasBeenEditedThisFrame = false;
        ActiveIdUsingNavDirMask = 0x00;
        ActiveIdUsingKeyInputMask = 0x00;
        ActiveIdClickOffset = ImVec2(-1, -1);
        ActiveIdWindow = NULL;
        ActiveIdSource = ImGuiInputSource_None;
        ActiveIdPreviousFrame = 0;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEditedBefore = false;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;

        NavWindow = NULL;
        NavId = NavFocusScopeId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavJustMovedToId = NavJustMovedToFocusScopeId = NavNextActivateId = 0;
        NavJustMovedToKeyMods = ImGuiKeyModFlags_None;
        NavInputSource = ImGuiInputSource_None;
        NavScoringRect = ImRect();
        NavScoringCount = 0;
        NavLayer = ImGuiNavLayer_Main;
        NavIdTabCounter = INT_MAX;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveSubmitted = false;
        NavMoveScoringItems = false;
        NavMoveForwardToNextFrame = false;
        NavMoveFlags = ImGuiNavMoveFlags_None;
        NavMoveScrollFlags = ImGuiScrollFlags_None;
        NavMoveKeyMods = ImGuiKeyModFlags_None;
        NavMoveDir = NavMoveDirForDebug = NavMoveClipDir = ImGuiDir_None;

        NavTabbingWindow = NULL;
        NavTabbingCounter = 0;
        NavTabbingId = 0;
        NavTabbingDir = false;
        NavTabbingDirty = false;

        DimBgRatio = 0.0f;
        MouseCursor = ImGuiMouseCursor_Arrow;

        DragDropActive = DragDropWithinSource = DragDropWithinTarget = false;
        DragDropSourceFlags = ImGuiDragDropFlags_None;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = ImGuiDragDropFlags_None;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdCurr = DragDropAcceptIdPrev = 0;
        DragDropAcceptFrameCount = -1;
        DragDropHoldJustPressedId = 0;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        DockContext = NULL;

        CurrentTabBar = NextTabBar = NULL;
        CurrentTable = NextTable = NULL;

        LogType = ImGuiLogType_None;
        LogFile = NULL;
        LogNextPrefix = LogNextSuffix = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = FramerateSecPerFrameCount = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
        memset(TempBuffer, 0, sizeof(TempBuffer));

        if (shared_font_atlas)
        {
            IO.Fonts = shared_font_atlas;
            FontAtlasOwnedByContext = false;
        }
        else
        {
            IO.Fonts = IM_NEW(ImFontAtlas)();
            FontAtlasOwnedByContext = true;
        }
    }
};

//-----------------------------------------------------------------------------
// [SECTION] ImGuiWindow class
//-----------------------------------------------------------------------------

// An ImGui window.
// A window is rendering into a single ImDrawList.
// Each window can be split into vertical columns using BeginColumns()/EndColumns().
// 'Public' members are available to power users, but not advertised in imgui.h.
// 'Internal' members are not to be touched at all.
struct IMGUI_API ImGuiWindow
{
    char*                   Name;                               // Window name, owned by the window.
    ImGuiID                 ID;                                 // == ImHashStr(Name)
    ImGuiWindowFlags        Flags, FlagsPreviousFrame;          // See ImGuiWindowFlags
    ImGuiWindowClass        WindowClass;                        // User-provided class for custom settings, e.g. "Debug"
    ImGuiViewport*          Viewport;                           // User-provided viewport.
    ImGuiID                 ViewportId;                         // User-provided viewport id.
    ImVec2                  ViewportPos;                        // We backup the viewport position (after viewport merging) so we can restore it on subsequent frames if the viewport disappeared.
    int                     ViewportAllowPlatformMonitorDpiScale; // We backup this flag from the viewport so we can restore it on subsequent frames if the viewport disappeared.
    ImDrawList*             DrawList;                           // The draw list for the window.
    ImDrawList*             DrawListInst;                       // The draw list instance.
    ImGuiWindow*            ParentWindow;                       // If we are a child window, this is pointing to our parent. Otherwise NULL.
    ImGuiWindow*            RootWindow;                         // Point to ourself or first ancestor that is not a child window. Doesn't cross popups/modals.
    ImGuiWindow*            RootWindowPopupTree;                // Point to ourself or first ancestor that is not a child window. Crosses popups/modals. Used for IsWindowNavFocusable().
    ImGuiWindow*            RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will be highlighted with a colored title bar.
    ImGuiWindow*            RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    ImGuiID                 NavLastChildNavWindow;              // When going to the menu layer, this is the child window we came from. (This could be moved to the NavLayer data?)
    ImGuiID                 NavLastIds[ImGuiNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1).
    ImRect                  NavRectRel[ImGuiNavLayer_COUNT];    // Reference rectangle for navigation additions, in window-relative coordinates.
    int                     MemoryDrawListIdxCapacity;          // Draw list memory capacity to reduce temporary allocations.
    int                     MemoryDrawListVtxCapacity;
    bool                    MemoryCompacted;                    // Set when window extraneous data have been garbage collected

    // Positioning
    ImVec2                  Pos;                                // Position of the window
    ImVec2                  Size;                               // Current size (==SizeFull below)
    ImVec2                  SizeFull;                           // Size including non-client area along with menu-bar and scrollbars.
    ImVec2                  ContentSize;                        // Size of contents/scrollable client area (calculated from the extents reach of the cursor) from previous frame. Does not include window decoration or scrolling region.
    ImVec2                  ContentSizeIdeal;
    ImVec2                  ContentSizeExplicit;                // Size of contents/scrollable client area explicitly request by the user via SetNextWindowContentSize().
    ImVec2                  WindowPadding;                      // Window padding at the time of Begin().
    float                   WindowRounding;                     // Window rounding at the time of Begin().
    float                   WindowBorderSize;                   // Window border size at the time of Begin().
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    ImGuiID                 MoveId;                             // == window->GetID("#MOVE")
    ImGuiID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    ImVec2                  Scroll;
    ImVec2                  ScrollMax;
    ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    ImVec2                  ScrollTargetCenterRatio;            // 0.0f = top, 0.5f = center, 1.0f = bottom. When using SetScrollHereY() or SetScrollFromPosY() the calculation will use this ratio to scroll to a given position.
    ImVec2                  ScrollTargetEdgeSnapDist;           // 0.0f = no snap, >0.0f snapping threshold
    ImVec2                  ScrollbarSizes;                     // Size taken by each scrollbars on their smaller axis. Pay attention! ScrollbarSizes.x == width of the vertical scrollbar, ScrollbarSizes.y = height of the horizontal scrollbar.
    bool                    ScrollbarX, ScrollbarY;             // Are scrollbars visible?
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Is the window collapsed? (an active window may be collapsed)
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be skipped (e.g. window is collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (and skip items from being rendered)
    bool                    IsFallbackWindow;                   // Set on the "Debug##Default" window.
    bool                    IsExplicitChild;                    // Set when passed a parent* argument to Begin()/BeginChild()
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, 0-3: corners, 4-7: borders)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Begin() order within immediate parent window, if we are a child window.
    short                   BeginOrderWithinContext;            // Begin() order within entire context.
    short                   FocusOrder;                         // Order within WindowsFocusOrder[], used when sorting the focus stack.
    ImGuiID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    ImS8                    AutoFitFramesX, AutoFitFramesY;
    ImS8                    AutoFitChildFramesX, AutoFitChildFramesY;
    bool                    AutoFitOnlyGrows;
    ImGuiDir                AutoPosLastDirection;
    ImS8                    HiddenFramesCanSkipItems;           // Hide the window for N frames
    ImS8                    HiddenFramesCannotSkipItems;        // Hide the window for N frames and don't allow skipping inside the window
    ImS8                    HiddenFramesForRenderOnly;          // Hide the window for N frames, but rendering is still occurring
    ImS8                    DisableInputsFrames;                // Disable window interactions for N frames
    ImGuiCond               SetWindowPosAllowFlags;             // store condition flags for next SetWindowPos() call.
    ImGuiCond               SetWindowSizeAllowFlags;            // store condition flags for next SetWindowSize() call.
    ImGuiCond               SetWindowCollapsedAllowFlags;       // store condition flags for next SetWindowCollapsed() call.
    ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero condition
    ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning

    ImGuiWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called ImGuiDrawContext, hence the DC variable name.
    ImVector<ImGuiID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack. (In theory this should be in the TempData structure)
    ImGuiStorage            StateStorage;                       // Storage for current window (if any)
    ImGuiOldColumns*        CurrentColumns;                     // Current column set
    int                     CurrentTableIdx;                    // Index of current table in g.Tables
    ImGuiLayoutType         LayoutType;                         // Layout type, see ImGuiLayoutType_
    ImGuiWindow*            ParentWindowInBeginStack;
    float                   OuterRectClippedA_Backup;           // Main clipping rectangle of the window
    float                   OuterRectClippedB_Backup;

public:
    ImGuiWindow(ImGuiContext* context, const char* name);
    ~ImGuiWindow();

    ImGuiID     GetID(const char* str, const char* str_end = NULL);
    ImGuiID     GetID(const void* ptr);
    ImGuiID     GetID(int n);
    ImGuiID     GetIDFromRectangle(const ImRect& r_abs);

    // We don't publicize the Begin/End functions directly so you cannot be tempted to call them out of order.
    // We do however need them declared here so they can be called by the docking functions.
    void        Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    void        End();

    void        BringToFocusFront(int flags = 0);
    void        BringToDisplayFront();
    void        BringToDisplayBack();
    void        BringToDisplayBehind(ImGuiWindow* behind_window);
    bool        IsChildWindow() const { return ParentWindow != NULL && IsExplicitChild; }
    bool        IsFocused(ImGuiFocusedFlags flags = 0) const;
    bool        IsHovered(ImGuiHoveredFlags flags = 0) const;
    bool        IsRectVisible(const ImRect& rect) const;
    bool        IsPosVisible(const ImVec2& pos) const;

    ImRect      Rect() const            { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    ImRect      InnerRect() const       { return ImRect(Pos.x + WindowPadding.x, Pos.y + WindowPadding.y, Pos.x + Size.x - WindowPadding.x, Pos.y + Size.y - WindowPadding.y); }
    ImRect      ContentRect() const     { return ImRect(Pos.x + WindowPadding.x, Pos.y + WindowPadding.y + TitleBarHeight() + MenuBarHeight(), Pos.x + Size.x - WindowPadding.x, Pos.y + Size.y - WindowPadding.y - ScrollbarSizes.y); }
    ImRect      WorkRect() const        { return ImRect(Pos.x, Pos.y + TitleBarHeight() + MenuBarHeight(), Pos.x + Size.x, Pos.y + Size.y); }
    float       TitleBarHeight() const  { return (Flags & ImGuiWindowFlags_NoTitleBar) ? 0.0f : DC.CurrLineSize.y; }
    float       MenuBarHeight() const   { return (Flags & ImGuiWindowFlags_MenuBar) ? DC.MenuBarOffset.y : 0.0f; }
};

//-----------------------------------------------------------------------------
// [SECTION] Tab bar structures
//-----------------------------------------------------------------------------

struct ImGuiTabBar
{
    ImVector<ImGuiTabItem>  Tabs;
    ImGuiID                 ID;
    ImGuiID                 SelectedTabId;
    ImGuiID                 NextSelectedTabId;
    ImGuiID                 VisibleTabId;
    int                     CurrFrameVisible;
    int                     PrevFrameVisible;
    ImRect                  BarRect;
    float                   CurrValue;
    float                   PrevValue;
    ImGuiAxis               Axis;
    ImGuiTabBarFlags        Flags;
    ImGuiID                 ReorderRequestTabId;
    ImS8                    ReorderRequestDir;
    bool                    WantLayout;
    bool                    VisibleTabWasSubmitted;
    short                   LastTabItemIdx;
    float                   ItemSpacingY;
    ImVec2                  FramePadding;
    ImVec2                  BackupCursorPos;
    ImGuiTextBuffer         TabsNames;

    ImGuiTabBar();
    int GetTabOrder(const ImGuiTabItem* tab) const;
    const char* GetTabName(const ImGuiTabItem* tab) const;
};

struct ImGuiTabItem
{
    ImGuiID                 ID;
    ImGuiTabItemFlags       Flags;
    ImGuiWindow*            Window;
    int                     LastFrameVisible;
    int                     LastFrameSelected;
    float                   Offset;
    float                   Width;
    float                   ContentWidth;
    ImS16                   NameOffset;
    ImS16                   BeginOrder;
    ImS16                   IndexDuringLayout;
    bool                    WantClose;

    ImGuiTabItem() { ID = 0; Flags = ImGuiTabItemFlags_None; Window = NULL; LastFrameVisible = -1; LastFrameSelected = -1; Offset = 0.0f; Width = ContentWidth = 0.0f; NameOffset = -1; BeginOrder = -1; IndexDuringLayout = -1; WantClose = false; }
};

//-----------------------------------------------------------------------------
// [SECTION] Table structures
//-----------------------------------------------------------------------------

struct ImGuiTableColumn
{
    ImGuiTableColumnFlags   Flags;
    float                   WidthGiven;
    float                   MinX;
    float                   MaxX;
    float                   WidthRequest;
    float                   WidthAuto;
    float                   StretchWeight;
    float                   InitStretchWeightOrWidth;
    ImRect                  ClipRect;
    ImGuiID                 UserID;
    float                   WorkMinX;
    float                   WorkMaxX;
    float                   ItemWidth;
    float                   ContentMaxXFrozen;
    float                   ContentMaxXUnfrozen;
    float                   ContentMaxXHeadersUsed;
    float                   ContentMaxXHeadersIdeal;
    short                   NameOffset;
    ImGuiTableColumnIdx     DisplayOrder;
    ImGuiTableColumnIdx     IndexWithinEnabledSet;
    ImGuiTableColumnIdx     PrevEnabledColumn;
    ImGuiTableColumnIdx     NextEnabledColumn;
    ImGuiTableColumnIdx     SortOrder;
    ImGuiSortDirection      SortDirection;
    ImGuiTableColumnIdx     SortDirectionsAvailCount;
    ImGuiTableColumnIdx     SortDirectionsAvailMask;
    ImGuiTableColumnIdx     SortDirectionsAvailLast;

    ImGuiTableColumn()      { memset(this, 0, sizeof(*this)); StretchWeight = -1.0f; }
};

struct ImGuiTable
{
    ImGuiID                 ID;
    ImGuiTableFlags         Flags;
    void*                   RawData;
    ImGuiTableTempData*     TempData;
    ImSpan<ImGuiTableColumn>Columns;
    ImSpan<ImGuiTableColumnIdx>DisplayOrderToIndex;
    ImSpan<ImGuiTableCellData>RowCellData;
    ImBitArray<IMGUI_TABLE_MAX_COLUMNS> EnabledColumns;
    int                     ColumnsCount;
    int                     EnabledMask;
    int                     RowBgColorCounter;
    int                     RowCellDataCurrent;
    float                   RowMinHeight;
    float                   RowMaxHeight;
    float                   HeaderHeight;
    float                   FooterHeight;
    float                   ColumnsTotalWidth;
    float                   ColumnsAutoFitWidth;
    float                   ResizedColumnNextWidth;
    float                   ResizeLockMinContentsX2;
    float                   RefScale;
    ImRect                  OuterRect;
    ImRect                  InnerRect;
    ImRect                  WorkRect;
    ImRect                  ClipRect;
    ImRect                  HostClipRect;
    ImRect                  HostBackupInnerClipRect;
    ImGuiWindow*            InnerWindow;
    ImGuiWindow*            HostWindow;
    ImGuiTextBuffer         ColumnsNames;
    ImGuiTableColumnIdx     ResizedColumn;
    ImGuiTableColumnIdx     LastResizedColumn;
    ImGuiTableColumnIdx     HeldHeaderColumn;
    ImGuiTableColumnIdx     ReorderColumn;
    ImGuiTableColumnIdx     ReorderColumnDir;
    ImGuiTableColumnIdx     HoveredColumnBody;
    ImGuiTableColumnIdx     HoveredColumnHeader;
    short                   OuterWindowCheckboxPos;
    short                   LastFrameActive;
    ImGuiTableColumnIdx     FreezeCols;
    ImGuiTableColumnIdx     FreezeRows;
    ImGuiTableColumnIdx     RowCellDataAvail;
    ImGuiTableColumnIdx     RowCellDataLimit;
    ImGuiTableColumnIdx     SortSpecsCount;
    ImGuiTableSortSpecs*    SortSpecs;
    bool                    SortSpecsDirty;
    bool                    WantSort;
    bool                    WantSettingsSave;

    ImGuiTable()            { memset(this, 0, sizeof(*this)); LastFrameActive = -1; }
    ~ImGuiTable()           { IM_FREE(RawData); }
};

//-----------------------------------------------------------------------------
// [SECTION] Obsolete structures (will be removed)
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
struct ImGuiImeData
{
    bool WantVisible;
    ImGuiImeData() { WantVisible = false; }
};
#endif

//-----------------------------------------------------------------------------
// [SECTION] Helpers for static analysis
//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif // #ifndef IMGUI_DISABLE
