// dear vsonyp0wer, v1.70 WIP
// (internal structures/api)

// You may use this file to debug, understand or extend vsonyp0wer features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define vsonyp0wer_DEFINE_MATH_OPERATORS
// To implement maths operators for ImVec2 (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)
#define vsonyp0wer_DEFINE_MATH_OPERATORS
/*

Index of this file:
// Header mess
// Forward declarations
// STB libraries includes
// Context pointer
// Generic helpers
// Misc data structures
// Main vsonyp0wer context
// Tab bar, tab item
// Internal API

*/

#pragma once

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#ifndef vsonyp0wer_VERSION
#error Must include vsonyp0wer.h before vsonyp0wer_internal.h
#endif

#include <stdio.h>      // FILE*
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251) // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when vsonyp0wer_API is set to__declspec(dllexport)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#endif

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

struct ImRect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct vsonyp0werColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct vsonyp0werColumnData;             // Stohnly data for a single column
struct vsonyp0werColumns;                // Stohnly data for a columns set
struct vsonyp0werContext;                // Main vsonyp0wer context
struct vsonyp0werGroupData;              // Stacked stohnly data for BeginGroup()/EndGroup()
struct vsonyp0werInputTextState;         // Internal state of the currently focused/edited text input box
struct vsonyp0werItemHoveredDataBackup;  // Backup and restore IsItemHovered() internal data
struct vsonyp0werMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct vsonyp0werNavMoveResult;          // Result of a directional navigation move query result
struct vsonyp0werNextWindowData;         // Stohnly for SetNexWindow** functions
struct vsonyp0werPopupRef;               // Stohnly for current popup stack
struct vsonyp0werSettingsHandler;        // Stohnly for one type registered in the .ini file
struct vsonyp0werStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct vsonyp0werTabBar;                 // Stohnly for a tab bar
struct vsonyp0werTabItem;                // Stohnly for a tab item (within a tab bar)
struct vsonyp0werWindow;                 // Stohnly for one window
struct vsonyp0werWindowTempData;         // Temporary stohnly for one window (that's the data which in theory we could ditch at the end of the frame)
struct vsonyp0werWindowSettings;         // Stohnly for window settings stored in .ini file (we keep one of those even if the actual window wasn't instanced during this session)

// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef int vsonyp0werLayoutType;        // -> enum vsonyp0werLayoutType_        // Enum: Horizontal or vertical
typedef int vsonyp0werButtonFlags;       // -> enum vsonyp0werButtonFlags_       // Flags: for ButtonEx(), ButtonBehavior()
typedef int vsonyp0werDragFlags;         // -> enum vsonyp0werDragFlags_         // Flags: for DragBehavior()
typedef int vsonyp0werItemFlags;         // -> enum vsonyp0werItemFlags_         // Flags: for PushItemFlag()
typedef int vsonyp0werItemStatusFlags;   // -> enum vsonyp0werItemStatusFlags_   // Flags: for DC.LastItemStatusFlags
typedef int vsonyp0werNavHighlightFlags; // -> enum vsonyp0werNavHighlightFlags_ // Flags: for RenderNavHighlight()
typedef int vsonyp0werNavDirSourceFlags; // -> enum vsonyp0werNavDirSourceFlags_ // Flags: for GetNavInputAmount2d()
typedef int vsonyp0werNavMoveFlags;      // -> enum vsonyp0werNavMoveFlags_      // Flags: for navigation requests
typedef int vsonyp0werSeparatorFlags;    // -> enum vsonyp0werSeparatorFlags_    // Flags: for Separator() - internal
typedef int vsonyp0werSliderFlags;       // -> enum vsonyp0werSliderFlags_       // Flags: for SliderBehavior()
typedef int vsonyp0werTextFlags;         // -> enum vsonyp0werTextFlags_         // Flags: for TextEx()

//-------------------------------------------------------------------------
// STB libraries includes
//-------------------------------------------------------------------------

namespace vsdns
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             vsonyp0werInputTextState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   -1.0f
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999
#include "vsdns_textedit.h"

} // namespace vsdns

//-----------------------------------------------------------------------------
// Context pointer
//-----------------------------------------------------------------------------

#ifndef Gvsonyp0wer
extern vsonyp0wer_API vsonyp0werContext* Gvsonyp0wer;  // Current implicit vsonyp0wer context pointer
#endif

//-----------------------------------------------------------------------------
// Generic helpers
//-----------------------------------------------------------------------------

#define IM_PI           3.14159265358979323846f
#ifdef _WIN32
#define IM_NEWLINE      "\r\n"   // Play it nice with Windows users (2018/05 news: Microsoft announced that Notepad will finally display Unix-style carriage returns!)
#else
#define IM_NEWLINE      "\n"
#endif
#define IM_TABSIZE      (4)

#define vsonyp0wer_DEBUG_LOG(_FMT,...)       printf("[%05d] " _FMT, Gvsonyp0wer->FrameCount, __VA_ARGS__)
#define IM_STATIC_ASSERT(_COND)         typedef char static_assertion_##__line__[(_COND)?1:-1]
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifdef _MSC_VER
#define vsonyp0wer_CDECL __cdecl
#else
#define vsonyp0wer_CDECL
#endif

// Helpers: UTF-8 <> wchar
vsonyp0wer_API int           ImTextStrToUtf8(char* buf, int buf_size, const ImWchar * in_text, const ImWchar * in_text_end);      // return output UTF-8 bytes count
vsonyp0wer_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);          // read one character. return input UTF-8 bytes count
vsonyp0wer_API int           ImTextStrFromUtf8(ImWchar * buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
vsonyp0wer_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                            // return number of UTF-8 code-points (NOT bytes count)
vsonyp0wer_API int           ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end);                        // return number of bytes to express one char in UTF-8
vsonyp0wer_API int           ImTextCountUtf8BytesFromStr(const ImWchar * in_text, const ImWchar * in_text_end);                   // return number of bytes to express string in UTF-8

// Helpers: Misc
vsonyp0wer_API ImU32         ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
vsonyp0wer_API ImU32         ImHashStr(const char* data, size_t data_size, ImU32 seed = 0);
vsonyp0wer_API void* ImFileLoadToMemory(const char* filename, const char* file_open_mode, size_t * out_file_size = NULL, int padding_bytes = 0);
vsonyp0wer_API FILE * ImFileOpen(const char* filename, const char* file_open_mode);
static inline bool      ImCharIsBlankA(char c) { return c == ' ' || c == '\t'; }
static inline bool      ImCharIsBlankW(unsigned int c) { return c == ' ' || c == '\t' || c == 0x3000; }
static inline bool      ImIsPowerOfTwo(int v) { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }
#define ImQsort         qsort
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
static inline ImU32     ImHash(const void* data, int size, ImU32 seed = 0) { return size ? ImHashData(data, (size_t)size, seed) : ImHashStr((const char*)data, 0, seed); } // [moved to ImHashStr/ImHashData in 1.68]
#endif

// Helpers: Geometry
vsonyp0wer_API ImVec2        ImLineClosestPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & p);
vsonyp0wer_API bool          ImTriangleContainsPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p);
vsonyp0wer_API ImVec2        ImTriangleClosestPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p);
vsonyp0wer_API void          ImTriangleBarycentricCoords(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p, float& out_u, float& out_v, float& out_w);
vsonyp0wer_API vsonyp0werDir      ImGetDirQuadrantFromDelta(float dx, float dy);

// Helpers: String
vsonyp0wer_API int           ImStricmp(const char* str1, const char* str2);
vsonyp0wer_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
vsonyp0wer_API void          ImStrncpy(char* dst, const char* src, size_t count);
vsonyp0wer_API char* ImStrdup(const char* str);
vsonyp0wer_API char* ImStrdupcpy(char* dst, size_t * p_dst_size, const char* str);
vsonyp0wer_API const char* ImStrchrRange(const char* str_begin, const char* str_end, char c);
vsonyp0wer_API int           ImStrlenW(const ImWchar * str);
vsonyp0wer_API const char* ImStreolRange(const char* str, const char* str_end);                // End end-of-line
vsonyp0wer_API const ImWchar* ImStrbolW(const ImWchar * buf_mid_line, const ImWchar * buf_begin);   // Find beginning-of-line
vsonyp0wer_API const char* ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
vsonyp0wer_API void          ImStrTrimBlanks(char* str);
vsonyp0wer_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
vsonyp0wer_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);
vsonyp0wer_API const char* ImParseFormatFindStart(const char* format);
vsonyp0wer_API const char* ImParseFormatFindEnd(const char* format);
vsonyp0wer_API const char* ImParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
vsonyp0wer_API int           ImParseFormatPrecision(const char* format, int default_value);

