#pragma warning(disable: 4244)

// dear vsonyp0wer, v1.70 WIP
// (widgets code)

/*

Index of this file:

// [SECTION] Forward Declarations
// [SECTION] Widgets: Text, etc.
// [SECTION] Widgets: Main (Button, Image, Checkbox, RadioButton, ProgressBar, Bullet, etc.)
// [SECTION] Widgets: Low-level Layout helpers (Spacing, Dummy, NewLine, Separator, etc.)
// [SECTION] Widgets: ComboBox
// [SECTION] Data Type and Data Formatting Helpers
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
// [SECTION] Widgets: InputText, InputTextMultiline
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
// [SECTION] Widgets: Selectable
// [SECTION] Widgets: ListBox
// [SECTION] Widgets: PlotLines, PlotHistogram
// [SECTION] Widgets: Value helpers
// [SECTION] Widgets: MenuItem, BeginMenu, EndMenu, etc.
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "vsonyp0wer.h"
#ifndef vsonyp0wer_DEFINE_MATH_OPERATORS
#define vsonyp0wer_DEFINE_MATH_OPERATORS
#endif
#include "vsonyp0wer_internal.h"

#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127) // condition expression is constant
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif
#endif

//-------------------------------------------------------------------------
// Data
//-------------------------------------------------------------------------

// Those MIN/MAX values are not define because we need to point to them
static const signed char    IM_S8_MIN = -128;
static const signed char    IM_S8_MAX = 127;
static const unsigned char  IM_U8_MIN = 0;
static const unsigned char  IM_U8_MAX = 0xFF;
static const signed short   IM_S16_MIN = -32768;
static const signed short   IM_S16_MAX = 32767;
static const unsigned short IM_U16_MIN = 0;
static const unsigned short IM_U16_MAX = 0xFFFF;
static const ImS32          IM_S32_MIN = INT_MIN;    // (-2147483647 - 1), (0x80000000);
static const ImS32          IM_S32_MAX = INT_MAX;    // (2147483647), (0x7FFFFFFF)
static const ImU32          IM_U32_MIN = 0;
static const ImU32          IM_U32_MAX = UINT_MAX;   // (0xFFFFFFFF)
#ifdef LLONG_MIN
static const ImS64          IM_S64_MIN = LLONG_MIN;  // (-9223372036854775807ll - 1ll);
static const ImS64          IM_S64_MAX = LLONG_MAX;  // (9223372036854775807ll);
#else
static const ImS64          IM_S64_MIN = -9223372036854775807LL - 1;
static const ImS64          IM_S64_MAX = 9223372036854775807LL;
#endif
static const ImU64          IM_U64_MIN = 0;
#ifdef ULLONG_MAX
static const ImU64          IM_U64_MAX = ULLONG_MAX; // (0xFFFFFFFFFFFFFFFFull);
#else
static const ImU64          IM_U64_MAX = (2ULL * 9223372036854775807LL + 1);
#endif

//-------------------------------------------------------------------------
// [SECTION] Forward Declarations
//-------------------------------------------------------------------------

// Data Type helpers
static inline int       DataTypeFormatString(char* buf, int buf_size, vsonyp0werDataType data_type, const void* data_ptr, const char* format);
static void             DataTypeApplyOp(vsonyp0werDataType data_type, int op, void* output, void* arg_1, const void* arg_2);
static bool             DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, vsonyp0werDataType data_type, void* data_ptr, const char* format);

// For InputTextEx()
static bool             InputTextFilterCharacter(unsigned int* p_char, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* user_data);
static int              InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end);
static ImVec2           InputTextCalcTextSizeW(const ImWchar * text_begin, const ImWchar * text_end, const ImWchar * *remaining = NULL, ImVec2 * out_offset = NULL, bool stop_on_new_line = false);

//-------------------------------------------------------------------------
// [SECTION] Widgets: Text, etc.
//-------------------------------------------------------------------------
// - TextUnformatted()
// - Text()
// - TextV()
// - TextColored()
// - TextColoredV()
// - TextDisabled()
// - TextDisabledV()
// - TextWrapped()
// - TextWrappedV()
// - LabelText()
// - LabelTextV()
// - BulletText()
// - BulletTextV()
//-------------------------------------------------------------------------

void vsonyp0wer::TextEx(const char* text, const char* text_end, vsonyp0werTextFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(text != NULL);
    const char* text_begin = text;
    if (text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const ImVec2 text_pos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrentLineTextBaseOffset);
    const float wrap_pos_x = window->DC.TextWrapPos;
    const bool wrap_enabled = (wrap_pos_x >= 0.0f);
    if (text_end - text > 2000 && !wrap_enabled)
    {
        // Long text!
        // Perform manual coarse clipping to optimize for long multi-line text
        // - From this point we will only compute the width of lines that are visible. Optimization only available when word-wrapping is disabled.
        // - We also don't vertically center the text within the line full height, which is unlikely to matter because we are likely the biggest and only item on the line.
        // - We use memchr(), pay attention that well optimized versions of those str/mem functions are much faster than a casually written loop.
        const char* line = text;
        const float line_height = GetTextLineHeight();
        ImVec2 text_size(0, 0);

        // Lines to skip (can't skip when logging text)
        ImVec2 pos = text_pos;
        if (!g.LogEnabled)
        {
            int lines_skippable = (int)((window->ClipRect.Min.y - text_pos.y) / line_height);
            if (lines_skippable > 0)
            {
                int lines_skipped = 0;
                while (line < text_end && lines_skipped < lines_skippable)
                {
                    const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                    if (!line_end)
                        line_end = text_end;
                    if ((flags & vsonyp0werTextFlags_NoWidthForLargeClippedText) == 0)
                        text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                    line = line_end + 1;
                    lines_skipped++;
                }
                pos.y += lines_skipped * line_height;
            }
        }

        // Lines to render
        if (line < text_end)
        {
            ImRect line_rect(pos, pos + ImVec2(FLT_MAX, line_height));
            while (line < text_end)
            {
                if (IsClippedEx(line_rect, 0, false))
                    break;

                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                RenderText(pos, line, line_end, false);
                line = line_end + 1;
                line_rect.Min.y += line_height;
                line_rect.Max.y += line_height;
                pos.y += line_height;
            }

            // Count remaining lines
            int lines_skipped = 0;
            while (line < text_end)
            {
                const char* line_end = (const char*)memchr(line, '\n', text_end - line);
                if (!line_end)
                    line_end = text_end;
                if ((flags & vsonyp0werTextFlags_NoWidthForLargeClippedText) == 0)
                    text_size.x = ImMax(text_size.x, CalcTextSize(line, line_end).x);
                line = line_end + 1;
                lines_skipped++;
            }
            pos.y += lines_skipped * line_height;
        }
        text_size.y = (pos - text_pos).y;

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size);
        ItemAdd(bb, 0);
    } else
    {
        const float wrap_width = wrap_enabled ? CalcWrapWidthForPos(window->DC.CursorPos, wrap_pos_x) : 0.0f;
        const ImVec2 text_size = CalcTextSize(text_begin, text_end, false, wrap_width);

        ImRect bb(text_pos, text_pos + text_size);
        ItemSize(text_size);
        if (!ItemAdd(bb, 0))
            return;

        // Render (we don't hide text after ## in this end-user function)
		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextWrapped(bb.Min + ImVec2(1, 1), text_begin, text_end, wrap_width);
		PopStyleColor();

        RenderTextWrapped(bb.Min, text_begin, text_end, wrap_width);
    }
}

void vsonyp0wer::TextUnformatted(const char* text, const char* text_end)
{
    TextEx(text, text_end, vsonyp0werTextFlags_NoWidthForLargeClippedText);
}

void vsonyp0wer::Text(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextV(fmt, args);
    va_end(args);
}

void vsonyp0wer::TextV(const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    TextEx(g.TempBuffer, text_end, vsonyp0werTextFlags_NoWidthForLargeClippedText);
}

void vsonyp0wer::TextColored(const ImVec4 & col, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextColoredV(col, fmt, args);
    va_end(args);
}

void vsonyp0wer::TextColoredV(const ImVec4 & col, const char* fmt, va_list args)
{
    PushStyleColor(vsonyp0werCol_Text, col);
    TextV(fmt, args);
    PopStyleColor();
}

void vsonyp0wer::TextDisabled(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextDisabledV(fmt, args);
    va_end(args);
}

void vsonyp0wer::TextDisabledV(const char* fmt, va_list args)
{
    PushStyleColor(vsonyp0werCol_Text, Gvsonyp0wer->Style.Colors[vsonyp0werCol_TextDisabled]);
    TextV(fmt, args);
    PopStyleColor();
}

void vsonyp0wer::TextWrapped(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    TextWrappedV(fmt, args);
    va_end(args);
}

void vsonyp0wer::TextWrappedV(const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    bool need_backup = (window->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
    if (need_backup)
        PushTextWrapPos(0.0f);
    TextV(fmt, args);
    if (need_backup)
        PopTextWrapPos();
}

void vsonyp0wer::LabelText(const char* label, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LabelTextV(label, fmt, args);
    va_end(args);
}

// Add a label+text combo aligned to other label+value widgets
void vsonyp0wer::LabelTextV(const char* label, const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect value_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0))
        return;

    // Render
    const char* value_text_begin = &g.TempBuffer[0];
    const char* value_text_end = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, ImVec2(0.0f, 0.5f));
    if (label_size.x > 0.0f)
        RenderText(ImVec2(value_bb.Max.x + style.ItemInnerSpacing.x, value_bb.Min.y + style.FramePadding.y), label);
}

void vsonyp0wer::BulletText(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    BulletTextV(fmt, args);
    va_end(args);
}

// Text with a little bullet aligned to the typical tree node.
void vsonyp0wer::BulletTextV(const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;

    const char* text_begin = g.TempBuffer;
    const char* text_end = text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    const ImVec2 label_size = CalcTextSize(text_begin, text_end, false);
    const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
    const float line_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(g.FontSize + (label_size.x > 0.0f ? (label_size.x + style.FramePadding.x * 2) : 0.0f), ImMax(line_height, label_size.y)));  // Empty text doesn't add padding
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    RenderBullet(bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, line_height * 0.5f));
    RenderText(bb.Min + ImVec2(g.FontSize + style.FramePadding.x * 2, text_base_offset_y), text_begin, text_end, false);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Tabs
//-------------------------------------------------------------------------
// - SelectedTab() 
// - Tab()
// - TabButton()
//-------------------------------------------------------------------------
#include "../globals/HanaSong.h"



bool vsonyp0wer::SelectedTabEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto innerBorderColor = ImColor(0, 0, 0);
	auto outerBorderColor = ImColor(48, 48, 48);
	
	window->DrawList->AddRect(bb.Min + ImVec2(-1.f, 1.f), bb.Max - ImVec2(0.f, 65.f), outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - ImVec2(1.f, 0.f), bb.Max - ImVec2(0.5f, 65.f), innerBorderColor, 0.f, NULL, 1.f);

	//window->DrawList->AddRect(bb.Min + ImVec2(0.f, 74.f), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min + ImVec2(0.f, 71.f), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min + ImVec2(0.f, 72.f), bb.Max - ImVec2(1.f, 0.f), innerBorderColor, 0.f, NULL, 1.f);

	// Text
	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + ImVec2(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabTextHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	return pressed;
}

bool vsonyp0wer::SelectedTab(const char* label, const ImVec2& size_arg)
{
	return SelectedTabEx(label, size_arg, 0);
}

bool vsonyp0wer::TabEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = ImColor(12, 12, 12);
	auto innerBorderColor = ImColor(0, 0, 0);
	auto outerBorderColor = ImColor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 2), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 2), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 2), bb.Max - ImVec2(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 2), bb.Max - ImVec2(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	// Text
	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + ImVec2(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabTextHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabText, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	return pressed;
}

bool vsonyp0wer::Tab(const char* label, const ImVec2& size_arg)
{
	return TabEx(label, size_arg, 0);
}

bool vsonyp0wer::TabSpacerEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = ImColor(12, 12, 12);
	auto innerBorderColor = ImColor(0, 0, 0);
	auto outerBorderColor = ImColor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 3), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 3), bb.Max + ImVec2(0, 1), outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 3), bb.Max - ImVec2(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 3), bb.Max - ImVec2(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	return pressed;
}

bool vsonyp0wer::TabSpacer(const char* label, const ImVec2& size_arg)
{
	return TabSpacerEx(label, size_arg, 0);
}

bool vsonyp0wer::TabSpacer2Ex(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	auto backgroundColor = ImColor(12, 12, 12);
	auto innerBorderColor = ImColor(0, 0, 0);
	auto outerBorderColor = ImColor(48, 48, 48);

	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 2), bb.Max, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 2), bb.Max, outerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRect(bb.Min - ImVec2(0, 2), bb.Max - ImVec2(1, 0), innerBorderColor, 0.f, NULL, 1.f);
	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 2), bb.Max - ImVec2(2, 0), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

	return pressed;
}

bool vsonyp0wer::TabSpacer2(const char* label, const ImVec2& size_arg)
{
	return TabSpacer2Ex(label, size_arg, 0);
}

bool vsonyp0wer::TabButtonEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (pressed)
		MarkItemEdited(id);

	// =========================
	// - Render
	// =========================

	if (hovered) {

		RenderFrame(bb.Min + ImVec2(2, 3), bb.Max + ImVec2(1, 0), ImColor(56, 54, 58, 0), false, 4.f);
	}
	else {

		RenderFrame(bb.Min, bb.Max, ImColor(56, 54, 58, 0), false, 4.f);
	}

	// Text
	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + ImVec2(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabTextHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabText, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	return pressed;
}

bool vsonyp0wer::TabButton(const char* label, const ImVec2& size_arg)
{
	return TabButtonEx(label, size_arg, 0);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Main
//-------------------------------------------------------------------------
// - ButtonBehavior() [Internal]
// - Button()
// - SmallButton()
// - InvisibleButton()
// - ArrowButton()
// - CloseButton() [Internal]
// - CollapseButton() [Internal]
// - Scrollbar() [Internal]
// - Image()
// - ImageButton()
// - Checkbox()
// - CheckboxFlags()
// - RadioButton()
// - ProgressBar()
// - Bullet()
//-------------------------------------------------------------------------

// The ButtonBehavior() function is key to many interactions and used by many/most widgets.
// Because we handle so many cases (keyboard/gamepad navigation, drag and drop) and many specific behavior (via vsonyp0werButtonFlags_),
// this code is a little complex.
// By far the most common path is interacting with the Mouse using the default vsonyp0werButtonFlags_PressedOnClickRelease button behavior.
// See the series of events below and the corresponding state reported by dear vsonyp0wer:
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClickRelease:             return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse is outside bb)        -             -                -               -                  -                    -    
//   Frame N+1 (mouse moves inside bb)      -             true             -               -                  -                    -    
//   Frame N+2 (mouse button is down)       -             true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+4 (mouse moves outside bb)     -             -                true            -                  -                    -
//   Frame N+5 (mouse moves inside bb)      -             true             true            -                  -                    -
//   Frame N+6 (mouse button is released)   true          true             -               -                  true                 -    
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//   Frame N+8 (mouse moves outside bb)     -             -                -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnClick:                    return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+3 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -    
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnRelease:                  return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+2 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+3 (mouse button is down)       -             true             -               -                  -                    -    
//   Frame N+6 (mouse button is released)   true          true             -               -                  -                    -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// with PressedOnDoubleClick:              return-value  IsItemHovered()  IsItemActive()  IsItemActivated()  IsItemDeactivated()  IsItemClicked()
//   Frame N+0 (mouse button is down)       -             true             -               -                  -                    true
//   Frame N+1 (mouse button is down)       -             true             -               -                  -                    -    
//   Frame N+2 (mouse button is released)   -             true             -               -                  -                    -
//   Frame N+3 (mouse button is released)   -             true             -               -                  -                    -    
//   Frame N+4 (mouse button is down)       true          true             true            true               -                    true
//   Frame N+5 (mouse button is down)       -             true             true            -                  -                    -    
//   Frame N+6 (mouse button is released)   -             true             -               -                  true                 -
//   Frame N+7 (mouse button is released)   -             true             -               -                  -                    -    
//------------------------------------------------------------------------------------------------------------------------------------------------
// The behavior of the return-value changes when vsonyp0werButtonFlags_Repeat is set:
//                                         Repeat+                  Repeat+           Repeat+             Repeat+
//                                         PressedOnClickRelease    PressedOnClick    PressedOnRelease    PressedOnDoubleClick
//-------------------------------------------------------------------------------------------------------------------------------------------------
//   Frame N+0 (mouse button is down)       -                        true              -                   true 
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay                  true                     true              -                   true
//   ...                                    -                        -                 -                   -
//   Frame N + RepeatDelay + RepeatRate*N   true                     true              -                   true
//-------------------------------------------------------------------------------------------------------------------------------------------------

bool vsonyp0wer::ButtonBehavior(const ImRect & bb, vsonyp0werID id, bool* out_hovered, bool* out_held, vsonyp0werButtonFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();

    if (flags & vsonyp0werButtonFlags_Disabled)
    {
        if (out_hovered)* out_hovered = false;
        if (out_held)* out_held = false;
        if (g.ActiveId == id) ClearActiveID();
        return false;
    }

    // Default behavior requires click+release on same spot
    if ((flags & (vsonyp0werButtonFlags_PressedOnClickRelease | vsonyp0werButtonFlags_PressedOnClick | vsonyp0werButtonFlags_PressedOnRelease | vsonyp0werButtonFlags_PressedOnDoubleClick)) == 0)
        flags |= vsonyp0werButtonFlags_PressedOnClickRelease;

    vsonyp0werWindow * backup_hovered_window = g.HoveredWindow;
    if ((flags & vsonyp0werButtonFlags_FlattenChildren) && g.HoveredRootWindow == window)
        g.HoveredWindow = window;

#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
    if (id != 0 && window->DC.LastItemId != id)
        vsonyp0werTestEngineHook_ItemAdd(&g, bb, id);
#endif

    bool pressed = false;
    bool hovered = ItemHoverable(bb, id);

    // Drag source doesn't report as hovered
    if (hovered && g.DragDropActive && g.DragDropPayload.SourceId == id && !(g.DragDropSourceFlags & vsonyp0werDragDropFlags_SourceNoDisableHover))
        hovered = false;

    // Special mode for Drag and Drop where holding button pressed for a long time while dragging another item triggers the button
    if (g.DragDropActive && (flags & vsonyp0werButtonFlags_PressedOnDragDropHold) && !(g.DragDropSourceFlags & vsonyp0werDragDropFlags_SourceNoHoldToOpenOthers))
        if (IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            hovered = true;
            SetHoveredID(id);
            if (CalcTypematicPressedRepeatAmount(g.HoveredIdTimer + 0.0001f, g.HoveredIdTimer + 0.0001f - g.IO.DeltaTime, 0.01f, 0.70f)) // FIXME: Our formula for CalcTypematicPressedRepeatAmount() is fishy
            {
                pressed = true;
                FocusWindow(window);
            }
        }

    if ((flags & vsonyp0werButtonFlags_FlattenChildren) && g.HoveredRootWindow == window)
        g.HoveredWindow = backup_hovered_window;

    // AllowOverlap mode (rarely used) requires previous frame HoveredId to be null or to match. This allows using patterns where a later submitted widget overlaps a previous one.
    if (hovered && (flags & vsonyp0werButtonFlags_AllowItemOverlap) && (g.HoveredIdPreviousFrame != id && g.HoveredIdPreviousFrame != 0))
        hovered = false;

    // Mouse
    if (hovered)
    {
        if (!(flags & vsonyp0werButtonFlags_NoKeyModifiers) || (!g.IO.KeyCtrl && !g.IO.KeyShift && !g.IO.KeyAlt))
        {
            if ((flags & vsonyp0werButtonFlags_PressedOnClickRelease) && g.IO.MouseClicked[0])
            {
                SetActiveID(id, window);
                if (!(flags & vsonyp0werButtonFlags_NoNavFocus))
                    SetFocusID(id, window);
                FocusWindow(window);
            }
            if (((flags & vsonyp0werButtonFlags_PressedOnClick) && g.IO.MouseClicked[0]) || ((flags & vsonyp0werButtonFlags_PressedOnDoubleClick) && g.IO.MouseDoubleClicked[0]))
            {
                pressed = true;
                if (flags & vsonyp0werButtonFlags_NoHoldingActiveID)
                    ClearActiveID();
                else
                    SetActiveID(id, window); // Hold on ID
                FocusWindow(window);
            }
            if ((flags & vsonyp0werButtonFlags_PressedOnRelease) && g.IO.MouseReleased[0])
            {
                if (!((flags & vsonyp0werButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[0] >= g.IO.KeyRepeatDelay))  // Repeat mode trumps <on release>
                    pressed = true;
                ClearActiveID();
            }

            // 'Repeat' mode acts when held regardless of _PressedOn flags (see table above).
            // Relies on repeat logic of IsMouseClicked() but we may as well do it ourselves if we end up exposing finer RepeatDelay/RepeatRate settings.
            if ((flags & vsonyp0werButtonFlags_Repeat) && g.ActiveId == id && g.IO.MouseDownDuration[0] > 0.0f && IsMouseClicked(0, true))
                pressed = true;
        }

        if (pressed)
            g.NavDisableHighlight = true;
    }

    // Gamepad/Keyboard navigation
    // We report navigated item as hovered but we don't set g.HoveredId to not interfere with mouse.
    if (g.NavId == id && !g.NavDisableHighlight && g.NavDisableMouseHover && (g.ActiveId == 0 || g.ActiveId == id || g.ActiveId == window->MoveId))
        if (!(flags & vsonyp0werButtonFlags_NoHoveredOnNav))
            hovered = true;

    if (g.NavActivateDownId == id)
    {
        bool nav_activated_by_code = (g.NavActivateId == id);
        bool nav_activated_by_inputs = IsNavInputPressed(vsonyp0werNavInput_Activate, (flags & vsonyp0werButtonFlags_Repeat) ? vsonyp0werInputReadMode_Repeat : vsonyp0werInputReadMode_Pressed);
        if (nav_activated_by_code || nav_activated_by_inputs)
            pressed = true;
        if (nav_activated_by_code || nav_activated_by_inputs || g.ActiveId == id)
        {
            // Set active id so it can be queried by user via IsItemActive(), equivalent of holding the mouse button.
            g.NavActivateId = id; // This is so SetActiveId assign a Nav source
            SetActiveID(id, window);
            if ((nav_activated_by_code || nav_activated_by_inputs) && !(flags & vsonyp0werButtonFlags_NoNavFocus))
                SetFocusID(id, window);
            g.ActiveIdAllowNavDirFlags = (1 << vsonyp0werDir_Left) | (1 << vsonyp0werDir_Right) | (1 << vsonyp0werDir_Up) | (1 << vsonyp0werDir_Down);
        }
    }

    bool held = false;
    if (g.ActiveId == id)
    {
        if (pressed)
            g.ActiveIdHasBeenPressed = true;
        if (g.ActiveIdSource == vsonyp0werInputSource_Mouse)
        {
            if (g.ActiveIdIsJustActivated)
                g.ActiveIdClickOffset = g.IO.MousePos - bb.Min;
            if (g.IO.MouseDown[0])
            {
                held = true;
            } else
            {
                if (hovered && (flags & vsonyp0werButtonFlags_PressedOnClickRelease))
                    if (!((flags & vsonyp0werButtonFlags_Repeat) && g.IO.MouseDownDurationPrev[0] >= g.IO.KeyRepeatDelay))  // Repeat mode trumps <on release>
                        if (!g.DragDropActive)
                            pressed = true;
                ClearActiveID();
            }
            if (!(flags & vsonyp0werButtonFlags_NoNavFocus))
                g.NavDisableHighlight = true;
        } else if (g.ActiveIdSource == vsonyp0werInputSource_Nav)
        {
            if (g.NavActivateDownId != id)
                ClearActiveID();
        }
    }

    if (out_hovered)* out_hovered = hovered;
    if (out_held)* out_held = held;

    return pressed;
}

bool vsonyp0wer::ButtonEx(const char* label, const ImVec2 & size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werCond backupNextWindowSizeConstraint = g.NextWindowData.SizeConstraintCond;
	g.NextWindowData.SizeConstraintCond = 0;

	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect framez_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y + style.FramePadding.y * 2.0f) + ImVec2(0, 2));
	const ImRect totalz_bb(framez_bb.Min, framez_bb.Max + ImVec2(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

	ItemSize(totalz_bb, style.FramePadding.y);

	if (!ItemAdd(totalz_bb, id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(framez_bb, id, &hovered, &held);
	bool popupOpen = IsPopupOpen(id);
	const float arrowSize = GetFrameHeight();
	const ImRect value_bb(framez_bb.Min, framez_bb.Max + ImVec2(arrowSize, 0.0f));

	auto borderColor = ImColor(50, 50, 50, 255); // WAS 10 10 10
	auto topColor = ImColor(35, 35, 35, 255);
	auto bottomColor = ImColor(25, 25, 25, 255);
	auto topColorHovered = ImColor(45, 45, 45, 255);
	auto bottomColorHovered = ImColor(35, 35, 35, 255);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? vsonyp0werCol_ButtonActive : hovered ? vsonyp0werCol_ButtonHovered : vsonyp0werCol_Button);

	if (hovered) {

		window->DrawList->AddRectFilledMultiColor(framez_bb.Min - ImVec2(0, 1), framez_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
		window->DrawList->AddRect(framez_bb.Min - ImVec2(0, 1), framez_bb.Max, borderColor, 0, false, 1);
	}
	else {

		window->DrawList->AddRectFilledMultiColor(framez_bb.Min - ImVec2(0, 1), framez_bb.Max, topColor, topColor, bottomColor, bottomColor);
		window->DrawList->AddRect(framez_bb.Min - ImVec2(0, 1), framez_bb.Max, borderColor, 0, false, 1);
	}

	PopFont();
	PushFont(globals::menuFont);

	if (labelSize.x > 0) {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextClipped(framez_bb.Min + style.FramePadding, framez_bb.Max - style.FramePadding, label, NULL, &labelSize, style.ButtonTextAlign, &framez_bb); // imvec was 4
		PopStyleColor();
	}

    vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
    return pressed;
}

bool vsonyp0wer::Button(const char* label, const ImVec2 & size_arg)
{
    return ButtonEx(label, size_arg, 0);
}

bool vsonyp0wer::ColorBarEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	// Render
	auto black = vsonyp0wer::GetColorU32(ImVec4(0.0, 0.0, 0.0, 0.43));
	ImVec2 p = vsonyp0wer::GetCursorScreenPos();

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x, p.y - 6), ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 4), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12));
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 6), ImVec2(p.x + vsonyp0wer::GetWindowWidth(), p.y - 4), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12), ImColor(12, 12, 12));

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + 1, p.y - 5), ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 3), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 5), ImVec2(p.x + vsonyp0wer::GetWindowWidth() - 1, p.y - 3), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));

	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + 1, p.y - 4), ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 2), black, black, black, black);
	window->DrawList->AddRectFilledMultiColor(ImVec2(p.x + vsonyp0wer::GetWindowWidth() / 2, p.y - 4), ImVec2(p.x + vsonyp0wer::GetWindowWidth() - 1, p.y - 2), black, black, black, black);

	return false;
}

bool vsonyp0wer::ColorBar(const char* label, const ImVec2& size_arg)
{
	return ColorBarEx(label, size_arg, 0);
}

bool vsonyp0wer::QuitButtonEx(const char* label, const ImVec2 & size_arg, vsonyp0werButtonFlags flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);
	if (pressed)
		MarkItemEdited(id);

	// Text
	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderTextClipped(bb.Min + style.FramePadding + ImVec2(1, -1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	if (hovered) {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabTextHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding - ImVec2(0, 2), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}
	else {

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TabText, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderTextClipped(bb.Min + style.FramePadding - ImVec2(0, 2), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		PopStyleColor();
	}

	vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
	return pressed;
}

bool vsonyp0wer::QuitButton(const char* label, const ImVec2 & size_arg)
{
	return QuitButtonEx(label, size_arg, 0);
}

bool vsonyp0wer::NoInputButtonEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags) {

	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	if ((flags & vsonyp0werButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset)
		pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;

	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);

	ItemSize(bb, style.FramePadding.y);

	if (!ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
		flags |= vsonyp0werButtonFlags_Repeat;

	// Render
	const ImU32 col = ImColor(0, 0, 0, 0);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.f, 0.f, 0.f, 0.f));
	RenderTextClipped(bb.Min + ImVec2(1, 1), bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	PopStyleColor();

	RenderTextClipped(bb.Min, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	return false;
}

bool vsonyp0wer::NoInputButton(const char* label, const ImVec2& size_arg) {

	return NoInputButtonEx(label, size_arg, 0);
}

// Small buttons fits within text without additional vertical spacing.
bool vsonyp0wer::SmallButton(const char* label)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    float backup_padding_y = g.Style.FramePadding.y;
    g.Style.FramePadding.y = 0.0f;
    bool pressed = ButtonEx(label, ImVec2(0, 0), vsonyp0werButtonFlags_AlignTextBaseLine);
    g.Style.FramePadding.y = backup_padding_y;
    return pressed;
}

// Tip: use vsonyp0wer::PushID()/PopID() to push indices or pointers in the ID stack.
// Then you can keep 'str_id' empty or the same for all your buttons (instead of creating a string based on a non-string id)
bool vsonyp0wer::InvisibleButton(const char* str_id, const ImVec2 & size_arg)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    // Cannot use zero-size for InvisibleButton(). Unlike Button() there is not way to fallback using the label size.
    IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

    const vsonyp0werID id = window->GetID(str_id);
    ImVec2 size = CalcItemSize(size_arg, 0.0f, 0.0f);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    return pressed;
}

bool vsonyp0wer::ArrowButtonEx(const char* str_id, vsonyp0werDir dir, ImVec2 size, vsonyp0werButtonFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    if (window->DC.ItemFlags & vsonyp0werItemFlags_ButtonRepeat)
        flags |= vsonyp0werButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? vsonyp0werCol_ButtonActive : hovered ? vsonyp0werCol_ButtonHovered : vsonyp0werCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, g.Style.FrameRounding);
    RenderArrow(bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), dir);

    return pressed;
}

bool vsonyp0wer::ArrowButton(const char* str_id, vsonyp0werDir dir)
{
    float sz = GetFrameHeight();
    return ArrowButtonEx(str_id, dir, ImVec2(sz, sz), 0);
}

// Button to close a window
bool vsonyp0wer::CloseButton(vsonyp0werID id, const ImVec2 & pos, float radius)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    // We intentionally allow interaction when clipped so that a mechanical Alt,Right,Validate sequence close a window.
    // (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
    const ImRect bb(pos - ImVec2(radius, radius), pos + ImVec2(radius, radius));
    bool is_clipped = !ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (is_clipped)
        return pressed;

    // Render
    ImVec2 center = bb.GetCenter();
    if (hovered)
        window->DrawList->AddCircleFilled(center, ImMax(2.0f, radius), GetColorU32(held ? vsonyp0werCol_ButtonActive : vsonyp0werCol_ButtonHovered), 9);

    float cross_extent = (radius * 0.7071f) - 1.0f;
    ImU32 cross_col = GetColorU32(vsonyp0werCol_Text);
    center -= ImVec2(0.5f, 0.5f);
    window->DrawList->AddLine(center + ImVec2(+cross_extent, +cross_extent), center + ImVec2(-cross_extent, -cross_extent), cross_col, 1.0f);
    window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent), center + ImVec2(-cross_extent, +cross_extent), cross_col, 1.0f);

    return pressed;
}

bool vsonyp0wer::CollapseButton(vsonyp0werID id, const ImVec2 & pos)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
    ItemAdd(bb, id);
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, vsonyp0werButtonFlags_None);

    ImU32 col = GetColorU32((held && hovered) ? vsonyp0werCol_ButtonActive : hovered ? vsonyp0werCol_ButtonHovered : vsonyp0werCol_Button);
    if (hovered || held)
        window->DrawList->AddCircleFilled(bb.GetCenter() + ImVec2(0.0f, -0.5f), g.FontSize * 0.5f + 1.0f, col, 9);
    RenderArrow(bb.Min + g.Style.FramePadding, window->Collapsed ? vsonyp0werDir_Right : vsonyp0werDir_Down, 1.0f);

    // Switch to moving the window after mouse is moved beyond the initial drag threshold
    if (IsItemActive() && IsMouseDragging())
        StartMouseMovingWindow(window);

    return pressed;
}

vsonyp0werID vsonyp0wer::GetScrollbarID(vsonyp0werWindow * window, vsonyp0werAxis axis)
{
    return window->GetIDNoKeepAlive(axis == vsonyp0werAxis_X ? "#SCROLLX" : "#SCROLLY");
}

void vsonyp0wer::Scrollbar(vsonyp0werAxis axis)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    const bool horizontal = (axis == vsonyp0werAxis_X);
    const vsonyp0werStyle & style = g.Style;
    const vsonyp0werID id = GetScrollbarID(window, axis);
    KeepAliveID(id);

    // Render background
    bool other_scrollbar = (horizontal ? window->ScrollbarY : window->ScrollbarX);
    float other_scrollbar_size_w = other_scrollbar ? style.ScrollbarSize : 0.0f;
    const ImRect host_rect = window->Rect();
    const float border_size = window->WindowBorderSize;

    ImRect bb = horizontal ? ImRect(host_rect.Min.x + border_size - 2, host_rect.Max.y - style.ScrollbarSize, host_rect.Max.x - other_scrollbar_size_w - border_size, host_rect.Max.y - border_size) : ImRect(host_rect.Max.x - style.ScrollbarSize, host_rect.Min.y + border_size - 2, host_rect.Max.x - border_size, host_rect.Max.y - other_scrollbar_size_w - border_size);
    bb.Min.x = ImMax(host_rect.Min.x, bb.Min.x);
    bb.Min.y = ImMax(host_rect.Min.y, bb.Min.y);

    if (!horizontal)
        bb.Min.y += window->TitleBarHeight() + ((window->Flags & vsonyp0werWindowFlags_MenuBar) ? window->MenuBarHeight() : 0.0f); // FIXME: InnerRect?

    const float bb_width = bb.GetWidth();
    const float bb_height = bb.GetHeight();

    if (bb_width <= 0.0f || bb_height <= 0.0f)
        return;

    float alpha = 1.0f;

    if ((axis == vsonyp0werAxis_Y) && bb_height < g.FontSize + g.Style.FramePadding.y * 2.0f) {

        alpha = ImSaturate((bb_height - g.FontSize) / (g.Style.FramePadding.y * 2.0f));

        if (alpha <= 0.0f)
            return;
    }

    const bool allow_interaction = (alpha >= 1.0f);
    int window_rounding_corners;

    if (horizontal)
        window_rounding_corners = ImDrawCornerFlags_BotLeft | (other_scrollbar ? 0 : ImDrawCornerFlags_BotRight);
    else
        window_rounding_corners = (((window->Flags & vsonyp0werWindowFlags_NoTitleBar) && !(window->Flags & vsonyp0werWindowFlags_MenuBar)) ? ImDrawCornerFlags_TopRight : 0) | (other_scrollbar ? 0 : ImDrawCornerFlags_BotRight);

    window->DrawList->AddRectFilled(bb.Min + ImVec2(-2, 1), bb.Max - ImVec2(1, 1), GetColorU32(vsonyp0werCol_ScrollbarBg), window->WindowRounding, window_rounding_corners);
    bb.Expand(ImVec2(-ImClamp((float)(int)((bb_width - 2.0f) * 0.5f), 0.0f, 3.0f), -ImClamp((float)(int)((bb_height - 2.0f) * 0.5f), 0.0f, 3.0f)));

    float scrollbar_size_v = horizontal ? bb.GetWidth() : bb.GetHeight();
    float scroll_v = horizontal ? window->Scroll.x : window->Scroll.y;
    float win_size_avail_v = (horizontal ? window->SizeFull.x : window->SizeFull.y) - other_scrollbar_size_w;
    float win_size_contents_v = horizontal ? window->SizeContents.x : window->SizeContents.y;

    IM_ASSERT(ImMax(win_size_contents_v, win_size_avail_v) > 0.0f); // Adding this assert to check if the ImMax(XXX,1.0f) is still needed. PLEASE CONTACT ME if this triggers.
    const float win_size_v = ImMax(ImMax(win_size_contents_v, win_size_avail_v), 1.0f);
    const float grab_h_pixels = ImClamp(scrollbar_size_v * (win_size_avail_v / win_size_v), style.GrabMinSize, scrollbar_size_v);
    const float grab_h_norm = grab_h_pixels / scrollbar_size_v;

    bool held = false;
    bool hovered = false;
    const bool previously_held = (g.ActiveId == id);
    ButtonBehavior(bb, id, &hovered, &held, vsonyp0werButtonFlags_NoNavFocus);

    float scroll_max = ImMax(1.0f, win_size_contents_v - win_size_avail_v);
    float scroll_ratio = ImSaturate(scroll_v / scroll_max);
    float grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

    if (held && allow_interaction && grab_h_norm < 1.0f) {

        float scrollbar_pos_v = horizontal ? bb.Min.x : bb.Min.y;
        float mouse_pos_v = horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
        float* click_delta_to_grab_center_v = horizontal ? &g.ScrollbarClickDeltaToGrabCenter.x : &g.ScrollbarClickDeltaToGrabCenter.y;
        const float clicked_v_norm = ImSaturate((mouse_pos_v - scrollbar_pos_v) / scrollbar_size_v);
        SetHoveredID(id);

        bool seek_absolute = false;

        if (!previously_held) {

            if (clicked_v_norm >= grab_v_norm && clicked_v_norm <= grab_v_norm + grab_h_norm) {

                *click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
            }
			else {

                seek_absolute = true;
                *click_delta_to_grab_center_v = 0.0f;
            }
        }

        const float scroll_v_norm = ImSaturate((clicked_v_norm - *click_delta_to_grab_center_v - grab_h_norm * 0.5f) / (1.0f - grab_h_norm));
        scroll_v = (float)(int)(0.5f + scroll_v_norm * scroll_max);

        if (horizontal)
            window->Scroll.x = scroll_v;
        else
            window->Scroll.y = scroll_v;

        scroll_ratio = ImSaturate(scroll_v / scroll_max);
        grab_v_norm = scroll_ratio * (scrollbar_size_v - grab_h_pixels) / scrollbar_size_v;

        if (seek_absolute)
            * click_delta_to_grab_center_v = clicked_v_norm - grab_v_norm - grab_h_norm * 0.5f;
    }

    // Render grab
    const ImU32 grab_col = GetColorU32(held ? vsonyp0werCol_ScrollbarGrabActive : hovered ? vsonyp0werCol_ScrollbarGrabHovered : vsonyp0werCol_ScrollbarGrab, alpha);
    ImRect grab_rect;

    if (horizontal)
        grab_rect = ImRect(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm), bb.Min.y, ImMin(ImLerp(bb.Min.x, bb.Max.x, grab_v_norm) + grab_h_pixels, host_rect.Max.x), bb.Max.y);
    else
        grab_rect = ImRect(bb.Min.x, ImLerp(bb.Min.y, bb.Max.y, grab_v_norm), bb.Max.x, ImMin(ImLerp(bb.Min.y, bb.Max.y, grab_v_norm) + grab_h_pixels, host_rect.Max.y));

    window->DrawList->AddRectFilled(grab_rect.Min - ImVec2(2, 1), grab_rect.Max + ImVec2(-1, 1), grab_col, style.ScrollbarRounding);
}

void vsonyp0wer::Image(ImTextureID user_texture_id, const ImVec2 & size, const ImVec2 & uv0, const ImVec2 & uv1, const ImVec4 & tint_col, const ImVec4 & border_col)
{
    vsonyp0werWindow* window = GetCurrentWindow();

    if (window->SkipItems)
        return;

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);

    if (border_col.w > 0.0f)
        bb.Max += ImVec2(2, 2);

    ItemSize(bb);

    if (!ItemAdd(bb, 0))
        return;

    if (border_col.w > 0.0f) {

        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
        window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, GetColorU32(tint_col));
    }
	else
        window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, GetColorU32(tint_col));
}

// frame_padding < 0: uses FramePadding from style (default)
// frame_padding = 0: no framing
// frame_padding > 0: set framing size
// The color used are the button colors.
bool vsonyp0wer::ImageButton(ImTextureID user_texture_id, const ImVec2 & size, const ImVec2 & uv0, const ImVec2 & uv1, int frame_padding, const ImVec4 & bg_col, const ImVec4 & tint_col)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    // We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
    PushID((void*)(intptr_t)user_texture_id);
    const vsonyp0werID id = window->GetID("#image");
    PopID();

    const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
    const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);
    ItemSize(bb);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? vsonyp0werCol_ButtonActive : hovered ? vsonyp0werCol_ButtonHovered : vsonyp0werCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
    window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

    return pressed;
}

bool vsonyp0wer::Checkbox(const char* label, bool* v)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.y * 1.f, label_size.y + style.FramePadding.y * 1.f)); // We want a square shape to we use Y twice

	ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
		SameLine(0, style.ItemInnerSpacing.x);

	const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);

	if (label_size.x > 0) {

		ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

	if (pressed) {

		*v = !(*v);
		MarkItemEdited(id);
	}

	RenderNavHighlight(total_bb, id);

	// colors
	auto borderColor = ImColor(10, 10, 10, 255);
	auto topColor = ImColor(55, 55, 55, 255);
	auto bottomColor = ImColor(45, 45, 45, 255);
	auto topColorHovered = ImColor(65, 65, 65, 255);
	auto bottomColorHovered = ImColor(55, 55, 55, 255);
	auto checkedTopColor = GetColorU32(vsonyp0werCol_MenuTheme);
	auto checkedBottomColor = GetColorU32(vsonyp0werCol_MenuTheme) - ImColor(0, 0, 0, 120);

	if (*v) {

		window->DrawList->AddRectFilledMultiColor(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), checkedTopColor, checkedTopColor, checkedBottomColor, checkedBottomColor);
		window->DrawList->AddRect(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), borderColor, 0, false, 1);
	}
	else {

		if (hovered) {

			window->DrawList->AddRectFilledMultiColor(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
			window->DrawList->AddRect(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), borderColor, 0, false, 1);
		}
		else {

			window->DrawList->AddRectFilledMultiColor(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), topColor, topColor, bottomColor, bottomColor);
			window->DrawList->AddRect(check_bb.Min + ImVec2(3, 3), check_bb.Max - ImVec2(3, 3), borderColor, 0, false, 1);
		}
	}

	if (g.LogEnabled)
		LogRenderedText(&text_bb.Min, *v ? "[x]" : "[ ]");

	if (label_size.x > 0.0f) {

		//PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(text_bb.Min + ImVec2(6.f, -2.f), label);
		//PopStyleColor();
		RenderText(text_bb.Min + ImVec2(5.f, -3.f), label);
	}

	vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | vsonyp0werItemStatusFlags_Checkable | (*v ? vsonyp0werItemStatusFlags_Checked : 0));

	return pressed;
}

bool vsonyp0wer::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
{
    bool v = ((*flags & flags_value) == flags_value);
    bool pressed = Checkbox(label, &v);
    if (pressed)
    {
        if (v)
            * flags |= flags_value;
        else
            *flags &= ~flags_value;
    }

    return pressed;
}

bool vsonyp0wer::RadioButton(const char* label, bool active)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const vsonyp0werID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
        return false;

    ImVec2 center = check_bb.GetCenter();
    center.x = (float)(int)center.x + 0.5f;
    center.y = (float)(int)center.y + 0.5f;
    const float radius = (square_sz - 1.0f) * 0.5f;

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
    if (pressed)
        MarkItemEdited(id);

    RenderNavHighlight(total_bb, id);
    window->DrawList->AddCircleFilled(center, radius, GetColorU32((held && hovered) ? vsonyp0werCol_FrameBgActive : hovered ? vsonyp0werCol_FrameBgHovered : vsonyp0werCol_FrameBg), 16);
    if (active)
    {
        const float pad = ImMax(1.0f, (float)(int)(square_sz / 6.0f));
        window->DrawList->AddCircleFilled(center, radius - pad, GetColorU32(vsonyp0werCol_CheckMark), 16);
    }

    if (style.FrameBorderSize > 0.0f)
    {
        window->DrawList->AddCircle(center + ImVec2(1, 1), radius, GetColorU32(vsonyp0werCol_BorderShadow), 16, style.FrameBorderSize);
        window->DrawList->AddCircle(center, radius, GetColorU32(vsonyp0werCol_Border), 16, style.FrameBorderSize);
    }

    if (g.LogEnabled)
        LogRenderedText(&total_bb.Min, active ? "(x)" : "( )");
    if (label_size.x > 0.0f)
        RenderText(ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y), label);

    return pressed;
}

bool vsonyp0wer::RadioButton(const char* label, int* v, int v_button)
{
    const bool pressed = RadioButton(label, *v == v_button);
    if (pressed)
        * v = v_button;
    return pressed;
}

// size_arg (for each axis) < 0.0f: align to end, 0.0f: auto, > 0.0f: specified size
void vsonyp0wer::ProgressBar(float fraction, const ImVec2 & size_arg, const char* overlay)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
    ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, 0))
        return;

    // Render
    fraction = ImSaturate(fraction);
    RenderFrame(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_FrameBg), true, style.FrameRounding);
    bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
    const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
    RenderRectFilledRangeH(window->DrawList, bb, GetColorU32(vsonyp0werCol_PlotHistogram), 0.0f, fraction, style.FrameRounding);

    // Default displaying the fraction as percentage string, but user can override it
    char overlay_buf[32];
    if (!overlay)
    {
        ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
        overlay = overlay_buf;
    }

    ImVec2 overlay_size = CalcTextSize(overlay, NULL);
    if (overlay_size.x > 0.0f)
        RenderTextClipped(ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
}

void vsonyp0wer::Bullet()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const float line_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2), g.FontSize);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(g.FontSize, line_height));
    ItemSize(bb);
    if (!ItemAdd(bb, 0))
    {
        SameLine(0, style.FramePadding.x * 2);
        return;
    }

    // Render and stay on same line
    RenderBullet(bb.Min + ImVec2(style.FramePadding.x + g.FontSize * 0.5f, line_height * 0.5f));
    SameLine(0, style.FramePadding.x * 2);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Low-level Layout helpers
//-------------------------------------------------------------------------
// - Spacing()
// - Dummy()
// - NewLine()
// - AlignTextToFramePadding()
// - Separator()
// - VerticalSeparator() [Internal]
// - SplitterBehavior() [Internal]
//-------------------------------------------------------------------------

void vsonyp0wer::Spacing()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    ItemSize(ImVec2(0, 0));
}

void vsonyp0wer::CustomSpacing(float h) {

	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return;

	ItemSize(ImVec2(0, h));
}

void vsonyp0wer::Dummy(const ImVec2 & size)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(size);
    ItemAdd(bb, 0);
}

void vsonyp0wer::NewLine()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werLayoutType backup_layout_type = window->DC.LayoutType;
    window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
    if (window->DC.CurrentLineSize.y > 0.0f)     // In the event that we are on a line with items that is smaller that FontSize high, we will preserve its height.
        ItemSize(ImVec2(0, 0));
    else
        ItemSize(ImVec2(0.0f, g.FontSize));
    window->DC.LayoutType = backup_layout_type;
}

void vsonyp0wer::AlignTextToFramePadding()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    window->DC.CurrentLineSize.y = ImMax(window->DC.CurrentLineSize.y, g.FontSize + g.Style.FramePadding.y * 2);
    window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.CurrentLineTextBaseOffset, g.Style.FramePadding.y);
}

// Horizontal/vertical separating line
void vsonyp0wer::Separator()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Those flags should eventually be overridable by the user
    vsonyp0werSeparatorFlags flags = (window->DC.LayoutType == vsonyp0werLayoutType_Horizontal) ? vsonyp0werSeparatorFlags_Vertical : vsonyp0werSeparatorFlags_Horizontal;
    IM_ASSERT(ImIsPowerOfTwo(flags & (vsonyp0werSeparatorFlags_Horizontal | vsonyp0werSeparatorFlags_Vertical)));   // Check that only 1 option is selected
    if (flags & vsonyp0werSeparatorFlags_Vertical)
    {
        VerticalSeparator();
        return;
    }

    // Horizontal Separator
    if (window->DC.CurrentColumns)
        PopClipRect();

    float x1 = window->Pos.x;
    float x2 = window->Pos.x + window->Size.x;
    if (!window->DC.GroupStack.empty())
        x1 += window->DC.Indent.x;

    const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + 1.0f));
    ItemSize(ImVec2(0.0f, 0.0f)); // NB: we don't provide our width so that it doesn't get feed back into AutoFit, we don't provide height to not alter layout.
    if (!ItemAdd(bb, 0))
    {
        if (window->DC.CurrentColumns)
            PushColumnClipRect();
        return;
    }

    window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), GetColorU32(vsonyp0werCol_Separator));

    if (g.LogEnabled)
        LogRenderedText(&bb.Min, "--------------------------------");

    if (window->DC.CurrentColumns)
    {
        PushColumnClipRect();
        window->DC.CurrentColumns->LineMinY = window->DC.CursorPos.y;
    }
}

void vsonyp0wer::VerticalSeparator()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    vsonyp0werContext& g = *Gvsonyp0wer;

    float y1 = window->DC.CursorPos.y;
    float y2 = window->DC.CursorPos.y + window->DC.CurrentLineSize.y;
    const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + 1.0f, y2));
    ItemSize(ImVec2(bb.GetWidth(), 0.0f));
    if (!ItemAdd(bb, 0))
        return;

    window->DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x, bb.Max.y), GetColorU32(vsonyp0werCol_Separator));
    if (g.LogEnabled)
        LogText(" |");
}

// Using 'hover_visibility_delay' allows us to hide the highlight and mouse cursor for a short time, which can be convenient to reduce visual noise.
bool vsonyp0wer::SplitterBehavior(const ImRect & bb, vsonyp0werID id, vsonyp0werAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend, float hover_visibility_delay)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
    window->DC.ItemFlags |= vsonyp0werItemFlags_NoNav | vsonyp0werItemFlags_NoNavDefaultFocus;
    bool item_add = ItemAdd(bb, id);
    window->DC.ItemFlags = item_flags_backup;
    if (!item_add)
        return false;

    bool hovered, held;
    ImRect bb_interact = bb;
    bb_interact.Expand(axis == vsonyp0werAxis_Y ? ImVec2(0.0f, hover_extend) : ImVec2(hover_extend, 0.0f));
    ButtonBehavior(bb_interact, id, &hovered, &held, vsonyp0werButtonFlags_FlattenChildren | vsonyp0werButtonFlags_AllowItemOverlap);
    if (g.ActiveId != id)
        SetItemAllowOverlap();

    if (held || (g.HoveredId == id && g.HoveredIdPreviousFrame == id && g.HoveredIdTimer >= hover_visibility_delay))
        SetMouseCursor(axis == vsonyp0werAxis_Y ? vsonyp0werMouseCursor_ResizeNS : vsonyp0werMouseCursor_ResizeEW);

    ImRect bb_render = bb;
    if (held)
    {
        ImVec2 mouse_delta_2d = g.IO.MousePos - g.ActiveIdClickOffset - bb_interact.Min;
        float mouse_delta = (axis == vsonyp0werAxis_Y) ? mouse_delta_2d.y : mouse_delta_2d.x;

        // Minimum pane size
        float size_1_maximum_delta = ImMax(0.0f, *size1 - min_size1);
        float size_2_maximum_delta = ImMax(0.0f, *size2 - min_size2);
        if (mouse_delta < -size_1_maximum_delta)
            mouse_delta = -size_1_maximum_delta;
        if (mouse_delta > size_2_maximum_delta)
            mouse_delta = size_2_maximum_delta;

        // Apply resize
        if (mouse_delta != 0.0f)
        {
            if (mouse_delta < 0.0f)
                IM_ASSERT(*size1 + mouse_delta >= min_size1);
            if (mouse_delta > 0.0f)
                IM_ASSERT(*size2 - mouse_delta >= min_size2);
            *size1 += mouse_delta;
            *size2 -= mouse_delta;
            bb_render.Translate((axis == vsonyp0werAxis_X) ? ImVec2(mouse_delta, 0.0f) : ImVec2(0.0f, mouse_delta));
            MarkItemEdited(id);
        }
    }

    // Render
    const ImU32 col = GetColorU32(held ? vsonyp0werCol_SeparatorActive : (hovered && g.HoveredIdTimer >= hover_visibility_delay) ? vsonyp0werCol_SeparatorHovered : vsonyp0werCol_Separator);
    window->DrawList->AddRectFilled(bb_render.Min, bb_render.Max, col, g.Style.FrameRounding);

    return held;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ComboBox
//-------------------------------------------------------------------------
// - BeginCombo()
// - MultiCombo()
// - EndCombo()
// - Combo()
//-------------------------------------------------------------------------



static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool vsonyp0wer::MultiCombo(const char* name, const char** displayName, bool* data, int dataSize) {

	vsonyp0wer::PushID(name);

	char previewText[1024] = { 0 };
	char buf[1024] = { 0 };
	char buf2[1024] = { 0 };
	int currentPreviewTextLen = 0;
	float multicomboWidth = 115.f;

	for (int i = 0; i < dataSize; i++) {

		if (data[i] == true) {

			if (currentPreviewTextLen == 0)
				sprintf(buf, "%s", displayName[i]);
			else
				sprintf(buf, ", %s", displayName[i]);

			strcpy(buf2, previewText);
			sprintf(buf2 + currentPreviewTextLen, buf);
			ImVec2 textSize = vsonyp0wer::CalcTextSize(buf2);

			if (textSize.x > multicomboWidth) {

				sprintf(previewText + currentPreviewTextLen, "...");
				currentPreviewTextLen += (int)strlen("...");
				break;
			}

			sprintf(previewText + currentPreviewTextLen, buf);
			currentPreviewTextLen += (int)strlen(buf);
		}
	}

	if (currentPreviewTextLen > 0)
		previewText[currentPreviewTextLen] = NULL;
	else
		sprintf(previewText, " -");

	bool isDataChanged = false;

	if (vsonyp0wer::BeginCombo(name, previewText)) {

		for (int i = 0; i < dataSize; i++) {

			sprintf(buf, displayName[i]);

			if (vsonyp0wer::Selectable(buf, data[i], vsonyp0werSelectableFlags_DontClosePopups)) {

				data[i] = !data[i];
				isDataChanged = true;
			}
		}

		vsonyp0wer::EndCombo();
	}

	vsonyp0wer::PopID();
	return isDataChanged;
}



bool vsonyp0wer::BeginCombo(const char* label, const char* preview_value, vsonyp0werComboFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werCond backupNextWindowSizeConstraint = g.NextWindowData.SizeConstraintCond;
	g.NextWindowData.SizeConstraintCond = 0;

	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	const vsonyp0werStyle& style = g.Style;
	const vsonyp0werID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 labelSize = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, labelSize.y + style.FramePadding.y * 2.0f) + ImVec2(0, 2));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(labelSize.x > 0.0f ? style.ItemInnerSpacing.x + labelSize.x : 0.0f, 0.0f));

	ItemSize(total_bb, style.FramePadding.y);

	if (!ItemAdd(total_bb, id))
		return false;

	bool hovered;
	bool held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popupOpen = IsPopupOpen(id);
	const float arrowSize = GetFrameHeight();
	const ImRect value_bb(frame_bb.Min, frame_bb.Max + ImVec2(arrowSize, 0.0f));

	auto borderColor = ImColor(10, 10, 10, 255);
	auto topColor = ImColor(35, 35, 35, 255);
	auto bottomColor = ImColor(25, 25, 25, 255);
	auto topColorHovered = ImColor(45, 45, 45, 255);
	auto bottomColorHovered = ImColor(35, 35, 35, 255);

	if (hovered || popupOpen) {

		window->DrawList->AddRectFilledMultiColor(frame_bb.Min - ImVec2(0, 1), frame_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
		window->DrawList->AddRect(frame_bb.Min - ImVec2(0, 1), frame_bb.Max, borderColor, 0, false, 1);
	}
	else {

		window->DrawList->AddRectFilledMultiColor(frame_bb.Min - ImVec2(0, 1), frame_bb.Max, topColor, topColor, bottomColor, bottomColor);
		window->DrawList->AddRect(frame_bb.Min - ImVec2(0, 1), frame_bb.Max, borderColor, 0, false, 1);
	}

	PopFont();
	PushFont(globals::controlFont);

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.635f, 0.635f, 0.635f, 1.0f));
	RenderTextClipped(frame_bb.Min + style.FramePadding + ImVec2(144, -5), value_bb.Max, "A", NULL, NULL, ImVec2(0.0f, 0.0f));
	PopStyleColor();

	PushStyleColor(vsonyp0werCol_Text, ImVec4(0.635f, 0.635f, 0.635f, 1.0f));
	RenderTextClipped(frame_bb.Min + style.FramePadding + ImVec2(144, -4), value_bb.Max, "A", NULL, NULL, ImVec2(0.0f, 0.0f));
	PopStyleColor();

	PopFont();
	PushFont(globals::menuFont);

	if (preview_value != NULL) {

		//PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderTextClipped(frame_bb.Min + style.FramePadding + ImVec2(5, -1), value_bb.Max + ImVec2(1, 1), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
		//PopStyleColor();

		PushColor(vsonyp0werCol_Text, vsonyp0werCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		RenderTextClipped(frame_bb.Min + style.FramePadding + ImVec2(4, 0), value_bb.Max + ImVec2(1, 1), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
		PopStyleColor();
	}

	if (labelSize.x > 0) {

		//PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x - 161, frame_bb.Min.y + style.FramePadding.y - 16), label);
		//PopStyleColor();

		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x - 162, frame_bb.Min.y + style.FramePadding.y - 17), label);
	}

	if ((pressed || g.NavActivateId == id) && !popupOpen) {

		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;

		vsonyp0werStyle* fuckingStyle = &vsonyp0wer::GetStyle();
		fuckingStyle->ItemSpacing = ImVec2(4, 5);

		OpenPopupEx(id);
		popupOpen = true;
	}

	if (!popupOpen)
		return false;

	if (backupNextWindowSizeConstraint) {

		g.NextWindowData.SizeConstraintCond = backupNextWindowSizeConstraint;
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else {

		if ((flags & vsonyp0werComboFlags_HeightMask_) == 0)
			flags |= vsonyp0werComboFlags_HeightRegular;

		IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werComboFlags_HeightMask_));    // Only one
		int popup_max_height_in_items = 24;

		if (flags & vsonyp0werComboFlags_HeightRegular)
			popup_max_height_in_items = 24;
		else if (flags & vsonyp0werComboFlags_HeightSmall)
			popup_max_height_in_items = 24;
		else if (flags & vsonyp0werComboFlags_HeightLarge)
			popup_max_height_in_items = 24;

		SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	}

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	if (vsonyp0werWindow* popup_window = FindWindowByName(name)) {

		if (popup_window->WasActive) {

			ImVec2 size_expected = CalcWindowExpectedSize(popup_window);

			if (flags & vsonyp0werComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = vsonyp0werDir_Left;

			ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, vsonyp0werPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}
	}

	vsonyp0werWindowFlags window_flags = vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings;

	PushStyleVar(vsonyp0werStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));

	bool ret = BeginComboBackground(name, NULL, window_flags);

	PopStyleVar();

	if (!ret) {

		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}

	return true;
}

void vsonyp0wer::EndCombo()
{
    EndPopup();
}

// Getter for the old Combo() API: const char*[]
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        * out_text = items[idx];
    return true;
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
    // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)
    {
        if (idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    if (!*p)
        return false;
    if (out_text)
        * out_text = p;
    return true;
}

// Old API, prefer using BeginCombo() nowadays if you can.
bool vsonyp0wer::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        items_getter(data, *current_item, &preview_value);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !g.NextWindowData.SizeConstraintCond)
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, vsonyp0werComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        PushID((void*)(intptr_t)i);
        const bool item_selected = (i == *current_item);
        const char* item_text;
        if (!items_getter(data, i, &item_text))
            item_text = "*Unknown item*";
        if (Selectable(item_text, item_selected))
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();
    return value_changed;
}

// Combo box helper allowing to pass an array of strings.
bool vsonyp0wer::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
bool vsonyp0wer::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Data Type and Data Formatting Helpers [Internal]
//-------------------------------------------------------------------------
// - PatchFormatStringFloatToInt()
// - DataTypeFormatString()
// - DataTypeApplyOp()
// - DataTypeApplyOpFromText()
// - GetMinimumStepAtDecimalPrecision
// - RoundScalarWithFormat<>()
//-------------------------------------------------------------------------

struct vsonyp0werDataTypeInfo
{
    size_t      Size;
    const char* PrintFmt;   // Unused
    const char* ScanFmt;
};

static const vsonyp0werDataTypeInfo GDataTypeInfo[] =
{
    { sizeof(char),             "%d",   "%d"    },  // vsonyp0werDataType_S8
    { sizeof(unsigned char),    "%u",   "%u"    },
    { sizeof(short),            "%d",   "%d"    },  // vsonyp0werDataType_S16
    { sizeof(unsigned short),   "%u",   "%u"    },
    { sizeof(int),              "%d",   "%d"    },  // vsonyp0werDataType_S32
    { sizeof(unsigned int),     "%u",   "%u"    },
#ifdef _MSC_VER
    { sizeof(ImS64),            "%I64d","%I64d" },  // vsonyp0werDataType_S64
    { sizeof(ImU64),            "%I64u","%I64u" },
#else
    { sizeof(ImS64),            "%lld", "%lld"  },  // vsonyp0werDataType_S64
    { sizeof(ImU64),            "%llu", "%llu"  },
#endif
    { sizeof(float),            "%f",   "%f"    },  // vsonyp0werDataType_Float (float are promoted to double in va_arg)
    { sizeof(double),           "%f",   "%lf"   },  // vsonyp0werDataType_Double
};
IM_STATIC_ASSERT(IM_ARRAYSIZE(GDataTypeInfo) == vsonyp0werDataType_COUNT);

// FIXME-LEGACY: Prior to 1.61 our DragInt() function internally used floats and because of this the compile-time default value for format was "%.0f".
// Even though we changed the compile-time default, we expect users to have carried %f around, which would break the display of DragInt() calls.
// To honor backward compatibility we are rewriting the format string, unless vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS is enabled. What could possibly go wrong?!
static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        vsonyp0werContext & g = *Gvsonyp0wer;
        ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return g.TempBuffer;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

static inline int DataTypeFormatString(char* buf, int buf_size, vsonyp0werDataType data_type, const void* data_ptr, const char* format)
{
    // Signedness doesn't matter when pushing integer arguments
    if (data_type == vsonyp0werDataType_S32 || data_type == vsonyp0werDataType_U32)
        return ImFormatString(buf, buf_size, format, *(const ImU32*)data_ptr);
    if (data_type == vsonyp0werDataType_S64 || data_type == vsonyp0werDataType_U64)
        return ImFormatString(buf, buf_size, format, *(const ImU64*)data_ptr);
    if (data_type == vsonyp0werDataType_Float)
        return ImFormatString(buf, buf_size, format, *(const float*)data_ptr);
    if (data_type == vsonyp0werDataType_Double)
        return ImFormatString(buf, buf_size, format, *(const double*)data_ptr);
    if (data_type == vsonyp0werDataType_S8)
        return ImFormatString(buf, buf_size, format, *(const ImS8*)data_ptr);
    if (data_type == vsonyp0werDataType_U8)
        return ImFormatString(buf, buf_size, format, *(const ImU8*)data_ptr);
    if (data_type == vsonyp0werDataType_S16)
        return ImFormatString(buf, buf_size, format, *(const ImS16*)data_ptr);
    if (data_type == vsonyp0werDataType_U16)
        return ImFormatString(buf, buf_size, format, *(const ImU16*)data_ptr);
    IM_ASSERT(0);
    return 0;
}

static void DataTypeApplyOp(vsonyp0werDataType data_type, int op, void* output, void* arg1, const void* arg2)
{
    IM_ASSERT(op == '+' || op == '-');
    switch (data_type)
    {
    case vsonyp0werDataType_S8:
        if (op == '+') { *(ImS8*)output = ImAddClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        if (op == '-') { *(ImS8*)output = ImSubClampOverflow(*(const ImS8*)arg1, *(const ImS8*)arg2, IM_S8_MIN, IM_S8_MAX); }
        return;
    case vsonyp0werDataType_U8:
        if (op == '+') { *(ImU8*)output = ImAddClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        if (op == '-') { *(ImU8*)output = ImSubClampOverflow(*(const ImU8*)arg1, *(const ImU8*)arg2, IM_U8_MIN, IM_U8_MAX); }
        return;
    case vsonyp0werDataType_S16:
        if (op == '+') { *(ImS16*)output = ImAddClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        if (op == '-') { *(ImS16*)output = ImSubClampOverflow(*(const ImS16*)arg1, *(const ImS16*)arg2, IM_S16_MIN, IM_S16_MAX); }
        return;
    case vsonyp0werDataType_U16:
        if (op == '+') { *(ImU16*)output = ImAddClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        if (op == '-') { *(ImU16*)output = ImSubClampOverflow(*(const ImU16*)arg1, *(const ImU16*)arg2, IM_U16_MIN, IM_U16_MAX); }
        return;
    case vsonyp0werDataType_S32:
        if (op == '+') { *(ImS32*)output = ImAddClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        if (op == '-') { *(ImS32*)output = ImSubClampOverflow(*(const ImS32*)arg1, *(const ImS32*)arg2, IM_S32_MIN, IM_S32_MAX); }
        return;
    case vsonyp0werDataType_U32:
        if (op == '+') { *(ImU32*)output = ImAddClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        if (op == '-') { *(ImU32*)output = ImSubClampOverflow(*(const ImU32*)arg1, *(const ImU32*)arg2, IM_U32_MIN, IM_U32_MAX); }
        return;
    case vsonyp0werDataType_S64:
        if (op == '+') { *(ImS64*)output = ImAddClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        if (op == '-') { *(ImS64*)output = ImSubClampOverflow(*(const ImS64*)arg1, *(const ImS64*)arg2, IM_S64_MIN, IM_S64_MAX); }
        return;
    case vsonyp0werDataType_U64:
        if (op == '+') { *(ImU64*)output = ImAddClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        if (op == '-') { *(ImU64*)output = ImSubClampOverflow(*(const ImU64*)arg1, *(const ImU64*)arg2, IM_U64_MIN, IM_U64_MAX); }
        return;
    case vsonyp0werDataType_Float:
        if (op == '+') { *(float*)output = *(const float*)arg1 + *(const float*)arg2; }
        if (op == '-') { *(float*)output = *(const float*)arg1 - *(const float*)arg2; }
        return;
    case vsonyp0werDataType_Double:
        if (op == '+') { *(double*)output = *(const double*)arg1 + *(const double*)arg2; }
        if (op == '-') { *(double*)output = *(const double*)arg1 - *(const double*)arg2; }
        return;
    case vsonyp0werDataType_COUNT: break;
    }
    IM_ASSERT(0);
}

// User can input math operators (e.g. +100) to edit a numerical values.
// NB: This is _not_ a full expression evaluator. We should probably add one and replace this dumb mess..
static bool DataTypeApplyOpFromText(const char* buf, const char* initial_value_buf, vsonyp0werDataType data_type, void* data_ptr, const char* format)
{
    while (ImCharIsBlankA(*buf))
        buf++;

    // We don't support '-' op because it would conflict with inputing negative value.
    // Instead you can use +-100 to subtract from an existing value
    char op = buf[0];
    if (op == '+' || op == '*' || op == '/')
    {
        buf++;
        while (ImCharIsBlankA(*buf))
            buf++;
    } else
    {
        op = 0;
    }
    if (!buf[0])
        return false;

    // Copy the value in an opaque buffer so we can compare at the end of the function if it changed at all.
    IM_ASSERT(data_type < vsonyp0werDataType_COUNT);
    int data_backup[2];
    IM_ASSERT(GDataTypeInfo[data_type].Size <= sizeof(data_backup));
    memcpy(data_backup, data_ptr, GDataTypeInfo[data_type].Size);

    if (format == NULL)
        format = GDataTypeInfo[data_type].ScanFmt;

    // FIXME-LEGACY: The aim is to remove those operators and write a proper expression evaluator at some point..
    int arg1i = 0;
    if (data_type == vsonyp0werDataType_S32)
    {
        int* v = (int*)data_ptr;
        int arg0i = *v;
        float arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0i) < 1)
            return false;
        // Store operand in a float so we can use fractional value for multipliers (*1.1), but constant always parsed as integer so we can fit big integers (e.g. 2000000003) past float precision
        if (op == '+') { if (sscanf(buf, "%d", &arg1i))* v = (int)(arg0i + arg1i); }                   // Add (use "+-" to subtract)
        else if (op == '*') { if (sscanf(buf, "%f", &arg1f))* v = (int)(arg0i * arg1f); }                   // Multiply
        else if (op == '/') { if (sscanf(buf, "%f", &arg1f) && arg1f != 0.0f) * v = (int)(arg0i / arg1f); }  // Divide
        else { if (sscanf(buf, format, &arg1i) == 1) * v = arg1i; }                           // Assign constant
    } else if (data_type == vsonyp0werDataType_Float)
    {
        // For floats we have to ignore format with precision (e.g. "%.2f") because sscanf doesn't take them in
        format = "%f";
        float* v = (float*)data_ptr;
        float arg0f = *v, arg1f = 0.0f;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) * v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == vsonyp0werDataType_Double)
    {
        format = "%lf"; // scanf differentiate float/double unlike printf which forces everything to double because of ellipsis
        double* v = (double*)data_ptr;
        double arg0f = *v, arg1f = 0.0;
        if (op && sscanf(initial_value_buf, format, &arg0f) < 1)
            return false;
        if (sscanf(buf, format, &arg1f) < 1)
            return false;
        if (op == '+') { *v = arg0f + arg1f; }                    // Add (use "+-" to subtract)
        else if (op == '*') { *v = arg0f * arg1f; }                    // Multiply
        else if (op == '/') { if (arg1f != 0.0f) * v = arg0f / arg1f; } // Divide
        else { *v = arg1f; }                            // Assign constant
    } else if (data_type == vsonyp0werDataType_U32 || data_type == vsonyp0werDataType_S64 || data_type == vsonyp0werDataType_U64)
    {
        // All other types assign constant
        // We don't bother handling support for legacy operators since they are a little too crappy. Instead we will later implement a proper expression evaluator in the future.
        sscanf(buf, format, data_ptr);
    } else
    {
        // Small types need a 32-bit buffer to receive the result from scanf()
        int v32;
        sscanf(buf, format, &v32);
        if (data_type == vsonyp0werDataType_S8)
            * (ImS8*)data_ptr = (ImS8)ImClamp(v32, (int)IM_S8_MIN, (int)IM_S8_MAX);
        else if (data_type == vsonyp0werDataType_U8)
            * (ImU8*)data_ptr = (ImU8)ImClamp(v32, (int)IM_U8_MIN, (int)IM_U8_MAX);
        else if (data_type == vsonyp0werDataType_S16)
            * (ImS16*)data_ptr = (ImS16)ImClamp(v32, (int)IM_S16_MIN, (int)IM_S16_MAX);
        else if (data_type == vsonyp0werDataType_U16)
            * (ImU16*)data_ptr = (ImU16)ImClamp(v32, (int)IM_U16_MIN, (int)IM_U16_MAX);
        else
            IM_ASSERT(0);
    }

    return memcmp(data_backup, data_ptr, GDataTypeInfo[data_type].Size) != 0;
}

static float GetMinimumStepAtDecimalPrecision(int decimal_precision)
{
    static const float min_steps[10] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
    if (decimal_precision < 0)
        return FLT_MIN;
    return (decimal_precision < IM_ARRAYSIZE(min_steps)) ? min_steps[decimal_precision] : ImPow(10.0f, (float)-decimal_precision);
}

template<typename TYPE>
static const char* ImAtoi(const char* src, TYPE * output)
{
    int negative = 0;
    if (*src == '-') { negative = 1; src++; }
    if (*src == '+') { src++; }
    TYPE v = 0;
    while (*src >= '0' && *src <= '9')
        v = (v * 10) + (*src++ - '0');
    *output = negative ? -v : v;
    return src;
}

template<typename TYPE, typename SIGNEDTYPE>
TYPE vsonyp0wer::RoundScalarWithFormatT(const char* format, vsonyp0werDataType data_type, TYPE v)
{
    const char* fmt_start = ImParseFormatFindStart(format);
    if (fmt_start[0] != '%' || fmt_start[1] == '%') // Don't apply if the value is not visible in the format string
        return v;
    char v_str[64];
    ImFormatString(v_str, IM_ARRAYSIZE(v_str), fmt_start, v);
    const char* p = v_str;
    while (*p == ' ')
        p++;
    if (data_type == vsonyp0werDataType_Float || data_type == vsonyp0werDataType_Double)
        v = (TYPE)ImAtof(p);
    else
        ImAtoi(p, (SIGNEDTYPE*)& v);
    return v;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: DragScalar, DragFloat, DragInt, etc.
//-------------------------------------------------------------------------
// - DragBehaviorT<>() [Internal]
// - DragBehavior() [Internal]
// - DragScalar()
// - DragScalarN()
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
//-------------------------------------------------------------------------

// This is called by DragBehavior() when the widget is active (held by mouse or being manipulated with Nav controls)
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool vsonyp0wer::DragBehaviorT(vsonyp0werDataType data_type, TYPE * v, float v_speed, const TYPE v_min, const TYPE v_max, const char* format, float power, vsonyp0werDragFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werAxis axis = (flags & vsonyp0werDragFlags_Vertical) ? vsonyp0werAxis_Y : vsonyp0werAxis_X;
    const bool is_decimal = (data_type == vsonyp0werDataType_Float) || (data_type == vsonyp0werDataType_Double);
    const bool has_min_max = (v_min != v_max);
    const bool is_power = (power != 1.0f && is_decimal && has_min_max && (v_max - v_min < FLT_MAX));

    // Default tweak speed
    if (v_speed == 0.0f && has_min_max && (v_max - v_min < FLT_MAX))
        v_speed = (float)((v_max - v_min) * g.DragSpeedDefaultRatio);

    // Inputs accumulates into g.DragCurrentAccum, which is flushed into the current value as soon as it makes a difference with our precision settings
    float adjust_delta = 0.0f;
    if (g.ActiveIdSource == vsonyp0werInputSource_Mouse && IsMousePosValid() && g.IO.MouseDragMaxDistanceSqr[0] > 1.0f * 1.0f)
    {
        adjust_delta = g.IO.MouseDelta[axis];
        if (g.IO.KeyAlt)
            adjust_delta *= 1.0f / 100.0f;
        if (g.IO.KeyShift)
            adjust_delta *= 10.0f;
    } else if (g.ActiveIdSource == vsonyp0werInputSource_Nav)
    {
        int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
        adjust_delta = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_Keyboard | vsonyp0werNavDirSourceFlags_PadDPad, vsonyp0werInputReadMode_RepeatFast, 1.0f / 10.0f, 10.0f)[axis];
        v_speed = ImMax(v_speed, GetMinimumStepAtDecimalPrecision(decimal_precision));
    }
    adjust_delta *= v_speed;

    // For vertical drag we currently assume that Up=higher value (like we do with vertical sliders). This may become a parameter.
    if (axis == vsonyp0werAxis_Y)
        adjust_delta = -adjust_delta;

    // Clear current value on activation
    // Avoid altering values and clamping when we are _already_ past the limits and heading in the same direction, so e.g. if range is 0..255, current value is 300 and we are pushing to the right side, keep the 300.
    bool is_just_activated = g.ActiveIdIsJustActivated;
    bool is_already_past_limits_and_pushing_outward = has_min_max && ((*v >= v_max && adjust_delta > 0.0f) || (*v <= v_min && adjust_delta < 0.0f));
    bool is_drag_direction_change_with_power = is_power && ((adjust_delta < 0 && g.DragCurrentAccum > 0) || (adjust_delta > 0 && g.DragCurrentAccum < 0));
    if (is_just_activated || is_already_past_limits_and_pushing_outward || is_drag_direction_change_with_power)
    {
        g.DragCurrentAccum = 0.0f;
        g.DragCurrentAccumDirty = false;
    } else if (adjust_delta != 0.0f)
    {
        g.DragCurrentAccum += adjust_delta;
        g.DragCurrentAccumDirty = true;
    }

    if (!g.DragCurrentAccumDirty)
        return false;

    TYPE v_cur = *v;
    FLOATTYPE v_old_ref_for_accum_remainder = (FLOATTYPE)0.0f;

    if (is_power)
    {
        // Offset + round to user desired precision, with a curve on the v_min..v_max range to get more precision on one side of the range
        FLOATTYPE v_old_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        FLOATTYPE v_new_norm_curved = v_old_norm_curved + (g.DragCurrentAccum / (v_max - v_min));
        v_cur = v_min + (TYPE)ImPow(ImSaturate((float)v_new_norm_curved), power) * (v_max - v_min);
        v_old_ref_for_accum_remainder = v_old_norm_curved;
    } else
    {
        v_cur += (TYPE)g.DragCurrentAccum;
    }

    // Round to user desired precision based on format string
    v_cur = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_cur);

    // Preserve remainder after rounding has been applied. This also allow slow tweaking of values.
    g.DragCurrentAccumDirty = false;
    if (is_power)
    {
        FLOATTYPE v_cur_norm_curved = ImPow((FLOATTYPE)(v_cur - v_min) / (FLOATTYPE)(v_max - v_min), (FLOATTYPE)1.0f / power);
        g.DragCurrentAccum -= (float)(v_cur_norm_curved - v_old_ref_for_accum_remainder);
    } else
    {
        g.DragCurrentAccum -= (float)((SIGNEDTYPE)v_cur - (SIGNEDTYPE)* v);
    }

    // Lose zero sign for float/double
    if (v_cur == (TYPE)-0)
        v_cur = (TYPE)0;

    // Clamp values (+ handle overflow/wrap-around for integer types)
    if (*v != v_cur && has_min_max)
    {
        if (v_cur < v_min || (v_cur > * v && adjust_delta < 0.0f && !is_decimal))
            v_cur = v_min;
        if (v_cur > v_max || (v_cur < *v && adjust_delta > 0.0f && !is_decimal))
            v_cur = v_max;
    }

    // Apply result
    if (*v == v_cur)
        return false;
    *v = v_cur;
    return true;
}

bool vsonyp0wer::DragBehavior(vsonyp0werID id, vsonyp0werDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power, vsonyp0werDragFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.ActiveId == id)
    {
        if (g.ActiveIdSource == vsonyp0werInputSource_Mouse && !g.IO.MouseDown[0])
            ClearActiveID();
        else if (g.ActiveIdSource == vsonyp0werInputSource_Nav && g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            ClearActiveID();
    }
    if (g.ActiveId != id)
        return false;

    switch (data_type)
    {
    case vsonyp0werDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)v;  bool r = DragBehaviorT<ImS32, ImS32, float >(vsonyp0werDataType_S32, &v32, v_speed, v_min ? *(const ImS8*)v_min : IM_S8_MIN, v_max ? *(const ImS8*)v_max : IM_S8_MAX, format, power, flags); if (r)* (ImS8*)v = (ImS8)v32; return r; }
    case vsonyp0werDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)v;  bool r = DragBehaviorT<ImU32, ImS32, float >(vsonyp0werDataType_U32, &v32, v_speed, v_min ? *(const ImU8*)v_min : IM_U8_MIN, v_max ? *(const ImU8*)v_max : IM_U8_MAX, format, power, flags); if (r)* (ImU8*)v = (ImU8)v32; return r; }
    case vsonyp0werDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)v; bool r = DragBehaviorT<ImS32, ImS32, float >(vsonyp0werDataType_S32, &v32, v_speed, v_min ? *(const ImS16*)v_min : IM_S16_MIN, v_max ? *(const ImS16*)v_max : IM_S16_MAX, format, power, flags); if (r)* (ImS16*)v = (ImS16)v32; return r; }
    case vsonyp0werDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)v; bool r = DragBehaviorT<ImU32, ImS32, float >(vsonyp0werDataType_U32, &v32, v_speed, v_min ? *(const ImU16*)v_min : IM_U16_MIN, v_max ? *(const ImU16*)v_max : IM_U16_MAX, format, power, flags); if (r)* (ImU16*)v = (ImU16)v32; return r; }
    case vsonyp0werDataType_S32:    return DragBehaviorT<ImS32, ImS32, float >(data_type, (ImS32*)v, v_speed, v_min ? *(const ImS32*)v_min : IM_S32_MIN, v_max ? *(const ImS32*)v_max : IM_S32_MAX, format, power, flags);
    case vsonyp0werDataType_U32:    return DragBehaviorT<ImU32, ImS32, float >(data_type, (ImU32*)v, v_speed, v_min ? *(const ImU32*)v_min : IM_U32_MIN, v_max ? *(const ImU32*)v_max : IM_U32_MAX, format, power, flags);
    case vsonyp0werDataType_S64:    return DragBehaviorT<ImS64, ImS64, double>(data_type, (ImS64*)v, v_speed, v_min ? *(const ImS64*)v_min : IM_S64_MIN, v_max ? *(const ImS64*)v_max : IM_S64_MAX, format, power, flags);
    case vsonyp0werDataType_U64:    return DragBehaviorT<ImU64, ImS64, double>(data_type, (ImU64*)v, v_speed, v_min ? *(const ImU64*)v_min : IM_U64_MIN, v_max ? *(const ImU64*)v_max : IM_U64_MAX, format, power, flags);
    case vsonyp0werDataType_Float:  return DragBehaviorT<float, float, float >(data_type, (float*)v, v_speed, v_min ? *(const float*)v_min : -FLT_MAX, v_max ? *(const float*)v_max : FLT_MAX, format, power, flags);
    case vsonyp0werDataType_Double: return DragBehaviorT<double, double, double>(data_type, (double*)v, v_speed, v_min ? *(const double*)v_min : -DBL_MAX, v_max ? *(const double*)v_max : DBL_MAX, format, power, flags);
    case vsonyp0werDataType_COUNT:  break;
    }
    IM_ASSERT(0);
    return false;
}

bool vsonyp0wer::DragScalar(const char* label, vsonyp0werDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (power != 1.0f)
        IM_ASSERT(v_min != NULL && v_max != NULL); // When using a power curve the drag needs to have known bounds

    vsonyp0werContext & g = *Gvsonyp0wer;
    const vsonyp0werStyle & style = g.Style;
    const vsonyp0werID id = window->GetID(label);
    const float w = CalcItemWidth();

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    const bool hovered = ItemHoverable(frame_bb, id);

    // Default format string when passing NULL
    // Patch old "%.0f" format string to use "%d", read function comments for more details.
    IM_ASSERT(data_type >= 0 && data_type < vsonyp0werDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;
    else if (data_type == vsonyp0werDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    // Tabbing or CTRL-clicking on Drag turns it into an input box
    bool start_text_input = false;
    const bool focus_requested = FocusableItemRegister(window, id);
    if (focus_requested || (hovered && (g.IO.MouseClicked[0] || g.IO.MouseDoubleClicked[0])) || g.NavActivateId == id || (g.NavInputId == id && g.ScalarAsInputTextId != id))
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdAllowNavDirFlags = (1 << vsonyp0werDir_Up) | (1 << vsonyp0werDir_Down);
        if (focus_requested || g.IO.KeyCtrl || g.IO.MouseDoubleClicked[0] || g.NavInputId == id)
        {
            start_text_input = true;
            g.ScalarAsInputTextId = 0;
        }
    }
    if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
    {
        window->DC.CursorPos = frame_bb.Min;
        FocusableItemUnregister(window);
        return InputScalarAsWidgetReplacement(frame_bb, id, label, data_type, v, format);
    }

    // Actual drag behavior
    const bool value_changed = DragBehavior(id, data_type, v, v_speed, v_min, v_max, format, power, vsonyp0werDragFlags_None);
    if (value_changed)
        MarkItemEdited(id);

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? vsonyp0werCol_FrameBgActive : g.HoveredId == id ? vsonyp0werCol_FrameBgHovered : vsonyp0werCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

bool vsonyp0wer::DragScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, float v_speed, const void* v_min, const void* v_max, const char* format, float power)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= DragScalar("", data_type, v, v_speed, v_min, v_max, format, power);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool vsonyp0wer::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalar(label, vsonyp0werDataType_Float, v, v_speed, &v_min, &v_max, format, power);
}

bool vsonyp0wer::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, vsonyp0werDataType_Float, v, 2, v_speed, &v_min, &v_max, format, power);
}

bool vsonyp0wer::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, vsonyp0werDataType_Float, v, 3, v_speed, &v_min, &v_max, format, power);
}

bool vsonyp0wer::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power)
{
    return DragScalarN(label, vsonyp0werDataType_Float, v, 4, v_speed, &v_min, &v_max, format, power);
}

bool vsonyp0wer::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* format, const char* format_max, float power)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = DragFloat("##min", v_current_min, v_speed, (v_min >= v_max) ? -FLT_MAX : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragFloat("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? FLT_MAX : v_max, format_max ? format_max : format, power);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();
    return value_changed;
}

// NB: v_speed is float to allow adjusting the drag speed with more precision
bool vsonyp0wer::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalar(label, vsonyp0werDataType_S32, v, v_speed, &v_min, &v_max, format);
}

bool vsonyp0wer::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, vsonyp0werDataType_S32, v, 2, v_speed, &v_min, &v_max, format);
}

bool vsonyp0wer::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, vsonyp0werDataType_S32, v, 3, v_speed, &v_min, &v_max, format);
}

bool vsonyp0wer::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* format)
{
    return DragScalarN(label, vsonyp0werDataType_S32, v, 4, v_speed, &v_min, &v_max, format);
}

bool vsonyp0wer::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* format, const char* format_max)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2);

    bool value_changed = DragInt("##min", v_current_min, v_speed, (v_min >= v_max) ? INT_MIN : v_min, (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max), format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);
    value_changed |= DragInt("##max", v_current_max, v_speed, (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min), (v_min >= v_max) ? INT_MAX : v_max, format_max ? format_max : format);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: SliderScalar, SliderFloat, SliderInt, etc.
//-------------------------------------------------------------------------
// - SliderBehaviorT<>() [Internal]
// - SliderBehavior() [Internal]
// - SliderScalar()
// - SliderScalarN()
// - SliderFloat()
// - SliderFloat2()
// - SliderFloat3()
// - SliderFloat4()
// - SliderAngle()
// - SliderInt()
// - SliderInt2()
// - SliderInt3()
// - SliderInt4()
// - VSliderScalar()
// - VSliderFloat()
// - VSliderInt()
//-------------------------------------------------------------------------

template<typename TYPE, typename FLOATTYPE>
float vsonyp0wer::SliderCalcRatioFromValueT(vsonyp0werDataType data_type, TYPE v, TYPE v_min, TYPE v_max, float power, float linear_zero_pos)
{
    if (v_min == v_max)
        return 0.0f;

    const bool is_power = (power != 1.0f) && (data_type == vsonyp0werDataType_Float || data_type == vsonyp0werDataType_Double);
    const TYPE v_clamped = (v_min < v_max) ? ImClamp(v, v_min, v_max) : ImClamp(v, v_max, v_min);
    if (is_power)
    {
        if (v_clamped < 0.0f)
        {
            const float f = 1.0f - (float)((v_clamped - v_min) / (ImMin((TYPE)0, v_max) - v_min));
            return (1.0f - ImPow(f, 1.0f / power)) * linear_zero_pos;
        } else
        {
            const float f = (float)((v_clamped - ImMax((TYPE)0, v_min)) / (v_max - ImMax((TYPE)0, v_min)));
            return linear_zero_pos + ImPow(f, 1.0f / power) * (1.0f - linear_zero_pos);
        }
    }

    // Linear slider
    return (float)((FLOATTYPE)(v_clamped - v_min) / (FLOATTYPE)(v_max - v_min));
}

// FIXME: Move some of the code into SliderBehavior(). Current responsability is larger than what the equivalent DragBehaviorT<> does, we also do some rendering, etc.
template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool vsonyp0wer::SliderBehaviorT(const ImRect & bb, vsonyp0werID id, vsonyp0werDataType data_type, TYPE * v, const TYPE v_min, const TYPE v_max, const char* format, float power, vsonyp0werSliderFlags flags, ImRect * out_grab_bb)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;

    const vsonyp0werAxis axis = (flags & vsonyp0werSliderFlags_Vertical) ? vsonyp0werAxis_Y : vsonyp0werAxis_X;
    const bool is_decimal = (data_type == vsonyp0werDataType_Float) || (data_type == vsonyp0werDataType_Double);
    const bool is_power = (power != 1.0f) && is_decimal;

    const float grab_padding = 2.0f;
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = style.GrabMinSize;
    SIGNEDTYPE v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
    if (!is_decimal && v_range >= 0)                                             // v_range < 0 may happen on integer overflows
        grab_sz = ImMax((float)(slider_sz / (v_range + 1)), style.GrabMinSize);  // For integer sliders: if possible have the grab size represent 1 unit
    grab_sz = ImMin(grab_sz, slider_sz);
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    // For power curve sliders that cross over sign boundary we want the curve to be symmetric around 0.0f
    float linear_zero_pos;   // 0.0->1.0f
    if (is_power && v_min * v_max < 0.0f)
    {
        // Different sign
        const FLOATTYPE linear_dist_min_to_0 = ImPow(v_min >= 0 ? (FLOATTYPE)v_min : -(FLOATTYPE)v_min, (FLOATTYPE)1.0f / power);
        const FLOATTYPE linear_dist_max_to_0 = ImPow(v_max >= 0 ? (FLOATTYPE)v_max : -(FLOATTYPE)v_max, (FLOATTYPE)1.0f / power);
        linear_zero_pos = (float)(linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0));
    } else
    {
        // Same sign
        linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
    }

    // Process interacting with the slider
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == vsonyp0werInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            } else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (axis == vsonyp0werAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        } else if (g.ActiveIdSource == vsonyp0werInputSource_Nav)
        {
            const ImVec2 delta2 = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_Keyboard | vsonyp0werNavDirSourceFlags_PadDPad, vsonyp0werInputReadMode_RepeatFast, 0.0f, 0.0f);
            float delta = (axis == vsonyp0werAxis_X) ? delta2.x : -delta2.y;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            } else if (delta != 0.0f)
            {
                clicked_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
                const int decimal_precision = is_decimal ? ImParseFormatPrecision(format, 3) : 0;
                if ((decimal_precision > 0) || is_power)
                {
                    delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
                    if (IsNavInputDown(vsonyp0werNavInput_TweakSlow))
                        delta /= 10.0f;
                } else
                {
                    if ((v_range >= -100.0f && v_range <= 100.0f) || IsNavInputDown(vsonyp0werNavInput_TweakSlow))
                        delta = ((delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
                    else
                        delta /= 100.0f;
                }
                if (IsNavInputDown(vsonyp0werNavInput_TweakFast))
                    delta *= 10.0f;
                set_new_value = true;
                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
                    set_new_value = false;
                else
                    clicked_t = ImSaturate(clicked_t + delta);
            }
        }

        if (set_new_value)
        {
            TYPE v_new;
            if (is_power)
            {
                // Account for power curve scale on both sides of the zero
                if (clicked_t < linear_zero_pos)
                {
                    // Negative: rescale to the negative range before powering
                    float a = 1.0f - (clicked_t / linear_zero_pos);
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMin(v_max, (TYPE)0), v_min, a);
                } else
                {
                    // Positive: rescale to the positive range before powering
                    float a;
                    if (ImFabs(linear_zero_pos - 1.0f) > 1.e-6f)
                        a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                    else
                        a = clicked_t;
                    a = ImPow(a, power);
                    v_new = ImLerp(ImMax(v_min, (TYPE)0), v_max, a);
                }
            } else
            {
                // Linear slider
                if (is_decimal)
                {
                    v_new = ImLerp(v_min, v_max, clicked_t);
                } else
                {
                    // For integer values we want the clicking position to match the grab box so we round above
                    // This code is carefully tuned to work with large values (e.g. high ranges of U64) while preserving this property..
                    FLOATTYPE v_new_off_f = (v_max - v_min) * clicked_t;
                    TYPE v_new_off_floor = (TYPE)(v_new_off_f);
                    TYPE v_new_off_round = (TYPE)(v_new_off_f + (FLOATTYPE)0.5);
                    if (!is_decimal && v_new_off_floor < v_new_off_round)
                        v_new = v_min + v_new_off_round;
                    else
                        v_new = v_min + v_new_off_floor;
                }
            }

            // Round to user desired precision based on format string
            v_new = RoundScalarWithFormatT<TYPE, SIGNEDTYPE>(format, data_type, v_new);

            // Apply result
            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = ImRect(bb.Min, bb.Min);
    } else
    {
        // Output grab position so it can be displayed by the caller
        float grab_t = SliderCalcRatioFromValueT<TYPE, FLOATTYPE>(data_type, *v, v_min, v_max, power, linear_zero_pos);
        if (axis == vsonyp0werAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == vsonyp0werAxis_X)
            * out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;
}

// For 32-bits and larger types, slider bounds are limited to half the natural type range.
// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
bool vsonyp0wer::SliderBehavior(const ImRect & bb, vsonyp0werID id, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power, vsonyp0werSliderFlags flags, ImRect * out_grab_bb)
{
    switch (data_type)
    {
    case vsonyp0werDataType_S8: { ImS32 v32 = (ImS32) * (ImS8*)v;  bool r = SliderBehaviorT<ImS32, ImS32, float >(bb, id, vsonyp0werDataType_S32, &v32, *(const ImS8*)v_min, *(const ImS8*)v_max, format, power, flags, out_grab_bb); if (r)* (ImS8*)v = (ImS8)v32;  return r; }
    case vsonyp0werDataType_U8: { ImU32 v32 = (ImU32) * (ImU8*)v;  bool r = SliderBehaviorT<ImU32, ImS32, float >(bb, id, vsonyp0werDataType_U32, &v32, *(const ImU8*)v_min, *(const ImU8*)v_max, format, power, flags, out_grab_bb); if (r)* (ImU8*)v = (ImU8)v32;  return r; }
    case vsonyp0werDataType_S16: { ImS32 v32 = (ImS32) * (ImS16*)v; bool r = SliderBehaviorT<ImS32, ImS32, float >(bb, id, vsonyp0werDataType_S32, &v32, *(const ImS16*)v_min, *(const ImS16*)v_max, format, power, flags, out_grab_bb); if (r)* (ImS16*)v = (ImS16)v32; return r; }
    case vsonyp0werDataType_U16: { ImU32 v32 = (ImU32) * (ImU16*)v; bool r = SliderBehaviorT<ImU32, ImS32, float >(bb, id, vsonyp0werDataType_U32, &v32, *(const ImU16*)v_min, *(const ImU16*)v_max, format, power, flags, out_grab_bb); if (r)* (ImU16*)v = (ImU16)v32; return r; }
    case vsonyp0werDataType_S32:
        IM_ASSERT(*(const ImS32*)v_min >= IM_S32_MIN / 2 && *(const ImS32*)v_max <= IM_S32_MAX / 2);
        return SliderBehaviorT<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)v, *(const ImS32*)v_min, *(const ImS32*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_U32:
        IM_ASSERT(*(const ImU32*)v_min <= IM_U32_MAX / 2);
        return SliderBehaviorT<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)v, *(const ImU32*)v_min, *(const ImU32*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_S64:
        IM_ASSERT(*(const ImS64*)v_min >= IM_S64_MIN / 2 && *(const ImS64*)v_max <= IM_S64_MAX / 2);
        return SliderBehaviorT<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)v, *(const ImS64*)v_min, *(const ImS64*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_U64:
        IM_ASSERT(*(const ImU64*)v_min <= IM_U64_MAX / 2);
        return SliderBehaviorT<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)v, *(const ImU64*)v_min, *(const ImU64*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_Float:
        IM_ASSERT(*(const float*)v_min >= -FLT_MAX / 2.0f && *(const float*)v_max <= FLT_MAX / 2.0f);
        return SliderBehaviorT<float, float, float >(bb, id, data_type, (float*)v, *(const float*)v_min, *(const float*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_Double:
        IM_ASSERT(*(const double*)v_min >= -DBL_MAX / 2.0f && *(const double*)v_max <= DBL_MAX / 2.0f);
        return SliderBehaviorT<double, double, double>(bb, id, data_type, (double*)v, *(const double*)v_min, *(const double*)v_max, format, power, flags, out_grab_bb);
    case vsonyp0werDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return false;
}

bool vsonyp0wer::SliderScalar(const char* label, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;

	const vsonyp0werID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y - 6)); // 6
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, -0.0f));

	if (!ItemAdd(total_bb, id, &frame_bb)) {

		ItemSize(total_bb, style.FramePadding.y);
		return false;
	}

	IM_ASSERT(data_type >= 0 && data_type < vsonyp0werDataType_COUNT);

	if (format == NULL)
		format = GDataTypeInfo[data_type].PrintFmt;
	else if (data_type == vsonyp0werDataType_S32 && strcmp(format, "%d") != 0)
		format = PatchFormatStringFloatToInt(format);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool tab_focus_requested = FocusableItemRegister(window, id);
	const bool hovered = ItemHoverable(frame_bb, id);

	if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || (g.NavInputId == id && g.ScalarAsInputTextId != id)) {

		SetActiveID(id, window);
		SetFocusID(id, window);
		FocusWindow(window);
		g.ActiveIdAllowNavDirFlags = (1 << vsonyp0werDir_Up) | (1 << vsonyp0werDir_Down);

		if (tab_focus_requested || g.IO.KeyCtrl || g.NavInputId == id)
			g.ScalarAsInputTextId = 0;
	}

	ItemSize(total_bb, style.FramePadding.y);

	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, vsonyp0werSliderFlags_None, &grab_bb);

	if (value_changed)
		MarkItemEdited(id);

	// Draw our shit
	auto borderColor = ImColor(10, 10, 10, 255);
	auto topColor = ImColor(52, 52, 52, 255);
	auto bottomColor = ImColor(68, 68, 68, 255);
	auto topColorHovered = ImColor(62, 62, 62, 255);
	auto bottomColorHovered = ImColor(78, 78, 78, 255);
	auto grabTopColor = GetColorU32(vsonyp0werCol_MenuTheme);
	auto grabBottomColor = GetColorU32(vsonyp0werCol_MenuTheme) - ImColor(0, 0, 0, 125);

	if (hovered || IsItemActive())
		window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(0, 1), frame_bb.Max, topColorHovered, topColorHovered, bottomColorHovered, bottomColorHovered);
	else
		window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(0, 1), frame_bb.Max, topColor, topColor, bottomColor, bottomColor);

	window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(0, 1), grab_bb.Max + ImVec2(1, 1), grabTopColor, grabTopColor, grabBottomColor, grabBottomColor);
	window->DrawList->AddRect(frame_bb.Min + ImVec2(0, 1), frame_bb.Max, borderColor, 0, false, 0.4);

	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);

	PopFont();
	PushFont(globals::menuFont);

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(ImVec2(grab_bb.Min.x - 101, grab_bb.Max.y - 5), ImVec2(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, ImVec2(0.51f, 0.5f));
	PopStyleColor();

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(ImVec2(grab_bb.Min.x - 105, grab_bb.Max.y - 5), ImVec2(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, ImVec2(0.51f, 0.5f));
	PopStyleColor();

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(ImVec2(grab_bb.Min.x - 101, grab_bb.Max.y - 7), ImVec2(grab_bb.Max.x + 101, grab_bb.Max.y + 6), value_buf, value_buf_end, NULL, ImVec2(0.51f, 0.5f));
	PopStyleColor();

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(ImVec2(grab_bb.Min.x - 105, grab_bb.Max.y - 7), ImVec2(grab_bb.Max.x + 101, grab_bb.Max.y + 6), value_buf, value_buf_end, NULL, ImVec2(0.51f, 0.5f));
	PopStyleColor();

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderTextClipped(ImVec2(grab_bb.Min.x - 103, grab_bb.Max.y - 6), ImVec2(grab_bb.Max.x + 101, grab_bb.Max.y + 8), value_buf, value_buf_end, NULL, ImVec2(0.51f, 0.5f));
	PopStyleColor();

	PopFont();
	PushFont(globals::menuFont);

	const ImRect text_bb(window->DC.CursorPos + ImVec2(-2, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y - 2) + label_size);

	if (label_size.x > 0) {

		//PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		//RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x - 162, frame_bb.Min.y + style.FramePadding.y - 17), label);
		//PopStyleColor();

		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x - 163, frame_bb.Min.y + style.FramePadding.y - 18), label);
	}

	return value_changed;
}

// Add multiple sliders on 1 line for compact edition of multiple components
bool vsonyp0wer::SliderScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, float power)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= SliderScalar("", data_type, v, v_min, v_max, format, power);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool vsonyp0wer::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)
{
    return SliderScalar(label, vsonyp0werDataType_Float, v, &v_min, &v_max, format, power);
}

bool vsonyp0wer::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, vsonyp0werDataType_Float, v, 2, &v_min, &v_max, format, power);
}

bool vsonyp0wer::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, vsonyp0werDataType_Float, v, 3, &v_min, &v_max, format, power);
}

bool vsonyp0wer::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power)
{
    return SliderScalarN(label, vsonyp0werDataType_Float, v, 4, &v_min, &v_max, format, power);
}

bool vsonyp0wer::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format)
{
    if (format == NULL)
        format = "%.0f deg";
    float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
    bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, 1.0f);
    *v_rad = v_deg * (2 * IM_PI) / 360.0f;
    return value_changed;
}

bool vsonyp0wer::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    return SliderScalar(label, vsonyp0werDataType_S32, v, &v_min, &v_max, format);
}

bool vsonyp0wer::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, vsonyp0werDataType_S32, v, 2, &v_min, &v_max, format);
}

bool vsonyp0wer::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, vsonyp0werDataType_S32, v, 3, &v_min, &v_max, format);
}

bool vsonyp0wer::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format)
{
    return SliderScalarN(label, vsonyp0werDataType_S32, v, 4, &v_min, &v_max, format);
}

bool vsonyp0wer::VSliderScalar(const char* label, const ImVec2 & size, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const vsonyp0werID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, id))
        return false;

    // Default format string when passing NULL
    // Patch old "%.0f" format string to use "%d", read function comments for more details.
    IM_ASSERT(data_type >= 0 && data_type < vsonyp0werDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;
    else if (data_type == vsonyp0werDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    if ((hovered && g.IO.MouseClicked[0]) || g.NavActivateId == id || g.NavInputId == id)
    {
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        g.ActiveIdAllowNavDirFlags = (1 << vsonyp0werDir_Left) | (1 << vsonyp0werDir_Right);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? vsonyp0werCol_FrameBgActive : g.HoveredId == id ? vsonyp0werCol_FrameBgHovered : vsonyp0werCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, v, v_min, v_max, format, power, vsonyp0werSliderFlags_Vertical, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Render grab
    if (grab_bb.Max.y > grab_bb.Min.y)
        window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? vsonyp0werCol_SliderGrabActive : vsonyp0werCol_SliderGrab), style.GrabRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    // For the vertical slider we allow centered text to overlap the frame padding
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, v, format);
    RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.0f));
    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return value_changed;
}

bool vsonyp0wer::VSliderFloat(const char* label, const ImVec2 & size, float* v, float v_min, float v_max, const char* format, float power)
{
    return VSliderScalar(label, size, vsonyp0werDataType_Float, v, &v_min, &v_max, format, power);
}

bool vsonyp0wer::VSliderInt(const char* label, const ImVec2 & size, int* v, int v_min, int v_max, const char* format)
{
    return VSliderScalar(label, size, vsonyp0werDataType_S32, v, &v_min, &v_max, format);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputScalar, InputFloat, InputInt, etc.
//-------------------------------------------------------------------------
// - ImParseFormatFindStart() [Internal]
// - ImParseFormatFindEnd() [Internal]
// - ImParseFormatTrimDecorations() [Internal]
// - ImParseFormatPrecision() [Internal]
// - InputScalarAsWidgetReplacement() [Internal]
// - InputScalar()
// - InputScalarN()
// - InputFloat()
// - InputFloat2()
// - InputFloat3()
// - InputFloat4()
// - InputInt()
// - InputInt2()
// - InputInt3()
// - InputInt4()
// - InputDouble()
//-------------------------------------------------------------------------

// We don't use strchr() because our strings are usually very short and often start with '%'
const char* ImParseFormatFindStart(const char* fmt)
{
    while (char c = fmt[0])
    {
        if (c == '%' && fmt[1] != '%')
            return fmt;
        else if (c == '%')
            fmt++;
        fmt++;
    }
    return fmt;
}

const char* ImParseFormatFindEnd(const char* fmt)
{
    // Printf/scanf types modifiers: I/L/h/j/l/t/w/z. Other uppercase letters qualify as types aka end of the format.
    if (fmt[0] != '%')
        return fmt;
    const unsigned int ignored_uppercase_mask = (1 << ('I' - 'A')) | (1 << ('L' - 'A'));
    const unsigned int ignored_lowercase_mask = (1 << ('h' - 'a')) | (1 << ('j' - 'a')) | (1 << ('l' - 'a')) | (1 << ('t' - 'a')) | (1 << ('w' - 'a')) | (1 << ('z' - 'a'));
    for (char c; (c = *fmt) != 0; fmt++)
    {
        if (c >= 'A' && c <= 'Z' && ((1 << (c - 'A')) & ignored_uppercase_mask) == 0)
            return fmt + 1;
        if (c >= 'a' && c <= 'z' && ((1 << (c - 'a')) & ignored_lowercase_mask) == 0)
            return fmt + 1;
    }
    return fmt;
}

// Extract the format out of a format string with leading or trailing decorations
//  fmt = "blah blah"  -> return fmt
//  fmt = "%.3f"       -> return fmt
//  fmt = "hello %.3f" -> return fmt + 6
//  fmt = "%.3f hello" -> return buf written with "%.3f"
const char* ImParseFormatTrimDecorations(const char* fmt, char* buf, size_t buf_size)
{
    const char* fmt_start = ImParseFormatFindStart(fmt);
    if (fmt_start[0] != '%')
        return fmt;
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);
    if (fmt_end[0] == 0) // If we only have leading decoration, we don't need to copy the data.
        return fmt_start;
    ImStrncpy(buf, fmt_start, ImMin((size_t)(fmt_end - fmt_start) + 1, buf_size));
    return buf;
}

// Parse display precision back from the display format string
// FIXME: This is still used by some navigation code path to infer a minimum tweak step, but we should aim to rework widgets so it isn't needed.
int ImParseFormatPrecision(const char* fmt, int default_precision)
{
    fmt = ImParseFormatFindStart(fmt);
    if (fmt[0] != '%')
        return default_precision;
    fmt++;
    while (*fmt >= '0' && *fmt <= '9')
        fmt++;
    int precision = INT_MAX;
    if (*fmt == '.')
    {
        fmt = ImAtoi<int>(fmt + 1, &precision);
        if (precision < 0 || precision > 99)
            precision = default_precision;
    }
    if (*fmt == 'e' || *fmt == 'E') // Maximum precision with scientific notation
        precision = -1;
    if ((*fmt == 'g' || *fmt == 'G') && precision == INT_MAX)
        precision = -1;
    return (precision == INT_MAX) ? default_precision : precision;
}

// Create text input in place of an active drag/slider (used when doing a CTRL+Click on drag/slider widgets)
// FIXME: Facilitate using this in variety of other situations.
bool vsonyp0wer::InputScalarAsWidgetReplacement(const ImRect & bb, vsonyp0werID id, const char* label, vsonyp0werDataType data_type, void* data_ptr, const char* format)
{
    IM_UNUSED(id);
    vsonyp0werContext& g = *Gvsonyp0wer;

    // On the first frame, g.ScalarAsInputTextId == 0, then on subsequent frames it becomes == id.
    // We clear ActiveID on the first frame to allow the InputText() taking it back.
    if (g.ScalarAsInputTextId == 0)
        ClearActiveID();

    char fmt_buf[32];
    char data_buf[32];
    format = ImParseFormatTrimDecorations(format, fmt_buf, IM_ARRAYSIZE(fmt_buf));
    DataTypeFormatString(data_buf, IM_ARRAYSIZE(data_buf), data_type, data_ptr, format);
    ImStrTrimBlanks(data_buf);
    vsonyp0werInputTextFlags flags = vsonyp0werInputTextFlags_AutoSelectAll | ((data_type == vsonyp0werDataType_Float || data_type == vsonyp0werDataType_Double) ? vsonyp0werInputTextFlags_CharsScientific : vsonyp0werInputTextFlags_CharsDecimal);
    bool value_changed = InputTextEx(label, NULL, data_buf, IM_ARRAYSIZE(data_buf), bb.GetSize(), flags);
    if (g.ScalarAsInputTextId == 0)
    {
        // First frame we started displaying the InputText widget, we expect it to take the active id.
        IM_ASSERT(g.ActiveId == id);
        g.ScalarAsInputTextId = g.ActiveId;
    }
    if (value_changed)
        return DataTypeApplyOpFromText(data_buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, NULL);
    return false;
}

bool vsonyp0wer::InputScalar(const char* label, vsonyp0werDataType data_type, void* data_ptr, const void* step, const void* step_fast, const char* format, vsonyp0werInputTextFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werStyle& style = g.Style;

    IM_ASSERT(data_type >= 0 && data_type < vsonyp0werDataType_COUNT);
    if (format == NULL)
        format = GDataTypeInfo[data_type].PrintFmt;

    char buf[64];
    DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, data_ptr, format);

    bool value_changed = false;
    if ((flags & (vsonyp0werInputTextFlags_CharsHexadecimal | vsonyp0werInputTextFlags_CharsScientific)) == 0)
        flags |= vsonyp0werInputTextFlags_CharsDecimal;
    flags |= vsonyp0werInputTextFlags_AutoSelectAll;

    if (step != NULL)
    {
        const float button_size = GetFrameHeight();

        BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
        PushID(label);
        PushItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
        if (InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);
        PopItemWidth();

        // Step buttons
        const ImVec2 backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        vsonyp0werButtonFlags button_flags = vsonyp0werButtonFlags_Repeat | vsonyp0werButtonFlags_DontClosePopups;
        if (flags & vsonyp0werInputTextFlags_ReadOnly)
            button_flags |= vsonyp0werButtonFlags_Disabled;
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("-", ImVec2(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '-', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        if (ButtonEx("+", ImVec2(button_size, button_size), button_flags))
        {
            DataTypeApplyOp(data_type, '+', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
            value_changed = true;
        }
        SameLine(0, style.ItemInnerSpacing.x);
        TextEx(label, FindRenderedTextEnd(label));
        style.FramePadding = backup_frame_padding;

        PopID();
        EndGroup();
    } else
    {
        if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
            value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);
    }

    return value_changed;
}

bool vsonyp0wer::InputScalarN(const char* label, vsonyp0werDataType data_type, void* v, int components, const void* step, const void* step_fast, const char* format, vsonyp0werInputTextFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components);
    size_t type_size = GDataTypeInfo[data_type].Size;
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        value_changed |= InputScalar("", data_type, v, step, step_fast, format, flags);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
        v = (void*)((char*)v + type_size);
    }
    PopID();

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    return value_changed;
}

bool vsonyp0wer::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, vsonyp0werInputTextFlags flags)
{
    flags |= vsonyp0werInputTextFlags_CharsScientific;
    return InputScalar(label, vsonyp0werDataType_Float, (void*)v, (void*)(step > 0.0f ? &step : NULL), (void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
}

bool vsonyp0wer::InputFloat2(const char* label, float v[2], const char* format, vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool vsonyp0wer::InputFloat3(const char* label, float v[3], const char* format, vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool vsonyp0wer::InputFloat4(const char* label, float v[4], const char* format, vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_Float, v, 4, NULL, NULL, format, flags);
}

// Prefer using "const char* format" directly, which is more flexible and consistent with other API.
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
bool vsonyp0wer::InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, vsonyp0werInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputFloat(label, v, step, step_fast, format, flags);
}

bool vsonyp0wer::InputFloat2(const char* label, float v[2], int decimal_precision, vsonyp0werInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, vsonyp0werDataType_Float, v, 2, NULL, NULL, format, flags);
}

bool vsonyp0wer::InputFloat3(const char* label, float v[3], int decimal_precision, vsonyp0werInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, vsonyp0werDataType_Float, v, 3, NULL, NULL, format, flags);
}

bool vsonyp0wer::InputFloat4(const char* label, float v[4], int decimal_precision, vsonyp0werInputTextFlags flags)
{
    char format[16] = "%f";
    if (decimal_precision >= 0)
        ImFormatString(format, IM_ARRAYSIZE(format), "%%.%df", decimal_precision);
    return InputScalarN(label, vsonyp0werDataType_Float, v, 4, NULL, NULL, format, flags);
}
#endif // vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS

bool vsonyp0wer::InputInt(const char* label, int* v, int step, int step_fast, vsonyp0werInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & vsonyp0werInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return InputScalar(label, vsonyp0werDataType_S32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool vsonyp0wer::InputInt2(const char* label, int v[2], vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_S32, v, 2, NULL, NULL, "%d", flags);
}

bool vsonyp0wer::InputInt3(const char* label, int v[3], vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_S32, v, 3, NULL, NULL, "%d", flags);
}

bool vsonyp0wer::InputInt4(const char* label, int v[4], vsonyp0werInputTextFlags flags)
{
    return InputScalarN(label, vsonyp0werDataType_S32, v, 4, NULL, NULL, "%d", flags);
}

bool vsonyp0wer::InputDouble(const char* label, double* v, double step, double step_fast, const char* format, vsonyp0werInputTextFlags flags)
{
    flags |= vsonyp0werInputTextFlags_CharsScientific;
    return InputScalar(label, vsonyp0werDataType_Double, (void*)v, (void*)(step > 0.0 ? &step : NULL), (void*)(step_fast > 0.0 ? &step_fast : NULL), format, flags);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: InputText, InputTextMultiline, InputTextWithHint
//-------------------------------------------------------------------------
// - InputText()
// - InputTextWithHint()
// - InputTextMultiline()
// - InputTextEx() [Internal]
//-------------------------------------------------------------------------

bool vsonyp0wer::InputText(const char* label, char* buf, size_t buf_size, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & vsonyp0werInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

bool vsonyp0wer::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2 & size, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* user_data)
{
    return InputTextEx(label, NULL, buf, (int)buf_size, size, flags | vsonyp0werInputTextFlags_Multiline, callback, user_data);
}

bool vsonyp0wer::InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & vsonyp0werInputTextFlags_Multiline)); // call InputTextMultiline()
    return InputTextEx(label, hint, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

static int InputTextCalcTextLenAndLineCount(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (char c = *s++) // We are only matching for \n so we can ignore UTF-8 decoding
        if (c == '\n')
            line_count++;
    s--;
    if (s[0] != '\n' && s[0] != '\r')
        line_count++;
    *out_text_end = s;
    return line_count;
}

static ImVec2 InputTextCalcTextSizeW(const ImWchar * text_begin, const ImWchar * text_end, const ImWchar * *remaining, ImVec2 * out_offset, bool stop_on_new_line)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const ImWchar* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        * out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

    if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
        text_size.y += line_height;

    if (remaining)
        * remaining = s;

    return text_size;
}

// Wrapper for stb_textedit.h to edit text (our wrapper is for: statically sized buffer, single-line, wchar characters. InputText converts between UTF-8 and wchar)
namespace vsdns
{

    static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj) { return obj->CurLenW; }
    static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx) { return obj->TextW[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; return Gvsonyp0wer->Font->GetCharAdvance(c) * (Gvsonyp0wer->FontSize / Gvsonyp0wer->Font->FontSize); }
    static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x10000 ? 0 : key; }
    static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow * r, STB_TEXTEDIT_STRING * obj, int line_start_idx)
    {
        const ImWchar* text = obj->TextW.Data;
        const ImWchar* text_remaining = NULL;
        const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

    static bool is_separator(unsigned int c) { return ImCharIsBlankW(c) || c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|'; }
    static int  is_word_boundary_from_right(STB_TEXTEDIT_STRING * obj, int idx) { return idx > 0 ? (is_separator(obj->TextW[idx - 1]) && !is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
#ifdef __APPLE__    // FIXME: Move setting to IO structure
    static int  is_word_boundary_from_left(STB_TEXTEDIT_STRING * obj, int idx) { return idx > 0 ? (!is_separator(obj->TextW[idx - 1]) && is_separator(obj->TextW[idx])) : 1; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
#else
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING * obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
#endif
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL    // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING * obj, int pos, int n)
    {
        ImWchar* dst = obj->TextW.Data + pos;

        // We maintain our buffer length in both UTF-8 and wchar formats
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        // Offset remaining text (FIXME-OPT: Use memmove)
        const ImWchar * src = obj->TextW.Data + pos + n;
        while (ImWchar c = *src++)
            * dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING * obj, int pos, const ImWchar * new_text, int new_text_len)
    {
        const bool is_resizable = (obj->UserFlags & vsonyp0werInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenW;
        IM_ASSERT(pos <= text_len);

        const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
        if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        // Grow internal buffer if needed
        if (new_text_len + text_len + 1 > obj->TextW.Size)
        {
            if (!is_resizable)
                return false;
            IM_ASSERT(text_len < obj->TextW.Size);
            obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
        }

        ImWchar* text = obj->TextW.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
        memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

        obj->CurLenW += new_text_len;
        obj->CurLenA += new_text_len_utf8;
        obj->TextW[obj->CurLenW] = '\0';

        return true;
    }

    // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x10000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x10001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x10002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x10003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x10004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x10005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x10006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x10007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x10008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x10009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x1000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x1000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x1000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x1000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_SHIFT        0x20000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "vsdns_textedit.h"

}

void vsonyp0werInputTextState::OnKeyPressed(int key)
{
    stb_textedit_key(this, &Stb, key);
    CursorFollow = true;
    CursorAnimReset();
}

vsonyp0werInputTextCallbackData::vsonyp0werInputTextCallbackData()
{
    memset(this, 0, sizeof(*this));
}

// Public API to manipulate UTF-8 text
// We expose UTF-8 to the user (unlike the STB_TEXTEDIT_* functions which are manipulating wchar)
// FIXME: The existence of this rarely exercised code path is a bit of a nuisance.
void vsonyp0werInputTextCallbackData::DeleteChars(int pos, int bytes_count)
{
    IM_ASSERT(pos + bytes_count <= BufTextLen);
    char* dst = Buf + pos;
    const char* src = Buf + pos + bytes_count;
    while (char c = *src++)
        * dst++ = c;
    *dst = '\0';

    if (CursorPos + bytes_count >= pos)
        CursorPos -= bytes_count;
    else if (CursorPos >= pos)
        CursorPos = pos;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen -= bytes_count;
}

void vsonyp0werInputTextCallbackData::InsertChars(int pos, const char* new_text, const char* new_text_end)
{
    const bool is_resizable = (Flags & vsonyp0werInputTextFlags_CallbackResize) != 0;
    const int new_text_len = new_text_end ? (int)(new_text_end - new_text) : (int)strlen(new_text);
    if (new_text_len + BufTextLen >= BufSize)
    {
        if (!is_resizable)
            return;

        // Contrary to STB_TEXTEDIT_INSERTCHARS() this is working in the UTF8 buffer, hence the midly similar code (until we remove the U16 buffer alltogether!)
        vsonyp0werContext& g = *Gvsonyp0wer;
        vsonyp0werInputTextState* edit_state = &g.InputTextState;
        IM_ASSERT(edit_state->ID != 0 && g.ActiveId == edit_state->ID);
        IM_ASSERT(Buf == edit_state->TextA.Data);
        int new_buf_size = BufTextLen + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1;
        edit_state->TextA.reserve(new_buf_size + 1);
        Buf = edit_state->TextA.Data;
        BufSize = edit_state->BufCapacityA = new_buf_size;
    }

    if (BufTextLen != pos)
        memmove(Buf + pos + new_text_len, Buf + pos, (size_t)(BufTextLen - pos));
    memcpy(Buf + pos, new_text, (size_t)new_text_len * sizeof(char));
    Buf[BufTextLen + new_text_len] = '\0';

    if (CursorPos >= pos)
        CursorPos += new_text_len;
    SelectionStart = SelectionEnd = CursorPos;
    BufDirty = true;
    BufTextLen += new_text_len;
}

// Return false to discard a character.
static bool InputTextFilterCharacter(unsigned int* p_char, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* user_data)
{
    unsigned int c = *p_char;

    // Filter non-printable (NB: isprint is unreliable! see #2467)
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n' && (flags & vsonyp0werInputTextFlags_Multiline));
        pass |= (c == '\t' && (flags & vsonyp0werInputTextFlags_AllowTabInput));
        if (!pass)
            return false;
    }

    // Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
    if (c >= 0xE000 && c <= 0xF8FF)
        return false;

    // Generic named filters
    if (flags & (vsonyp0werInputTextFlags_CharsDecimal | vsonyp0werInputTextFlags_CharsHexadecimal | vsonyp0werInputTextFlags_CharsUppercase | vsonyp0werInputTextFlags_CharsNoBlank | vsonyp0werInputTextFlags_CharsScientific))
    {
        if (flags & vsonyp0werInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        if (flags & vsonyp0werInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != '.') && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        if (flags & vsonyp0werInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        if (flags & vsonyp0werInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                * p_char = (c += (unsigned int)('A' - 'a'));

        if (flags & vsonyp0werInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;
    }

    // Custom callback filter
    if (flags & vsonyp0werInputTextFlags_CallbackCharFilter)
    {
        vsonyp0werInputTextCallbackData callback_data;
        memset(&callback_data, 0, sizeof(vsonyp0werInputTextCallbackData));
        callback_data.EventFlag = vsonyp0werInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

// Edit a string of text
// - buf_size account for the zero-terminator, so a buf_size of 6 can hold "Hello" but not "Hello!".
//   This is so we can easily call InputText() on static arrays using ARRAYSIZE() and to match
//   Note that in std::string world, capacity() would omit 1 byte used by the zero-terminator.
// - When active, hold on a privately held copy of the text (and apply back to 'buf'). So changing 'buf' while the InputText is active has no effect.
// - If you want to use vsonyp0wer::InputText() with std::string, see misc/cpp/vsonyp0wer_stdlib.h
// (FIXME: Rather confusing and messy function, among the worse part of our codebase, expecting to rewrite a V2 at some point.. Partly because we are
//  doing UTF8 > U16 > UTF8 conversions on the go to easily interface with stb_textedit. Ideally should stay in UTF-8 all the time. See https://github.com/nothings/stb/issues/188)
bool vsonyp0wer::InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2 & size_arg, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback, void* callback_user_data)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(!((flags & vsonyp0werInputTextFlags_CallbackHistory) && (flags & vsonyp0werInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & vsonyp0werInputTextFlags_CallbackCompletion) && (flags & vsonyp0werInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werIO& io = g.IO;
    const vsonyp0werStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & vsonyp0werInputTextFlags_Multiline) != 0;
    const bool is_readonly = (flags & vsonyp0werInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & vsonyp0werInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & vsonyp0werInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & vsonyp0werInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL); // Must provide a callback if you set the vsonyp0werInputTextFlags_CallbackResize flag!

    if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope,
        BeginGroup();
    const vsonyp0werID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), (is_multiline ? GetTextLineHeight() * 8.0f : label_size.y) + style.FramePadding.y * 2.0f); // Arbitrary default of 8 lines high for multi-line
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

    vsonyp0werWindow * draw_window = window;
    if (is_multiline)
    {
        if (!ItemAdd(total_bb, id, &frame_bb))
        {
            ItemSize(total_bb, style.FramePadding.y);
            EndGroup();
            return false;
        }
        if (!BeginChildFrame(id, frame_bb.GetSize()))
        {
            EndChildFrame();
            EndGroup();
            return false;
        }
        draw_window = GetCurrentWindow();
        draw_window->DC.NavLayerActiveMaskNext |= draw_window->DC.NavLayerCurrentMask; // This is to ensure that EndChild() will display a navigation highlight
        size.x -= draw_window->ScrollbarSizes.x;
    } else
    {
        ItemSize(total_bb, style.FramePadding.y);
        if (!ItemAdd(total_bb, id, &frame_bb))
            return false;
    }
    const bool hovered = ItemHoverable(frame_bb, id);
    if (hovered)
        g.MouseCursor = vsonyp0werMouseCursor_TextInput;

    // NB: we are only allowed to access 'edit_state' if we are the active widget.
    vsonyp0werInputTextState* state = NULL;
    if (g.InputTextState.ID == id)
        state = &g.InputTextState;

    const bool focus_requested = FocusableItemRegister(window, id);
    const bool focus_requested_by_code = focus_requested && (g.FocusRequestCurrWindow == window && g.FocusRequestCurrCounterAll == window->DC.FocusCounterAll);
    const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_nav_input_start = (g.ActiveId != id) && ((g.NavInputId == id) || (g.NavActivateId == id && g.NavInputSource == vsonyp0werInputSource_NavKeyboard));
    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetScrollbarID(draw_window, vsonyp0werAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetScrollbarID(draw_window, vsonyp0werAxis_Y);

    bool clear_active_id = false;
    bool select_all = (g.ActiveId != id) && ((flags & vsonyp0werInputTextFlags_AutoSelectAll) != 0 || user_nav_input_start) && (!is_multiline);

    const bool init_make_active = (focus_requested || user_clicked || user_scroll_finish || user_nav_input_start);
    const bool init_state = (init_make_active || user_scroll_active);
    if (init_state && g.ActiveId != id)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();

        // Take a copy of the initial buffer value (both in original UTF-8 format and converted to wchar)
        // From the moment we focused we are ignoring the content of 'buf' (unless we are in read-only mode)
        const int buf_len = (int)strlen(buf);
        state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
        memcpy(state->InitialTextA.Data, buf, buf_len + 1);

        // Start edition
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);          // wchar count <= UTF-8 count. we use +1 to make sure that .Data is always pointing to at least an empty string.
        state->TextA.resize(0);
        state->TextAIsValid = false;                // TextA is not valid yet (we will display buf until then)
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

        // Preserve cursor position and undo/redo stack if we come back to same widget
        // FIXME: For non-readonly widgets we might be able to require that TextAIsValid && TextA == buf ? (untested) and discard undo stack if user buffer has changed.
        const bool recycle_state = (state->ID == id);
        if (recycle_state)
        {
            // Recycle existing cursor/selection/undo stack but clamp position
            // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
            state->CursorClamp();
        } else
        {
            state->ID = id;
            state->ScrollX = 0.0f;
            stb_textedit_initialize_state(&state->Stb, !is_multiline);
            if (!is_multiline && focus_requested_by_code)
                select_all = true;
        }
        if (flags & vsonyp0werInputTextFlags_AlwaysInsertMode)
            state->Stb.insert_mode = 1;
        if (!is_multiline && (focus_requested_by_tab || (user_clicked && io.KeyCtrl)))
            select_all = true;
    }

    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
        IM_ASSERT(vsonyp0werNavInput_COUNT < 32);
        g.ActiveIdBlockNavInputFlags = (1 << vsonyp0werNavInput_Cancel);
        if (flags & (vsonyp0werInputTextFlags_CallbackCompletion | vsonyp0werInputTextFlags_AllowTabInput))  // Disable keyboard tabbing out as we will use the \t character.
            g.ActiveIdBlockNavInputFlags |= (1 << vsonyp0werNavInput_KeyTab_);
        if (!is_multiline && !(flags & vsonyp0werInputTextFlags_CallbackHistory))
            g.ActiveIdAllowNavDirFlags = ((1 << vsonyp0werDir_Up) | (1 << vsonyp0werDir_Down));
    }

    // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    // Release focus when we click outside
    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
        clear_active_id = true;

    // Lock the decision of whether we are going to take the path displaying the cursor or selection
    const bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool enter_pressed = false;

    // When read-only we always use the live data passed to the function
    // FIXME-OPT: Because our selection/cursor code currently needs the wide text we need to convert it when active, which is not ideal :(
    if (is_readonly && state != NULL && (render_cursor || render_selection))
    {
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);
        state->CursorClamp();
        render_selection &= state->HasSelection();
    }

    // Select the buffer to render.
    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
    const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

    // Password pushes a temporary font with only a fallback glyph
    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->DisplayOffset = g.Font->DisplayOffset;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    // Process mouse inputs and character inputs
    int backup_current_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        backup_current_text_length = state->CurLenA;
        state->BufCapacityA = buf_size;
        state->UserFlags = flags;
        state->UserCallback = callback;
        state->UserCallbackData = callback_user_data;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];
        g.WantTextInputNextFrame = 1;

        // Edit in progress
        const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y - style.FramePadding.y) : (g.FontSize * 0.5f));

        const bool is_osx = io.ConfigMacOSXBehaviors;
        if (select_all || (hovered && !is_osx && io.MouseDoubleClicked[0]))
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        } else if (hovered && is_osx && io.MouseDoubleClicked[0])
        {
            // Double-click select a word only, OS X style (by simulating keystrokes)
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
            state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
        } else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        } else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        // It is ill-defined whether the back-end needs to send a \t character when pressing the TAB keys.
        // Win32 and GLFW naturally do it but not SDL.
        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
        if ((flags & vsonyp0werInputTextFlags_AllowTabInput) && IsKeyPressedMap(vsonyp0werKey_Tab) && !ignore_char_inputs && !io.KeyShift && !is_readonly)
            if (!io.InputQueueCharacters.contains('\t'))
            {
                unsigned int c = '\t'; // Insert TAB
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }

        // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
        // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !user_nav_input_start)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {
                    // Insert character if they pass filtering
                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t' && io.KeyShift)
                        continue;
                    if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        state->OnKeyPressed((int)c);
                }

            // Consume characters
            io.InputQueueCharacters.resize(0);
        }
    }

    // Process other shortcuts/key-presses
    bool cancel_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);
        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_osx = io.ConfigMacOSXBehaviors;
        const bool is_shortcut_key = (is_osx ? (io.KeySuper && !io.KeyCtrl) : (io.KeyCtrl && !io.KeySuper)) && !io.KeyAlt && !io.KeyShift; // OS X style: Shortcuts using Cmd/Super instead of Ctrl
        const bool is_osx_shift_shortcut = is_osx && io.KeySuper && io.KeyShift && !io.KeyCtrl && !io.KeyAlt;
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End
        const bool is_ctrl_key_only = io.KeyCtrl && !io.KeyShift && !io.KeyAlt && !io.KeySuper;
        const bool is_shift_key_only = io.KeyShift && !io.KeyCtrl && !io.KeyAlt && !io.KeySuper;

        const bool is_cut = ((is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_X)) || (is_shift_key_only && IsKeyPressedMap(vsonyp0werKey_Delete))) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = ((is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_C)) || (is_ctrl_key_only && IsKeyPressedMap(vsonyp0werKey_Insert))) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = ((is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_V)) || (is_shift_key_only && IsKeyPressedMap(vsonyp0werKey_Insert))) && !is_readonly;
        const bool is_undo = ((is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_Z)) && !is_readonly && is_undoable);
        const bool is_redo = ((is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_Y)) || (is_osx_shift_shortcut && IsKeyPressedMap(vsonyp0werKey_Z))) && !is_readonly && is_undoable;

        if (IsKeyPressedMap(vsonyp0werKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetWindowScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetWindowScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_Delete) && !is_readonly) { state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask); } else if (IsKeyPressedMap(vsonyp0werKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        } else if (IsKeyPressedMap(vsonyp0werKey_Enter))
        {
            bool ctrl_enter_for_new_line = (flags & vsonyp0werInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                enter_pressed = clear_active_id = true;
            } else if (!is_readonly)
            {
                unsigned int c = '\n'; // Insert new line
                if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                    state->OnKeyPressed((int)c);
            }
        } else if (IsKeyPressedMap(vsonyp0werKey_Escape))
        {
            clear_active_id = cancel_edit = true;
        } else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        } else if (is_shortcut_key && IsKeyPressedMap(vsonyp0werKey_A))
        {
            state->SelectAll();
            state->CursorFollow = true;
        } else if (is_cut || is_copy)
        {
            // Cut, Copy
            if (io.SetClipboardTextFn)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                SetClipboardText(clipboard_data);
                MemFree(clipboard_data);
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, &state->Stb);
            }
        } else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s; )
                {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if (c == 0)
                        break;
                    if (c >= 0x10000 || !InputTextFilterCharacter(&c, flags, callback, callback_user_data))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    // Process callbacks and apply result back to user's buffer.
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        const char* apply_new_text = NULL;
        int apply_new_text_length = 0;
        if (cancel_edit)
        {
            // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
            if (!is_readonly && strcmp(buf, state->InitialTextA.Data) != 0)
            {
                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;
            }
        }

        // When using 'vsonyp0werInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
        // If we didn't do that, code like InputInt() with vsonyp0werInputTextFlags_EnterReturnsTrue would fail. Also this allows the user to use InputText() with vsonyp0werInputTextFlags_EnterReturnsTrue without maintaining any user-side stohnly.
        bool apply_edit_back_to_user_buffer = !cancel_edit || (enter_pressed && (flags & vsonyp0werInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.
            if (!is_readonly)
            {
                state->TextAIsValid = true;
                state->TextA.resize(state->TextW.Size * 4 + 1);
                ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
            }

            // User callback
            if ((flags & (vsonyp0werInputTextFlags_CallbackCompletion | vsonyp0werInputTextFlags_CallbackHistory | vsonyp0werInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                vsonyp0werInputTextFlags event_flag = 0;
                vsonyp0werKey event_key = vsonyp0werKey_COUNT;
                if ((flags & vsonyp0werInputTextFlags_CallbackCompletion) != 0 && IsKeyPressedMap(vsonyp0werKey_Tab))
                {
                    event_flag = vsonyp0werInputTextFlags_CallbackCompletion;
                    event_key = vsonyp0werKey_Tab;
                } else if ((flags & vsonyp0werInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(vsonyp0werKey_UpArrow))
                {
                    event_flag = vsonyp0werInputTextFlags_CallbackHistory;
                    event_key = vsonyp0werKey_UpArrow;
                } else if ((flags & vsonyp0werInputTextFlags_CallbackHistory) != 0 && IsKeyPressedMap(vsonyp0werKey_DownArrow))
                {
                    event_flag = vsonyp0werInputTextFlags_CallbackHistory;
                    event_key = vsonyp0werKey_DownArrow;
                } else if (flags & vsonyp0werInputTextFlags_CallbackAlways)
                    event_flag = vsonyp0werInputTextFlags_CallbackAlways;

                if (event_flag)
                {
                    vsonyp0werInputTextCallbackData callback_data;
                    memset(&callback_data, 0, sizeof(vsonyp0werInputTextCallbackData));
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    callback_data.EventKey = event_key;
                    callback_data.Buf = state->TextA.Data;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                    ImWchar* text = state->TextW.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    IM_ASSERT(callback_data.Buf == state->TextA.Data);  // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    if (callback_data.CursorPos != utf8_cursor_pos) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start) { state->Stb.select_start = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                    if (callback_data.SelectionEnd != utf8_selection_end) { state->Stb.select_end = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                    if (callback_data.BufDirty)
                    {
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                            state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length));
                        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                        state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        state->CursorAnimReset();
                    }
                }
            }

            // Will copy result string if modified
            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
            }
        }

        // Copy result to user buffer
        if (apply_new_text)
        {
            IM_ASSERT(apply_new_text_length >= 0);
            if (backup_current_text_length != apply_new_text_length && is_resizable)
            {
                vsonyp0werInputTextCallbackData callback_data;
                callback_data.EventFlag = vsonyp0werInputTextFlags_CallbackResize;
                callback_data.Flags = flags;
                callback_data.Buf = buf;
                callback_data.BufTextLen = apply_new_text_length;
                callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
                callback_data.UserData = callback_user_data;
                callback(&callback_data);
                buf = callback_data.Buf;
                buf_size = callback_data.BufSize;
                apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
                IM_ASSERT(apply_new_text_length <= buf_size);
            }

            // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
            ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
            value_changed = true;
        }

        // Clear temporary user stohnly
        state->UserFlags = 0;
        state->UserCallback = NULL;
        state->UserCallbackData = NULL;
    }

    // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
    if (clear_active_id && g.ActiveId == id)
        ClearActiveID();

    // Render frame
    if (!is_multiline)
    {
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(vsonyp0werCol_FrameBg), true, style.FrameRounding);
    }

    const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : frame_bb.Min + style.FramePadding;
    ImVec2 text_size(0.0f, 0.0f);

    // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
    // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
    // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
    const char* buf_display_end = NULL; // We have specialized paths below for setting the length
    if (is_displaying_hint)
    {
        buf_display = hint;
        buf_display_end = hint + strlen(hint);
    }

    // Render text. We currently only render selection when the widget is active or while scrolling.
    // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        // Render text (with cursor and selection)
        // This is going to be messy. We need to:
        // - Display the text (this alone can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
        const ImWchar * text_begin = state->TextW.Data;
        ImVec2 cursor_offset, select_start_offset;

        {
            // Find lines numbers straddling 'cursor' (slot 0) and 'select_start' (slot 1) positions.
            const ImWchar* searches_input_ptr[2] = { NULL, NULL };
            int searches_result_line_no[2] = { -1000, -1000 };
            int searches_remaining = 0;
            if (render_cursor)
            {
                searches_input_ptr[0] = text_begin + state->Stb.cursor;
                searches_result_line_no[0] = -1;
                searches_remaining++;
            }
            if (render_selection)
            {
                searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                searches_result_line_no[1] = -1;
                searches_remaining++;
            }

            // Iterate all lines to find our line numbers
            // In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
            searches_remaining += is_multiline ? 1 : 0;
            int line_count = 0;
            //for (const ImWchar* s = text_begin; (s = (const ImWchar*)wcschr((const wchar_t*)s, (wchar_t)'\n')) != NULL; s++)  // FIXME-OPT: Could use this when wchar_t are 16-bits
            for (const ImWchar* s = text_begin; *s != 0; s++)
                if (*s == '\n')
                {
                    line_count++;
                    if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                    if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                }
            line_count++;
            if (searches_result_line_no[0] == -1)
                searches_result_line_no[0] = line_count;
            if (searches_result_line_no[1] == -1)
                searches_result_line_no[1] = line_count;

            // Calculate 2d position by finding the beginning of the line and measuring distance
            cursor_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
            cursor_offset.y = searches_result_line_no[0] * g.FontSize;
            if (searches_result_line_no[1] >= 0)
            {
                select_start_offset.x = InputTextCalcTextSizeW(ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                select_start_offset.y = searches_result_line_no[1] * g.FontSize;
            }

            // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
            if (is_multiline)
                text_size = ImVec2(size.x, line_count * g.FontSize);
        }

        // Scroll
        if (render_cursor && state->CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & vsonyp0werInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = size.x * 0.25f;
                if (cursor_offset.x < state->ScrollX)
                    state->ScrollX = (float)(int)ImMax(0.0f, cursor_offset.x - scroll_increment_x);
                else if (cursor_offset.x - size.x >= state->ScrollX)
                    state->ScrollX = (float)(int)(cursor_offset.x - size.x + scroll_increment_x);
            } else
            {
                state->ScrollX = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                float scroll_y = draw_window->Scroll.y;
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - size.y >= scroll_y)
                    scroll_y = cursor_offset.y - size.y;
                draw_window->DC.CursorPos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                draw_window->Scroll.y = scroll_y;
                draw_pos.y = draw_window->DC.CursorPos.y;
            }

            state->CursorFollow = false;
        }

        // Draw selection
        const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);
        if (render_selection)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

            ImU32 bg_color = GetColorU32(vsonyp0werCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
            float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    //p = (const ImWchar*)wmemchr((const wchar_t*)p, '\n', text_selected_end - p);  // FIXME-OPT: Could use this when wchar_t are 16-bits
                    //p = p ? p + 1 : text_selected_end;
                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                } else
                {
                    ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = (float)(int)(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                    ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, bg_color);
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? vsonyp0werCol_TextDisabled : vsonyp0werCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }

        // Draw blinking cursor
        if (render_cursor)
        {
            state->CursorAnim += io.DeltaTime;
            bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
            ImVec2 cursor_screen_pos = draw_pos + cursor_offset - draw_scroll;
            ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
            if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
                draw_window->DrawList->AddLine(cursor_screen_rect.Min, cursor_screen_rect.GetBL(), GetColorU32(vsonyp0werCol_Text));

            // Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
            if (!is_readonly)
                g.PlatformImePos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
        }
    } else
    {
        // Render text only (no selection, no cursor)
        if (is_multiline)
            text_size = ImVec2(size.x, InputTextCalcTextLenAndLineCount(buf_display, &buf_display_end) * g.FontSize); // We don't need width
        else if (!is_displaying_hint && g.ActiveId == id)
            buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint)
            buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
        {
            ImU32 col = GetColorU32(is_displaying_hint ? vsonyp0werCol_TextDisabled : vsonyp0werCol_Text);
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, col, buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
        }
    }

    if (is_multiline)
    {
        Dummy(text_size + ImVec2(0.0f, g.FontSize)); // Always add room to scroll an extra line
        EndChildFrame();
        EndGroup();
    }

    if (is_password && !is_displaying_hint)
        PopFont();

    // Log as text
    if (g.LogEnabled && !(is_password && !is_displaying_hint))
        LogRenderedText(&draw_pos, buf_display, buf_display_end);

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    if (value_changed)
        MarkItemEdited(id);

    vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    if ((flags & vsonyp0werInputTextFlags_EnterReturnsTrue) != 0)
        return enter_pressed;
    else
        return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ColorEdit, ColorPicker, ColorButton, etc.
//-------------------------------------------------------------------------
// - ColorEdit3()
// - ColorEdit4()
// - ColorPicker3()
// - RenderColorRectWithAlphaCheckerboard() [Internal]
// - ColorPicker4()
// - ColorButton()
// - SetColorEditOptions()
// - ColorTooltip() [Internal]
// - ColorEditOptionsPopup() [Internal]
// - ColorPickerOptionsPopup() [Internal]
//-------------------------------------------------------------------------

bool vsonyp0wer::ColorEdit3(const char* label, float col[3], vsonyp0werColorEditFlags flags)
{
    return ColorEdit4(label, col, flags | vsonyp0werColorEditFlags_NoAlpha);
}

// Edit colors components (each component in 0.0f..1.0f range).
// See enum vsonyp0werColorEditFlags_ for available options. e.g. Only access 3 floats if vsonyp0werColorEditFlags_NoAlpha flag is set.
// With typical options: Left-click on colored square to open color picker. Right-click to open option menu. CTRL-Click over input fields to edit them and TAB to go to next item.

bool vsonyp0wer::ColorEdit4(const char* label, float col[4], vsonyp0werColorEditFlags flags) {

    vsonyp0werWindow* window = GetCurrentWindow();

    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_extra = (flags & vsonyp0werColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_items_all = CalcItemWidth() - w_extra;
    const char* label_display_end = FindRenderedTextEnd(label);

    BeginGroup();
    PushID(label);

    const vsonyp0werColorEditFlags flags_untouched = flags;

    if (flags & vsonyp0werColorEditFlags_NoInputs)
        flags = (flags & (~vsonyp0werColorEditFlags__DisplayMask)) | vsonyp0werColorEditFlags_DisplayRGB | vsonyp0werColorEditFlags_NoOptions;

    if (!(flags & vsonyp0werColorEditFlags_NoOptions))
        ColorEditOptionsPopup(col, flags);

    if (!(flags & vsonyp0werColorEditFlags__DisplayMask))
        flags |= (g.ColorEditOptions & vsonyp0werColorEditFlags__DisplayMask);

    if (!(flags & vsonyp0werColorEditFlags__DataTypeMask))
        flags |= (g.ColorEditOptions & vsonyp0werColorEditFlags__DataTypeMask);

    if (!(flags & vsonyp0werColorEditFlags__PickerMask))
        flags |= (g.ColorEditOptions & vsonyp0werColorEditFlags__PickerMask);

    if (!(flags & vsonyp0werColorEditFlags__InputMask))
        flags |= (g.ColorEditOptions & vsonyp0werColorEditFlags__InputMask);

    flags |= (g.ColorEditOptions & ~(vsonyp0werColorEditFlags__DisplayMask | vsonyp0werColorEditFlags__DataTypeMask | vsonyp0werColorEditFlags__PickerMask | vsonyp0werColorEditFlags__InputMask));

    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__DisplayMask)); // Check that only 1 is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__InputMask));   // Check that only 1 is selected

    const bool alpha = (flags & vsonyp0werColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & vsonyp0werColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };

    if ((flags & vsonyp0werColorEditFlags_InputHSV) && (flags & vsonyp0werColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & vsonyp0werColorEditFlags_InputRGB) && (flags & vsonyp0werColorEditFlags_DisplayHSV))
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    if ((flags & (vsonyp0werColorEditFlags_DisplayRGB | vsonyp0werColorEditFlags_DisplayHSV)) != 0 && (flags & vsonyp0werColorEditFlags_NoInputs) == 0) {

        const float w_item_one = ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
        const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

        const bool hide_prefix = (w_item_one <= CalcTextSize((flags & vsonyp0werColorEditFlags_Float) ? "M:0.000" : "M:000").x);

        static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };

        static const char* fmt_table_int[3][4] = {

            {   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
            { "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
            { "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
        };

        static const char* fmt_table_float[3][4] = {

            {   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
            { "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
            { "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
        };

        const int fmt_idx = hide_prefix ? 0 : (flags & vsonyp0werColorEditFlags_DisplayHSV) ? 2 : 1;

        PushItemWidth(w_item_one);

        for (int n = 0; n < components; n++) {

            if (n > 0)
                SameLine(0, style.ItemInnerSpacing.x);

            if (n + 1 == components)
                PushItemWidth(w_item_last);

            if (flags & vsonyp0werColorEditFlags_Float) {

                value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
                value_changed_as_float |= value_changed;
            }
			else
                value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);

			if (!(flags & vsonyp0werColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context");
        }

        PopItemWidth();
        PopItemWidth();

    }
	else if ((flags & vsonyp0werColorEditFlags_DisplayHex) != 0 && (flags & vsonyp0werColorEditFlags_NoInputs) == 0) {

        char buf[64];

        if (alpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

        PushItemWidth(w_items_all);

        if (InputText("##Text", buf, IM_ARRAYSIZE(buf), vsonyp0werInputTextFlags_CharsHexadecimal | vsonyp0werInputTextFlags_CharsUppercase)) {

            value_changed = true;
            char* p = buf;

            while (*p == '#' || ImCharIsBlankA(*p))
                p++;

            i[0] = i[1] = i[2] = i[3] = 0;

            if (alpha)
                sscanf(p, "%02X%02X%02X%02X", (unsigned int*)& i[0], (unsigned int*)& i[1], (unsigned int*)& i[2], (unsigned int*)& i[3]); // Treat at unsigned (%X is unsigned)
            else
                sscanf(p, "%02X%02X%02X", (unsigned int*)& i[0], (unsigned int*)& i[1], (unsigned int*)& i[2]);
        }

        if (!(flags & vsonyp0werColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        PopItemWidth();
    }

    vsonyp0werWindow* picker_active_window = NULL;

    if (!(flags & vsonyp0werColorEditFlags_NoSmallPreview)) {

        if (!(flags & vsonyp0werColorEditFlags_NoInputs))
            SameLine(0, style.ItemInnerSpacing.x);

        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

        if (ColorButton("##ColorButton", col_v4, flags)) {

            if (!(flags & vsonyp0werColorEditFlags_NoPicker)) {

                g.ColorPickerRef = col_v4;
                OpenPopup("picker");
                SetNextWindowPos(window->DC.LastItemRect.GetBL() + ImVec2(-1, style.ItemSpacing.y));
            }
        }

        if (!(flags & vsonyp0werColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context");

        if (BeginPopup("picker")) {

            picker_active_window = g.CurrentWindow;

            if (label != label_display_end) {

                TextEx(label, label_display_end);
                Spacing();
            }

            vsonyp0werColorEditFlags picker_flags_to_forward = vsonyp0werColorEditFlags__DataTypeMask | vsonyp0werColorEditFlags__PickerMask | vsonyp0werColorEditFlags__InputMask | vsonyp0werColorEditFlags_HDR | vsonyp0werColorEditFlags_NoAlpha | vsonyp0werColorEditFlags_AlphaBar;
            vsonyp0werColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | vsonyp0werColorEditFlags_DisplayHSV | vsonyp0werColorEditFlags_NoLabel | vsonyp0werColorEditFlags_AlphaPreviewHalf;

            PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
            value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
            PopItemWidth();

            EndPopup();
        }
    }

    if (label != label_display_end && !(flags & vsonyp0werColorEditFlags_NoLabel)) {

        SameLine(0, style.ItemInnerSpacing.x);
        TextEx(label, label_display_end);
    }

    if (value_changed && picker_active_window == NULL) {

		if (!value_changed_as_float) {

			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;
		}
            
        if ((flags & vsonyp0werColorEditFlags_DisplayHSV) && (flags & vsonyp0werColorEditFlags_InputRGB))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
        if ((flags & vsonyp0werColorEditFlags_DisplayRGB) && (flags & vsonyp0werColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];

        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    if ((window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_HoveredRect) && !(flags & vsonyp0werColorEditFlags_NoDragDrop) && BeginDragDropTarget()) {

        bool accepted_drag_drop = false;

        if (const vsonyp0werPayload * payload = AcceptDragDropPayload(vsonyp0wer_PAYLOAD_TYPE_COLOR_3F)) {

            memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512
            value_changed = accepted_drag_drop = true;
        }

        if (const vsonyp0werPayload * payload = AcceptDragDropPayload(vsonyp0wer_PAYLOAD_TYPE_COLOR_4F)) {

            memcpy((float*)col, payload->Data, sizeof(float) * components);
            value_changed = accepted_drag_drop = true;
        }

        if (accepted_drag_drop && (flags & vsonyp0werColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);

        EndDragDropTarget();
    }

    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        window->DC.LastItemId = g.ActiveId;

    if (value_changed)
        MarkItemEdited(window->DC.LastItemId);

    return value_changed;
}

bool vsonyp0wer::ColorPicker3(const char* label, float col[3], vsonyp0werColorEditFlags flags)
{
    float col4[4] = { col[0], col[1], col[2], 1.0f };
    if (!ColorPicker4(label, col4, flags | vsonyp0werColorEditFlags_NoAlpha))
        return false;
    col[0] = col4[0]; col[1] = col4[1]; col[2] = col4[2];
    return true;
}

static inline ImU32 ImAlphaBlendColor(ImU32 col_a, ImU32 col_b)
{
    float t = ((col_b >> IM_COL32_A_SHIFT) & 0xFF) / 255.f;
    int r = ImLerp((int)(col_a >> IM_COL32_R_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_R_SHIFT) & 0xFF, t);
    int g = ImLerp((int)(col_a >> IM_COL32_G_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_G_SHIFT) & 0xFF, t);
    int b = ImLerp((int)(col_a >> IM_COL32_B_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_B_SHIFT) & 0xFF, t);
    return IM_COL32(r, g, b, 0xFF);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners overlap multiple cells. Caller currently responsible for avoiding that.
// I spent a non reasonable amount of time trying to getting this right for ColorButton with rounding+anti-aliasing+vsonyp0werColorEditFlags_HalfAlphaPreview flag + various grid sizes and offsets, and eventually gave up... probably more reasonable to disable rounding alltogether.
void vsonyp0wer::RenderColorRectWithAlphaCheckerboard(ImVec2 p_min, ImVec2 p_max, ImU32 col, float grid_step, ImVec2 grid_off, float rounding, int rounding_corners_flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (((col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) < 0xFF)
    {
        ImU32 col_bg1 = GetColorU32(ImAlphaBlendColor(IM_COL32(204, 204, 204, 255), col));
        ImU32 col_bg2 = GetColorU32(ImAlphaBlendColor(IM_COL32(128, 128, 128, 255), col));
        window->DrawList->AddRectFilled(p_min, p_max, col_bg1, rounding, rounding_corners_flags);

        int yi = 0;
        for (float y = p_min.y + grid_off.y; y < p_max.y; y += grid_step, yi++)
        {
            float y1 = ImClamp(y, p_min.y, p_max.y), y2 = ImMin(y + grid_step, p_max.y);
            if (y2 <= y1)
                continue;
            for (float x = p_min.x + grid_off.x + (yi & 1) * grid_step; x < p_max.x; x += grid_step * 2.0f)
            {
                float x1 = ImClamp(x, p_min.x, p_max.x), x2 = ImMin(x + grid_step, p_max.x);
                if (x2 <= x1)
                    continue;
                int rounding_corners_flags_cell = 0;
                if (y1 <= p_min.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_TopRight; }
                if (y2 >= p_max.y) { if (x1 <= p_min.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotLeft; if (x2 >= p_max.x) rounding_corners_flags_cell |= ImDrawCornerFlags_BotRight; }
                rounding_corners_flags_cell &= rounding_corners_flags;
                window->DrawList->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col_bg2, rounding_corners_flags_cell ? rounding : 0.0f, rounding_corners_flags_cell);
            }
        }
    } else
    {
        window->DrawList->AddRectFilled(p_min, p_max, col, rounding, rounding_corners_flags);
    }
}

// Helper for ColorPicker4()
static void RenderArrowsForVerticalBar(ImDrawList * draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w)
{
    vsonyp0wer::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), vsonyp0werDir_Right, IM_COL32_BLACK);
    vsonyp0wer::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, vsonyp0werDir_Right, IM_COL32_WHITE);
    vsonyp0wer::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), vsonyp0werDir_Left, IM_COL32_BLACK);
    vsonyp0wer::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, vsonyp0werDir_Left, IM_COL32_WHITE);
}

static inline float ColorSquareSize2() {

	vsonyp0werContext& g = *Gvsonyp0wer;
	return g.FontSize + g.Style.FramePadding.y * 2.0f;
}

static void ColorPickerOptionsPopup2(vsonyp0werColorEditFlags flags, float* ref_col) {

	bool allow_opt_picker = !(flags & vsonyp0werColorEditFlags__PickerMask);
	bool allow_opt_alpha_bar = !(flags & vsonyp0werColorEditFlags_NoAlpha) && !(flags & vsonyp0werColorEditFlags_AlphaBar);

	if ((!allow_opt_picker && !allow_opt_alpha_bar) || !vsonyp0wer::BeginPopup("context"))
		return;

	vsonyp0werContext& g = *Gvsonyp0wer;

	if (allow_opt_picker) {

		ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (ColorSquareSize2() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
		vsonyp0wer::PushItemWidth(picker_size.x);

		for (int picker_type = 0; picker_type < 2; picker_type++) {

			if (picker_type > 0)
				vsonyp0wer::Separator();

			vsonyp0wer::PushID(picker_type);
			vsonyp0werColorEditFlags picker_flags = vsonyp0werColorEditFlags_NoInputs | vsonyp0werColorEditFlags_NoOptions | vsonyp0werColorEditFlags_NoLabel | vsonyp0werColorEditFlags_NoSidePreview | (flags & vsonyp0werColorEditFlags_NoAlpha);

			if (picker_type == 0)
				picker_flags |= vsonyp0werColorEditFlags_PickerHueBar;

			if (picker_type == 1)
				picker_flags |= vsonyp0werColorEditFlags_PickerHueWheel;

			ImVec2 backup_pos = vsonyp0wer::GetCursorScreenPos();

			if (vsonyp0wer::Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
				g.ColorEditOptions = (g.ColorEditOptions & ~vsonyp0werColorEditFlags__PickerMask) | (picker_flags & vsonyp0werColorEditFlags__PickerMask);

			vsonyp0wer::SetCursorScreenPos(backup_pos);
			ImVec4 dummy_ref_col;
			memcpy(&dummy_ref_col.x, ref_col, sizeof(float) * (picker_flags & vsonyp0werColorEditFlags_NoAlpha ? 3 : 4));
			vsonyp0wer::ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
			vsonyp0wer::PopID();
		}
		vsonyp0wer::PopItemWidth();
	}

	if (allow_opt_alpha_bar) {

		if (allow_opt_picker)
			vsonyp0wer::Separator();

		vsonyp0wer::CheckboxFlags("Alpha Bar", (unsigned int*)&g.ColorEditOptions, vsonyp0werColorEditFlags_AlphaBar);
	}

	vsonyp0wer::EndPopup();
}

// Note: ColorPicker4() only accesses 3 floats if vsonyp0werColorEditFlags_NoAlpha flag is set.
// (In C++ the 'float col[4]' notation for a function argument is equivalent to 'float* col', we only specify a size to facilitate understanding of the code.)
// FIXME: we adjust the big color square height based on item width, which may cause a flickering feedback loop (if automatic height makes a vertical scrollbar appears, affecting automatic width..)
bool vsonyp0wer::ColorPicker4(const char* label, float col[4], vsonyp0werColorEditFlags flags, const float* ref_col)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = GetCurrentWindow();
	ImDrawList* draw_list = window->DrawList;
	vsonyp0werStyle& style = g.Style;
	vsonyp0werIO& io = g.IO;

	PushID(label);
	BeginGroup();

	if (!(flags & vsonyp0werColorEditFlags_NoSidePreview))
		flags |= vsonyp0werColorEditFlags_NoSmallPreview;

	if (!(flags & vsonyp0werColorEditFlags_NoOptions))
		ColorPickerOptionsPopup2(flags, col);

	if (!(flags & vsonyp0werColorEditFlags__PickerMask))
		flags |= ((g.ColorEditOptions & vsonyp0werColorEditFlags__PickerMask) ? g.ColorEditOptions : vsonyp0werColorEditFlags__OptionsDefault) & vsonyp0werColorEditFlags__PickerMask;

	IM_ASSERT(ImIsPowerOfTwo((int)(flags & vsonyp0werColorEditFlags__PickerMask))); // Check that only 1 is selected

	if (!(flags & vsonyp0werColorEditFlags_NoOptions))
		flags |= (g.ColorEditOptions & vsonyp0werColorEditFlags_AlphaBar);

	// Setup
	bool alpha_bar;

	if (flags & vsonyp0werColorEditFlags_AlphaBar)
		alpha_bar = true;
	else if (flags & vsonyp0werColorEditFlags_NoAlpha)
		alpha_bar = false;

	ImVec2 picker_pos = window->DC.CursorPos + ImVec2(1, 1);
	float bars_width = 12; // Arbitrary smallish width of Hue/Alpha picking bars
	float sv_picker_size = 152; // ImMax( bars_width * 1, CalcItemWidth( ) - ( alpha_bar ? 2 : 1 ) * ( bars_width + style.ItemInnerSpacing.x ) ); // Saturation/Value picking box
	float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
	float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
	float bars_triangles_half_sz = (float)(int)(bars_width * 0.20f);

	float wheel_thickness = sv_picker_size * 0.08f;
	float wheel_r_outer = sv_picker_size * 0.50f;
	float wheel_r_inner = wheel_r_outer - wheel_thickness;
	ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width)*0.5f, picker_pos.y + sv_picker_size * 0.5f);

	// Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated for logic.
	float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
	ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f); // Hue point.
	ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f); // Black point.
	ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f); // White point.

	float H, S, V;
	ColorConvertRGBtoHSV(col[0], col[1], col[2], H, S, V);

	bool value_changed = false, value_changed_h = false, value_changed_sv = false;

	// SV rectangle logic
	InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
	if (IsItemActive()) {
		S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
		V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
		value_changed = value_changed_sv = true;
	}

	// Hue bar logic
	SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
	InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
	if (IsItemActive()) {
		H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
		value_changed = value_changed_h = true;
	}

	if (!(flags & vsonyp0werColorEditFlags_NoLabel)) {
		const char* label_display_end = FindRenderedTextEnd(label);
		if (label != label_display_end) {
			if ((flags & vsonyp0werColorEditFlags_NoSidePreview))
				SameLine(0, style.ItemInnerSpacing.x);
		}
	}

	// Convert back color to RGB
	if (value_changed_h || value_changed_sv)
		ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f, col[0], col[1], col[2]);

	// Try to cancel hue wrap (after ColorEdit), if any
	if (value_changed) {
		float new_H, new_S, new_V;
		ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0) {
			if (new_V <= 0 && V != new_V)
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
			else if (new_S <= 0)
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
		}
	}

	ImVec4 hue_color_f(1, 1, 1, 1); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
	ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
	ImU32 col32_no_alpha = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 1.0f));

	const ImU32 hue_colors[6 + 1] = { IM_COL32(255,0,0,255), IM_COL32(255,255,0,255), IM_COL32(0,255,0,255), IM_COL32(0,255,255,255), IM_COL32(0,0,255,255), IM_COL32(255,0,255,255), IM_COL32(255,0,0,255) };
	ImVec2 sv_cursor_pos;

	if (flags & vsonyp0werColorEditFlags_PickerHueBar) {
		// Alpha bar logic
		if (alpha_bar) {
			SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
			InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
			if (IsItemActive()) {
				col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
				value_changed = true;
			}
		}

		// Render SV Square
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), IM_COL32_WHITE, hue_color32, hue_color32, IM_COL32_WHITE);
		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), IM_COL32_BLACK_TRANS, IM_COL32_BLACK_TRANS, IM_COL32_BLACK, IM_COL32_BLACK);
		draw_list->AddRect(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), IM_COL32_BLACK);

		sv_cursor_pos.x = ImClamp((float)(int)(picker_pos.x + ImSaturate(S) * sv_picker_size + 0.5f), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
		sv_cursor_pos.y = ImClamp((float)(int)(picker_pos.y + ImSaturate(1 - V) * sv_picker_size + 0.5f), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

		// Render Hue Bar
		for (int i = 0; i < 6; ++i)
			draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), hue_colors[i], hue_colors[i], hue_colors[i + 1], hue_colors[i + 1]);

		float bar0_line_y = (float)(int)(picker_pos.y + H * (sv_picker_size - 4.0f + 0.5f));
		draw_list->AddRectFilled(ImVec2(bar0_pos_x, bar0_line_y), ImVec2(bar0_pos_x + bars_width, bar0_line_y + 4), IM_COL32_BLACK);
		draw_list->AddRectFilled(ImVec2(bar0_pos_x + 0, bar0_line_y + 1), ImVec2(bar0_pos_x + bars_width - 1, bar0_line_y + 3), hue_color32);

		draw_list->AddRect(ImVec2(bar0_pos_x - 1, picker_pos.y - 0), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size + 0), IM_COL32(0, 0, 0, 255));
	}

	// Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to be out of range)
	float sv_cursor_rad = 3;
	draw_list->AddRectFilled(sv_cursor_pos - ImVec2(2, 2), sv_cursor_pos + ImVec2(2, 2), IM_COL32(0, 0, 0, 255));
	draw_list->AddRectFilled(sv_cursor_pos - ImVec2(1, 1), sv_cursor_pos + ImVec2(1, 1), col32_no_alpha);

	if (alpha_bar) {

		float alpha = ImSaturate(col[3]);

		ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);

		RenderColorRectWithAlphaCheckerboard(bar1_bb.Min, bar1_bb.Max, IM_COL32(0, 0, 0, 255), bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));

		draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, col32_no_alpha, col32_no_alpha, col32_no_alpha & ~IM_COL32_A_MASK, col32_no_alpha & ~IM_COL32_A_MASK);
		draw_list->AddRect(bar1_bb.Min - ImVec2(1, 0), bar1_bb.Max, IM_COL32_BLACK);

		float bar1_line_y = (float)(int)(picker_pos.y + (1.0f - alpha) * (sv_picker_size - 4.0f + 0.5f));

		RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);

		draw_list->AddRect(ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bar1_pos_x + bars_width, bar1_line_y + 4), IM_COL32_BLACK);
		draw_list->AddRectFilled(ImVec2(bar1_pos_x + 0, bar1_line_y + 1), ImVec2(bar1_pos_x + bars_width - 1, bar1_line_y + 3), IM_COL32(col[0], col[1], col[2], col[3]));
	}

	EndGroup();
	PopID();

	return value_changed;
}

// A little colored square. Return true when clicked.
// FIXME: May want to display/ignore the alpha component in the color display? Yet show it in the tooltip.
// 'desc_id' is not called 'label' because we don't display it next to the button, but only in the tooltip.
// Note that 'col' may be encoded in HSV if vsonyp0werColorEditFlags_InputHSV is set.
bool vsonyp0wer::ColorButton(const char* desc_id, const ImVec4 & col, vsonyp0werColorEditFlags flags, ImVec2 size)
{
	vsonyp0werWindow* window = GetCurrentWindow();

	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;

	const vsonyp0werID id = window->GetID(desc_id);
	float default_size = GetFrameHeight();

	if (size.x == 0.0f)
		size.x = default_size;

	if (size.y == 0.0f)
		size.y = default_size;

	const ImRect bb(window->DC.CursorPos + ImVec2(0, 3), window->DC.CursorPos + size - ImVec2(0, 5));
	ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);

	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (flags & vsonyp0werColorEditFlags_NoAlpha)
		flags &= ~(vsonyp0werColorEditFlags_AlphaPreview | vsonyp0werColorEditFlags_AlphaPreviewHalf);

	ImVec4 col_without_alpha(col.x, col.y, col.z, 1.0f);

	float grid_step = ImMin(size.x, size.y) / 2.99f;
	float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);

	ImRect bb_inner = bb;

	ImVec4 col_source = (flags & vsonyp0werColorEditFlags_AlphaPreview) ? col : col_without_alpha;
	auto gradient = GetColorU32(col_source) - ImColor(0, 0, 0, 100);
	window->DrawList->AddRectFilledMultiColor(bb_inner.Min, bb_inner.Max, GetColorU32(col_source), GetColorU32(col_source), gradient, gradient);

	RenderNavHighlight(bb, id);

	if (g.Style.FrameBorderSize > 0.0f)
		RenderFrameBorder(bb.Min, bb.Max, rounding);
	else
		window->DrawList->AddRect(bb.Min, bb.Max, ImColor(10, 10, 10, 255), rounding); // Color button are often in need of some sort of border

	// Tooltip
	if (!(flags & vsonyp0werColorEditFlags_NoTooltip) && hovered)
		ColorTooltip(desc_id, &col.x, flags & (vsonyp0werColorEditFlags_NoAlpha | vsonyp0werColorEditFlags_AlphaPreview | vsonyp0werColorEditFlags_AlphaPreviewHalf));

	if (pressed)
		MarkItemEdited(id);

	return pressed;
}

// Initialize/override default color options
void vsonyp0wer::SetColorEditOptions(vsonyp0werColorEditFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if ((flags & vsonyp0werColorEditFlags__DisplayMask) == 0)
        flags |= vsonyp0werColorEditFlags__OptionsDefault & vsonyp0werColorEditFlags__DisplayMask;
    if ((flags & vsonyp0werColorEditFlags__DataTypeMask) == 0)
        flags |= vsonyp0werColorEditFlags__OptionsDefault & vsonyp0werColorEditFlags__DataTypeMask;
    if ((flags & vsonyp0werColorEditFlags__PickerMask) == 0)
        flags |= vsonyp0werColorEditFlags__OptionsDefault & vsonyp0werColorEditFlags__PickerMask;
    if ((flags & vsonyp0werColorEditFlags__InputMask) == 0)
        flags |= vsonyp0werColorEditFlags__OptionsDefault & vsonyp0werColorEditFlags__InputMask;
    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__DisplayMask));    // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__DataTypeMask));   // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__PickerMask));     // Check only 1 option is selected
    IM_ASSERT(ImIsPowerOfTwo(flags & vsonyp0werColorEditFlags__InputMask));      // Check only 1 option is selected
    g.ColorEditOptions = flags;
}

// Note: only access 3 floats if vsonyp0werColorEditFlags_NoAlpha flag is set.
void vsonyp0wer::ColorTooltip(const char* text, const float* col, vsonyp0werColorEditFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    BeginTooltipEx(0, true);
    const char* text_end = text ? FindRenderedTextEnd(text, NULL) : text;
    if (text_end > text)
    {
        TextEx(text, text_end);
        Separator();
    }

    ImVec2 sz(g.FontSize * 3 + g.Style.FramePadding.y * 2, g.FontSize * 3 + g.Style.FramePadding.y * 2);
    ImVec4 cf(col[0], col[1], col[2], (flags & vsonyp0werColorEditFlags_NoAlpha) ? 1.0f : col[3]);
    int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & vsonyp0werColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
    ColorButton("##preview", cf, (flags & (vsonyp0werColorEditFlags__InputMask | vsonyp0werColorEditFlags_NoAlpha | vsonyp0werColorEditFlags_AlphaPreview | vsonyp0werColorEditFlags_AlphaPreviewHalf)) | vsonyp0werColorEditFlags_NoTooltip, sz);
    SameLine();
    if ((flags & vsonyp0werColorEditFlags_InputRGB) || !(flags & vsonyp0werColorEditFlags__InputMask))
    {
        if (flags & vsonyp0werColorEditFlags_NoAlpha)
            Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
        else
            Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
    } else if (flags & vsonyp0werColorEditFlags_InputHSV)
    {
        if (flags & vsonyp0werColorEditFlags_NoAlpha)
            Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
        else
            Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
    }
    EndTooltip();
}

void vsonyp0wer::ColorEditOptionsPopup(const float* col, vsonyp0werColorEditFlags flags)
{
    bool allow_opt_inputs = !(flags & vsonyp0werColorEditFlags__DisplayMask);
    bool allow_opt_datatype = !(flags & vsonyp0werColorEditFlags__DataTypeMask);
    if ((!allow_opt_inputs && !allow_opt_datatype) || !BeginPopup("context"))
        return;
    vsonyp0werContext & g = *Gvsonyp0wer;
    vsonyp0werColorEditFlags opts = g.ColorEditOptions;
    if (allow_opt_inputs)
    {
        if (RadioButton("RGB", (opts & vsonyp0werColorEditFlags_DisplayRGB) != 0)) opts = (opts & ~vsonyp0werColorEditFlags__DisplayMask) | vsonyp0werColorEditFlags_DisplayRGB;
        if (RadioButton("HSV", (opts & vsonyp0werColorEditFlags_DisplayHSV) != 0)) opts = (opts & ~vsonyp0werColorEditFlags__DisplayMask) | vsonyp0werColorEditFlags_DisplayHSV;
        if (RadioButton("Hex", (opts & vsonyp0werColorEditFlags_DisplayHex) != 0)) opts = (opts & ~vsonyp0werColorEditFlags__DisplayMask) | vsonyp0werColorEditFlags_DisplayHex;
    }
    if (allow_opt_datatype)
    {
        if (allow_opt_inputs) Separator();
        if (RadioButton("0..255", (opts & vsonyp0werColorEditFlags_Uint8) != 0)) opts = (opts & ~vsonyp0werColorEditFlags__DataTypeMask) | vsonyp0werColorEditFlags_Uint8;
        if (RadioButton("0.00..1.00", (opts & vsonyp0werColorEditFlags_Float) != 0)) opts = (opts & ~vsonyp0werColorEditFlags__DataTypeMask) | vsonyp0werColorEditFlags_Float;
    }

    if (allow_opt_inputs || allow_opt_datatype)
        Separator();
    if (Button("Copy as..", ImVec2(-1, 0)))
        OpenPopup("Copy");
    if (BeginPopup("Copy"))
    {
        int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & vsonyp0werColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
        char buf[64];
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%.3ff, %.3ff, %.3ff, %.3ff)", col[0], col[1], col[2], (flags & vsonyp0werColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        if (Selectable(buf))
            SetClipboardText(buf);
        ImFormatString(buf, IM_ARRAYSIZE(buf), "(%d,%d,%d,%d)", cr, cg, cb, ca);
        if (Selectable(buf))
            SetClipboardText(buf);
        if (flags & vsonyp0werColorEditFlags_NoAlpha)
            ImFormatString(buf, IM_ARRAYSIZE(buf), "0x%02X%02X%02X", cr, cg, cb);
        else
            ImFormatString(buf, IM_ARRAYSIZE(buf), "0x%02X%02X%02X%02X", cr, cg, cb, ca);
        if (Selectable(buf))
            SetClipboardText(buf);
        EndPopup();
    }

    g.ColorEditOptions = opts;
    EndPopup();
}

void vsonyp0wer::ColorPickerOptionsPopup(const float* ref_col, vsonyp0werColorEditFlags flags)
{
    bool allow_opt_picker = !(flags & vsonyp0werColorEditFlags__PickerMask);
    bool allow_opt_alpha_bar = !(flags & vsonyp0werColorEditFlags_NoAlpha) && !(flags & vsonyp0werColorEditFlags_AlphaBar);
    if ((!allow_opt_picker && !allow_opt_alpha_bar) || !BeginPopup("context"))
        return;
    vsonyp0werContext & g = *Gvsonyp0wer;
    if (allow_opt_picker)
    {
        ImVec2 picker_size(g.FontSize * 8, ImMax(g.FontSize * 8 - (GetFrameHeight() + g.Style.ItemInnerSpacing.x), 1.0f)); // FIXME: Picker size copied from main picker function
        PushItemWidth(picker_size.x);
        for (int picker_type = 0; picker_type < 2; picker_type++)
        {
            // Draw small/thumbnail version of each picker type (over an invisible button for selection)
            if (picker_type > 0) Separator();
            PushID(picker_type);
            vsonyp0werColorEditFlags picker_flags = vsonyp0werColorEditFlags_NoInputs | vsonyp0werColorEditFlags_NoOptions | vsonyp0werColorEditFlags_NoLabel | vsonyp0werColorEditFlags_NoSidePreview | (flags & vsonyp0werColorEditFlags_NoAlpha);
            if (picker_type == 0) picker_flags |= vsonyp0werColorEditFlags_PickerHueBar;
            if (picker_type == 1) picker_flags |= vsonyp0werColorEditFlags_PickerHueWheel;
            ImVec2 backup_pos = GetCursorScreenPos();
            if (Selectable("##selectable", false, 0, picker_size)) // By default, Selectable() is closing popup
                g.ColorEditOptions = (g.ColorEditOptions & ~vsonyp0werColorEditFlags__PickerMask) | (picker_flags & vsonyp0werColorEditFlags__PickerMask);
            SetCursorScreenPos(backup_pos);
            ImVec4 dummy_ref_col;
            memcpy(&dummy_ref_col, ref_col, sizeof(float) * ((picker_flags & vsonyp0werColorEditFlags_NoAlpha) ? 3 : 4));
            ColorPicker4("##dummypicker", &dummy_ref_col.x, picker_flags);
            PopID();
        }
        PopItemWidth();
    }
    if (allow_opt_alpha_bar)
    {
        if (allow_opt_picker) Separator();
        CheckboxFlags("Alpha Bar", (unsigned int*)& g.ColorEditOptions, vsonyp0werColorEditFlags_AlphaBar);
    }
    EndPopup();
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: TreeNode, CollapsingHeader, etc.
//-------------------------------------------------------------------------
// - TreeNode()
// - TreeNodeV()
// - TreeNodeEx()
// - TreeNodeExV()
// - TreeNodeBehavior() [Internal]
// - TreePush()
// - TreePop()
// - TreeAdvanceToLabelPos()
// - GetTreeNodeToLabelSpacing()
// - SetNextTreeNodeOpen()
// - CollapsingHeader()
//-------------------------------------------------------------------------

bool vsonyp0wer::TreeNode(const char* str_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool vsonyp0wer::TreeNode(const void* ptr_id, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, 0, fmt, args);
    va_end(args);
    return is_open;
}

bool vsonyp0wer::TreeNode(const char* label)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    return TreeNodeBehavior(window->GetID(label), 0, label, NULL);
}

bool vsonyp0wer::TreeNodeV(const char* str_id, const char* fmt, va_list args)
{
    return TreeNodeExV(str_id, 0, fmt, args);
}

bool vsonyp0wer::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
{
    return TreeNodeExV(ptr_id, 0, fmt, args);
}

bool vsonyp0wer::TreeNodeEx(const char* label, vsonyp0werTreeNodeFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags, label, NULL);
}

bool vsonyp0wer::TreeNodeEx(const char* str_id, vsonyp0werTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(str_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool vsonyp0wer::TreeNodeEx(const void* ptr_id, vsonyp0werTreeNodeFlags flags, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool is_open = TreeNodeExV(ptr_id, flags, fmt, args);
    va_end(args);
    return is_open;
}

bool vsonyp0wer::TreeNodeExV(const char* str_id, vsonyp0werTreeNodeFlags flags, const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(str_id), flags, g.TempBuffer, label_end);
}

bool vsonyp0wer::TreeNodeExV(const void* ptr_id, vsonyp0werTreeNodeFlags flags, const char* fmt, va_list args)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const char* label_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
    return TreeNodeBehavior(window->GetID(ptr_id), flags, g.TempBuffer, label_end);
}

bool vsonyp0wer::TreeNodeBehaviorIsOpen(vsonyp0werID id, vsonyp0werTreeNodeFlags flags)
{
    if (flags & vsonyp0werTreeNodeFlags_Leaf)
        return true;

    // We only write to the tree stohnly if the user clicks (or explicitly use SetNextTreeNode*** functions)
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    vsonyp0werStohnly* stohnly = window->DC.StateStohnly;

    bool is_open;
    if (g.NextTreeNodeOpenCond != 0)
    {
        if (g.NextTreeNodeOpenCond & vsonyp0werCond_Always)
        {
            is_open = g.NextTreeNodeOpenVal;
            stohnly->SetInt(id, is_open);
        } else
        {
            // We treat vsonyp0werCond_Once and vsonyp0werCond_FirstUseEver the same because tree node state are not saved persistently.
            const int stored_value = stohnly->GetInt(id, -1);
            if (stored_value == -1)
            {
                is_open = g.NextTreeNodeOpenVal;
                stohnly->SetInt(id, is_open);
            } else
            {
                is_open = stored_value != 0;
            }
        }
        g.NextTreeNodeOpenCond = 0;
    } else
    {
        is_open = stohnly->GetInt(id, (flags & vsonyp0werTreeNodeFlags_DefaultOpen) ? 1 : 0) != 0;
    }

    // When logging is enabled, we automatically expand tree nodes (but *NOT* collapsing headers.. seems like sensible behavior).
    // NB- If we are above max depth we still allow manually opened nodes to be logged.
    if (g.LogEnabled && !(flags & vsonyp0werTreeNodeFlags_NoAutoOpenOnLog) && (window->DC.TreeDepth - g.LogDepthRef) < g.LogDepthToExpand)
        is_open = true;

    return is_open;
}

bool vsonyp0wer::TreeNodeBehavior(vsonyp0werID id, vsonyp0werTreeNodeFlags flags, const char* label, const char* label_end)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const bool display_frame = (flags & vsonyp0werTreeNodeFlags_Framed) != 0;
    const ImVec2 padding = (display_frame || (flags & vsonyp0werTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);

    if (!label_end)
        label_end = FindRenderedTextEnd(label);
    const ImVec2 label_size = CalcTextSize(label, label_end, false);

    // We vertically grow up to current line height up the typical widget height.
    const float text_base_offset_y = ImMax(padding.y, window->DC.CurrentLineTextBaseOffset); // Latch before ItemSize changes it
    const float frame_height = ImMax(ImMin(window->DC.CurrentLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
    ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(GetContentRegionMaxScreen().x, window->DC.CursorPos.y + frame_height));
    if (display_frame)
    {
        // Framed header expand a little outside the default padding
        frame_bb.Min.x -= (float)(int)(window->WindowPadding.x * 0.5f) - 1;
        frame_bb.Max.x += (float)(int)(window->WindowPadding.x * 0.5f) - 1;
    }

    const float text_offset_x = (g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2));   // Collapser arrow width + Spacing
    const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
    ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);

    // For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
    // (Ideally we'd want to add a flag for the user to specify if we want the hit test to be done up to the right side of the content or not)
    const ImRect interact_bb = display_frame ? frame_bb : ImRect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + text_width + style.ItemSpacing.x * 2, frame_bb.Max.y);
    bool is_open = TreeNodeBehaviorIsOpen(id, flags);
    bool is_leaf = (flags & vsonyp0werTreeNodeFlags_Leaf) != 0;

    // Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
    // For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
    // This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
    if (is_open && !g.NavIdIsAlive && (flags & vsonyp0werTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & vsonyp0werTreeNodeFlags_NoTreePushOnOpen))
        window->DC.TreeDepthMayJumpToParentOnPop |= (1 << window->DC.TreeDepth);

    bool item_add = ItemAdd(interact_bb, id);
    window->DC.LastItemStatusFlags |= vsonyp0werItemStatusFlags_HasDisplayRect;
    window->DC.LastItemDisplayRect = frame_bb;

    if (!item_add)
    {
        if (is_open && !(flags & vsonyp0werTreeNodeFlags_NoTreePushOnOpen))
            TreePushRawID(id);
        vsonyp0wer_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | (is_leaf ? 0 : vsonyp0werItemStatusFlags_Openable) | (is_open ? vsonyp0werItemStatusFlags_Opened : 0));
        return is_open;
    }

    // Flags that affects opening behavior:
    // - 0 (default) .................... single-click anywhere to open
    // - OpenOnDoubleClick .............. double-click anywhere to open
    // - OpenOnArrow .................... single-click on arrow to open
    // - OpenOnDoubleClick|OpenOnArrow .. single-click on arrow or double-click anywhere to open
    vsonyp0werButtonFlags button_flags = vsonyp0werButtonFlags_NoKeyModifiers;
    if (flags & vsonyp0werTreeNodeFlags_AllowItemOverlap)
        button_flags |= vsonyp0werButtonFlags_AllowItemOverlap;
    if (flags & vsonyp0werTreeNodeFlags_OpenOnDoubleClick)
        button_flags |= vsonyp0werButtonFlags_PressedOnDoubleClick | ((flags & vsonyp0werTreeNodeFlags_OpenOnArrow) ? vsonyp0werButtonFlags_PressedOnClickRelease : 0);
    if (!is_leaf)
        button_flags |= vsonyp0werButtonFlags_PressedOnDragDropHold;

    bool selected = (flags & vsonyp0werTreeNodeFlags_Selected) != 0;
    const bool was_selected = selected;

    bool hovered, held;
    bool pressed = ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
    bool toggled = false;
    if (!is_leaf)
    {
        if (pressed)
        {
            toggled = !(flags & (vsonyp0werTreeNodeFlags_OpenOnArrow | vsonyp0werTreeNodeFlags_OpenOnDoubleClick)) || (g.NavActivateId == id);
            if (flags & vsonyp0werTreeNodeFlags_OpenOnArrow)
                toggled |= IsMouseHoveringRect(interact_bb.Min, ImVec2(interact_bb.Min.x + text_offset_x, interact_bb.Max.y)) && (!g.NavDisableMouseHover);
            if (flags & vsonyp0werTreeNodeFlags_OpenOnDoubleClick)
                toggled |= g.IO.MouseDoubleClicked[0];
            if (g.DragDropActive && is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
                toggled = false;
        }

        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == vsonyp0werDir_Left && is_open)
        {
            toggled = true;
            NavMoveRequestCancel();
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == vsonyp0werDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
        {
            toggled = true;
            NavMoveRequestCancel();
        }

        if (toggled)
        {
            is_open = !is_open;
            window->DC.StateStohnly->SetInt(id, is_open);
        }
    }
    if (flags & vsonyp0werTreeNodeFlags_AllowItemOverlap)
        SetItemAllowOverlap();

    // In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        window->DC.LastItemStatusFlags |= vsonyp0werItemStatusFlags_ToggledSelection;

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? vsonyp0werCol_HeaderActive : hovered ? vsonyp0werCol_HeaderHovered : vsonyp0werCol_Header);
    const ImVec2 text_pos = frame_bb.Min + ImVec2(text_offset_x, text_base_offset_y);
    vsonyp0werNavHighlightFlags nav_highlight_flags = vsonyp0werNavHighlightFlags_TypeThin;
    if (display_frame)
    {
        // Framed type
        RenderFrame(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding);
        RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        RenderArrow(frame_bb.Min + ImVec2(padding.x, text_base_offset_y), is_open ? vsonyp0werDir_Down : vsonyp0werDir_Right, 1.0f);
        if (g.LogEnabled)
        {
            // NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
            const char log_prefix[] = "\n##";
            const char log_suffix[] = "##";
            LogRenderedText(&text_pos, log_prefix, log_prefix + 3);
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
            LogRenderedText(&text_pos, log_suffix, log_suffix + 2);
        } else
        {
            RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
        }
    } else
    {
        // Unframed typed for tree nodes
        if (hovered || selected)
        {
            RenderFrame(frame_bb.Min, frame_bb.Max, col, false);
            RenderNavHighlight(frame_bb, id, nav_highlight_flags);
        }

        if (flags & vsonyp0werTreeNodeFlags_Bullet)
            RenderBullet(frame_bb.Min + ImVec2(text_offset_x * 0.5f, g.FontSize * 0.50f + text_base_offset_y));
        else if (!is_leaf)
            RenderArrow(frame_bb.Min + ImVec2(padding.x, g.FontSize * 0.15f + text_base_offset_y), is_open ? vsonyp0werDir_Down : vsonyp0werDir_Right, 0.70f);
        if (g.LogEnabled)
            LogRenderedText(&text_pos, ">");
        RenderText(text_pos, label, label_end, false);
    }

    if (is_open && !(flags & vsonyp0werTreeNodeFlags_NoTreePushOnOpen))
        TreePushRawID(id);
    vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | (is_leaf ? 0 : vsonyp0werItemStatusFlags_Openable) | (is_open ? vsonyp0werItemStatusFlags_Opened : 0));
    return is_open;
}

void vsonyp0wer::TreePush(const char* str_id)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(str_id ? str_id : "#TreePush");
}

void vsonyp0wer::TreePush(const void* ptr_id)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    PushID(ptr_id ? ptr_id : (const void*)"#TreePush");
}

void vsonyp0wer::TreePushRawID(vsonyp0werID id)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    Indent();
    window->DC.TreeDepth++;
    window->IDStack.push_back(id);
}

void vsonyp0wer::TreePop()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    Unindent();

    window->DC.TreeDepth--;
    if (g.NavMoveDir == vsonyp0werDir_Left && g.NavWindow == window && NavMoveRequestButNoResultYet())
        if (g.NavIdIsAlive && (window->DC.TreeDepthMayJumpToParentOnPop & (1 << window->DC.TreeDepth)))
        {
            SetNavID(window->IDStack.back(), g.NavLayer);
            NavMoveRequestCancel();
        }
    window->DC.TreeDepthMayJumpToParentOnPop &= (1 << window->DC.TreeDepth) - 1;

    IM_ASSERT(window->IDStack.Size > 1); // There should always be 1 element in the IDStack (pushed during window creation). If this triggers you called TreePop/PopID too much.
    PopID();
}

void vsonyp0wer::TreeAdvanceToLabelPos()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.CurrentWindow->DC.CursorPos.x += GetTreeNodeToLabelSpacing();
}

// Horizontal distance preceding label when using TreeNode() or Bullet()
float vsonyp0wer::GetTreeNodeToLabelSpacing()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize + (g.Style.FramePadding.x * 2.0f);
}

void vsonyp0wer::SetNextTreeNodeOpen(bool is_open, vsonyp0werCond cond)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.CurrentWindow->SkipItems)
        return;
    g.NextTreeNodeOpenVal = is_open;
    g.NextTreeNodeOpenCond = cond ? cond : vsonyp0werCond_Always;
}

// CollapsingHeader returns true when opened but do not indent nor push into the ID stack (because of the vsonyp0werTreeNodeFlags_NoTreePushOnOpen flag).
// This is basically the same as calling TreeNodeEx(label, vsonyp0werTreeNodeFlags_CollapsingHeader). You can remove the _NoTreePushOnOpen flag if you want behavior closer to normal TreeNode().
bool vsonyp0wer::CollapsingHeader(const char* label, vsonyp0werTreeNodeFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    return TreeNodeBehavior(window->GetID(label), flags | vsonyp0werTreeNodeFlags_CollapsingHeader, label);
}

bool vsonyp0wer::CollapsingHeader(const char* label, bool* p_open, vsonyp0werTreeNodeFlags flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    if (p_open && !*p_open)
        return false;

    vsonyp0werID id = window->GetID(label);
    bool is_open = TreeNodeBehavior(id, flags | vsonyp0werTreeNodeFlags_CollapsingHeader | (p_open ? vsonyp0werTreeNodeFlags_AllowItemOverlap : 0), label);
    if (p_open)
    {
        // Create a small overlapping close button // FIXME: We can evolve this into user accessible helpers to add extra buttons on title bars, headers, etc.
        vsonyp0werContext& g = *Gvsonyp0wer;
        vsonyp0werItemHoveredDataBackup last_item_backup;
        float button_radius = g.FontSize * 0.5f;
        ImVec2 button_center = ImVec2(ImMin(window->DC.LastItemRect.Max.x, window->ClipRect.Max.x) - g.Style.FramePadding.x - button_radius, window->DC.LastItemRect.GetCenter().y);
        if (CloseButton(window->GetID((void*)((intptr_t)id + 1)), button_center, button_radius))
            * p_open = false;
        last_item_backup.Restore();
    }

    return is_open;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Selectable
//-------------------------------------------------------------------------
// - Selectable()
//-------------------------------------------------------------------------

// Tip: pass a non-visible label (e.g. "##dummy") then you can use the space to draw other text or image.
// But you need to make sure the ID is unique, e.g. enclose calls in PushID/PopID or use ##unique_id.
bool vsonyp0wer::Selectable(const char* label, bool selected, vsonyp0werSelectableFlags flags, const ImVec2 & size_arg)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;

	if ((flags & vsonyp0werSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
		PopClipRect();

	vsonyp0werID id = window->GetID(label);
	ImVec2 label_size = CalcTextSize(label, NULL, true);
	ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrentLineTextBaseOffset;
	ImRect bb_inner(pos, pos + size);
	ItemSize(size);

	// Fill horizontal space.
	ImVec2 window_padding = window->WindowPadding;
	float max_x = (flags & vsonyp0werSelectableFlags_SpanAllColumns) ? GetWindowContentRegionMax().x : GetContentRegionMax().x;
	float w_draw = ImMax(label_size.x, window->Pos.x + max_x - window_padding.x - pos.x);
	ImVec2 size_draw((size_arg.x != 0 && !(flags & vsonyp0werSelectableFlags_DrawFillAvailWidth)) ? size_arg.x : w_draw, size_arg.y != 0.0f ? size_arg.y : size.y);
	ImRect bb(pos, pos + size_draw);
	if (size_arg.x == 0.0f || (flags & vsonyp0werSelectableFlags_DrawFillAvailWidth))
		bb.Max.x += window_padding.x;

	// Selectables are tightly packed together so we extend the box to cover spacing between selectable.
	const float spacing_x = style.ItemSpacing.x;
	const float spacing_y = style.ItemSpacing.y;
	const float spacing_L = (float)(int)(spacing_x * 0.50f);
	const float spacing_U = (float)(int)(spacing_y * 0.50f);
	bb.Min.x -= spacing_L;
	bb.Min.y -= spacing_U;
	bb.Max.x += (spacing_x - spacing_L);
	bb.Max.y += (spacing_y - spacing_U);

	bool item_add;
	if (flags & vsonyp0werSelectableFlags_Disabled)
	{
		vsonyp0werItemFlags backup_item_flags = window->DC.ItemFlags;
		window->DC.ItemFlags |= vsonyp0werItemFlags_Disabled | vsonyp0werItemFlags_NoNavDefaultFocus;
		item_add = ItemAdd(bb, id);
		window->DC.ItemFlags = backup_item_flags;
	}
	else
	{
		item_add = ItemAdd(bb, id);
	}
	if (!item_add)
	{
		if ((flags & vsonyp0werSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
			PushColumnClipRect();
		return false;
	}

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	vsonyp0werButtonFlags button_flags = 0;
	if (flags & vsonyp0werSelectableFlags_NoHoldingActiveID) button_flags |= vsonyp0werButtonFlags_NoHoldingActiveID;
	if (flags & vsonyp0werSelectableFlags_PressedOnClick) button_flags |= vsonyp0werButtonFlags_PressedOnClick;
	if (flags & vsonyp0werSelectableFlags_PressedOnRelease) button_flags |= vsonyp0werButtonFlags_PressedOnRelease;
	if (flags & vsonyp0werSelectableFlags_Disabled) button_flags |= vsonyp0werButtonFlags_Disabled;
	if (flags & vsonyp0werSelectableFlags_AllowDoubleClick) button_flags |= vsonyp0werButtonFlags_PressedOnClickRelease | vsonyp0werButtonFlags_PressedOnDoubleClick;
	if (flags & vsonyp0werSelectableFlags_AllowItemOverlap) button_flags |= vsonyp0werButtonFlags_AllowItemOverlap;

	if (flags & vsonyp0werSelectableFlags_Disabled)
		selected = false;

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
	// Hovering selectable with mouse updates NavId accordingly so navigation can be resumed with gamepad/keyboard (this doesn't happen on most widgets)
	if (pressed || hovered)
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			g.NavDisableHighlight = true;
			SetNavID(id, window->DC.NavLayerCurrent);
		}
	if (pressed)
		MarkItemEdited(id);

	if (flags & vsonyp0werSelectableFlags_AllowItemOverlap)
		SetItemAllowOverlap();

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		window->DC.LastItemStatusFlags |= vsonyp0werItemStatusFlags_ToggledSelection;

	// Render
	if (hovered)
		window->DrawList->AddRectFilled(bb.Min, bb.Max + ImVec2(0, 1), ImColor(24, 24, 24, 255));

	if ((flags & vsonyp0werSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) {

		PushColumnClipRect();
		bb.Max.x -= (GetContentRegionMax().x - max_x);
	}

	if (flags & vsonyp0werSelectableFlags_Disabled)
		PushStyleColor(vsonyp0werCol_Text, g.Style.Colors[vsonyp0werCol_TextDisabled]);

	if (selected || hovered)
		PushColor(vsonyp0werCol_Text, vsonyp0werCol_MenuTheme, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	else
		PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextDisabled, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

	if (selected || hovered) {

		PopFont();
		PushFont(globals::boldMenuFont);

		RenderTextClipped(bb_inner.Min + ImVec2(4, 0), bb.Max, label, NULL, &label_size, ImVec2(0.0f, 0.0f));

		PopFont();
		PushFont(globals::menuFont);
	}
	else
		RenderTextClipped(bb_inner.Min + ImVec2(4, 0), bb.Max, label, NULL, &label_size, ImVec2(0.0f, 0.0f));	

	PopStyleColor();

	// Automatically close popups
	if (pressed && (window->Flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & vsonyp0werItemFlags_SelectableDontClosePopup))
		CloseCurrentPopup();

	vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
	return pressed;
}

bool vsonyp0wer::Selectable(const char* label, bool* p_selected, vsonyp0werSelectableFlags flags, const ImVec2 & size_arg)
{
    if (Selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: ListBox
//-------------------------------------------------------------------------
// - ListBox()
// - ListBoxHeader()
// - ListBoxFooter()
//-------------------------------------------------------------------------
// FIXME: This is an old API. We should redesign some of it, rename ListBoxHeader->BeginListBox, ListBoxFooter->EndListBox
// and promote using them over existing ListBox() functions, similarly to change with combo boxes.
//-------------------------------------------------------------------------

// FIXME: In principle this function should be called BeginListBox(). We should rename it after re-evaluating if we want to keep the same signature.
// Helper to calculate the size of a listbox and display a label on the right.
// Tip: To have a list filling the entire window width, PushItemWidth(-1) and pass an non-visible label e.g. "##empty"
bool vsonyp0wer::ListBoxHeader(const char* label, const ImVec2 & size_arg)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const vsonyp0werStyle& style = GetStyle();
    const vsonyp0werID id = GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    // Size default to hold ~7 items. Fractional number of items helps seeing that we can scroll down/up without looking at scrollbar.
    ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), GetTextLineHeightWithSpacing() * 7.4f + style.ItemSpacing.y);
    ImVec2 frame_size = ImVec2(size.x, ImMax(size.y, label_size.y));
    ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
    window->DC.LastItemRect = bb; // Forward stohnly for ListBoxFooter.. dodgy.

    if (!IsRectVisible(bb.Min, bb.Max))
    {
        ItemSize(bb.GetSize(), style.FramePadding.y);
        ItemAdd(bb, 0, &frame_bb);
        return false;
    }

    BeginGroup();
    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    BeginChildFrame(id, frame_bb.GetSize());
    return true;
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
bool vsonyp0wer::ListBoxHeader(const char* label, int items_count, int height_in_items)
{
    // Size default to hold ~7.25 items.
    // We add +25% worth of item height to allow the user to see at a glance if there are more items up/down, without looking at the scrollbar.
    // We don't add this extra bit if items_count <= height_in_items. It is slightly dodgy, because it means a dynamic list of items will make the widget resize occasionally when it crosses that size.
    // I am expecting that someone will come and complain about this behavior in a remote future, then we can advise on a better solution.
    if (height_in_items < 0)
        height_in_items = ImMin(items_count, 7);
    const vsonyp0werStyle& style = GetStyle();
    float height_in_items_f = (height_in_items < items_count) ? (height_in_items + 0.25f) : (height_in_items + 0.00f);

    // We include ItemSpacing.y so that a list sized for the exact number of items doesn't make a scrollbar appears. We could also enforce that by passing a flag to BeginChild().
    ImVec2 size;
    size.x = 0.0f;
    size.y = GetTextLineHeightWithSpacing() * height_in_items_f + style.FramePadding.y * 2.0f;
    return ListBoxHeader(label, size);
}

// FIXME: In principle this function should be called EndListBox(). We should rename it after re-evaluating if we want to keep the same signature.
void vsonyp0wer::ListBoxFooter()
{
    vsonyp0werWindow* parent_window = GetCurrentWindow()->ParentWindow;
    const ImRect bb = parent_window->DC.LastItemRect;
    const vsonyp0werStyle& style = GetStyle();

    EndChildFrame();

    // Redeclare item size so that it includes the label (we have stored the full size in LastItemRect)
    // We call SameLine() to restore DC.CurrentLine* data
    SameLine();
    parent_window->DC.CursorPos = bb.Min;
    ItemSize(bb, style.FramePadding.y);
    EndGroup();
}

bool vsonyp0wer::ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_items)
{
    const bool value_changed = ListBox(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_items);
    return value_changed;
}

bool vsonyp0wer::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
    if (!ListBoxHeader(label, items_count, height_in_items))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    vsonyp0werContext& g = *Gvsonyp0wer;
    bool value_changed = false;
    vsonyp0werListClipper clipper(items_count, GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if (!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";

            PushID(i);
            if (Selectable(item_text, item_selected))
            {
                *current_item = i;
                value_changed = true;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }
    ListBoxFooter();
    if (value_changed)
        MarkItemEdited(g.CurrentWindow->DC.LastItemId);

    return value_changed;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: PlotLines, PlotHistogram
//-------------------------------------------------------------------------
// - PlotEx() [Internal]
// - PlotLines()
// - PlotHistogram()
//-------------------------------------------------------------------------

void vsonyp0wer::PlotEx(vsonyp0werPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const vsonyp0werID id = window->GetID(label);

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
    const bool hovered = ItemHoverable(frame_bb, id);

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

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(vsonyp0werCol_FrameBg), true, style.FrameRounding);

    const int values_count_min = (plot_type == vsonyp0werPlotType_Lines) ? 2 : 1;
    if (values_count >= values_count_min)
    {
        int res_w = ImMin((int)frame_size.x, values_count) + ((plot_type == vsonyp0werPlotType_Lines) ? -1 : 0);
        int item_count = values_count + ((plot_type == vsonyp0werPlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered && inner_bb.Contains(g.IO.MousePos))
        {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < values_count);

            const float v0 = values_getter(data, (v_idx + values_offset) % values_count);
            const float v1 = values_getter(data, (v_idx + 1 + values_offset) % values_count);
            if (plot_type == vsonyp0werPlotType_Lines)
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx + 1, v1);
            else if (plot_type == vsonyp0werPlotType_Histogram)
                SetTooltip("%d: %8.4g", v_idx, v0);
            v_hovered = v_idx;
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (scale_min == scale_max) ? 0.0f : (1.0f / (scale_max - scale_min));

        float v0 = values_getter(data, (0 + values_offset) % values_count);
        float t0 = 0.0f;
        ImVec2 tp0 = ImVec2(t0, 1.0f - ImSaturate((v0 - scale_min) * inv_scale));                       // Point in the normalized space of our target rectangle
        float histogram_zero_line_t = (scale_min * scale_max < 0.0f) ? (-scale_min * inv_scale) : (scale_min < 0.0f ? 0.0f : 1.0f);   // Where does the zero line stands

        const ImU32 col_base = GetColorU32((plot_type == vsonyp0werPlotType_Lines) ? vsonyp0werCol_PlotLines : vsonyp0werCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == vsonyp0werPlotType_Lines) ? vsonyp0werCol_PlotLinesHovered : vsonyp0werCol_PlotHistogramHovered);

        for (int n = 0; n < res_w; n++)
        {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < values_count);
            const float v1 = values_getter(data, (v1_idx + values_offset + 1) % values_count);
            const ImVec2 tp1 = ImVec2(t1, 1.0f - ImSaturate((v1 - scale_min) * inv_scale));

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == vsonyp0werPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            if (plot_type == vsonyp0werPlotType_Lines)
            {
                window->DrawList->AddLine(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            } else if (plot_type == vsonyp0werPlotType_Histogram)
            {
                if (pos1.x >= pos0.x + 2.0f)
                    pos1.x -= 1.0f;
                window->DrawList->AddRectFilled(pos0, pos1, v_hovered == v1_idx ? col_hovered : col_base);
            }

            t0 = t1;
            tp0 = tp1;
        }
    }

    // Text overlay
    if (overlay_text)
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, overlay_text, NULL, NULL, ImVec2(0.5f, 0.0f));

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
}

struct vsonyp0werPlotArrayGetterData
{
    const float* Values;
    int Stride;

    vsonyp0werPlotArrayGetterData(const float* values, int stride) { Values = values; Stride = stride; }
};

static float Plot_ArrayGetter(void* data, int idx)
{
    vsonyp0werPlotArrayGetterData* plot_data = (vsonyp0werPlotArrayGetterData*)data;
    const float v = *(const float*)(const void*)((const unsigned char*)plot_data->Values + (size_t)idx * plot_data->Stride);
    return v;
}

void vsonyp0wer::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    vsonyp0werPlotArrayGetterData data(values, stride);
    PlotEx(vsonyp0werPlotType_Lines, label, &Plot_ArrayGetter, (void*)& data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void vsonyp0wer::PlotLines(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(vsonyp0werPlotType_Lines, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void vsonyp0wer::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
{
    vsonyp0werPlotArrayGetterData data(values, stride);
    PlotEx(vsonyp0werPlotType_Histogram, label, &Plot_ArrayGetter, (void*)& data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

void vsonyp0wer::PlotHistogram(const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
{
    PlotEx(vsonyp0werPlotType_Histogram, label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: Value helpers
// Those is not very useful, legacy API.
//-------------------------------------------------------------------------
// - Value()
//-------------------------------------------------------------------------

void vsonyp0wer::Value(const char* prefix, bool b)
{
    Text("%s: %s", prefix, (b ? "true" : "false"));
}

void vsonyp0wer::Value(const char* prefix, int v)
{
    Text("%s: %d", prefix, v);
}

void vsonyp0wer::Value(const char* prefix, unsigned int v)
{
    Text("%s: %d", prefix, v);
}

void vsonyp0wer::Value(const char* prefix, float v, const char* float_format)
{
    if (float_format)
    {
        char fmt[64];
        ImFormatString(fmt, IM_ARRAYSIZE(fmt), "%%s: %s", float_format);
        Text(fmt, prefix, v);
    } else
    {
        Text("%s: %.3f", prefix, v);
    }
}

//-------------------------------------------------------------------------
// [SECTION] MenuItem, BeginMenu, EndMenu, etc.
//-------------------------------------------------------------------------
// - vsonyp0werMenuColumns [Internal]
// - BeginMainMenuBar()
// - EndMainMenuBar()
// - BeginMenuBar()
// - EndMenuBar()
// - BeginMenu()
// - EndMenu()
// - MenuItem()
//-------------------------------------------------------------------------

// Helpers for internal use
vsonyp0werMenuColumns::vsonyp0werMenuColumns()
{
    Spacing = Width = NextWidth = 0.0f;
    memset(Pos, 0, sizeof(Pos));
    memset(NextWidths, 0, sizeof(NextWidths));
}

void vsonyp0werMenuColumns::Update(int count, float spacing, bool clear)
{
    IM_ASSERT(count == IM_ARRAYSIZE(Pos));
    IM_UNUSED(count);
    Width = NextWidth = 0.0f;
    Spacing = spacing;
    if (clear)
        memset(NextWidths, 0, sizeof(NextWidths));
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
    {
        if (i > 0 && NextWidths[i] > 0.0f)
            Width += Spacing;
        Pos[i] = (float)(int)Width;
        Width += NextWidths[i];
        NextWidths[i] = 0.0f;
    }
}

float vsonyp0werMenuColumns::DeclColumns(float w0, float w1, float w2) // not using va_arg because they promote float to double
{
    NextWidth = 0.0f;
    NextWidths[0] = ImMax(NextWidths[0], w0);
    NextWidths[1] = ImMax(NextWidths[1], w1);
    NextWidths[2] = ImMax(NextWidths[2], w2);
    for (int i = 0; i < IM_ARRAYSIZE(Pos); i++)
        NextWidth += NextWidths[i] + ((i > 0 && NextWidths[i] > 0.0f) ? Spacing : 0.0f);
    return ImMax(Width, NextWidth);
}

float vsonyp0werMenuColumns::CalcExtraSpace(float avail_w)
{
    return ImMax(0.0f, avail_w - Width);
}

// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
bool vsonyp0wer::BeginMainMenuBar()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
    SetNextWindowPos(ImVec2(0.0f, 0.0f));
    SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y));
    PushStyleVar(vsonyp0werStyleVar_WindowRounding, 0.0f);
    PushStyleVar(vsonyp0werStyleVar_WindowMinSize, ImVec2(0, 0));
    vsonyp0werWindowFlags window_flags = vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoScrollbar | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_MenuBar;
    bool is_open = Begin("##MainMenuBar", NULL, window_flags) && BeginMenuBar();
    PopStyleVar(2);
    g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
    if (!is_open)
    {
        End();
        return false;
    }
    return true; //-V1020
}

void vsonyp0wer::EndMainMenuBar()
{
    EndMenuBar();

    // When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.CurrentWindow == g.NavWindow && g.NavLayer == 0)
        FocusPreviousWindowIgnoringOne(g.NavWindow);

    End();
}

bool vsonyp0wer::BeginMenuBar()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
    if (!(window->Flags & vsonyp0werWindowFlags_MenuBar))
        return false;

    IM_ASSERT(!window->DC.MenuBarAppending);
    BeginGroup(); // Backup position on layer 0
    PushID("##menubar");

    // We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
    // We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
    ImRect bar_rect = window->MenuBarRect();
    ImRect clip_rect(ImFloor(bar_rect.Min.x + 0.5f), ImFloor(bar_rect.Min.y + window->WindowBorderSize + 0.5f), ImFloor(ImMax(bar_rect.Min.x, bar_rect.Max.x - window->WindowRounding) + 0.5f), ImFloor(bar_rect.Max.y + 0.5f));
    clip_rect.ClipWith(window->OuterRectClipped);
    PushClipRect(clip_rect.Min, clip_rect.Max, false);

    window->DC.CursorPos = ImVec2(bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
    window->DC.LayoutType = vsonyp0werLayoutType_Horizontal;
    window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);
    window->DC.MenuBarAppending = true;
    AlignTextToFramePadding();
    return true;
}

void vsonyp0wer::EndMenuBar()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
    if (NavMoveRequestButNoResultYet() && (g.NavMoveDir == vsonyp0werDir_Left || g.NavMoveDir == vsonyp0werDir_Right) && (g.NavWindow->Flags & vsonyp0werWindowFlags_ChildMenu))
    {
        vsonyp0werWindow* nav_earliest_child = g.NavWindow;
        while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & vsonyp0werWindowFlags_ChildMenu))
            nav_earliest_child = nav_earliest_child->ParentWindow;
        if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == vsonyp0werLayoutType_Horizontal && g.NavMoveRequestForward == vsonyp0werNavForward_None)
        {
            // To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
            // This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth the hassle/cost)
            IM_ASSERT(window->DC.NavLayerActiveMaskNext & 0x02); // Sanity check
            FocusWindow(window);
            SetNavIDWithRectRel(window->NavLastIds[1], 1, window->NavRectRel[1]);
            g.NavLayer = vsonyp0werNavLayer_Menu;
            g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
            g.NavMoveRequestForward = vsonyp0werNavForward_ForwardQueued;
            NavMoveRequestCancel();
        }
    }

    IM_ASSERT(window->Flags & vsonyp0werWindowFlags_MenuBar);
    IM_ASSERT(window->DC.MenuBarAppending);
    PopClipRect();
    PopID();
    window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->MenuBarRect().Min.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
    window->DC.GroupStack.back().AdvanceCursor = false;
    EndGroup(); // Restore position on layer 0
    window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
    window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
    window->DC.MenuBarAppending = false;
}

bool vsonyp0wer::BeginMenu(const char* label, bool enabled)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    const vsonyp0werID id = window->GetID(label);

    ImVec2 label_size = CalcTextSize(label, NULL, true);

    bool pressed;
    bool menu_is_open = IsPopupOpen(id);
    bool menuset_is_open = !(window->Flags & vsonyp0werWindowFlags_Popup) && (g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].OpenParentId == window->IDStack.back());
    vsonyp0werWindow * backed_nav_window = g.NavWindow;
    if (menuset_is_open)
        g.NavWindow = window;  // Odd hack to allow hovering across menus of a same menu-set (otherwise we wouldn't be able to hover parent)

    // The reference position stored in popup_pos will be used by Begin() to find a suitable position for the child menu,
    // However the final position is going to be different! It is choosen by FindBestWindowPosForPopup().
    // e.g. Menus tend to overlap each other horizontally to amplify relative Z-ordering.
    ImVec2 popup_pos, pos = window->DC.CursorPos;
    if (window->DC.LayoutType == vsonyp0werLayoutType_Horizontal)
    {
        // Menu inside an horizontal menu bar
        // Selectable extend their highlight by half ItemSpacing in each direction.
        // For ChildMenu, the popup position will be overwritten by the call to FindBestWindowPosForPopup() in Begin()
        popup_pos = ImVec2(pos.x - 1.0f - (float)(int)(style.ItemSpacing.x * 0.5f), pos.y - style.FramePadding.y + window->MenuBarHeight());
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
        PushStyleVar(vsonyp0werStyleVar_ItemSpacing, style.ItemSpacing * 2.0f);
        float w = label_size.x;
        pressed = Selectable(label, menu_is_open, vsonyp0werSelectableFlags_NoHoldingActiveID | vsonyp0werSelectableFlags_PressedOnClick | vsonyp0werSelectableFlags_DontClosePopups | (!enabled ? vsonyp0werSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        // Menu inside a menu
        popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
        float w = window->MenuColumns.DeclColumns(label_size.x, 0.0f, (float)(int)(g.FontSize * 1.20f)); // Feedback to next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - w);
        pressed = Selectable(label, menu_is_open, vsonyp0werSelectableFlags_NoHoldingActiveID | vsonyp0werSelectableFlags_PressedOnClick | vsonyp0werSelectableFlags_DontClosePopups | vsonyp0werSelectableFlags_DrawFillAvailWidth | (!enabled ? vsonyp0werSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
        if (!enabled) PushStyleColor(vsonyp0werCol_Text, g.Style.Colors[vsonyp0werCol_TextDisabled]);
        RenderArrow(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.30f, 0.0f), vsonyp0werDir_Right);
        if (!enabled) PopStyleColor();
    }

    const bool hovered = enabled && ItemHoverable(window->DC.LastItemRect, id);
    if (menuset_is_open)
        g.NavWindow = backed_nav_window;

    bool want_open = false, want_close = false;
    if (window->DC.LayoutType == vsonyp0werLayoutType_Vertical) // (window->Flags & (vsonyp0werWindowFlags_Popup|vsonyp0werWindowFlags_ChildMenu))
    {
        // Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers, so menus feels more reactive.
        bool moving_within_opened_triangle = false;
        if (g.HoveredWindow == window && g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].ParentWindow == window && !(window->Flags & vsonyp0werWindowFlags_MenuBar))
        {
            if (vsonyp0werWindow * next_window = g.OpenPopupStack[g.BeginPopupStack.Size].Window)
            {
                // FIXME-DPI: Values should be derived from a master "scale" factor.
                ImRect next_window_rect = next_window->Rect();
                ImVec2 ta = g.IO.MousePos - g.IO.MouseDelta;
                ImVec2 tb = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetTL() : next_window_rect.GetTR();
                ImVec2 tc = (window->Pos.x < next_window->Pos.x) ? next_window_rect.GetBL() : next_window_rect.GetBR();
                float extra = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, 5.0f, 30.0f); // add a bit of extra slack.
                ta.x += (window->Pos.x < next_window->Pos.x) ? -0.5f : +0.5f;    // to avoid numerical issues
                tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.0f);             // triangle is maximum 200 high to limit the slope and the bias toward large sub-menus // FIXME: Multiply by fb_scale?
                tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.0f);
                moving_within_opened_triangle = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);
                //window->DrawList->PushClipRectFullScreen(); window->DrawList->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? IM_COL32(0,128,0,128) : IM_COL32(128,0,0,128)); window->DrawList->PopClipRect(); // Debug
            }
        }

        want_close = (menu_is_open && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !moving_within_opened_triangle);
        want_open = (!menu_is_open && hovered && !moving_within_opened_triangle) || (!menu_is_open && hovered && pressed);

        if (g.NavActivateId == id)
        {
            want_close = menu_is_open;
            want_open = !menu_is_open;
        }
        if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == vsonyp0werDir_Right) // Nav-Right to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    } else
    {
        // Menu bar
        if (menu_is_open && pressed && menuset_is_open) // Click an open menu again to close it
        {
            want_close = true;
            want_open = menu_is_open = false;
        } else if (pressed || (hovered && menuset_is_open && !menu_is_open)) // First click to open, then hover to open others
        {
            want_open = true;
        } else if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == vsonyp0werDir_Down) // Nav-Down to open
        {
            want_open = true;
            NavMoveRequestCancel();
        }
    }

    if (!enabled) // explicitly close if an open menu becomes disabled, facilitate users code a lot in pattern such as 'if (BeginMenu("options", has_object)) { ..use object.. }'
        want_close = true;
    if (want_close && IsPopupOpen(id))
        ClosePopupToLevel(g.BeginPopupStack.Size, true);

    vsonyp0wer_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | vsonyp0werItemStatusFlags_Openable | (menu_is_open ? vsonyp0werItemStatusFlags_Opened : 0));

    if (!menu_is_open && want_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size)
    {
        // Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
        OpenPopup(label);
        return false;
    }

    menu_is_open |= want_open;
    if (want_open)
        OpenPopup(label);

    if (menu_is_open)
    {
        // Sub-menus are ChildWindow so that mouse can be hovering across them (otherwise top-most popup menu would steal focus and not allow hovering on parent menu)
        SetNextWindowPos(popup_pos, vsonyp0werCond_Always);
        vsonyp0werWindowFlags flags = vsonyp0werWindowFlags_ChildMenu | vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_NoNavFocus;
        if (window->Flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_ChildMenu))
            flags |= vsonyp0werWindowFlags_ChildWindow;
        menu_is_open = BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
    }

    return menu_is_open;
}

void vsonyp0wer::EndMenu()
{
    // Nav: When a left move request _within our child menu_ failed, close ourselves (the _parent_ menu).
    // A menu doesn't close itself because EndMenuBar() wants the catch the last Left<>Right inputs.
    // However, it means that with the current code, a BeginMenu() from outside another menu or a menu-bar won't be closable with the Left direction.
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (g.NavWindow && g.NavWindow->ParentWindow == window && g.NavMoveDir == vsonyp0werDir_Left && NavMoveRequestButNoResultYet() && window->DC.LayoutType == vsonyp0werLayoutType_Vertical)
    {
        ClosePopupToLevel(g.BeginPopupStack.Size, true);
        NavMoveRequestCancel();
    }

    EndPopup();
}

bool vsonyp0wer::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werStyle& style = g.Style;
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 label_size = CalcTextSize(label, NULL, true);

    vsonyp0werSelectableFlags flags = vsonyp0werSelectableFlags_PressedOnRelease | (enabled ? 0 : vsonyp0werSelectableFlags_Disabled);
    bool pressed;
    if (window->DC.LayoutType == vsonyp0werLayoutType_Horizontal)
    {
        // Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
        // Note that in this situation we render neither the shortcut neither the selected tick mark
        float w = label_size.x;
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
        PushStyleVar(vsonyp0werStyleVar_ItemSpacing, style.ItemSpacing * 2.0f);
        pressed = Selectable(label, false, flags, ImVec2(w, 0.0f));
        PopStyleVar();
        window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
    } else
    {
        ImVec2 shortcut_size = shortcut ? CalcTextSize(shortcut, NULL) : ImVec2(0.0f, 0.0f);
        float w = window->MenuColumns.DeclColumns(label_size.x, shortcut_size.x, (float)(int)(g.FontSize * 1.20f)); // Feedback for next frame
        float extra_w = ImMax(0.0f, GetContentRegionAvail().x - w);
        pressed = Selectable(label, false, flags | vsonyp0werSelectableFlags_DrawFillAvailWidth, ImVec2(w, 0.0f));
        if (shortcut_size.x > 0.0f)
        {
            PushStyleColor(vsonyp0werCol_Text, g.Style.Colors[vsonyp0werCol_TextDisabled]);
            RenderText(pos + ImVec2(window->MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
            PopStyleColor();
        }
        if (selected)
            RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.40f, g.FontSize * 0.134f * 0.5f), GetColorU32(enabled ? vsonyp0werCol_Text : vsonyp0werCol_TextDisabled), g.FontSize * 0.866f);
    }

    vsonyp0wer_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | vsonyp0werItemStatusFlags_Checkable | (selected ? vsonyp0werItemStatusFlags_Checked : 0));
    return pressed;
}

bool vsonyp0wer::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
{
    if (MenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
    {
        if (p_selected)
            * p_selected = !*p_selected;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabBar, EndTabBar, etc.
//-------------------------------------------------------------------------
// [BETA API] API may evolve! This code has been extracted out of the Docking branch,
// and some of the construct which are not used in Master may be left here to facilitate merging.
//-------------------------------------------------------------------------
// - BeginTabBar()
// - BeginTabBarEx() [Internal]
// - EndTabBar()
// - TabBarLayout() [Internal]
// - TabBarCalcTabID() [Internal]
// - TabBarCalcMaxTabWidth() [Internal]
// - TabBarFindTabById() [Internal]
// - TabBarRemoveTab() [Internal]
// - TabBarCloseTab() [Internal]
// - TabBarScrollClamp()v
// - TabBarScrollToTab() [Internal]
// - TabBarQueueChangeTabOrder() [Internal]
// - TabBarScrollingButtons() [Internal]
// - TabBarTabListPopupButton() [Internal]
//-------------------------------------------------------------------------

namespace vsonyp0wer
{
    static void             TabBarLayout(vsonyp0werTabBar* tab_bar);
    static ImU32            TabBarCalcTabID(vsonyp0werTabBar* tab_bar, const char* label);
    static float            TabBarCalcMaxTabWidth();
    static float            TabBarScrollClamp(vsonyp0werTabBar* tab_bar, float scrolling);
    static void             TabBarScrollToTab(vsonyp0werTabBar* tab_bar, vsonyp0werTabItem* tab);
    static vsonyp0werTabItem* TabBarScrollingButtons(vsonyp0werTabBar* tab_bar);
    static vsonyp0werTabItem* TabBarTabListPopupButton(vsonyp0werTabBar* tab_bar);
}

vsonyp0werTabBar::vsonyp0werTabBar()
{
    ID = 0;
    SelectedTabId = NextSelectedTabId = VisibleTabId = 0;
    CurrFrameVisible = PrevFrameVisible = -1;
    ContentsHeight = 0.0f;
    OffsetMax = OffsetNextTab = 0.0f;
    ScrollingAnim = ScrollingTarget = ScrollingTargetDistToVisibility = ScrollingSpeed = 0.0f;
    Flags = vsonyp0werTabBarFlags_None;
    ReorderRequestTabId = 0;
    ReorderRequestDir = 0;
    WantLayout = VisibleTabWasSubmitted = false;
    LastTabItemIdx = -1;
}

static int vsonyp0wer_CDECL TabItemComparerByVisibleOffset(const void* lhs, const void* rhs)
{
    const vsonyp0werTabItem* a = (const vsonyp0werTabItem*)lhs;
    const vsonyp0werTabItem* b = (const vsonyp0werTabItem*)rhs;
    return (int)(a->Offset - b->Offset);
}

static int vsonyp0wer_CDECL TabBarSortItemComparer(const void* lhs, const void* rhs)
{
    const vsonyp0werTabBarSortItem* a = (const vsonyp0werTabBarSortItem*)lhs;
    const vsonyp0werTabBarSortItem* b = (const vsonyp0werTabBarSortItem*)rhs;
    if (int d = (int)(b->Width - a->Width))
        return d;
    return (b->Index - a->Index);
}

static vsonyp0werTabBar * GetTabBarFromTabBarRef(const vsonyp0werTabBarRef & ref)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return ref.Ptr ? ref.Ptr : g.TabBars.GetByIndex(ref.IndexInMainPool);
}

static vsonyp0werTabBarRef GetTabBarRefFromTabBar(vsonyp0werTabBar * tab_bar)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.TabBars.Contains(tab_bar))
        return vsonyp0werTabBarRef(g.TabBars.GetIndex(tab_bar));
    return vsonyp0werTabBarRef(tab_bar);
}

bool    vsonyp0wer::BeginTabBar(const char* str_id, vsonyp0werTabBarFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    vsonyp0werID id = window->GetID(str_id);
    vsonyp0werTabBar* tab_bar = g.TabBars.GetOrAddByKey(id);
    ImRect tab_bar_bb = ImRect(window->DC.CursorPos.x, window->DC.CursorPos.y, window->InnerClipRect.Max.x, window->DC.CursorPos.y + g.FontSize + g.Style.FramePadding.y * 2);
    tab_bar->ID = id;
    return BeginTabBarEx(tab_bar, tab_bar_bb, flags | vsonyp0werTabBarFlags_IsFocused);
}

bool    vsonyp0wer::BeginTabBarEx(vsonyp0werTabBar * tab_bar, const ImRect & tab_bar_bb, vsonyp0werTabBarFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    if ((flags & vsonyp0werTabBarFlags_DockNode) == 0)
        window->IDStack.push_back(tab_bar->ID);

    // Add to stack
    g.CurrentTabBarStack.push_back(GetTabBarRefFromTabBar(tab_bar));
    g.CurrentTabBar = tab_bar;

    if (tab_bar->CurrFrameVisible == g.FrameCount)
    {
        //vsonyp0wer_DEBUG_LOG("BeginTabBarEx already called this frame\n", g.FrameCount);
        IM_ASSERT(0);
        return true;
    }

    // When toggling back from ordered to manually-reorderable, shuffle tabs to enforce the last visible order.
    // Otherwise, the most recently inserted tabs would move at the end of visible list which can be a little too confusing or magic for the user.
    if ((flags & vsonyp0werTabBarFlags_Reorderable) && !(tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable) && tab_bar->Tabs.Size > 1 && tab_bar->PrevFrameVisible != -1)
        ImQsort(tab_bar->Tabs.Data, tab_bar->Tabs.Size, sizeof(vsonyp0werTabItem), TabItemComparerByVisibleOffset);

    // Flags
    if ((flags & vsonyp0werTabBarFlags_FittingPolicyMask_) == 0)
        flags |= vsonyp0werTabBarFlags_FittingPolicyDefault_;

    tab_bar->Flags = flags;
    tab_bar->BarRect = tab_bar_bb;
    tab_bar->WantLayout = true; // Layout will be done on the first call to ItemTab()
    tab_bar->PrevFrameVisible = tab_bar->CurrFrameVisible;
    tab_bar->CurrFrameVisible = g.FrameCount;
    tab_bar->FramePadding = g.Style.FramePadding;

    // Layout
    ItemSize(ImVec2(tab_bar->OffsetMax, tab_bar->BarRect.GetHeight()));
    window->DC.CursorPos.x = tab_bar->BarRect.Min.x;

    // Draw separator
    const ImU32 col = GetColorU32((flags & vsonyp0werTabBarFlags_IsFocused) ? vsonyp0werCol_TabActive : vsonyp0werCol_Tab);
    const float y = tab_bar->BarRect.Max.y - 1.0f;
    {
        const float separator_min_x = tab_bar->BarRect.Min.x - window->WindowPadding.x;
        const float separator_max_x = tab_bar->BarRect.Max.x + window->WindowPadding.x;
        window->DrawList->AddLine(ImVec2(separator_min_x, y), ImVec2(separator_max_x, y), col, 1.0f);
    }
    return true;
}

void    vsonyp0wer::EndTabBar()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    vsonyp0werTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar != NULL && "Mismatched BeginTabBar()/EndTabBar()!");
        return; // FIXME-ERRORHANDLING
    }
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    // Restore the last visible height if no tab is visible, this reduce vertical flicker/movement when a tabs gets removed without calling SetTabItemClosed().
    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    if (tab_bar->VisibleTabWasSubmitted || tab_bar->VisibleTabId == 0 || tab_bar_appearing)
        tab_bar->ContentsHeight = ImMax(window->DC.CursorPos.y - tab_bar->BarRect.Max.y, 0.0f);
    else
        window->DC.CursorPos.y = tab_bar->BarRect.Max.y + tab_bar->ContentsHeight;

    if ((tab_bar->Flags & vsonyp0werTabBarFlags_DockNode) == 0)
        PopID();

    g.CurrentTabBarStack.pop_back();
    g.CurrentTabBar = g.CurrentTabBarStack.empty() ? NULL : GetTabBarFromTabBarRef(g.CurrentTabBarStack.back());
}

// This is called only once a frame before by the first call to ItemTab()
// The reason we're not calling it in BeginTabBar() is to leave a chance to the user to call the SetTabItemClosed() functions.
static void vsonyp0wer::TabBarLayout(vsonyp0werTabBar * tab_bar)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    tab_bar->WantLayout = false;

    // Garbage collect
    int tab_dst_n = 0;
    for (int tab_src_n = 0; tab_src_n < tab_bar->Tabs.Size; tab_src_n++)
    {
        vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_src_n];
        if (tab->LastFrameVisible < tab_bar->PrevFrameVisible)
        {
            if (tab->ID == tab_bar->SelectedTabId)
                tab_bar->SelectedTabId = 0;
            continue;
        }
        if (tab_dst_n != tab_src_n)
            tab_bar->Tabs[tab_dst_n] = tab_bar->Tabs[tab_src_n];
        tab_dst_n++;
    }
    if (tab_bar->Tabs.Size != tab_dst_n)
        tab_bar->Tabs.resize(tab_dst_n);

    // Setup next selected tab
    vsonyp0werID scroll_track_selected_tab_id = 0;
    if (tab_bar->NextSelectedTabId)
    {
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId;
        tab_bar->NextSelectedTabId = 0;
        scroll_track_selected_tab_id = tab_bar->SelectedTabId;
    }

    // Process order change request (we could probably process it when requested but it's just saner to do it in a single spot).
    if (tab_bar->ReorderRequestTabId != 0)
    {
        if (vsonyp0werTabItem * tab1 = TabBarFindTabByID(tab_bar, tab_bar->ReorderRequestTabId))
        {
            //IM_ASSERT(tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable); // <- this may happen when using debug tools
            int tab2_order = tab_bar->GetTabOrder(tab1) + tab_bar->ReorderRequestDir;
            if (tab2_order >= 0 && tab2_order < tab_bar->Tabs.Size)
            {
                vsonyp0werTabItem* tab2 = &tab_bar->Tabs[tab2_order];
                vsonyp0werTabItem item_tmp = *tab1;
                *tab1 = *tab2;
                *tab2 = item_tmp;
                if (tab2->ID == tab_bar->SelectedTabId)
                    scroll_track_selected_tab_id = tab2->ID;
                tab1 = tab2 = NULL;
            }
            if (tab_bar->Flags & vsonyp0werTabBarFlags_SaveSettings)
                MarkIniSettingsDirty();
        }
        tab_bar->ReorderRequestTabId = 0;
    }

    // Tab List Popup (will alter tab_bar->BarRect and therefore the available width!)
    const bool tab_list_popup_button = (tab_bar->Flags & vsonyp0werTabBarFlags_TabListPopupButton) != 0;
    if (tab_list_popup_button)
        if (vsonyp0werTabItem * tab_to_select = TabBarTabListPopupButton(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    ImVector<vsonyp0werTabBarSortItem> & width_sort_buffer = g.TabSortByWidthBuffer;
    width_sort_buffer.resize(tab_bar->Tabs.Size);

    // Compute ideal widths
    float width_total_contents = 0.0f;
    vsonyp0werTabItem * most_recently_selected_tab = NULL;
    bool found_selected_tab_id = false;
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_n];
        IM_ASSERT(tab->LastFrameVisible >= tab_bar->PrevFrameVisible);

        if (most_recently_selected_tab == NULL || most_recently_selected_tab->LastFrameSelected < tab->LastFrameSelected)
            most_recently_selected_tab = tab;
        if (tab->ID == tab_bar->SelectedTabId)
            found_selected_tab_id = true;

        // Refresh tab width immediately, otherwise changes of style e.g. style.FramePadding.x would noticeably lag in the tab bar.
        // Additionally, when using TabBarAddTab() to manipulate tab bar order we occasionally insert new tabs that don't have a width yet,
        // and we cannot wait for the next BeginTabItem() call. We cannot compute this width within TabBarAddTab() because font size depends on the active window.
        const char* tab_name = tab_bar->GetTabName(tab);
        tab->WidthContents = TabItemCalcSize(tab_name, (tab->Flags & vsonyp0werTabItemFlags_NoCloseButton) ? false : true).x;

        width_total_contents += (tab_n > 0 ? g.Style.ItemInnerSpacing.x : 0.0f) + tab->WidthContents;

        // Store data so we can build an array sorted by width if we need to shrink tabs down
        width_sort_buffer[tab_n].Index = tab_n;
        width_sort_buffer[tab_n].Width = tab->WidthContents;
    }

    // Compute width
    const float width_avail = tab_bar->BarRect.GetWidth();
    float width_excess = (width_avail < width_total_contents) ? (width_total_contents - width_avail) : 0.0f;
    if (width_excess > 0.0f && (tab_bar->Flags & vsonyp0werTabBarFlags_FittingPolicyResizeDown))
    {
        // If we don't have enough room, resize down the largest tabs first
        if (tab_bar->Tabs.Size > 1)
            ImQsort(width_sort_buffer.Data, (size_t)width_sort_buffer.Size, sizeof(vsonyp0werTabBarSortItem), TabBarSortItemComparer);
        int tab_count_same_width = 1;
        while (width_excess > 0.0f && tab_count_same_width < tab_bar->Tabs.Size)
        {
            while (tab_count_same_width < tab_bar->Tabs.Size && width_sort_buffer[0].Width == width_sort_buffer[tab_count_same_width].Width)
                tab_count_same_width++;
            float width_to_remove_per_tab_max = (tab_count_same_width < tab_bar->Tabs.Size) ? (width_sort_buffer[0].Width - width_sort_buffer[tab_count_same_width].Width) : (width_sort_buffer[0].Width - 1.0f);
            float width_to_remove_per_tab = ImMin(width_excess / tab_count_same_width, width_to_remove_per_tab_max);
            for (int tab_n = 0; tab_n < tab_count_same_width; tab_n++)
                width_sort_buffer[tab_n].Width -= width_to_remove_per_tab;
            width_excess -= width_to_remove_per_tab * tab_count_same_width;
        }
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
            tab_bar->Tabs[width_sort_buffer[tab_n].Index].Width = (float)(int)width_sort_buffer[tab_n].Width;
    } else
    {
        const float tab_max_width = TabBarCalcMaxTabWidth();
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_n];
            tab->Width = ImMin(tab->WidthContents, tab_max_width);
        }
    }

    // Layout all active tabs
    float offset_x = 0.0f;
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
    {
        vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_n];
        tab->Offset = offset_x;
        if (scroll_track_selected_tab_id == 0 && g.NavJustMovedToId == tab->ID)
            scroll_track_selected_tab_id = tab->ID;
        offset_x += tab->Width + g.Style.ItemInnerSpacing.x;
    }
    tab_bar->OffsetMax = ImMax(offset_x - g.Style.ItemInnerSpacing.x, 0.0f);
    tab_bar->OffsetNextTab = 0.0f;

    // Horizontal scrolling buttons
    const bool scrolling_buttons = (tab_bar->OffsetMax > tab_bar->BarRect.GetWidth() && tab_bar->Tabs.Size > 1) && !(tab_bar->Flags & vsonyp0werTabBarFlags_NoTabListScrollingButtons) && (tab_bar->Flags & vsonyp0werTabBarFlags_FittingPolicyScroll);
    if (scrolling_buttons)
        if (vsonyp0werTabItem * tab_to_select = TabBarScrollingButtons(tab_bar)) // NB: Will alter BarRect.Max.x!
            scroll_track_selected_tab_id = tab_bar->SelectedTabId = tab_to_select->ID;

    // If we have lost the selected tab, select the next most recently active one
    if (found_selected_tab_id == false)
        tab_bar->SelectedTabId = 0;
    if (tab_bar->SelectedTabId == 0 && tab_bar->NextSelectedTabId == 0 && most_recently_selected_tab != NULL)
        scroll_track_selected_tab_id = tab_bar->SelectedTabId = most_recently_selected_tab->ID;

    // Lock in visible tab
    tab_bar->VisibleTabId = tab_bar->SelectedTabId;
    tab_bar->VisibleTabWasSubmitted = false;

    // Update scrolling
    if (scroll_track_selected_tab_id)
        if (vsonyp0werTabItem * scroll_track_selected_tab = TabBarFindTabByID(tab_bar, scroll_track_selected_tab_id))
            TabBarScrollToTab(tab_bar, scroll_track_selected_tab);
    tab_bar->ScrollingAnim = TabBarScrollClamp(tab_bar, tab_bar->ScrollingAnim);
    tab_bar->ScrollingTarget = TabBarScrollClamp(tab_bar, tab_bar->ScrollingTarget);
    if (tab_bar->ScrollingAnim != tab_bar->ScrollingTarget)
    {
        // Scrolling speed adjust itself so we can always reach our target in 1/3 seconds.
        // Teleport if we are aiming far off the visible line
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, 70.0f * g.FontSize);
        tab_bar->ScrollingSpeed = ImMax(tab_bar->ScrollingSpeed, ImFabs(tab_bar->ScrollingTarget - tab_bar->ScrollingAnim) / 0.3f);
        const bool teleport = (tab_bar->PrevFrameVisible + 1 < g.FrameCount) || (tab_bar->ScrollingTargetDistToVisibility > 10.0f * g.FontSize);
        tab_bar->ScrollingAnim = teleport ? tab_bar->ScrollingTarget : ImLinearSweep(tab_bar->ScrollingAnim, tab_bar->ScrollingTarget, g.IO.DeltaTime * tab_bar->ScrollingSpeed);
    } else
    {
        tab_bar->ScrollingSpeed = 0.0f;
    }

    // Clear name buffers
    if ((tab_bar->Flags & vsonyp0werTabBarFlags_DockNode) == 0)
        tab_bar->TabsNames.Buf.resize(0);
}

// Dockables uses Name/ID in the global namespace. Non-dockable items use the ID stack.
static ImU32   vsonyp0wer::TabBarCalcTabID(vsonyp0werTabBar * tab_bar, const char* label)
{
    if (tab_bar->Flags & vsonyp0werTabBarFlags_DockNode)
    {
        vsonyp0werID id = ImHashStr(label, 0);
        KeepAliveID(id);
        return id;
    } else
    {
        vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
        return window->GetID(label);
    }
}

static float vsonyp0wer::TabBarCalcMaxTabWidth()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize* 20.0f;
}

vsonyp0werTabItem* vsonyp0wer::TabBarFindTabByID(vsonyp0werTabBar * tab_bar, vsonyp0werID tab_id)
{
    if (tab_id != 0)
        for (int n = 0; n < tab_bar->Tabs.Size; n++)
            if (tab_bar->Tabs[n].ID == tab_id)
                return &tab_bar->Tabs[n];
    return NULL;
}

// The *TabId fields be already set by the docking system _before_ the actual TabItem was created, so we clear them regardless.
void vsonyp0wer::TabBarRemoveTab(vsonyp0werTabBar * tab_bar, vsonyp0werID tab_id)
{
    if (vsonyp0werTabItem * tab = TabBarFindTabByID(tab_bar, tab_id))
        tab_bar->Tabs.erase(tab);
    if (tab_bar->VisibleTabId == tab_id) { tab_bar->VisibleTabId = 0; }
    if (tab_bar->SelectedTabId == tab_id) { tab_bar->SelectedTabId = 0; }
    if (tab_bar->NextSelectedTabId == tab_id) { tab_bar->NextSelectedTabId = 0; }
}

// Called on manual closure attempt
void vsonyp0wer::TabBarCloseTab(vsonyp0werTabBar * tab_bar, vsonyp0werTabItem * tab)
{
    if ((tab_bar->VisibleTabId == tab->ID) && !(tab->Flags & vsonyp0werTabItemFlags_UnsavedDocument))
    {
        // This will remove a frame of lag for selecting another tab on closure.
        // However we don't run it in the case where the 'Unsaved' flag is set, so user gets a chance to fully undo the closure
        tab->LastFrameVisible = -1;
        tab_bar->SelectedTabId = tab_bar->NextSelectedTabId = 0;
    } else if ((tab_bar->VisibleTabId != tab->ID) && (tab->Flags & vsonyp0werTabItemFlags_UnsavedDocument))
    {
        // Actually select before expecting closure
        tab_bar->NextSelectedTabId = tab->ID;
    }
}

static float vsonyp0wer::TabBarScrollClamp(vsonyp0werTabBar * tab_bar, float scrolling)
{
    scrolling = ImMin(scrolling, tab_bar->OffsetMax - tab_bar->BarRect.GetWidth());
    return ImMax(scrolling, 0.0f);
}

static void vsonyp0wer::TabBarScrollToTab(vsonyp0werTabBar * tab_bar, vsonyp0werTabItem * tab)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    float margin = g.FontSize * 1.0f; // When to scroll to make Tab N+1 visible always make a bit of N visible to suggest more scrolling area (since we don't have a scrollbar)
    int order = tab_bar->GetTabOrder(tab);
    float tab_x1 = tab->Offset + (order > 0 ? -margin : 0.0f);
    float tab_x2 = tab->Offset + tab->Width + (order + 1 < tab_bar->Tabs.Size ? margin : 1.0f);
    tab_bar->ScrollingTargetDistToVisibility = 0.0f;
    if (tab_bar->ScrollingTarget > tab_x1)
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax(tab_bar->ScrollingAnim - tab_x2, 0.0f);
        tab_bar->ScrollingTarget = tab_x1;
    } else if (tab_bar->ScrollingTarget < tab_x2 - tab_bar->BarRect.GetWidth())
    {
        tab_bar->ScrollingTargetDistToVisibility = ImMax((tab_x1 - tab_bar->BarRect.GetWidth()) - tab_bar->ScrollingAnim, 0.0f);
        tab_bar->ScrollingTarget = tab_x2 - tab_bar->BarRect.GetWidth();
    }
}

void vsonyp0wer::TabBarQueueChangeTabOrder(vsonyp0werTabBar * tab_bar, const vsonyp0werTabItem * tab, int dir)
{
    IM_ASSERT(dir == -1 || dir == +1);
    IM_ASSERT(tab_bar->ReorderRequestTabId == 0);
    tab_bar->ReorderRequestTabId = tab->ID;
    tab_bar->ReorderRequestDir = dir;
}

static vsonyp0werTabItem * vsonyp0wer::TabBarScrollingButtons(vsonyp0werTabBar * tab_bar)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    const ImVec2 arrow_button_size(g.FontSize - 2.0f, g.FontSize + g.Style.FramePadding.y * 2.0f);
    const float scrolling_buttons_width = arrow_button_size.x * 2.0f;

    const ImVec2 backup_cursor_pos = window->DC.CursorPos;
    //window->DrawList->AddRect(ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y), ImVec2(tab_bar->BarRect.Max.x, tab_bar->BarRect.Max.y), IM_COL32(255,0,0,255));

    const ImRect avail_bar_rect = tab_bar->BarRect;
    bool want_clip_rect = !avail_bar_rect.Contains(ImRect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(scrolling_buttons_width, 0.0f)));
    if (want_clip_rect)
        PushClipRect(tab_bar->BarRect.Min, tab_bar->BarRect.Max + ImVec2(g.Style.ItemInnerSpacing.x, 0.0f), true);

    vsonyp0werTabItem * tab_to_select = NULL;

    int select_dir = 0;
    ImVec4 arrow_col = g.Style.Colors[vsonyp0werCol_Text];
    arrow_col.w *= 0.5f;

    PushStyleColor(vsonyp0werCol_Text, arrow_col);
    PushStyleColor(vsonyp0werCol_Button, ImVec4(0, 0, 0, 0));
    const float backup_repeat_delay = g.IO.KeyRepeatDelay;
    const float backup_repeat_rate = g.IO.KeyRepeatRate;
    g.IO.KeyRepeatDelay = 0.250f;
    g.IO.KeyRepeatRate = 0.200f;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##<", vsonyp0werDir_Left, arrow_button_size, vsonyp0werButtonFlags_PressedOnClick | vsonyp0werButtonFlags_Repeat))
        select_dir = -1;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Max.x - scrolling_buttons_width + arrow_button_size.x, tab_bar->BarRect.Min.y);
    if (ArrowButtonEx("##>", vsonyp0werDir_Right, arrow_button_size, vsonyp0werButtonFlags_PressedOnClick | vsonyp0werButtonFlags_Repeat))
        select_dir = +1;
    PopStyleColor(2);
    g.IO.KeyRepeatRate = backup_repeat_rate;
    g.IO.KeyRepeatDelay = backup_repeat_delay;

    if (want_clip_rect)
        PopClipRect();

    if (select_dir != 0)
        if (vsonyp0werTabItem * tab_item = TabBarFindTabByID(tab_bar, tab_bar->SelectedTabId))
        {
            int selected_order = tab_bar->GetTabOrder(tab_item);
            int target_order = selected_order + select_dir;
            tab_to_select = &tab_bar->Tabs[(target_order >= 0 && target_order < tab_bar->Tabs.Size) ? target_order : selected_order]; // If we are at the end of the list, still scroll to make our tab visible
        }
    window->DC.CursorPos = backup_cursor_pos;
    tab_bar->BarRect.Max.x -= scrolling_buttons_width + 1.0f;

    return tab_to_select;
}

static vsonyp0werTabItem* vsonyp0wer::TabBarTabListPopupButton(vsonyp0werTabBar * tab_bar)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    // We use g.Style.FramePadding.y to match the square ArrowButton size
    const float tab_list_popup_button_width = g.FontSize + g.Style.FramePadding.y;
    const ImVec2 backup_cursor_pos = window->DC.CursorPos;
    window->DC.CursorPos = ImVec2(tab_bar->BarRect.Min.x - g.Style.FramePadding.y, tab_bar->BarRect.Min.y);
    tab_bar->BarRect.Min.x += tab_list_popup_button_width;

    ImVec4 arrow_col = g.Style.Colors[vsonyp0werCol_Text];
    arrow_col.w *= 0.5f;
    PushStyleColor(vsonyp0werCol_Text, arrow_col);
    PushStyleColor(vsonyp0werCol_Button, ImVec4(0, 0, 0, 0));
    bool open = BeginCombo("##v", NULL, vsonyp0werComboFlags_NoPreview);
    PopStyleColor(2);

    vsonyp0werTabItem* tab_to_select = NULL;
    if (open)
    {
        for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
        {
            vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_n];
            const char* tab_name = tab_bar->GetTabName(tab);
            if (Selectable(tab_name, tab_bar->SelectedTabId == tab->ID))
                tab_to_select = tab;
        }
        EndCombo();
    }

    window->DC.CursorPos = backup_cursor_pos;
    return tab_to_select;
}

//-------------------------------------------------------------------------
// [SECTION] Widgets: BeginTabItem, EndTabItem, etc.
//-------------------------------------------------------------------------
// [BETA API] API may evolve! This code has been extracted out of the Docking branch,
// and some of the construct which are not used in Master may be left here to facilitate merging.
//-------------------------------------------------------------------------
// - BeginTabItem()
// - EndTabItem()
// - TabItemEx() [Internal]
// - SetTabItemClosed()
// - TabItemCalcSize() [Internal]
// - TabItemBackground() [Internal]
// - TabItemLabelAndCloseButton() [Internal]
//-------------------------------------------------------------------------

bool    vsonyp0wer::BeginTabItem(const char* label, bool* p_open, vsonyp0werTabItemFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.CurrentWindow->SkipItems)
        return false;

    vsonyp0werTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar && "Needs to be called between BeginTabBar() and EndTabBar()!");
        return false; // FIXME-ERRORHANDLING
    }
    bool ret = TabItemEx(tab_bar, label, p_open, flags);
    if (ret && !(flags & vsonyp0werTabItemFlags_NoPushId))
    {
        vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
        g.CurrentWindow->IDStack.push_back(tab->ID);    // We already hashed 'label' so push into the ID stack directly instead of doing another hash through PushID(label)
    }
    return ret;
}

void    vsonyp0wer::EndTabItem()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.CurrentWindow->SkipItems)
        return;

    vsonyp0werTabBar* tab_bar = g.CurrentTabBar;
    if (tab_bar == NULL)
    {
        IM_ASSERT(tab_bar != NULL && "Needs to be called between BeginTabBar() and EndTabBar()!");
        return;
    }
    IM_ASSERT(tab_bar->LastTabItemIdx >= 0);
    vsonyp0werTabItem * tab = &tab_bar->Tabs[tab_bar->LastTabItemIdx];
    if (!(tab->Flags & vsonyp0werTabItemFlags_NoPushId))
        g.CurrentWindow->IDStack.pop_back();
}

bool    vsonyp0wer::TabItemEx(vsonyp0werTabBar * tab_bar, const char* label, bool* p_open, vsonyp0werTabItemFlags flags)
{
    // Layout whole tab bar if not already done
    if (tab_bar->WantLayout)
        TabBarLayout(tab_bar);

    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    const vsonyp0werStyle& style = g.Style;
    const vsonyp0werID id = TabBarCalcTabID(tab_bar, label);

    // If the user called us with *p_open == false, we early out and don't render. We make a dummy call to ItemAdd() so that attempts to use a contextual popup menu with an implicit ID won't use an older ID.
    if (p_open && !*p_open)
    {
        PushItemFlag(vsonyp0werItemFlags_NoNav | vsonyp0werItemFlags_NoNavDefaultFocus, true);
        ItemAdd(ImRect(), id);
        PopItemFlag();
        return false;
    }

    // Calculate tab contents size
    ImVec2 size = TabItemCalcSize(label, p_open != NULL);

    // Acquire tab data
    vsonyp0werTabItem * tab = TabBarFindTabByID(tab_bar, id);
    bool tab_is_new = false;
    if (tab == NULL)
    {
        tab_bar->Tabs.push_back(vsonyp0werTabItem());
        tab = &tab_bar->Tabs.back();
        tab->ID = id;
        tab->Width = size.x;
        tab_is_new = true;
    }
    tab_bar->LastTabItemIdx = (short)tab_bar->Tabs.index_from_ptr(tab);
    tab->WidthContents = size.x;

    if (p_open == NULL)
        flags |= vsonyp0werTabItemFlags_NoCloseButton;

    const bool tab_bar_appearing = (tab_bar->PrevFrameVisible + 1 < g.FrameCount);
    const bool tab_bar_focused = (tab_bar->Flags & vsonyp0werTabBarFlags_IsFocused) != 0;
    const bool tab_appearing = (tab->LastFrameVisible + 1 < g.FrameCount);
    tab->LastFrameVisible = g.FrameCount;
    tab->Flags = flags;

    // Append name with zero-terminator
    tab->NameOffset = tab_bar->TabsNames.size();
    tab_bar->TabsNames.append(label, label + strlen(label) + 1);

    // If we are not reorderable, always reset offset based on submission order.
    // (We already handled layout and sizing using the previous known order, but sizing is not affected by order!)
    if (!tab_appearing && !(tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable))
    {
        tab->Offset = tab_bar->OffsetNextTab;
        tab_bar->OffsetNextTab += tab->Width + g.Style.ItemInnerSpacing.x;
    }

    // Update selected tab
    if (tab_appearing && (tab_bar->Flags & vsonyp0werTabBarFlags_AutoSelectNewTabs) && tab_bar->NextSelectedTabId == 0)
        if (!tab_bar_appearing || tab_bar->SelectedTabId == 0)
            tab_bar->NextSelectedTabId = id;  // New tabs gets activated
    if ((flags & vsonyp0werTabItemFlags_SetSelected) && (tab_bar->SelectedTabId != id)) // SetSelected can only be passed on explicit tab bar
        tab_bar->NextSelectedTabId = id;

    // Lock visibility
    bool tab_contents_visible = (tab_bar->VisibleTabId == id);
    if (tab_contents_visible)
        tab_bar->VisibleTabWasSubmitted = true;

    // On the very first frame of a tab bar we let first tab contents be visible to minimize appearing glitches
    if (!tab_contents_visible && tab_bar->SelectedTabId == 0 && tab_bar_appearing)
        if (tab_bar->Tabs.Size == 1 && !(tab_bar->Flags & vsonyp0werTabBarFlags_AutoSelectNewTabs))
            tab_contents_visible = true;

    if (tab_appearing && !(tab_bar_appearing && !tab_is_new))
    {
        PushItemFlag(vsonyp0werItemFlags_NoNav | vsonyp0werItemFlags_NoNavDefaultFocus, true);
        ItemAdd(ImRect(), id);
        PopItemFlag();
        return tab_contents_visible;
    }

    if (tab_bar->SelectedTabId == id)
        tab->LastFrameSelected = g.FrameCount;

    // Backup current layout position
    const ImVec2 backup_main_cursor_pos = window->DC.CursorPos;

    // Layout
    size.x = tab->Width;
    window->DC.CursorPos = tab_bar->BarRect.Min + ImVec2((float)(int)tab->Offset - tab_bar->ScrollingAnim, 0.0f);
    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos, pos + size);

    // We don't have CPU clipping primitives to clip the CloseButton (until it becomes a texture), so need to add an extra draw call (temporary in the case of vertical animation)
    bool want_clip_rect = (bb.Min.x < tab_bar->BarRect.Min.x) || (bb.Max.x >= tab_bar->BarRect.Max.x);
    if (want_clip_rect)
        PushClipRect(ImVec2(ImMax(bb.Min.x, tab_bar->BarRect.Min.x), bb.Min.y - 1), ImVec2(tab_bar->BarRect.Max.x, bb.Max.y), true);

    ItemSize(bb, style.FramePadding.y);
    if (!ItemAdd(bb, id))
    {
        if (want_clip_rect)
            PopClipRect();
        window->DC.CursorPos = backup_main_cursor_pos;
        return tab_contents_visible;
    }

    // Click to Select a tab
    vsonyp0werButtonFlags button_flags = (vsonyp0werButtonFlags_PressedOnClick | vsonyp0werButtonFlags_AllowItemOverlap);
    if (g.DragDropActive)
        button_flags |= vsonyp0werButtonFlags_PressedOnDragDropHold;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);
    if (pressed)
        tab_bar->NextSelectedTabId = id;
    hovered |= (g.HoveredId == id);

    // Allow the close button to overlap unless we are dragging (in which case we don't want any overlapping tabs to be hovered)
    if (!held)
        SetItemAllowOverlap();

    // Drag and drop: re-order tabs
    if (held && !tab_appearing && IsMouseDragging(0))
    {
        if (!g.DragDropActive && (tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable))
        {
            // While moving a tab it will jump on the other side of the mouse, so we also test for MouseDelta.x
            if (g.IO.MouseDelta.x < 0.0f && g.IO.MousePos.x < bb.Min.x)
            {
                if (tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, -1);
            } else if (g.IO.MouseDelta.x > 0.0f && g.IO.MousePos.x > bb.Max.x)
            {
                if (tab_bar->Flags & vsonyp0werTabBarFlags_Reorderable)
                    TabBarQueueChangeTabOrder(tab_bar, tab, +1);
            }
        }
    }

#if 0
    if (hovered && g.HoveredIdNotActiveTimer > 0.50f && bb.GetWidth() < tab->WidthContents)
    {
        // Enlarge tab display when hovering
        bb.Max.x = bb.Min.x + (float)(int)ImLerp(bb.GetWidth(), tab->WidthContents, ImSaturate((g.HoveredIdNotActiveTimer - 0.40f) * 6.0f));
        display_draw_list = GetForegroundDrawList(window);
        TabItemBackground(display_draw_list, bb, flags, GetColorU32(vsonyp0werCol_TitleBgActive));
    }
#endif

    // Render tab shape
    ImDrawList * display_draw_list = window->DrawList;
    const ImU32 tab_col = GetColorU32((held || hovered) ? vsonyp0werCol_TabHovered : tab_contents_visible ? (tab_bar_focused ? vsonyp0werCol_TabActive : vsonyp0werCol_TabUnfocusedActive) : (tab_bar_focused ? vsonyp0werCol_Tab : vsonyp0werCol_TabUnfocused));
    TabItemBackground(display_draw_list, bb, flags, tab_col);
    RenderNavHighlight(bb, id);

    // Select with right mouse button. This is so the common idiom for context menu automatically highlight the current widget.
    const bool hovered_unblocked = IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup);
    if (hovered_unblocked && (IsMouseClicked(1) || IsMouseReleased(1)))
        tab_bar->NextSelectedTabId = id;

    if (tab_bar->Flags & vsonyp0werTabBarFlags_NoCloseWithMiddleMouseButton)
        flags |= vsonyp0werTabItemFlags_NoCloseWithMiddleMouseButton;

    // Render tab label, process close button
    const vsonyp0werID close_button_id = p_open ? window->GetID((void*)((intptr_t)id + 1)) : 0;
    bool just_closed = TabItemLabelAndCloseButton(display_draw_list, bb, flags, tab_bar->FramePadding, label, id, close_button_id);
    if (just_closed && p_open != NULL)
    {
        *p_open = false;
        TabBarCloseTab(tab_bar, tab);
    }

    // Restore main window position so user can draw there
    if (want_clip_rect)
        PopClipRect();
    window->DC.CursorPos = backup_main_cursor_pos;

    // Tooltip (FIXME: Won't work over the close button because ItemOverlap systems messes up with HoveredIdTimer)
    // We test IsItemHovered() to discard e.g. when another item is active or drag and drop over the tab bar (which g.HoveredId ignores)
    if (g.HoveredId == id && !held && g.HoveredIdNotActiveTimer > 0.50f && IsItemHovered())
        if (!(tab_bar->Flags & vsonyp0werTabBarFlags_NoTooltip))
            SetTooltip("%.*s", (int)(FindRenderedTextEnd(label) - label), label);

    return tab_contents_visible;
}

// [Public] This is call is 100% optional but it allows to remove some one-frame glitches when a tab has been unexpectedly removed.
// To use it to need to call the function SetTabItemClosed() after BeginTabBar() and before any call to BeginTabItem()
void    vsonyp0wer::SetTabItemClosed(const char* label)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    bool is_within_manual_tab_bar = g.CurrentTabBar && !(g.CurrentTabBar->Flags & vsonyp0werTabBarFlags_DockNode);
    if (is_within_manual_tab_bar)
    {
        vsonyp0werTabBar* tab_bar = g.CurrentTabBar;
        IM_ASSERT(tab_bar->WantLayout);         // Needs to be called AFTER BeginTabBar() and BEFORE the first call to BeginTabItem()
        vsonyp0werID tab_id = TabBarCalcTabID(tab_bar, label);
        TabBarRemoveTab(tab_bar, tab_id);
    }
}

ImVec2 vsonyp0wer::TabItemCalcSize(const char* label, bool has_close_button)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size = ImVec2(label_size.x + g.Style.FramePadding.x, label_size.y + g.Style.FramePadding.y * 2.0f);
    if (has_close_button)
        size.x += g.Style.FramePadding.x + (g.Style.ItemInnerSpacing.x + g.FontSize); // We use Y intentionally to fit the close button circle.
    else
        size.x += g.Style.FramePadding.x + 1.0f;
    return ImVec2(ImMin(size.x, TabBarCalcMaxTabWidth()), size.y);
}

void vsonyp0wer::TabItemBackground(ImDrawList * draw_list, const ImRect & bb, vsonyp0werTabItemFlags flags, ImU32 col)
{
    // While rendering tabs, we trim 1 pixel off the top of our bounding box so they can fit within a regular frame height while looking "detached" from it.
    vsonyp0werContext& g = *Gvsonyp0wer;
    const float width = bb.GetWidth();
    IM_UNUSED(flags);
    IM_ASSERT(width > 0.0f);
    const float rounding = ImMax(0.0f, ImMin(g.Style.TabRounding, width * 0.5f - 1.0f));
    const float y1 = bb.Min.y + 1.0f;
    const float y2 = bb.Max.y - 1.0f;
    draw_list->PathLineTo(ImVec2(bb.Min.x, y2));
    draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding, y1 + rounding), rounding, 6, 9);
    draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding, y1 + rounding), rounding, 9, 12);
    draw_list->PathLineTo(ImVec2(bb.Max.x, y2));
    draw_list->PathFillConvex(col);
    if (g.Style.TabBorderSize > 0.0f)
    {
        draw_list->PathLineTo(ImVec2(bb.Min.x + 0.5f, y2));
        draw_list->PathArcToFast(ImVec2(bb.Min.x + rounding + 0.5f, y1 + rounding + 0.5f), rounding, 6, 9);
        draw_list->PathArcToFast(ImVec2(bb.Max.x - rounding - 0.5f, y1 + rounding + 0.5f), rounding, 9, 12);
        draw_list->PathLineTo(ImVec2(bb.Max.x - 0.5f, y2));
        draw_list->PathStroke(GetColorU32(vsonyp0werCol_Border), false, g.Style.TabBorderSize);
    }
}

// Render text label (with custom clipping) + Unsaved Document marker + Close Button logic
// We tend to lock style.FramePadding for a given tab-bar, hence the 'frame_padding' parameter.
bool vsonyp0wer::TabItemLabelAndCloseButton(ImDrawList * draw_list, const ImRect & bb, vsonyp0werTabItemFlags flags, ImVec2 frame_padding, const char* label, vsonyp0werID tab_id, vsonyp0werID close_button_id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    if (bb.GetWidth() <= 1.0f)
        return false;

    // Render text label (with clipping + alpha gradient) + unsaved marker
    const char* TAB_UNSAVED_MARKER = "*";
    ImRect text_pixel_clip_bb(bb.Min.x + frame_padding.x, bb.Min.y + frame_padding.y, bb.Max.x - frame_padding.x, bb.Max.y);
    if (flags & vsonyp0werTabItemFlags_UnsavedDocument)
    {
        text_pixel_clip_bb.Max.x -= CalcTextSize(TAB_UNSAVED_MARKER, NULL, false).x;
        ImVec2 unsaved_marker_pos(ImMin(bb.Min.x + frame_padding.x + label_size.x + 2, text_pixel_clip_bb.Max.x), bb.Min.y + frame_padding.y + (float)(int)(-g.FontSize * 0.25f));
        RenderTextClippedEx(draw_list, unsaved_marker_pos, bb.Max - frame_padding, TAB_UNSAVED_MARKER, NULL, NULL);
    }
    ImRect text_ellipsis_clip_bb = text_pixel_clip_bb;

    // Close Button
    // We are relying on a subtle and confusing distinction between 'hovered' and 'g.HoveredId' which happens because we are using vsonyp0werButtonFlags_AllowOverlapMode + SetItemAllowOverlap()
    //  'hovered' will be true when hovering the Tab but NOT when hovering the close button
    //  'g.HoveredId==id' will be true when hovering the Tab including when hovering the close button
    //  'g.ActiveId==close_button_id' will be true when we are holding on the close button, in which case both hovered booleans are false
    bool close_button_pressed = false;
    bool close_button_visible = false;
    if (close_button_id != 0)
        if (g.HoveredId == tab_id || g.HoveredId == close_button_id || g.ActiveId == close_button_id)
            close_button_visible = true;
    if (close_button_visible)
    {
        vsonyp0werItemHoveredDataBackup last_item_backup;
        const float close_button_sz = g.FontSize * 0.5f;
        if (CloseButton(close_button_id, ImVec2(bb.Max.x - frame_padding.x - close_button_sz, bb.Min.y + frame_padding.y + close_button_sz), close_button_sz))
            close_button_pressed = true;
        last_item_backup.Restore();

        // Close with middle mouse button
        if (!(flags & vsonyp0werTabItemFlags_NoCloseWithMiddleMouseButton) && IsMouseClicked(2))
            close_button_pressed = true;

        text_pixel_clip_bb.Max.x -= close_button_sz * 2.0f;
    }

    // Label with ellipsis
    // FIXME: This should be extracted into a helper but the use of text_pixel_clip_bb and !close_button_visible makes it tricky to abstract at the moment
    const char* label_display_end = FindRenderedTextEnd(label);
    if (label_size.x > text_ellipsis_clip_bb.GetWidth())
    {
        const int ellipsis_dot_count = 3;
        const float ellipsis_width = (1.0f + 1.0f) * ellipsis_dot_count - 1.0f;
        const char* label_end = NULL;
        float label_size_clipped_x = g.Font->CalcTextSizeA(g.FontSize, text_ellipsis_clip_bb.GetWidth() - ellipsis_width + 1.0f, 0.0f, label, label_display_end, &label_end).x;
        if (label_end == label && label_end < label_display_end)    // Always display at least 1 character if there's no room for character + ellipsis
        {
            label_end = label + ImTextCountUtf8BytesFromChar(label, label_display_end);
            label_size_clipped_x = g.Font->CalcTextSizeA(g.FontSize, FLT_MAX, 0.0f, label, label_end).x;
        }
        while (label_end > label && ImCharIsBlankA(label_end[-1])) // Trim trailing space
        {
            label_end--;
            label_size_clipped_x -= g.Font->CalcTextSizeA(g.FontSize, FLT_MAX, 0.0f, label_end, label_end + 1).x; // Ascii blanks are always 1 byte
        }
        RenderTextClippedEx(draw_list, text_pixel_clip_bb.Min, text_pixel_clip_bb.Max, label, label_end, &label_size, ImVec2(0.0f, 0.0f));

        const float ellipsis_x = text_pixel_clip_bb.Min.x + label_size_clipped_x + 1.0f;
        if (!close_button_visible && ellipsis_x + ellipsis_width <= bb.Max.x)
            RenderPixelEllipsis(draw_list, ImVec2(ellipsis_x, text_pixel_clip_bb.Min.y), ellipsis_dot_count, GetColorU32(vsonyp0werCol_Text));
    } else
    {
        RenderTextClippedEx(draw_list, text_pixel_clip_bb.Min, text_pixel_clip_bb.Max, label, label_display_end, &label_size, ImVec2(0.0f, 0.0f));
    }

    return close_button_pressed;
}



































































// Junk Code By Troll Face & Thaisen's Gen
void VgSVqMvNovTtINGSvKvwBkodrwMZbgDaKIQYHiQl26034908() {     int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15390764 = -897052653;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK23009797 = -864057629;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1151943 = -755396791;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK85239577 = -147426255;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK10524440 = -4761526;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15161976 = -409167526;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK42127871 = -242293034;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82672551 = -638570559;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29535058 = -204023922;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2031894 = -617710670;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK90939864 = -339104193;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5796753 = -258343280;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK91751304 = -442320861;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8834308 = -143394510;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96786237 = -744459029;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18744097 = -672052293;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87995052 = -282198296;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79447624 = -359781313;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17731464 = -520030929;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK11166006 = -162632421;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12153462 = -149575558;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72035577 = -435537466;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26954145 = -261195325;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88664290 = -713080349;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK9885237 = -14338471;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86084400 = -664433477;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1683284 = -935687635;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24773983 = -604758846;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK16664162 = -576336571;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK99428596 = -301423754;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK45129659 = -866782642;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83934793 = -729197788;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2751482 = -40660126;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19234345 = -538881353;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13052945 = -30655085;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1618148 = -22642904;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK362851 = -324285295;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28751407 = -179482675;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83978325 = -809364880;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96347649 = -639630478;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK32506481 = -869149971;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK33025591 = 65201890;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88761827 = -126940627;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK98321831 = -223407447;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26874742 = -894220468;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12404122 = -580469263;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK84875357 = -591051955;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18018019 = -169237765;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72674794 = -600619838;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8857472 = -416389183;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12955688 = -451123188;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK64351750 = -903029471;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK54241036 = -834760039;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51020986 = -281752007;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13881446 = -531858058;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK43355187 = -361515188;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96055651 = -502862305;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12487652 = 57683557;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK75354341 = -33087785;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24440040 = -340328049;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13478693 = -473479892;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17353889 = -637534188;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK66008389 = 37766012;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK30106461 = -902600168;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK56902234 = -750928029;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7005071 = -609906406;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK3045272 = -117683155;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72516960 = -903439509;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95781362 = -12739425;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95168089 = -621816126;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18381247 = -247766999;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59243645 = -2715621;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95469299 = -550416433;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK21383814 = -880400452;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK78659524 = -293482450;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79127870 = -114777449;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83273749 = -208596839;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28632314 = 62212121;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK61789548 = -818859881;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK97481114 = -433869208;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1209043 = 26618477;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83665265 = -666449870;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK52099188 = 95860991;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7806691 = -59947389;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86472909 = -850300566;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK80777908 = -963753172;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29693758 = -894437750;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51730496 = -758908119;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5352899 = 92976705;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK69697758 = -669139897;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5562497 = -519780599;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87875198 = -281968853;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK53397066 = -46394891;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59538285 = -369036831;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82868956 = -66150586;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15152593 = -131615783;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK67017201 = -972564122;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK58655367 = -224340459;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK41419598 = -472479419;    int jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19869672 = -897052653;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15390764 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK23009797;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK23009797 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1151943;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1151943 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK85239577;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK85239577 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK10524440;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK10524440 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15161976;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15161976 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK42127871;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK42127871 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82672551;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82672551 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29535058;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29535058 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2031894;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2031894 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK90939864;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK90939864 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5796753;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5796753 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK91751304;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK91751304 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8834308;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8834308 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96786237;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96786237 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18744097;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18744097 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87995052;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87995052 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79447624;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79447624 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17731464;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17731464 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK11166006;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK11166006 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12153462;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12153462 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72035577;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72035577 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26954145;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26954145 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88664290;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88664290 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK9885237;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK9885237 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86084400;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86084400 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1683284;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1683284 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24773983;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24773983 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK16664162;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK16664162 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK99428596;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK99428596 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK45129659;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK45129659 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83934793;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83934793 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2751482;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK2751482 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19234345;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19234345 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13052945;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13052945 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1618148;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1618148 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK362851;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK362851 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28751407;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28751407 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83978325;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83978325 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96347649;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96347649 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK32506481;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK32506481 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK33025591;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK33025591 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88761827;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK88761827 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK98321831;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK98321831 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26874742;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK26874742 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12404122;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12404122 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK84875357;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK84875357 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18018019;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18018019 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72674794;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72674794 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8857472;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK8857472 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12955688;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12955688 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK64351750;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK64351750 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK54241036;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK54241036 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51020986;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51020986 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13881446;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13881446 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK43355187;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK43355187 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96055651;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK96055651 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12487652;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK12487652 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK75354341;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK75354341 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24440040;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK24440040 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13478693;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK13478693 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17353889;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK17353889 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK66008389;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK66008389 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK30106461;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK30106461 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK56902234;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK56902234 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7005071;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7005071 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK3045272;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK3045272 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72516960;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK72516960 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95781362;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95781362 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95168089;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95168089 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18381247;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK18381247 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59243645;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59243645 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95469299;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK95469299 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK21383814;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK21383814 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK78659524;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK78659524 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79127870;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK79127870 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83273749;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83273749 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28632314;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK28632314 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK61789548;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK61789548 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK97481114;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK97481114 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1209043;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK1209043 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83665265;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK83665265 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK52099188;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK52099188 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7806691;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK7806691 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86472909;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK86472909 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK80777908;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK80777908 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29693758;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK29693758 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51730496;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK51730496 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5352899;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5352899 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK69697758;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK69697758 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5562497;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK5562497 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87875198;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK87875198 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK53397066;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK53397066 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59538285;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK59538285 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82868956;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK82868956 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15152593;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15152593 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK67017201;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK67017201 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK58655367;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK58655367 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK41419598;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK41419598 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19869672;     jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK19869672 = jFYZmHQNQOyTMDGkroetzWlHstjAwiKMuJNwMOqjfzJNhjOGBfhSEXmMUQhtXQQhDmZCqK15390764;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NrwclkOvtpYpBWTvdjwNONFmPbOgQqoPKcbHQaMFDVSaIhRHpud14725299() {     float KJBwAmJMKVheDBJeYuMfCWrJtufe91007094 = -298560905;    float KJBwAmJMKVheDBJeYuMfCWrJtufe74300308 = -362495244;    float KJBwAmJMKVheDBJeYuMfCWrJtufe85675270 = -263601696;    float KJBwAmJMKVheDBJeYuMfCWrJtufe32762073 = -846959468;    float KJBwAmJMKVheDBJeYuMfCWrJtufe55730761 = -520449211;    float KJBwAmJMKVheDBJeYuMfCWrJtufe36668352 = -725129314;    float KJBwAmJMKVheDBJeYuMfCWrJtufe84641099 = -378572991;    float KJBwAmJMKVheDBJeYuMfCWrJtufe53981540 = -412050579;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28354199 = -916363918;    float KJBwAmJMKVheDBJeYuMfCWrJtufe93505593 = -781559676;    float KJBwAmJMKVheDBJeYuMfCWrJtufe31170252 = -327862565;    float KJBwAmJMKVheDBJeYuMfCWrJtufe53745105 = -897560170;    float KJBwAmJMKVheDBJeYuMfCWrJtufe1234533 = -156186607;    float KJBwAmJMKVheDBJeYuMfCWrJtufe62983219 = -129433049;    float KJBwAmJMKVheDBJeYuMfCWrJtufe50396102 = -952894592;    float KJBwAmJMKVheDBJeYuMfCWrJtufe11183674 = -951218198;    float KJBwAmJMKVheDBJeYuMfCWrJtufe58419955 = -393669329;    float KJBwAmJMKVheDBJeYuMfCWrJtufe40393910 = -894177282;    float KJBwAmJMKVheDBJeYuMfCWrJtufe9479218 = -371992965;    float KJBwAmJMKVheDBJeYuMfCWrJtufe7947768 = -381208429;    float KJBwAmJMKVheDBJeYuMfCWrJtufe47227749 = -581557250;    float KJBwAmJMKVheDBJeYuMfCWrJtufe45162816 = 58306219;    float KJBwAmJMKVheDBJeYuMfCWrJtufe5919297 = -391801930;    float KJBwAmJMKVheDBJeYuMfCWrJtufe80837249 = -903220088;    float KJBwAmJMKVheDBJeYuMfCWrJtufe44039529 = -485543373;    float KJBwAmJMKVheDBJeYuMfCWrJtufe51958644 = -86676603;    float KJBwAmJMKVheDBJeYuMfCWrJtufe56073750 = -722761711;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28912421 = -159183585;    float KJBwAmJMKVheDBJeYuMfCWrJtufe41322173 = -349701192;    float KJBwAmJMKVheDBJeYuMfCWrJtufe96850670 = -453136155;    float KJBwAmJMKVheDBJeYuMfCWrJtufe75137501 = -332186777;    float KJBwAmJMKVheDBJeYuMfCWrJtufe95290646 = -385952884;    float KJBwAmJMKVheDBJeYuMfCWrJtufe83052860 = -663560637;    float KJBwAmJMKVheDBJeYuMfCWrJtufe6720674 = -875724420;    float KJBwAmJMKVheDBJeYuMfCWrJtufe50014662 = -496058066;    float KJBwAmJMKVheDBJeYuMfCWrJtufe36004165 = -543116878;    float KJBwAmJMKVheDBJeYuMfCWrJtufe16496148 = -898639487;    float KJBwAmJMKVheDBJeYuMfCWrJtufe60969509 = -46355740;    float KJBwAmJMKVheDBJeYuMfCWrJtufe83864085 = -346120468;    float KJBwAmJMKVheDBJeYuMfCWrJtufe65477873 = -456338908;    float KJBwAmJMKVheDBJeYuMfCWrJtufe6980776 = -938169572;    float KJBwAmJMKVheDBJeYuMfCWrJtufe99588411 = -511716518;    float KJBwAmJMKVheDBJeYuMfCWrJtufe60421939 = -264327615;    float KJBwAmJMKVheDBJeYuMfCWrJtufe46850996 = -786985949;    float KJBwAmJMKVheDBJeYuMfCWrJtufe52709537 = -526263225;    float KJBwAmJMKVheDBJeYuMfCWrJtufe34459435 = -358585152;    float KJBwAmJMKVheDBJeYuMfCWrJtufe52468837 = -615574004;    float KJBwAmJMKVheDBJeYuMfCWrJtufe79913857 = 24149665;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28304713 = -166093758;    float KJBwAmJMKVheDBJeYuMfCWrJtufe38283053 = -115065823;    float KJBwAmJMKVheDBJeYuMfCWrJtufe22438691 = 57442397;    float KJBwAmJMKVheDBJeYuMfCWrJtufe13023189 = -149603229;    float KJBwAmJMKVheDBJeYuMfCWrJtufe97451291 = -209904516;    float KJBwAmJMKVheDBJeYuMfCWrJtufe8570273 = -205479630;    float KJBwAmJMKVheDBJeYuMfCWrJtufe67497549 = -133313495;    float KJBwAmJMKVheDBJeYuMfCWrJtufe45844279 = -256867124;    float KJBwAmJMKVheDBJeYuMfCWrJtufe68381012 = -970693315;    float KJBwAmJMKVheDBJeYuMfCWrJtufe4838021 = -360381608;    float KJBwAmJMKVheDBJeYuMfCWrJtufe88722543 = -261416096;    float KJBwAmJMKVheDBJeYuMfCWrJtufe3772118 = -333772609;    float KJBwAmJMKVheDBJeYuMfCWrJtufe80594602 = 97632397;    float KJBwAmJMKVheDBJeYuMfCWrJtufe55728678 = -119389406;    float KJBwAmJMKVheDBJeYuMfCWrJtufe12659368 = 37650612;    float KJBwAmJMKVheDBJeYuMfCWrJtufe31503529 = -363227764;    float KJBwAmJMKVheDBJeYuMfCWrJtufe18368092 = -349372899;    float KJBwAmJMKVheDBJeYuMfCWrJtufe35879605 = -941909681;    float KJBwAmJMKVheDBJeYuMfCWrJtufe70692244 = -133999534;    float KJBwAmJMKVheDBJeYuMfCWrJtufe94513858 = -280462187;    float KJBwAmJMKVheDBJeYuMfCWrJtufe12968557 = -633374983;    float KJBwAmJMKVheDBJeYuMfCWrJtufe14391937 = -309777715;    float KJBwAmJMKVheDBJeYuMfCWrJtufe94687526 = 47421288;    float KJBwAmJMKVheDBJeYuMfCWrJtufe97450446 = -247313590;    float KJBwAmJMKVheDBJeYuMfCWrJtufe56529824 = -448056814;    float KJBwAmJMKVheDBJeYuMfCWrJtufe44001344 = -915654058;    float KJBwAmJMKVheDBJeYuMfCWrJtufe966993 = -443038857;    float KJBwAmJMKVheDBJeYuMfCWrJtufe47639338 = 30159268;    float KJBwAmJMKVheDBJeYuMfCWrJtufe84740876 = -677366167;    float KJBwAmJMKVheDBJeYuMfCWrJtufe59068300 = -604815981;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28127713 = -276956863;    float KJBwAmJMKVheDBJeYuMfCWrJtufe9580094 = -26958222;    float KJBwAmJMKVheDBJeYuMfCWrJtufe99489807 = -471102599;    float KJBwAmJMKVheDBJeYuMfCWrJtufe76159892 = -646911376;    float KJBwAmJMKVheDBJeYuMfCWrJtufe607709 = -993089827;    float KJBwAmJMKVheDBJeYuMfCWrJtufe3039121 = -134635370;    float KJBwAmJMKVheDBJeYuMfCWrJtufe74411980 = -410578553;    float KJBwAmJMKVheDBJeYuMfCWrJtufe62114312 = -82583549;    float KJBwAmJMKVheDBJeYuMfCWrJtufe97839354 = -76048368;    float KJBwAmJMKVheDBJeYuMfCWrJtufe74482588 = -358081007;    float KJBwAmJMKVheDBJeYuMfCWrJtufe39223125 = -642410926;    float KJBwAmJMKVheDBJeYuMfCWrJtufe4170383 = -139190943;    float KJBwAmJMKVheDBJeYuMfCWrJtufe67623153 = -572423564;    float KJBwAmJMKVheDBJeYuMfCWrJtufe11658127 = -438257879;    float KJBwAmJMKVheDBJeYuMfCWrJtufe72246965 = -784939645;    float KJBwAmJMKVheDBJeYuMfCWrJtufe80091968 = 87652140;    float KJBwAmJMKVheDBJeYuMfCWrJtufe84883271 = -453971305;    float KJBwAmJMKVheDBJeYuMfCWrJtufe51252097 = -718780166;    float KJBwAmJMKVheDBJeYuMfCWrJtufe86929044 = -449367131;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28918411 = -901099852;    float KJBwAmJMKVheDBJeYuMfCWrJtufe28482904 = -337613050;    float KJBwAmJMKVheDBJeYuMfCWrJtufe16829933 = -298560905;     KJBwAmJMKVheDBJeYuMfCWrJtufe91007094 = KJBwAmJMKVheDBJeYuMfCWrJtufe74300308;     KJBwAmJMKVheDBJeYuMfCWrJtufe74300308 = KJBwAmJMKVheDBJeYuMfCWrJtufe85675270;     KJBwAmJMKVheDBJeYuMfCWrJtufe85675270 = KJBwAmJMKVheDBJeYuMfCWrJtufe32762073;     KJBwAmJMKVheDBJeYuMfCWrJtufe32762073 = KJBwAmJMKVheDBJeYuMfCWrJtufe55730761;     KJBwAmJMKVheDBJeYuMfCWrJtufe55730761 = KJBwAmJMKVheDBJeYuMfCWrJtufe36668352;     KJBwAmJMKVheDBJeYuMfCWrJtufe36668352 = KJBwAmJMKVheDBJeYuMfCWrJtufe84641099;     KJBwAmJMKVheDBJeYuMfCWrJtufe84641099 = KJBwAmJMKVheDBJeYuMfCWrJtufe53981540;     KJBwAmJMKVheDBJeYuMfCWrJtufe53981540 = KJBwAmJMKVheDBJeYuMfCWrJtufe28354199;     KJBwAmJMKVheDBJeYuMfCWrJtufe28354199 = KJBwAmJMKVheDBJeYuMfCWrJtufe93505593;     KJBwAmJMKVheDBJeYuMfCWrJtufe93505593 = KJBwAmJMKVheDBJeYuMfCWrJtufe31170252;     KJBwAmJMKVheDBJeYuMfCWrJtufe31170252 = KJBwAmJMKVheDBJeYuMfCWrJtufe53745105;     KJBwAmJMKVheDBJeYuMfCWrJtufe53745105 = KJBwAmJMKVheDBJeYuMfCWrJtufe1234533;     KJBwAmJMKVheDBJeYuMfCWrJtufe1234533 = KJBwAmJMKVheDBJeYuMfCWrJtufe62983219;     KJBwAmJMKVheDBJeYuMfCWrJtufe62983219 = KJBwAmJMKVheDBJeYuMfCWrJtufe50396102;     KJBwAmJMKVheDBJeYuMfCWrJtufe50396102 = KJBwAmJMKVheDBJeYuMfCWrJtufe11183674;     KJBwAmJMKVheDBJeYuMfCWrJtufe11183674 = KJBwAmJMKVheDBJeYuMfCWrJtufe58419955;     KJBwAmJMKVheDBJeYuMfCWrJtufe58419955 = KJBwAmJMKVheDBJeYuMfCWrJtufe40393910;     KJBwAmJMKVheDBJeYuMfCWrJtufe40393910 = KJBwAmJMKVheDBJeYuMfCWrJtufe9479218;     KJBwAmJMKVheDBJeYuMfCWrJtufe9479218 = KJBwAmJMKVheDBJeYuMfCWrJtufe7947768;     KJBwAmJMKVheDBJeYuMfCWrJtufe7947768 = KJBwAmJMKVheDBJeYuMfCWrJtufe47227749;     KJBwAmJMKVheDBJeYuMfCWrJtufe47227749 = KJBwAmJMKVheDBJeYuMfCWrJtufe45162816;     KJBwAmJMKVheDBJeYuMfCWrJtufe45162816 = KJBwAmJMKVheDBJeYuMfCWrJtufe5919297;     KJBwAmJMKVheDBJeYuMfCWrJtufe5919297 = KJBwAmJMKVheDBJeYuMfCWrJtufe80837249;     KJBwAmJMKVheDBJeYuMfCWrJtufe80837249 = KJBwAmJMKVheDBJeYuMfCWrJtufe44039529;     KJBwAmJMKVheDBJeYuMfCWrJtufe44039529 = KJBwAmJMKVheDBJeYuMfCWrJtufe51958644;     KJBwAmJMKVheDBJeYuMfCWrJtufe51958644 = KJBwAmJMKVheDBJeYuMfCWrJtufe56073750;     KJBwAmJMKVheDBJeYuMfCWrJtufe56073750 = KJBwAmJMKVheDBJeYuMfCWrJtufe28912421;     KJBwAmJMKVheDBJeYuMfCWrJtufe28912421 = KJBwAmJMKVheDBJeYuMfCWrJtufe41322173;     KJBwAmJMKVheDBJeYuMfCWrJtufe41322173 = KJBwAmJMKVheDBJeYuMfCWrJtufe96850670;     KJBwAmJMKVheDBJeYuMfCWrJtufe96850670 = KJBwAmJMKVheDBJeYuMfCWrJtufe75137501;     KJBwAmJMKVheDBJeYuMfCWrJtufe75137501 = KJBwAmJMKVheDBJeYuMfCWrJtufe95290646;     KJBwAmJMKVheDBJeYuMfCWrJtufe95290646 = KJBwAmJMKVheDBJeYuMfCWrJtufe83052860;     KJBwAmJMKVheDBJeYuMfCWrJtufe83052860 = KJBwAmJMKVheDBJeYuMfCWrJtufe6720674;     KJBwAmJMKVheDBJeYuMfCWrJtufe6720674 = KJBwAmJMKVheDBJeYuMfCWrJtufe50014662;     KJBwAmJMKVheDBJeYuMfCWrJtufe50014662 = KJBwAmJMKVheDBJeYuMfCWrJtufe36004165;     KJBwAmJMKVheDBJeYuMfCWrJtufe36004165 = KJBwAmJMKVheDBJeYuMfCWrJtufe16496148;     KJBwAmJMKVheDBJeYuMfCWrJtufe16496148 = KJBwAmJMKVheDBJeYuMfCWrJtufe60969509;     KJBwAmJMKVheDBJeYuMfCWrJtufe60969509 = KJBwAmJMKVheDBJeYuMfCWrJtufe83864085;     KJBwAmJMKVheDBJeYuMfCWrJtufe83864085 = KJBwAmJMKVheDBJeYuMfCWrJtufe65477873;     KJBwAmJMKVheDBJeYuMfCWrJtufe65477873 = KJBwAmJMKVheDBJeYuMfCWrJtufe6980776;     KJBwAmJMKVheDBJeYuMfCWrJtufe6980776 = KJBwAmJMKVheDBJeYuMfCWrJtufe99588411;     KJBwAmJMKVheDBJeYuMfCWrJtufe99588411 = KJBwAmJMKVheDBJeYuMfCWrJtufe60421939;     KJBwAmJMKVheDBJeYuMfCWrJtufe60421939 = KJBwAmJMKVheDBJeYuMfCWrJtufe46850996;     KJBwAmJMKVheDBJeYuMfCWrJtufe46850996 = KJBwAmJMKVheDBJeYuMfCWrJtufe52709537;     KJBwAmJMKVheDBJeYuMfCWrJtufe52709537 = KJBwAmJMKVheDBJeYuMfCWrJtufe34459435;     KJBwAmJMKVheDBJeYuMfCWrJtufe34459435 = KJBwAmJMKVheDBJeYuMfCWrJtufe52468837;     KJBwAmJMKVheDBJeYuMfCWrJtufe52468837 = KJBwAmJMKVheDBJeYuMfCWrJtufe79913857;     KJBwAmJMKVheDBJeYuMfCWrJtufe79913857 = KJBwAmJMKVheDBJeYuMfCWrJtufe28304713;     KJBwAmJMKVheDBJeYuMfCWrJtufe28304713 = KJBwAmJMKVheDBJeYuMfCWrJtufe38283053;     KJBwAmJMKVheDBJeYuMfCWrJtufe38283053 = KJBwAmJMKVheDBJeYuMfCWrJtufe22438691;     KJBwAmJMKVheDBJeYuMfCWrJtufe22438691 = KJBwAmJMKVheDBJeYuMfCWrJtufe13023189;     KJBwAmJMKVheDBJeYuMfCWrJtufe13023189 = KJBwAmJMKVheDBJeYuMfCWrJtufe97451291;     KJBwAmJMKVheDBJeYuMfCWrJtufe97451291 = KJBwAmJMKVheDBJeYuMfCWrJtufe8570273;     KJBwAmJMKVheDBJeYuMfCWrJtufe8570273 = KJBwAmJMKVheDBJeYuMfCWrJtufe67497549;     KJBwAmJMKVheDBJeYuMfCWrJtufe67497549 = KJBwAmJMKVheDBJeYuMfCWrJtufe45844279;     KJBwAmJMKVheDBJeYuMfCWrJtufe45844279 = KJBwAmJMKVheDBJeYuMfCWrJtufe68381012;     KJBwAmJMKVheDBJeYuMfCWrJtufe68381012 = KJBwAmJMKVheDBJeYuMfCWrJtufe4838021;     KJBwAmJMKVheDBJeYuMfCWrJtufe4838021 = KJBwAmJMKVheDBJeYuMfCWrJtufe88722543;     KJBwAmJMKVheDBJeYuMfCWrJtufe88722543 = KJBwAmJMKVheDBJeYuMfCWrJtufe3772118;     KJBwAmJMKVheDBJeYuMfCWrJtufe3772118 = KJBwAmJMKVheDBJeYuMfCWrJtufe80594602;     KJBwAmJMKVheDBJeYuMfCWrJtufe80594602 = KJBwAmJMKVheDBJeYuMfCWrJtufe55728678;     KJBwAmJMKVheDBJeYuMfCWrJtufe55728678 = KJBwAmJMKVheDBJeYuMfCWrJtufe12659368;     KJBwAmJMKVheDBJeYuMfCWrJtufe12659368 = KJBwAmJMKVheDBJeYuMfCWrJtufe31503529;     KJBwAmJMKVheDBJeYuMfCWrJtufe31503529 = KJBwAmJMKVheDBJeYuMfCWrJtufe18368092;     KJBwAmJMKVheDBJeYuMfCWrJtufe18368092 = KJBwAmJMKVheDBJeYuMfCWrJtufe35879605;     KJBwAmJMKVheDBJeYuMfCWrJtufe35879605 = KJBwAmJMKVheDBJeYuMfCWrJtufe70692244;     KJBwAmJMKVheDBJeYuMfCWrJtufe70692244 = KJBwAmJMKVheDBJeYuMfCWrJtufe94513858;     KJBwAmJMKVheDBJeYuMfCWrJtufe94513858 = KJBwAmJMKVheDBJeYuMfCWrJtufe12968557;     KJBwAmJMKVheDBJeYuMfCWrJtufe12968557 = KJBwAmJMKVheDBJeYuMfCWrJtufe14391937;     KJBwAmJMKVheDBJeYuMfCWrJtufe14391937 = KJBwAmJMKVheDBJeYuMfCWrJtufe94687526;     KJBwAmJMKVheDBJeYuMfCWrJtufe94687526 = KJBwAmJMKVheDBJeYuMfCWrJtufe97450446;     KJBwAmJMKVheDBJeYuMfCWrJtufe97450446 = KJBwAmJMKVheDBJeYuMfCWrJtufe56529824;     KJBwAmJMKVheDBJeYuMfCWrJtufe56529824 = KJBwAmJMKVheDBJeYuMfCWrJtufe44001344;     KJBwAmJMKVheDBJeYuMfCWrJtufe44001344 = KJBwAmJMKVheDBJeYuMfCWrJtufe966993;     KJBwAmJMKVheDBJeYuMfCWrJtufe966993 = KJBwAmJMKVheDBJeYuMfCWrJtufe47639338;     KJBwAmJMKVheDBJeYuMfCWrJtufe47639338 = KJBwAmJMKVheDBJeYuMfCWrJtufe84740876;     KJBwAmJMKVheDBJeYuMfCWrJtufe84740876 = KJBwAmJMKVheDBJeYuMfCWrJtufe59068300;     KJBwAmJMKVheDBJeYuMfCWrJtufe59068300 = KJBwAmJMKVheDBJeYuMfCWrJtufe28127713;     KJBwAmJMKVheDBJeYuMfCWrJtufe28127713 = KJBwAmJMKVheDBJeYuMfCWrJtufe9580094;     KJBwAmJMKVheDBJeYuMfCWrJtufe9580094 = KJBwAmJMKVheDBJeYuMfCWrJtufe99489807;     KJBwAmJMKVheDBJeYuMfCWrJtufe99489807 = KJBwAmJMKVheDBJeYuMfCWrJtufe76159892;     KJBwAmJMKVheDBJeYuMfCWrJtufe76159892 = KJBwAmJMKVheDBJeYuMfCWrJtufe607709;     KJBwAmJMKVheDBJeYuMfCWrJtufe607709 = KJBwAmJMKVheDBJeYuMfCWrJtufe3039121;     KJBwAmJMKVheDBJeYuMfCWrJtufe3039121 = KJBwAmJMKVheDBJeYuMfCWrJtufe74411980;     KJBwAmJMKVheDBJeYuMfCWrJtufe74411980 = KJBwAmJMKVheDBJeYuMfCWrJtufe62114312;     KJBwAmJMKVheDBJeYuMfCWrJtufe62114312 = KJBwAmJMKVheDBJeYuMfCWrJtufe97839354;     KJBwAmJMKVheDBJeYuMfCWrJtufe97839354 = KJBwAmJMKVheDBJeYuMfCWrJtufe74482588;     KJBwAmJMKVheDBJeYuMfCWrJtufe74482588 = KJBwAmJMKVheDBJeYuMfCWrJtufe39223125;     KJBwAmJMKVheDBJeYuMfCWrJtufe39223125 = KJBwAmJMKVheDBJeYuMfCWrJtufe4170383;     KJBwAmJMKVheDBJeYuMfCWrJtufe4170383 = KJBwAmJMKVheDBJeYuMfCWrJtufe67623153;     KJBwAmJMKVheDBJeYuMfCWrJtufe67623153 = KJBwAmJMKVheDBJeYuMfCWrJtufe11658127;     KJBwAmJMKVheDBJeYuMfCWrJtufe11658127 = KJBwAmJMKVheDBJeYuMfCWrJtufe72246965;     KJBwAmJMKVheDBJeYuMfCWrJtufe72246965 = KJBwAmJMKVheDBJeYuMfCWrJtufe80091968;     KJBwAmJMKVheDBJeYuMfCWrJtufe80091968 = KJBwAmJMKVheDBJeYuMfCWrJtufe84883271;     KJBwAmJMKVheDBJeYuMfCWrJtufe84883271 = KJBwAmJMKVheDBJeYuMfCWrJtufe51252097;     KJBwAmJMKVheDBJeYuMfCWrJtufe51252097 = KJBwAmJMKVheDBJeYuMfCWrJtufe86929044;     KJBwAmJMKVheDBJeYuMfCWrJtufe86929044 = KJBwAmJMKVheDBJeYuMfCWrJtufe28918411;     KJBwAmJMKVheDBJeYuMfCWrJtufe28918411 = KJBwAmJMKVheDBJeYuMfCWrJtufe28482904;     KJBwAmJMKVheDBJeYuMfCWrJtufe28482904 = KJBwAmJMKVheDBJeYuMfCWrJtufe16829933;     KJBwAmJMKVheDBJeYuMfCWrJtufe16829933 = KJBwAmJMKVheDBJeYuMfCWrJtufe91007094;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void oDPwZsijOPswSjZSuYSoPysyI80964581() {     long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86981320 = -837041218;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25326044 = -24145525;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35944635 = -867857978;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52289839 = -696765846;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew68837621 = -732262015;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13651596 = -842147064;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58923484 = -392232883;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58871883 = -883450097;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17513937 = -996731555;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86824723 = -399654588;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65898063 = -60165643;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52389929 = -765846143;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79278128 = -602149977;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82382406 = -681701658;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95038436 = -295509752;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew93497254 = -588109577;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42252868 = -152923440;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15159066 = -472603898;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25362021 = -881628675;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90942004 = -682498833;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4994874 = -835829933;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew91997664 = -729613623;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4454491 = -758401284;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew88636353 = -400424652;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86288126 = -149036130;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53071533 = -488210200;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79586361 = -766021836;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17623442 = -222421211;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew40812130 = -934391481;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59932061 = -273207695;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24126758 = -385643556;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew55099778 = -68601964;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17981738 = -368455328;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66642159 = -511136593;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11429606 = -755238285;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew69274818 = -306917757;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47139595 = -69314870;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew43491919 = -596010244;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew80118086 = -496476796;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew76731422 = -642823637;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83085009 = -135590669;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83602020 = -61938833;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82453495 = -895164715;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4869652 = -602948459;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77496255 = -658928906;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew49222626 = -701717936;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew1350014 = -299705575;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82965711 = -465906072;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53330359 = -996618267;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70697476 = -942562357;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13111216 = -287441926;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90292535 = -806431168;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59226045 = -844783505;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew8154126 = -853612615;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew75969708 = -430492398;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew94983656 = -7427595;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew20871553 = -265744242;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47308282 = -367433326;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66001713 = -447729716;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15766088 = -144051816;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34065234 = 23874772;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41300043 = -69811673;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew18059754 = -949058617;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew57581875 = -623523861;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew62697965 = 85988967;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew10798286 = -991563680;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34408192 = -297390816;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew12635970 = 8986616;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70952800 = -926463374;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25763618 = -988591995;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46357659 = -418794707;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew98760948 = -556913196;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35040979 = -976127103;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48630599 = -138805038;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew7856995 = -446908164;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew21392853 = -673891101;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9544169 = -834448909;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99584838 = -55452826;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11140098 = -741495746;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew37065500 = -447318195;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew51721520 = -88504626;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew96620650 = -200115764;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64293083 = -225802944;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70114653 = -991829125;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46820846 = -985765769;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew33834223 = -579212388;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95873733 = -223818459;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9827612 = -514842714;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90672450 = 19355805;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew16445950 = -647810690;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48403266 = 58826484;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99831313 = -701881544;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77490206 = -48280528;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64351998 = -252424980;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42666188 = -566698410;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41784967 = 34221004;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65542267 = -112880217;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24871620 = -171640855;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42171687 = -588937426;    long IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66697970 = -837041218;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86981320 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25326044;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25326044 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35944635;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35944635 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52289839;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52289839 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew68837621;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew68837621 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13651596;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13651596 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58923484;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58923484 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58871883;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew58871883 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17513937;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17513937 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86824723;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86824723 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65898063;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65898063 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52389929;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew52389929 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79278128;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79278128 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82382406;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82382406 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95038436;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95038436 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew93497254;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew93497254 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42252868;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42252868 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15159066;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15159066 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25362021;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25362021 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90942004;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90942004 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4994874;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4994874 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew91997664;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew91997664 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4454491;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4454491 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew88636353;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew88636353 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86288126;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86288126 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53071533;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53071533 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79586361;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew79586361 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17623442;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17623442 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew40812130;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew40812130 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59932061;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59932061 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24126758;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24126758 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew55099778;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew55099778 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17981738;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew17981738 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66642159;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66642159 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11429606;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11429606 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew69274818;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew69274818 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47139595;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47139595 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew43491919;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew43491919 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew80118086;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew80118086 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew76731422;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew76731422 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83085009;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83085009 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83602020;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew83602020 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82453495;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82453495 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4869652;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew4869652 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77496255;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77496255 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew49222626;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew49222626 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew1350014;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew1350014 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82965711;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew82965711 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53330359;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew53330359 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70697476;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70697476 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13111216;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew13111216 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90292535;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90292535 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59226045;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew59226045 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew8154126;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew8154126 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew75969708;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew75969708 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew94983656;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew94983656 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew20871553;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew20871553 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47308282;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew47308282 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66001713;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66001713 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15766088;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew15766088 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34065234;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34065234 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41300043;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41300043 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew18059754;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew18059754 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew57581875;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew57581875 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew62697965;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew62697965 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew10798286;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew10798286 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34408192;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew34408192 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew12635970;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew12635970 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70952800;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70952800 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25763618;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew25763618 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46357659;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46357659 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew98760948;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew98760948 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35040979;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew35040979 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48630599;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48630599 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew7856995;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew7856995 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew21392853;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew21392853 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9544169;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9544169 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99584838;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99584838 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11140098;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew11140098 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew37065500;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew37065500 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew51721520;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew51721520 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew96620650;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew96620650 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64293083;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64293083 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70114653;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew70114653 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46820846;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew46820846 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew33834223;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew33834223 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95873733;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew95873733 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9827612;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew9827612 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90672450;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew90672450 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew16445950;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew16445950 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48403266;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew48403266 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99831313;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew99831313 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77490206;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew77490206 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64351998;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew64351998 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42666188;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42666188 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41784967;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew41784967 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65542267;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew65542267 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24871620;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew24871620 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42171687;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew42171687 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66697970;     IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew66697970 = IGWZvstNSEcKyVZfkEXalBeHloozoQpKRnRmMJTXzizew86981320;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FOgVYFtfWYuEQzlJqZjPnklOwkwyOlQsbuAbL72286414() {     double gTraNjHKzEYglmbLVIUcS94290328 = -134270671;    double gTraNjHKzEYglmbLVIUcS24245453 = -651553385;    double gTraNjHKzEYglmbLVIUcS21359141 = -815050260;    double gTraNjHKzEYglmbLVIUcS50228836 = -775181828;    double gTraNjHKzEYglmbLVIUcS65541589 = -318054841;    double gTraNjHKzEYglmbLVIUcS95792598 = -26735307;    double gTraNjHKzEYglmbLVIUcS20067528 = -256553725;    double gTraNjHKzEYglmbLVIUcS98771963 = -880751886;    double gTraNjHKzEYglmbLVIUcS30415112 = -993797367;    double gTraNjHKzEYglmbLVIUcS33645735 = -132250901;    double gTraNjHKzEYglmbLVIUcS11053204 = -175997596;    double gTraNjHKzEYglmbLVIUcS88916495 = -796395686;    double gTraNjHKzEYglmbLVIUcS99330624 = -2642889;    double gTraNjHKzEYglmbLVIUcS42001041 = -592232636;    double gTraNjHKzEYglmbLVIUcS68417779 = 72977283;    double gTraNjHKzEYglmbLVIUcS97010088 = -125659977;    double gTraNjHKzEYglmbLVIUcS90091029 = -298681235;    double gTraNjHKzEYglmbLVIUcS78775208 = -134407304;    double gTraNjHKzEYglmbLVIUcS21046743 = -649307065;    double gTraNjHKzEYglmbLVIUcS37546043 = 49546188;    double gTraNjHKzEYglmbLVIUcS49135756 = -50698423;    double gTraNjHKzEYglmbLVIUcS20593432 = -778420410;    double gTraNjHKzEYglmbLVIUcS78091826 = -818054244;    double gTraNjHKzEYglmbLVIUcS31990891 = -493099143;    double gTraNjHKzEYglmbLVIUcS11709243 = -650973988;    double gTraNjHKzEYglmbLVIUcS48017705 = -315868082;    double gTraNjHKzEYglmbLVIUcS70156887 = -470770819;    double gTraNjHKzEYglmbLVIUcS51873093 = -985887618;    double gTraNjHKzEYglmbLVIUcS58497279 = -485538129;    double gTraNjHKzEYglmbLVIUcS43427067 = -764447586;    double gTraNjHKzEYglmbLVIUcS18542802 = -123655511;    double gTraNjHKzEYglmbLVIUcS78341972 = -296332352;    double gTraNjHKzEYglmbLVIUcS57359752 = 15293330;    double gTraNjHKzEYglmbLVIUcS2970380 = 79311004;    double gTraNjHKzEYglmbLVIUcS34808021 = -749781169;    double gTraNjHKzEYglmbLVIUcS41420884 = -28661174;    double gTraNjHKzEYglmbLVIUcS62092263 = -677296943;    double gTraNjHKzEYglmbLVIUcS90249439 = -75998162;    double gTraNjHKzEYglmbLVIUcS47508074 = 95483673;    double gTraNjHKzEYglmbLVIUcS99956596 = -267089138;    double gTraNjHKzEYglmbLVIUcS92163310 = -426860201;    double gTraNjHKzEYglmbLVIUcS4871380 = -608726147;    double gTraNjHKzEYglmbLVIUcS21805845 = -880658049;    double gTraNjHKzEYglmbLVIUcS50447683 = -989732163;    double gTraNjHKzEYglmbLVIUcS33291470 = -51780038;    double gTraNjHKzEYglmbLVIUcS10259410 = -155664485;    double gTraNjHKzEYglmbLVIUcS28037210 = -97392955;    double gTraNjHKzEYglmbLVIUcS24006633 = -838739055;    double gTraNjHKzEYglmbLVIUcS47434911 = -643151194;    double gTraNjHKzEYglmbLVIUcS96912839 = 43189712;    double gTraNjHKzEYglmbLVIUcS88320198 = -337661126;    double gTraNjHKzEYglmbLVIUcS98853158 = -55029048;    double gTraNjHKzEYglmbLVIUcS31930884 = -967697131;    double gTraNjHKzEYglmbLVIUcS45946608 = -560753941;    double gTraNjHKzEYglmbLVIUcS23150658 = -842918951;    double gTraNjHKzEYglmbLVIUcS73696896 = -355850262;    double gTraNjHKzEYglmbLVIUcS46153627 = -833499141;    double gTraNjHKzEYglmbLVIUcS89368249 = -221951118;    double gTraNjHKzEYglmbLVIUcS38519593 = -24207841;    double gTraNjHKzEYglmbLVIUcS17523885 = 97813241;    double gTraNjHKzEYglmbLVIUcS25635711 = -555964488;    double gTraNjHKzEYglmbLVIUcS68194434 = -270666107;    double gTraNjHKzEYglmbLVIUcS40274685 = -295213757;    double gTraNjHKzEYglmbLVIUcS86988044 = -129349782;    double gTraNjHKzEYglmbLVIUcS15102933 = 91404609;    double gTraNjHKzEYglmbLVIUcS32711232 = -879665245;    double gTraNjHKzEYglmbLVIUcS31556743 = -711689016;    double gTraNjHKzEYglmbLVIUcS96360244 = 18046107;    double gTraNjHKzEYglmbLVIUcS7193021 = -842451467;    double gTraNjHKzEYglmbLVIUcS26996895 = -898361543;    double gTraNjHKzEYglmbLVIUcS34917825 = -448363035;    double gTraNjHKzEYglmbLVIUcS99841590 = -122683073;    double gTraNjHKzEYglmbLVIUcS31267134 = -129890977;    double gTraNjHKzEYglmbLVIUcS21090146 = -282217927;    double gTraNjHKzEYglmbLVIUcS45382732 = -523593611;    double gTraNjHKzEYglmbLVIUcS44264376 = -441972276;    double gTraNjHKzEYglmbLVIUcS98787586 = -897762361;    double gTraNjHKzEYglmbLVIUcS27644144 = -828322082;    double gTraNjHKzEYglmbLVIUcS98699421 = -341319106;    double gTraNjHKzEYglmbLVIUcS1449833 = -395309504;    double gTraNjHKzEYglmbLVIUcS19980495 = -118475128;    double gTraNjHKzEYglmbLVIUcS46150254 = -632031765;    double gTraNjHKzEYglmbLVIUcS4438183 = -242736425;    double gTraNjHKzEYglmbLVIUcS61584439 = -428727842;    double gTraNjHKzEYglmbLVIUcS55106869 = -326786461;    double gTraNjHKzEYglmbLVIUcS19689643 = 31373536;    double gTraNjHKzEYglmbLVIUcS46411088 = -328635221;    double gTraNjHKzEYglmbLVIUcS11413144 = -423952730;    double gTraNjHKzEYglmbLVIUcS79819721 = -77770046;    double gTraNjHKzEYglmbLVIUcS61111124 = -293930908;    double gTraNjHKzEYglmbLVIUcS95267257 = -195162033;    double gTraNjHKzEYglmbLVIUcS72724014 = -355345825;    double gTraNjHKzEYglmbLVIUcS51729846 = 48209678;    double gTraNjHKzEYglmbLVIUcS29984190 = 97670432;    double gTraNjHKzEYglmbLVIUcS74320885 = -711124650;    double gTraNjHKzEYglmbLVIUcS23968877 = -56194095;    double gTraNjHKzEYglmbLVIUcS64596695 = -213512391;    double gTraNjHKzEYglmbLVIUcS34817801 = -651308268;    double gTraNjHKzEYglmbLVIUcS35344750 = -981136773;    double gTraNjHKzEYglmbLVIUcS580239 = -134270671;     gTraNjHKzEYglmbLVIUcS94290328 = gTraNjHKzEYglmbLVIUcS24245453;     gTraNjHKzEYglmbLVIUcS24245453 = gTraNjHKzEYglmbLVIUcS21359141;     gTraNjHKzEYglmbLVIUcS21359141 = gTraNjHKzEYglmbLVIUcS50228836;     gTraNjHKzEYglmbLVIUcS50228836 = gTraNjHKzEYglmbLVIUcS65541589;     gTraNjHKzEYglmbLVIUcS65541589 = gTraNjHKzEYglmbLVIUcS95792598;     gTraNjHKzEYglmbLVIUcS95792598 = gTraNjHKzEYglmbLVIUcS20067528;     gTraNjHKzEYglmbLVIUcS20067528 = gTraNjHKzEYglmbLVIUcS98771963;     gTraNjHKzEYglmbLVIUcS98771963 = gTraNjHKzEYglmbLVIUcS30415112;     gTraNjHKzEYglmbLVIUcS30415112 = gTraNjHKzEYglmbLVIUcS33645735;     gTraNjHKzEYglmbLVIUcS33645735 = gTraNjHKzEYglmbLVIUcS11053204;     gTraNjHKzEYglmbLVIUcS11053204 = gTraNjHKzEYglmbLVIUcS88916495;     gTraNjHKzEYglmbLVIUcS88916495 = gTraNjHKzEYglmbLVIUcS99330624;     gTraNjHKzEYglmbLVIUcS99330624 = gTraNjHKzEYglmbLVIUcS42001041;     gTraNjHKzEYglmbLVIUcS42001041 = gTraNjHKzEYglmbLVIUcS68417779;     gTraNjHKzEYglmbLVIUcS68417779 = gTraNjHKzEYglmbLVIUcS97010088;     gTraNjHKzEYglmbLVIUcS97010088 = gTraNjHKzEYglmbLVIUcS90091029;     gTraNjHKzEYglmbLVIUcS90091029 = gTraNjHKzEYglmbLVIUcS78775208;     gTraNjHKzEYglmbLVIUcS78775208 = gTraNjHKzEYglmbLVIUcS21046743;     gTraNjHKzEYglmbLVIUcS21046743 = gTraNjHKzEYglmbLVIUcS37546043;     gTraNjHKzEYglmbLVIUcS37546043 = gTraNjHKzEYglmbLVIUcS49135756;     gTraNjHKzEYglmbLVIUcS49135756 = gTraNjHKzEYglmbLVIUcS20593432;     gTraNjHKzEYglmbLVIUcS20593432 = gTraNjHKzEYglmbLVIUcS78091826;     gTraNjHKzEYglmbLVIUcS78091826 = gTraNjHKzEYglmbLVIUcS31990891;     gTraNjHKzEYglmbLVIUcS31990891 = gTraNjHKzEYglmbLVIUcS11709243;     gTraNjHKzEYglmbLVIUcS11709243 = gTraNjHKzEYglmbLVIUcS48017705;     gTraNjHKzEYglmbLVIUcS48017705 = gTraNjHKzEYglmbLVIUcS70156887;     gTraNjHKzEYglmbLVIUcS70156887 = gTraNjHKzEYglmbLVIUcS51873093;     gTraNjHKzEYglmbLVIUcS51873093 = gTraNjHKzEYglmbLVIUcS58497279;     gTraNjHKzEYglmbLVIUcS58497279 = gTraNjHKzEYglmbLVIUcS43427067;     gTraNjHKzEYglmbLVIUcS43427067 = gTraNjHKzEYglmbLVIUcS18542802;     gTraNjHKzEYglmbLVIUcS18542802 = gTraNjHKzEYglmbLVIUcS78341972;     gTraNjHKzEYglmbLVIUcS78341972 = gTraNjHKzEYglmbLVIUcS57359752;     gTraNjHKzEYglmbLVIUcS57359752 = gTraNjHKzEYglmbLVIUcS2970380;     gTraNjHKzEYglmbLVIUcS2970380 = gTraNjHKzEYglmbLVIUcS34808021;     gTraNjHKzEYglmbLVIUcS34808021 = gTraNjHKzEYglmbLVIUcS41420884;     gTraNjHKzEYglmbLVIUcS41420884 = gTraNjHKzEYglmbLVIUcS62092263;     gTraNjHKzEYglmbLVIUcS62092263 = gTraNjHKzEYglmbLVIUcS90249439;     gTraNjHKzEYglmbLVIUcS90249439 = gTraNjHKzEYglmbLVIUcS47508074;     gTraNjHKzEYglmbLVIUcS47508074 = gTraNjHKzEYglmbLVIUcS99956596;     gTraNjHKzEYglmbLVIUcS99956596 = gTraNjHKzEYglmbLVIUcS92163310;     gTraNjHKzEYglmbLVIUcS92163310 = gTraNjHKzEYglmbLVIUcS4871380;     gTraNjHKzEYglmbLVIUcS4871380 = gTraNjHKzEYglmbLVIUcS21805845;     gTraNjHKzEYglmbLVIUcS21805845 = gTraNjHKzEYglmbLVIUcS50447683;     gTraNjHKzEYglmbLVIUcS50447683 = gTraNjHKzEYglmbLVIUcS33291470;     gTraNjHKzEYglmbLVIUcS33291470 = gTraNjHKzEYglmbLVIUcS10259410;     gTraNjHKzEYglmbLVIUcS10259410 = gTraNjHKzEYglmbLVIUcS28037210;     gTraNjHKzEYglmbLVIUcS28037210 = gTraNjHKzEYglmbLVIUcS24006633;     gTraNjHKzEYglmbLVIUcS24006633 = gTraNjHKzEYglmbLVIUcS47434911;     gTraNjHKzEYglmbLVIUcS47434911 = gTraNjHKzEYglmbLVIUcS96912839;     gTraNjHKzEYglmbLVIUcS96912839 = gTraNjHKzEYglmbLVIUcS88320198;     gTraNjHKzEYglmbLVIUcS88320198 = gTraNjHKzEYglmbLVIUcS98853158;     gTraNjHKzEYglmbLVIUcS98853158 = gTraNjHKzEYglmbLVIUcS31930884;     gTraNjHKzEYglmbLVIUcS31930884 = gTraNjHKzEYglmbLVIUcS45946608;     gTraNjHKzEYglmbLVIUcS45946608 = gTraNjHKzEYglmbLVIUcS23150658;     gTraNjHKzEYglmbLVIUcS23150658 = gTraNjHKzEYglmbLVIUcS73696896;     gTraNjHKzEYglmbLVIUcS73696896 = gTraNjHKzEYglmbLVIUcS46153627;     gTraNjHKzEYglmbLVIUcS46153627 = gTraNjHKzEYglmbLVIUcS89368249;     gTraNjHKzEYglmbLVIUcS89368249 = gTraNjHKzEYglmbLVIUcS38519593;     gTraNjHKzEYglmbLVIUcS38519593 = gTraNjHKzEYglmbLVIUcS17523885;     gTraNjHKzEYglmbLVIUcS17523885 = gTraNjHKzEYglmbLVIUcS25635711;     gTraNjHKzEYglmbLVIUcS25635711 = gTraNjHKzEYglmbLVIUcS68194434;     gTraNjHKzEYglmbLVIUcS68194434 = gTraNjHKzEYglmbLVIUcS40274685;     gTraNjHKzEYglmbLVIUcS40274685 = gTraNjHKzEYglmbLVIUcS86988044;     gTraNjHKzEYglmbLVIUcS86988044 = gTraNjHKzEYglmbLVIUcS15102933;     gTraNjHKzEYglmbLVIUcS15102933 = gTraNjHKzEYglmbLVIUcS32711232;     gTraNjHKzEYglmbLVIUcS32711232 = gTraNjHKzEYglmbLVIUcS31556743;     gTraNjHKzEYglmbLVIUcS31556743 = gTraNjHKzEYglmbLVIUcS96360244;     gTraNjHKzEYglmbLVIUcS96360244 = gTraNjHKzEYglmbLVIUcS7193021;     gTraNjHKzEYglmbLVIUcS7193021 = gTraNjHKzEYglmbLVIUcS26996895;     gTraNjHKzEYglmbLVIUcS26996895 = gTraNjHKzEYglmbLVIUcS34917825;     gTraNjHKzEYglmbLVIUcS34917825 = gTraNjHKzEYglmbLVIUcS99841590;     gTraNjHKzEYglmbLVIUcS99841590 = gTraNjHKzEYglmbLVIUcS31267134;     gTraNjHKzEYglmbLVIUcS31267134 = gTraNjHKzEYglmbLVIUcS21090146;     gTraNjHKzEYglmbLVIUcS21090146 = gTraNjHKzEYglmbLVIUcS45382732;     gTraNjHKzEYglmbLVIUcS45382732 = gTraNjHKzEYglmbLVIUcS44264376;     gTraNjHKzEYglmbLVIUcS44264376 = gTraNjHKzEYglmbLVIUcS98787586;     gTraNjHKzEYglmbLVIUcS98787586 = gTraNjHKzEYglmbLVIUcS27644144;     gTraNjHKzEYglmbLVIUcS27644144 = gTraNjHKzEYglmbLVIUcS98699421;     gTraNjHKzEYglmbLVIUcS98699421 = gTraNjHKzEYglmbLVIUcS1449833;     gTraNjHKzEYglmbLVIUcS1449833 = gTraNjHKzEYglmbLVIUcS19980495;     gTraNjHKzEYglmbLVIUcS19980495 = gTraNjHKzEYglmbLVIUcS46150254;     gTraNjHKzEYglmbLVIUcS46150254 = gTraNjHKzEYglmbLVIUcS4438183;     gTraNjHKzEYglmbLVIUcS4438183 = gTraNjHKzEYglmbLVIUcS61584439;     gTraNjHKzEYglmbLVIUcS61584439 = gTraNjHKzEYglmbLVIUcS55106869;     gTraNjHKzEYglmbLVIUcS55106869 = gTraNjHKzEYglmbLVIUcS19689643;     gTraNjHKzEYglmbLVIUcS19689643 = gTraNjHKzEYglmbLVIUcS46411088;     gTraNjHKzEYglmbLVIUcS46411088 = gTraNjHKzEYglmbLVIUcS11413144;     gTraNjHKzEYglmbLVIUcS11413144 = gTraNjHKzEYglmbLVIUcS79819721;     gTraNjHKzEYglmbLVIUcS79819721 = gTraNjHKzEYglmbLVIUcS61111124;     gTraNjHKzEYglmbLVIUcS61111124 = gTraNjHKzEYglmbLVIUcS95267257;     gTraNjHKzEYglmbLVIUcS95267257 = gTraNjHKzEYglmbLVIUcS72724014;     gTraNjHKzEYglmbLVIUcS72724014 = gTraNjHKzEYglmbLVIUcS51729846;     gTraNjHKzEYglmbLVIUcS51729846 = gTraNjHKzEYglmbLVIUcS29984190;     gTraNjHKzEYglmbLVIUcS29984190 = gTraNjHKzEYglmbLVIUcS74320885;     gTraNjHKzEYglmbLVIUcS74320885 = gTraNjHKzEYglmbLVIUcS23968877;     gTraNjHKzEYglmbLVIUcS23968877 = gTraNjHKzEYglmbLVIUcS64596695;     gTraNjHKzEYglmbLVIUcS64596695 = gTraNjHKzEYglmbLVIUcS34817801;     gTraNjHKzEYglmbLVIUcS34817801 = gTraNjHKzEYglmbLVIUcS35344750;     gTraNjHKzEYglmbLVIUcS35344750 = gTraNjHKzEYglmbLVIUcS580239;     gTraNjHKzEYglmbLVIUcS580239 = gTraNjHKzEYglmbLVIUcS94290328;}
// Junk Finished
