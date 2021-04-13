// dear vsonyp0wer, v1.70 WIP
// (drawing and font code)

/*

Index of this file:

// [SECTION] STB libraries implementation
// [SECTION] Style functions
// [SECTION] ImDrawList
// [SECTION] ImDrawData
// [SECTION] Helpers ShadeVertsXXX functions
// [SECTION] ImFontConfig
// [SECTION] ImFontAtlas
// [SECTION] ImFontAtlas glyph ranges helpers
// [SECTION] ImFontGlyphRangesBuilder
// [SECTION] ImFont
// [SECTION] Internal Render Helpers
// [SECTION] Decompression code
// [SECTION] Default font data (ProggyClean.ttf)

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "vsonyp0wer.h"
#ifndef vsonyp0wer_DEFINE_MATH_OPERATORS
#define vsonyp0wer_DEFINE_MATH_OPERATORS
#endif
#include "vsonyp0wer_internal.h"

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__) || defined(__APPLE__)
#include <alloca.h>     // alloca (glibc uses <alloca.h>. Note that Cygwin may have _WIN32 defined, so the order matters here)
#elif defined(_WIN32)
#include <malloc.h>     // alloca
#if !defined(alloca)
#define alloca _alloca  // for clang with MS Codegen
#endif
#else
#include <stdlib.h>     // alloca
#endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wcomma")
#pragma clang diagnostic ignored "-Wcomma"                  // warning : possible misuse of comma operator here             //
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"      // warning : macro name is a reserved identifier                //
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wstack-protector"          // warning: stack protector not protecting local variables: variable length buffer
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries implementation
//-------------------------------------------------------------------------

// Compile time options:
//#define vsonyp0wer_STB_NAMESPACE           vsdns
//#define vsonyp0wer_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define vsonyp0wer_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define vsonyp0wer_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define vsonyp0wer_DISABLE_STB_RECT_PACK_IMPLEMENTATION

#ifdef vsonyp0wer_STB_NAMESPACE
namespace vsonyp0wer_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wcast-qual"              // warning : cast from 'const xxxx *' to 'xxx *' drops const qualifier //
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'const xxxx *' to type 'xxxx *' casts away qualifiers
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION                        // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef vsonyp0wer_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STBRP_ASSERT(x)     IM_ASSERT(x)
#define STBRP_SORT          ImQsort
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef vsonyp0wer_STB_RECT_PACK_FILENAME
#include vsonyp0wer_STB_RECT_PACK_FILENAME
#else
#include "vsdns_rectpack.h"
#endif
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef vsonyp0wer_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     IM_ASSERT(x)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef vsonyp0wer_STB_TRUETYPE_FILENAME
#include vsonyp0wer_STB_TRUETYPE_FILENAME
#else
#include "vsdns_truetype.h"
#endif
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef vsonyp0wer_STB_NAMESPACE
} // namespace vsdns
using namespace vsonyp0wer_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// [SECTION] Style functions
//-----------------------------------------------------------------------------



void vsonyp0wer::StyleColorsDark(vsonyp0werStyle* dst)
{
    vsonyp0werStyle* style = dst ? dst : &vsonyp0wer::GetStyle();
    ImVec4* colors = style->Colors;

	colors[vsonyp0werCol_Text] = ImColor(213, 213, 213, 255);
	colors[vsonyp0werCol_MenuTheme] = ImColor(180, 230, 30);
	colors[vsonyp0werCol_TabText] = ImColor(100, 100, 100, 255);
	colors[vsonyp0werCol_TextShadow] = ImColor(2, 2, 2, 255);
	colors[vsonyp0werCol_TabTextHovered] = ImColor(185, 185, 185, 255);
	colors[vsonyp0werCol_TextDisabled] = ImColor(213, 213, 213, 255);
	colors[vsonyp0werCol_WindowBg] = ImColor(40, 40, 40, 255);
	colors[vsonyp0werCol_ChildBg] = ImColor(17, 17, 17, 255);
	colors[vsonyp0werCol_PopupBg] = ImColor(65, 65, 65, 255);
	colors[vsonyp0werCol_Border] = ImColor(10, 10, 10, 255);
	colors[vsonyp0werCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[vsonyp0werCol_FrameBg] = ImColor(32, 32, 38, 255);
	colors[vsonyp0werCol_FrameBgHovered] = ImColor(32, 32, 38, 255);
	colors[vsonyp0werCol_FrameBgActive] = ImColor(32, 32, 38, 255);
	colors[vsonyp0werCol_TitleBg] = ImColor(38, 31, 71, 255);
	colors[vsonyp0werCol_TitleBgActive] = ImColor(38, 31, 71, 255);
	colors[vsonyp0werCol_TitleBgCollapsed] = ImColor(38, 31, 71, 255);
	colors[vsonyp0werCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[vsonyp0werCol_ScrollbarBg] = ImColor(45, 45, 45, 255);
	colors[vsonyp0werCol_ScrollbarGrab] = ImColor(65, 65, 65);
	colors[vsonyp0werCol_ScrollbarGrabHovered] = ImColor(75, 75, 75, 255);
	colors[vsonyp0werCol_ScrollbarGrabActive] = ImColor(75, 75, 75, 255);
	colors[vsonyp0werCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[vsonyp0werCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[vsonyp0werCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[vsonyp0werCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[vsonyp0werCol_TitleButton] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[vsonyp0werCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[vsonyp0werCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[vsonyp0werCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[vsonyp0werCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[vsonyp0werCol_Separator] = colors[vsonyp0werCol_Border];
	colors[vsonyp0werCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[vsonyp0werCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[vsonyp0werCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[vsonyp0werCol_ResizeGripHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[vsonyp0werCol_ResizeGripActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[vsonyp0werCol_Tab] = ImColor(34, 34, 34, 0);
	colors[vsonyp0werCol_TabHovered] = ImColor(51, 51, 51, 0);
	colors[vsonyp0werCol_TabActive] = ImColor(51, 51, 51, 0);
	colors[vsonyp0werCol_TabUnfocused] = ImLerp(colors[vsonyp0werCol_Tab], colors[vsonyp0werCol_TitleBg], 0.80f);
	colors[vsonyp0werCol_TabUnfocusedActive] = ImLerp(colors[vsonyp0werCol_TabActive], colors[vsonyp0werCol_TitleBg], 0.40f);
	colors[vsonyp0werCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[vsonyp0werCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[vsonyp0werCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[vsonyp0werCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[vsonyp0werCol_TextSelectedBg] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[vsonyp0werCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[vsonyp0werCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[vsonyp0werCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[vsonyp0werCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[vsonyp0werCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void vsonyp0wer::StyleColorsClassic(vsonyp0werStyle* dst)
{
    vsonyp0werStyle* style = dst ? dst : &vsonyp0wer::GetStyle();
    ImVec4* colors = style->Colors;

    colors[vsonyp0werCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[vsonyp0werCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[vsonyp0werCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
    colors[vsonyp0werCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[vsonyp0werCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[vsonyp0werCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[vsonyp0werCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[vsonyp0werCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[vsonyp0werCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
    colors[vsonyp0werCol_FrameBgActive] = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
    colors[vsonyp0werCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
    colors[vsonyp0werCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
    colors[vsonyp0werCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[vsonyp0werCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
    colors[vsonyp0werCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[vsonyp0werCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[vsonyp0werCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[vsonyp0werCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[vsonyp0werCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[vsonyp0werCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[vsonyp0werCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[vsonyp0werCol_Button] = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
    colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
    colors[vsonyp0werCol_ButtonActive] = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
    colors[vsonyp0werCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[vsonyp0werCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[vsonyp0werCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[vsonyp0werCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[vsonyp0werCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[vsonyp0werCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[vsonyp0werCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
    colors[vsonyp0werCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[vsonyp0werCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[vsonyp0werCol_Tab] = ImLerp(colors[vsonyp0werCol_Header], colors[vsonyp0werCol_TitleBgActive], 0.80f);
    colors[vsonyp0werCol_TabHovered] = colors[vsonyp0werCol_HeaderHovered];
    colors[vsonyp0werCol_TabActive] = ImLerp(colors[vsonyp0werCol_HeaderActive], colors[vsonyp0werCol_TitleBgActive], 0.60f);
    colors[vsonyp0werCol_TabUnfocused] = ImLerp(colors[vsonyp0werCol_Tab], colors[vsonyp0werCol_TitleBg], 0.80f);
    colors[vsonyp0werCol_TabUnfocusedActive] = ImLerp(colors[vsonyp0werCol_TabActive], colors[vsonyp0werCol_TitleBg], 0.40f);
    colors[vsonyp0werCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[vsonyp0werCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[vsonyp0werCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[vsonyp0werCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[vsonyp0werCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[vsonyp0werCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[vsonyp0werCol_NavHighlight] = colors[vsonyp0werCol_HeaderHovered];
    colors[vsonyp0werCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[vsonyp0werCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[vsonyp0werCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void vsonyp0wer::StyleColorsLight(vsonyp0werStyle* dst)
{
    vsonyp0werStyle* style = dst ? dst : &vsonyp0wer::GetStyle();
    ImVec4* colors = style->Colors;

    colors[vsonyp0werCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[vsonyp0werCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[vsonyp0werCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[vsonyp0werCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[vsonyp0werCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[vsonyp0werCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[vsonyp0werCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[vsonyp0werCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[vsonyp0werCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[vsonyp0werCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[vsonyp0werCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[vsonyp0werCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[vsonyp0werCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[vsonyp0werCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[vsonyp0werCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[vsonyp0werCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[vsonyp0werCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[vsonyp0werCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[vsonyp0werCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[vsonyp0werCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    colors[vsonyp0werCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
    colors[vsonyp0werCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[vsonyp0werCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[vsonyp0werCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[vsonyp0werCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[vsonyp0werCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[vsonyp0werCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[vsonyp0werCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[vsonyp0werCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
    colors[vsonyp0werCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
    colors[vsonyp0werCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[vsonyp0werCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[vsonyp0werCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[vsonyp0werCol_Tab] = ImLerp(colors[vsonyp0werCol_Header], colors[vsonyp0werCol_TitleBgActive], 0.90f);
    colors[vsonyp0werCol_TabHovered] = colors[vsonyp0werCol_HeaderHovered];
    colors[vsonyp0werCol_TabActive] = ImLerp(colors[vsonyp0werCol_HeaderActive], colors[vsonyp0werCol_TitleBgActive], 0.60f);
    colors[vsonyp0werCol_TabUnfocused] = ImLerp(colors[vsonyp0werCol_Tab], colors[vsonyp0werCol_TitleBg], 0.80f);
    colors[vsonyp0werCol_TabUnfocusedActive] = ImLerp(colors[vsonyp0werCol_TabActive], colors[vsonyp0werCol_TitleBg], 0.40f);
    colors[vsonyp0werCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[vsonyp0werCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[vsonyp0werCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[vsonyp0werCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[vsonyp0werCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[vsonyp0werCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[vsonyp0werCol_NavHighlight] = colors[vsonyp0werCol_HeaderHovered];
    colors[vsonyp0werCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[vsonyp0werCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[vsonyp0werCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

ImDrawListSharedData::ImDrawListSharedData()
{
    Font = NULL;
    FontSize = 0.0f;
    CurveTessellationTol = 0.0f;
    ClipRectFullscreen = ImVec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);

    // Const data
    for (int i = 0; i < IM_ARRAYSIZE(CircleVtx12); i++)
    {
        const float a = ((float)i * 2 * IM_PI) / (float)IM_ARRAYSIZE(CircleVtx12);
        CircleVtx12[i] = ImVec2(ImCos(a), ImSin(a));
    }
}

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    Flags = ImDrawListFlags_AntiAliasedLines | ImDrawListFlags_AntiAliasedFill;
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    // NB: Do not clear channels so our allocations are re-used after the first frame.
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

ImDrawList* ImDrawList::CloneOutput() const
{
    ImDrawList* dst = IM_NEW(ImDrawList(NULL));
    dst->CmdBuffer = CmdBuffer;
    dst->IdxBuffer = IdxBuffer;
    dst->VtxBuffer = VtxBuffer;
    dst->Flags = Flags;
    return dst;
}

// Using macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug builds
#define GetCurrentClipRect()    (_ClipRectStack.Size ? _ClipRectStack.Data[_ClipRectStack.Size-1]  : _Data->ClipRectFullscreen)
#define GetCurrentTextureId()   (_TextureIdStack.Size ? _TextureIdStack.Data[_TextureIdStack.Size-1] : (ImTextureID)NULL)

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const ImVec4 curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size - 1] : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) != 0) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}

void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(ImVec4)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}

#undef GetCurrentClipRect
#undef GetCurrentTextureId

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level vsonyp0wer::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect && _ClipRectStack.Size)
    {
        ImVec4 current = _ClipRectStack.Data[_ClipRectStack.Size - 1];
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(_Data->ClipRectFullscreen.x, _Data->ClipRectFullscreen.y), ImVec2(_Data->ClipRectFullscreen.z, _Data->ClipRectFullscreen.w));
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::PushTextureID(ImTextureID texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

void ImDrawList::ChannelsSplit(int channels_count)
{
    IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _ChannelsCount = channels_count;

    // _Channels[] (24/32 bytes each) hold stohnly that we'll swap with this->_CmdBuffer/_IdxBuffer
    // The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        } else
        {
            _Channels[i].CmdBuffer.resize(0);
            _Channels[i].IdxBuffer.resize(0);
        }
        if (_Channels[i].CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = _ClipRectStack.back();
            draw_cmd.TextureId = _TextureIdStack.back();
            _Channels[i].CmdBuffer.push_back(draw_cmd);
        }
    }
}

void ImDrawList::ChannelsMerge()
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_ChannelsCount <= 1)
        return;

    ChannelsSetCurrent(0);
    if (CmdBuffer.Size && CmdBuffer.back().ElemCount == 0)
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd * cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    UpdateClipRect(); // We call this instead of AddDrawCmd(), so that empty channels won't produce an extra draw call.
    _ChannelsCount = 1;
}

void ImDrawList::ChannelsSetCurrent(int idx)
{
    IM_ASSERT(idx < _ChannelsCount);
    if (_ChannelsCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
    memcpy(&_Channels.Data[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelsCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}

// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size - 1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_old_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

    int idx_buffer_old_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_old_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2 & a, const ImVec2 & c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(_Data->TexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2 & a, const ImVec2 & c, const ImVec2 & uv_a, const ImVec2 & uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & d, const ImVec2 & uv_a, const ImVec2 & uv_b, const ImVec2 & uv_c, const ImVec2 & uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx + 2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx + 3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using ImVec2 and superflous function calls to optimize debug/non-inlined builds.
// Those macros expects l-values.
#define IM_NORMALIZE2F_OVER_ZERO(VX,VY)                         { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / ImSqrt(d2); VX *= inv_len; VY *= inv_len; } }
#define IM_NORMALIZE2F_OVER_EPSILON_CLAMP(VX,VY,EPS,INVLENMAX)  { float d2 = VX*VX + VY*VY; if (d2 > EPS)  { float inv_len = 1.0f / ImSqrt(d2); if (inv_len > INVLENMAX) inv_len = INVLENMAX; VX *= inv_len; VY *= inv_len; } }

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the ImVec2 math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddPolyline(const ImVec2 * points, const int points_count, ImU32 col, bool closed, float thickness)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    int count = points_count;
    if (!closed)
        count = points_count - 1;

    const bool thick_line = thickness > 1.0f;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;

        const int idx_count = thick_line ? count * 18 : count * 12;
        const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2)); //-V630
        ImVec2 * temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            float dx = points[i2].x - points[i1].x;
            float dy = points[i2].y - points[i1].y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i1].x = dy;
            temp_normals[i1].y = -dx;
        }
        if (!closed)
            temp_normals[points_count - 1] = temp_normals[points_count - 2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
                temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

                // Avehnly normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f)
                    dm_x *= AA_SIZE;
                dm_y *= AA_SIZE;

                // Add temporary vertexes
                ImVec2 * out_vtx = &temp_points[i2 * 2];
                out_vtx[0].x = points[i2].x + dm_x;
                out_vtx[0].y = points[i2].y + dm_y;
                out_vtx[1].x = points[i2].x - dm_x;
                out_vtx[1].y = points[i2].y - dm_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        } else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
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

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
                unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 4;

                // Avehnly normals
                float dm_x = (temp_normals[i1].x + temp_normals[i2].x) * 0.5f;
                float dm_y = (temp_normals[i1].y + temp_normals[i2].y) * 0.5f;
                IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f);
                float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
                float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
                float dm_in_x = dm_x * half_inner_thickness;
                float dm_in_y = dm_y * half_inner_thickness;

                // Add temporary vertexes
                ImVec2 * out_vtx = &temp_points[i2 * 4];
                out_vtx[0].x = points[i2].x + dm_out_x;
                out_vtx[0].y = points[i2].y + dm_out_y;
                out_vtx[1].x = points[i2].x + dm_in_x;
                out_vtx[1].y = points[i2].y + dm_in_y;
                out_vtx[2].x = points[i2].x - dm_in_x;
                out_vtx[2].y = points[i2].y - dm_in_y;
                out_vtx[3].x = points[i2].x - dm_out_x;
                out_vtx[3].y = points[i2].y - dm_out_y;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[1] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[2] = (ImDrawIdx)(idx1 + 2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[4] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[5] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2 + 1); _IdxWritePtr[7] = (ImDrawIdx)(idx1 + 1); _IdxWritePtr[8] = (ImDrawIdx)(idx1 + 0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1 + 0); _IdxWritePtr[10] = (ImDrawIdx)(idx2 + 0); _IdxWritePtr[11] = (ImDrawIdx)(idx2 + 1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2 + 2); _IdxWritePtr[13] = (ImDrawIdx)(idx1 + 2); _IdxWritePtr[14] = (ImDrawIdx)(idx1 + 3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1 + 3); _IdxWritePtr[16] = (ImDrawIdx)(idx2 + 3); _IdxWritePtr[17] = (ImDrawIdx)(idx2 + 2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count * 6;
        const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
            const ImVec2 & p1 = points[i1];
            const ImVec2 & p2 = points[i2];

            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            dx *= (thickness * 0.5f);
            dy *= (thickness * 0.5f);

            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + 2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx + 3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

// We intentionally avoid using ImVec2 and its math operators here to reduce cost to a minimum for debug/non-inlined builds.
void ImDrawList::AddConvexPolyFilled(const ImVec2 * points, const int points_count, ImU32 col)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;

    if (Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & ~IM_COL32_A_MASK;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2 * temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            IM_NORMALIZE2F_OVER_ZERO(dx, dy);
            temp_normals[i0].x = dy;
            temp_normals[i0].y = -dx;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Avehnly normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            float dm_x = (n0.x + n1.x) * 0.5f;
            float dm_y = (n0.y + n1.y) * 0.5f;
            IM_NORMALIZE2F_OVER_EPSILON_CLAMP(dm_x, dm_y, 0.000001f, 100.0f);
            dm_x *= AA_SIZE * 0.5f;
            dm_y *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos.x = (points[i1].x - dm_x); _VtxWritePtr[0].pos.y = (points[i1].y - dm_y); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos.x = (points[i1].x + dm_x); _VtxWritePtr[1].pos.y = (points[i1].y + dm_y); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    } else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx + i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2 & centre, float radius, int a_min_of_12, int a_max_of_12)
{
    if (radius == 0.0f || a_min_of_12 > a_max_of_12)
    {
        _Path.push_back(centre);
        return;
    }
    _Path.reserve(_Path.Size + (a_max_of_12 - a_min_of_12 + 1));
    for (int a = a_min_of_12; a <= a_max_of_12; a++)
    {
        const ImVec2& c = _Data->CircleVtx12[a % IM_ARRAYSIZE(_Data->CircleVtx12)];
        _Path.push_back(ImVec2(centre.x + c.x * radius, centre.y + c.y * radius));
    }
}

void ImDrawList::PathArcTo(const ImVec2 & centre, float radius, float a_min, float a_max, int num_segments)
{
    if (radius == 0.0f)
    {
        _Path.push_back(centre);
        return;
    }

    // Note that we are adding a point at both a_min and a_max.
    // If you are trying to draw a full closed circle you don't want the overlapping points!
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        _Path.push_back(ImVec2(centre.x + ImCos(a) * radius, centre.y + ImSin(a) * radius));
    }
}

static void PathBezierToCasteljau(ImVector<ImVec2> * path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy))
    {
        path->push_back(ImVec2(x4, y4));
    } else if (level < 10)
    {
        float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
        float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
        float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
        float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
        float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
        float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;

        PathBezierToCasteljau(path, x1, y1, x12, y12, x123, y123, x1234, y1234, tess_tol, level + 1);
        PathBezierToCasteljau(path, x1234, y1234, x234, y234, x34, y34, x4, y4, tess_tol, level + 1);
    }
}

void ImDrawList::PathBezierCurveTo(const ImVec2 & p2, const ImVec2 & p3, const ImVec2 & p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        // Auto-tessellated
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, _Data->CurveTessellationTol, 0);
    } else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
        {
            float t = t_step * i_step;
            float u = 1.0f - t;
            float w1 = u * u * u;
            float w2 = 3 * u * u * t;
            float w3 = 3 * u * t * t;
            float w4 = t * t * t;
            _Path.push_back(ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y));
        }
    }
}

void ImDrawList::PathRect(const ImVec2 & a, const ImVec2 & b, float rounding, int rounding_corners)
{
    rounding = ImMin(rounding, ImFabs(b.x - a.x) * (((rounding_corners & ImDrawCornerFlags_Top) == ImDrawCornerFlags_Top) || ((rounding_corners & ImDrawCornerFlags_Bot) == ImDrawCornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(b.y - a.y) * (((rounding_corners & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((rounding_corners & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

    if (rounding <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x, a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x, b.y));
    } else
    {
        const float rounding_tl = (rounding_corners & ImDrawCornerFlags_TopLeft) ? rounding : 0.0f;
        const float rounding_tr = (rounding_corners & ImDrawCornerFlags_TopRight) ? rounding : 0.0f;
        const float rounding_br = (rounding_corners & ImDrawCornerFlags_BotRight) ? rounding : 0.0f;
        const float rounding_bl = (rounding_corners & ImDrawCornerFlags_BotLeft) ? rounding : 0.0f;
        PathArcToFast(ImVec2(a.x + rounding_tl, a.y + rounding_tl), rounding_tl, 6, 9);
        PathArcToFast(ImVec2(b.x - rounding_tr, a.y + rounding_tr), rounding_tr, 9, 12);
        PathArcToFast(ImVec2(b.x - rounding_br, b.y - rounding_br), rounding_br, 0, 3);
        PathArcToFast(ImVec2(a.x + rounding_bl, b.y - rounding_bl), rounding_bl, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2 & a, const ImVec2 & b, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(a + ImVec2(0.5f, 0.5f));
    PathLineTo(b + ImVec2(0.5f, 0.5f));
    PathStroke(col, false, thickness);
}

// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
void ImDrawList::AddRect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (Flags & ImDrawListFlags_AntiAliasedLines)
        PathRect(a + ImVec2(0.5f, 0.5f), b - ImVec2(0.50f, 0.50f), rounding, rounding_corners_flags);
    else
        PathRect(a + ImVec2(0.5f, 0.5f), b - ImVec2(0.49f, 0.49f), rounding, rounding_corners_flags); // Better looking lower-right corner and rounded non-AA shapes.
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(a, b, rounding, rounding_corners_flags);
        PathFillConvex(col);
    } else
    {
        PrimReserve(6, 4);
        PrimRect(a, b, col);
    }
}

void ImDrawList::AddRectFilledMultiColor(const ImVec2 & a, const ImVec2 & c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = _Data->TexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx + 3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(ImVec2(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(ImVec2(a.x, c.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & d, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & d, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathFillConvex(col);
}

void ImDrawList::AddTriangle(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFillConvex(col);
}

void ImDrawList::AddCircle(const ImVec2 & centre, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius - 0.5f, 0.0f, a_max, num_segments - 1);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2 & centre, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    // Because we are filling a closed shape we remove 1 from the count of segments/points
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments - 1);
    PathFillConvex(col);
}

void ImDrawList::AddBezierCurve(const ImVec2 & pos0, const ImVec2 & cp0, const ImVec2 & cp1, const ImVec2 & pos1, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(pos0);
    PathBezierCurveTo(cp0, cp1, pos1, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont * font, float font_size, const ImVec2 & pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4 * cpu_fine_clip_rect)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Pull default font/size from the shared ImDrawListSharedData instance
    if (font == NULL)
        font = _Data->Font;
    if (font_size == 0.0f)
        font_size = _Data->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level vsonyp0wer::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    ImVec4 clip_rect = _ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL);
}

void ImDrawList::AddText(const ImVec2 & pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2 & a, const ImVec2 & b, const ImVec2 & uv_a, const ImVec2 & uv_b, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(a, b, uv_a, uv_b, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageQuad(ImTextureID user_texture_id, const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & d, const ImVec2 & uv_a, const ImVec2 & uv_b, const ImVec2 & uv_c, const ImVec2 & uv_d, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);

    if (push_texture_id)
        PopTextureID();
}

void ImDrawList::AddImageRounded(ImTextureID user_texture_id, const ImVec2 & a, const ImVec2 & b, const ImVec2 & uv_a, const ImVec2 & uv_b, ImU32 col, float rounding, int rounding_corners)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    if (rounding <= 0.0f || (rounding_corners & ImDrawCornerFlags_All) == 0)
    {
        AddImage(user_texture_id, a, b, uv_a, uv_b, col);
        return;
    }

    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    int vert_start_idx = VtxBuffer.Size;
    PathRect(a, b, rounding, rounding_corners);
    PathFillConvex(col);
    int vert_end_idx = VtxBuffer.Size;
    vsonyp0wer::ShadeVertsLinearUV(this, vert_start_idx, vert_end_idx, a, b, uv_a, uv_b, true);

    if (push_texture_id)
        PopTextureID();
}

//-----------------------------------------------------------------------------
// [SECTION] ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2 & fb_scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * fb_scale.x, cmd->ClipRect.y * fb_scale.y, cmd->ClipRect.z * fb_scale.x, cmd->ClipRect.w * fb_scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void vsonyp0wer::ShadeVertsLinearColorGradientKeepAlpha(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
        int r = ImLerp((int)(col0 >> IM_COL32_R_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF, t);
        int g = ImLerp((int)(col0 >> IM_COL32_G_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF, t);
        int b = ImLerp((int)(col0 >> IM_COL32_B_SHIFT) & 0xFF, (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF, t);
        vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
    }
}

// Distribute UV over (a, b) rectangle
void vsonyp0wer::ShadeVertsLinearUV(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, const ImVec2 & a, const ImVec2 & b, const ImVec2 & uv_a, const ImVec2 & uv_b, bool clamp)
{
    const ImVec2 size = b - a;
    const ImVec2 uv_size = uv_b - uv_a;
    const ImVec2 scale = ImVec2(
        size.x != 0.0f ? (uv_size.x / size.x) : 0.0f,
        size.y != 0.0f ? (uv_size.y / size.y) : 0.0f);

    ImDrawVert * vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert * vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
    if (clamp)
    {
        const ImVec2 min = ImMin(uv_a, uv_b);
        const ImVec2 max = ImMax(uv_a, uv_b);
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = ImClamp(uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale), min, max);
    } else
    {
        for (ImDrawVert* vertex = vert_start; vertex < vert_end; ++vertex)
            vertex->uv = uv_a + ImMul(ImVec2(vertex->pos.x, vertex->pos.y) - a, scale);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3; // FIXME: 2 may be a better default?
    OversampleV = 1;
    PixelSnapH = true;
    GlyphExtraSpacing = ImVec2(0.25f, 0.0f);
    GlyphOffset = ImVec2(0.15f, 0.0f);
    GlyphRanges = NULL;
    GlyphMinAdvanceX = 0.0f;
    GlyphMaxAdvanceX = FLT_MAX;
    MergeMode = false;
    RasterizerFlags = 0x00;
    RasterizerMultiply = 1.15f;
    memset(Name, 0, sizeof(Name));
    DstFont = NULL;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The white texels on the top left are the ones we'll use everywhere in vsonyp0wer to render filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF = 108;
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
const unsigned int FONT_ATLAS_DEFAULT_TEX_DATA_ID = 0x80000000;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
    "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
    "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
    "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
    "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
    "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
    "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
    "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
    "------------        -    X    -           X           -X.....................X-           ------------------"
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
    "                                                      -  X..X           X..X  -                             "
    "                                                      -   X.X           X.X   -                             "
    "                                                      -    XX           XX    -                             "
};

static const ImVec2 FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[vsonyp0werMouseCursor_COUNT][3] =
{
    // Pos ........ Size ......... Offset ......
    { ImVec2(0,3), ImVec2(12,19), ImVec2(0, 0) }, // vsonyp0werMouseCursor_Arrow
    { ImVec2(13,0), ImVec2(7,16), ImVec2(1, 8) }, // vsonyp0werMouseCursor_TextInput
    { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // vsonyp0werMouseCursor_ResizeAll
    { ImVec2(21,0), ImVec2(9,23), ImVec2(4,11) }, // vsonyp0werMouseCursor_ResizeNS
    { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 4) }, // vsonyp0werMouseCursor_ResizeEW
    { ImVec2(73,0), ImVec2(17,17), ImVec2(8, 8) }, // vsonyp0werMouseCursor_ResizeNESW
    { ImVec2(55,0), ImVec2(17,17), ImVec2(8, 8) }, // vsonyp0werMouseCursor_ResizeNWSE
    { ImVec2(91,0), ImVec2(17,22), ImVec2(5, 0) }, // vsonyp0werMouseCursor_Hand
};

ImFontAtlas::ImFontAtlas()
{
    Locked = false;
    Flags = ImFontAtlasFlags_None;
    TexID = (ImTextureID)NULL;
    TexDesiredWidth = 0;
    TexGlyphPadding = 1;

    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = 0;
    TexUvScale = ImVec2(0.0f, 0.0f);
    TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

ImFontAtlas::~ImFontAtlas()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            IM_FREE(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
    CustomRects.clear();
    for (int n = 0; n < IM_ARRAYSIZE(CustomRectIds); n++)
        CustomRectIds[n] = -1;
}

void    ImFontAtlas::ClearTexData()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    if (TexPixelsAlpha8)
        IM_FREE(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        IM_FREE(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    for (int i = 0; i < Fonts.Size; i++)
        IM_DELETE(Fonts[i]);
    Fonts.clear();
}

void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if (TexPixelsAlpha8 == NULL)
    {
        if (ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if (out_width)* out_width = TexWidth;
    if (out_height)* out_height = TexHeight;
    if (out_bytes_per_pixel)* out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels = NULL;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        if (pixels)
        {
            TexPixelsRGBA32 = (unsigned int*)IM_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = TexPixelsRGBA32;
            for (int n = TexWidth * TexHeight; n > 0; n--)
                * dst++ = IM_COL32(255, 255, 255, (unsigned int)(*src++));
        }
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width)* out_width = TexWidth;
    if (out_height)* out_height = TexHeight;
    if (out_bytes_per_pixel)* out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig * font_cfg)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
        Fonts.push_back(IM_NEW(ImFont));
    else
        IM_ASSERT(!Fonts.empty() && "Cannot use MergeMode for the first font"); // When using MergeMode make sure that a font has already been added before. You can use vsonyp0wer::GetIO().Fonts->AddFontDefault() to add the default vsonyp0wer font.

    ConfigData.push_back(*font_cfg);
    ImFontConfig & new_font_cfg = ConfigData.back();
    if (new_font_cfg.DstFont == NULL)
        new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = IM_ALLOC(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    // Invalidate texture
    ClearTexData();
    return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char* input);
static unsigned int stb_decompress(unsigned char* output, const unsigned char* input, unsigned int length);
static const char* GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c) { return c >= '\\' ? c - 36 : c - 35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianness.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont * ImFontAtlas::AddFontDefault(const ImFontConfig * font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.SizePixels <= 0.0f)
        font_cfg.SizePixels = 13.0f * 1.0f;
    if (font_cfg.Name[0] == '\0')
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "ProggyClean.ttf, %dpx", (int)font_cfg.SizePixels);

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    const ImWchar * glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges : GetGlyphRangesDefault();
    ImFont * font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, font_cfg.SizePixels, &font_cfg, glyph_ranges);
    font->DisplayOffset.y = 1.0f;
    return font;
}

ImFont * ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    size_t data_size = 0;
    void* data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT(0); // Could not load file.
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", p, size_pixels);
    }
    return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig * font_cfg_template, const ImWchar * glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char*)IM_ALLOC(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig * font_cfg, const ImWchar * glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = IM_ALLOC((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont * font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    IM_FREE(compressed_ttf);
    return font;
}

int ImFontAtlas::AddCustomRectRegular(unsigned int id, int width, int height)
{
    IM_ASSERT(id >= 0x10000);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    CustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

int ImFontAtlas::AddCustomRectFontGlyph(ImFont * font, ImWchar id, int width, int height, float advance_x, const ImVec2 & offset)
{
    IM_ASSERT(font != NULL);
    IM_ASSERT(width > 0 && width <= 0xFFFF);
    IM_ASSERT(height > 0 && height <= 0xFFFF);
    CustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphAdvanceX = advance_x;
    r.GlyphOffset = offset;
    r.Font = font;
    CustomRects.push_back(r);
    return CustomRects.Size - 1; // Return index
}

void ImFontAtlas::CalcCustomRectUV(const CustomRect * rect, ImVec2 * out_uv_min, ImVec2 * out_uv_max)
{
    IM_ASSERT(TexWidth > 0 && TexHeight > 0);   // Font atlas needs to be built before we can calculate UV coordinates
    IM_ASSERT(rect->IsPacked());                // Make sure the rectangle has been packed
    *out_uv_min = ImVec2((float)rect->X * TexUvScale.x, (float)rect->Y * TexUvScale.y);
    *out_uv_max = ImVec2((float)(rect->X + rect->Width) * TexUvScale.x, (float)(rect->Y + rect->Height) * TexUvScale.y);
}

bool ImFontAtlas::GetMouseCursorTexData(vsonyp0werMouseCursor cursor_type, ImVec2 * out_offset, ImVec2 * out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2])
{
    if (cursor_type <= vsonyp0werMouseCursor_None || cursor_type >= vsonyp0werMouseCursor_COUNT)
        return false;
    if (Flags & ImFontAtlasFlags_NoMouseCursors)
        return false;

    IM_ASSERT(CustomRectIds[0] != -1);
    ImFontAtlas::CustomRect & r = CustomRects[CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    ImVec2 pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] + ImVec2((float)r.X, (float)r.Y);
    ImVec2 size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
    *out_size = size;
    *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];
    out_uv_border[0] = (pos)* TexUvScale;
    out_uv_border[1] = (pos + size) * TexUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    out_uv_fill[0] = (pos)* TexUvScale;
    out_uv_fill[1] = (pos + size) * TexUvScale;
    return true;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(!Locked && "Cannot modify a locked ImFontAtlas between NewFrame() and EndFrame/Render()!");
    return ImFontAtlasBuildWithStbTruetype(this);
}

void    ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_brighten_factor)
{
    for (unsigned int i = 0; i < 256; i++)
    {
        unsigned int value = (unsigned int)(i * in_brighten_factor);
        out_table[i] = value > 255 ? 255 : (value & 0xFF);
    }
}

void    ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride)
{
    unsigned char* data = pixels + x + y * stride;
    for (int j = h; j > 0; j--, data += stride)
        for (int i = 0; i < w; i++)
            data[i] = table[data[i]];
}

// Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
// (C++03 doesn't allow instancing ImVector<> with function-local types so we declare the type here.)
struct ImFontBuildSrcData
{
    stbtt_fontinfo      FontInfo;
    stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
    stbrp_rect* Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
    stbtt_packedchar* PackedChars;        // Output glyphs
    const ImWchar* SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
    int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
    int                 GlyphsHighest;      // Highest requested codepoint
    int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
    ImBoolVector        GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
    ImVector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
struct ImFontBuildDstData
{
    int                 SrcCount;           // Number of source fonts targeting this destination font.
    int                 GlyphsHighest;
    int                 GlyphsCount;
    ImBoolVector        GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
};

static void UnpackBoolVectorToFlatIndexList(const ImBoolVector * in, ImVector<int> * out)
{
    IM_ASSERT(sizeof(in->Stohnly.Data[0]) == sizeof(int));
    const int* it_begin = in->Stohnly.begin();
    const int* it_end = in->Stohnly.end();
    for (const int* it = it_begin; it < it_end; it++)
        if (int entries_32 = *it)
            for (int bit_n = 0; bit_n < 32; bit_n++)
                if (entries_32 & (1 << bit_n))
                    out->push_back((int)((it - it_begin) << 5) + bit_n);
}

bool    ImFontAtlasBuildWithStbTruetype(ImFontAtlas * atlas)
{
    IM_ASSERT(atlas->ConfigData.Size > 0);

    ImFontAtlasBuildRegisterDefaultCustomRects(atlas);

    // Clear atlas
    atlas->TexID = (ImTextureID)NULL;
    atlas->TexWidth = atlas->TexHeight = 0;
    atlas->TexUvScale = ImVec2(0.0f, 0.0f);
    atlas->TexUvWhitePixel = ImVec2(0.0f, 0.0f);
    atlas->ClearTexData();

    // Temporary stohnly for building
    ImVector<ImFontBuildSrcData> src_tmp_array;
    ImVector<ImFontBuildDstData> dst_tmp_array;
    src_tmp_array.resize(atlas->ConfigData.Size);
    dst_tmp_array.resize(atlas->Fonts.Size);
    memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
    memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

    // 1. Initialize font loading structure, check font data validity
    for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

        // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual debugging nicer than when storing indices)
        src_tmp.DstIndex = -1;
        for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
            if (cfg.DstFont == atlas->Fonts[output_i])
                src_tmp.DstIndex = output_i;
        IM_ASSERT(src_tmp.DstIndex != -1); // cfg.DstFont not pointing within atlas->Fonts[] array?
        if (src_tmp.DstIndex == -1)
            return false;

        // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Measure highest codepoints
        ImFontBuildDstData & dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            src_tmp.GlyphsHighest = ImMax(src_tmp.GlyphsHighest, (int)src_range[1]);
        dst_tmp.SrcCount++;
        dst_tmp.GlyphsHighest = ImMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
    }

    // 2. For every requested codepoint, check for their presence in the font data, and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int total_glyphs_count = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        ImFontBuildDstData& dst_tmp = dst_tmp_array[src_tmp.DstIndex];
        src_tmp.GlyphsSet.Resize(src_tmp.GlyphsHighest + 1);
        if (dst_tmp.GlyphsSet.Stohnly.empty())
            dst_tmp.GlyphsSet.Resize(dst_tmp.GlyphsHighest + 1);

        for (const ImWchar* src_range = src_tmp.SrcRanges; src_range[0] && src_range[1]; src_range += 2)
            for (int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++)
            {
                if (dst_tmp.GlyphsSet.GetBit(codepoint))    // Don't overwrite existing glyphs. We could make this an option for MergeMode (e.g. MergeOverwrite==true)
                    continue;
                if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))    // It is actually in the font?
                    continue;

                // Add to avail set/counters
                src_tmp.GlyphsCount++;
                dst_tmp.GlyphsCount++;
                src_tmp.GlyphsSet.SetBit(codepoint, true);
                dst_tmp.GlyphsSet.SetBit(codepoint, true);
                total_glyphs_count++;
            }
    }

    // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are requested _and_ available _and_ not overlapping another)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
        UnpackBoolVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
        src_tmp.GlyphsSet.Clear();
        IM_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
    }
    for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
        dst_tmp_array[dst_i].GlyphsSet.Clear();
    dst_tmp_array.clear();

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    // (We technically don't need to zero-clear buf_rects, but let's do it for the sake of sanity)
    ImVector<stbrp_rect> buf_rects;
    ImVector<stbtt_packedchar> buf_packedchars;
    buf_rects.resize(total_glyphs_count);
    buf_packedchars.resize(total_glyphs_count);
    memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
    memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

    // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
    int total_surface = 0;
    int buf_rects_out_n = 0;
    int buf_packedchars_out_n = 0;
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        src_tmp.Rects = &buf_rects[buf_rects_out_n];
        src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
        buf_rects_out_n += src_tmp.GlyphsCount;
        buf_packedchars_out_n += src_tmp.GlyphsCount;

        // Convert our ranges in the format stb_truetype wants
        ImFontConfig & cfg = atlas->ConfigData[src_i];
        src_tmp.PackRange.font_size = cfg.SizePixels;
        src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
        src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
        src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
        src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
        src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
        src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

        // Gather the sizes of all rectangles we will need to pack (this loop is based on stbtt_PackFontRangesGatherRects)
        const float scale = (cfg.SizePixels > 0) ? stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) : stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
        const int padding = atlas->TexGlyphPadding;
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++)
        {
            int x0, y0, x1, y1;
            const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo, src_tmp.GlyphsList[glyph_i]);
            IM_ASSERT(glyph_index_in_font != 0);
            stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo, glyph_index_in_font, scale * cfg.OversampleH, scale * cfg.OversampleV, 0, 0, &x0, &y0, &x1, &y1);
            src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
            src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
            total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
        }
    }

    // We need a width for the skyline algorithm, any width!
    // The exact width doesn't really matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    // User can override TexDesiredWidth and TexGlyphPadding if they wish, otherwise we use a simple heuristic to select the width based on expected surface.
    const int surface_sqrt = (int)ImSqrt((float)total_surface) + 1;
    atlas->TexHeight = 0;
    if (atlas->TexDesiredWidth > 0)
        atlas->TexWidth = atlas->TexDesiredWidth;
    else
        atlas->TexWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 : (surface_sqrt >= 2048 * 0.7f) ? 2048 : (surface_sqrt >= 1024 * 0.7f) ? 1024 : 512;

    // 5. Start packing
    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    const int TEX_HEIGHT_MAX = 1024 * 32;
    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
    ImFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

    // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely tall texture at this point.
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbrp_pack_rects((stbrp_context*)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

        // Extend texture height and mark missing glyphs as non-packed so we won't render them.
        // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX or if a single if larger than TexWidth?)
        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
            if (src_tmp.Rects[glyph_i].was_packed)
                atlas->TexHeight = ImMax(atlas->TexHeight, src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
    }

    // 7. Allocate texture
    atlas->TexHeight = (atlas->Flags & ImFontAtlasFlags_NoPowerOfTwoHeight) ? (atlas->TexHeight + 1) : ImUpperPowerOfTwo(atlas->TexHeight);
    atlas->TexUvScale = ImVec2(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
    atlas->TexPixelsAlpha8 = (unsigned char*)IM_ALLOC(atlas->TexWidth * atlas->TexHeight);
    memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
    spc.pixels = atlas->TexPixelsAlpha8;
    spc.height = atlas->TexHeight;

    // 8. Render/rasterize font characters into the texture
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontConfig& cfg = atlas->ConfigData[src_i];
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        stbtt_PackFontRangesRenderIntoRects(&spc, &src_tmp.FontInfo, &src_tmp.PackRange, 1, src_tmp.Rects);

        // Apply multiply operator
        if (cfg.RasterizerMultiply != 1.0f)
        {
            unsigned char multiply_table[256];
            ImFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
            stbrp_rect* r = &src_tmp.Rects[0];
            for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
                if (r->was_packed)
                    ImFontAtlasBuildMultiplyRectAlpha8(multiply_table, atlas->TexPixelsAlpha8, r->x, r->y, r->w, r->h, atlas->TexWidth * 1);
        }
        src_tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    buf_rects.clear();

    // 9. Setup ImFont and glyphs for runtime
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    {
        ImFontBuildSrcData& src_tmp = src_tmp_array[src_i];
        if (src_tmp.GlyphsCount == 0)
            continue;

        ImFontConfig & cfg = atlas->ConfigData[src_i];
        ImFont * dst_font = cfg.DstFont; // We can have multiple input fonts writing into a same destination font (when using MergeMode=true)

        const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&src_tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        const float ascent = ImFloor(unscaled_ascent * font_scale + ((unscaled_ascent > 0.0f) ? +1 : -1));
        const float descent = ImFloor(unscaled_descent * font_scale + ((unscaled_descent > 0.0f) ? +1 : -1));
        ImFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
        const float font_off_x = cfg.GlyphOffset.x;
        const float font_off_y = cfg.GlyphOffset.y + (float)(int)(dst_font->Ascent + 0.5f);

        for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
        {
            const int codepoint = src_tmp.GlyphsList[glyph_i];
            const stbtt_packedchar& pc = src_tmp.PackedChars[glyph_i];

            const float char_advance_x_org = pc.xadvance;
            const float char_advance_x_mod = ImClamp(char_advance_x_org, cfg.GlyphMinAdvanceX, cfg.GlyphMaxAdvanceX);
            float char_off_x = font_off_x;
            if (char_advance_x_org != char_advance_x_mod)
                char_off_x += cfg.PixelSnapH ? (float)(int)((char_advance_x_mod - char_advance_x_org) * 0.5f) : (char_advance_x_mod - char_advance_x_org) * 0.5f;

            // Register glyph
            stbtt_aligned_quad q;
            float dummy_x = 0.0f, dummy_y = 0.0f;
            stbtt_GetPackedQuad(src_tmp.PackedChars, atlas->TexWidth, atlas->TexHeight, glyph_i, &dummy_x, &dummy_y, &q, 0);
            dst_font->AddGlyph((ImWchar)codepoint, q.x0 + char_off_x, q.y0 + font_off_y, q.x1 + char_off_x, q.y1 + font_off_y, q.s0, q.t0, q.s1, q.t1, char_advance_x_mod);
        }
    }

    // Cleanup temporary (ImVector doesn't honor destructor)
    for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
        src_tmp_array[src_i].~ImFontBuildSrcData();

    ImFontAtlasBuildFinish(atlas);
    return true;
}

void ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas * atlas)
{
    if (atlas->CustomRectIds[0] >= 0)
        return;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1, FONT_ATLAS_DEFAULT_TEX_DATA_H);
    else
        atlas->CustomRectIds[0] = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_ID, 2, 2);
}

void ImFontAtlasBuildSetupFont(ImFontAtlas * atlas, ImFont * font, ImFontConfig * font_config, float ascent, float descent)
{
    if (!font_config->MergeMode)
    {
        font->ClearOutputData();
        font->FontSize = font_config->SizePixels;
        font->ConfigData = font_config;
        font->ContainerAtlas = atlas;
        font->Ascent = ascent;
        font->Descent = descent;
    }
    font->ConfigDataCount++;
}

void ImFontAtlasBuildPackCustomRects(ImFontAtlas * atlas, void* stbrp_context_opaque)
{
    stbrp_context* pack_context = (stbrp_context*)stbrp_context_opaque;
    IM_ASSERT(pack_context != NULL);

    ImVector<ImFontAtlas::CustomRect> & user_rects = atlas->CustomRects;
    IM_ASSERT(user_rects.Size >= 1); // We expect at least the default custom rects to be registered, else something went wrong.

    ImVector<stbrp_rect> pack_rects;
    pack_rects.resize(user_rects.Size);
    memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
    for (int i = 0; i < user_rects.Size; i++)
    {
        pack_rects[i].w = user_rects[i].Width;
        pack_rects[i].h = user_rects[i].Height;
    }
    stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
    for (int i = 0; i < pack_rects.Size; i++)
        if (pack_rects[i].was_packed)
        {
            user_rects[i].X = pack_rects[i].x;
            user_rects[i].Y = pack_rects[i].y;
            IM_ASSERT(pack_rects[i].w == user_rects[i].Width && pack_rects[i].h == user_rects[i].Height);
            atlas->TexHeight = ImMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
        }
}

static void ImFontAtlasBuildRenderDefaultTexData(ImFontAtlas * atlas)
{
    IM_ASSERT(atlas->CustomRectIds[0] >= 0);
    IM_ASSERT(atlas->TexPixelsAlpha8 != NULL);
    ImFontAtlas::CustomRect & r = atlas->CustomRects[atlas->CustomRectIds[0]];
    IM_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID);
    IM_ASSERT(r.IsPacked());

    const int w = atlas->TexWidth;
    if (!(atlas->Flags & ImFontAtlasFlags_NoMouseCursors))
    {
        // Render/copy pixels
        IM_ASSERT(r.Width == FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1 && r.Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
        for (int y = 0, n = 0; y < FONT_ATLAS_DEFAULT_TEX_DATA_H; y++)
            for (int x = 0; x < FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF; x++, n++)
            {
                const int offset0 = (int)(r.X + x) + (int)(r.Y + y) * w;
                const int offset1 = offset0 + FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
                atlas->TexPixelsAlpha8[offset0] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == '.' ? 0xFF : 0x00;
                atlas->TexPixelsAlpha8[offset1] = FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[n] == 'X' ? 0xFF : 0x00;
            }
    } else
    {
        IM_ASSERT(r.Width == 2 && r.Height == 2);
        const int offset = (int)(r.X) + (int)(r.Y) * w;
        atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] = atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
    }
    atlas->TexUvWhitePixel = ImVec2((r.X + 0.5f) * atlas->TexUvScale.x, (r.Y + 0.5f) * atlas->TexUvScale.y);
}

void ImFontAtlasBuildFinish(ImFontAtlas * atlas)
{
    // Render into our custom data block
    ImFontAtlasBuildRenderDefaultTexData(atlas);

    // Register custom rectangle glyphs
    for (int i = 0; i < atlas->CustomRects.Size; i++)
    {
        const ImFontAtlas::CustomRect& r = atlas->CustomRects[i];
        if (r.Font == NULL || r.ID > 0x10000)
            continue;

        IM_ASSERT(r.Font->ContainerAtlas == atlas);
        ImVec2 uv0, uv1;
        atlas->CalcCustomRectUV(&r, &uv0, &uv1);
        r.Font->AddGlyph((ImWchar)r.ID, r.GlyphOffset.x, r.GlyphOffset.y, r.GlyphOffset.x + r.Width, r.GlyphOffset.y + r.Height, uv0.x, uv0.y, uv1.x, uv1.y, r.GlyphAdvanceX);
    }

    // Build all fonts lookup tables
    for (int i = 0; i < atlas->Fonts.Size; i++)
        if (atlas->Fonts[i]->DirtyLookupTables)
            atlas->Fonts[i]->BuildLookupTable();
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar * ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2122, 0x2122, // �
		0xC2, 0xB0, // �
        0,
    };
    return &ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD79D, // Korean characters
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
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint, const short* accumulative_offsets, int accumulative_offsets_count, ImWchar * out_ranges)
{
    for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2)
    {
        out_ranges[0] = out_ranges[1] = (ImWchar)(base_codepoint + accumulative_offsets[n]);
        base_codepoint += accumulative_offsets[n];
    }
    out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] ImFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const ImWchar* ImFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
    // Store 2500 regularly used characters for Simplified Chinese.
    // Sourced from https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
    // This table covers 97.97% of all characters used during the month in July, 1987.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,3,2,1,2,2,1,1,1,1,1,5,2,1,2,3,3,3,2,2,4,1,1,1,2,1,5,2,3,1,2,1,2,1,1,2,1,1,2,2,1,4,1,1,1,1,5,10,1,2,19,2,1,2,1,2,1,2,1,2,
        1,5,1,6,3,2,1,2,2,1,1,1,4,8,5,1,1,4,1,1,3,1,2,1,5,1,2,1,1,1,10,1,1,5,2,4,6,1,4,2,2,2,12,2,1,1,6,1,1,1,4,1,1,4,6,5,1,4,2,2,4,10,7,1,1,4,2,4,
        2,1,4,3,6,10,12,5,7,2,14,2,9,1,1,6,7,10,4,7,13,1,5,4,8,4,1,1,2,28,5,6,1,1,5,2,5,20,2,2,9,8,11,2,9,17,1,8,6,8,27,4,6,9,20,11,27,6,68,2,2,1,1,
        1,2,1,2,2,7,6,11,3,3,1,1,3,1,2,1,1,1,1,1,3,1,1,8,3,4,1,5,7,2,1,4,4,8,4,2,1,2,1,1,4,5,6,3,6,2,12,3,1,3,9,2,4,3,4,1,5,3,3,1,3,7,1,5,1,1,1,1,2,
        3,4,5,2,3,2,6,1,1,2,1,7,1,7,3,4,5,15,2,2,1,5,3,22,19,2,1,1,1,1,2,5,1,1,1,6,1,1,12,8,2,9,18,22,4,1,1,5,1,16,1,2,7,10,15,1,1,6,2,4,1,2,4,1,6,
        1,1,3,2,4,1,6,4,5,1,2,1,1,2,1,10,3,1,3,2,1,9,3,2,5,7,2,19,4,3,6,1,1,1,1,1,4,3,2,1,1,1,2,5,3,1,1,1,2,2,1,1,2,1,1,2,1,3,1,1,1,3,7,1,4,1,1,2,1,
        1,2,1,2,4,4,3,8,1,1,1,2,1,3,5,1,3,1,3,4,6,2,2,14,4,6,6,11,9,1,15,3,1,28,5,2,5,5,3,1,3,4,5,4,6,14,3,2,3,5,21,2,7,20,10,1,2,19,2,4,28,28,2,3,
        2,1,14,4,1,26,28,42,12,40,3,52,79,5,14,17,3,2,2,11,3,4,6,3,1,8,2,23,4,5,8,10,4,2,7,3,5,1,1,6,3,1,2,2,2,5,28,1,1,7,7,20,5,3,29,3,17,26,1,8,4,
        27,3,6,11,23,5,3,4,6,13,24,16,6,5,10,25,35,7,3,2,3,3,14,3,6,2,6,1,4,2,3,8,2,1,1,3,3,3,4,1,1,13,2,2,4,5,2,1,14,14,1,2,2,1,4,5,2,3,1,14,3,12,
        3,17,2,16,5,1,2,1,8,9,3,19,4,2,2,4,17,25,21,20,28,75,1,10,29,103,4,1,2,1,1,4,2,4,1,2,3,24,2,2,2,1,1,2,1,3,8,1,1,1,2,1,1,3,1,1,1,6,1,5,3,1,1,
        1,3,4,1,1,5,2,1,5,6,13,9,16,1,1,1,1,3,2,3,2,4,5,2,5,2,2,3,7,13,7,2,2,1,1,1,1,2,3,3,2,1,6,4,9,2,1,14,2,14,2,1,18,3,4,14,4,11,41,15,23,15,23,
        176,1,3,4,1,1,1,1,5,3,1,2,3,7,3,1,1,2,1,2,4,4,6,2,4,1,9,7,1,10,5,8,16,29,1,1,2,2,3,1,3,5,2,4,5,4,1,1,2,2,3,3,7,1,6,10,1,17,1,44,4,6,2,1,1,6,
        5,4,2,10,1,6,9,2,8,1,24,1,2,13,7,8,8,2,1,4,1,3,1,3,3,5,2,5,10,9,4,9,12,2,1,6,1,10,1,1,7,7,4,10,8,3,1,13,4,3,1,6,1,3,5,2,1,2,17,16,5,2,16,6,
        1,4,2,1,3,3,6,8,5,11,11,1,3,3,2,4,6,10,9,5,7,4,7,4,7,1,1,4,2,1,3,6,8,7,1,6,11,5,5,3,24,9,4,2,7,13,5,1,8,82,16,61,1,1,1,4,2,2,16,10,3,8,1,1,
        6,4,2,1,3,1,1,1,4,3,8,4,2,2,1,1,1,1,1,6,3,5,1,1,4,6,9,2,1,1,1,2,1,7,2,1,6,1,5,4,4,3,1,8,1,3,3,1,3,2,2,2,2,3,1,6,1,2,1,2,1,3,7,1,8,2,1,2,1,5,
        2,5,3,5,10,1,2,1,1,3,2,5,11,3,9,3,5,1,1,5,9,1,2,1,5,7,9,9,8,1,3,3,3,6,8,2,3,2,1,1,32,6,1,2,15,9,3,7,13,1,3,10,13,2,14,1,13,10,2,1,3,10,4,15,
        2,15,15,10,1,3,9,6,9,32,25,26,47,7,3,2,3,1,6,3,4,3,2,8,5,4,1,9,4,2,2,19,10,6,2,3,8,1,2,2,4,2,1,9,4,4,4,6,4,8,9,2,3,1,1,1,1,3,5,5,1,3,8,4,6,
        2,1,4,12,1,5,3,7,13,2,5,8,1,6,1,2,5,14,6,1,5,2,4,8,15,5,1,23,6,62,2,10,1,1,8,1,2,2,10,4,2,2,9,2,1,1,3,2,3,1,5,3,3,2,1,3,8,1,1,1,11,3,1,1,4,
        3,7,1,14,1,2,3,12,5,2,5,1,6,7,5,7,14,11,1,3,1,8,9,12,2,1,11,8,4,4,2,6,10,9,13,1,1,3,1,5,1,3,2,4,4,1,18,2,3,14,11,4,29,4,2,7,1,3,13,9,2,2,5,
        3,5,20,7,16,8,5,72,34,6,4,22,12,12,28,45,36,9,7,39,9,191,1,1,1,4,11,8,4,9,2,3,22,1,1,1,1,4,17,1,7,7,1,11,31,10,2,4,8,2,3,2,1,4,2,16,4,32,2,
        3,19,13,4,9,1,5,2,14,8,1,1,3,6,19,6,5,1,16,6,2,10,8,5,1,2,3,1,5,5,1,11,6,6,1,3,3,2,6,3,8,1,1,4,10,7,5,7,7,5,8,9,2,1,3,4,1,1,3,1,3,3,2,6,16,
        1,4,6,3,1,10,6,1,3,15,2,9,2,10,25,13,9,16,6,2,2,10,11,4,3,9,1,2,6,6,5,4,30,40,1,10,7,12,14,33,6,3,6,7,3,1,3,1,11,14,4,9,5,12,11,49,18,51,31,
        140,31,2,2,1,5,1,8,1,10,1,4,4,3,24,1,10,1,3,6,6,16,3,4,5,2,1,4,2,57,10,6,22,2,22,3,7,22,6,10,11,36,18,16,33,36,2,5,5,1,1,1,4,10,1,4,13,2,7,
        5,2,9,3,4,1,7,43,3,7,3,9,14,7,9,1,11,1,1,3,7,4,18,13,1,14,1,3,6,10,73,2,2,30,6,1,11,18,19,13,22,3,46,42,37,89,7,3,16,34,2,2,3,9,1,7,1,1,1,2,
        2,4,10,7,3,10,3,9,5,28,9,2,6,13,7,3,1,3,10,2,7,2,11,3,6,21,54,85,2,1,4,2,2,1,39,3,21,2,2,5,1,1,1,4,1,1,3,4,15,1,3,2,4,4,2,3,8,2,20,1,8,7,13,
        4,1,26,6,2,9,34,4,21,52,10,4,4,1,5,12,2,11,1,7,2,30,12,44,2,30,1,1,3,6,16,9,17,39,82,2,2,24,7,1,7,3,16,9,14,44,2,1,2,1,2,3,5,2,4,1,6,7,5,3,
        2,6,1,11,5,11,2,1,18,19,8,1,3,24,29,2,1,3,5,2,2,1,13,6,5,1,46,11,3,5,1,1,5,8,2,10,6,12,6,3,7,11,2,4,16,13,2,5,1,1,2,2,5,2,28,5,2,23,10,8,4,
        4,22,39,95,38,8,14,9,5,1,13,5,4,3,13,12,11,1,9,1,27,37,2,5,4,4,63,211,95,2,2,2,1,3,5,2,1,1,2,2,1,1,1,3,2,4,1,2,1,1,5,2,2,1,1,2,3,1,3,1,1,1,
        3,1,4,2,1,3,6,1,1,3,7,15,5,3,2,5,3,9,11,4,2,22,1,6,3,8,7,1,4,28,4,16,3,3,25,4,4,27,27,1,4,1,2,2,7,1,3,5,2,28,8,2,14,1,8,6,16,25,3,3,3,14,3,
        3,1,1,2,1,4,6,3,8,4,1,1,1,2,3,6,10,6,2,3,18,3,2,5,5,4,3,1,5,2,5,4,23,7,6,12,6,4,17,11,9,5,1,1,10,5,12,1,1,11,26,33,7,3,6,1,17,7,1,5,12,1,11,
        2,4,1,8,14,17,23,1,2,1,7,8,16,11,9,6,5,2,6,4,16,2,8,14,1,11,8,9,1,1,1,9,25,4,11,19,7,2,15,2,12,8,52,7,5,19,2,16,4,36,8,1,16,8,24,26,4,6,2,9,
        5,4,36,3,28,12,25,15,37,27,17,12,59,38,5,32,127,1,2,9,17,14,4,1,2,1,1,8,11,50,4,14,2,19,16,4,17,5,4,5,26,12,45,2,23,45,104,30,12,8,3,10,2,2,
        3,3,1,4,20,7,2,9,6,15,2,20,1,3,16,4,11,15,6,134,2,5,59,1,2,2,2,1,9,17,3,26,137,10,211,59,1,2,4,1,4,1,1,1,2,6,2,3,1,1,2,3,2,3,1,3,4,4,2,3,3,
        1,4,3,1,7,2,2,3,1,2,1,3,3,3,2,2,3,2,1,3,14,6,1,3,2,9,6,15,27,9,34,145,1,1,2,1,1,1,1,2,1,1,1,1,2,2,2,3,1,2,1,1,1,2,3,5,8,3,5,2,4,1,3,2,2,2,12,
        4,1,1,1,10,4,5,1,20,4,16,1,15,9,5,12,2,9,2,5,4,2,26,19,7,1,26,4,30,12,15,42,1,6,8,172,1,1,4,2,1,1,11,2,2,4,2,1,2,1,10,8,1,2,1,4,5,1,2,5,1,8,
        4,1,3,4,2,1,6,2,1,3,4,1,2,1,1,1,1,12,5,7,2,4,3,1,1,1,3,3,6,1,2,2,3,3,3,2,1,2,12,14,11,6,6,4,12,2,8,1,7,10,1,35,7,4,13,15,4,3,23,21,28,52,5,
        26,5,6,1,7,10,2,7,53,3,2,1,1,1,2,163,532,1,10,11,1,3,3,4,8,2,8,6,2,2,23,22,4,2,2,4,2,1,3,1,3,3,5,9,8,2,1,2,8,1,10,2,12,21,20,15,105,2,3,1,1,
        3,2,3,1,1,2,5,1,4,15,11,19,1,1,1,1,5,4,5,1,1,2,5,3,5,12,1,2,5,1,11,1,1,15,9,1,4,5,3,26,8,2,1,3,1,1,15,19,2,12,1,2,5,2,7,2,19,2,20,6,26,7,5,
        2,2,7,34,21,13,70,2,128,1,1,2,1,1,2,1,1,3,2,2,2,15,1,4,1,3,4,42,10,6,1,49,85,8,1,2,1,1,4,4,2,3,6,1,5,7,4,3,211,4,1,2,1,2,5,1,2,4,2,2,6,5,6,
        10,3,4,48,100,6,2,16,296,5,27,387,2,2,3,7,16,8,5,38,15,39,21,9,10,3,7,59,13,27,21,47,5,21,6
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x2000, 0x206F, // General Punctuation
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
}

const ImWchar* ImFontAtlas::GetGlyphRangesJapanese()
{
    // 1946 common ideograms code points for Japanese
    // Sourced from http://theinstructionlimit.com/common-kanji-character-ranges-for-xna-spritefont-rendering
    // FIXME: Source a list of the revised 2136 Joyo Kanji list from 2010 and rebuild this.
    // You can use ImFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing ranges or adding new characters.
    // (Stored as accumulative offsets from the initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
    static const short accumulative_offsets_from_0x4E00[] =
    {
        0,1,2,4,1,1,1,1,2,1,6,2,2,1,8,5,7,11,1,2,10,10,8,2,4,20,2,11,8,2,1,2,1,6,2,1,7,5,3,7,1,1,13,7,9,1,4,6,1,2,1,10,1,1,9,2,2,4,5,6,14,1,1,9,3,18,
        5,4,2,2,10,7,1,1,1,3,2,4,3,23,2,10,12,2,14,2,4,13,1,6,10,3,1,7,13,6,4,13,5,2,3,17,2,2,5,7,6,4,1,7,14,16,6,13,9,15,1,1,7,16,4,7,1,19,9,2,7,15,
        2,6,5,13,25,4,14,13,11,25,1,1,1,2,1,2,2,3,10,11,3,3,1,1,4,4,2,1,4,9,1,4,3,5,5,2,7,12,11,15,7,16,4,5,16,2,1,1,6,3,3,1,1,2,7,6,6,7,1,4,7,6,1,1,
        2,1,12,3,3,9,5,8,1,11,1,2,3,18,20,4,1,3,6,1,7,3,5,5,7,2,2,12,3,1,4,2,3,2,3,11,8,7,4,17,1,9,25,1,1,4,2,2,4,1,2,7,1,1,1,3,1,2,6,16,1,2,1,1,3,12,
        20,2,5,20,8,7,6,2,1,1,1,1,6,2,1,2,10,1,1,6,1,3,1,2,1,4,1,12,4,1,3,1,1,1,1,1,10,4,7,5,13,1,15,1,1,30,11,9,1,15,38,14,1,32,17,20,1,9,31,2,21,9,
        4,49,22,2,1,13,1,11,45,35,43,55,12,19,83,1,3,2,3,13,2,1,7,3,18,3,13,8,1,8,18,5,3,7,25,24,9,24,40,3,17,24,2,1,6,2,3,16,15,6,7,3,12,1,9,7,3,3,
        3,15,21,5,16,4,5,12,11,11,3,6,3,2,31,3,2,1,1,23,6,6,1,4,2,6,5,2,1,1,3,3,22,2,6,2,3,17,3,2,4,5,1,9,5,1,1,6,15,12,3,17,2,14,2,8,1,23,16,4,2,23,
        8,15,23,20,12,25,19,47,11,21,65,46,4,3,1,5,6,1,2,5,26,2,1,1,3,11,1,1,1,2,1,2,3,1,1,10,2,3,1,1,1,3,6,3,2,2,6,6,9,2,2,2,6,2,5,10,2,4,1,2,1,2,2,
        3,1,1,3,1,2,9,23,9,2,1,1,1,1,5,3,2,1,10,9,6,1,10,2,31,25,3,7,5,40,1,15,6,17,7,27,180,1,3,2,2,1,1,1,6,3,10,7,1,3,6,17,8,6,2,2,1,3,5,5,8,16,14,
        15,1,1,4,1,2,1,1,1,3,2,7,5,6,2,5,10,1,4,2,9,1,1,11,6,1,44,1,3,7,9,5,1,3,1,1,10,7,1,10,4,2,7,21,15,7,2,5,1,8,3,4,1,3,1,6,1,4,2,1,4,10,8,1,4,5,
        1,5,10,2,7,1,10,1,1,3,4,11,10,29,4,7,3,5,2,3,33,5,2,19,3,1,4,2,6,31,11,1,3,3,3,1,8,10,9,12,11,12,8,3,14,8,6,11,1,4,41,3,1,2,7,13,1,5,6,2,6,12,
        12,22,5,9,4,8,9,9,34,6,24,1,1,20,9,9,3,4,1,7,2,2,2,6,2,28,5,3,6,1,4,6,7,4,2,1,4,2,13,6,4,4,3,1,8,8,3,2,1,5,1,2,2,3,1,11,11,7,3,6,10,8,6,16,16,
        22,7,12,6,21,5,4,6,6,3,6,1,3,2,1,2,8,29,1,10,1,6,13,6,6,19,31,1,13,4,4,22,17,26,33,10,4,15,12,25,6,67,10,2,3,1,6,10,2,6,2,9,1,9,4,4,1,2,16,2,
        5,9,2,3,8,1,8,3,9,4,8,6,4,8,11,3,2,1,1,3,26,1,7,5,1,11,1,5,3,5,2,13,6,39,5,1,5,2,11,6,10,5,1,15,5,3,6,19,21,22,2,4,1,6,1,8,1,4,8,2,4,2,2,9,2,
        1,1,1,4,3,6,3,12,7,1,14,2,4,10,2,13,1,17,7,3,2,1,3,2,13,7,14,12,3,1,29,2,8,9,15,14,9,14,1,3,1,6,5,9,11,3,38,43,20,7,7,8,5,15,12,19,15,81,8,7,
        1,5,73,13,37,28,8,8,1,15,18,20,165,28,1,6,11,8,4,14,7,15,1,3,3,6,4,1,7,14,1,1,11,30,1,5,1,4,14,1,4,2,7,52,2,6,29,3,1,9,1,21,3,5,1,26,3,11,14,
        11,1,17,5,1,2,1,3,2,8,1,2,9,12,1,1,2,3,8,3,24,12,7,7,5,17,3,3,3,1,23,10,4,4,6,3,1,16,17,22,3,10,21,16,16,6,4,10,2,1,1,2,8,8,6,5,3,3,3,39,25,
        15,1,1,16,6,7,25,15,6,6,12,1,22,13,1,4,9,5,12,2,9,1,12,28,8,3,5,10,22,60,1,2,40,4,61,63,4,1,13,12,1,4,31,12,1,14,89,5,16,6,29,14,2,5,49,18,18,
        5,29,33,47,1,17,1,19,12,2,9,7,39,12,3,7,12,39,3,1,46,4,12,3,8,9,5,31,15,18,3,2,2,66,19,13,17,5,3,46,124,13,57,34,2,5,4,5,8,1,1,1,4,3,1,17,5,
        3,5,3,1,8,5,6,3,27,3,26,7,12,7,2,17,3,7,18,78,16,4,36,1,2,1,6,2,1,39,17,7,4,13,4,4,4,1,10,4,2,4,6,3,10,1,19,1,26,2,4,33,2,73,47,7,3,8,2,4,15,
        18,1,29,2,41,14,1,21,16,41,7,39,25,13,44,2,2,10,1,13,7,1,7,3,5,20,4,8,2,49,1,10,6,1,6,7,10,7,11,16,3,12,20,4,10,3,1,2,11,2,28,9,2,4,7,2,15,1,
        27,1,28,17,4,5,10,7,3,24,10,11,6,26,3,2,7,2,2,49,16,10,16,15,4,5,27,61,30,14,38,22,2,7,5,1,3,12,23,24,17,17,3,3,2,4,1,6,2,7,5,1,1,5,1,1,9,4,
        1,3,6,1,8,2,8,4,14,3,5,11,4,1,3,32,1,19,4,1,13,11,5,2,1,8,6,8,1,6,5,13,3,23,11,5,3,16,3,9,10,1,24,3,198,52,4,2,2,5,14,5,4,22,5,20,4,11,6,41,
        1,5,2,2,11,5,2,28,35,8,22,3,18,3,10,7,5,3,4,1,5,3,8,9,3,6,2,16,22,4,5,5,3,3,18,23,2,6,23,5,27,8,1,33,2,12,43,16,5,2,3,6,1,20,4,2,9,7,1,11,2,
        10,3,14,31,9,3,25,18,20,2,5,5,26,14,1,11,17,12,40,19,9,6,31,83,2,7,9,19,78,12,14,21,76,12,113,79,34,4,1,1,61,18,85,10,2,2,13,31,11,50,6,33,159,
        179,6,6,7,4,4,2,4,2,5,8,7,20,32,22,1,3,10,6,7,28,5,10,9,2,77,19,13,2,5,1,4,4,7,4,13,3,9,31,17,3,26,2,6,6,5,4,1,7,11,3,4,2,1,6,2,20,4,1,9,2,6,
        3,7,1,1,1,20,2,3,1,6,2,3,6,2,4,8,1,5,13,8,4,11,23,1,10,6,2,1,3,21,2,2,4,24,31,4,10,10,2,5,192,15,4,16,7,9,51,1,2,1,1,5,1,1,2,1,3,5,3,1,3,4,1,
        3,1,3,3,9,8,1,2,2,2,4,4,18,12,92,2,10,4,3,14,5,25,16,42,4,14,4,2,21,5,126,30,31,2,1,5,13,3,22,5,6,6,20,12,1,14,12,87,3,19,1,8,2,9,9,3,3,23,2,
        3,7,6,3,1,2,3,9,1,3,1,6,3,2,1,3,11,3,1,6,10,3,2,3,1,2,1,5,1,1,11,3,6,4,1,7,2,1,2,5,5,34,4,14,18,4,19,7,5,8,2,6,79,1,5,2,14,8,2,9,2,1,36,28,16,
        4,1,1,1,2,12,6,42,39,16,23,7,15,15,3,2,12,7,21,64,6,9,28,8,12,3,3,41,59,24,51,55,57,294,9,9,2,6,2,15,1,2,13,38,90,9,9,9,3,11,7,1,1,1,5,6,3,2,
        1,2,2,3,8,1,4,4,1,5,7,1,4,3,20,4,9,1,1,1,5,5,17,1,5,2,6,2,4,1,4,5,7,3,18,11,11,32,7,5,4,7,11,127,8,4,3,3,1,10,1,1,6,21,14,1,16,1,7,1,3,6,9,65,
        51,4,3,13,3,10,1,1,12,9,21,110,3,19,24,1,1,10,62,4,1,29,42,78,28,20,18,82,6,3,15,6,84,58,253,15,155,264,15,21,9,14,7,58,40,39,
    };
    static ImWchar base_ranges[] = // not zero-terminated
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF  // Half-width characters
    };
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = { 0 };
    if (!full_ranges[0])
    {
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        UnpackAccumulativeOffsetsIntoRanges(0x4E00, accumulative_offsets_from_0x4E00, IM_ARRAYSIZE(accumulative_offsets_from_0x4E00), full_ranges + IM_ARRAYSIZE(base_ranges));
    }
    return &full_ranges[0];
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
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void ImFontGlyphRangesBuilder::AddText(const char* text, const char* text_end)
{
    while (text_end ? (text < text_end) : *text)
    {
        unsigned int c = 0;
        int c_len = ImTextCharFromUtf8(&c, text, text_end);
        text += c_len;
        if (c_len == 0)
            break;
        if (c < 0x10000)
            AddChar((ImWchar)c);
    }
}

void ImFontGlyphRangesBuilder::AddRanges(const ImWchar * ranges)
{
    for (; ranges[0]; ranges += 2)
        for (ImWchar c = ranges[0]; c <= ranges[1]; c++)
            AddChar(c);
}

void ImFontGlyphRangesBuilder::BuildRanges(ImVector<ImWchar> * out_ranges)
{
    for (int n = 0; n < 0x10000; n++)
        if (GetBit(n))
        {
            out_ranges->push_back((ImWchar)n);
            while (n < 0x10000 && GetBit(n + 1))
                n++;
            out_ranges->push_back((ImWchar)n);
        }
    out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
    FallbackChar = (ImWchar)'?';
    DisplayOffset = ImVec2(0.0f, 0.0f);
    FallbackGlyph = NULL;
    ContainerAtlas = NULL;
    ConfigData = NULL;
    ConfigDataCount = 0;
    DirtyLookupTables = false;
    Scale = 1.0f;
    Ascent = Descent = 0.0f;
    MetricsTotalSurface = 0;
}

ImFont::~ImFont()
{
    ClearOutputData();
}

void    ImFont::ClearOutputData()
{
    FontSize = 0.0f;
    FallbackAdvanceX = 0.0f;
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
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
    IndexAdvanceX.clear();
    IndexLookup.clear();
    DirtyLookupTables = false;
    GrowIndex(max_codepoint + 1);
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
        IndexLookup[codepoint] = (ImWchar)i;
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((ImWchar)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
            Glyphs.resize(Glyphs.Size + 1);
        ImFontGlyph & tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((ImWchar)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.AdvanceX *= IM_TABSIZE;
        IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
        IndexLookup[(int)tab_glyph.Codepoint] = (ImWchar)(Glyphs.Size - 1);
    }

    FallbackGlyph = FindGlyphNoFallback(FallbackChar);
    FallbackAdvanceX = FallbackGlyph ? FallbackGlyph->AdvanceX : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexAdvanceX[i] < 0.0f)
            IndexAdvanceX[i] = FallbackAdvanceX;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

void ImFont::GrowIndex(int new_size)
{
    IM_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
    if (new_size <= IndexLookup.Size)
        return;
    IndexAdvanceX.resize(new_size, -1.0f);
    IndexLookup.resize(new_size, (ImWchar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0 are often fairly close to zero.
// Not to be mistaken with texture coordinates, which are held by u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis).
void ImFont::AddGlyph(ImWchar codepoint, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x)
{
    Glyphs.resize(Glyphs.Size + 1);
    ImFontGlyph & glyph = Glyphs.back();
    glyph.Codepoint = (ImWchar)codepoint;
    glyph.X0 = x0;
    glyph.Y0 = y0;
    glyph.X1 = x1;
    glyph.Y1 = y1;
    glyph.U0 = u0;
    glyph.V0 = v0;
    glyph.U1 = u1;
    glyph.V1 = v1;
    glyph.AdvanceX = advance_x + ConfigData->GlyphExtraSpacing.x;  // Bake spacing into AdvanceX

    if (ConfigData->PixelSnapH)
        glyph.AdvanceX = (float)(int)(glyph.AdvanceX + 0.5f);

    // Compute rough surface usage metrics (+1 to account for avehnly padding, +0.99 to round)
    DirtyLookupTables = true;
    MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + 1.99f) * (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + 1.99f);
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
    IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
    int index_size = IndexLookup.Size;

    if (dst < index_size && IndexLookup.Data[dst] == (ImWchar)-1 && !overwrite_dst) // 'dst' already exists
        return;
    if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
        return;

    GrowIndex(dst + 1);
    IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (ImWchar)-1;
    IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const ImFontGlyph * ImFont::FindGlyph(ImWchar c) const
{
    if (c >= IndexLookup.Size)
        return FallbackGlyph;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return FallbackGlyph;
    return &Glyphs.Data[i];
}

const ImFontGlyph * ImFont::FindGlyphNoFallback(ImWchar c) const
{
    if (c >= IndexLookup.Size)
        return NULL;
    const ImWchar i = IndexLookup.Data[c];
    if (i == (ImWchar)-1)
        return NULL;
    return &Glyphs.Data[i];
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)* s;
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

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
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
        } else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            } else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width >= wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
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
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)* s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        * remaining = s;

    return text_size;
}

void ImFont::RenderChar(ImDrawList * draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // Match behavior of RenderText(), those 4 codepoints are hard-coded.
        return;
    if (const ImFontGlyph * glyph = FindGlyph(c))
    {
        float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
        pos.x = (float)(int)pos.x + DisplayOffset.x;
        pos.y = (float)(int)pos.y + DisplayOffset.y;
        draw_list->PrimReserve(6, 4);
        draw_list->PrimRectUV(ImVec2(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale), ImVec2(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale), ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
    }
}

void ImFont::RenderText(ImDrawList * draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4 & clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // vsonyp0wer functions generally already provides a valid text_end, so this is merely to handle direct calls.

    // Align to be pixel perfect
    pos.x = (float)(int)pos.x + DisplayOffset.x;
    pos.y = (float)(int)pos.y + DisplayOffset.y;
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

    // For large text, scan for the last visible line in order to avoid over-reserving in the call to PrimReserve()
    // Note that very large horizontal line will still be affected by the issue (e.g. a one megabyte string buffer without a newline will likely crash atm)
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

    // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
    const int vtx_count_max = (int)(text_end - s) * 4;
    const int idx_count_max = (int)(text_end - s) * 6;
    const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
    draw_list->PrimReserve(idx_count_max, vtx_count_max);

    ImDrawVert * vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx * idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)* s;
        if (c < 0x80)
        {
            s += 1;
        } else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;
                if (y > clip_rect.w)
                    break; // break out of main loop
                continue;
            }
            if (c == '\r')
                continue;
        }

        float char_width = 0.0f;
        if (const ImFontGlyph * glyph = FindGlyph((ImWchar)c))
        {
            char_width = glyph->AdvanceX * scale;

            // Arbitrarily assume that both space and tabs are empty glyphs as an optimization
            if (c != ' ' && c != '\t')
            {
                // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
                float x1 = x + glyph->X0 * scale;
                float x2 = x + glyph->X1 * scale;
                float y1 = y + glyph->Y0 * scale;
                float y2 = y + glyph->Y1 * scale;
                if (x1 <= clip_rect.z && x2 >= clip_rect.x)
                {
                    // Render a character
                    float u1 = glyph->U0;
                    float v1 = glyph->V0;
                    float u2 = glyph->U1;
                    float v2 = glyph->V1;

                    // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                    if (cpu_fine_clip)
                    {
                        if (x1 < clip_rect.x)
                        {
                            u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clip_rect.x;
                        }
                        if (y1 < clip_rect.y)
                        {
                            v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clip_rect.y;
                        }
                        if (x2 > clip_rect.z)
                        {
                            u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clip_rect.z;
                        }
                        if (y2 > clip_rect.w)
                        {
                            v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clip_rect.w;
                        }
                        if (y1 >= y2)
                        {
                            x += char_width;
                            continue;
                        }
                    }

                    // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug builds. Inlined here:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                }
            }
        }

        x += char_width;
    }

    // Give back unused vertices
    draw_list->VtxBuffer.resize((int)(vtx_write - draw_list->VtxBuffer.Data));
    draw_list->IdxBuffer.resize((int)(idx_write - draw_list->IdxBuffer.Data));
    draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_IdxWritePtr = idx_write;
    draw_list->_VtxCurrentIdx = (unsigned int)draw_list->VtxBuffer.Size;
}

//-----------------------------------------------------------------------------
// [SECTION] Internal Render Helpers
// (progressively moved from vsonyp0wer.cpp to here when they are redesigned to stop accessing vsonyp0wer global state)
//-----------------------------------------------------------------------------
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
// - RenderPixelEllipsis()
//-----------------------------------------------------------------------------

void vsonyp0wer::RenderMouseCursor(ImDrawList * draw_list, ImVec2 pos, float scale, vsonyp0werMouseCursor mouse_cursor)
{
    if (mouse_cursor == vsonyp0werMouseCursor_None)
        return;
    IM_ASSERT(mouse_cursor > vsonyp0werMouseCursor_None && mouse_cursor < vsonyp0werMouseCursor_COUNT);

    const ImU32 col_shadow = IM_COL32(0, 0, 0, 48);
    const ImU32 col_border = IM_COL32(0, 0, 0, 255);          // Black
    const ImU32 col_fill = IM_COL32(255, 255, 255, 255);    // White

    ImFontAtlas * font_atlas = draw_list->_Data->Font->ContainerAtlas;
    ImVec2 offset, size, uv[4];
    if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2]))
    {
        pos -= offset;
        const ImTextureID tex_id = font_atlas->TexID;
        draw_list->PushTextureID(tex_id);
        draw_list->AddImage(tex_id, pos + ImVec2(1, 0) * scale, pos + ImVec2(1, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos + ImVec2(2, 0) * scale, pos + ImVec2(2, 0) * scale + size * scale, uv[2], uv[3], col_shadow);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[2], uv[3], col_border);
        draw_list->AddImage(tex_id, pos, pos + size * scale, uv[0], uv[1], col_fill);
        draw_list->PopTextureID();
    }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz.x is length from base to tip. half_sz.y is length on each side.
void vsonyp0wer::RenderArrowPointingAt(ImDrawList * draw_list, ImVec2 pos, ImVec2 half_sz, vsonyp0werDir direction, ImU32 col)
{
    switch (direction)
    {
    case vsonyp0werDir_Left:  draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), pos, col); return;
    case vsonyp0werDir_Right: draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), pos, col); return;
    case vsonyp0werDir_Up:    draw_list->AddTriangleFilled(ImVec2(pos.x + half_sz.x, pos.y + half_sz.y), ImVec2(pos.x - half_sz.x, pos.y + half_sz.y), pos, col); return;
    case vsonyp0werDir_Down:  draw_list->AddTriangleFilled(ImVec2(pos.x - half_sz.x, pos.y - half_sz.y), ImVec2(pos.x + half_sz.x, pos.y - half_sz.y), pos, col); return;
    case vsonyp0werDir_None: case vsonyp0werDir_COUNT: break; // Fix warnings
    }
}

static inline float ImAcos01(float x)
{
    if (x <= 0.0f) return IM_PI * 0.5f;
    if (x >= 1.0f) return 0.0f;
    return ImAcos(x);
    //return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to ImDrawList.
void vsonyp0wer::RenderRectFilledRangeH(ImDrawList * draw_list, const ImRect & rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding)
{
    if (x_end_norm == x_start_norm)
        return;
    if (x_start_norm > x_end_norm)
        ImSwap(x_start_norm, x_end_norm);

    ImVec2 p0 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_start_norm), rect.Min.y);
    ImVec2 p1 = ImVec2(ImLerp(rect.Min.x, rect.Max.x, x_end_norm), rect.Max.y);
    if (rounding == 0.0f)
    {
        draw_list->AddRectFilled(p0, p1, col, 0.0f);
        return;
    }

    rounding = ImClamp(ImMin((rect.Max.x - rect.Min.x) * 0.5f, (rect.Max.y - rect.Min.y) * 0.5f) - 1.0f, 0.0f, rounding);
    const float inv_rounding = 1.0f / rounding;
    const float arc0_b = ImAcos01(1.0f - (p0.x - rect.Min.x) * inv_rounding);
    const float arc0_e = ImAcos01(1.0f - (p1.x - rect.Min.x) * inv_rounding);
    const float half_pi = IM_PI * 0.5f; // We will == compare to this because we know this is the exact value ImAcos01 can return.
    const float x0 = ImMax(p0.x, rect.Min.x + rounding);
    if (arc0_b == arc0_e)
    {
        draw_list->PathLineTo(ImVec2(x0, p1.y));
        draw_list->PathLineTo(ImVec2(x0, p0.y));
    } else if (arc0_b == 0.0f && arc0_e == half_pi)
    {
        draw_list->PathArcToFast(ImVec2(x0, p1.y - rounding), rounding, 3, 6); // BL
        draw_list->PathArcToFast(ImVec2(x0, p0.y + rounding), rounding, 6, 9); // TR
    } else
    {
        draw_list->PathArcTo(ImVec2(x0, p1.y - rounding), rounding, IM_PI - arc0_e, IM_PI - arc0_b, 3); // BL
        draw_list->PathArcTo(ImVec2(x0, p0.y + rounding), rounding, IM_PI + arc0_b, IM_PI + arc0_e, 3); // TR
    }
    if (p1.x > rect.Min.x + rounding)
    {
        const float arc1_b = ImAcos01(1.0f - (rect.Max.x - p1.x) * inv_rounding);
        const float arc1_e = ImAcos01(1.0f - (rect.Max.x - p0.x) * inv_rounding);
        const float x1 = ImMin(p1.x, rect.Max.x - rounding);
        if (arc1_b == arc1_e)
        {
            draw_list->PathLineTo(ImVec2(x1, p0.y));
            draw_list->PathLineTo(ImVec2(x1, p1.y));
        } else if (arc1_b == 0.0f && arc1_e == half_pi)
        {
            draw_list->PathArcToFast(ImVec2(x1, p0.y + rounding), rounding, 9, 12); // TR
            draw_list->PathArcToFast(ImVec2(x1, p1.y - rounding), rounding, 0, 3);  // BR
        } else
        {
            draw_list->PathArcTo(ImVec2(x1, p0.y + rounding), rounding, -arc1_e, -arc1_b, 3); // TR
            draw_list->PathArcTo(ImVec2(x1, p1.y - rounding), rounding, +arc1_b, +arc1_e, 3); // BR
        }
    }
    draw_list->PathFillConvex(col);
}

// FIXME: Rendering an ellipsis "..." is a surprisingly tricky problem for us... we cannot rely on font glyph having it,
// and regular dot are typically too wide. If we render a dot/shape ourselves it comes with the risk that it wouldn't match
// the boldness or positioning of what the font uses...
void vsonyp0wer::RenderPixelEllipsis(ImDrawList * draw_list, ImVec2 pos, int count, ImU32 col)
{
    ImFont* font = draw_list->_Data->Font;
    const float font_scale = draw_list->_Data->FontSize / font->FontSize;
    pos.y += (float)(int)(font->DisplayOffset.y + font->Ascent * font_scale + 0.5f - 1.0f);
    for (int dot_n = 0; dot_n < count; dot_n++)
        draw_list->AddRectFilled(ImVec2(pos.x + dot_n * 2.0f, pos.y), ImVec2(pos.x + dot_n * 2.0f + 1.0f, pos.y + 1.0f), col);
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char* input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char* stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char* stb__barrier_in_b;
static unsigned char* stb__dout;
static void stb__match(const unsigned char* data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e + 1; return; }
    while (length--)* stb__dout++ = *data++;
}

static void stb__lit(const unsigned char* data, unsigned int length)
{
    IM_ASSERT(stb__dout + length <= stb__barrier_out_e);
    if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
    if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e + 1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char* stb_decompress_token(const unsigned char* i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
        else if (*i >= 0x08)  stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char* buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen) {
        for (i = 0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char* output, const unsigned char* i, unsigned int /*length*/)
{
    unsigned int olen;
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        const unsigned char* old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen);
        if (stb__dout > output + olen)
            return 0;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980 + 1] =