// Helpers: ImVec2/ImVec4 operators
// We are keeping those disabled by default so they don't leak in user space, to allow user enabling implicit cast operators between ImVec2 and their own types (using IM_VEC2_CLASS_EXTRA etc.)
// We unfortunately don't have a unary- operator for ImVec2 because this would needs to be defined inside the class itself.
#ifdef vsonyp0wer_DEFINE_MATH_OPERATORS
static inline ImVec2 operator*(const ImVec2 & lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2 & lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator+=(ImVec2 & lhs, const ImVec2 & rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2 & lhs, const ImVec2 & rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2 & lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2 & lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec4 operator+(const ImVec4 & lhs, const ImVec4 & rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4 & lhs, const ImVec4 & rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4 & lhs, const ImVec4 & rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
#endif

// Helpers: Maths
// - Wrapper for standard libs functions. (Note that vsonyp0wer_demo.cpp does _not_ use them to keep the code easy to copy)
#ifndef vsonyp0wer_DISABLE_MATH_FUNCTIONS
static inline float  ImFabs(float x) { return fabsf(x); }
static inline float  ImSqrt(float x) { return sqrtf(x); }
static inline float  ImPow(float x, float y) { return powf(x, y); }
static inline double ImPow(double x, double y) { return pow(x, y); }
static inline float  ImFmod(float x, float y) { return fmodf(x, y); }
static inline double ImFmod(double x, double y) { return fmod(x, y); }
static inline float  ImCos(float x) { return cosf(x); }
static inline float  ImSin(float x) { return sinf(x); }
static inline float  ImAcos(float x) { return acosf(x); }
static inline float  ImAtan2(float y, float x) { return atan2f(y, x); }
static inline double ImAtof(const char* s) { return atof(s); }
static inline float  ImFloorStd(float x) { return floorf(x); }   // we already uses our own ImFloor() { return (float)(int)v } internally so the standard one wrapper is named differently (it's used by stb_truetype)
static inline float  ImCeil(float x) { return ceilf(x); }
#endif
// - ImMin/ImMax/ImClamp/ImLerp/ImSwap are used by widgets which support for variety of types: signed/unsigned int/long long float/double
// (Exceptionally using templates here but we could also redefine them for variety of types)
template<typename T> static inline T ImMin(T lhs, T rhs) { return lhs < rhs ? lhs : rhs; }
template<typename T> static inline T ImMax(T lhs, T rhs) { return lhs >= rhs ? lhs : rhs; }
template<typename T> static inline T ImClamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }
template<typename T> static inline T ImLerp(T a, T b, float t) { return (T)(a + (b - a) * t); }
template<typename T> static inline void ImSwap(T & a, T & b) { T tmp = a; a = b; b = tmp; }
template<typename T> static inline T ImAddClampOverflow(T a, T b, T mn, T mx) { if (b < 0 && (a < mn - b)) return mn; if (b > 0 && (a > mx - b)) return mx; return a + b; }
template<typename T> static inline T ImSubClampOverflow(T a, T b, T mn, T mx) { if (b > 0 && (a < mn + b)) return mn; if (b < 0 && (a > mx + b)) return mx; return a - b; }
// - Misc maths helpers
static inline ImVec2 ImMin(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImClamp(const ImVec2 & v, const ImVec2 & mn, ImVec2 mx) { return ImVec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline ImVec2 ImLerp(const ImVec2 & a, const ImVec2 & b, float t) { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2 & a, const ImVec2 & b, const ImVec2 & t) { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4 & a, const ImVec4 & b, float t) { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const ImVec2 & lhs) { return lhs.x* lhs.x + lhs.y * lhs.y; }
static inline float  ImLengthSqr(const ImVec4 & lhs) { return lhs.x* lhs.x + lhs.y * lhs.y + lhs.z * lhs.z + lhs.w * lhs.w; }
static inline float  ImInvLength(const ImVec2 & lhs, float fail_value) { float d = lhs.x * lhs.x + lhs.y * lhs.y; if (d > 0.0f) return 1.0f / ImSqrt(d); return fail_value; }
static inline float  ImFloor(float f) { return (float)(int)f; }
static inline ImVec2 ImFloor(const ImVec2 & v) { return ImVec2((float)(int)v.x, (float)(int)v.y); }
static inline int    ImModPositive(int a, int b) { return (a + b) % b; }
static inline float  ImDot(const ImVec2 & a, const ImVec2 & b) { return a.x* b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2 & v, float cos_a, float sin_a) { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed) { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2 & lhs, const ImVec2 & rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }

// Helper: ImBoolVector. Store 1-bit per value.
// Note that Resize() currently clears the whole vector.
struct ImBoolVector
{
    ImVector<int>   Stohnly;
    ImBoolVector() { }
    void            Resize(int sz) { Stohnly.resize((sz + 31) >> 5); memset(Stohnly.Data, 0, (size_t)Stohnly.Size * sizeof(Stohnly.Data[0])); }
    void            Clear() { Stohnly.clear(); }
    bool            GetBit(int n) const { int off = (n >> 5); int mask = 1 << (n & 31); return (Stohnly[off] & mask) != 0; }
    void            SetBit(int n, bool v) { int off = (n >> 5); int mask = 1 << (n & 31); if (v) Stohnly[off] |= mask; else Stohnly[off] &= ~mask; }
};

// Helper: ImPool<>. Basic keyed stohnly for contiguous instances, slow/amortized insertion, O(1) indexable, O(Log N) queries by ID over a dense/hot buffer,
// Honor constructor/destructor. Add/remove invalidate all pointers. Indexes have the same lifetime as the associated object.
typedef int ImPoolIdx;
template<typename T>
struct vsonyp0wer_API ImPool
{
    ImVector<T>     Data;       // Contiguous data
    vsonyp0werStohnly    Map;        // ID->Index
    ImPoolIdx       FreeIdx;    // Next free idx to use

    ImPool() { FreeIdx = 0; }
    ~ImPool() { Clear(); }
    T* GetByKey(vsonyp0werID key) { int idx = Map.GetInt(key, -1); return (idx != -1) ? &Data[idx] : NULL; }
    T * GetByIndex(ImPoolIdx n) { return &Data[n]; }
    ImPoolIdx   GetIndex(const T * p) const { IM_ASSERT(p >= Data.Data && p < Data.Data + Data.Size); return (ImPoolIdx)(p - Data.Data); }
    T * GetOrAddByKey(vsonyp0werID key) { int* p_idx = Map.GetIntRef(key, -1); if (*p_idx != -1) return &Data[*p_idx]; *p_idx = FreeIdx; return Add(); }
    bool        Contains(const T * p) const { return (p >= Data.Data && p < Data.Data + Data.Size); }
    void        Clear() { for (int n = 0; n < Map.Data.Size; n++) { int idx = Map.Data[n].val_i; if (idx != -1) Data[idx].~T(); } Map.Clear(); Data.clear(); FreeIdx = 0; }
    T* Add() { int idx = FreeIdx; if (idx == Data.Size) { Data.resize(Data.Size + 1); FreeIdx++; } else { FreeIdx = *(int*)& Data[idx]; } IM_PLACEMENT_NEW(&Data[idx]) T(); return &Data[idx]; }
    void        Remove(vsonyp0werID key, const T * p) { Remove(key, GetIndex(p)); }
    void        Remove(vsonyp0werID key, ImPoolIdx idx) { Data[idx].~T(); *(int*)& Data[idx] = FreeIdx; FreeIdx = idx; Map.SetInt(key, -1); }
    void        Reserve(int capacity) { Data.reserve(capacity); Map.Data.reserve(capacity); }
    int         GetSize() const { return Data.Size; }
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

enum vsonyp0werButtonFlags_
{
    vsonyp0werButtonFlags_None = 0,
    vsonyp0werButtonFlags_Repeat = 1 << 0,   // hold to repeat
    vsonyp0werButtonFlags_PressedOnClickRelease = 1 << 1,   // [Default] return true on click + release on same item
    vsonyp0werButtonFlags_PressedOnClick = 1 << 2,   // return true on click (default requires click+release)
    vsonyp0werButtonFlags_PressedOnRelease = 1 << 3,   // return true on release (default requires click+release)
    vsonyp0werButtonFlags_PressedOnDoubleClick = 1 << 4,   // return true on double-click (default requires click+release)
    vsonyp0werButtonFlags_FlattenChildren = 1 << 5,   // allow interactions even if a child window is overlapping
    vsonyp0werButtonFlags_AllowItemOverlap = 1 << 6,   // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
    vsonyp0werButtonFlags_DontClosePopups = 1 << 7,   // disable automatically closing parent popup on press // [UNUSED]
    vsonyp0werButtonFlags_Disabled = 1 << 8,   // disable interactions
    vsonyp0werButtonFlags_AlignTextBaseLine = 1 << 9,   // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
    vsonyp0werButtonFlags_NoKeyModifiers = 1 << 10,  // disable interaction if a key modifier is held
    vsonyp0werButtonFlags_NoHoldingActiveID = 1 << 11,  // don't set ActiveId while holding the mouse (vsonyp0werButtonFlags_PressedOnClick only)
    vsonyp0werButtonFlags_PressedOnDragDropHold = 1 << 12,  // press when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
    vsonyp0werButtonFlags_NoNavFocus = 1 << 13,  // don't override navigation focus when activated
    vsonyp0werButtonFlags_NoHoveredOnNav = 1 << 14   // don't report as hovered when navigated on
};

enum vsonyp0werSliderFlags_
{
    vsonyp0werSliderFlags_None = 0,
    vsonyp0werSliderFlags_Vertical = 1 << 0
};

enum vsonyp0werDragFlags_
{
    vsonyp0werDragFlags_None = 0,
    vsonyp0werDragFlags_Vertical = 1 << 0
};

enum vsonyp0werColumnsFlags_
{
    // Default: 0
    vsonyp0werColumnsFlags_None = 0,
    vsonyp0werColumnsFlags_NoBorder = 1 << 0,   // Disable column dividers
    vsonyp0werColumnsFlags_NoResize = 1 << 1,   // Disable resizing columns when clicking on the dividers
    vsonyp0werColumnsFlags_NoPreserveWidths = 1 << 2,   // Disable column width preservation when adjusting columns
    vsonyp0werColumnsFlags_NoForceWithinWindow = 1 << 3,   // Disable forcing columns to fit within window
    vsonyp0werColumnsFlags_GrowParentContentsSize = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.
};

enum vsonyp0werSelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of vsonyp0werSelectableFlags_
    vsonyp0werSelectableFlags_NoHoldingActiveID = 1 << 10,
    vsonyp0werSelectableFlags_PressedOnClick = 1 << 11,
    vsonyp0werSelectableFlags_PressedOnRelease = 1 << 12,
    vsonyp0werSelectableFlags_DrawFillAvailWidth = 1 << 13,
    vsonyp0werSelectableFlags_AllowItemOverlap = 1 << 14
};

enum vsonyp0werSeparatorFlags_
{
    vsonyp0werSeparatorFlags_None = 0,
    vsonyp0werSeparatorFlags_Horizontal = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
    vsonyp0werSeparatorFlags_Vertical = 1 << 1
};

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
// This is going to be exposed in vsonyp0wer.h when stabilized enough.
enum vsonyp0werItemFlags_
{
    vsonyp0werItemFlags_NoTabStop = 1 << 0,  // false
    vsonyp0werItemFlags_ButtonRepeat = 1 << 1,  // false    // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
    vsonyp0werItemFlags_Disabled = 1 << 2,  // false    // [BETA] Disable interactions but doesn't affect visuals yet. See github.com/ocornut/vsonyp0wer/issues/211
    vsonyp0werItemFlags_NoNav = 1 << 3,  // false
    vsonyp0werItemFlags_NoNavDefaultFocus = 1 << 4,  // false
    vsonyp0werItemFlags_SelectableDontClosePopup = 1 << 5,  // false    // MenuItem/Selectable() automatically closes current Popup window
    vsonyp0werItemFlags_Default_ = 0
};

// Stohnly for LastItem data
enum vsonyp0werItemStatusFlags_
{
    vsonyp0werItemStatusFlags_None = 0,
    vsonyp0werItemStatusFlags_HoveredRect = 1 << 0,
    vsonyp0werItemStatusFlags_HasDisplayRect = 1 << 1,
    vsonyp0werItemStatusFlags_Edited = 1 << 2,   // Value exposed by item was edited in the current frame (should match the bool return value of most widgets)
    vsonyp0werItemStatusFlags_ToggledSelection = 1 << 3    // Set when Selectable(), TreeNode() reports toggling a selection. We can't report "Selected" because reporting the change allows us to handle clipping with less issues.

#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
    , // [vsonyp0wer-test only]
    vsonyp0werItemStatusFlags_Openable = 1 << 10,  //
    vsonyp0werItemStatusFlags_Opened = 1 << 11,  //
    vsonyp0werItemStatusFlags_Checkable = 1 << 12,  //
    vsonyp0werItemStatusFlags_Checked = 1 << 13   //
#endif
};

enum vsonyp0werTextFlags_
{
    vsonyp0werTextFlags_None = 0,
    vsonyp0werTextFlags_NoWidthForLargeClippedText = 1 << 0
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
// Horizontal/Vertical enums are fixed to 0/1 so they may be used to index ImVec2
enum vsonyp0werLayoutType_
{
    vsonyp0werLayoutType_Horizontal = 0,
    vsonyp0werLayoutType_Vertical = 1
};

enum vsonyp0werLogType
{
    vsonyp0werLogType_None = 0,
    vsonyp0werLogType_TTY,
    vsonyp0werLogType_File,
    vsonyp0werLogType_Buffer,
    vsonyp0werLogType_Clipboard
};

// X/Y enums are fixed to 0/1 so they may be used to index ImVec2
enum vsonyp0werAxis
{
    vsonyp0werAxis_None = -1,
    vsonyp0werAxis_X = 0,
    vsonyp0werAxis_Y = 1
};

enum vsonyp0werPlotType
{
    vsonyp0werPlotType_Lines,
    vsonyp0werPlotType_Histogram
};

enum vsonyp0werInputSource
{
    vsonyp0werInputSource_None = 0,
    vsonyp0werInputSource_Mouse,
    vsonyp0werInputSource_Nav,
    vsonyp0werInputSource_NavKeyboard,   // Only used occasionally for stohnly, not tested/handled by most code
    vsonyp0werInputSource_NavGamepad,    // "
    vsonyp0werInputSource_COUNT
};

// FIXME-NAV: Clarify/expose various repeat delay/rate
enum vsonyp0werInputReadMode
{
    vsonyp0werInputReadMode_Down,
    vsonyp0werInputReadMode_Pressed,
    vsonyp0werInputReadMode_Released,
    vsonyp0werInputReadMode_Repeat,
    vsonyp0werInputReadMode_RepeatSlow,
    vsonyp0werInputReadMode_RepeatFast
};

enum vsonyp0werNavHighlightFlags_
{
    vsonyp0werNavHighlightFlags_None = 0,
    vsonyp0werNavHighlightFlags_TypeDefault = 1 << 0,
    vsonyp0werNavHighlightFlags_TypeThin = 1 << 1,
    vsonyp0werNavHighlightFlags_AlwaysDraw = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
    vsonyp0werNavHighlightFlags_NoRounding = 1 << 3
};

enum vsonyp0werNavDirSourceFlags_
{
    vsonyp0werNavDirSourceFlags_None = 0,
    vsonyp0werNavDirSourceFlags_Keyboard = 1 << 0,
    vsonyp0werNavDirSourceFlags_PadDPad = 1 << 1,
    vsonyp0werNavDirSourceFlags_PadLStick = 1 << 2
};

enum vsonyp0werNavMoveFlags_
{
    vsonyp0werNavMoveFlags_None = 0,
    vsonyp0werNavMoveFlags_LoopX = 1 << 0,   // On failed request, restart from opposite side
    vsonyp0werNavMoveFlags_LoopY = 1 << 1,
    vsonyp0werNavMoveFlags_WrapX = 1 << 2,   // On failed request, request from opposite side one line down (when NavDir==right) or one line up (when NavDir==left)
    vsonyp0werNavMoveFlags_WrapY = 1 << 3,   // This is not super useful for provided for completeness
    vsonyp0werNavMoveFlags_AllowCurrentNavId = 1 << 4,   // Allow scoring and considering the current NavId as a move target candidate. This is used when the move source is offset (e.g. pressing PageDown actually needs to send a Up move request, if we are pressing PageDown from the bottom-most item we need to stay in place)
    vsonyp0werNavMoveFlags_AlsoScoreVisibleSet = 1 << 5    // Store alternate result in NavMoveResultLocalVisibleSet that only comprise elements that are already fully visible.
};

enum vsonyp0werNavForward
{
    vsonyp0werNavForward_None,
    vsonyp0werNavForward_ForwardQueued,
    vsonyp0werNavForward_ForwardActive
};

enum vsonyp0werNavLayer
{
    vsonyp0werNavLayer_Main = 0,    // Main scrolling layer
    vsonyp0werNavLayer_Menu = 1,    // Menu layer (access with Alt/vsonyp0werNavInput_Menu)
    vsonyp0werNavLayer_COUNT
};

enum vsonyp0werPopupPositionPolicy
{
    vsonyp0werPopupPositionPolicy_Default,
    vsonyp0werPopupPositionPolicy_ComboBox
};

// 1D vector (this odd construct is used to facilitate the transition between 1D and 2D, and the maintenance of some branches/patches)
struct ImVec1
{
    float   x;
    ImVec1() { x = 0.0f; }
    ImVec1(float _x) { x = _x; }
};


// 2D axis aligned bounding-box
// NB: we can't rely on ImVec2 math operators being available here
struct vsonyp0wer_API ImRect
{
    ImVec2      Min;    // Upper-left
    ImVec2      Max;    // Lower-right

    ImRect() : Min(FLT_MAX, FLT_MAX), Max(-FLT_MAX, -FLT_MAX) {}
    ImRect(const ImVec2& min, const ImVec2& max) : Min(min), Max(max) {}
    ImRect(const ImVec4& v) : Min(v.x, v.y), Max(v.z, v.w) {}
    ImRect(float x1, float y1, float x2, float y2) : Min(x1, y1), Max(x2, y2) {}

    ImVec2      GetCenter() const { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    ImVec2      GetSize() const { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
    float       GetWidth() const { return Max.x - Min.x; }
    float       GetHeight() const { return Max.y - Min.y; }
    ImVec2      GetTL() const { return Min; }                   // Top-left
    ImVec2      GetTR() const { return ImVec2(Max.x, Min.y); }  // Top-right
    ImVec2      GetBL() const { return ImVec2(Min.x, Max.y); }  // Bottom-left
    ImVec2      GetBR() const { return Max; }                   // Bottom-right
    bool        Contains(const ImVec2 & p) const { return p.x >= Min.x && p.y >= Min.y && p.x < Max.x && p.y < Max.y; }
    bool        Contains(const ImRect & r) const { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
    bool        Overlaps(const ImRect & r) const { return r.Min.y <  Max.y&& r.Max.y >  Min.y&& r.Min.x <  Max.x&& r.Max.x >  Min.x; }
    void        Add(const ImVec2 & p) { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
    void        Add(const ImRect & r) { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
    void        Expand(const float amount) { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
    void        Expand(const ImVec2 & amount) { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Translate(const ImVec2 & d) { Min.x += d.x; Min.y += d.y; Max.x += d.x; Max.y += d.y; }
    void        TranslateX(float dx) { Min.x += dx; Max.x += dx; }
    void        TranslateY(float dy) { Min.y += dy; Max.y += dy; }
    void        ClipWith(const ImRect & r) { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
    void        ClipWithFull(const ImRect & r) { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
    void        Floor() { Min.x = (float)(int)Min.x; Min.y = (float)(int)Min.y; Max.x = (float)(int)Max.x; Max.y = (float)(int)Max.y; }
    bool        IsInverted() const { return Min.x > Max.x || Min.y > Max.y; }
};

// Stacked color modifier, backup of modified data so we can restore it
struct vsonyp0werColorMod
{
    vsonyp0werCol    Col;
    ImVec4      BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct vsonyp0werStyleMod
{
    vsonyp0werStyleVar   VarIdx;
    union { int BackupInt[2]; float BackupFloat[2]; };
    vsonyp0werStyleMod(vsonyp0werStyleVar idx, int v) { VarIdx = idx; BackupInt[0] = v; }
    vsonyp0werStyleMod(vsonyp0werStyleVar idx, float v) { VarIdx = idx; BackupFloat[0] = v; }
    vsonyp0werStyleMod(vsonyp0werStyleVar idx, ImVec2 v) { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Stacked stohnly data for BeginGroup()/EndGroup()
struct vsonyp0werGroupData
{
    ImVec2      BackupCursorPos;
    ImVec2      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    ImVec2      BackupCurrentLineSize;
    float       BackupCurrentLineTextBaseOffset;
    vsonyp0werID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        AdvanceCursor;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and NOT a generic helper.
struct vsonyp0wer_API vsonyp0werMenuColumns
{
    float       Spacing;
    float       Width, NextWidth;
    float       Pos[3], NextWidths[3];

    vsonyp0werMenuColumns();
    void        Update(int count, float spacing, bool clear);
    float       DeclColumns(float w0, float w1, float w2);
    float       CalcExtraSpace(float avail_w);
};

// Internal state of the currently focused/edited text input box
struct vsonyp0wer_API vsonyp0werInputTextState
{
    vsonyp0werID                 ID;                     // widget id owning the text state
    int                     CurLenW, CurLenA;       // we need to maintain our buffer length in both UTF-8 and wchar format. UTF-8 len is valid even if TextA is not.
    ImVector<ImWchar>       TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>          TextA;                  // temporary UTF8 buffer for callbacks and other operations. this is not updated in every code-path! size=capacity.
    ImVector<char>          InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    bool                    TextAIsValid;           // temporary UTF8 buffer is not initially valid before we make the widget active (until then we pull the data from user argument)
    int                     BufCapacityA;           // end-user buffer capacity
    float                   ScrollX;                // horizontal scrolling/offset
    vsdns::STB_TexteditState Stb;                   // state for stb_textedit.h
    float                   CursorAnim;             // timer for cursor blink, reset on every user action so the cursor reappears immediately
    bool                    CursorFollow;           // set when we want scrolling to follow the current cursor position (not always!)
    bool                    SelectedAllMouseLock;   // after a double-click to select all, we ignore further mouse drags to update selection

    // Temporarily set when active
    vsonyp0werInputTextFlags     UserFlags;
    vsonyp0werInputTextCallback  UserCallback;
    void* UserCallbackData;

    vsonyp0werInputTextState() { memset(this, 0, sizeof(*this)); }
    void                ClearFreeMemory() { TextW.clear(); TextA.clear(); InitialTextA.clear(); }
    void                CursorAnimReset() { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
    void                CursorClamp() { Stb.cursor = ImMin(Stb.cursor, CurLenW); Stb.select_start = ImMin(Stb.select_start, CurLenW); Stb.select_end = ImMin(Stb.select_end, CurLenW); }
    bool                HasSelection() const { return Stb.select_start != Stb.select_end; }
    void                ClearSelection() { Stb.select_start = Stb.select_end = Stb.cursor; }
    void                SelectAll() { Stb.select_start = 0; Stb.cursor = Stb.select_end = CurLenW; Stb.has_preferred_x = 0; }
    int                 GetUndoAvailCount() const { return Stb.undostate.undo_point; }
    int                 GetRedoAvailCount() const { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void                OnKeyPressed(int key);      // Cannot be inline because we call in code in stb_textedit.h implementation
};

// Windows data saved in vsonyp0wer.ini file
struct vsonyp0werWindowSettings
{
    char* Name;
    vsonyp0werID     ID;
    ImVec2      Pos;
    ImVec2      Size;
    bool        Collapsed;

    vsonyp0werWindowSettings() { Name = NULL; ID = 0; Pos = Size = ImVec2(0, 0); Collapsed = false; }
};

struct vsonyp0werSettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    vsonyp0werID     TypeHash;       // == ImHashStr(TypeName, 0, 0)
    void* (*ReadOpenFn)(vsonyp0werContext* ctx, vsonyp0werSettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLineFn)(vsonyp0werContext* ctx, vsonyp0werSettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*WriteAllFn)(vsonyp0werContext* ctx, vsonyp0werSettingsHandler* handler, vsonyp0werTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void* UserData;

    vsonyp0werSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

// Stohnly for current popup stack
struct vsonyp0werPopupRef
{
    vsonyp0werID             PopupId;        // Set on OpenPopup()
    vsonyp0werWindow* Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    vsonyp0werWindow* ParentWindow;   // Set on OpenPopup()
    int                 OpenFrameCount; // Set on OpenPopup()
    vsonyp0werID             OpenParentId;   // Set on OpenPopup(), we need this to differentiate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
    ImVec2              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
    ImVec2              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup
};

struct vsonyp0werColumnData
{
    float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
    float               OffsetNormBeforeResize;
    vsonyp0werColumnsFlags   Flags;              // Not exposed
    ImRect              ClipRect;

    vsonyp0werColumnData() { OffsetNorm = OffsetNormBeforeResize = 0.0f; Flags = vsonyp0werColumnsFlags_None; }
};

struct vsonyp0werColumns
{
    vsonyp0werID             ID;
    vsonyp0werColumnsFlags   Flags;
    bool                IsFirstFrame;
    bool                IsBeingResized;
    int                 Current;
    int                 Count;
    float               MinX, MaxX;
    float               LineMinY, LineMaxY;
    float               BackupCursorPosY;       // Backup of CursorPos at the time of BeginColumns()
    float               BackupCursorMaxPosX;    // Backup of CursorMaxPos at the time of BeginColumns()
    ImVector<vsonyp0werColumnData> Columns;

    vsonyp0werColumns() { Clear(); }
    void Clear()
    {
        ID = 0;
        Flags = vsonyp0werColumnsFlags_None;
        IsFirstFrame = false;
        IsBeingResized = false;
        Current = 0;
        Count = 1;
        MinX = MaxX = 0.0f;
        LineMinY = LineMaxY = 0.0f;
        BackupCursorPosY = 0.0f;
        BackupCursorMaxPosX = 0.0f;
        Columns.clear();
    }
};

// Data shared between all ImDrawList instances
struct vsonyp0wer_API ImDrawListSharedData
{
    ImVec2          TexUvWhitePixel;            // UV of white pixel in the atlas
    ImFont* Font;                       // Current/default font (optional, for simplified AddText overload)
    float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
    float           CurveTessellationTol;
    ImVec4          ClipRectFullscreen;         // Value for PushClipRectFullscreen()

    // Const data
    // FIXME: Bake rounded corners fill/borders in atlas
    ImVec2          CircleVtx12[12];

    ImDrawListSharedData();
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

    void Clear() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
    void ClearFreeMemory() { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
    vsonyp0wer_API void FlattenIntoSingleLayer();
};

struct vsonyp0werNavMoveResult
{
    vsonyp0werID       ID;           // Best candidate
    vsonyp0werID       SelectScopeId;// Best candidate window current selectable group ID
    vsonyp0werWindow* Window;       // Best candidate window
    float         DistBox;      // Best candidate box distance to current NavId
    float         DistCenter;   // Best candidate center distance to current NavId
    float         DistAxial;
    ImRect        RectRel;      // Best candidate bounding box in window relative space

    vsonyp0werNavMoveResult() { Clear(); }
    void Clear() { ID = SelectScopeId = 0; Window = NULL; DistBox = DistCenter = DistAxial = FLT_MAX; RectRel = ImRect(); }
};

// Stohnly for SetNexWindow** functions
struct vsonyp0werNextWindowData
{
    vsonyp0werCond               PosCond;
    vsonyp0werCond               SizeCond;
    vsonyp0werCond               ContentSizeCond;
    vsonyp0werCond               CollapsedCond;
    vsonyp0werCond               SizeConstraintCond;
    vsonyp0werCond               FocusCond;
    vsonyp0werCond               BgAlphaCond;
    ImVec2                  PosVal;
    ImVec2                  PosPivotVal;
    ImVec2                  SizeVal;
    ImVec2                  ContentSizeVal;
    bool                    CollapsedVal;
    ImRect                  SizeConstraintRect;
    vsonyp0werSizeCallback       SizeCallback;
    void* SizeCallbackUserData;
    float                   BgAlphaVal;
    ImVec2                  MenuBarOffsetMinVal;                // This is not exposed publicly, so we don't clear it.

    vsonyp0werNextWindowData()
    {
        PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = BgAlphaCond = 0;
        PosVal = PosPivotVal = SizeVal = ImVec2(0.0f, 0.0f);
        ContentSizeVal = ImVec2(0.0f, 0.0f);
        CollapsedVal = false;
        SizeConstraintRect = ImRect();
        SizeCallback = NULL;
        SizeCallbackUserData = NULL;
        BgAlphaVal = FLT_MAX;
        MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
    }

    void    Clear()
    {
        PosCond = SizeCond = ContentSizeCond = CollapsedCond = SizeConstraintCond = FocusCond = BgAlphaCond = 0;
    }
};

//-----------------------------------------------------------------------------
// Tabs
//-----------------------------------------------------------------------------

struct vsonyp0werTabBarSortItem
{
    int             Index;
    float           Width;
};

struct vsonyp0werTabBarRef
{
    vsonyp0werTabBar* Ptr;                    // Either field can be set, not both. Dock node tab bars are loose while BeginTabBar() ones are in a pool.
    int             IndexInMainPool;

    vsonyp0werTabBarRef(vsonyp0werTabBar* ptr) { Ptr = ptr; IndexInMainPool = -1; }
    vsonyp0werTabBarRef(int index_in_main_pool) { Ptr = NULL; IndexInMainPool = index_in_main_pool; }
};

//-----------------------------------------------------------------------------
// Main vsonyp0wer context
//-----------------------------------------------------------------------------

struct vsonyp0werContext
{
    bool                    Initialized;
    bool                    FrameScopeActive;                   // Set by NewFrame(), cleared by EndFrame()
    bool                    FrameScopePushedImplicitWindow;     // Set by NewFrame(), cleared by EndFrame()
    bool                    FontAtlasOwnedByContext;            // Io.Fonts-> is owned by the vsonyp0werContext and will be destructed along with it.
    vsonyp0werIO                 IO;
    vsonyp0werStyle              Style;
    ImFont* Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
    ImDrawListSharedData    DrawListSharedData;

    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    ImVector<vsonyp0werWindow*>  Windows;                            // Windows, sorted in display order, back to front
    ImVector<vsonyp0werWindow*>  WindowsFocusOrder;                  // Windows, sorted in focus order, back to front
    ImVector<vsonyp0werWindow*>  WindowsSortBuffer;
    ImVector<vsonyp0werWindow*>  CurrentWindowStack;
    vsonyp0werStohnly            WindowsById;
    int                     WindowsActiveCount;
    vsonyp0werWindow* CurrentWindow;                      // Being drawn into
    vsonyp0werWindow* HoveredWindow;                      // Will catch mouse inputs
    vsonyp0werWindow* HoveredRootWindow;                  // Will catch mouse inputs (for focus/move only)
    vsonyp0werID                 HoveredId;                          // Hovered widget
    bool                    HoveredIdAllowOverlap;
    vsonyp0werID                 HoveredIdPreviousFrame;
    float                   HoveredIdTimer;                     // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;            // Measure contiguous hovering time where the item has not been active
    vsonyp0werID                 ActiveId;                           // Active widget
    vsonyp0werID                 ActiveIdPreviousFrame;
    vsonyp0werID                 ActiveIdIsAlive;                    // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
    bool                    ActiveIdHasBeenPressed;             // Track whether the active id led to a press (this is to allow changing between PressOnClick and PressOnRelease without pressing twice). Used by range_select branch.
    bool                    ActiveIdHasBeenEdited;              // Was the value associated to the widget Edited over the course of the Active state.
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEdited;
    int                     ActiveIdAllowNavDirFlags;           // Active widget allows using directional navigation (e.g. can activate a button and move away from it)
    int                     ActiveIdBlockNavInputFlags;
    ImVec2                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
    vsonyp0werWindow* ActiveIdWindow;
    vsonyp0werWindow* ActiveIdPreviousFrameWindow;
    vsonyp0werInputSource        ActiveIdSource;                     // Activating with mouse or nav (gamepad/keyboard)
    vsonyp0werID                 LastActiveId;                       // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;                  // Store the last non-zero ActiveId timer since the beginning of activation, useful for animation.
    ImVec2                  LastValidMousePos;
    vsonyp0werWindow* MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actually window that is moved is generally MovingWindow->RootWindow.
    ImVector<vsonyp0werColorMod> ColorModifiers;                     // Stack for PushStyleColor()/PopStyleColor()
    ImVector<vsonyp0werStyleMod> StyleModifiers;                     // Stack for PushStyleVar()/PopStyleVar()
    ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont()
    ImVector<vsonyp0werPopupRef> OpenPopupStack;                     // Which popups are open (persistent)
    ImVector<vsonyp0werPopupRef> BeginPopupStack;                    // Which level of BeginPopup() we are in (reset every frame)
    vsonyp0werNextWindowData     NextWindowData;                     // Stohnly for SetNextWindow** functions
    bool                    NextTreeNodeOpenVal;                // Stohnly for SetNextTreeNode** functions
    vsonyp0werCond               NextTreeNodeOpenCond;

    // Navigation data (for gamepad/keyboard)
    vsonyp0werWindow* NavWindow;                          // Focused window for navigation. Could be called 'FocusWindow'
    vsonyp0werID                 NavId;                              // Focused item for navigation
    vsonyp0werID                 NavActivateId;                      // ~~ (g.ActiveId == 0) && IsNavInputPressed(vsonyp0werNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    vsonyp0werID                 NavActivateDownId;                  // ~~ IsNavInputDown(vsonyp0werNavInput_Activate) ? NavId : 0
    vsonyp0werID                 NavActivatePressedId;               // ~~ IsNavInputPressed(vsonyp0werNavInput_Activate) ? NavId : 0
    vsonyp0werID                 NavInputId;                         // ~~ IsNavInputPressed(vsonyp0werNavInput_Input) ? NavId : 0
    vsonyp0werID                 NavJustTabbedId;                    // Just tabbed to this id.
    vsonyp0werID                 NavJustMovedToId;                   // Just navigated to this id (result of a successfully MoveRequest).
    vsonyp0werID                 NavJustMovedToMultiSelectScopeId;   // Just navigated to this select scope id (result of a successfully MoveRequest).
    vsonyp0werID                 NavNextActivateId;                  // Set by ActivateItem(), queued until next frame.
    vsonyp0werInputSource        NavInputSource;                     // Keyboard or Gamepad mode? THIS WILL ONLY BE None or NavGamepad or NavKeyboard.
    ImRect                  NavScoringRectScreen;               // Rectangle used for scoring, in screen space. Based of window->DC.NavRefRectRel[], modified for directional navigation scoring.
    int                     NavScoringCount;                    // Metrics for debugging
    vsonyp0werWindow* NavWindowingTarget;                 // When selecting a window (holding Menu+FocusPrev/Next, or equivalent of CTRL-TAB) this window is temporarily displayed front-most.
    vsonyp0werWindow* NavWindowingTargetAnim;             // Record of last valid NavWindowingTarget until DimBgRatio and NavWindowingHighlightAlpha becomes 0.0f
    vsonyp0werWindow* NavWindowingList;
    float                   NavWindowingTimer;
    float                   NavWindowingHighlightAlpha;
    bool                    NavWindowingToggleLayer;
    vsonyp0werNavLayer           NavLayer;                           // Layer we are navigating on. For now the system is hard-coded for 0=main contents and 1=menu/title bar, may expose layers later.
    int                     NavIdTabCounter;                    // == NavWindow->DC.FocusIdxTabCounter at time of NavId processing
    bool                    NavIdIsAlive;                       // Nav widget has been seen this frame ~~ NavRefRectRel is valid
    bool                    NavMousePosDirty;                   // When set we will update mouse position if (io.ConfigFlags & vsonyp0werConfigFlags_NavEnableSetMousePos) if set (NB: this not enabled by default)
    bool                    NavDisableHighlight;                // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why NavDisableHighlight isn't always != NavDisableMouseHover)
    bool                    NavDisableMouseHover;               // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is touched again.
    bool                    NavAnyRequest;                      // ~~ NavMoveRequest || NavInitRequest
    bool                    NavInitRequest;                     // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    vsonyp0werID                 NavInitResultId;
    ImRect                  NavInitResultRectRel;
    bool                    NavMoveFromClampedRefRect;          // Set by manual scrolling, if we scroll to a point where NavId isn't visible we reset navigation from visible items
    bool                    NavMoveRequest;                     // Move request for this frame
    vsonyp0werNavMoveFlags       NavMoveRequestFlags;
    vsonyp0werNavForward         NavMoveRequestForward;              // None / ForwardQueued / ForwardActive (this is used to navigate sibling parent menus from a child menu)
    vsonyp0werDir                NavMoveDir, NavMoveDirLast;         // Direction of the move request (left/right/up/down), direction of the previous move request
    vsonyp0werDir                NavMoveClipDir;
    vsonyp0werNavMoveResult      NavMoveResultLocal;                 // Best move request candidate within NavWindow
    vsonyp0werNavMoveResult      NavMoveResultLocalVisibleSet;       // Best move request candidate within NavWindow that are mostly visible (when using vsonyp0werNavMoveFlags_AlsoScoreVisibleSet flag)
    vsonyp0werNavMoveResult      NavMoveResultOther;                 // Best move request candidate within NavWindow's flattened hierarchy (when using vsonyp0werWindowFlags_NavFlattened flag)

    // Tabbing system (older than Nav, active even if Nav is disabled. FIXME-NAV: This needs a redesign!)
    vsonyp0werWindow* FocusRequestCurrWindow;             //
    vsonyp0werWindow* FocusRequestNextWindow;             //
    int                     FocusRequestCurrCounterAll;         // Any item being requested for focus, stored as an index (we on layout to be stable between the frame pressing TAB and the next frame, semi-ouch)
    int                     FocusRequestCurrCounterTab;         // Tab item being requested for focus, stored as an index
    int                     FocusRequestNextCounterAll;         // Stored for next frame
    int                     FocusRequestNextCounterTab;         // "
    bool                    FocusTabPressed;                    //

    // Render
    ImDrawData              DrawData;                           // Main ImDrawData instance to pass render information to the user
    ImDrawDataBuilder       DrawDataBuilder;
    float                   DimBgRatio;                         // 0.0..1.0 animation when fading in a dimming background (for modal window and CTRL+TAB list)
    ImDrawList              BackgroundDrawList;                 // First draw list to be rendered.
    ImDrawList              ForegroundDrawList;                 // Last draw list to be rendered. This is where we the render software mouse cursor (if io.MouseDrawCursor is set) and most debug overlays.
    vsonyp0werMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSourceOrTarget;
    vsonyp0werDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    vsonyp0werPayload            DragDropPayload;
    ImRect                  DragDropTargetRect;
    vsonyp0werID                 DragDropTargetId;
    vsonyp0werDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface;    // Target item surface (we resolve overlapping targets by prioritizing the smaller surface)
    vsonyp0werID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
    vsonyp0werID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
    ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly
    unsigned char           DragDropPayloadBufLocal[8];         // Local buffer for small payloads

    // Tab bars
    ImPool<vsonyp0werTabBar>             TabBars;
    vsonyp0werTabBar* CurrentTabBar;
    ImVector<vsonyp0werTabBarRef>        CurrentTabBarStack;
    ImVector<vsonyp0werTabBarSortItem>   TabSortByWidthBuffer;

    // Widget state
    vsonyp0werInputTextState     InputTextState;
    ImFont                  InputTextPasswordFont;
    vsonyp0werID                 ScalarAsInputTextId;                // Temporary text input when CTRL+clicking on a slider, etc.
    vsonyp0werColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
    ImVec4                  ColorPickerRef;
    bool                    DragCurrentAccumDirty;
    float                   DragCurrentAccum;                   // Accumulator for dragging modification. Always high-precision, not rounded by end-user precision settings
    float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    ImVec2                  ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use stohnly?
    int                     TooltipOverrideCount;
    ImVector<char>          PrivateClipboard;                   // If no custom clipboard handler is defined

    // Range-Select/Multi-Select
    // [This is unused in this branch, but left here to facilitate merging/syncing multiple branches]
    vsonyp0werID                 MultiSelectScopeId;

    // Platform support
    ImVec2                  PlatformImePos;                     // Cursor position request & last passed to the OS Input Method Editor
    ImVec2                  PlatformImeLastPos;

    // Settings
    bool                           SettingsLoaded;
    float                          SettingsDirtyTimer;          // Save .ini Settings to memory when time reaches zero
    vsonyp0werTextBuffer                SettingsIniData;             // In memory .ini settings
    ImVector<vsonyp0werSettingsHandler> SettingsHandlers;            // List of .ini settings handlers
    ImVector<vsonyp0werWindowSettings>  SettingsWindows;             // vsonyp0werWindow .ini settings entries (parsed from the last loaded .ini file and maintained on saving)

    // Logging
    bool                    LogEnabled;
    vsonyp0werLogType            LogType;
    FILE* LogFile;                            // If != NULL log to stdout/ file
    vsonyp0werTextBuffer         LogBuffer;                          // Accumulation buffer when log to clipboard. This is pointer so our Gvsonyp0wer static constructor doesn't call heap allocators.
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;            // Default/stored value for LogDepthMaxExpand if not specified in the LogXXX function call.

    // Misc
    float                   FramerateSecPerFrame[120];          // Calculate estimate of framerate for user over the last 2 seconds.
    int                     FramerateSecPerFrameIdx;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;          // Explicit capture via CaptureKeyboardFromApp()/CaptureMouseFromApp() sets those flags
    int                     WantCaptureKeyboardNextFrame;
    int                     WantTextInputNextFrame;
    char                    TempBuffer[1024 * 3 + 1];               // Temporary text buffer

    vsonyp0werContext(ImFontAtlas* shared_font_atlas) : BackgroundDrawList(NULL), ForegroundDrawList(NULL)
    {
        Initialized = false;
        FrameScopeActive = FrameScopePushedImplicitWindow = false;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        FontAtlasOwnedByContext = shared_font_atlas ? false : true;
        IO.Fonts = shared_font_atlas ? shared_font_atlas : IM_NEW(ImFontAtlas)();

        Time = 0.0f;
        FrameCount = 0;
        FrameCountEnded = FrameCountRendered = -1;
        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredRootWindow = NULL;
        HoveredId = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdPreviousFrame = 0;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdPreviousFrame = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdHasBeenPressed = false;
        ActiveIdHasBeenEdited = false;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEdited = false;
        ActiveIdAllowNavDirFlags = 0x00;
        ActiveIdBlockNavInputFlags = 0x00;
        ActiveIdClickOffset = ImVec2(-1, -1);
        ActiveIdWindow = ActiveIdPreviousFrameWindow = NULL;
        ActiveIdSource = vsonyp0werInputSource_None;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;
        LastValidMousePos = ImVec2(0.0f, 0.0f);
        MovingWindow = NULL;
        NextTreeNodeOpenVal = false;
        NextTreeNodeOpenCond = 0;

        NavWindow = NULL;
        NavId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavInputId = 0;
        NavJustTabbedId = NavJustMovedToId = NavJustMovedToMultiSelectScopeId = NavNextActivateId = 0;
        NavInputSource = vsonyp0werInputSource_None;
        NavScoringRectScreen = ImRect();
        NavScoringCount = 0;
        NavWindowingTarget = NavWindowingTargetAnim = NavWindowingList = NULL;
        NavWindowingTimer = NavWindowingHighlightAlpha = 0.0f;
        NavWindowingToggleLayer = false;
        NavLayer = vsonyp0werNavLayer_Main;
        NavIdTabCounter = INT_MAX;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveFromClampedRefRect = false;
        NavMoveRequest = false;
        NavMoveRequestFlags = 0;
        NavMoveRequestForward = vsonyp0werNavForward_None;
        NavMoveDir = NavMoveDirLast = NavMoveClipDir = vsonyp0werDir_None;

        FocusRequestCurrWindow = FocusRequestNextWindow = NULL;
        FocusRequestCurrCounterAll = FocusRequestCurrCounterTab = INT_MAX;
        FocusRequestNextCounterAll = FocusRequestNextCounterTab = INT_MAX;
        FocusTabPressed = false;

        DimBgRatio = 0.0f;
        BackgroundDrawList._Data = &DrawListSharedData;
        BackgroundDrawList._OwnerName = "##Background"; // Give it a name for debugging
        ForegroundDrawList._Data = &DrawListSharedData;
        ForegroundDrawList._OwnerName = "##Foreground"; // Give it a name for debugging
        MouseCursor = vsonyp0werMouseCursor_Arrow;

        DragDropActive = DragDropWithinSourceOrTarget = false;
        DragDropSourceFlags = 0;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = 0;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
        DragDropAcceptFrameCount = -1;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        CurrentTabBar = NULL;

        ScalarAsInputTextId = 0;
        ColorEditOptions = vsonyp0werColorEditFlags__OptionsDefault;
        DragCurrentAccumDirty = false;
        DragCurrentAccum = 0.0f;
        DragSpeedDefaultRatio = 1.0f / 100.0f;
        ScrollbarClickDeltaToGrabCenter = ImVec2(0.0f, 0.0f);
        TooltipOverrideCount = 0;

        MultiSelectScopeId = 0;

        PlatformImePos = PlatformImeLastPos = ImVec2(FLT_MAX, FLT_MAX);

        SettingsLoaded = false;
        SettingsDirtyTimer = 0.0f;

        LogEnabled = false;
        LogType = vsonyp0werLogType_None;
        LogFile = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
        memset(TempBuffer, 0, sizeof(TempBuffer));
    }
};

//-----------------------------------------------------------------------------
// vsonyp0werWindow
//-----------------------------------------------------------------------------

// Transient per-window data, reset at the beginning of the frame. This used to be called vsonyp0werDrawContext, hence the DC variable name in vsonyp0werWindow.
// FIXME: That's theory, in practice the delimitation between vsonyp0werWindow and vsonyp0werWindowTempData is quite tenuous and could be reconsidered.
struct vsonyp0wer_API vsonyp0werWindowTempData
{
    ImVec2                  CursorPos;
    ImVec2                  CursorPosPrevLine;
    ImVec2                  CursorStartPos;         // Initial position in client area with padding
    ImVec2                  CursorMaxPos;           // Used to implicitly calculate the size of our contents, always growing during the frame. Turned into window->SizeContents at the beginning of next frame
    ImVec2                  CurrentLineSize;
    float                   CurrentLineTextBaseOffset;
    ImVec2                  PrevLineSize;
    float                   PrevLineTextBaseOffset;
    int                     TreeDepth;
    ImU32                   TreeDepthMayJumpToParentOnPop; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31
    vsonyp0werID                 LastItemId;
    vsonyp0werItemStatusFlags    LastItemStatusFlags;
    ImRect                  LastItemRect;           // Interaction rect
    ImRect                  LastItemDisplayRect;    // End-user display rect (only valid if LastItemStatusFlags & vsonyp0werItemStatusFlags_HasDisplayRect)
    vsonyp0werNavLayer           NavLayerCurrent;        // Current layer, 0..31 (we currently only use 0..1)
    int                     NavLayerCurrentMask;    // = (1 << NavLayerCurrent) used by ItemAdd prior to clipping.
    int                     NavLayerActiveMask;     // Which layer have been written to (result from previous frame)
    int                     NavLayerActiveMaskNext; // Which layer have been written to (buffer for current frame)
    bool                    NavHideHighlightOneFrame;
    bool                    NavHasScroll;           // Set when scrolling can be used (ScrollMax > 0.0f)
    bool                    MenuBarAppending;       // FIXME: Remove this
    ImVec2                  MenuBarOffset;          // MenuBarOffset.x is sort of equivalent of a per-layer CursorPos.x, saved/restored as we switch to the menu bar. The only situation when MenuBarOffset.y is > 0 if when (SafeAreaPadding.y > FramePadding.y), often used on TVs.
    ImVector<vsonyp0werWindow*>  ChildWindows;
    vsonyp0werStohnly* StateStohnly;
    vsonyp0werLayoutType         LayoutType;
    vsonyp0werLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()
    int                     FocusCounterAll;        // Counter for focus/tabbing system. Start at -1 and increase as assigned via FocusableItemRegister() (FIXME-NAV: Needs redesign)
    int                     FocusCounterTab;        // (same, but only count widgets which you can Tab through)

    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    vsonyp0werItemFlags          ItemFlags;              // == ItemFlagsStack.back() [empty == vsonyp0werItemFlags_Default]
    float                   ItemWidth;              // == ItemWidthStack.back(). 0.0: default, >0.0: width in pixels, <0.0: align xx pixels to the right of window
    float                   TextWrapPos;            // == TextWrapPosStack.back() [empty == -1.0f]
    ImVector<vsonyp0werItemFlags>ItemFlagsStack;
    ImVector<float>         ItemWidthStack;
    ImVector<float>         TextWrapPosStack;
    ImVector<vsonyp0werGroupData>GroupStack;
    short                   StackSizesBackup[6];    // Store size of various stacks for asserting

    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    ImVec1                  GroupOffset;
    ImVec1                  ColumnsOffset;          // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    vsonyp0werColumns* CurrentColumns;         // Current columns set

    vsonyp0werWindowTempData()
    {
        CursorPos = CursorPosPrevLine = CursorStartPos = CursorMaxPos = ImVec2(0.0f, 0.0f);
        CurrentLineSize = PrevLineSize = ImVec2(0.0f, 0.0f);
        CurrentLineTextBaseOffset = PrevLineTextBaseOffset = 0.0f;
        TreeDepth = 0;
        TreeDepthMayJumpToParentOnPop = 0x00;
        LastItemId = 0;
        LastItemStatusFlags = 0;
        LastItemRect = LastItemDisplayRect = ImRect();
        NavLayerActiveMask = NavLayerActiveMaskNext = 0x00;
        NavLayerCurrent = vsonyp0werNavLayer_Main;
        NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
        NavHideHighlightOneFrame = false;
        NavHasScroll = false;
        MenuBarAppending = false;
        MenuBarOffset = ImVec2(0.0f, 0.0f);
        StateStohnly = NULL;
        LayoutType = ParentLayoutType = vsonyp0werLayoutType_Vertical;
        FocusCounterAll = FocusCounterTab = -1;

        ItemFlags = vsonyp0werItemFlags_Default_;
        ItemWidth = 0.0f;
        TextWrapPos = -1.0f;
        memset(StackSizesBackup, 0, sizeof(StackSizesBackup));

        Indent = ImVec1(0.0f);
        GroupOffset = ImVec1(0.0f);
        ColumnsOffset = ImVec1(0.0f);
        CurrentColumns = NULL;
    }
};

// Stohnly for one window
struct vsonyp0wer_API vsonyp0werWindow
{
    char* Name;
    vsonyp0werID                 ID;                                 // == ImHash(Name)
    vsonyp0werWindowFlags        Flags;                              // See enum vsonyp0werWindowFlags_
    ImVec2                  Pos;                                // Position (always rounded-up to nearest pixel)
    ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    ImVec2                  SizeFull;                           // Size when non collapsed
    ImVec2                  SizeFullAtLastBegin;                // Copy of SizeFull at the end of Begin. This is the reference value we'll use on the next frame to decide if we need scrollbars.
    ImVec2                  SizeContents;                       // Size of contents (== extents reach of the drawing cursor) from previous frame. Include decoration, window title, border, menu, etc.
    ImVec2                  SizeContentsExplicit;               // Size of contents explicitly set by the user via SetNextWindowContentSize()
    ImVec2                  WindowPadding;                      // Window padding at the time of begin.
    float                   WindowRounding;                     // Window rounding at the time of begin.
    float                   WindowBorderSize;                   // Window border size at the time of begin.
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    vsonyp0werID                 MoveId;                             // == window->GetID("#MOVE")
    vsonyp0werID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    ImVec2                  Scroll;
    ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    ImVec2                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    ImVec2                  ScrollbarSizes;                     // Size taken by scrollbars on each axis
    bool                    ScrollbarX, ScrollbarY;
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (== (HiddenFrames*** > 0))
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, otherwise 0-3)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Order within immediate parent window, if we are a child window. Otherwise 0.
    short                   BeginOrderWithinContext;            // Order within entire vsonyp0wer context. This is mostly used for debugging submission order related issues.
    vsonyp0werID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    int                     AutoFitFramesX, AutoFitFramesY;
    bool                    AutoFitOnlyGrows;
    int                     AutoFitChildAxises;
    vsonyp0werDir                AutoPosLastDirection;
    int                     HiddenFramesCanSkipItems;           // Hide the window for N frames
    int                     HiddenFramesCannotSkipItems;        // Hide the window for N frames while allowing items to be submitted so we can measure their size
    vsonyp0werCond               SetWindowPosAllowFlags;             // store acceptable condition flags for SetNextWindowPos() use.
    vsonyp0werCond               SetWindowSizeAllowFlags;            // store acceptable condition flags for SetNextWindowSize() use.
    vsonyp0werCond               SetWindowCollapsedAllowFlags;       // store acceptable condition flags for SetNextWindowCollapsed() use.
    ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
    ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning. ImVec2(0,0) when positioning from top-left corner; ImVec2(0.5f,0.5f) for centering; ImVec2(1,1) for bottom right.

    vsonyp0werWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called vsonyp0werDrawContext, hence the "DC" variable name.
    ImVector<vsonyp0werID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack
    ImRect                  ClipRect;                           // Current clipping rectangle. = DrawList->clip_rect_stack.back(). Scissoring / clipping rectangle. x1, y1, x2, y2.
    ImRect                  OuterRectClipped;                   // = WindowRect just after setup in Begin(). == window->Rect() for root window.
    ImRect                  InnerMainRect, InnerClipRect;
    ImRect                  ContentsRegionRect;                 // FIXME: This is currently confusing/misleading. Maximum visible content position ~~ Pos + (SizeContentsExplicit ? SizeContentsExplicit : Size - ScrollbarSizes) - CursorStartPos, per axis
    int                     LastFrameActive;                    // Last frame number the window was Active.
    float                   ItemWidthDefault;
    vsonyp0werMenuColumns        MenuColumns;                        // Simplified columns stohnly for menu items
    vsonyp0werStohnly            StateStohnly;
    ImVector<vsonyp0werColumns>  ColumnsStohnly;
    float                   FontWindowScale;                    // User scale multiplier per-window
    int                     SettingsIdx;                        // Index into SettingsWindow[] (indices are always valid as we only grow the array from the back)

    ImDrawList* DrawList;                           // == &DrawListInst (for backward compatibility reason with code using vsonyp0wer_internal.h we keep this a pointer)
    ImDrawList              DrawListInst;
    vsonyp0werWindow* ParentWindow;                       // If we are a child _or_ popup window, this is pointing to our parent. Otherwise NULL.
    vsonyp0werWindow* RootWindow;                         // Point to ourself or first ancestor that is not a child window.
    vsonyp0werWindow* RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will display TitleBgActive color when this window is active.
    vsonyp0werWindow* RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    vsonyp0werWindow* NavLastChildNavWindow;              // When going to the menu bar, we remember the child window we came from. (This could probably be made implicit if we kept g.Windows sorted by last focused including child window.)
    vsonyp0werID                 NavLastIds[vsonyp0werNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1)
    ImRect                  NavRectRel[vsonyp0werNavLayer_COUNT];    // Reference rectangle, in window relative space

public:
    vsonyp0werWindow(vsonyp0werContext* context, const char* name);
    ~vsonyp0werWindow();

    vsonyp0werID     GetID(const char* str, const char* str_end = NULL);
    vsonyp0werID     GetID(const void* ptr);
    vsonyp0werID     GetIDNoKeepAlive(const char* str, const char* str_end = NULL);
    vsonyp0werID     GetIDNoKeepAlive(const void* ptr);
    vsonyp0werID     GetIDFromRectangle(const ImRect& r_abs);

    // We don't use g.FontSize because the window may be != g.CurrentWidow.
    ImRect      Rect() const { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    float       CalcFontSize() const { return Gvsonyp0wer->FontBaseSize* FontWindowScale; }
    float       TitleBarHeight() const { return (Flags & vsonyp0werWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + Gvsonyp0wer->Style.FramePadding.y * 2.0f; }
    ImRect      TitleBarRect() const { return ImRect(Pos, ImVec2(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
    float       MenuBarHeight() const { return (Flags & vsonyp0werWindowFlags_MenuBar) ? DC.MenuBarOffset.y + CalcFontSize() + Gvsonyp0wer->Style.FramePadding.y * 2.0f : 0.0f; }
    ImRect      MenuBarRect() const { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

// Backup and restore just enough data to be able to use IsItemHovered() on item A after another B in the same window has overwritten the data.
struct vsonyp0werItemHoveredDataBackup
{
    vsonyp0werID                 LastItemId;
    vsonyp0werItemStatusFlags    LastItemStatusFlags;
    ImRect                  LastItemRect;
    ImRect                  LastItemDisplayRect;

    vsonyp0werItemHoveredDataBackup() { Backup(); }
    void Backup() { vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow; LastItemId = window->DC.LastItemId; LastItemStatusFlags = window->DC.LastItemStatusFlags; LastItemRect = window->DC.LastItemRect; LastItemDisplayRect = window->DC.LastItemDisplayRect; }
    void Restore() const { vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow; window->DC.LastItemId = LastItemId; window->DC.LastItemStatusFlags = LastItemStatusFlags; window->DC.LastItemRect = LastItemRect; window->DC.LastItemDisplayRect = LastItemDisplayRect; }
};

//-----------------------------------------------------------------------------
// Tab bar, tab item
//-----------------------------------------------------------------------------

enum vsonyp0werTabBarFlagsPrivate_
{
    vsonyp0werTabBarFlags_DockNode = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
    vsonyp0werTabBarFlags_IsFocused = 1 << 21,
    vsonyp0werTabBarFlags_SaveSettings = 1 << 22   // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs
};

enum vsonyp0werTabItemFlagsPrivate_
{
    vsonyp0werTabItemFlags_NoCloseButton = 1 << 20   // Store whether p_open is set or not, which we need to recompute WidthContents during layout.
};

// Stohnly for one active tab item (sizeof() 26~32 bytes)
struct vsonyp0werTabItem
{
    vsonyp0werID             ID;
    vsonyp0werTabItemFlags   Flags;
    int                 LastFrameVisible;
    int                 LastFrameSelected;      // This allows us to infer an ordered list of the last activated tabs with little maintenance
    int                 NameOffset;             // When Window==NULL, offset to name within parent vsonyp0werTabBar::TabsNames
    float               Offset;                 // Position relative to beginning of tab
    float               Width;                  // Width currently displayed
    float               WidthContents;          // Width of actual contents, stored during BeginTabItem() call

    vsonyp0werTabItem() { ID = Flags = 0; LastFrameVisible = LastFrameSelected = -1; NameOffset = -1; Offset = Width = WidthContents = 0.0f; }
};

// Stohnly for a tab bar (sizeof() 92~96 bytes)
struct vsonyp0werTabBar
{
    ImVector<vsonyp0werTabItem> Tabs;
    vsonyp0werID             ID;                     // Zero for tab-bars used by docking
    vsonyp0werID             SelectedTabId;          // Selected tab
    vsonyp0werID             NextSelectedTabId;
    vsonyp0werID             VisibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
    int                 CurrFrameVisible;
    int                 PrevFrameVisible;
    ImRect              BarRect;
    float               ContentsHeight;
    float               OffsetMax;              // Distance from BarRect.Min.x, locked during layout
    float               OffsetNextTab;          // Distance from BarRect.Min.x, incremented with each BeginTabItem() call, not used if vsonyp0werTabBarFlags_Reorderable if set.
    float               ScrollingAnim;
    float               ScrollingTarget;
    float               ScrollingTargetDistToVisibility;
    float               ScrollingSpeed;
    vsonyp0werTabBarFlags    Flags;
    vsonyp0werID             ReorderRequestTabId;
    int                 ReorderRequestDir;
    bool                WantLayout;
    bool                VisibleTabWasSubmitted;
    short               LastTabItemIdx;         // For BeginTabItem()/EndTabItem()
    ImVec2              FramePadding;           // style.FramePadding locked at the time of BeginTabBar()
    vsonyp0werTextBuffer     TabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

    vsonyp0werTabBar();
    int                 GetTabOrder(const vsonyp0werTabItem* tab) const { return Tabs.index_from_ptr(tab); }
    const char* GetTabName(const vsonyp0werTabItem* tab) const
    {
        IM_ASSERT(tab->NameOffset != -1 && tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
};

//-----------------------------------------------------------------------------
// Internal API
// No guarantee of forward compatibility here.
//-----------------------------------------------------------------------------

namespace vsonyp0wer
{
    // We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    // If this ever crash because g.CurrentWindow is NULL it means that either
    // - vsonyp0wer::NewFrame() has never been called, which is illegal.
    // - You are calling vsonyp0wer functions after vsonyp0wer::EndFrame()/vsonyp0wer::Render() and before the next vsonyp0wer::NewFrame(), which is also illegal.
    inline    vsonyp0werWindow* GetCurrentWindowRead() { vsonyp0werContext& g = *Gvsonyp0wer; return g.CurrentWindow; }
    inline    vsonyp0werWindow* GetCurrentWindow() { vsonyp0werContext& g = *Gvsonyp0wer; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
    vsonyp0wer_API vsonyp0werWindow* FindWindowByID(vsonyp0werID id);
    vsonyp0wer_API vsonyp0werWindow* FindWindowByName(const char* name);
    vsonyp0wer_API void          FocusWindow(vsonyp0werWindow* window);
    vsonyp0wer_API void          FocusPreviousWindowIgnoringOne(vsonyp0werWindow* ignore_window);
    vsonyp0wer_API void          BringWindowToFocusFront(vsonyp0werWindow* window);
    vsonyp0wer_API void          BringWindowToDisplayFront(vsonyp0werWindow* window);
    vsonyp0wer_API void          BringWindowToDisplayBack(vsonyp0werWindow* window);
    vsonyp0wer_API void          UpdateWindowParentAndRootLinks(vsonyp0werWindow* window, vsonyp0werWindowFlags flags, vsonyp0werWindow* parent_window);
    vsonyp0wer_API ImVec2        CalcWindowExpectedSize(vsonyp0werWindow* window);
    vsonyp0wer_API bool          IsWindowChildOf(vsonyp0werWindow* window, vsonyp0werWindow* potential_parent);
    vsonyp0wer_API bool          IsWindowNavFocusable(vsonyp0werWindow* window);
    vsonyp0wer_API void          SetWindowScrollX(vsonyp0werWindow* window, float new_scroll_x);
    vsonyp0wer_API void          SetWindowScrollY(vsonyp0werWindow* window, float new_scroll_y);
    vsonyp0wer_API float         GetWindowScrollMaxX(vsonyp0werWindow* window);
    vsonyp0wer_API float         GetWindowScrollMaxY(vsonyp0werWindow* window);
    vsonyp0wer_API ImRect        GetWindowAllowedExtentRect(vsonyp0werWindow* window);
    vsonyp0wer_API void          SetWindowPos(vsonyp0werWindow* window, const ImVec2& pos, vsonyp0werCond cond);
    vsonyp0wer_API void          SetWindowSize(vsonyp0werWindow* window, const ImVec2& size, vsonyp0werCond cond);
    vsonyp0wer_API void          SetWindowCollapsed(vsonyp0werWindow* window, bool collapsed, vsonyp0werCond cond);

    vsonyp0wer_API void          SetCurrentFont(ImFont* font);
    inline ImFont* GetDefaultFont() { vsonyp0werContext& g = *Gvsonyp0wer; return g.IO.FontDefault ? g.IO.FontDefault : g.IO.Fonts->Fonts[0]; }

    // Init
    vsonyp0wer_API void          Initialize(vsonyp0werContext* context);
    vsonyp0wer_API void          Shutdown(vsonyp0werContext* context);    // Since 1.60 this is a _private_ function. You can call DestroyContext() to destroy the context created by CreateContext().

    // NewFrame
    vsonyp0wer_API void          UpdateHoveredWindowAndCaptureFlags();
    vsonyp0wer_API void          StartMouseMovingWindow(vsonyp0werWindow* window);
    vsonyp0wer_API void          UpdateMouseMovingWindowNewFrame();
    vsonyp0wer_API void          UpdateMouseMovingWindowEndFrame();

    // Settings
    vsonyp0wer_API void                  MarkIniSettingsDirty();
    vsonyp0wer_API void                  MarkIniSettingsDirty(vsonyp0werWindow* window);
    vsonyp0wer_API vsonyp0werWindowSettings* CreateNewWindowSettings(const char* name);
    vsonyp0wer_API vsonyp0werWindowSettings* FindWindowSettings(vsonyp0werID id);
    vsonyp0wer_API vsonyp0werWindowSettings* FindOrCreateWindowSettings(const char* name);
    vsonyp0wer_API vsonyp0werSettingsHandler* FindSettingsHandler(const char* type_name);

    // Basic Accessors
    inline vsonyp0werID          GetItemID() { vsonyp0werContext& g = *Gvsonyp0wer; return g.CurrentWindow->DC.LastItemId; }
    inline vsonyp0werID          GetActiveID() { vsonyp0werContext& g = *Gvsonyp0wer; return g.ActiveId; }
    inline vsonyp0werID          GetFocusID() { vsonyp0werContext& g = *Gvsonyp0wer; return g.NavId; }
    vsonyp0wer_API void          SetActiveID(vsonyp0werID id, vsonyp0werWindow* window);
    vsonyp0wer_API void          SetFocusID(vsonyp0werID id, vsonyp0werWindow* window);
    vsonyp0wer_API void          ClearActiveID();
    vsonyp0wer_API vsonyp0werID       GetHoveredID();
    vsonyp0wer_API void          SetHoveredID(vsonyp0werID id);
    vsonyp0wer_API void          KeepAliveID(vsonyp0werID id);
    vsonyp0wer_API void          MarkItemEdited(vsonyp0werID id);

    // Basic Helpers for widget code
    vsonyp0wer_API void          ItemSize(const ImVec2& size, float text_offset_y = 0.0f);
    vsonyp0wer_API void          ItemSize(const ImRect& bb, float text_offset_y = 0.0f);
    vsonyp0wer_API bool          ItemAdd(const ImRect& bb, vsonyp0werID id, const ImRect* nav_bb = NULL);
    vsonyp0wer_API bool          ItemHoverable(const ImRect& bb, vsonyp0werID id);
    vsonyp0wer_API bool          IsClippedEx(const ImRect& bb, vsonyp0werID id, bool clip_even_when_logged);
    vsonyp0wer_API bool          FocusableItemRegister(vsonyp0werWindow* window, vsonyp0werID id);   // Return true if focus is requested
    vsonyp0wer_API void          FocusableItemUnregister(vsonyp0werWindow* window);
    vsonyp0wer_API ImVec2        CalcItemSize(ImVec2 size, float default_w, float default_h);
    vsonyp0wer_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
    vsonyp0wer_API void          PushMultiItemsWidths(int components, float width_full = 0.0f);
    vsonyp0wer_API void          PushItemFlag(vsonyp0werItemFlags option, bool enabled);
    vsonyp0wer_API void          PopItemFlag();
    vsonyp0wer_API bool          IsItemToggledSelection();                                           // was the last item selection toggled? (after Selectable(), TreeNode() etc. We only returns toggle _event_ in order to handle clipping correctly)
    vsonyp0wer_API ImVec2        GetContentRegionMaxScreen();

    // Logging/Capture
    vsonyp0wer_API void          LogBegin(vsonyp0werLogType type, int auto_open_depth);   // -> BeginCapture() when we design v2 api, for now stay under the radar by using the old name.
    vsonyp0wer_API void          LogToBuffer(int auto_open_depth = -1);              // Start logging/capturing to internal buffer

    // Popups, Modals, Tooltips
    vsonyp0wer_API void          OpenPopupEx(vsonyp0werID id);
    vsonyp0wer_API void          ClosePopupToLevel(int remaining, bool apply_focus_to_window_under);
    vsonyp0wer_API void          ClosePopupsOverWindow(vsonyp0werWindow* ref_window);
    vsonyp0wer_API bool          IsPopupOpen(vsonyp0werID id); // Test for id within current popup stack level (currently begin-ed into); this doesn't scan the whole popup stack!
    vsonyp0wer_API bool          BeginPopupEx(vsonyp0werID id, vsonyp0werWindowFlags extra_flags);
    vsonyp0wer_API void          BeginTooltipEx(vsonyp0werWindowFlags extra_flags, bool override_previous_tooltip = true);
    vsonyp0wer_API vsonyp0werWindow* GetFrontMostPopupModal();
    vsonyp0wer_API ImVec2        FindBestWindowPosForPopup(vsonyp0werWindow* window);
    vsonyp0wer_API ImVec2        FindBestWindowPosForPopupEx(const ImVec2& ref_pos, const ImVec2& size, vsonyp0werDir* last_dir, const ImRect& r_outer, const ImRect& r_avoid, vsonyp0werPopupPositionPolicy policy = vsonyp0werPopupPositionPolicy_Default);

    // Navigation
    vsonyp0wer_API void          NavInitWindow(vsonyp0werWindow* window, bool force_reinit);
    vsonyp0wer_API bool          NavMoveRequestButNoResultYet();
    vsonyp0wer_API void          NavMoveRequestCancel();
    vsonyp0wer_API void          NavMoveRequestForward(vsonyp0werDir move_dir, vsonyp0werDir clip_dir, const ImRect& bb_rel, vsonyp0werNavMoveFlags move_flags);
    vsonyp0wer_API void          NavMoveRequestTryWrapping(vsonyp0werWindow* window, vsonyp0werNavMoveFlags move_flags);
    vsonyp0wer_API float         GetNavInputAmount(vsonyp0werNavInput n, vsonyp0werInputReadMode mode);
    vsonyp0wer_API ImVec2        GetNavInputAmount2d(vsonyp0werNavDirSourceFlags dir_sources, vsonyp0werInputReadMode mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    vsonyp0wer_API int           CalcTypematicPressedRepeatAmount(float t, float t_prev, float repeat_delay, float repeat_rate);
    vsonyp0wer_API void          ActivateItem(vsonyp0werID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    vsonyp0wer_API void          SetNavID(vsonyp0werID id, int nav_layer);
    vsonyp0wer_API void          SetNavIDWithRectRel(vsonyp0werID id, int nav_layer, const ImRect& rect_rel);

    // Inputs
    inline bool             IsKeyPressedMap(vsonyp0werKey key, bool repeat = true) { const int key_index = Gvsonyp0wer->IO.KeyMap[key]; return (key_index >= 0) ? IsKeyPressed(key_index, repeat) : false; }
    inline bool             IsNavInputDown(vsonyp0werNavInput n) { return Gvsonyp0wer->IO.NavInputs[n] > 0.0f; }
    inline bool             IsNavInputPressed(vsonyp0werNavInput n, vsonyp0werInputReadMode mode) { return GetNavInputAmount(n, mode) > 0.0f; }
    inline bool             IsNavInputPressedAnyOfTwo(vsonyp0werNavInput n1, vsonyp0werNavInput n2, vsonyp0werInputReadMode mode) { return (GetNavInputAmount(n1, mode) + GetNavInputAmount(n2, mode)) > 0.0f; }

    // Drag and Drop
    vsonyp0wer_API bool          BeginDragDropTargetCustom(const ImRect & bb, vsonyp0werID id);
    vsonyp0wer_API void          ClearDragDrop();
    vsonyp0wer_API bool          IsDragDropPayloadBeingAccepted();

    // New Columns API (FIXME-WIP)
    vsonyp0wer_API void          BeginColumns(const char* str_id, int count, vsonyp0werColumnsFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    vsonyp0wer_API void          EndColumns();                                                             // close columns
    vsonyp0wer_API void          PushColumnClipRect(int column_index = -1);
    vsonyp0wer_API vsonyp0werID       GetColumnsID(const char* str_id, int count);
    vsonyp0wer_API vsonyp0werColumns * FindOrCreateColumns(vsonyp0werWindow * window, vsonyp0werID id);

    // Tab Bars
    vsonyp0wer_API bool          BeginTabBarEx(vsonyp0werTabBar * tab_bar, const ImRect & bb, vsonyp0werTabBarFlags flags);
    vsonyp0wer_API vsonyp0werTabItem * TabBarFindTabByID(vsonyp0werTabBar * tab_bar, vsonyp0werID tab_id);
    vsonyp0wer_API void          TabBarRemoveTab(vsonyp0werTabBar * tab_bar, vsonyp0werID tab_id);
    vsonyp0wer_API void          TabBarCloseTab(vsonyp0werTabBar * tab_bar, vsonyp0werTabItem * tab);
    vsonyp0wer_API void          TabBarQueueChangeTabOrder(vsonyp0werTabBar * tab_bar, const vsonyp0werTabItem * tab, int dir);
    vsonyp0wer_API bool          TabItemEx(vsonyp0werTabBar * tab_bar, const char* label, bool* p_open, vsonyp0werTabItemFlags flags);
    vsonyp0wer_API ImVec2        TabItemCalcSize(const char* label, bool has_close_button);
    vsonyp0wer_API void          TabItemBackground(ImDrawList * draw_list, const ImRect & bb, vsonyp0werTabItemFlags flags, ImU32 col);
    vsonyp0wer_API bool          TabItemLabelAndCloseButton(ImDrawList * draw_list, const ImRect & bb, vsonyp0werTabItemFlags flags, ImVec2 frame_padding, const char* label, vsonyp0werID tab_id, vsonyp0werID close_button_id);

    // Render helpers
    // AVOID USING OUTSIDE OF vsonyp0wer.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
    // NB: All position are in absolute pixels coordinates (we are never using window coordinates internally)
    vsonyp0wer_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    vsonyp0wer_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
    vsonyp0wer_API void          RenderTextClipped(const ImVec2 & pos_min, const ImVec2 & pos_max, const char* text, const char* text_end, const ImVec2 * text_size_if_known, const ImVec2 & align = ImVec2(0, 0), const ImRect * clip_rect = NULL);
    vsonyp0wer_API void          RenderTextClippedEx(ImDrawList * draw_list, const ImVec2 & pos_min, const ImVec2 & pos_max, const char* text, const char* text_end, const ImVec2 * text_size_if_known, const ImVec2 & align = ImVec2(0, 0), const ImRect * clip_rect = NULL);
    vsonyp0wer_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    vsonyp0wer_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
    vsonyp0wer_API void          RenderColorRectWithAlphaCheckerboard(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, int rounding_corners_flags = ~0);
    vsonyp0wer_API void          RenderArrow(ImVec2 pos, vsonyp0werDir dir, float scale = 1.0f);
    vsonyp0wer_API void          RenderBullet(ImVec2 pos);
    vsonyp0wer_API void          RenderCheckMark(ImVec2 pos, ImU32 col, float sz);
    vsonyp0wer_API void          RenderNavHighlight(const ImRect & bb, vsonyp0werID id, vsonyp0werNavHighlightFlags flags = vsonyp0werNavHighlightFlags_TypeDefault); // Navigation highlight
    vsonyp0wer_API const char* FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.
    vsonyp0wer_API void          LogRenderedText(const ImVec2 * ref_pos, const char* text, const char* text_end = NULL);

    // Render helpers (those functions don't access any vsonyp0wer state!)
    vsonyp0wer_API void          RenderMouseCursor(ImDrawList * draw_list, ImVec2 pos, float scale, vsonyp0werMouseCursor mouse_cursor = vsonyp0werMouseCursor_Arrow);
    vsonyp0wer_API void          RenderArrowPointingAt(ImDrawList * draw_list, ImVec2 pos, ImVec2 half_sz, vsonyp0werDir direction, ImU32 col);
    vsonyp0wer_API void          RenderRectFilledRangeH(ImDrawList * draw_list, const ImRect & rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    vsonyp0wer_API void          RenderPixelEllipsis(ImDrawList * draw_list, ImVec2 pos, int count, ImU32 col);

    // Widgets
    vsonyp0wer_API void          TextEx(const char* text, const char* text_end = NULL, vsonyp0werTextFlags flags = 0);
    vsonyp0wer_API bool          ButtonEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          ColorBarEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          QuitButtonEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          SelectedTabEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          TabEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          TabSpacerEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          TabSpacer2Ex(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          TabButtonEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool          GroupBoxTitleEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
	vsonyp0wer_API bool			NoInputButtonEx(const char* label, const ImVec2 & size_arg = ImVec2(0, 0), vsonyp0werButtonFlags flags = 0);
    vsonyp0wer_API bool          CloseButton(vsonyp0werID id, const ImVec2 & pos, float radius);
    vsonyp0wer_API bool          CollapseButton(vsonyp0werID id, const ImVec2 & pos);
    vsonyp0wer_API bool          ArrowButtonEx(const char* str_id, vsonyp0werDir dir, ImVec2 size_arg, vsonyp0werButtonFlags flags);
    vsonyp0wer_API void          Scrollbar(vsonyp0werAxis axis);
    vsonyp0wer_API vsonyp0werID       GetScrollbarID(vsonyp0werWindow * window, vsonyp0werAxis axis);
    vsonyp0wer_API void          VerticalSeparator();        // Vertical separator, for menu bars (use current line height). Not exposed because it is misleading and it doesn't have an effect on regular layout.

    // Widgets low-level behaviors
    vsonyp0wer_API bool          ButtonBehavior(const ImRect & bb, vsonyp0werID id, bool* out_hovered, bool* out_held, vsonyp0werButtonFlags flags = 0);
    vsonyp0wer_API bool          DragBehavior(vsonyp0werID id, vsonyp0werDataType data_type, void* v, float v_speed, const void* v_min, const void* v_max, const char* format, float power, vsonyp0werDragFlags flags);
    vsonyp0wer_API bool          SliderBehavior(const ImRect & bb, vsonyp0werID id, vsonyp0werDataType data_type, void* v, const void* v_min, const void* v_max, const char* format, float power, vsonyp0werSliderFlags flags, ImRect * out_grab_bb);
    vsonyp0wer_API bool          SplitterBehavior(const ImRect & bb, vsonyp0werID id, vsonyp0werAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f, float hover_visibility_delay = 0.0f);
    vsonyp0wer_API bool          TreeNodeBehavior(vsonyp0werID id, vsonyp0werTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    vsonyp0wer_API bool          TreeNodeBehaviorIsOpen(vsonyp0werID id, vsonyp0werTreeNodeFlags flags = 0);                     // Consume previous SetNextTreeNodeOpened() data, if any. May return true when logging
    vsonyp0wer_API void          TreePushRawID(vsonyp0werID id);

    // Template functions are instantiated in vsonyp0wer_widgets.cpp for a finite number of types.
    // To use them externally (for custom widget) you may need an "extern template" statement in your code in order to link to existing instances and silence Clang warnings (see #2036).
    // e.g. " extern template vsonyp0wer_API float RoundScalarWithFormatT<float, float>(const char* format, vsonyp0werDataType data_type, float v); "
    template<typename T, typename SIGNED_T, typename FLOAT_T>   vsonyp0wer_API bool  DragBehaviorT(vsonyp0werDataType data_type, T * v, float v_speed, T v_min, T v_max, const char* format, float power, vsonyp0werDragFlags flags);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   vsonyp0wer_API bool  SliderBehaviorT(const ImRect & bb, vsonyp0werID id, vsonyp0werDataType data_type, T * v, T v_min, T v_max, const char* format, float power, vsonyp0werSliderFlags flags, ImRect * out_grab_bb);
    template<typename T, typename FLOAT_T>                      vsonyp0wer_API float SliderCalcRatioFromValueT(vsonyp0werDataType data_type, T v, T v_min, T v_max, float power, float linear_zero_pos);
    template<typename T, typename SIGNED_T>                     vsonyp0wer_API T     RoundScalarWithFormatT(const char* format, vsonyp0werDataType data_type, T v);

    // InputText
    vsonyp0wer_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2 & size_arg, vsonyp0werInputTextFlags flags, vsonyp0werInputTextCallback callback = NULL, void* user_data = NULL);
    vsonyp0wer_API bool          InputScalarAsWidgetReplacement(const ImRect & bb, vsonyp0werID id, const char* label, vsonyp0werDataType data_type, void* data_ptr, const char* format);

    // Color
    vsonyp0wer_API void          ColorTooltip(const char* text, const float* col, vsonyp0werColorEditFlags flags);
    vsonyp0wer_API void          ColorEditOptionsPopup(const float* col, vsonyp0werColorEditFlags flags);
    vsonyp0wer_API void          ColorPickerOptionsPopup(const float* ref_col, vsonyp0werColorEditFlags flags);

    // Plot
    vsonyp0wer_API void          PlotEx(vsonyp0werPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size);

    // Shade functions (write over already created vertices)
    vsonyp0wer_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    vsonyp0wer_API void          ShadeVertsLinearUV(ImDrawList * draw_list, int vert_start_idx, int vert_end_idx, const ImVec2 & a, const ImVec2 & b, const ImVec2 & uv_a, const ImVec2 & uv_b, bool clamp);

} // namespace vsonyp0wer

// ImFontAtlas internals
vsonyp0wer_API bool              ImFontAtlasBuildWithStbTruetype(ImFontAtlas * atlas);
vsonyp0wer_API void              ImFontAtlasBuildRegisterDefaultCustomRects(ImFontAtlas * atlas);
vsonyp0wer_API void              ImFontAtlasBuildSetupFont(ImFontAtlas * atlas, ImFont * font, ImFontConfig * font_config, float ascent, float descent);
vsonyp0wer_API void              ImFontAtlasBuildPackCustomRects(ImFontAtlas * atlas, void* stbrp_context_opaque);
vsonyp0wer_API void              ImFontAtlasBuildFinish(ImFontAtlas * atlas);
vsonyp0wer_API void              ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
vsonyp0wer_API void              ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

// Test engine hooks (vsonyp0wer-test)
//#define vsonyp0wer_ENABLE_TEST_ENGINE
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
extern void                 vsonyp0werTestEngineHook_PreNewFrame(vsonyp0werContext * ctx);
extern void                 vsonyp0werTestEngineHook_PostNewFrame(vsonyp0werContext * ctx);
extern void                 vsonyp0werTestEngineHook_ItemAdd(vsonyp0werContext * ctx, const ImRect & bb, vsonyp0werID id);
extern void                 vsonyp0werTestEngineHook_ItemInfo(vsonyp0werContext * ctx, vsonyp0werID id, const char* label, vsonyp0werItemStatusFlags flags);
#define vsonyp0wer_TEST_ENGINE_ITEM_ADD(_BB, _ID)                vsonyp0werTestEngineHook_ItemAdd(&g, _BB, _ID)               // Register status flags
#define vsonyp0wer_TEST_ENGINE_ITEM_INFO(_ID, _LABEL, _FLAGS)  vsonyp0werTestEngineHook_ItemInfo(&g, _ID, _LABEL, _FLAGS)   // Register status flags
#else
#define vsonyp0wer_TEST_ENGINE_ITEM_ADD(_BB, _ID)                do { } while (0)
#define vsonyp0wer_TEST_ENGINE_ITEM_INFO(_ID, _LABEL, _FLAGS)  do { } while (0)
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif
