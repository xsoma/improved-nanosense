// dear vsonyp0wer, v1.70 WIP
// (headers)

// See vsonyp0wer.cpp file for documentation.
// Call and read vsonyp0wer::ShowDemoWindow() in vsonyp0wer_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' in vsonyp0wer.cpp for notes on how to setup Dear vsonyp0wer in your codebase.
// Get latest version at https://github.com/ocornut/vsonyp0wer

/*

Index of this file:
// Header mess
// Forward declarations and basic types
// vsonyp0wer API (Dear vsonyp0wer end-user API)
// Flags & Enumerations
// Memory allocations macros
// ImVector<>
// vsonyp0werStyle
// vsonyp0werIO
// Misc data structures (vsonyp0werInputTextCallbackData, vsonyp0werSizeCallbackData, vsonyp0werPayload)
// Obsolete functions
// Helpers (vsonyp0werOnceUponAFrame, vsonyp0werTextFilter, vsonyp0werTextBuffer, vsonyp0werStohnly, vsonyp0werListClipper, ImColor)
// Draw List API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)

*/

#pragma once

// Configuration file with compile-time options (edit imconfig.h or define vsonyp0wer_USER_CONFIG to your own filename)
#ifdef vsonyp0wer_USER_CONFIG
#include vsonyp0wer_USER_CONFIG
#endif
#if !defined(vsonyp0wer_DISABLE_INCLUDE_IMCONFIG_H) || defined(vsonyp0wer_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#include <float.h>                  // FLT_MAX
#include <stdarg.h>                 // va_list
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY99 then bounce up to XYY00, XYY01 etc. when release tagging happens)
#define vsonyp0wer_VERSION               "1.70 WIP"
#define vsonyp0wer_VERSION_NUM           16990
#define vsonyp0wer_CHECKVERSION()        vsonyp0wer::DebugCheckVersionAndDataLayout(vsonyp0wer_VERSION, sizeof(vsonyp0werIO), sizeof(vsonyp0werStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert))

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// vsonyp0wer_API is used for core vsonyp0wer functions, vsonyp0wer_IMPL_API is used for the default bindings files (vsonyp0wer_impl_xxx.h)
#ifndef vsonyp0wer_API
#define vsonyp0wer_API
#endif
#ifndef vsonyp0wer_IMPL_API
#define vsonyp0wer_IMPL_API              vsonyp0wer_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#if defined(__clang__) || defined(__GNUC__)
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // Apply printf-style warnings to user functions.
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))         // Size of a static C-style array. Don't use on pointers!
#define IM_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Standardized as offsetof() in modern C++.
#define IM_UNUSED(_VAR)             ((void)_VAR)                                // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.

// Warnings
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

struct ImDrawChannel;               // Temporary stohnly for ImDrawList ot output draw commands out of order, used by ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent vsonyp0wer context, but you may create one yourself)
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with vsonyp0wer_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct vsonyp0werContext;                // Dear vsonyp0wer context (opaque structure, unless including vsonyp0wer_internal.h)
struct vsonyp0werIO;                     // Main configuration and I/O between your application and vsonyp0wer
struct vsonyp0werInputTextCallbackData;  // Shared state of InputText() when using custom vsonyp0werInputTextCallback (rare/advanced use)
struct vsonyp0werListClipper;            // Helper to manually clip large list of items
struct vsonyp0werOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by vsonyp0wer_ONCE_UPON_A_FRAME macro
struct vsonyp0werPayload;                // User data payload for drag and drop operations
struct vsonyp0werSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct vsonyp0werStohnly;                // Helper for key->value stohnly
struct vsonyp0werStyle;                  // Runtime data for styling/colors
struct vsonyp0werTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct vsonyp0werTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbb][,ccccc]")

// Typedefs and Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
#ifndef ImTextureID
typedef void* ImTextureID;          // User data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in vsonyp0wer.cpp)
#endif
typedef unsigned int vsonyp0werID;       // Unique ID used by widgets (typically hashed from a stack of string)
typedef unsigned short ImWchar;     // A single U16 character for keyboard input/display. We encode them as multi bytes UTF-8 when used in strings.
typedef int vsonyp0werCol;               // -> enum vsonyp0werCol_             // Enum: A color identifier for styling
typedef int vsonyp0werCond;              // -> enum vsonyp0werCond_            // Enum: A condition for Set*()
typedef int vsonyp0werDataType;          // -> enum vsonyp0werDataType_        // Enum: A primary data type
typedef int vsonyp0werDir;               // -> enum vsonyp0werDir_             // Enum: A cardinal direction
typedef int vsonyp0werKey;               // -> enum vsonyp0werKey_             // Enum: A key identifier (vsonyp0wer-side enum)
typedef int vsonyp0werNavInput;          // -> enum vsonyp0werNavInput_        // Enum: An input identifier for navigation
typedef int vsonyp0werMouseCursor;       // -> enum vsonyp0werMouseCursor_     // Enum: A mouse cursor identifier
typedef int vsonyp0werStyleVar;          // -> enum vsonyp0werStyleVar_        // Enum: A variable identifier for styling
typedef int ImDrawCornerFlags;      // -> enum ImDrawCornerFlags_    // Flags: for ImDrawList::AddRect*() etc.
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int vsonyp0werBackendFlags;      // -> enum vsonyp0werBackendFlags_    // Flags: for io.BackendFlags
typedef int vsonyp0werColorEditFlags;    // -> enum vsonyp0werColorEditFlags_  // Flags: for ColorEdit*(), ColorPicker*()
typedef int vsonyp0werColumnsFlags;      // -> enum vsonyp0werColumnsFlags_    // Flags: for Columns(), BeginColumns()
typedef int vsonyp0werConfigFlags;       // -> enum vsonyp0werConfigFlags_     // Flags: for io.ConfigFlags
typedef int vsonyp0werComboFlags;        // -> enum vsonyp0werComboFlags_      // Flags: for BeginCombo()
typedef int vsonyp0werDragDropFlags;     // -> enum vsonyp0werDragDropFlags_   // Flags: for *DragDrop*()
typedef int vsonyp0werFocusedFlags;      // -> enum vsonyp0werFocusedFlags_    // Flags: for IsWindowFocused()
typedef int vsonyp0werHoveredFlags;      // -> enum vsonyp0werHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int vsonyp0werInputTextFlags;    // -> enum vsonyp0werInputTextFlags_  // Flags: for InputText*()
typedef int vsonyp0werSelectableFlags;   // -> enum vsonyp0werSelectableFlags_ // Flags: for Selectable()
typedef int vsonyp0werTabBarFlags;       // -> enum vsonyp0werTabBarFlags_     // Flags: for BeginTabBar()
typedef int vsonyp0werTabItemFlags;      // -> enum vsonyp0werTabItemFlags_    // Flags: for BeginTabItem()
typedef int vsonyp0werTreeNodeFlags;     // -> enum vsonyp0werTreeNodeFlags_   // Flags: for TreeNode*(),CollapsingHeader()
typedef int vsonyp0werWindowFlags;       // -> enum vsonyp0werWindowFlags_     // Flags: for Begin*()
typedef int (*vsonyp0werInputTextCallback)(vsonyp0werInputTextCallbackData* data);
typedef void (*vsonyp0werSizeCallback)(vsonyp0werSizeCallbackData* data);

// Scalar data types
typedef signed char         ImS8;   // 8-bit signed integer == char
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct ImVec2
{
    float     x, y;
    ImVec2() { x = y = 0.0f; }
    ImVec2(float _x, float _y) { x = _x; y = _y; }
    float  operator[] (size_t idx) const { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    float& operator[] (size_t idx) { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// 4D vector (often used to store floating-point colors)
struct ImVec4
{
    float     x, y, z, w;
    ImVec4() { x = y = z = w = 0.0f; }
    ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};

//-----------------------------------------------------------------------------
// vsonyp0wer: Dear vsonyp0wer end-user API
// (Inside a namespace so you can add extra functions in your own separate file. Please don't modify vsonyp0wer.cpp/.h!)
//-----------------------------------------------------------------------------

namespace vsonyp0wer
{
    // Context creation and access
    // Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between vsonyp0wer contexts.
    // All those functions are not reliant on the current context.
    vsonyp0wer_API vsonyp0werContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    vsonyp0wer_API void          DestroyContext(vsonyp0werContext* ctx = NULL);   // NULL = destroy current context
    vsonyp0wer_API vsonyp0werContext* GetCurrentContext();
    vsonyp0wer_API void          SetCurrentContext(vsonyp0werContext* ctx);
    vsonyp0wer_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert);

    // Main
    vsonyp0wer_API vsonyp0werIO& GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
    vsonyp0wer_API vsonyp0werStyle& GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.
    vsonyp0wer_API void          NewFrame();                                 // start a new vsonyp0wer frame, you can submit any command from this point until Render()/EndFrame().
    vsonyp0wer_API void          EndFrame();                                 // ends the vsonyp0wer frame. automatically called by Render(), you likely don't need to call that yourself directly. If you don't need to render data (skipping rendering) you may call EndFrame() but you'll have wasted CPU already! If you don't need to render, better to not create any vsonyp0wer windows and not call NewFrame() at all!
    vsonyp0wer_API void          Render();                                   // ends the vsonyp0wer frame, finalize the draw data. (Obsolete: optionally call io.RenderDrawListsFn if set. Nowadays, prefer calling your render function yourself.)
    vsonyp0wer_API ImDrawData* GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render. (Obsolete: this used to be passed to your io.RenderDrawListsFn() function.)

    // Demo, Debug, Information
    vsonyp0wer_API void          ShowDemoWindow(bool* p_open = NULL);        // create demo/test window (previously called ShowTestWindow). demonstrate most vsonyp0wer features. call this to learn about the library! try to make it always available in your application!
    vsonyp0wer_API void          ShowAboutWindow(bool* p_open = NULL);       // create about window. display Dear vsonyp0wer version, credits and build/system information.
    vsonyp0wer_API void          ShowMetricsWindow(bool* p_open = NULL);     // create metrics/debug window. display Dear vsonyp0wer internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
    vsonyp0wer_API void          ShowStyleEditor(vsonyp0werStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference vsonyp0werStyle structure to compare to, revert to and save to (else it uses the default style)
    vsonyp0wer_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
    vsonyp0wer_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
    vsonyp0wer_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate vsonyp0wer as a end-user (mouse/keyboard controls).
    vsonyp0wer_API const char* GetVersion();                               // get the compiled version string e.g. "1.23" (essentially the compiled value for vsonyp0wer_VERSION)

    // Styles
    vsonyp0wer_API void          StyleColorsDark(vsonyp0werStyle* dst = NULL);    // new, recommended style (default)
    vsonyp0wer_API void          StyleColorsClassic(vsonyp0werStyle* dst = NULL); // classic vsonyp0wer style
    vsonyp0wer_API void          StyleColorsLight(vsonyp0werStyle* dst = NULL);   // best used with borders and a custom, thicker font

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - You may append multiple times to the same window during the same frame.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc.
    //    where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    vsonyp0wer_API bool          Begin(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
    vsonyp0wer_API void          End();

	vsonyp0wer_API bool          BeginColorPickerBackground(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginTabsBackground(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginMenuBackground(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginShadowBackground(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginGroupBoxMain(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginGroupBoxScrollMain(const char* name, const char* groupboxName, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginComboBackground(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginEndBorder(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
    vsonyp0wer_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
    vsonyp0wer_API bool          BeginChild(vsonyp0werID id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
    vsonyp0wer_API void          EndChild();

	vsonyp0wer_API bool          BeginTabs(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginTabs(vsonyp0werID id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API void          EndTabs();

	vsonyp0wer_API bool          EndBorder(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          EndBorder(vsonyp0werID id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API void          EndEndBorder();

	vsonyp0wer_API bool          BeginShadow(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginShadow(vsonyp0werID id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API void          EndShadow();

	vsonyp0wer_API bool          BeginGroupBox(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginGroupBox(vsonyp0werID id, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API void          EndGroupBox();

	vsonyp0wer_API bool          BeginGroupBoxScroll(const char* str_id, const char* groupboxName, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API bool          BeginGroupBoxScroll(vsonyp0werID id, const char* groupboxName, const ImVec2& size = ImVec2(0, 0), bool border = false, vsonyp0werWindowFlags flags = 0);
	vsonyp0wer_API void          EndGroupBoxScroll();

    // Windows Utilities
    // - "current window" = the window we are appending into while inside a Begin()/End() block. "next window" = next window we will Begin() into.
    vsonyp0wer_API bool          IsWindowAppearing();
    vsonyp0wer_API bool          IsWindowCollapsed();
    vsonyp0wer_API bool          IsWindowFocused(vsonyp0werFocusedFlags flags = 0); // is current window focused? or its root/child, depending on flags. see flags for options.
    vsonyp0wer_API bool          IsWindowHovered(vsonyp0werHoveredFlags flags = 0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to vsonyp0wer or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    vsonyp0wer_API ImDrawList* GetWindowDrawList();                        // get draw list associated to the current window, to append your own drawing primitives
    vsonyp0wer_API ImVec2        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
    vsonyp0wer_API ImVec2        GetWindowSize();                            // get current window size
    vsonyp0wer_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
    vsonyp0wer_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)

    // Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
    vsonyp0wer_API void          SetNextWindowPos(const ImVec2& pos, vsonyp0werCond cond = 0, const ImVec2& pivot = ImVec2(0, 0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    vsonyp0wer_API void          SetNextWindowSize(const ImVec2& size, vsonyp0werCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    vsonyp0wer_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, vsonyp0werSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Use callback to apply non-trivial programmatic constraints.
    vsonyp0wer_API void          SetNextWindowContentSize(const ImVec2& size);                               // set next window content size (~ enforce the range of scrollbars). not including window decorations (title bar, menu bar, etc.). set an axis to 0.0f to leave it automatic. call before Begin()
    vsonyp0wer_API void          SetNextWindowCollapsed(bool collapsed, vsonyp0werCond cond = 0);                 // set next window collapsed state. call before Begin()
    vsonyp0wer_API void          SetNextWindowFocus();                                                       // set next window to be focused / front-most. call before Begin()
    vsonyp0wer_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily modify vsonyp0werCol_WindowBg/ChildBg/PopupBg. you may also use vsonyp0werWindowFlags_NoBackground.
    vsonyp0wer_API void          SetWindowPos(const ImVec2& pos, vsonyp0werCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    vsonyp0wer_API void          SetWindowSize(const ImVec2& size, vsonyp0werCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
    vsonyp0wer_API void          SetWindowCollapsed(bool collapsed, vsonyp0werCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    vsonyp0wer_API void          SetWindowFocus();                                                           // (not recommended) set current window to be focused / front-most. prefer using SetNextWindowFocus().
    vsonyp0wer_API void          SetWindowFontScale(float scale);                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows
    vsonyp0wer_API void          SetWindowPos(const char* name, const ImVec2& pos, vsonyp0werCond cond = 0);      // set named window position.
    vsonyp0wer_API void          SetWindowSize(const char* name, const ImVec2& size, vsonyp0werCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
    vsonyp0wer_API void          SetWindowCollapsed(const char* name, bool collapsed, vsonyp0werCond cond = 0);   // set named window collapsed state
    vsonyp0wer_API void          SetWindowFocus(const char* name);                                           // set named window to be focused / front-most. use NULL to remove focus.

    // Content region
    // - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
    vsonyp0wer_API ImVec2        GetContentRegionMax();                                          // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
    vsonyp0wer_API ImVec2        GetContentRegionAvail();                                        // == GetContentRegionMax() - GetCursorPos()
    vsonyp0wer_API float         GetContentRegionAvailWidth();                                   // == GetContentRegionAvail().x
    vsonyp0wer_API ImVec2        GetWindowContentRegionMin();                                    // content boundaries min (roughly (0,0)-Scroll), in window coordinates
    vsonyp0wer_API ImVec2        GetWindowContentRegionMax();                                    // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
    vsonyp0wer_API float         GetWindowContentRegionWidth();                                  //

    // Windows Scrolling
    vsonyp0wer_API float         GetScrollX();                                                   // get scrolling amount [0..GetScrollMaxX()]
    vsonyp0wer_API float         GetScrollY();                                                   // get scrolling amount [0..GetScrollMaxY()]
    vsonyp0wer_API float         GetScrollMaxX();                                                // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
    vsonyp0wer_API float         GetScrollMaxY();                                                // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
    vsonyp0wer_API void          SetScrollX(float scroll_x);                                     // set scrolling amount [0..GetScrollMaxX()]
    vsonyp0wer_API void          SetScrollY(float scroll_y);                                     // set scrolling amount [0..GetScrollMaxY()]
    vsonyp0wer_API void          SetScrollHereY(float center_y_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    vsonyp0wer_API void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

    // Parameters stacks (shared)
    vsonyp0wer_API void          PushFont(ImFont* font);                                         // use NULL as a shortcut to push default font
    vsonyp0wer_API void          PopFont();
    vsonyp0wer_API void          PushStyleColor(vsonyp0werCol idx, ImU32 col);
    vsonyp0wer_API void          PushStyleColor(vsonyp0werCol idx, const ImVec4& col);
	vsonyp0wer_API void			PushColor(vsonyp0werCol idx, vsonyp0werCol idx2, const ImVec4& col);
    vsonyp0wer_API void          PopStyleColor(int count = 1);
    vsonyp0wer_API void          PushStyleVar(vsonyp0werStyleVar idx, float val);
    vsonyp0wer_API void          PushStyleVar(vsonyp0werStyleVar idx, const ImVec2& val);
    vsonyp0wer_API void          PopStyleVar(int count = 1);
    vsonyp0wer_API const ImVec4& GetStyleColorVec4(vsonyp0werCol idx);                                // retrieve style color as stored in vsonyp0werStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.
    vsonyp0wer_API ImFont* GetFont();                                                      // get current font
    vsonyp0wer_API float         GetFontSize();                                                  // get current font size (= height in pixels) of current font with current scale applied
    vsonyp0wer_API ImVec2        GetFontTexUvWhitePixel();                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
    vsonyp0wer_API ImU32         GetColorU32(vsonyp0werCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier
    vsonyp0wer_API ImU32         GetColorU32(const ImVec4& col);                                 // retrieve given color with style alpha applied
    vsonyp0wer_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied

    // Parameters stacks (current window)
    vsonyp0wer_API void          PushItemWidth(float item_width);                                // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
    vsonyp0wer_API void          PopItemWidth();
    vsonyp0wer_API float         CalcItemWidth();                                                // width of item given pushed settings and current cursor position
    vsonyp0wer_API void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);                 // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
    vsonyp0wer_API void          PopTextWrapPos();
    vsonyp0wer_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);              // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
    vsonyp0wer_API void          PopAllowKeyboardFocus();
    vsonyp0wer_API void          PushButtonRepeat(bool repeat);                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
    vsonyp0wer_API void          PopButtonRepeat();

    // Cursor / Layout
    // - By "cursor" we mean the current output position.
    // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
    vsonyp0wer_API void          Separator();                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
    vsonyp0wer_API void          SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);  // call between widgets or groups to layout them horizontally. X position given in window coordinates.
    vsonyp0wer_API void          NewLine();                                                      // undo a SameLine() or force a new line when in an horizontal-layout context.
    vsonyp0wer_API void          Spacing();                                                      // add vertical spacing.
	vsonyp0wer_API void			CustomSpacing(float h);											// add custom vertical spacing
    vsonyp0wer_API void          Dummy(const ImVec2& size);                                      // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
    vsonyp0wer_API void          Indent(float indent_w = 0.0f);                                  // move content position toward the right, by style.IndentSpacing or indent_w if != 0
    vsonyp0wer_API void          Unindent(float indent_w = 0.0f);                                // move content position back to the left, by style.IndentSpacing or indent_w if != 0
    vsonyp0wer_API void          BeginGroup();                                                   // lock horizontal starting position
    vsonyp0wer_API void          EndGroup();                                                     // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
    vsonyp0wer_API ImVec2        GetCursorPos();                                                 // cursor position in window coordinates (relative to window position)
    vsonyp0wer_API float         GetCursorPosX();                                                //   (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc.
    vsonyp0wer_API float         GetCursorPosY();                                                //    other functions such as GetCursorScreenPos or everything in ImDrawList::
    vsonyp0wer_API void          SetCursorPos(const ImVec2& local_pos);                          //    are using the main, absolute coordinate system.
    vsonyp0wer_API void          SetCursorPosX(float local_x);                                   //    GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.)
    vsonyp0wer_API void          SetCursorPosY(float local_y);                                   //
    vsonyp0wer_API ImVec2        GetCursorStartPos();                                            // initial cursor position in window coordinates
    vsonyp0wer_API ImVec2        GetCursorScreenPos();                                           // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
    vsonyp0wer_API void          SetCursorScreenPos(const ImVec2& pos);                          // cursor position in absolute screen coordinates [0..io.DisplaySize]
    vsonyp0wer_API void          AlignTextToFramePadding();                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
    vsonyp0wer_API float         GetTextLineHeight();                                            // ~ FontSize
    vsonyp0wer_API float         GetTextLineHeightWithSpacing();                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
    vsonyp0wer_API float         GetFrameHeight();                                               // ~ FontSize + style.FramePadding.y * 2
    vsonyp0wer_API float         GetFrameHeightWithSpacing();                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

    // ID stack/scopes
    // - Read the FAQ for more details about how ID are handled in dear vsonyp0wer. If you are creating widgets in a loop you most
    //   likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
    // - The resulting ID are hashes of the entire stack.
    // - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
    // - In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
    //   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
    vsonyp0wer_API void          PushID(const char* str_id);                                     // push string into the ID stack (will hash string).
    vsonyp0wer_API void          PushID(const char* str_id_begin, const char* str_id_end);       // push string into the ID stack (will hash string).
    vsonyp0wer_API void          PushID(const void* ptr_id);                                     // push pointer into the ID stack (will hash pointer).
    vsonyp0wer_API void          PushID(int int_id);                                             // push integer into the ID stack (will hash integer).
    vsonyp0wer_API void          PopID();                                                        // pop from the ID stack.
    vsonyp0wer_API vsonyp0werID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into vsonyp0werStohnly yourself
    vsonyp0wer_API vsonyp0werID       GetID(const char* str_id_begin, const char* str_id_end);
    vsonyp0wer_API vsonyp0werID       GetID(const void* ptr_id);

    // Widgets: Text
    vsonyp0wer_API void          TextUnformatted(const char* text, const char* text_end = NULL);                // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
    vsonyp0wer_API void          Text(const char* fmt, ...)                                      IM_FMTARGS(1); // simple formatted text
    vsonyp0wer_API void          TextV(const char* fmt, va_list args)                            IM_FMTLIST(1);
    vsonyp0wer_API void          TextColored(const ImVec4& col, const char* fmt, ...)            IM_FMTARGS(2); // shortcut for PushStyleColor(vsonyp0werCol_Text, col); Text(fmt, ...); PopStyleColor();
    vsonyp0wer_API void          TextColoredV(const ImVec4& col, const char* fmt, va_list args)  IM_FMTLIST(2);
    vsonyp0wer_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(vsonyp0werCol_Text, style.Colors[vsonyp0werCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
    vsonyp0wer_API void          TextDisabledV(const char* fmt, va_list args)                    IM_FMTLIST(1);
    vsonyp0wer_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
    vsonyp0wer_API void          TextWrappedV(const char* fmt, va_list args)                     IM_FMTLIST(1);
    vsonyp0wer_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
    vsonyp0wer_API void          LabelTextV(const char* label, const char* fmt, va_list args)    IM_FMTLIST(2);
    vsonyp0wer_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
    vsonyp0wer_API void          BulletTextV(const char* fmt, va_list args)                      IM_FMTLIST(1);

    // Widgets: Main
    // - Most widgets return true when the value has been changed or when pressed/selected
	vsonyp0wer_API bool          SelectedTab(const char* label, const ImVec2& size = ImVec2(0, 0));    // selected tab
	vsonyp0wer_API bool          Tab(const char* label, const ImVec2& size = ImVec2(0, 0));    // tab
	vsonyp0wer_API bool          TabSpacer(const char* label, const ImVec2& size = ImVec2(0, 0));    // TabSpacer
	vsonyp0wer_API bool          TabSpacer2(const char* label, const ImVec2& size = ImVec2(0, 0));    // TabSpacer2
	vsonyp0wer_API bool          GroupBoxTitle(const char* label, const ImVec2& size = ImVec2(0, 0));
	vsonyp0wer_API bool          TabButton(const char* label, const ImVec2& size = ImVec2(0, 0));    // tab button
    vsonyp0wer_API bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));    // button
	vsonyp0wer_API bool          ColorBar(const char* label, const ImVec2& size = ImVec2(0, 0));    // ColorBar
	vsonyp0wer_API bool			QuitButton(const char* label, const ImVec2& size = ImVec2(0, 0));    // quit button
	vsonyp0wer_API bool          NoInputButton(const char* label, const ImVec2& size = ImVec2(0, 0));    // button without input
    vsonyp0wer_API bool          SmallButton(const char* label);                                 // button with FramePadding=(0,0) to easily embed within text
    vsonyp0wer_API bool          InvisibleButton(const char* str_id, const ImVec2& size);        // button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
    vsonyp0wer_API bool          ArrowButton(const char* str_id, vsonyp0werDir dir);                  // square button with an arrow shape
    vsonyp0wer_API void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    vsonyp0wer_API bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    vsonyp0wer_API bool          Checkbox(const char* label, bool* v);
    vsonyp0wer_API bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    vsonyp0wer_API bool          RadioButton(const char* label, bool active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
    vsonyp0wer_API bool          RadioButton(const char* label, int* v, int v_button);           // shortcut to handle the above pattern when value is an integer
    vsonyp0wer_API void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0), const char* overlay = NULL);
    vsonyp0wer_API void          Bullet();                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

    // Widgets: Combo Box
    // - The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
    // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
    vsonyp0wer_API bool          BeginCombo(const char* label, const char* preview_value, vsonyp0werComboFlags flags = 0);
	vsonyp0wer_API bool			MultiCombo(const char* name, const char** displayName, bool* data, int dataSize);
    vsonyp0wer_API void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    vsonyp0wer_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    vsonyp0wer_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    vsonyp0wer_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drags
    // - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
    // - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    // - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
    vsonyp0wer_API bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
    vsonyp0wer_API bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);
    vsonyp0wer_API bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                                       // If v_min >= v_max we have no bound
    vsonyp0wer_API bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    vsonyp0wer_API bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    vsonyp0wer_API bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
    vsonyp0wer_API bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL);
    vsonyp0wer_API bool          DragScalar(const char* label, vsonyp0werDataType data_type, void* v, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);
    vsonyp0wer_API bool          DragScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);

    // Widgets: Sliders
    // - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    vsonyp0wer_API bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    vsonyp0wer_API bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg");
    vsonyp0wer_API bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d");
    vsonyp0wer_API bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d");
    vsonyp0wer_API bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d");
    vsonyp0wer_API bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d");
    vsonyp0wer_API bool          SliderScalar(const char* label, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    vsonyp0wer_API bool          SliderScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
    vsonyp0wer_API bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
    vsonyp0wer_API bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d");
    vsonyp0wer_API bool          VSliderScalar(const char* label, const ImVec2& size, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);

    // Widgets: Input with Keyboard
    // - If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/vsonyp0wer_stdlib.h
    // - Most of the vsonyp0werInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
    vsonyp0wer_API bool          InputText(const char* label, char* buf, size_t buf_size, vsonyp0werInputTextFlags flags = 0, vsonyp0werInputTextCallback callback = NULL, void* user_data = NULL);
    vsonyp0wer_API bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), vsonyp0werInputTextFlags flags = 0, vsonyp0werInputTextCallback callback = NULL, void* user_data = NULL);
    vsonyp0wer_API bool          InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, vsonyp0werInputTextFlags flags = 0, vsonyp0werInputTextCallback callback = NULL, void* user_data = NULL);
    vsonyp0wer_API bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputInt2(const char* label, int v[2], vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputInt3(const char* label, int v[3], vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputInt4(const char* label, int v[4], vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputScalar(const char* label, vsonyp0werDataType data_type, void* v, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool          InputScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, vsonyp0werInputTextFlags flags = 0);

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
    // - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
    vsonyp0wer_API bool          ColorEdit3(const char* label, float col[3], vsonyp0werColorEditFlags flags = 0);
    vsonyp0wer_API bool          ColorEdit4(const char* label, float col[4], vsonyp0werColorEditFlags flags = 0);
    vsonyp0wer_API bool          ColorPicker3(const char* label, float col[3], vsonyp0werColorEditFlags flags = 0);
    vsonyp0wer_API bool          ColorPicker4(const char* label, float col[4], vsonyp0werColorEditFlags flags = 0, const float* ref_col = NULL);
    vsonyp0wer_API bool          ColorButton(const char* desc_id, const ImVec4& col, vsonyp0werColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));  // display a colored square/button, hover for details, return true when pressed.
    vsonyp0wer_API void          SetColorEditOptions(vsonyp0werColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    // - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
    vsonyp0wer_API bool          TreeNode(const char* label);
    vsonyp0wer_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);   // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
    vsonyp0wer_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);   // "
    vsonyp0wer_API bool          TreeNodeV(const char* str_id, const char* fmt, va_list args) IM_FMTLIST(2);
    vsonyp0wer_API bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args) IM_FMTLIST(2);
    vsonyp0wer_API bool          TreeNodeEx(const char* label, vsonyp0werTreeNodeFlags flags = 0);
    vsonyp0wer_API bool          TreeNodeEx(const char* str_id, vsonyp0werTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    vsonyp0wer_API bool          TreeNodeEx(const void* ptr_id, vsonyp0werTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    vsonyp0wer_API bool          TreeNodeExV(const char* str_id, vsonyp0werTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    vsonyp0wer_API bool          TreeNodeExV(const void* ptr_id, vsonyp0werTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    vsonyp0wer_API void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
    vsonyp0wer_API void          TreePush(const void* ptr_id = NULL);                                // "
    vsonyp0wer_API void          TreePop();                                                          // ~ Unindent()+PopId()
    vsonyp0wer_API void          TreeAdvanceToLabelPos();                                            // advance cursor x position by GetTreeNodeToLabelSpacing()
    vsonyp0wer_API float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    vsonyp0wer_API void          SetNextTreeNodeOpen(bool is_open, vsonyp0werCond cond = 0);              // set next TreeNode/CollapsingHeader open state.
    vsonyp0wer_API bool          CollapsingHeader(const char* label, vsonyp0werTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    vsonyp0wer_API bool          CollapsingHeader(const char* label, bool* p_open, vsonyp0werTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header

    // Widgets: Selectables
    // - A selectable highlights when hovered, and can display another color when selected.
    // - Neighbors selectable extend their highlight bounds in order to leave no gap between them.
    vsonyp0wer_API bool          Selectable(const char* label, bool selected = false, vsonyp0werSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));  // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    vsonyp0wer_API bool          Selectable(const char* label, bool* p_selected, vsonyp0werSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

    // Widgets: List Boxes
    // - FIXME: To be consistent with all the newer API, ListBoxHeader/ListBoxFooter should in reality be called BeginListBox/EndListBox. Will rename them.
    vsonyp0wer_API bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    vsonyp0wer_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
    vsonyp0wer_API bool          ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0)); // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
    vsonyp0wer_API bool          ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // "
    vsonyp0wer_API void          ListBoxFooter();                                                    // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

    // Widgets: Data Plotting
    vsonyp0wer_API void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    vsonyp0wer_API void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    vsonyp0wer_API void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    vsonyp0wer_API void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

    // Widgets: Value() Helpers.
    // - Those are merely shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the vsonyp0wer namespace)
    vsonyp0wer_API void          Value(const char* prefix, bool b);
    vsonyp0wer_API void          Value(const char* prefix, int v);
    vsonyp0wer_API void          Value(const char* prefix, unsigned int v);
    vsonyp0wer_API void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Widgets: Menus
    vsonyp0wer_API bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
    vsonyp0wer_API void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
    vsonyp0wer_API bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires vsonyp0werWindowFlags_MenuBar flag set on parent window).
    vsonyp0wer_API void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
    vsonyp0wer_API bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
    vsonyp0wer_API void          EndMenu();                                                          // only call EndMenu() if BeginMenu() returns true!
    vsonyp0wer_API bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by vsonyp0wer at the moment
    vsonyp0wer_API bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Tooltips
    vsonyp0wer_API void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
    vsonyp0wer_API void          EndTooltip();
    vsonyp0wer_API void          SetTooltip(const char* fmt, ...) IM_FMTARGS(1);                     // set a text-only tooltip, typically use with vsonyp0wer::IsItemHovered(). override any previous call to SetTooltip().
    vsonyp0wer_API void          SetTooltipV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Popups, Modals
    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by vsonyp0wer instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
    vsonyp0wer_API void          OpenPopup(const char* str_id);                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
    vsonyp0wer_API bool          BeginPopup(const char* str_id, vsonyp0werWindowFlags flags = 0);                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
    vsonyp0wer_API bool          BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked on last item. if you can pass a NULL str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
    vsonyp0wer_API bool          BeginPopupContextWindow(const char* str_id = NULL, int mouse_button = 1, bool also_over_items = true);  // helper to open and begin popup when clicked on current window.
    vsonyp0wer_API bool          BeginPopupContextVoid(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked in void (where there are no vsonyp0wer windows).
    vsonyp0wer_API bool          BeginPopupModal(const char* name, bool* p_open = NULL, vsonyp0werWindowFlags flags = 0);                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
    vsonyp0wer_API void          EndPopup();                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
    vsonyp0wer_API bool          OpenPopupOnItemClick(const char* str_id = NULL, int mouse_button = 1);                                  // helper to open popup when clicked on last item (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors). return true when just opened.
    vsonyp0wer_API bool          IsPopupOpen(const char* str_id);                                    // return true if the popup is open at the current begin-ed level of the popup stack.
    vsonyp0wer_API void          CloseCurrentPopup();                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

    // Columns
    // - You can also use SameLine(pos_x) to mimic simplified columns.
    // - The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear vsonyp0wer at the moment!)
    vsonyp0wer_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
    vsonyp0wer_API void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
    vsonyp0wer_API int           GetColumnIndex();                                                   // get current column index
    vsonyp0wer_API float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
    vsonyp0wer_API void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
    vsonyp0wer_API float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
    vsonyp0wer_API void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
    vsonyp0wer_API int           GetColumnsCount();

    // Tab Bars, Tabs
    // [BETA API] API may evolve!
    vsonyp0wer_API bool          BeginTabBar(const char* str_id, vsonyp0werTabBarFlags flags = 0);        // create and append into a TabBar
    vsonyp0wer_API void          EndTabBar();                                                        // only call EndTabBar() if BeginTabBar() returns true!
    vsonyp0wer_API bool          BeginTabItem(const char* label, bool* p_open = NULL, vsonyp0werTabItemFlags flags = 0);// create a Tab. Returns true if the Tab is selected.
    vsonyp0wer_API void          EndTabItem();                                                       // only call EndTabItem() if BeginTabItem() returns true!
    vsonyp0wer_API void          SetTabItemClosed(const char* tab_or_docked_window_label);           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

    // Logging/Capture
    // - All text output from the interface can be captured into tty/file/clipboard. By default, tree nodes are automatically opened during logging.
    vsonyp0wer_API void          LogToTTY(int auto_open_depth = -1);                                 // start logging to tty (stdout)
    vsonyp0wer_API void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);   // start logging to file
    vsonyp0wer_API void          LogToClipboard(int auto_open_depth = -1);                           // start logging to OS clipboard
    vsonyp0wer_API void          LogFinish();                                                        // stop logging (close file, etc.)
    vsonyp0wer_API void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
    vsonyp0wer_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)

    // Drag and Drop
    // [BETA API] API may evolve!
    vsonyp0wer_API bool          BeginDragDropSource(vsonyp0werDragDropFlags flags = 0);                                      // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
    vsonyp0wer_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, vsonyp0werCond cond = 0);  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear vsonyp0wer internal types. Data is copied and held by vsonyp0wer.
    vsonyp0wer_API void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
    vsonyp0wer_API bool                  BeginDragDropTarget();                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
    vsonyp0wer_API const vsonyp0werPayload* AcceptDragDropPayload(const char* type, vsonyp0werDragDropFlags flags = 0);          // accept contents of a given type. If vsonyp0werDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
    vsonyp0wer_API void                  EndDragDropTarget();                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
    vsonyp0wer_API const vsonyp0werPayload* GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use vsonyp0werPayload::IsDataType() to test for the payload type.

    // Clipping
    vsonyp0wer_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    vsonyp0wer_API void          PopClipRect();

    // Focus, Activation
    // - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
    vsonyp0wer_API void          SetItemDefaultFocus();                                              // make last item the default focused item of a window.
    vsonyp0wer_API void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

    // Item/Widgets Utilities
    // - Most of the functions are referring to the last/previous item we submitted.
    // - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
    vsonyp0wer_API bool          IsItemHovered(vsonyp0werHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See vsonyp0werHoveredFlags for more options.
    vsonyp0wer_API bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
    vsonyp0wer_API bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
    vsonyp0wer_API bool          IsItemClicked(int mouse_button = 0);                                // is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
    vsonyp0wer_API bool          IsItemVisible();                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
    vsonyp0wer_API bool          IsItemEdited();                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
    vsonyp0wer_API bool          IsItemActivated();                                                  // was the last item just made active (item was previously inactive).
    vsonyp0wer_API bool          IsItemDeactivated();                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
    vsonyp0wer_API bool          IsItemDeactivatedAfterEdit();                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
    vsonyp0wer_API bool          IsAnyItemHovered();                                                 // is any item hovered?
    vsonyp0wer_API bool          IsAnyItemActive();                                                  // is any item active?
    vsonyp0wer_API bool          IsAnyItemFocused();                                                 // is any item focused?
    vsonyp0wer_API ImVec2        GetItemRectMin();                                                   // get upper-left bounding rectangle of the last item (screen space)
    vsonyp0wer_API ImVec2        GetItemRectMax();                                                   // get lower-right bounding rectangle of the last item (screen space)
    vsonyp0wer_API ImVec2        GetItemRectSize();                                                  // get size of last item
    vsonyp0wer_API void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

    // Miscellaneous Utilities
    vsonyp0wer_API bool          IsRectVisible(const ImVec2& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
    vsonyp0wer_API bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
    vsonyp0wer_API double        GetTime();                                                          // get global vsonyp0wer time. incremented by io.DeltaTime every frame.
    vsonyp0wer_API int           GetFrameCount();                                                    // get global vsonyp0wer frame count. incremented by 1 every frame.
    vsonyp0wer_API ImDrawList* GetBackgroundDrawList();                                            // this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear vsonyp0wer contents.
    vsonyp0wer_API ImDrawList* GetForegroundDrawList();                                            // this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear vsonyp0wer contents.
    vsonyp0wer_API ImDrawListSharedData* GetDrawListSharedData();                                    // you may use this when creating your own ImDrawList instances.
    vsonyp0wer_API const char* GetStyleColorName(vsonyp0werCol idx);                                    // get a string corresponding to the enum value (for display, saving, etc.).
    vsonyp0wer_API void          SetStateStohnly(vsonyp0werStohnly* stohnly);                             // replace current window stohnly with our own (if you want to manipulate it yourself, typically clear subsection of it)
    vsonyp0wer_API vsonyp0werStohnly* GetStateStohnly();
    vsonyp0wer_API ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
    vsonyp0wer_API void          CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);    // calculate coarse clipping for large list of evenly sized items. Prefer using the vsonyp0werListClipper higher-level helper if you can.
    vsonyp0wer_API bool          BeginChildFrame(vsonyp0werID id, const ImVec2& size, vsonyp0werWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
    vsonyp0wer_API void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

    // Color Utilities
    vsonyp0wer_API ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    vsonyp0wer_API ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
    vsonyp0wer_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    vsonyp0wer_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs Utilities
    vsonyp0wer_API int           GetKeyIndex(vsonyp0werKey vsonyp0wer_key);                                    // map vsonyp0werKey_* values into user's key index. == io.KeyMap[key]
    vsonyp0wer_API bool          IsKeyDown(int user_key_index);                                      // is key being held. == io.KeysDown[user_key_index]. note that vsonyp0wer doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]!
    vsonyp0wer_API bool          IsKeyPressed(int user_key_index, bool repeat = true);               // was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    vsonyp0wer_API bool          IsKeyReleased(int user_key_index);                                  // was key released (went from Down to !Down)..
    vsonyp0wer_API int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    vsonyp0wer_API bool          IsMouseDown(int button);                                            // is mouse button held (0=left, 1=right, 2=middle)
    vsonyp0wer_API bool          IsAnyMouseDown();                                                   // is any mouse button held
    vsonyp0wer_API bool          IsMouseClicked(int button, bool repeat = false);                    // did mouse button clicked (went from !Down to Down) (0=left, 1=right, 2=middle)
    vsonyp0wer_API bool          IsMouseDoubleClicked(int button);                                   // did mouse button double-clicked. a double-click returns false in IsMouseClicked(). uses io.MouseDoubleClickTime.
    vsonyp0wer_API bool          IsMouseReleased(int button);                                        // did mouse button released (went from Down to !Down)
    vsonyp0wer_API bool          IsMouseDragging(int button = 0, float lock_threshold = -1.0f);      // is mouse dragging. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    vsonyp0wer_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);  // is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block.
    vsonyp0wer_API bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);                    // by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse
    vsonyp0wer_API ImVec2        GetMousePos();                                                      // shortcut to vsonyp0wer::GetIO().MousePos provided by user, to be consistent with other calls
    vsonyp0wer_API ImVec2        GetMousePosOnOpeningCurrentPopup();                                 // retrieve backup of mouse position at the time of opening popup we have BeginPopup() into
    vsonyp0wer_API ImVec2        GetMouseDragDelta(int button = 0, float lock_threshold = -1.0f);    // return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once. If lock_threshold < -1.0f uses io.MouseDraggingThreshold.
    vsonyp0wer_API void          ResetMouseDragDelta(int button = 0);                                //
    vsonyp0wer_API vsonyp0werMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in vsonyp0wer::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor vsonyp0wer will render those for you
    vsonyp0wer_API void          SetMouseCursor(vsonyp0werMouseCursor type);                              // set desired cursor type
    vsonyp0wer_API void          CaptureKeyboardFromApp(bool want_capture_keyboard_value = true);    // attention: misleading name! manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application to handle). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard_value"; after the next NewFrame() call.
    vsonyp0wer_API void          CaptureMouseFromApp(bool want_capture_mouse_value = true);          // attention: misleading name! manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application to handle). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse_value;" after the next NewFrame() call.

    // Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
    vsonyp0wer_API const char* GetClipboardText();
    vsonyp0wer_API void          SetClipboardText(const char* text);

    // Settings/.Ini Utilities
    // - The disk functions are automatically called if io.IniFilename != NULL (default is "vsonyp0wer.ini").
    // - Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.
    vsonyp0wer_API void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
    vsonyp0wer_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size = 0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
    vsonyp0wer_API void          SaveIniSettingsToDisk(const char* ini_filename);                    // this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext).
    vsonyp0wer_API const char* SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Memory Allocators
    // - All those functions are not reliant on the current context.
    // - If you reload the contents of vsonyp0wer.cpp at runtime, you may need to call SetCurrentContext() + SetAllocatorFunctions() again because we use global stohnly for those.
    vsonyp0wer_API void          SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data = NULL);
    vsonyp0wer_API void* MemAlloc(size_t size);
    vsonyp0wer_API void          MemFree(void* ptr);

} // namespace vsonyp0wer

//-----------------------------------------------------------------------------
// Flags & Enumerations
//-----------------------------------------------------------------------------

// Flags for vsonyp0wer::Begin()
enum vsonyp0werWindowFlags_
{
    vsonyp0werWindowFlags_None = 0,
    vsonyp0werWindowFlags_NoTitleBar = 1 << 0,   // Disable title-bar
    vsonyp0werWindowFlags_NoResize = 1 << 1,   // Disable user resizing with the lower-right grip
    vsonyp0werWindowFlags_NoMove = 1 << 2,   // Disable user moving the window
    vsonyp0werWindowFlags_NoScrollbar = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    vsonyp0werWindowFlags_NoScrollWithMouse = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    vsonyp0werWindowFlags_NoCollapse = 1 << 5,   // Disable user collapsing window by double-clicking on it
    vsonyp0werWindowFlags_AlwaysAutoResize = 1 << 6,   // Resize every window to its content every frame
    vsonyp0werWindowFlags_NoBackground = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    vsonyp0werWindowFlags_NoSavedSettings = 1 << 8,   // Never load/save settings in .ini file
    vsonyp0werWindowFlags_NoMouseInputs = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    vsonyp0werWindowFlags_MenuBar = 1 << 10,  // Has a menu-bar
    vsonyp0werWindowFlags_HorizontalScrollbar = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in vsonyp0wer_demo in the "Horizontal Scrolling" section.
    vsonyp0werWindowFlags_NoFocusOnAppearing = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    vsonyp0werWindowFlags_NoBringToFrontOnFocus = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    vsonyp0werWindowFlags_AlwaysVerticalScrollbar = 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    vsonyp0werWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    vsonyp0werWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    vsonyp0werWindowFlags_NoNavInputs = 1 << 18,  // No gamepad/keyboard navigation within the window
    vsonyp0werWindowFlags_NoNavFocus = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    vsonyp0werWindowFlags_UnsavedDocument = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
    vsonyp0werWindowFlags_NoNav = vsonyp0werWindowFlags_NoNavInputs | vsonyp0werWindowFlags_NoNavFocus,
    vsonyp0werWindowFlags_NoDecoration = vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoScrollbar | vsonyp0werWindowFlags_NoCollapse,
    vsonyp0werWindowFlags_NoInputs = vsonyp0werWindowFlags_NoMouseInputs | vsonyp0werWindowFlags_NoNavInputs | vsonyp0werWindowFlags_NoNavFocus,

    // [Internal]
    vsonyp0werWindowFlags_NavFlattened = 1 << 23,  // [BETA] Allow gamepad/keyboard navigation to cross over parent border to this child (only use on child that have no scrolling!)
    vsonyp0werWindowFlags_ChildWindow = 1 << 24,  // Don't use! For internal use by BeginChild()
    vsonyp0werWindowFlags_Tooltip = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    vsonyp0werWindowFlags_Popup = 1 << 26,  // Don't use! For internal use by BeginPopup()
    vsonyp0werWindowFlags_Modal = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    vsonyp0werWindowFlags_ChildMenu = 1 << 28   // Don't use! For internal use by BeginMenu()

    // [Obsolete]
    //vsonyp0werWindowFlags_ShowBorders          = 1 << 7,   // --> Set style.FrameBorderSize=1.0f / style.WindowBorderSize=1.0f to enable borders around windows and items
    //vsonyp0werWindowFlags_ResizeFromAnySide    = 1 << 17,  // --> Set io.ConfigWindowsResizeFromEdges and make sure mouse cursors are supported by back-end (io.BackendFlags & vsonyp0werBackendFlags_HasMouseCursors)
};

// Flags for vsonyp0wer::InputText()
enum vsonyp0werInputTextFlags_
{
    vsonyp0werInputTextFlags_None = 0,
    vsonyp0werInputTextFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-*/
    vsonyp0werInputTextFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    vsonyp0werInputTextFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
    vsonyp0werInputTextFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
    vsonyp0werInputTextFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
    vsonyp0werInputTextFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
    vsonyp0werInputTextFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
    vsonyp0werInputTextFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
    vsonyp0werInputTextFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
    vsonyp0werInputTextFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
    vsonyp0werInputTextFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    vsonyp0werInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    vsonyp0werInputTextFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
    vsonyp0werInputTextFlags_AlwaysInsertMode = 1 << 13,  // Insert mode
    vsonyp0werInputTextFlags_ReadOnly = 1 << 14,  // Read-only mode
    vsonyp0werInputTextFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
    vsonyp0werInputTextFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
    vsonyp0werInputTextFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
    vsonyp0werInputTextFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/vsonyp0wer_stdlib.h for an example of using this)
    // [Internal]
    vsonyp0werInputTextFlags_Multiline = 1 << 20   // For internal use by InputTextMultiline()
};

// Flags for vsonyp0wer::TreeNodeEx(), vsonyp0wer::CollapsingHeader*()
enum vsonyp0werTreeNodeFlags_
{
    vsonyp0werTreeNodeFlags_None = 0,
    vsonyp0werTreeNodeFlags_Selected = 1 << 0,   // Draw as selected
    vsonyp0werTreeNodeFlags_Framed = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
    vsonyp0werTreeNodeFlags_AllowItemOverlap = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
    vsonyp0werTreeNodeFlags_NoTreePushOnOpen = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    vsonyp0werTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    vsonyp0werTreeNodeFlags_DefaultOpen = 1 << 5,   // Default node to be open
    vsonyp0werTreeNodeFlags_OpenOnDoubleClick = 1 << 6,   // Need double-click to open node
    vsonyp0werTreeNodeFlags_OpenOnArrow = 1 << 7,   // Only open when clicking on the arrow part. If vsonyp0werTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    vsonyp0werTreeNodeFlags_Leaf = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
    vsonyp0werTreeNodeFlags_Bullet = 1 << 9,   // Display a bullet instead of arrow
    vsonyp0werTreeNodeFlags_FramePadding = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    //vsonyp0werTreeNodeFlags_SpanAllAvailWidth  = 1 << 11,  // FIXME: TODO: Extend hit box horizontally even if not framed
    //vsonyp0werTreeNodeFlags_NoScrollOnOpen     = 1 << 12,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
    vsonyp0werTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
    vsonyp0werTreeNodeFlags_CollapsingHeader = vsonyp0werTreeNodeFlags_Framed | vsonyp0werTreeNodeFlags_NoTreePushOnOpen | vsonyp0werTreeNodeFlags_NoAutoOpenOnLog

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werTreeNodeFlags_AllowOverlapMode = vsonyp0werTreeNodeFlags_AllowItemOverlap
#endif
};

// Flags for vsonyp0wer::Selectable()
enum vsonyp0werSelectableFlags_
{
    vsonyp0werSelectableFlags_None = 0,
    vsonyp0werSelectableFlags_DontClosePopups = 1 << 0,   // Clicking this don't close parent popup window
    vsonyp0werSelectableFlags_SpanAllColumns = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    vsonyp0werSelectableFlags_AllowDoubleClick = 1 << 2,   // Generate press events on double clicks too
    vsonyp0werSelectableFlags_Disabled = 1 << 3    // Cannot be selected, display greyed out text
};

// Flags for vsonyp0wer::BeginCombo()
enum vsonyp0werComboFlags_
{
    vsonyp0werComboFlags_None = 0,
    vsonyp0werComboFlags_PopupAlignLeft = 1 << 0,   // Align the popup toward the left by default
    vsonyp0werComboFlags_HeightSmall = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
    vsonyp0werComboFlags_HeightRegular = 1 << 2,   // Max ~8 items visible (default)
    vsonyp0werComboFlags_HeightLarge = 1 << 3,   // Max ~20 items visible
    vsonyp0werComboFlags_HeightLargest = 1 << 4,   // As many fitting items as possible
    vsonyp0werComboFlags_NoArrowButton = 1 << 5,   // Display on the preview box without the square arrow button
    vsonyp0werComboFlags_NoPreview = 1 << 6,   // Display only a square arrow button
    vsonyp0werComboFlags_HeightMask_ = vsonyp0werComboFlags_HeightSmall | vsonyp0werComboFlags_HeightRegular | vsonyp0werComboFlags_HeightLarge | vsonyp0werComboFlags_HeightLargest
};

// Flags for vsonyp0wer::BeginTabBar()
enum vsonyp0werTabBarFlags_
{
    vsonyp0werTabBarFlags_None = 0,
    vsonyp0werTabBarFlags_Reorderable = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
    vsonyp0werTabBarFlags_AutoSelectNewTabs = 1 << 1,   // Automatically select new tabs when they appear
    vsonyp0werTabBarFlags_TabListPopupButton = 1 << 2,
    vsonyp0werTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    vsonyp0werTabBarFlags_NoTabListScrollingButtons = 1 << 4,
    vsonyp0werTabBarFlags_NoTooltip = 1 << 5,   // Disable tooltips when hovering a tab
    vsonyp0werTabBarFlags_FittingPolicyResizeDown = 1 << 6,   // Resize tabs when they don't fit
    vsonyp0werTabBarFlags_FittingPolicyScroll = 1 << 7,   // Add scroll buttons when tabs don't fit
    vsonyp0werTabBarFlags_FittingPolicyMask_ = vsonyp0werTabBarFlags_FittingPolicyResizeDown | vsonyp0werTabBarFlags_FittingPolicyScroll,
    vsonyp0werTabBarFlags_FittingPolicyDefault_ = vsonyp0werTabBarFlags_FittingPolicyResizeDown
};

// Flags for vsonyp0wer::BeginTabItem()
enum vsonyp0werTabItemFlags_
{
    vsonyp0werTabItemFlags_None = 0,
    vsonyp0werTabItemFlags_UnsavedDocument = 1 << 0,   // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
    vsonyp0werTabItemFlags_SetSelected = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
    vsonyp0werTabItemFlags_NoCloseWithMiddleMouseButton = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    vsonyp0werTabItemFlags_NoPushId = 1 << 3    // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
};

// Flags for vsonyp0wer::IsWindowFocused()
enum vsonyp0werFocusedFlags_
{
    vsonyp0werFocusedFlags_None = 0,
    vsonyp0werFocusedFlags_ChildWindows = 1 << 0,   // IsWindowFocused(): Return true if any children of the window is focused
    vsonyp0werFocusedFlags_RootWindow = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
    vsonyp0werFocusedFlags_AnyWindow = 1 << 2,   // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use vsonyp0wer::GetIO().WantCaptureMouse instead.
    vsonyp0werFocusedFlags_RootAndChildWindows = vsonyp0werFocusedFlags_RootWindow | vsonyp0werFocusedFlags_ChildWindows
};

// Flags for vsonyp0wer::IsItemHovered(), vsonyp0wer::IsWindowHovered()
// Note: if you are trying to check whether your mouse should be dispatched to vsonyp0wer or to your app, you should use the 'io.WantCaptureMouse' boolean for that. Please read the FAQ!
// Note: windows with the vsonyp0werWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
enum vsonyp0werHoveredFlags_
{
    vsonyp0werHoveredFlags_None = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
    vsonyp0werHoveredFlags_ChildWindows = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
    vsonyp0werHoveredFlags_RootWindow = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
    vsonyp0werHoveredFlags_AnyWindow = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
    vsonyp0werHoveredFlags_AllowWhenBlockedByPopup = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
    //vsonyp0werHoveredFlags_AllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
    vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
    vsonyp0werHoveredFlags_AllowWhenOverlapped = 1 << 6,   // Return true even if the position is overlapped by another window
    vsonyp0werHoveredFlags_AllowWhenDisabled = 1 << 7,   // Return true even if the item is disabled
    vsonyp0werHoveredFlags_RectOnly = vsonyp0werHoveredFlags_AllowWhenBlockedByPopup | vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem | vsonyp0werHoveredFlags_AllowWhenOverlapped,
    vsonyp0werHoveredFlags_RootAndChildWindows = vsonyp0werHoveredFlags_RootWindow | vsonyp0werHoveredFlags_ChildWindows
};

// Flags for vsonyp0wer::BeginDragDropSource(), vsonyp0wer::AcceptDragDropPayload()
enum vsonyp0werDragDropFlags_
{
    vsonyp0werDragDropFlags_None = 0,
    // BeginDragDropSource() flags
    vsonyp0werDragDropFlags_SourceNoPreviewTooltip = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
    vsonyp0werDragDropFlags_SourceNoDisableHover = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
    vsonyp0werDragDropFlags_SourceNoHoldToOpenOthers = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    vsonyp0werDragDropFlags_SourceAllowNullID = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear vsonyp0wer ecosystem and so we made it explicit.
    vsonyp0werDragDropFlags_SourceExtern = 1 << 4,   // External source (from outside of vsonyp0wer), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    vsonyp0werDragDropFlags_SourceAutoExpirePayload = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    // AcceptDragDropPayload() flags
    vsonyp0werDragDropFlags_AcceptBeforeDelivery = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    vsonyp0werDragDropFlags_AcceptNoDrawDefaultRect = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
    vsonyp0werDragDropFlags_AcceptNoPreviewTooltip = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
    vsonyp0werDragDropFlags_AcceptPeekOnly = vsonyp0werDragDropFlags_AcceptBeforeDelivery | vsonyp0werDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.
};

// Standard Drag and Drop payload types. You can define you own payload types using short strings. Types starting with '_' are defined by Dear vsonyp0wer.
#define vsonyp0wer_PAYLOAD_TYPE_COLOR_3F     "_COL3F"    // float[3]: Standard type for colors, without alpha. User code may use this type.
#define vsonyp0wer_PAYLOAD_TYPE_COLOR_4F     "_COL4F"    // float[4]: Standard type for colors. User code may use this type.

// A primary data type
enum vsonyp0werDataType_
{
    vsonyp0werDataType_S8,       // char
    vsonyp0werDataType_U8,       // unsigned char
    vsonyp0werDataType_S16,      // short
    vsonyp0werDataType_U16,      // unsigned short
    vsonyp0werDataType_S32,      // int
    vsonyp0werDataType_U32,      // unsigned int
    vsonyp0werDataType_S64,      // long long / __int64
    vsonyp0werDataType_U64,      // unsigned long long / unsigned __int64
    vsonyp0werDataType_Float,    // float
    vsonyp0werDataType_Double,   // double
    vsonyp0werDataType_COUNT
};

// A cardinal direction
enum vsonyp0werDir_
{
    vsonyp0werDir_None = -1,
    vsonyp0werDir_Left = 0,
    vsonyp0werDir_Right = 1,
    vsonyp0werDir_Up = 2,
    vsonyp0werDir_Down = 3,
    vsonyp0werDir_COUNT
};

// User fill vsonyp0werIO.KeyMap[] array with indices into the vsonyp0werIO.KeysDown[512] array
enum vsonyp0werKey_
{
    vsonyp0werKey_Tab,
    vsonyp0werKey_LeftArrow,
    vsonyp0werKey_RightArrow,
    vsonyp0werKey_UpArrow,
    vsonyp0werKey_DownArrow,
    vsonyp0werKey_PageUp,
    vsonyp0werKey_PageDown,
    vsonyp0werKey_Home,
    vsonyp0werKey_End,
    vsonyp0werKey_Insert,
    vsonyp0werKey_Delete,
    vsonyp0werKey_Backspace,
    vsonyp0werKey_Space,
    vsonyp0werKey_Enter,
    vsonyp0werKey_Escape,
    vsonyp0werKey_A,         // for text edit CTRL+A: select all
    vsonyp0werKey_C,         // for text edit CTRL+C: copy
    vsonyp0werKey_V,         // for text edit CTRL+V: paste
    vsonyp0werKey_X,         // for text edit CTRL+X: cut
    vsonyp0werKey_Y,         // for text edit CTRL+Y: redo
    vsonyp0werKey_Z,         // for text edit CTRL+Z: undo
    vsonyp0werKey_COUNT
};

// Gamepad/Keyboard directional navigation
// Keyboard: Set io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableKeyboard to enable. NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
// Gamepad:  Set io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableGamepad to enable. Back-end: set vsonyp0werBackendFlags_HasGamepad and fill the io.NavInputs[] fields before calling NewFrame(). Note that io.NavInputs[] is cleared by EndFrame().
// Read instructions in vsonyp0wer.cpp for more details. Download PNG/PSD at http://goo.gl/9LgVZW.
enum vsonyp0werNavInput_
{
    // Gamepad Mapping
    vsonyp0werNavInput_Activate,      // activate / open / toggle / tweak value       // e.g. Cross  (PS4), A (Xbox), A (Switch), Space (Keyboard)
    vsonyp0werNavInput_Cancel,        // cancel / close / exit                        // e.g. Circle (PS4), B (Xbox), B (Switch), Escape (Keyboard)
    vsonyp0werNavInput_Input,         // text input / on-screen keyboard              // e.g. Triang.(PS4), Y (Xbox), X (Switch), Return (Keyboard)
    vsonyp0werNavInput_Menu,          // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4), X (Xbox), Y (Switch), Alt (Keyboard)
    vsonyp0werNavInput_DpadLeft,      // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
    vsonyp0werNavInput_DpadRight,     //
    vsonyp0werNavInput_DpadUp,        //
    vsonyp0werNavInput_DpadDown,      //
    vsonyp0werNavInput_LStickLeft,    // scroll / move window (w/ PadMenu)            // e.g. Left Analog Stick Left/Right/Up/Down
    vsonyp0werNavInput_LStickRight,   //
    vsonyp0werNavInput_LStickUp,      //
    vsonyp0werNavInput_LStickDown,    //
    vsonyp0werNavInput_FocusPrev,     // next window (w/ PadMenu)                     // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    vsonyp0werNavInput_FocusNext,     // prev window (w/ PadMenu)                     // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
    vsonyp0werNavInput_TweakSlow,     // slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    vsonyp0werNavInput_TweakFast,     // faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)

    // [Internal] Don't use directly! This is used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
    // Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from io.KeysDown[] instead of io.NavInputs[].
    vsonyp0werNavInput_KeyMenu_,      // toggle menu                                  // = io.KeyAlt
    vsonyp0werNavInput_KeyTab_,       // tab                                          // = Tab key
    vsonyp0werNavInput_KeyLeft_,      // move left                                    // = Arrow keys
    vsonyp0werNavInput_KeyRight_,     // move right
    vsonyp0werNavInput_KeyUp_,        // move up
    vsonyp0werNavInput_KeyDown_,      // move down
    vsonyp0werNavInput_COUNT,
    vsonyp0werNavInput_InternalStart_ = vsonyp0werNavInput_KeyMenu_
};

// Configuration flags stored in io.ConfigFlags. Set by user/application.
enum vsonyp0werConfigFlags_
{
    vsonyp0werConfigFlags_None = 0,
    vsonyp0werConfigFlags_NavEnableKeyboard = 1 << 0,   // Master keyboard navigation enable flag. NewFrame() will automatically fill io.NavInputs[] based on io.KeysDown[].
    vsonyp0werConfigFlags_NavEnableGamepad = 1 << 1,   // Master gamepad navigation enable flag. This is mostly to instruct your vsonyp0wer back-end to fill io.NavInputs[]. Back-end also needs to set vsonyp0werBackendFlags_HasGamepad.
    vsonyp0werConfigFlags_NavEnableSetMousePos = 1 << 2,   // Instruct navigation to move the mouse cursor. May be useful on TV/console systems where moving a virtual mouse is awkward. Will update io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor io.WantSetMousePos requests in your binding, otherwise vsonyp0wer will react as if the mouse is jumping around back and forth.
    vsonyp0werConfigFlags_NavNoCaptureKeyboard = 1 << 3,   // Instruct navigation to not set the io.WantCaptureKeyboard flag when io.NavActive is set.
    vsonyp0werConfigFlags_NoMouse = 1 << 4,   // Instruct vsonyp0wer to clear mouse position/buttons in NewFrame(). This allows ignoring the mouse information set by the back-end.
    vsonyp0werConfigFlags_NoMouseCursorChange = 1 << 5,   // Instruct back-end to not alter mouse cursor shape and visibility. Use if the back-end cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from vsonyp0wer by reading GetMouseCursor() yourself instead.

    // User stohnly (to allow your back-end/engine to communicate to code that may be shared between multiple projects. Those flags are not used by core vsonyp0wer)
    vsonyp0werConfigFlags_IsSRGB = 1 << 20,  // Application is SRGB-aware.
    vsonyp0werConfigFlags_IsTouchScreen = 1 << 21   // Application is using a touch screen instead of a mouse.
};

// Back-end capabilities flags stored in io.BackendFlags. Set by vsonyp0wer_impl_xxx or custom back-end.
enum vsonyp0werBackendFlags_
{
    vsonyp0werBackendFlags_None = 0,
    vsonyp0werBackendFlags_HasGamepad = 1 << 0,   // Back-end supports gamepad and currently has one connected.
    vsonyp0werBackendFlags_HasMouseCursors = 1 << 1,   // Back-end supports honoring GetMouseCursor() value to change the OS cursor shape.
    vsonyp0werBackendFlags_HasSetMousePos = 1 << 2    // Back-end supports io.WantSetMousePos requests to reposition the OS mouse position (only used if vsonyp0werConfigFlags_NavEnableSetMousePos is set).
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum vsonyp0werCol_
{
	vsonyp0werCol_MenuTheme,
	vsonyp0werCol_Text,
	vsonyp0werCol_TextShadow,
	vsonyp0werCol_TextDisabled,
	vsonyp0werCol_WindowBg,              // Background of normal windows
	vsonyp0werCol_ChildBg,               // Background of child windows
	vsonyp0werCol_PopupBg,               // Background of popups, menus, tooltips windows
	vsonyp0werCol_Border,
	vsonyp0werCol_BorderShadow,
	vsonyp0werCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
	vsonyp0werCol_FrameBgHovered,
	vsonyp0werCol_FrameBgActive,
	vsonyp0werCol_TitleBg,
	vsonyp0werCol_TitleBgActive,
	vsonyp0werCol_TitleBgCollapsed,
	vsonyp0werCol_MenuBarBg,
	vsonyp0werCol_ScrollbarBg,
	vsonyp0werCol_ScrollbarGrab,
	vsonyp0werCol_ScrollbarGrabHovered,
	vsonyp0werCol_ScrollbarGrabActive,
	vsonyp0werCol_CheckMark,
	vsonyp0werCol_SliderGrab,
	vsonyp0werCol_SliderGrabActive,
	vsonyp0werCol_TitleButton,
	vsonyp0werCol_Button,
	vsonyp0werCol_ButtonHovered,
	vsonyp0werCol_ButtonActive,
	vsonyp0werCol_Header,
	vsonyp0werCol_HeaderHovered,
	vsonyp0werCol_HeaderActive,
	vsonyp0werCol_Separator,
	vsonyp0werCol_SeparatorHovered,
	vsonyp0werCol_SeparatorActive,
	vsonyp0werCol_ResizeGrip,
	vsonyp0werCol_ResizeGripHovered,
	vsonyp0werCol_ResizeGripActive,
	vsonyp0werCol_Tab,
	vsonyp0werCol_TabHovered,
	vsonyp0werCol_TabActive,
	vsonyp0werCol_TabUnfocused,
	vsonyp0werCol_TabUnfocusedActive,
	vsonyp0werCol_TabText,
	vsonyp0werCol_TabTextHovered,
	vsonyp0werCol_PlotLines,
	vsonyp0werCol_PlotLinesHovered,
	vsonyp0werCol_PlotHistogram,
	vsonyp0werCol_PlotHistogramHovered,
	vsonyp0werCol_TextSelectedBg,
	vsonyp0werCol_DragDropTarget,
	vsonyp0werCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
	vsonyp0werCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
	vsonyp0werCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	vsonyp0werCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
	vsonyp0werCol_COUNT

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werCol_ModalWindowDarkening = vsonyp0werCol_ModalWindowDimBg                      // [renamed in 1.63]
    , vsonyp0werCol_ChildWindowBg = vsonyp0werCol_ChildBg                                      // [renamed in 1.53]
    , vsonyp0werCol_Column = vsonyp0werCol_Separator, vsonyp0werCol_ColumnHovered = vsonyp0werCol_SeparatorHovered, vsonyp0werCol_ColumnActive = vsonyp0werCol_SeparatorActive  // [renamed in 1.51]
    //vsonyp0werCol_CloseButton, vsonyp0werCol_CloseButtonActive, vsonyp0werCol_CloseButtonHovered, // [unused since 1.60+] the close button now uses regular button colors.
    //vsonyp0werCol_ComboBg,                                                              // [unused since 1.53+] ComboBg has been merged with PopupBg, so a redirect isn't accurate.
#endif
};

// Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify the vsonyp0werStyle structure.
// NB: the enum only refers to fields of vsonyp0werStyle which makes sense to be pushed/popped inside UI code. During initialization, feel free to just poke into vsonyp0werStyle directly.
// NB: if changing this enum, you need to update the associated internal table GStyleVarInfo[] accordingly. This is where we link enum values to members offset/type.
enum vsonyp0werStyleVar_
{
    // Enum name --------------------- // Member in vsonyp0werStyle structure (see vsonyp0werStyle for descriptions)
    vsonyp0werStyleVar_Alpha,               // float     Alpha
    vsonyp0werStyleVar_WindowPadding,       // ImVec2    WindowPadding
    vsonyp0werStyleVar_WindowRounding,      // float     WindowRounding
    vsonyp0werStyleVar_WindowBorderSize,    // float     WindowBorderSize
    vsonyp0werStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
    vsonyp0werStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
    vsonyp0werStyleVar_ChildRounding,       // float     ChildRounding
    vsonyp0werStyleVar_ChildBorderSize,     // float     ChildBorderSize
    vsonyp0werStyleVar_PopupRounding,       // float     PopupRounding
    vsonyp0werStyleVar_PopupBorderSize,     // float     PopupBorderSize
    vsonyp0werStyleVar_FramePadding,        // ImVec2    FramePadding
    vsonyp0werStyleVar_FrameRounding,       // float     FrameRounding
    vsonyp0werStyleVar_FrameBorderSize,     // float     FrameBorderSize
    vsonyp0werStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
    vsonyp0werStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
    vsonyp0werStyleVar_IndentSpacing,       // float     IndentSpacing
    vsonyp0werStyleVar_ScrollbarSize,       // float     ScrollbarSize
    vsonyp0werStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
    vsonyp0werStyleVar_GrabMinSize,         // float     GrabMinSize
    vsonyp0werStyleVar_GrabRounding,        // float     GrabRounding
    vsonyp0werStyleVar_TabRounding,         // float     TabRounding
    vsonyp0werStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
    vsonyp0werStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
    vsonyp0werStyleVar_COUNT

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werStyleVar_Count_ = vsonyp0werStyleVar_COUNT, vsonyp0werStyleVar_ChildWindowRounding = vsonyp0werStyleVar_ChildRounding
#endif
};

// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum vsonyp0werColorEditFlags_
{
    vsonyp0werColorEditFlags_None = 0,
    vsonyp0werColorEditFlags_NoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
    vsonyp0werColorEditFlags_NoPicker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
    vsonyp0werColorEditFlags_NoOptions = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    vsonyp0werColorEditFlags_NoSmallPreview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
    vsonyp0werColorEditFlags_NoInputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
    vsonyp0werColorEditFlags_NoTooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    vsonyp0werColorEditFlags_NoLabel = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    vsonyp0werColorEditFlags_NoSidePreview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
    vsonyp0werColorEditFlags_NoDragDrop = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

    // User Options (right-click on widget to change some of them).
    vsonyp0werColorEditFlags_AlphaBar = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    vsonyp0werColorEditFlags_AlphaPreview = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    vsonyp0werColorEditFlags_AlphaPreviewHalf = 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    vsonyp0werColorEditFlags_HDR = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use vsonyp0werColorEditFlags_Float flag as well).
    vsonyp0werColorEditFlags_DisplayRGB = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
    vsonyp0werColorEditFlags_DisplayHSV = 1 << 21,  // [Display]    // "
    vsonyp0werColorEditFlags_DisplayHex = 1 << 22,  // [Display]    // "
    vsonyp0werColorEditFlags_Uint8 = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    vsonyp0werColorEditFlags_Float = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    vsonyp0werColorEditFlags_PickerHueBar = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
    vsonyp0werColorEditFlags_PickerHueWheel = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
    vsonyp0werColorEditFlags_InputRGB = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
    vsonyp0werColorEditFlags_InputHSV = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.

    // Defaults Options. You can set application defaults using SetColorEditOptions(). The intent is that you probably don't want to
    // override them in most of your calls. Let the user choose via the option menu and/or call SetColorEditOptions() once during startup.
    vsonyp0werColorEditFlags__OptionsDefault = vsonyp0werColorEditFlags_Uint8 | vsonyp0werColorEditFlags_DisplayRGB | vsonyp0werColorEditFlags_InputRGB | vsonyp0werColorEditFlags_PickerHueBar,

    // [Internal] Masks
    vsonyp0werColorEditFlags__DisplayMask = vsonyp0werColorEditFlags_DisplayRGB | vsonyp0werColorEditFlags_DisplayHSV | vsonyp0werColorEditFlags_DisplayHex,
    vsonyp0werColorEditFlags__DataTypeMask = vsonyp0werColorEditFlags_Uint8 | vsonyp0werColorEditFlags_Float,
    vsonyp0werColorEditFlags__PickerMask = vsonyp0werColorEditFlags_PickerHueWheel | vsonyp0werColorEditFlags_PickerHueBar,
    vsonyp0werColorEditFlags__InputMask = vsonyp0werColorEditFlags_InputRGB | vsonyp0werColorEditFlags_InputHSV

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werColorEditFlags_RGB = vsonyp0werColorEditFlags_DisplayRGB, vsonyp0werColorEditFlags_HSV = vsonyp0werColorEditFlags_DisplayHSV, vsonyp0werColorEditFlags_HEX = vsonyp0werColorEditFlags_DisplayHex
#endif
};

// Enumeration for GetMouseCursor()
// User code may request binding to display given cursor by calling SetMouseCursor(), which is why we have some cursors that are marked unused here
enum vsonyp0werMouseCursor_
{
    vsonyp0werMouseCursor_None = -1,
    vsonyp0werMouseCursor_Arrow = 0,
    vsonyp0werMouseCursor_TextInput,         // When hovering over InputText, etc.
    vsonyp0werMouseCursor_ResizeAll,         // (Unused by vsonyp0wer functions)
    vsonyp0werMouseCursor_ResizeNS,          // When hovering over an horizontal border
    vsonyp0werMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
    vsonyp0werMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
    vsonyp0werMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
    vsonyp0werMouseCursor_Hand,              // (Unused by vsonyp0wer functions. Use for e.g. hyperlinks)
    vsonyp0werMouseCursor_COUNT

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werMouseCursor_Count_ = vsonyp0werMouseCursor_COUNT
#endif
};

// Enumateration for vsonyp0wer::SetWindow***(), SetNextWindow***(), SetNextTreeNode***() functions
// Represent a condition.
// Important: Treat as a regular enum! Do NOT combine multiple values using binary operators! All the functions above treat 0 as a shortcut to vsonyp0werCond_Always.
enum vsonyp0werCond_
{
    vsonyp0werCond_Always = 1 << 0,   // Set the variable
    vsonyp0werCond_Once = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
    vsonyp0werCond_FirstUseEver = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    vsonyp0werCond_Appearing = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)

    // Obsolete names (will be removed)
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    , vsonyp0werSetCond_Always = vsonyp0werCond_Always, vsonyp0werSetCond_Once = vsonyp0werCond_Once, vsonyp0werSetCond_FirstUseEver = vsonyp0werCond_FirstUseEver, vsonyp0werSetCond_Appearing = vsonyp0werCond_Appearing
#endif
};

//-----------------------------------------------------------------------------
// Helpers: Memory allocations macros
// IM_MALLOC(), IM_FREE(), IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE()
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a dummy parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
//-----------------------------------------------------------------------------

struct ImNewDummy {};
inline void* operator new(size_t, ImNewDummy, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewDummy, void*) {} // This is only required so we can use the symmetrical new()
#define IM_ALLOC(_SIZE)                     vsonyp0wer::MemAlloc(_SIZE)
#define IM_FREE(_PTR)                       vsonyp0wer::MemFree(_PTR)
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewDummy(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewDummy(), vsonyp0wer::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p) { if (p) { p->~T(); vsonyp0wer::MemFree(p); } }

//-----------------------------------------------------------------------------
// Helper: ImVector<>
// Lightweight std::vector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
// You generally do NOT need to care or use this ever. But we need to make it available in vsonyp0wer.h because some of our data structures are relying on it.
// Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
// do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear vsonyp0wer can be safely initialized by a zero-memset.
//-----------------------------------------------------------------------------

template<typename T>
struct ImVector
{
    int                 Size;
    int                 Capacity;
    T* Data;

    // Provide standard typedefs but we don't use them ourselves.
    typedef T                   value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    // Constructors, destructor
    inline ImVector() { Size = Capacity = 0; Data = NULL; }
    inline ImVector(const ImVector<T>& src) { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline ImVector<T>& operator=(const ImVector<T>& src) { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~ImVector() { if (Data) IM_FREE(Data); }

    inline bool         empty() const { return Size == 0; }
    inline int          size() const { return Size; }
    inline int          size_in_bytes() const { return Size * (int)sizeof(T); }
    inline int          capacity() const { return Capacity; }
    inline T& operator[](int i) { IM_ASSERT(i < Size); return Data[i]; }
    inline const T& operator[](int i) const { IM_ASSERT(i < Size); return Data[i]; }

    inline void         clear() { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }
    inline T* begin() { return Data; }
    inline const T* begin() const { return Data; }
    inline T* end() { return Data + Size; }
    inline const T* end() const { return Data + Size; }
    inline T& front() { IM_ASSERT(Size > 0); return Data[0]; }
    inline const T& front() const { IM_ASSERT(Size > 0); return Data[0]; }
    inline T& back() { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T& back() const { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(ImVector<T>& rhs) { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    inline void         resize(int new_size, const T & v) { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
    inline void         reserve(int new_capacity) { if (new_capacity <= Capacity) return; T * new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const T & v) { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
    inline void         pop_back() { IM_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T & v) { if (Size == 0) push_back(v); else insert(Data, v); }
    inline T* erase(const T * it) { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
    inline T * erase(const T * it, const T * it_last) { IM_ASSERT(it >= Data && it < Data + Size && it_last > it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T * erase_unsorted(const T * it) { IM_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T * insert(const T * it, const T & v) { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T & v) const { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline int          index_from_ptr(const T * it) const { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};

//-----------------------------------------------------------------------------
// vsonyp0werStyle
// You may modify the vsonyp0wer::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and vsonyp0wer::PushStyleColor(vsonyp0werCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct vsonyp0werStyle
{
    float       Alpha;                      // Global alpha applies to everything in vsonyp0wer.
    ImVec2      WindowPadding;              // Padding within a window.
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
    ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
    ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns.
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    float       TabBorderSize;              // Thickness of border around tabs.
    ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    ImVec2      SelectableTextAlign;        // Alignment of selectable text when selectable is larger than text. Defaults to (0.0f, 0.0f) (top-left aligned).
    ImVec2      DisplayWindowPadding;       // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
    ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
    bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    ImVec4      Colors[vsonyp0werCol_COUNT];

    vsonyp0wer_API vsonyp0werStyle();
    vsonyp0wer_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// vsonyp0werIO
// Communicate most settings and inputs/outputs to Dear vsonyp0wer using this structure.
// Access via vsonyp0wer::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
//-----------------------------------------------------------------------------

struct vsonyp0werIO
{
    //------------------------------------------------------------------
    // Configuration (fill once)                // Default value
    //------------------------------------------------------------------

    vsonyp0werConfigFlags   ConfigFlags;             // = 0              // See vsonyp0werConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
    vsonyp0werBackendFlags  BackendFlags;            // = 0              // See vsonyp0werBackendFlags_ enum. Set by back-end (vsonyp0wer_impl_xxx files or custom back-end) to communicate features supported by the back-end.
    ImVec2      DisplaySize;                    // <unset>          // Main display size, in pixels.
    float       DeltaTime;                      // = 1.0f/60.0f     // Time elapsed since last frame, in seconds.
    float       IniSavingRate;                  // = 5.0f           // Minimum time between saving positions/sizes to .ini file, in seconds.
    const char* IniFilename;                    // = "vsonyp0wer.ini"    // Path to .ini file. Set NULL to disable automatic .ini loading/saving, if e.g. you want to manually load/save from memory.
    const char* LogFilename;                    // = "vsonyp0wer_log.txt"// Path to .log file (default parameter to vsonyp0wer::LogToFile when no file is specified).
    float       MouseDoubleClickTime;           // = 0.30f          // Time for a double-click, in seconds.
    float       MouseDoubleClickMaxDist;        // = 6.0f           // Distance threshold to stay in to validate a double-click, in pixels.
    float       MouseDragThreshold;             // = 6.0f           // Distance threshold before considering we are dragging.
    int         KeyMap[vsonyp0werKey_COUNT];         // <unset>          // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.
    float       KeyRepeatDelay;                 // = 0.250f         // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float       KeyRepeatRate;                  // = 0.050f         // When holding a key/button, rate at which it repeats, in seconds.
    void* UserData;                       // = NULL           // Store your own data for retrieval by callbacks.

    ImFontAtlas* Fonts;                          // <auto>           // Load, rasterize and pack one or more fonts into a single texture.
    float       FontGlobalScale;                // = 1.0f           // Global scale all fonts
    bool        FontAllowUserScaling;           // = false          // Allow user scaling text of individual window with CTRL+Wheel.
    ImFont* FontDefault;                    // = NULL           // Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
    ImVec2      DisplayFramebufferScale;        // = (1, 1)         // For retina display or other situations where window coordinates are different from framebuffer coordinates. This generally ends up in ImDrawData::FramebufferScale.

    // Miscellaneous options
    bool        MouseDrawCursor;                // = false          // Request vsonyp0wer to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by back-end implementations.
    bool        ConfigMacOSXBehaviors;          // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text Start and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl (was called io.OptMacOSXBehaviors prior to 1.63)
    bool        ConfigInputTextCursorBlink;     // = true           // Set to false to disable blinking cursor, for users who consider it distracting. (was called: io.OptCursorBlink prior to 1.63)
    bool        ConfigWindowsResizeFromEdges;   // = true           // Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & vsonyp0werBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be a per-window vsonyp0werWindowFlags_ResizeFromAnySide flag)
    bool        ConfigWindowsMoveFromTitleBarOnly; // = false       // [BETA] Set to true to only allow moving windows when clicked+dragged from the title bar. Windows without a title bar are not affected.

    //------------------------------------------------------------------
    // Platform Functions
    // (the vsonyp0wer_impl_xxxx back-end files are setting those up for you)
    //------------------------------------------------------------------

    // Optional: Platform/Renderer back-end name (informational only! will be displayed in About Window) + User data for back-end/wrappers to store their own stuff.
    const char* BackendPlatformName;            // = NULL
    const char* BackendRendererName;            // = NULL
    void* BackendPlatformUserData;        // = NULL
    void* BackendRendererUserData;        // = NULL
    void* BackendLanguageUserData;        // = NULL

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void* ClipboardUserData;

    // Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
    // (default to use native imm32 api on Windows)
    void        (*ImeSetInputScreenPosFn)(int x, int y);
    void* ImeWindowHandle;                // = NULL           // (Windows) Set this to your HWND to get automatic IME cursor positioning.

#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    // [OBSOLETE since 1.60+] Rendering function, will be automatically called in Render(). Please call your rendering function yourself now!
    // You can obtain the ImDrawData* by calling vsonyp0wer::GetDrawData() after Render(). See example applications if you are unsure of how to implement this.
    void        (*RenderDrawListsFn)(ImDrawData* data);
#else
    // This is only here to keep vsonyp0werIO the same size/layout, so that vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS can exceptionally be used outside of imconfig.h.
    void* RenderDrawListsFnUnused;
#endif

    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------

    ImVec2      MousePos;                       // Mouse position, in pixels. Set to ImVec2(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
    bool        MouseDown[5];                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. vsonyp0wer itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
    float       MouseWheelH;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
    bool        KeyCtrl;                        // Keyboard modifier pressed: Control
    bool        KeyShift;                       // Keyboard modifier pressed: Shift
    bool        KeyAlt;                         // Keyboard modifier pressed: Alt
    bool        KeySuper;                       // Keyboard modifier pressed: Cmd/Super/Windows
    bool        KeysDown[512];                  // Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys).
    float       NavInputs[vsonyp0werNavInput_COUNT]; // Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame().

    // Functions
    vsonyp0wer_API void  AddInputCharacter(ImWchar c);               // Queue new character input
    vsonyp0wer_API void  AddInputCharactersUTF8(const char* str);    // Queue new characters input from an UTF-8 string
    vsonyp0wer_API void  ClearInputCharacters();                     // Clear the text input buffer manually

    //------------------------------------------------------------------
    // Output - Retrieve after calling NewFrame()
    //------------------------------------------------------------------

    bool        WantCaptureMouse;               // When io.WantCaptureMouse is true, vsonyp0wer will use the mouse inputs, do not dispatch them to your main game/application (in both cases, always pass on mouse inputs to vsonyp0wer). (e.g. unclicked mouse is hovering over an vsonyp0wer window, widget is active, mouse was clicked over an vsonyp0wer window, etc.).
    bool        WantCaptureKeyboard;            // When io.WantCaptureKeyboard is true, vsonyp0wer will use the keyboard inputs, do not dispatch them to your main game/application (in both cases, always pass keyboard inputs to vsonyp0wer). (e.g. InputText active, or an vsonyp0wer window is focused and navigation is enabled, etc.).
    bool        WantTextInput;                  // Mobile/console: when io.WantTextInput is true, you may display an on-screen keyboard. This is set by vsonyp0wer when it wants textual keyboard input to happen (e.g. when a InputText widget is active).
    bool        WantSetMousePos;                // MousePos has been altered, back-end should reposition mouse on next frame. Set only when vsonyp0werConfigFlags_NavEnableSetMousePos flag is enabled.
    bool        WantSaveIniSettings;            // When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. IMPORTANT: You need to clear io.WantSaveIniSettings yourself.
    bool        NavActive;                      // Directional navigation is currently allowed (will handle vsonyp0werKey_NavXXX events) = a window is focused and it doesn't use the vsonyp0werWindowFlags_NoNavInputs flag.
    bool        NavVisible;                     // Directional navigation is visible and allowed (will handle vsonyp0werKey_NavXXX events).
    float       Framerate;                      // Application framerate estimation, in frame per second. Solely for convenience. Rolling avehnly estimation based on IO.DeltaTime over 120 frames
    int         MetricsRenderVertices;          // Vertices output during last call to Render()
    int         MetricsRenderIndices;           // Indices output during last call to Render() = number of triangles * 3
    int         MetricsRenderWindows;           // Number of visible windows
    int         MetricsActiveWindows;           // Number of active windows
    int         MetricsActiveAllocations;       // Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple vsonyp0wer contexts.
    ImVec2      MouseDelta;                     // Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta.

    //------------------------------------------------------------------
    // [Internal] vsonyp0wer will maintain those fields. Forward compatibility not guaranteed!
    //------------------------------------------------------------------

    ImVec2      MousePosPrev;                   // Previous mouse position (note that MouseDelta is not necessary == MousePos-MousePosPrev, in case either position is invalid)
    ImVec2      MouseClickedPos[5];             // Position at time of clicking
    double      MouseClickedTime[5];            // Time of last click (used to figure out double-click)
    bool        MouseClicked[5];                // Mouse button went from !Down to Down
    bool        MouseDoubleClicked[5];          // Has mouse button been double-clicked?
    bool        MouseReleased[5];               // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];              // Track if button was clicked inside a window. We don't request mouse capture from the application if click started outside vsonyp0wer bounds.
    float       MouseDownDuration[5];           // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];       // Previous time the mouse button has been down
    ImVec2      MouseDragMaxDistanceAbs[5];     // Maximum distance, absolute, on each axis, of how much mouse has traveled from the clicking point
    float       MouseDragMaxDistanceSqr[5];     // Squared maximum distance of how much mouse has traveled from the clicking point
    float       KeysDownDuration[512];          // Duration the keyboard key has been down (0.0f == just pressed)
    float       KeysDownDurationPrev[512];      // Previous duration the key has been down
    float       NavInputsDownDuration[vsonyp0werNavInput_COUNT];
    float       NavInputsDownDurationPrev[vsonyp0werNavInput_COUNT];
    ImVector<ImWchar> InputQueueCharacters;     // Queue of _characters_ input (obtained by platform back-end). Fill using AddInputCharacter() helper.

    vsonyp0wer_API   vsonyp0werIO();
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

// Shared state of InputText(), passed as an argument to your callback when a vsonyp0werInputTextFlags_Callback* flag is used.
// The callback function should return 0 by default.
// Callbacks (follow a flag name and see comments in vsonyp0werInputTextFlags_ declarations for more details)
// - vsonyp0werInputTextFlags_CallbackCompletion:  Callback on pressing TAB
// - vsonyp0werInputTextFlags_CallbackHistory:     Callback on pressing Up/Down arrows
// - vsonyp0werInputTextFlags_CallbackAlways:      Callback on each iteration
// - vsonyp0werInputTextFlags_CallbackCharFilter:  Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
// - vsonyp0werInputTextFlags_CallbackResize:      Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow.
struct vsonyp0werInputTextCallbackData
{
    vsonyp0werInputTextFlags EventFlag;      // One vsonyp0werInputTextFlags_Callback*    // Read-only
    vsonyp0werInputTextFlags Flags;          // What user passed to InputText()      // Read-only
    void* UserData;       // What user passed to InputText()      // Read-only

    // Arguments for the different callback events
    // - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
    // - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
    ImWchar             EventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
    vsonyp0werKey            EventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
    char* Buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
    int                 BufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator stohnly. In C land: == strlen(some_text), in C++ land: string.length()
    int                 BufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator stohnly. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
    bool                BufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
    int                 CursorPos;      //                                      // Read-write   // [Completion,History,Always]
    int                 SelectionStart; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
    int                 SelectionEnd;   //                                      // Read-write   // [Completion,History,Always]

    // Helper functions for text manipulation.
    // Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
    vsonyp0wer_API vsonyp0werInputTextCallbackData();
    vsonyp0wer_API void      DeleteChars(int pos, int bytes_count);
    vsonyp0wer_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
    bool                HasSelection() const { return SelectionStart != SelectionEnd; }
};

// Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
struct vsonyp0werSizeCallbackData
{
    void* UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
    ImVec2  Pos;            // Read-only.   Window position, for reference.
    ImVec2  CurrentSize;    // Read-only.   Current window size.
    ImVec2  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
};

// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
struct vsonyp0werPayload
{
    // Members
    void* Data;               // Data (copied and owned by dear vsonyp0wer)
    int             DataSize;           // Data size

    // [Internal]
    vsonyp0werID         SourceId;           // Source item id
    vsonyp0werID         SourceParentId;     // Source parent id (if available)
    int             DataFrameCount;     // Data timestamp
    char            DataType[32 + 1];     // Data type tag (short user-supplied string, 32 characters max)
    bool            Preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
    bool            Delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

    vsonyp0werPayload() { Clear(); }
    void Clear() { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
    bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
    bool IsPreview() const { return Preview; }
    bool IsDelivery() const { return Delivery; }
};

//-----------------------------------------------------------------------------
// Obsolete functions (Will be removed! Read 'API BREAKING CHANGES' section in vsonyp0wer.cpp for details)
// Please keep your copy of dear vsonyp0wer up to date! Occasionally set '#define vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS' in imconfig.h to stay ahead.
//-----------------------------------------------------------------------------

#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
namespace vsonyp0wer
{
    // OBSOLETED in 1.69 (from Mar 2019)
    static inline ImDrawList* GetOverlayDrawList() { return GetForegroundDrawList(); }
    // OBSOLETED in 1.66 (from Sep 2018)
    static inline void  SetScrollHere(float center_ratio = 0.5f) { SetScrollHereY(center_ratio); }
    // OBSOLETED in 1.63 (between Aug 2018 and Sept 2018)
    static inline bool  IsItemDeactivatedAfterChange() { return IsItemDeactivatedAfterEdit(); }
    // OBSOLETED in 1.61 (between Apr 2018 and Aug 2018)
    vsonyp0wer_API bool      InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, vsonyp0werInputTextFlags flags = 0); // Use the 'const char* format' version instead of 'decimal_precision'!
    vsonyp0wer_API bool      InputFloat2(const char* label, float v[2], int decimal_precision, vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool      InputFloat3(const char* label, float v[3], int decimal_precision, vsonyp0werInputTextFlags flags = 0);
    vsonyp0wer_API bool      InputFloat4(const char* label, float v[4], int decimal_precision, vsonyp0werInputTextFlags flags = 0);
    // OBSOLETED in 1.60 (between Dec 2017 and Apr 2018)
    static inline bool  IsAnyWindowFocused() { return IsWindowFocused(vsonyp0werFocusedFlags_AnyWindow); }
    static inline bool  IsAnyWindowHovered() { return IsWindowHovered(vsonyp0werHoveredFlags_AnyWindow); }
    static inline ImVec2 CalcItemRectClosestPoint(const ImVec2& pos, bool on_edge = false, float outward = 0.f) { IM_UNUSED(on_edge); IM_UNUSED(outward); IM_ASSERT(0); return pos; }
    // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    static inline void  ShowTestWindow() { return ShowDemoWindow(); }
    static inline bool  IsRootWindowFocused() { return IsWindowFocused(vsonyp0werFocusedFlags_RootWindow); }
    static inline bool  IsRootWindowOrAnyChildFocused() { return IsWindowFocused(vsonyp0werFocusedFlags_RootAndChildWindows); }
    static inline void  SetNextWindowContentWidth(float w) { SetNextWindowContentSize(ImVec2(w, 0.0f)); }
    static inline float GetItemsLineHeightWithSpacing() { return GetFrameHeightWithSpacing(); }
    // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    vsonyp0wer_API bool      Begin(const char* name, bool* p_open, const ImVec2& size_on_first_use, float bg_alpha_override = -1.0f, vsonyp0werWindowFlags flags = 0); // Use SetNextWindowSize(size, vsonyp0werCond_FirstUseEver) + SetNextWindowBgAlpha() instead.
    static inline bool  IsRootWindowOrAnyChildHovered() { return IsWindowHovered(vsonyp0werHoveredFlags_RootAndChildWindows); }
    static inline void  AlignFirstTextHeightToWidgets() { AlignTextToFramePadding(); }
    static inline void  SetNextWindowPosCenter(vsonyp0werCond c = 0) { vsonyp0werIO& io = GetIO(); SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), c, ImVec2(0.5f, 0.5f)); }
    // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    static inline bool  IsItemHoveredRect() { return IsItemHovered(vsonyp0werHoveredFlags_RectOnly); }
    static inline bool  IsPosHoveringAnyWindow(const ImVec2&) { IM_ASSERT(0); return false; } // This was misleading and partly broken. You probably want to use the vsonyp0wer::GetIO().WantCaptureMouse flag instead.
    static inline bool  IsMouseHoveringAnyWindow() { return IsWindowHovered(vsonyp0werHoveredFlags_AnyWindow); }
    static inline bool  IsMouseHoveringWindow() { return IsWindowHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup | vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem); }
}
typedef vsonyp0werInputTextCallback      vsonyp0werTextEditCallback;    // OBSOLETE in 1.63 (from Aug 2018): made the names consistent
typedef vsonyp0werInputTextCallbackData  vsonyp0werTextEditCallbackData;
#endif

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Helper: Execute a block of code at maximum once a frame. Convenient if you want to quickly create an UI within deep-nested code that runs multiple times every frame.
// Usage: static vsonyp0werOnceUponAFrame oaf; if (oaf) vsonyp0wer::Text("This will be called only once per frame");
struct vsonyp0werOnceUponAFrame
{
    vsonyp0werOnceUponAFrame() { RefFrame = -1; }
    mutable int RefFrame;
    operator bool() const { int current_frame = vsonyp0wer::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// Helper: Macro for vsonyp0werOnceUponAFrame. Attention: The macro expands into 2 statement so make sure you don't use it within e.g. an if() statement without curly braces.
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
#define vsonyp0wer_ONCE_UPON_A_FRAME     static vsonyp0werOnceUponAFrame vsonyp0wer_oaf; if (vsonyp0wer_oaf)    // OBSOLETED in 1.51, will remove!
#endif

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct vsonyp0werTextFilter
{
    vsonyp0wer_API           vsonyp0werTextFilter(const char* default_filter = "");
    vsonyp0wer_API bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);  // Helper calling InputText+Build
    vsonyp0wer_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
    vsonyp0wer_API void      Build();
    void                Clear() { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }

    // [Internal]
    struct TextRange
    {
        const char* b;
        const char* e;

        TextRange() { b = e = NULL; }
        TextRange(const char* _b, const char* _e) { b = _b; e = _e; }
        const char* begin() const { return b; }
        const char* end() const { return e; }
        bool            empty() const { return b == e; }
        vsonyp0wer_API void  split(char separator, ImVector<TextRange>* out) const;
    };
    char                InputBuf[256];
    ImVector<TextRange> Filters;
    int                 CountGrep;
};

// Helper: Growable text buffer for logging/accumulating text
// (this could be called 'vsonyp0werTextBuilder' / 'vsonyp0werStringBuilder')
struct vsonyp0werTextBuffer
{
    ImVector<char>      Buf;
    static char         EmptyString[1];

    vsonyp0werTextBuffer() { }
    inline char         operator[](int i) { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    const char* begin() const { return Buf.Data ? &Buf.front() : EmptyString; }
    const char* end() const { return Buf.Data ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
    int                 size() const { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() { return Buf.Size <= 1; }
    void                clear() { Buf.clear(); }
    void                reserve(int capacity) { Buf.reserve(capacity); }
    const char* c_str() const { return Buf.Data ? Buf.Data : EmptyString; }
    vsonyp0wer_API void      append(const char* str, const char* str_end = NULL);
    vsonyp0wer_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
    vsonyp0wer_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// Helper: Key->Value stohnly
// Typically you don't have to worry about this since a stohnly is held within each Window.
// We use it to e.g. store collapse state for a tree (Int 0/1)
// This is optimized for efficient lookup (dichotomy into a contiguous buffer) and rare insertion (typically tied to user interactions aka max once a frame)
// You can use it as custom user stohnly for temporary values. Declare your own stohnly if, for example:
// - You want to manipulate the open/close state of a particular sub-tree in your interface (tree node uses Int 0/1 to store their state).
// - You want to store custom debug data easily without adding or editing structures in your code (probably not efficient, but convenient)
// Types are NOT stored, so it is up to you to make sure your Key don't collide with different types.
struct vsonyp0werStohnly
{
    struct Pair
    {
        vsonyp0werID key;
        union { int val_i; float val_f; void* val_p; };
        Pair(vsonyp0werID _key, int _val_i) { key = _key; val_i = _val_i; }
        Pair(vsonyp0werID _key, float _val_f) { key = _key; val_f = _val_f; }
        Pair(vsonyp0werID _key, void* _val_p) { key = _key; val_p = _val_p; }
    };
    ImVector<Pair>      Data;

    // - Get***() functions find pair, never add/allocate. Pairs are sorted so a query is O(log N)
    // - Set***() functions find pair, insertion on demand if missing.
    // - Sorted insertion is costly, paid once. A typical frame shouldn't need to insert any new pair.
    void                Clear() { Data.clear(); }
    vsonyp0wer_API int       GetInt(vsonyp0werID key, int default_val = 0) const;
    vsonyp0wer_API void      SetInt(vsonyp0werID key, int val);
    vsonyp0wer_API bool      GetBool(vsonyp0werID key, bool default_val = false) const;
    vsonyp0wer_API void      SetBool(vsonyp0werID key, bool val);
    vsonyp0wer_API float     GetFloat(vsonyp0werID key, float default_val = 0.0f) const;
    vsonyp0wer_API void      SetFloat(vsonyp0werID key, float val);
    vsonyp0wer_API void* GetVoidPtr(vsonyp0werID key) const; // default_val is NULL
    vsonyp0wer_API void      SetVoidPtr(vsonyp0werID key, void* val);

    // - Get***Ref() functions finds pair, insert on demand if missing, return pointer. Useful if you intend to do Get+Set.
    // - References are only valid until a new value is added to the stohnly. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
    // - A typical use case where this is convenient for quick hacking (e.g. add stohnly during a live Edit&Continue session if you can't modify existing struct)
    //      float* pvar = vsonyp0wer::GetFloatRef(key); vsonyp0wer::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
    vsonyp0wer_API int* GetIntRef(vsonyp0werID key, int default_val = 0);
    vsonyp0wer_API bool* GetBoolRef(vsonyp0werID key, bool default_val = false);
    vsonyp0wer_API float* GetFloatRef(vsonyp0werID key, float default_val = 0.0f);
    vsonyp0wer_API void** GetVoidPtrRef(vsonyp0werID key, void* default_val = NULL);

    // Use on your own stohnly if you know only integer are being stored (open/close all tree nodes)
    vsonyp0wer_API void      SetAllInt(int val);

    // For quicker full rebuild of a stohnly (instead of an incremental one), you may add all your contents and then sort once.
    vsonyp0wer_API void      BuildSortByKey();
};

// Helper: Manually clip large list of items.
// If you are submitting lots of evenly spaced items and you have a random access to the list, you can perform coarse clipping based on visibility to save yourself from processing those items at all.
// The clipper calculates the range of visible items and advance the cursor to compensate for the non-visible items we have skipped.
// vsonyp0wer already clip items based on their bounds but it needs to measure text size to do so. Coarse clipping before submission makes this cost and your own data fetching/submission cost null.
// Usage:
//     vsonyp0werListClipper clipper(1000);  // we have 1000 elements, evenly spaced.
//     while (clipper.Step())
//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//             vsonyp0wer::Text("line number %d", i);
// - Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height (step skipped if we passed a known height as second arg to constructor).
// - Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
// - (Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user call Step(). Does nothing and switch to Step 3.)
// - Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
struct vsonyp0werListClipper
{
    float   StartPosY;
    float   ItemsHeight;
    int     ItemsCount, StepNo, DisplayStart, DisplayEnd;

    // items_count:  Use -1 to ignore (you can call Begin later). Use INT_MAX if you don't know how many items you have (in which case the cursor won't be advanced in the final step).
    // items_height: Use -1.0f to be calculated automatically on first step. Otherwise pass in the distance between your items, typically GetTextLineHeightWithSpacing() or GetFrameHeightWithSpacing().
    // If you don't specify an items_height, you NEED to call Step(). If you specify items_height you may call the old Begin()/End() api directly, but prefer calling Step().
    vsonyp0werListClipper(int items_count = -1, float items_height = -1.0f) { Begin(items_count, items_height); } // NB: Begin() initialize every fields (as we allow user to call Begin/End multiple times on a same instance if they want).
    ~vsonyp0werListClipper() { IM_ASSERT(ItemsCount == -1); }      // Assert if user forgot to call End() or Step() until false.

    vsonyp0wer_API bool Step();                                              // Call until it returns false. The DisplayStart/DisplayEnd fields will be set and you can process/draw those items.
    vsonyp0wer_API void Begin(int items_count, float items_height = -1.0f);  // Automatically called by constructor if you passed 'items_count' or by Step() in Step 1.
    vsonyp0wer_API void End();                                               // Automatically called on the last call of Step() that returns false.
};

// Helpers macros to generate 32-bits encoded colors
#ifdef vsonyp0wer_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// Helper: ImColor() implicitly converts colors to either ImU32 (packed 4x1 byte) or ImVec4 (4x1 float)
// Prefer using IM_COL32() macros if you want a guaranteed compile-time ImU32 for usage with ImDrawList API.
// **Avoid storing ImColor! Store either u32 of ImVec4. This is not a full-featured color class. MAY OBSOLETE.
// **None of the vsonyp0wer API are using ImColor directly but you can use it as a convenience to pass colors in either ImU32 or ImVec4 formats. Explicitly cast to ImU32 or ImVec4 if needed.
struct ImColor
{
    ImVec4              Value;

    ImColor() { Value.x = Value.y = Value.z = Value.w = 0.0f; }
    ImColor(int r, int g, int b, int a = 255) { float sc = 1.0f / 255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
    ImColor(ImU32 rgba) { float sc = 1.0f / 255.0f; Value.x = (float)((rgba >> IM_COL32_R_SHIFT) & 0xFF) * sc; Value.y = (float)((rgba >> IM_COL32_G_SHIFT) & 0xFF) * sc; Value.z = (float)((rgba >> IM_COL32_B_SHIFT) & 0xFF) * sc; Value.w = (float)((rgba >> IM_COL32_A_SHIFT) & 0xFF) * sc; }
    ImColor(float r, float g, float b, float a = 1.0f) { Value.x = r; Value.y = g; Value.z = b; Value.w = a; }
    ImColor(const ImVec4 & col) { Value = col; }
    inline operator ImU32() const { return vsonyp0wer::ColorConvertFloat4ToU32(Value); }
    inline operator ImVec4() const { return Value; }

    // FIXME-OBSOLETE: May need to obsolete/cleanup those helpers.
    inline void    SetHSV(float h, float s, float v, float a = 1.0f) { vsonyp0wer::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
    static ImColor HSV(float h, float s, float v, float a = 1.0f) { float r, g, b; vsonyp0wer::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImColor(r, g, b, a); }
};

//-----------------------------------------------------------------------------
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// Draw callbacks for advanced uses.
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to: A) Change your GPU render state,
// B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);

// Typically, 1 command = 1 GPU draw call (unless command is a callback)
struct ImDrawCmd
{
    unsigned int    ElemCount;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
    ImVec4          ClipRect;               // Clipping rectangle (x1, y1, x2, y2). Subtract ImDrawData->DisplayPos to get clipping rectangle in "viewport" coordinates
    ImTextureID     TextureId;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
    ImDrawCallback  UserCallback;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
    void* UserCallbackData;       // The draw callback code can access this.

    ImDrawCmd() { ElemCount = 0; ClipRect.x = ClipRect.y = ClipRect.z = ClipRect.w = 0.0f; TextureId = (ImTextureID)NULL; UserCallback = NULL; UserCallbackData = NULL; }
};

// Vertex index (override with '#define ImDrawIdx unsigned int' in imconfig.h)
#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;
#endif

// Vertex layout
#ifndef vsonyp0wer_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert
{
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
};
#else
// You can override the vertex format layout by defining vsonyp0wer_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef)
// NOTE: vsonyp0wer DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
vsonyp0wer_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// Draw channels are used by the Columns API to "split" the render list into different channels while building, so items of each column can be batched together.
// You can also use them to simulate drawing layers and submit primitives in a different order than how they will be rendered.
struct ImDrawChannel
{
    ImVector<ImDrawCmd>     CmdBuffer;
    ImVector<ImDrawIdx>     IdxBuffer;
};

enum ImDrawCornerFlags_
{
    ImDrawCornerFlags_TopLeft = 1 << 0, // 0x1
    ImDrawCornerFlags_TopRight = 1 << 1, // 0x2
    ImDrawCornerFlags_BotLeft = 1 << 2, // 0x4
    ImDrawCornerFlags_BotRight = 1 << 3, // 0x8
    ImDrawCornerFlags_Top = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,   // 0x3
    ImDrawCornerFlags_Bot = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,   // 0xC
    ImDrawCornerFlags_Left = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,    // 0x5
    ImDrawCornerFlags_Right = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,  // 0xA
    ImDrawCornerFlags_All = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of ImDrawCornerFlags_All, as a convenience
};

enum ImDrawListFlags_
{
    ImDrawListFlags_None = 0,
    ImDrawListFlags_AntiAliasedLines = 1 << 0,  // Lines are anti-aliased (*2 the number of triangles for 1.0f wide line, otherwise *3 the number of triangles)
    ImDrawListFlags_AntiAliasedFill = 1 << 1   // Filled shapes have anti-aliased edges (*2 the number of vertices)
};

// Draw command list
// This is the low-level list of polygons that vsonyp0wer functions are filling. At the end of the frame, all command lists are passed to your vsonyp0werIO::RenderDrawListFn function for rendering.
// Each vsonyp0wer window contains its own ImDrawList. You can use vsonyp0wer::GetWindowDrawList() to access the current window draw list and draw custom primitives.
// You can interleave normal vsonyp0wer:: calls and adding primitives to the current draw list.
// All positions are generally in pixel coordinates (top-left at (0,0), bottom-right at io.DisplaySize), but you are totally free to apply whatever transformation matrix to want to the data (if you apply such transformation you'll want to apply it to ClipRect as well)
// Important: Primitives are always added to the list and not culled (culling is done at higher-level by vsonyp0wer:: functions), if you use this API a lot consider coarse culling your drawn objects.
struct ImDrawList
{
    // This is what you have to render
    ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
    ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
    ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

    // [Internal, used while building lists]
    const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use vsonyp0wer::GetDrawListSharedData() to get the one from current vsonyp0wer context)
    const char* _OwnerName;         // Pointer to owner window's name for debugging
    unsigned int            _VtxCurrentIdx;     // [Internal] == VtxBuffer.Size
    ImDrawVert* _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx* _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector<ImVec4>        _ClipRectStack;     // [Internal]
    ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
    ImVector<ImVec2>        _Path;              // [Internal] current path building
    int                     _ChannelsCurrent;   // [Internal] current channel number (0)
    int                     _ChannelsCount;     // [Internal] number of active channels (1+)
    ImVector<ImDrawChannel> _Channels;          // [Internal] draw channels for columns API (not resized down so _ChannelsCount may be smaller than _Channels.Size)

    // If you want to create ImDrawList instances, pass them vsonyp0wer::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without vsonyp0wer)
    ImDrawList(const ImDrawListSharedData* shared_data) { _Data = shared_data; _OwnerName = NULL; Clear(); }
    ~ImDrawList() { ClearFreeMemory(); }
    vsonyp0wer_API void  PushClipRect(ImVec2 clip_rect_min, ImVec2 clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level vsonyp0wer::PushClipRect() to affect logic (hit-testing and widget culling)
    vsonyp0wer_API void  PushClipRectFullScreen();
    vsonyp0wer_API void  PopClipRect();
    vsonyp0wer_API void  PushTextureID(ImTextureID texture_id);
    vsonyp0wer_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.x, cr.y); }
    inline ImVec2   GetClipRectMax() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.z, cr.w); }

    // Primitives
    vsonyp0wer_API void  AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
    vsonyp0wer_API void  AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);   // a: upper-left, b: lower-right (== upper-left + size), rounding_corners_flags: 4-bits corresponding to which corner to round
    vsonyp0wer_API void  AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);                     // a: upper-left, b: lower-right (== upper-left + size)
    vsonyp0wer_API void  AddRectFilledMultiColor(const ImVec2& a, const ImVec2& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    vsonyp0wer_API void  AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness = 1.0f);
    vsonyp0wer_API void  AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col);
    vsonyp0wer_API void  AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness = 1.0f);
    vsonyp0wer_API void  AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col);
    vsonyp0wer_API void  AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
    vsonyp0wer_API void  AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments = 12);
    vsonyp0wer_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    vsonyp0wer_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
    vsonyp0wer_API void  AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
    vsonyp0wer_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 0), const ImVec2& uv_c = ImVec2(1, 1), const ImVec2& uv_d = ImVec2(0, 1), ImU32 col = IM_COL32_WHITE);
    vsonyp0wer_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
    vsonyp0wer_API void  AddPolyline(const ImVec2* points, int num_points, ImU32 col, bool closed, float thickness);
    vsonyp0wer_API void  AddConvexPolyFilled(const ImVec2* points, int num_points, ImU32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
    vsonyp0wer_API void  AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments = 0);

    // Stateful path API, add points then finish with PathFillConvex() or PathStroke()
    inline    void  PathClear() { _Path.Size = 0; }
    inline    void  PathLineTo(const ImVec2& pos) { _Path.push_back(pos); }
    inline    void  PathLineToMergeDuplicate(const ImVec2& pos) { if (_Path.Size == 0 || memcmp(&_Path.Data[_Path.Size - 1], &pos, 8) != 0) _Path.push_back(pos); }
    inline    void  PathFillConvex(ImU32 col) { AddConvexPolyFilled(_Path.Data, _Path.Size, col); _Path.Size = 0; }  // Note: Anti-aliased filling requires points to be in clockwise order.
    inline    void  PathStroke(ImU32 col, bool closed, float thickness = 1.0f) { AddPolyline(_Path.Data, _Path.Size, col, closed, thickness); _Path.Size = 0; }
    vsonyp0wer_API void  PathArcTo(const ImVec2 & centre, float radius, float a_min, float a_max, int num_segments = 10);
    vsonyp0wer_API void  PathArcToFast(const ImVec2 & centre, float radius, int a_min_of_12, int a_max_of_12);                                            // Use precomputed angles for a 12 steps circle
    vsonyp0wer_API void  PathBezierCurveTo(const ImVec2 & p1, const ImVec2 & p2, const ImVec2 & p3, int num_segments = 0);
    vsonyp0wer_API void  PathRect(const ImVec2 & rect_min, const ImVec2 & rect_max, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);

    // Channels
    // - Use to simulate layers. By switching channels to can render out-of-order (e.g. submit foreground primitives before background primitives)
    // - Use to minimize draw calls (e.g. if going back-and-forth between multiple non-overlapping clipping rectangles, prefer to append into separate channels then merge at the end)
    vsonyp0wer_API void  ChannelsSplit(int channels_count);
    vsonyp0wer_API void  ChannelsMerge();
    vsonyp0wer_API void  ChannelsSetCurrent(int channel_index);

    // Advanced
    vsonyp0wer_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
    vsonyp0wer_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible
    vsonyp0wer_API ImDrawList * CloneOutput() const;                                  // Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer.

    // Internal helpers
    // NB: all primitives needs to be reserved via PrimReserve() beforehand!
    vsonyp0wer_API void  Clear();
    vsonyp0wer_API void  ClearFreeMemory();
    vsonyp0wer_API void  PrimReserve(int idx_count, int vtx_count);
    vsonyp0wer_API void  PrimRect(const ImVec2 & a, const ImVec2 & b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
    vsonyp0wer_API void  PrimRectUV(const ImVec2 & a, const ImVec2 & b, const ImVec2 & uv_a, const ImVec2 & uv_b, ImU32 col);
    vsonyp0wer_API void  PrimQuadUV(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & d, const ImVec2 & uv_a, const ImVec2 & uv_b, const ImVec2 & uv_c, const ImVec2 & uv_d, ImU32 col);
    inline    void  PrimWriteVtx(const ImVec2 & pos, const ImVec2 & uv, ImU32 col) { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
    inline    void  PrimWriteIdx(ImDrawIdx idx) { *_IdxWritePtr = idx; _IdxWritePtr++; }
    inline    void  PrimVtx(const ImVec2 & pos, const ImVec2 & uv, ImU32 col) { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }
    vsonyp0wer_API void  UpdateClipRect();
    vsonyp0wer_API void  UpdateTextureID();
};

// All draw data to render an vsonyp0wer frame
// (NB: the style and the naming convention here is a little inconsistent but we preserve them for backward compatibility purpose)
struct ImDrawData
{
    bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
    ImDrawList** CmdLists;               // Array of ImDrawList* to render. The ImDrawList are owned by vsonyp0werContext and only pointed to from here.
    int             CmdListsCount;          // Number of ImDrawList* to render
    int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
    int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
    ImVec2          DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
    ImVec2          DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)
    ImVec2          FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.

    // Functions
    ImDrawData() { Valid = false; Clear(); }
    ~ImDrawData() { Clear(); }
    void Clear() { Valid = false; CmdLists = NULL; CmdListsCount = TotalVtxCount = TotalIdxCount = 0; DisplayPos = DisplaySize = FramebufferScale = ImVec2(0.f, 0.f); } // The ImDrawList are owned by vsonyp0werContext!
    vsonyp0wer_API void  DeIndexAllBuffers();                    // Helper to convert all buffers from indexed to non-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
    vsonyp0wer_API void  ScaleClipRects(const ImVec2& fb_scale); // Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than vsonyp0wer expects, or if there is a difference between your window resolution and framebuffer resolution.
};

//-----------------------------------------------------------------------------
// Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFontGlyphRangesBuilder, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig
{
    void* FontData;               //          // TTF/OTF data
    int             FontDataSize;           //          // TTF/OTF data size
    bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
    int             FontNo;                 // 0        // Index of font within TTF/OTF file
    float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
    int             OversampleH;            // 3        // Rasterize at higher quality for sub-pixel positioning. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
    int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
    bool            PixelSnapH;             // false    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
    ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
    ImVec2          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
    const ImWchar* GlyphRanges;            // NULL     // Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list). THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE.
    float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
    float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
    bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
    unsigned int    RasterizerFlags;        // 0x00     // Settings for custom font rasterizer (e.g. vsonyp0werFreeType). Leave as zero if you aren't using one.
    float           RasterizerMultiply;     // 1.0f     // Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.

    // [Internal]
    char            Name[40];               // Name (strictly to ease debugging)
    ImFont* DstFont;

    vsonyp0wer_API ImFontConfig();
};

struct ImFontGlyph
{
    ImWchar         Codepoint;          // 0x0000..0xFFFF
    float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
    float           X0, Y0, X1, Y1;     // Glyph corners
    float           U0, V0, U1, V1;     // Texture coordinates
};

// Helper to build glyph ranges from text/string data. Feed your application strings/characters to it then call BuildRanges().
// This is essentially a tightly packed of vector of 64k booleans = 8KB stohnly.
struct ImFontGlyphRangesBuilder
{
    ImVector<int> UsedChars;            // Store 1-bit per Unicode code point (0=unused, 1=used)

    ImFontGlyphRangesBuilder() { UsedChars.resize(0x10000 / sizeof(int)); memset(UsedChars.Data, 0, 0x10000 / sizeof(int)); }
    bool            GetBit(int n) const { int off = (n >> 5); int mask = 1 << (n & 31); return (UsedChars[off] & mask) != 0; }  // Get bit n in the array
    void            SetBit(int n) { int off = (n >> 5); int mask = 1 << (n & 31); UsedChars[off] |= mask; }               // Set bit n in the array
    void            AddChar(ImWchar c) { SetBit(c); }                          // Add character
    vsonyp0wer_API void  AddText(const char* text, const char* text_end = NULL);     // Add string (each character of the UTF-8 string are added)
    vsonyp0wer_API void  AddRanges(const ImWchar * ranges);                           // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
    vsonyp0wer_API void  BuildRanges(ImVector<ImWchar> * out_ranges);                 // Output new ranges
};

enum ImFontAtlasFlags_
{
    ImFontAtlasFlags_None = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
    ImFontAtlasFlags_NoMouseCursors = 1 << 1    // Don't build software mouse cursors into the atlas
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear vsonyp0wer.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see vsonyp0wer_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
struct ImFontAtlas
{
    vsonyp0wer_API ImFontAtlas();
    vsonyp0wer_API ~ImFontAtlas();
    vsonyp0wer_API ImFont* AddFont(const ImFontConfig* font_cfg);
    vsonyp0wer_API ImFont* AddFontDefault(const ImFontConfig* font_cfg = NULL);
    vsonyp0wer_API ImFont* AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
    vsonyp0wer_API ImFont* AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    vsonyp0wer_API ImFont* AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    vsonyp0wer_API ImFont* AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
    vsonyp0wer_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
    vsonyp0wer_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
    vsonyp0wer_API void              ClearFonts();               // Clear output font data (glyphs stohnly, UV coordinates).
    vsonyp0wer_API void              Clear();                    // Clear all input and output.

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
    vsonyp0wer_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
    vsonyp0wer_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
    vsonyp0wer_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
    bool                        IsBuilt() { return Fonts.Size > 0 && (TexPixelsAlpha8 != NULL || TexPixelsRGBA32 != NULL); }
    void                        SetTexID(ImTextureID id) { TexID = id; }

    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------

    // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
    // NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
    // NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.
    vsonyp0wer_API const ImWchar* GetGlyphRangesDefault();                // Basic Latin, Extended Latin
    vsonyp0wer_API const ImWchar* GetGlyphRangesKorean();                 // Default + Korean characters
    vsonyp0wer_API const ImWchar* GetGlyphRangesJapanese();               // Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
    vsonyp0wer_API const ImWchar* GetGlyphRangesChineseFull();            // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
    vsonyp0wer_API const ImWchar* GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
    vsonyp0wer_API const ImWchar* GetGlyphRangesCyrillic();               // Default + about 400 Cyrillic characters
    vsonyp0wer_API const ImWchar* GetGlyphRangesThai();                   // Default + Thai characters
    vsonyp0wer_API const ImWchar* GetGlyphRangesVietnamese();             // Default + Vietname characters

    //-------------------------------------------
    // Custom Rectangles/Glyphs API
    //-------------------------------------------

    // You can request arbitrary rectangles to be packed into the atlas, for your own purposes. After calling Build(), you can query the rectangle position and render your pixels.
    // You can also request your rectangles to be mapped as font glyph (given a font + Unicode point), so you can render e.g. custom colorful icons and use them as regular glyphs.
    struct CustomRect
    {
        unsigned int    ID;             // Input    // User ID. Use <0x10000 to map into a font glyph, >=0x10000 for other/internal/custom texture data.
        unsigned short  Width, Height;  // Input    // Desired rectangle dimension
        unsigned short  X, Y;           // Output   // Packed position in Atlas
        float           GlyphAdvanceX;  // Input    // For custom font glyphs only (ID<0x10000): glyph xadvance
        ImVec2          GlyphOffset;    // Input    // For custom font glyphs only (ID<0x10000): glyph display offset
        ImFont* Font;           // Input    // For custom font glyphs only (ID<0x10000): target font
        CustomRect() { ID = 0xFFFFFFFF; Width = Height = 0; X = Y = 0xFFFF; GlyphAdvanceX = 0.0f; GlyphOffset = ImVec2(0, 0); Font = NULL; }
        bool IsPacked() const { return X != 0xFFFF; }
    };

    vsonyp0wer_API int       AddCustomRectRegular(unsigned int id, int width, int height);                                                                   // Id needs to be >= 0x10000. Id >= 0x80000000 are reserved for vsonyp0wer and ImDrawList
    vsonyp0wer_API int       AddCustomRectFontGlyph(ImFont * font, ImWchar id, int width, int height, float advance_x, const ImVec2 & offset = ImVec2(0, 0));   // Id needs to be < 0x10000 to register a rectangle to map into a specific font.
    const CustomRect * GetCustomRectByIndex(int index) const { if (index < 0) return NULL; return &CustomRects[index]; }

    // [Internal]
    vsonyp0wer_API void      CalcCustomRectUV(const CustomRect * rect, ImVec2 * out_uv_min, ImVec2 * out_uv_max);
    vsonyp0wer_API bool      GetMouseCursorTexData(vsonyp0werMouseCursor cursor, ImVec2 * out_offset, ImVec2 * out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2]);

    //-------------------------------------------
    // Members
    //-------------------------------------------

    bool                        Locked;             // Marked as Locked by vsonyp0wer::NewFrame() so attempt to modify the atlas will assert.
    ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
    ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    int                         TexGlyphPadding;    // Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0.

    // [Internal]
    // NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
    unsigned char* TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int* TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    ImVec2                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
    ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling vsonyp0wer::NewFrame(), use vsonyp0wer::PushFont()/PopFont() to change the current font.
    ImVector<CustomRect>        CustomRects;        // Rectangles for packing custom texture data into the atlas.
    ImVector<ImFontConfig>      ConfigData;         // Internal data
    int                         CustomRectIds[1];   // Identifiers of custom texture rectangle used by ImFontAtlas/ImDrawList

#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    typedef ImFontGlyphRangesBuilder GlyphRangesBuilder; // OBSOLETE 1.67+
#endif
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont
{
    // Members: Hot ~20/24 bytes (for CalcTextSize)
    ImVector<float>             IndexAdvanceX;      // 12-16 // out //            // Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this this info, and are often bottleneck in large UI).
    float                       FallbackAdvanceX;   // 4     // out // = FallbackGlyph->AdvanceX
    float                       FontSize;           // 4     // in  //            // Height of characters/line, set during loading (don't change after loading)

    // Members: Hot ~36/48 bytes (for CalcTextSize + render loop)
    ImVector<ImWchar>           IndexLookup;        // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
    ImVector<ImFontGlyph>       Glyphs;             // 12-16 // out //            // All glyphs.
    const ImFontGlyph* FallbackGlyph;      // 4-8   // out // = FindGlyph(FontFallbackChar)
    ImVec2                      DisplayOffset;      // 8     // in  // = (0,0)    // Offset font rendering by xx pixels

    // Members: Cold ~32/40 bytes
    ImFontAtlas* ContainerAtlas;     // 4-8   // out //            // What we has been loaded into
    const ImFontConfig* ConfigData;         // 4-8   // in  //            // Pointer within ContainerAtlas->ConfigData
    short                       ConfigDataCount;    // 2     // in  // ~ 1        // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
    ImWchar                     FallbackChar;       // 2     // in  // = '?'      // Replacement glyph if one isn't found. Only set via SetFallbackChar()
    float                       Scale;              // 4     // in  // = 1.f      // Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
    float                       Ascent, Descent;    // 4+4   // out //            // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
    int                         MetricsTotalSurface;// 4     // out //            // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
    bool                        DirtyLookupTables;  // 1     // out //

    // Methods
    vsonyp0wer_API ImFont();
    vsonyp0wer_API ~ImFont();
    vsonyp0wer_API const ImFontGlyph* FindGlyph(ImWchar c) const;
    vsonyp0wer_API const ImFontGlyph* FindGlyphNoFallback(ImWchar c) const;
    float                       GetCharAdvance(ImWchar c) const { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
    bool                        IsLoaded() const { return ContainerAtlas != NULL; }
    const char* GetDebugName() const { return ConfigData ? ConfigData->Name : "<unknown>"; }

    // 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
    vsonyp0wer_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
    vsonyp0wer_API const char* CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;
    vsonyp0wer_API void              RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const;
    vsonyp0wer_API void              RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false) const;

    // [Internal] Don't use!
    vsonyp0wer_API void              BuildLookupTable();
    vsonyp0wer_API void              ClearOutputData();
    vsonyp0wer_API void              GrowIndex(int new_size);
    vsonyp0wer_API void              AddGlyph(ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
    vsonyp0wer_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
    vsonyp0wer_API void              SetFallbackChar(ImWchar c);

#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    typedef ImFontGlyph Glyph; // OBSOLETE 1.52+
#endif
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif

// Include vsonyp0wer_user.h at the end of vsonyp0wer.h (convenient for user to only explicitly include vanilla vsonyp0wer.h)
#ifdef vsonyp0wer_INCLUDE_vsonyp0wer_USER_H
#include "vsonyp0wer_user.h"
#endif