"7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/"
"2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#"
"`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uL"
"i@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`N"
"kfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N"
"*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)"
"tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CX"
"ow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc."
"x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`G"
"CRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)"
"U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#"
"'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/QTtG?-.*^N-4B/ZM"
"_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`gu"
"Ft(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76/"
"/oL?#h7gl85[qW/NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L"
"%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#"
"OQFKNX@QI'IoPp7nb,QU//MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+q3n0#3U1InDjF682-SjMXJK)("
"h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5h"
"o;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnO"
"j%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq=?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-"
"sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/:chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-"
"eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjO"
"M7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%"
"LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/)Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]"
"%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(et"
"Hg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:"
"a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL("
"$/V,;(kXZejWO`<[5?\?ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
"nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?"
"7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;"
")g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3M"
"D?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX("
"P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJs"
"bIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Q"
"h4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//,]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-"
"V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5i"
"sZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7"
".m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@"
"$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/k]uj9EwsG>%veR*"
"hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/,;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u"
"@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#"
"w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#"
"u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0"
"d=j.LQf./Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB8"
"6e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#"
"b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD"
":k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/:p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+"
"tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*"
"$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7"
":d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A"
"7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7"
"u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kT"
"LwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/dl2&8'<M"
":^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&]>"
"_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%"
"hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//)>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;"
"^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:"
"+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)k11IM1c^j%"
"9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-"
"CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*"
"hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY"
"8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-"
"S(qw%sf/@%#B6;/U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`(X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`"
"0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/);TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/"
"+fsd;l#SafT/f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLj"
"M=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V"
"?6*C()dOp7:WL,b&3Rg/.cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/.?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPK"
"Yq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa"
">gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>"
"[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@I"
"wOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-xkA#"
"Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$"
"MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*.jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)"
"i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/,SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo"
"1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$P"
"iDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAO"
"URQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#"
";u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>"
"w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#"
"d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4"
"A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#"
"/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/=#eG8=#t8J5#`+78#4uI-#"
"m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#"
"TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC=2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP"
"GT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xp"
"O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return proggy_clean_ttf_compressed_data_base85;
}



































































// Junk Code By Troll Face & Thaisen's Gen
void COEWMJIxZsywardqKLaEqSnhoNxwydxzggcwmKeK10333274() {     int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD46196952 = -682670934;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54641177 = -154395641;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2477470 = -321397099;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89758025 = -832339707;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD90148012 = -870836302;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD28509093 = -617506382;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89547739 = -133991979;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD48325496 = -406192137;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD72593211 = -442090441;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD35142742 = -369394681;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45430153 = -75887332;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40027269 = -552170372;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8205407 = -360378443;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD80547685 = -971366078;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63463653 = -250084200;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18876726 = -319505424;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30036035 = -930669377;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30861095 = -545377009;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36954125 = -946287236;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD19350228 = -640821228;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD71148580 = -961150502;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79630446 = -503231681;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51790094 = -572344627;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD20116340 = 95365994;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40039899 = -383736754;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26398105 = -922250017;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD67558538 = -111927568;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3402526 = -64237413;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD70265087 = -883668920;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD16504131 = -126315113;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3654193 = -223117295;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD94585237 = -542717234;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD75950510 = -956252435;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54256169 = -129283923;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD73767732 = -799063595;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD55145402 = -794271176;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97084837 = -588641371;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99510151 = -289393565;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4770839 = -725122201;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD43664564 = -819226503;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD13034772 = -778812753;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89291644 = 26636707;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97074182 = -249833532;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89650796 = -364819080;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95902624 = -744899181;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD92924855 = -872592113;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30822361 = -377660776;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD68471126 = -480921648;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD23560096 = -198685564;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD33534042 = -770640482;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD65298029 = -773545492;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4818259 = -231079397;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47272094 = -602876633;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD57034507 = -719394060;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD39184693 = -543214703;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD66566506 = -79439253;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2851083 = -582051014;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82361130 = -316763094;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49718127 = -348602954;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63749908 = -948586285;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD60950555 = -405578815;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD86145213 = 30245433;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD78060408 = -522523218;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD56089080 = -215775328;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD31488549 = -46277387;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD50844915 = -533170099;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD64076759 = -595917937;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD53949238 = -131094520;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6779953 = -72302484;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8318252 = -455813024;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD21791888 = -730864054;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30525883 = -541275813;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26090257 = -820254809;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD93289561 = -27060734;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6315457 = -862008475;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD81856936 = -887787210;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82556263 = -153398150;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD62139298 = -107525548;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD24213716 = -159734825;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47115043 = -511144641;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95575744 = -444589242;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99087411 = -631005921;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79842430 = -865551849;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36731045 = -13028438;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51206101 = -352769622;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD98835933 = -992037898;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47313143 = -939840601;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18916003 = -136858376;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD15071477 = -586069221;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD7201227 = -619624342;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD52294320 = -112220163;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD14723708 = -171878277;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49792025 = -940790612;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD41020931 = -776535916;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82714008 = -313647689;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26889558 = -709058187;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD11231236 = -450840076;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40985103 = 65941796;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD58162247 = -218541694;    int umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45057709 = -682670934;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD46196952 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54641177;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54641177 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2477470;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2477470 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89758025;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89758025 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD90148012;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD90148012 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD28509093;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD28509093 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89547739;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89547739 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD48325496;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD48325496 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD72593211;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD72593211 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD35142742;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD35142742 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45430153;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45430153 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40027269;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40027269 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8205407;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8205407 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD80547685;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD80547685 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63463653;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63463653 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18876726;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18876726 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30036035;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30036035 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30861095;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30861095 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36954125;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36954125 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD19350228;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD19350228 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD71148580;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD71148580 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79630446;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79630446 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51790094;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51790094 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD20116340;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD20116340 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40039899;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40039899 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26398105;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26398105 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD67558538;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD67558538 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3402526;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3402526 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD70265087;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD70265087 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD16504131;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD16504131 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3654193;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD3654193 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD94585237;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD94585237 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD75950510;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD75950510 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54256169;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD54256169 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD73767732;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD73767732 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD55145402;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD55145402 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97084837;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97084837 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99510151;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99510151 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4770839;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4770839 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD43664564;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD43664564 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD13034772;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD13034772 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89291644;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89291644 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97074182;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD97074182 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89650796;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD89650796 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95902624;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95902624 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD92924855;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD92924855 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30822361;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30822361 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD68471126;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD68471126 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD23560096;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD23560096 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD33534042;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD33534042 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD65298029;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD65298029 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4818259;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD4818259 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47272094;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47272094 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD57034507;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD57034507 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD39184693;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD39184693 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD66566506;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD66566506 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2851083;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD2851083 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82361130;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82361130 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49718127;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49718127 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63749908;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD63749908 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD60950555;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD60950555 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD86145213;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD86145213 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD78060408;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD78060408 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD56089080;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD56089080 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD31488549;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD31488549 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD50844915;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD50844915 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD64076759;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD64076759 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD53949238;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD53949238 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6779953;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6779953 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8318252;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD8318252 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD21791888;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD21791888 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30525883;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD30525883 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26090257;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26090257 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD93289561;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD93289561 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6315457;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD6315457 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD81856936;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD81856936 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82556263;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82556263 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD62139298;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD62139298 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD24213716;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD24213716 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47115043;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47115043 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95575744;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD95575744 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99087411;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD99087411 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79842430;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD79842430 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36731045;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD36731045 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51206101;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD51206101 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD98835933;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD98835933 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47313143;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD47313143 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18916003;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD18916003 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD15071477;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD15071477 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD7201227;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD7201227 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD52294320;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD52294320 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD14723708;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD14723708 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49792025;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD49792025 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD41020931;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD41020931 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82714008;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD82714008 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26889558;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD26889558 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD11231236;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD11231236 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40985103;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD40985103 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD58162247;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD58162247 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45057709;     umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD45057709 = umGUYEeSkaYAZEGjbXhyMLyvvzPuOhEzgPJDLSIjjeMYExacZsUOcfyioLZZpkECVYTJuD46196952;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void UOYXionzPdYWnBNHqJkFtzJoDHmvlEJCGxskZlihSJojvHLHjsU63473359() {     float XmdrVEkTaBrOLTWQqJchEaCmkKat97682626 = -449175566;    float XmdrVEkTaBrOLTWQqJchEaCmkKat79125601 = -780634351;    float XmdrVEkTaBrOLTWQqJchEaCmkKat86072674 = -563999543;    float XmdrVEkTaBrOLTWQqJchEaCmkKat28548257 = -659872285;    float XmdrVEkTaBrOLTWQqJchEaCmkKat71506275 = -23260483;    float XmdrVEkTaBrOLTWQqJchEaCmkKat53755788 = -538123580;    float XmdrVEkTaBrOLTWQqJchEaCmkKat76812556 = 19256383;    float XmdrVEkTaBrOLTWQqJchEaCmkKat61726711 = -865517606;    float XmdrVEkTaBrOLTWQqJchEaCmkKat76376350 = -296443275;    float XmdrVEkTaBrOLTWQqJchEaCmkKat61855125 = -619864960;    float XmdrVEkTaBrOLTWQqJchEaCmkKat78226738 = -605638922;    float XmdrVEkTaBrOLTWQqJchEaCmkKat39580890 = -125008421;    float XmdrVEkTaBrOLTWQqJchEaCmkKat53315803 = -698419701;    float XmdrVEkTaBrOLTWQqJchEaCmkKat75619400 = -684456628;    float XmdrVEkTaBrOLTWQqJchEaCmkKat32788120 = -206705405;    float XmdrVEkTaBrOLTWQqJchEaCmkKat23538801 = -96314393;    float XmdrVEkTaBrOLTWQqJchEaCmkKat64483010 = -721284114;    float XmdrVEkTaBrOLTWQqJchEaCmkKat3898709 = -386793320;    float XmdrVEkTaBrOLTWQqJchEaCmkKat28381736 = -357710797;    float XmdrVEkTaBrOLTWQqJchEaCmkKat57978336 = -11525197;    float XmdrVEkTaBrOLTWQqJchEaCmkKat59179189 = -406919685;    float XmdrVEkTaBrOLTWQqJchEaCmkKat9393346 = -793630530;    float XmdrVEkTaBrOLTWQqJchEaCmkKat72210403 = -117330495;    float XmdrVEkTaBrOLTWQqJchEaCmkKat97701897 = -60339590;    float XmdrVEkTaBrOLTWQqJchEaCmkKat77791448 = -996095411;    float XmdrVEkTaBrOLTWQqJchEaCmkKat91981537 = 9753450;    float XmdrVEkTaBrOLTWQqJchEaCmkKat98807926 = -55006075;    float XmdrVEkTaBrOLTWQqJchEaCmkKat47754696 = -758085748;    float XmdrVEkTaBrOLTWQqJchEaCmkKat79128523 = -795465413;    float XmdrVEkTaBrOLTWQqJchEaCmkKat4896279 = -590120069;    float XmdrVEkTaBrOLTWQqJchEaCmkKat93918269 = -308189354;    float XmdrVEkTaBrOLTWQqJchEaCmkKat16274991 = -263403985;    float XmdrVEkTaBrOLTWQqJchEaCmkKat33935438 = -444857835;    float XmdrVEkTaBrOLTWQqJchEaCmkKat88683645 = -52255141;    float XmdrVEkTaBrOLTWQqJchEaCmkKat89242504 = -208042916;    float XmdrVEkTaBrOLTWQqJchEaCmkKat19674178 = -37888128;    float XmdrVEkTaBrOLTWQqJchEaCmkKat60056905 = -635259589;    float XmdrVEkTaBrOLTWQqJchEaCmkKat14665640 = -199207143;    float XmdrVEkTaBrOLTWQqJchEaCmkKat80715621 = -581366132;    float XmdrVEkTaBrOLTWQqJchEaCmkKat32367772 = -217884845;    float XmdrVEkTaBrOLTWQqJchEaCmkKat74887192 = -60425773;    float XmdrVEkTaBrOLTWQqJchEaCmkKat89442750 = 53906508;    float XmdrVEkTaBrOLTWQqJchEaCmkKat56955071 = -747826222;    float XmdrVEkTaBrOLTWQqJchEaCmkKat44839895 = -451850810;    float XmdrVEkTaBrOLTWQqJchEaCmkKat36747510 = -283875630;    float XmdrVEkTaBrOLTWQqJchEaCmkKat55720401 = -634642804;    float XmdrVEkTaBrOLTWQqJchEaCmkKat84574489 = -513032896;    float XmdrVEkTaBrOLTWQqJchEaCmkKat26013302 = -612546226;    float XmdrVEkTaBrOLTWQqJchEaCmkKat44034644 = -680194883;    float XmdrVEkTaBrOLTWQqJchEaCmkKat52276397 = 47933408;    float XmdrVEkTaBrOLTWQqJchEaCmkKat91806135 = -774593233;    float XmdrVEkTaBrOLTWQqJchEaCmkKat15842759 = -437916499;    float XmdrVEkTaBrOLTWQqJchEaCmkKat8239637 = -702064203;    float XmdrVEkTaBrOLTWQqJchEaCmkKat47050847 = -42541376;    float XmdrVEkTaBrOLTWQqJchEaCmkKat4294170 = -770326921;    float XmdrVEkTaBrOLTWQqJchEaCmkKat88289281 = -655545037;    float XmdrVEkTaBrOLTWQqJchEaCmkKat6915198 = -563303856;    float XmdrVEkTaBrOLTWQqJchEaCmkKat88370777 = -403659953;    float XmdrVEkTaBrOLTWQqJchEaCmkKat50756808 = -663776874;    float XmdrVEkTaBrOLTWQqJchEaCmkKat79524738 = 66986067;    float XmdrVEkTaBrOLTWQqJchEaCmkKat54947861 = -383117506;    float XmdrVEkTaBrOLTWQqJchEaCmkKat29057860 = -222657870;    float XmdrVEkTaBrOLTWQqJchEaCmkKat82598188 = 29947807;    float XmdrVEkTaBrOLTWQqJchEaCmkKat71480071 = -706323207;    float XmdrVEkTaBrOLTWQqJchEaCmkKat67936856 = -211675606;    float XmdrVEkTaBrOLTWQqJchEaCmkKat61951747 = -242234937;    float XmdrVEkTaBrOLTWQqJchEaCmkKat5645453 = -680150587;    float XmdrVEkTaBrOLTWQqJchEaCmkKat64632158 = -546164561;    float XmdrVEkTaBrOLTWQqJchEaCmkKat86376896 = -376413712;    float XmdrVEkTaBrOLTWQqJchEaCmkKat13113943 = -68817277;    float XmdrVEkTaBrOLTWQqJchEaCmkKat63481896 = -461054804;    float XmdrVEkTaBrOLTWQqJchEaCmkKat49817370 = -422076971;    float XmdrVEkTaBrOLTWQqJchEaCmkKat23183088 = -805427188;    float XmdrVEkTaBrOLTWQqJchEaCmkKat96013964 = -39825953;    float XmdrVEkTaBrOLTWQqJchEaCmkKat83091144 = -951099425;    float XmdrVEkTaBrOLTWQqJchEaCmkKat69736439 = -360826194;    float XmdrVEkTaBrOLTWQqJchEaCmkKat52438274 = 54195692;    float XmdrVEkTaBrOLTWQqJchEaCmkKat27370509 = -665479686;    float XmdrVEkTaBrOLTWQqJchEaCmkKat60954387 = -776463961;    float XmdrVEkTaBrOLTWQqJchEaCmkKat22071047 = -261452608;    float XmdrVEkTaBrOLTWQqJchEaCmkKat7407048 = -477213654;    float XmdrVEkTaBrOLTWQqJchEaCmkKat72794624 = -442459850;    float XmdrVEkTaBrOLTWQqJchEaCmkKat3720053 = 22109135;    float XmdrVEkTaBrOLTWQqJchEaCmkKat26852126 = -743398822;    float XmdrVEkTaBrOLTWQqJchEaCmkKat13090144 = -815526836;    float XmdrVEkTaBrOLTWQqJchEaCmkKat78075510 = -870272856;    float XmdrVEkTaBrOLTWQqJchEaCmkKat8035355 = -561339783;    float XmdrVEkTaBrOLTWQqJchEaCmkKat86884591 = -302316460;    float XmdrVEkTaBrOLTWQqJchEaCmkKat84389475 = -281928221;    float XmdrVEkTaBrOLTWQqJchEaCmkKat953223 = -552497880;    float XmdrVEkTaBrOLTWQqJchEaCmkKat12758980 = -474584272;    float XmdrVEkTaBrOLTWQqJchEaCmkKat71686127 = -131599637;    float XmdrVEkTaBrOLTWQqJchEaCmkKat63908831 = -535430270;    float XmdrVEkTaBrOLTWQqJchEaCmkKat1190884 = -548352199;    float XmdrVEkTaBrOLTWQqJchEaCmkKat77419910 = -834767340;    float XmdrVEkTaBrOLTWQqJchEaCmkKat45829332 = -837767903;    float XmdrVEkTaBrOLTWQqJchEaCmkKat6844562 = -976041299;    float XmdrVEkTaBrOLTWQqJchEaCmkKat85475000 = 58496985;    float XmdrVEkTaBrOLTWQqJchEaCmkKat76903039 = -140175204;    float XmdrVEkTaBrOLTWQqJchEaCmkKat74795763 = -449175566;     XmdrVEkTaBrOLTWQqJchEaCmkKat97682626 = XmdrVEkTaBrOLTWQqJchEaCmkKat79125601;     XmdrVEkTaBrOLTWQqJchEaCmkKat79125601 = XmdrVEkTaBrOLTWQqJchEaCmkKat86072674;     XmdrVEkTaBrOLTWQqJchEaCmkKat86072674 = XmdrVEkTaBrOLTWQqJchEaCmkKat28548257;     XmdrVEkTaBrOLTWQqJchEaCmkKat28548257 = XmdrVEkTaBrOLTWQqJchEaCmkKat71506275;     XmdrVEkTaBrOLTWQqJchEaCmkKat71506275 = XmdrVEkTaBrOLTWQqJchEaCmkKat53755788;     XmdrVEkTaBrOLTWQqJchEaCmkKat53755788 = XmdrVEkTaBrOLTWQqJchEaCmkKat76812556;     XmdrVEkTaBrOLTWQqJchEaCmkKat76812556 = XmdrVEkTaBrOLTWQqJchEaCmkKat61726711;     XmdrVEkTaBrOLTWQqJchEaCmkKat61726711 = XmdrVEkTaBrOLTWQqJchEaCmkKat76376350;     XmdrVEkTaBrOLTWQqJchEaCmkKat76376350 = XmdrVEkTaBrOLTWQqJchEaCmkKat61855125;     XmdrVEkTaBrOLTWQqJchEaCmkKat61855125 = XmdrVEkTaBrOLTWQqJchEaCmkKat78226738;     XmdrVEkTaBrOLTWQqJchEaCmkKat78226738 = XmdrVEkTaBrOLTWQqJchEaCmkKat39580890;     XmdrVEkTaBrOLTWQqJchEaCmkKat39580890 = XmdrVEkTaBrOLTWQqJchEaCmkKat53315803;     XmdrVEkTaBrOLTWQqJchEaCmkKat53315803 = XmdrVEkTaBrOLTWQqJchEaCmkKat75619400;     XmdrVEkTaBrOLTWQqJchEaCmkKat75619400 = XmdrVEkTaBrOLTWQqJchEaCmkKat32788120;     XmdrVEkTaBrOLTWQqJchEaCmkKat32788120 = XmdrVEkTaBrOLTWQqJchEaCmkKat23538801;     XmdrVEkTaBrOLTWQqJchEaCmkKat23538801 = XmdrVEkTaBrOLTWQqJchEaCmkKat64483010;     XmdrVEkTaBrOLTWQqJchEaCmkKat64483010 = XmdrVEkTaBrOLTWQqJchEaCmkKat3898709;     XmdrVEkTaBrOLTWQqJchEaCmkKat3898709 = XmdrVEkTaBrOLTWQqJchEaCmkKat28381736;     XmdrVEkTaBrOLTWQqJchEaCmkKat28381736 = XmdrVEkTaBrOLTWQqJchEaCmkKat57978336;     XmdrVEkTaBrOLTWQqJchEaCmkKat57978336 = XmdrVEkTaBrOLTWQqJchEaCmkKat59179189;     XmdrVEkTaBrOLTWQqJchEaCmkKat59179189 = XmdrVEkTaBrOLTWQqJchEaCmkKat9393346;     XmdrVEkTaBrOLTWQqJchEaCmkKat9393346 = XmdrVEkTaBrOLTWQqJchEaCmkKat72210403;     XmdrVEkTaBrOLTWQqJchEaCmkKat72210403 = XmdrVEkTaBrOLTWQqJchEaCmkKat97701897;     XmdrVEkTaBrOLTWQqJchEaCmkKat97701897 = XmdrVEkTaBrOLTWQqJchEaCmkKat77791448;     XmdrVEkTaBrOLTWQqJchEaCmkKat77791448 = XmdrVEkTaBrOLTWQqJchEaCmkKat91981537;     XmdrVEkTaBrOLTWQqJchEaCmkKat91981537 = XmdrVEkTaBrOLTWQqJchEaCmkKat98807926;     XmdrVEkTaBrOLTWQqJchEaCmkKat98807926 = XmdrVEkTaBrOLTWQqJchEaCmkKat47754696;     XmdrVEkTaBrOLTWQqJchEaCmkKat47754696 = XmdrVEkTaBrOLTWQqJchEaCmkKat79128523;     XmdrVEkTaBrOLTWQqJchEaCmkKat79128523 = XmdrVEkTaBrOLTWQqJchEaCmkKat4896279;     XmdrVEkTaBrOLTWQqJchEaCmkKat4896279 = XmdrVEkTaBrOLTWQqJchEaCmkKat93918269;     XmdrVEkTaBrOLTWQqJchEaCmkKat93918269 = XmdrVEkTaBrOLTWQqJchEaCmkKat16274991;     XmdrVEkTaBrOLTWQqJchEaCmkKat16274991 = XmdrVEkTaBrOLTWQqJchEaCmkKat33935438;     XmdrVEkTaBrOLTWQqJchEaCmkKat33935438 = XmdrVEkTaBrOLTWQqJchEaCmkKat88683645;     XmdrVEkTaBrOLTWQqJchEaCmkKat88683645 = XmdrVEkTaBrOLTWQqJchEaCmkKat89242504;     XmdrVEkTaBrOLTWQqJchEaCmkKat89242504 = XmdrVEkTaBrOLTWQqJchEaCmkKat19674178;     XmdrVEkTaBrOLTWQqJchEaCmkKat19674178 = XmdrVEkTaBrOLTWQqJchEaCmkKat60056905;     XmdrVEkTaBrOLTWQqJchEaCmkKat60056905 = XmdrVEkTaBrOLTWQqJchEaCmkKat14665640;     XmdrVEkTaBrOLTWQqJchEaCmkKat14665640 = XmdrVEkTaBrOLTWQqJchEaCmkKat80715621;     XmdrVEkTaBrOLTWQqJchEaCmkKat80715621 = XmdrVEkTaBrOLTWQqJchEaCmkKat32367772;     XmdrVEkTaBrOLTWQqJchEaCmkKat32367772 = XmdrVEkTaBrOLTWQqJchEaCmkKat74887192;     XmdrVEkTaBrOLTWQqJchEaCmkKat74887192 = XmdrVEkTaBrOLTWQqJchEaCmkKat89442750;     XmdrVEkTaBrOLTWQqJchEaCmkKat89442750 = XmdrVEkTaBrOLTWQqJchEaCmkKat56955071;     XmdrVEkTaBrOLTWQqJchEaCmkKat56955071 = XmdrVEkTaBrOLTWQqJchEaCmkKat44839895;     XmdrVEkTaBrOLTWQqJchEaCmkKat44839895 = XmdrVEkTaBrOLTWQqJchEaCmkKat36747510;     XmdrVEkTaBrOLTWQqJchEaCmkKat36747510 = XmdrVEkTaBrOLTWQqJchEaCmkKat55720401;     XmdrVEkTaBrOLTWQqJchEaCmkKat55720401 = XmdrVEkTaBrOLTWQqJchEaCmkKat84574489;     XmdrVEkTaBrOLTWQqJchEaCmkKat84574489 = XmdrVEkTaBrOLTWQqJchEaCmkKat26013302;     XmdrVEkTaBrOLTWQqJchEaCmkKat26013302 = XmdrVEkTaBrOLTWQqJchEaCmkKat44034644;     XmdrVEkTaBrOLTWQqJchEaCmkKat44034644 = XmdrVEkTaBrOLTWQqJchEaCmkKat52276397;     XmdrVEkTaBrOLTWQqJchEaCmkKat52276397 = XmdrVEkTaBrOLTWQqJchEaCmkKat91806135;     XmdrVEkTaBrOLTWQqJchEaCmkKat91806135 = XmdrVEkTaBrOLTWQqJchEaCmkKat15842759;     XmdrVEkTaBrOLTWQqJchEaCmkKat15842759 = XmdrVEkTaBrOLTWQqJchEaCmkKat8239637;     XmdrVEkTaBrOLTWQqJchEaCmkKat8239637 = XmdrVEkTaBrOLTWQqJchEaCmkKat47050847;     XmdrVEkTaBrOLTWQqJchEaCmkKat47050847 = XmdrVEkTaBrOLTWQqJchEaCmkKat4294170;     XmdrVEkTaBrOLTWQqJchEaCmkKat4294170 = XmdrVEkTaBrOLTWQqJchEaCmkKat88289281;     XmdrVEkTaBrOLTWQqJchEaCmkKat88289281 = XmdrVEkTaBrOLTWQqJchEaCmkKat6915198;     XmdrVEkTaBrOLTWQqJchEaCmkKat6915198 = XmdrVEkTaBrOLTWQqJchEaCmkKat88370777;     XmdrVEkTaBrOLTWQqJchEaCmkKat88370777 = XmdrVEkTaBrOLTWQqJchEaCmkKat50756808;     XmdrVEkTaBrOLTWQqJchEaCmkKat50756808 = XmdrVEkTaBrOLTWQqJchEaCmkKat79524738;     XmdrVEkTaBrOLTWQqJchEaCmkKat79524738 = XmdrVEkTaBrOLTWQqJchEaCmkKat54947861;     XmdrVEkTaBrOLTWQqJchEaCmkKat54947861 = XmdrVEkTaBrOLTWQqJchEaCmkKat29057860;     XmdrVEkTaBrOLTWQqJchEaCmkKat29057860 = XmdrVEkTaBrOLTWQqJchEaCmkKat82598188;     XmdrVEkTaBrOLTWQqJchEaCmkKat82598188 = XmdrVEkTaBrOLTWQqJchEaCmkKat71480071;     XmdrVEkTaBrOLTWQqJchEaCmkKat71480071 = XmdrVEkTaBrOLTWQqJchEaCmkKat67936856;     XmdrVEkTaBrOLTWQqJchEaCmkKat67936856 = XmdrVEkTaBrOLTWQqJchEaCmkKat61951747;     XmdrVEkTaBrOLTWQqJchEaCmkKat61951747 = XmdrVEkTaBrOLTWQqJchEaCmkKat5645453;     XmdrVEkTaBrOLTWQqJchEaCmkKat5645453 = XmdrVEkTaBrOLTWQqJchEaCmkKat64632158;     XmdrVEkTaBrOLTWQqJchEaCmkKat64632158 = XmdrVEkTaBrOLTWQqJchEaCmkKat86376896;     XmdrVEkTaBrOLTWQqJchEaCmkKat86376896 = XmdrVEkTaBrOLTWQqJchEaCmkKat13113943;     XmdrVEkTaBrOLTWQqJchEaCmkKat13113943 = XmdrVEkTaBrOLTWQqJchEaCmkKat63481896;     XmdrVEkTaBrOLTWQqJchEaCmkKat63481896 = XmdrVEkTaBrOLTWQqJchEaCmkKat49817370;     XmdrVEkTaBrOLTWQqJchEaCmkKat49817370 = XmdrVEkTaBrOLTWQqJchEaCmkKat23183088;     XmdrVEkTaBrOLTWQqJchEaCmkKat23183088 = XmdrVEkTaBrOLTWQqJchEaCmkKat96013964;     XmdrVEkTaBrOLTWQqJchEaCmkKat96013964 = XmdrVEkTaBrOLTWQqJchEaCmkKat83091144;     XmdrVEkTaBrOLTWQqJchEaCmkKat83091144 = XmdrVEkTaBrOLTWQqJchEaCmkKat69736439;     XmdrVEkTaBrOLTWQqJchEaCmkKat69736439 = XmdrVEkTaBrOLTWQqJchEaCmkKat52438274;     XmdrVEkTaBrOLTWQqJchEaCmkKat52438274 = XmdrVEkTaBrOLTWQqJchEaCmkKat27370509;     XmdrVEkTaBrOLTWQqJchEaCmkKat27370509 = XmdrVEkTaBrOLTWQqJchEaCmkKat60954387;     XmdrVEkTaBrOLTWQqJchEaCmkKat60954387 = XmdrVEkTaBrOLTWQqJchEaCmkKat22071047;     XmdrVEkTaBrOLTWQqJchEaCmkKat22071047 = XmdrVEkTaBrOLTWQqJchEaCmkKat7407048;     XmdrVEkTaBrOLTWQqJchEaCmkKat7407048 = XmdrVEkTaBrOLTWQqJchEaCmkKat72794624;     XmdrVEkTaBrOLTWQqJchEaCmkKat72794624 = XmdrVEkTaBrOLTWQqJchEaCmkKat3720053;     XmdrVEkTaBrOLTWQqJchEaCmkKat3720053 = XmdrVEkTaBrOLTWQqJchEaCmkKat26852126;     XmdrVEkTaBrOLTWQqJchEaCmkKat26852126 = XmdrVEkTaBrOLTWQqJchEaCmkKat13090144;     XmdrVEkTaBrOLTWQqJchEaCmkKat13090144 = XmdrVEkTaBrOLTWQqJchEaCmkKat78075510;     XmdrVEkTaBrOLTWQqJchEaCmkKat78075510 = XmdrVEkTaBrOLTWQqJchEaCmkKat8035355;     XmdrVEkTaBrOLTWQqJchEaCmkKat8035355 = XmdrVEkTaBrOLTWQqJchEaCmkKat86884591;     XmdrVEkTaBrOLTWQqJchEaCmkKat86884591 = XmdrVEkTaBrOLTWQqJchEaCmkKat84389475;     XmdrVEkTaBrOLTWQqJchEaCmkKat84389475 = XmdrVEkTaBrOLTWQqJchEaCmkKat953223;     XmdrVEkTaBrOLTWQqJchEaCmkKat953223 = XmdrVEkTaBrOLTWQqJchEaCmkKat12758980;     XmdrVEkTaBrOLTWQqJchEaCmkKat12758980 = XmdrVEkTaBrOLTWQqJchEaCmkKat71686127;     XmdrVEkTaBrOLTWQqJchEaCmkKat71686127 = XmdrVEkTaBrOLTWQqJchEaCmkKat63908831;     XmdrVEkTaBrOLTWQqJchEaCmkKat63908831 = XmdrVEkTaBrOLTWQqJchEaCmkKat1190884;     XmdrVEkTaBrOLTWQqJchEaCmkKat1190884 = XmdrVEkTaBrOLTWQqJchEaCmkKat77419910;     XmdrVEkTaBrOLTWQqJchEaCmkKat77419910 = XmdrVEkTaBrOLTWQqJchEaCmkKat45829332;     XmdrVEkTaBrOLTWQqJchEaCmkKat45829332 = XmdrVEkTaBrOLTWQqJchEaCmkKat6844562;     XmdrVEkTaBrOLTWQqJchEaCmkKat6844562 = XmdrVEkTaBrOLTWQqJchEaCmkKat85475000;     XmdrVEkTaBrOLTWQqJchEaCmkKat85475000 = XmdrVEkTaBrOLTWQqJchEaCmkKat76903039;     XmdrVEkTaBrOLTWQqJchEaCmkKat76903039 = XmdrVEkTaBrOLTWQqJchEaCmkKat74795763;     XmdrVEkTaBrOLTWQqJchEaCmkKat74795763 = XmdrVEkTaBrOLTWQqJchEaCmkKat97682626;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RZXAUKWUMdLcxXcBtOwuBROEu17167454() {     long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74188681 = -574524668;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77127472 = -996406637;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19505047 = -416627210;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo29428874 = -693104399;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo84166618 = -708800872;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo5146958 = -764656562;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6263063 = -651097280;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo1366822 = -143109158;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4849290 = -748619533;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77245532 = -232373827;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo58731287 = 95048300;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo34731102 = 6008310;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo40987740 = -720025794;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86602547 = -694439511;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16903992 = -351236773;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo39636733 = -370013838;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42353810 = -308763542;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo54601113 = -214465218;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44629661 = -490141746;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo60361271 = -695279145;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4490241 = -54980043;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo41246111 = 30252841;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo2572592 = -186355232;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo87779331 = 78999838;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8836557 = -365288059;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88326964 = -705984929;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo38392407 = -433115269;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo85674041 = -657590094;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36873524 = -266244556;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83270815 = -318752914;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13103441 = -496320040;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36504812 = -481704047;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57388212 = -694544760;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo89848623 = -483064107;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo10727112 = -404520329;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16878894 = -598120399;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22316903 = -379058898;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55705576 = -671652831;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9656297 = -267307635;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22339586 = -407838478;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9509370 = -948060574;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo61597225 = -357419581;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo7853386 = -330861524;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19473013 = -918740659;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo81505817 = -139418348;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16022466 = -440444167;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51465786 = -283471144;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo28909326 = -919314649;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo27924699 = -396780154;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo43792478 = 81449220;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8043092 = -713112625;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo76663245 = -717391578;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo189243 = -560834230;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo73704671 = -696080724;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo12677213 = -192947190;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo32942571 = -504777509;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74554880 = -710051406;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo31725715 = -395627048;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20592318 = -227816341;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo95839653 = 97184056;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo66754550 = -231541294;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20589021 = -993507186;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64493298 = -876864602;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo21578474 = -329866619;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64142091 = -736053787;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22226475 = -423247654;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77342889 = -299446931;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51139117 = -136961687;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75875435 = -189919183;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo25099 = -753116374;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo17319831 = -990954941;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86648234 = -637110712;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44944816 = -947157584;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22290076 = 17696731;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo50851902 = -747218571;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42893015 = -697560462;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo33392725 = -638885635;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83099579 = -267614573;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6273515 = -781581815;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo92814091 = -924843892;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36861179 = -322513785;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9483082 = -513800620;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57749343 = -160809941;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo93081045 = -247693776;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75227724 = -605640289;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36440195 = -778928462;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36315569 = -920869817;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83683540 = -998464036;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77171410 = -190116918;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77784541 = -899742820;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42324013 = -888068994;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo90591187 = -983431850;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo35113258 = -343836491;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13816644 = -264491692;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55585035 = -76297184;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88920348 = -954553389;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo97103927 = -480554979;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86274911 = 99005095;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55330921 = -82686873;    long LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo59279342 = -574524668;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74188681 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77127472;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77127472 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19505047;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19505047 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo29428874;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo29428874 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo84166618;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo84166618 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo5146958;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo5146958 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6263063;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6263063 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo1366822;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo1366822 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4849290;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4849290 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77245532;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77245532 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo58731287;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo58731287 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo34731102;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo34731102 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo40987740;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo40987740 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86602547;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86602547 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16903992;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16903992 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo39636733;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo39636733 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42353810;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42353810 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo54601113;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo54601113 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44629661;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44629661 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo60361271;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo60361271 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4490241;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo4490241 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo41246111;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo41246111 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo2572592;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo2572592 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo87779331;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo87779331 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8836557;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8836557 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88326964;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88326964 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo38392407;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo38392407 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo85674041;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo85674041 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36873524;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36873524 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83270815;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83270815 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13103441;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13103441 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36504812;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36504812 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57388212;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57388212 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo89848623;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo89848623 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo10727112;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo10727112 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16878894;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16878894 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22316903;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22316903 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55705576;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55705576 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9656297;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9656297 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22339586;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22339586 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9509370;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9509370 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo61597225;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo61597225 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo7853386;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo7853386 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19473013;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo19473013 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo81505817;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo81505817 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16022466;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo16022466 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51465786;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51465786 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo28909326;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo28909326 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo27924699;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo27924699 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo43792478;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo43792478 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8043092;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo8043092 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo76663245;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo76663245 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo189243;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo189243 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo73704671;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo73704671 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo12677213;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo12677213 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo32942571;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo32942571 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74554880;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74554880 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo31725715;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo31725715 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20592318;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20592318 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo95839653;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo95839653 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo66754550;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo66754550 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20589021;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo20589021 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64493298;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64493298 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo21578474;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo21578474 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64142091;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo64142091 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22226475;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22226475 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77342889;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77342889 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51139117;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo51139117 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75875435;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75875435 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo25099;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo25099 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo17319831;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo17319831 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86648234;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86648234 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44944816;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo44944816 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22290076;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo22290076 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo50851902;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo50851902 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42893015;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42893015 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo33392725;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo33392725 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83099579;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83099579 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6273515;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo6273515 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo92814091;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo92814091 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36861179;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36861179 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9483082;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo9483082 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57749343;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo57749343 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo93081045;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo93081045 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75227724;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo75227724 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36440195;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36440195 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36315569;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo36315569 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83683540;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo83683540 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77171410;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77171410 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77784541;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo77784541 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42324013;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo42324013 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo90591187;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo90591187 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo35113258;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo35113258 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13816644;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo13816644 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55585035;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55585035 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88920348;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo88920348 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo97103927;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo97103927 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86274911;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo86274911 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55330921;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo55330921 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo59279342;     LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo59279342 = LgTuvzcvxroeVRZxVkLNOXDnkuYJqkALTGJdCaRdigAgo74188681;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kWelHcnqZUEweyMVpjQqMhlnuaWGJlskpnllB8489286() {     double vPquOHepAMjAwZKwiSClr81497689 = -971754121;    double vPquOHepAMjAwZKwiSClr76046881 = -523814497;    double vPquOHepAMjAwZKwiSClr4919553 = -363819492;    double vPquOHepAMjAwZKwiSClr27367871 = -771520382;    double vPquOHepAMjAwZKwiSClr80870586 = -294593697;    double vPquOHepAMjAwZKwiSClr87287960 = 50755194;    double vPquOHepAMjAwZKwiSClr67407105 = -515418121;    double vPquOHepAMjAwZKwiSClr41266902 = -140410946;    double vPquOHepAMjAwZKwiSClr17750466 = -745685345;    double vPquOHepAMjAwZKwiSClr24066544 = 35029860;    double vPquOHepAMjAwZKwiSClr3886427 = -20783653;    double vPquOHepAMjAwZKwiSClr71257667 = -24541232;    double vPquOHepAMjAwZKwiSClr61040235 = -120518706;    double vPquOHepAMjAwZKwiSClr46221183 = -604970489;    double vPquOHepAMjAwZKwiSClr90283334 = 17250262;    double vPquOHepAMjAwZKwiSClr43149567 = 92435762;    double vPquOHepAMjAwZKwiSClr90191971 = -454521338;    double vPquOHepAMjAwZKwiSClr18217256 = -976268623;    double vPquOHepAMjAwZKwiSClr40314382 = -257820136;    double vPquOHepAMjAwZKwiSClr6965310 = 36765875;    double vPquOHepAMjAwZKwiSClr48631124 = -369848532;    double vPquOHepAMjAwZKwiSClr69841878 = -18553945;    double vPquOHepAMjAwZKwiSClr76209927 = -246008192;    double vPquOHepAMjAwZKwiSClr31133870 = -13674653;    double vPquOHepAMjAwZKwiSClr34257673 = -867225917;    double vPquOHepAMjAwZKwiSClr83273136 = -533642812;    double vPquOHepAMjAwZKwiSClr28962933 = -137864252;    double vPquOHepAMjAwZKwiSClr19923693 = -321056502;    double vPquOHepAMjAwZKwiSClr54558673 = -917391204;    double vPquOHepAMjAwZKwiSClr66765822 = -809992805;    double vPquOHepAMjAwZKwiSClr7519485 = -234331996;    double vPquOHepAMjAwZKwiSClr59747005 = -709434435;    double vPquOHepAMjAwZKwiSClr96766226 = -310796102;    double vPquOHepAMjAwZKwiSClr26176844 = -992616511;    double vPquOHepAMjAwZKwiSClr34105527 = -399063214;    double vPquOHepAMjAwZKwiSClr89024958 = -319863816;    double vPquOHepAMjAwZKwiSClr37269571 = -987040971;    double vPquOHepAMjAwZKwiSClr2463097 = -151640750;    double vPquOHepAMjAwZKwiSClr77046285 = -775347166;    double vPquOHepAMjAwZKwiSClr45564759 = -32103979;    double vPquOHepAMjAwZKwiSClr18587670 = -139330107;    double vPquOHepAMjAwZKwiSClr82866585 = -904206895;    double vPquOHepAMjAwZKwiSClr47205735 = -316354859;    double vPquOHepAMjAwZKwiSClr65051043 = -205524363;    double vPquOHepAMjAwZKwiSClr37301032 = -632269479;    double vPquOHepAMjAwZKwiSClr77059248 = -994390716;    double vPquOHepAMjAwZKwiSClr78152983 = -81158525;    double vPquOHepAMjAwZKwiSClr69950247 = -192147632;    double vPquOHepAMjAwZKwiSClr22029251 = -43313080;    double vPquOHepAMjAwZKwiSClr70007842 = -32798711;    double vPquOHepAMjAwZKwiSClr83252075 = -763331825;    double vPquOHepAMjAwZKwiSClr85223869 = 34010542;    double vPquOHepAMjAwZKwiSClr72894082 = -683747857;    double vPquOHepAMjAwZKwiSClr11497154 = -403222050;    double vPquOHepAMjAwZKwiSClr59858162 = -605373743;    double vPquOHepAMjAwZKwiSClr11655811 = -853200176;    double vPquOHepAMjAwZKwiSClr99836953 = -177806306;    double vPquOHepAMjAwZKwiSClr73785683 = -250144840;    double vPquOHepAMjAwZKwiSClr93110198 = -904294466;    double vPquOHepAMjAwZKwiSClr97597450 = -760950886;    double vPquOHepAMjAwZKwiSClr58325027 = -811380554;    double vPquOHepAMjAwZKwiSClr47483412 = -94361620;    double vPquOHepAMjAwZKwiSClr86708228 = -223019743;    double vPquOHepAMjAwZKwiSClr50984644 = -935692540;    double vPquOHepAMjAwZKwiSClr16547059 = -730638144;    double vPquOHepAMjAwZKwiSClr44139421 = -311349219;    double vPquOHepAMjAwZKwiSClr74491441 = -713745131;    double vPquOHepAMjAwZKwiSClr34863392 = -127902196;    double vPquOHepAMjAwZKwiSClr12115656 = -105907276;    double vPquOHepAMjAwZKwiSClr1258377 = -662885922;    double vPquOHepAMjAwZKwiSClr5879997 = 79476732;    double vPquOHepAMjAwZKwiSClr87728875 = -202880589;    double vPquOHepAMjAwZKwiSClr41170971 = -100921458;    double vPquOHepAMjAwZKwiSClr94749622 = -125716158;    double vPquOHepAMjAwZKwiSClr88377639 = -823904019;    double vPquOHepAMjAwZKwiSClr65764538 = -465641637;    double vPquOHepAMjAwZKwiSClr22636143 = -702199087;    double vPquOHepAMjAwZKwiSClr11158884 = 59516171;    double vPquOHepAMjAwZKwiSClr93832837 = -381405174;    double vPquOHepAMjAwZKwiSClr57198424 = -872835201;    double vPquOHepAMjAwZKwiSClr5120154 = -352484287;    double vPquOHepAMjAwZKwiSClr59012685 = -945716621;    double vPquOHepAMjAwZKwiSClr97894442 = -177743422;    double vPquOHepAMjAwZKwiSClr84550830 = -784592493;    double vPquOHepAMjAwZKwiSClr83513747 = 53339019;    double vPquOHepAMjAwZKwiSClr22295616 = -168342538;    double vPquOHepAMjAwZKwiSClr86852923 = 74313421;    double vPquOHepAMjAwZKwiSClr85269072 = -907574052;    double vPquOHepAMjAwZKwiSClr66318681 = -287242769;    double vPquOHepAMjAwZKwiSClr22449716 = -545863038;    double vPquOHepAMjAwZKwiSClr89188004 = -42057511;    double vPquOHepAMjAwZKwiSClr63483888 = -636896131;    double vPquOHepAMjAwZKwiSClr9352899 = -247346285;    double vPquOHepAMjAwZKwiSClr79448835 = 85603720;    double vPquOHepAMjAwZKwiSClr87239732 = -220723425;    double vPquOHepAMjAwZKwiSClr71104258 = 55031513;    double vPquOHepAMjAwZKwiSClr96158356 = -581187153;    double vPquOHepAMjAwZKwiSClr96221091 = -380662319;    double vPquOHepAMjAwZKwiSClr48503984 = -474886219;    double vPquOHepAMjAwZKwiSClr93161611 = -971754121;     vPquOHepAMjAwZKwiSClr81497689 = vPquOHepAMjAwZKwiSClr76046881;     vPquOHepAMjAwZKwiSClr76046881 = vPquOHepAMjAwZKwiSClr4919553;     vPquOHepAMjAwZKwiSClr4919553 = vPquOHepAMjAwZKwiSClr27367871;     vPquOHepAMjAwZKwiSClr27367871 = vPquOHepAMjAwZKwiSClr80870586;     vPquOHepAMjAwZKwiSClr80870586 = vPquOHepAMjAwZKwiSClr87287960;     vPquOHepAMjAwZKwiSClr87287960 = vPquOHepAMjAwZKwiSClr67407105;     vPquOHepAMjAwZKwiSClr67407105 = vPquOHepAMjAwZKwiSClr41266902;     vPquOHepAMjAwZKwiSClr41266902 = vPquOHepAMjAwZKwiSClr17750466;     vPquOHepAMjAwZKwiSClr17750466 = vPquOHepAMjAwZKwiSClr24066544;     vPquOHepAMjAwZKwiSClr24066544 = vPquOHepAMjAwZKwiSClr3886427;     vPquOHepAMjAwZKwiSClr3886427 = vPquOHepAMjAwZKwiSClr71257667;     vPquOHepAMjAwZKwiSClr71257667 = vPquOHepAMjAwZKwiSClr61040235;     vPquOHepAMjAwZKwiSClr61040235 = vPquOHepAMjAwZKwiSClr46221183;     vPquOHepAMjAwZKwiSClr46221183 = vPquOHepAMjAwZKwiSClr90283334;     vPquOHepAMjAwZKwiSClr90283334 = vPquOHepAMjAwZKwiSClr43149567;     vPquOHepAMjAwZKwiSClr43149567 = vPquOHepAMjAwZKwiSClr90191971;     vPquOHepAMjAwZKwiSClr90191971 = vPquOHepAMjAwZKwiSClr18217256;     vPquOHepAMjAwZKwiSClr18217256 = vPquOHepAMjAwZKwiSClr40314382;     vPquOHepAMjAwZKwiSClr40314382 = vPquOHepAMjAwZKwiSClr6965310;     vPquOHepAMjAwZKwiSClr6965310 = vPquOHepAMjAwZKwiSClr48631124;     vPquOHepAMjAwZKwiSClr48631124 = vPquOHepAMjAwZKwiSClr69841878;     vPquOHepAMjAwZKwiSClr69841878 = vPquOHepAMjAwZKwiSClr76209927;     vPquOHepAMjAwZKwiSClr76209927 = vPquOHepAMjAwZKwiSClr31133870;     vPquOHepAMjAwZKwiSClr31133870 = vPquOHepAMjAwZKwiSClr34257673;     vPquOHepAMjAwZKwiSClr34257673 = vPquOHepAMjAwZKwiSClr83273136;     vPquOHepAMjAwZKwiSClr83273136 = vPquOHepAMjAwZKwiSClr28962933;     vPquOHepAMjAwZKwiSClr28962933 = vPquOHepAMjAwZKwiSClr19923693;     vPquOHepAMjAwZKwiSClr19923693 = vPquOHepAMjAwZKwiSClr54558673;     vPquOHepAMjAwZKwiSClr54558673 = vPquOHepAMjAwZKwiSClr66765822;     vPquOHepAMjAwZKwiSClr66765822 = vPquOHepAMjAwZKwiSClr7519485;     vPquOHepAMjAwZKwiSClr7519485 = vPquOHepAMjAwZKwiSClr59747005;     vPquOHepAMjAwZKwiSClr59747005 = vPquOHepAMjAwZKwiSClr96766226;     vPquOHepAMjAwZKwiSClr96766226 = vPquOHepAMjAwZKwiSClr26176844;     vPquOHepAMjAwZKwiSClr26176844 = vPquOHepAMjAwZKwiSClr34105527;     vPquOHepAMjAwZKwiSClr34105527 = vPquOHepAMjAwZKwiSClr89024958;     vPquOHepAMjAwZKwiSClr89024958 = vPquOHepAMjAwZKwiSClr37269571;     vPquOHepAMjAwZKwiSClr37269571 = vPquOHepAMjAwZKwiSClr2463097;     vPquOHepAMjAwZKwiSClr2463097 = vPquOHepAMjAwZKwiSClr77046285;     vPquOHepAMjAwZKwiSClr77046285 = vPquOHepAMjAwZKwiSClr45564759;     vPquOHepAMjAwZKwiSClr45564759 = vPquOHepAMjAwZKwiSClr18587670;     vPquOHepAMjAwZKwiSClr18587670 = vPquOHepAMjAwZKwiSClr82866585;     vPquOHepAMjAwZKwiSClr82866585 = vPquOHepAMjAwZKwiSClr47205735;     vPquOHepAMjAwZKwiSClr47205735 = vPquOHepAMjAwZKwiSClr65051043;     vPquOHepAMjAwZKwiSClr65051043 = vPquOHepAMjAwZKwiSClr37301032;     vPquOHepAMjAwZKwiSClr37301032 = vPquOHepAMjAwZKwiSClr77059248;     vPquOHepAMjAwZKwiSClr77059248 = vPquOHepAMjAwZKwiSClr78152983;     vPquOHepAMjAwZKwiSClr78152983 = vPquOHepAMjAwZKwiSClr69950247;     vPquOHepAMjAwZKwiSClr69950247 = vPquOHepAMjAwZKwiSClr22029251;     vPquOHepAMjAwZKwiSClr22029251 = vPquOHepAMjAwZKwiSClr70007842;     vPquOHepAMjAwZKwiSClr70007842 = vPquOHepAMjAwZKwiSClr83252075;     vPquOHepAMjAwZKwiSClr83252075 = vPquOHepAMjAwZKwiSClr85223869;     vPquOHepAMjAwZKwiSClr85223869 = vPquOHepAMjAwZKwiSClr72894082;     vPquOHepAMjAwZKwiSClr72894082 = vPquOHepAMjAwZKwiSClr11497154;     vPquOHepAMjAwZKwiSClr11497154 = vPquOHepAMjAwZKwiSClr59858162;     vPquOHepAMjAwZKwiSClr59858162 = vPquOHepAMjAwZKwiSClr11655811;     vPquOHepAMjAwZKwiSClr11655811 = vPquOHepAMjAwZKwiSClr99836953;     vPquOHepAMjAwZKwiSClr99836953 = vPquOHepAMjAwZKwiSClr73785683;     vPquOHepAMjAwZKwiSClr73785683 = vPquOHepAMjAwZKwiSClr93110198;     vPquOHepAMjAwZKwiSClr93110198 = vPquOHepAMjAwZKwiSClr97597450;     vPquOHepAMjAwZKwiSClr97597450 = vPquOHepAMjAwZKwiSClr58325027;     vPquOHepAMjAwZKwiSClr58325027 = vPquOHepAMjAwZKwiSClr47483412;     vPquOHepAMjAwZKwiSClr47483412 = vPquOHepAMjAwZKwiSClr86708228;     vPquOHepAMjAwZKwiSClr86708228 = vPquOHepAMjAwZKwiSClr50984644;     vPquOHepAMjAwZKwiSClr50984644 = vPquOHepAMjAwZKwiSClr16547059;     vPquOHepAMjAwZKwiSClr16547059 = vPquOHepAMjAwZKwiSClr44139421;     vPquOHepAMjAwZKwiSClr44139421 = vPquOHepAMjAwZKwiSClr74491441;     vPquOHepAMjAwZKwiSClr74491441 = vPquOHepAMjAwZKwiSClr34863392;     vPquOHepAMjAwZKwiSClr34863392 = vPquOHepAMjAwZKwiSClr12115656;     vPquOHepAMjAwZKwiSClr12115656 = vPquOHepAMjAwZKwiSClr1258377;     vPquOHepAMjAwZKwiSClr1258377 = vPquOHepAMjAwZKwiSClr5879997;     vPquOHepAMjAwZKwiSClr5879997 = vPquOHepAMjAwZKwiSClr87728875;     vPquOHepAMjAwZKwiSClr87728875 = vPquOHepAMjAwZKwiSClr41170971;     vPquOHepAMjAwZKwiSClr41170971 = vPquOHepAMjAwZKwiSClr94749622;     vPquOHepAMjAwZKwiSClr94749622 = vPquOHepAMjAwZKwiSClr88377639;     vPquOHepAMjAwZKwiSClr88377639 = vPquOHepAMjAwZKwiSClr65764538;     vPquOHepAMjAwZKwiSClr65764538 = vPquOHepAMjAwZKwiSClr22636143;     vPquOHepAMjAwZKwiSClr22636143 = vPquOHepAMjAwZKwiSClr11158884;     vPquOHepAMjAwZKwiSClr11158884 = vPquOHepAMjAwZKwiSClr93832837;     vPquOHepAMjAwZKwiSClr93832837 = vPquOHepAMjAwZKwiSClr57198424;     vPquOHepAMjAwZKwiSClr57198424 = vPquOHepAMjAwZKwiSClr5120154;     vPquOHepAMjAwZKwiSClr5120154 = vPquOHepAMjAwZKwiSClr59012685;     vPquOHepAMjAwZKwiSClr59012685 = vPquOHepAMjAwZKwiSClr97894442;     vPquOHepAMjAwZKwiSClr97894442 = vPquOHepAMjAwZKwiSClr84550830;     vPquOHepAMjAwZKwiSClr84550830 = vPquOHepAMjAwZKwiSClr83513747;     vPquOHepAMjAwZKwiSClr83513747 = vPquOHepAMjAwZKwiSClr22295616;     vPquOHepAMjAwZKwiSClr22295616 = vPquOHepAMjAwZKwiSClr86852923;     vPquOHepAMjAwZKwiSClr86852923 = vPquOHepAMjAwZKwiSClr85269072;     vPquOHepAMjAwZKwiSClr85269072 = vPquOHepAMjAwZKwiSClr66318681;     vPquOHepAMjAwZKwiSClr66318681 = vPquOHepAMjAwZKwiSClr22449716;     vPquOHepAMjAwZKwiSClr22449716 = vPquOHepAMjAwZKwiSClr89188004;     vPquOHepAMjAwZKwiSClr89188004 = vPquOHepAMjAwZKwiSClr63483888;     vPquOHepAMjAwZKwiSClr63483888 = vPquOHepAMjAwZKwiSClr9352899;     vPquOHepAMjAwZKwiSClr9352899 = vPquOHepAMjAwZKwiSClr79448835;     vPquOHepAMjAwZKwiSClr79448835 = vPquOHepAMjAwZKwiSClr87239732;     vPquOHepAMjAwZKwiSClr87239732 = vPquOHepAMjAwZKwiSClr71104258;     vPquOHepAMjAwZKwiSClr71104258 = vPquOHepAMjAwZKwiSClr96158356;     vPquOHepAMjAwZKwiSClr96158356 = vPquOHepAMjAwZKwiSClr96221091;     vPquOHepAMjAwZKwiSClr96221091 = vPquOHepAMjAwZKwiSClr48503984;     vPquOHepAMjAwZKwiSClr48503984 = vPquOHepAMjAwZKwiSClr93161611;     vPquOHepAMjAwZKwiSClr93161611 = vPquOHepAMjAwZKwiSClr81497689;}
// Junk Finished
