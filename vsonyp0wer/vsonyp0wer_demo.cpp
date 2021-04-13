// dear vsonyp0wer, v1.70 WIP
// (demo code)

// Message to the person tempted to delete this file when integrating Dear vsonyp0wer into their code base:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other coders
// will want to refer to and call. Have the vsonyp0wer::ShowDemoWindow() function wired in an always-available debug menu of
// your game/app! Removing this file from your project is hindering access to documentation for everyone in your team,
// likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call vsonyp0wer::ShowDemoWindow().
// If you want to link core Dear vsonyp0wer in your shipped builds but want an easy guarantee that the demo will not be linked,
// you can setup your imconfig.h with #define vsonyp0wer_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In other situation, whenever you have Dear vsonyp0wer available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, vsonyp0wer_demo.cpp (that you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword:
// In this demo code, we frequently we use 'static' variables inside functions. A static variable persist across calls, so it is
// essentially like a global variable but declared inside the scope of the function. We do this as a way to gather code and data
// in the same place, to make the demo source code faster to read, faster to write, and smaller in size.
// It also happens to be a convenient way of storing simple UI related information as long as your function doesn't need to be reentrant
// or used in threads. This might be a pattern you will want to use in your code, but most of the real data you would be editing is
// likely going to be stored outside your functions.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "vsonyp0wer.h"
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"             // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"    // warning : 'xx' is deprecated: The POSIX name for this item.. // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"   // warning : cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wformat-security"            // warning : warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"      // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. vsonyp0wer coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wunused-macros"              // warning : warning: macro is not used                         // we define snprintf/vsnprintf on Windows so they are available, but not always used.
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant                  // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"           // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"          // warning : macro name is a reserved identifier                //
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"          // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"              // warning : format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"             // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"                   // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#if (__GNUC__ >= 6)
#pragma GCC diagnostic ignored "-Wmisleading-indentation"       // warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif
#endif

// Play it nice with Windows users. Notepad in 2017 still doesn't display text data with Unix-style \n.
#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#define snprintf    _snprintf
#define vsnprintf   _vsnprintf
#else
#define IM_NEWLINE  "\n"
#endif

#define IM_MAX(_A,_B)       (((_A) >= (_B)) ? (_A) : (_B))

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS) && defined(vsonyp0wer_DISABLE_TEST_WINDOWS) && !defined(vsonyp0wer_DISABLE_DEMO_WINDOWS)   // Obsolete name since 1.53, TEST->DEMO
#define vsonyp0wer_DISABLE_DEMO_WINDOWS
#endif

#if !defined(vsonyp0wer_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
static void HelpMarker(const char* desc)
{
    vsonyp0wer::TextDisabled("(?)");
    if (vsonyp0wer::IsItemHovered())
    {
        vsonyp0wer::BeginTooltip();
        vsonyp0wer::PushTextWrapPos(vsonyp0wer::GetFontSize() * 35.0f);
        vsonyp0wer::TextUnformatted(desc);
        vsonyp0wer::PopTextWrapPos();
        vsonyp0wer::EndTooltip();
    }
}

// Helper to display basic user controls.
void vsonyp0wer::ShowUserGuide()
{
    vsonyp0werIO& io = vsonyp0wer::GetIO();
    vsonyp0wer::BulletText("Double-click on title bar to collapse window.");
    vsonyp0wer::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
    vsonyp0wer::BulletText("Click and drag on any empty space to move window.");
    vsonyp0wer::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    vsonyp0wer::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    if (io.FontAllowUserScaling)
        vsonyp0wer::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    vsonyp0wer::BulletText("Mouse Wheel to scroll.");
    vsonyp0wer::BulletText("While editing text:\n");
    vsonyp0wer::Indent();
    vsonyp0wer::BulletText("Hold SHIFT or use mouse to select text.");
    vsonyp0wer::BulletText("CTRL+Left/Right to word jump.");
    vsonyp0wer::BulletText("CTRL+A or double-click to select all.");
    vsonyp0wer::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
    vsonyp0wer::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    vsonyp0wer::BulletText("ESCAPE to revert.");
    vsonyp0wer::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    vsonyp0wer::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear vsonyp0wer features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does. You may then search for keywords in the code when you are interested by a specific feature.
void vsonyp0wer::ShowDemoWindow(bool* p_open)
{
    IM_ASSERT(vsonyp0wer::GetCurrentContext() != NULL && "Missing dear vsonyp0wer context. Refer to examples app!"); // Exceptionally add an extra assert here for people confused with initial dear vsonyp0wer setup

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_documents = false;
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);
    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear vsonyp0wer Apps (accessible from the "Help" menu)
    static bool show_app_metrics = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics) { vsonyp0wer::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_style_editor) { vsonyp0wer::Begin("Style Editor", &show_app_style_editor); vsonyp0wer::ShowStyleEditor(); vsonyp0wer::End(); }
    if (show_app_about) { vsonyp0wer::ShowAboutWindow(&show_app_about); }

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

    vsonyp0werWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= vsonyp0werWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= vsonyp0werWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= vsonyp0werWindowFlags_MenuBar;
    if (no_move)            window_flags |= vsonyp0werWindowFlags_NoMove;
    if (no_resize)          window_flags |= vsonyp0werWindowFlags_NoResize;
    if (no_collapse)        window_flags |= vsonyp0werWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= vsonyp0werWindowFlags_NoNav;
    if (no_background)      window_flags |= vsonyp0werWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= vsonyp0werWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
    vsonyp0wer::SetNextWindowPos(ImVec2(650, 20), vsonyp0werCond_FirstUseEver);
    vsonyp0wer::SetNextWindowSize(ImVec2(550, 680), vsonyp0werCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!vsonyp0wer::Begin("vsonyp0wer Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        vsonyp0wer::End();
        return;
    }

    // Most "big" widgets share a common width settings by default.
    //vsonyp0wer::PushItemWidth(vsonyp0wer::GetWindowWidth() * 0.65f);    // Use 2/3 of the space for widgets and 1/3 for labels (default)
    vsonyp0wer::PushItemWidth(vsonyp0wer::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.

    // Menu Bar
    if (vsonyp0wer::BeginMenuBar())
    {
        if (vsonyp0wer::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            vsonyp0wer::EndMenu();
        }
        if (vsonyp0wer::BeginMenu("Examples"))
        {
            vsonyp0wer::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            vsonyp0wer::MenuItem("Console", NULL, &show_app_console);
            vsonyp0wer::MenuItem("Log", NULL, &show_app_log);
            vsonyp0wer::MenuItem("Simple layout", NULL, &show_app_layout);
            vsonyp0wer::MenuItem("Property editor", NULL, &show_app_property_editor);
            vsonyp0wer::MenuItem("Long text display", NULL, &show_app_long_text);
            vsonyp0wer::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            vsonyp0wer::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            vsonyp0wer::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            vsonyp0wer::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            vsonyp0wer::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            vsonyp0wer::MenuItem("Documents", NULL, &show_app_documents);
            vsonyp0wer::EndMenu();
        }
        if (vsonyp0wer::BeginMenu("Help"))
        {
            vsonyp0wer::MenuItem("Metrics", NULL, &show_app_metrics);
            vsonyp0wer::MenuItem("Style Editor", NULL, &show_app_style_editor);
            vsonyp0wer::MenuItem("About Dear vsonyp0wer", NULL, &show_app_about);
            vsonyp0wer::EndMenu();
        }
        vsonyp0wer::EndMenuBar();
    }

    vsonyp0wer::Text("dear vsonyp0wer says hello. (%s)", vsonyp0wer_VERSION);
    vsonyp0wer::Spacing();

    if (vsonyp0wer::CollapsingHeader("Help"))
    {
        vsonyp0wer::Text("PROGRAMMER GUIDE:");
        vsonyp0wer::BulletText("Please see the ShowDemoWindow() code in vsonyp0wer_demo.cpp. <- you are here!");
        vsonyp0wer::BulletText("Please see the comments in vsonyp0wer.cpp.");
        vsonyp0wer::BulletText("Please see the examples/ in application.");
        vsonyp0wer::BulletText("Enable 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        vsonyp0wer::BulletText("Enable 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        vsonyp0wer::Separator();

        vsonyp0wer::Text("USER GUIDE:");
        vsonyp0wer::ShowUserGuide();
    }

    if (vsonyp0wer::CollapsingHeader("Configuration"))
    {
        vsonyp0werIO& io = vsonyp0wer::GetIO();

        if (vsonyp0wer::TreeNode("Configuration##2"))
        {
            vsonyp0wer::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", (unsigned int*)& io.ConfigFlags, vsonyp0werConfigFlags_NavEnableKeyboard);
            vsonyp0wer::CheckboxFlags("io.ConfigFlags: NavEnableGamepad", (unsigned int*)& io.ConfigFlags, vsonyp0werConfigFlags_NavEnableGamepad);
            vsonyp0wer::SameLine(); HelpMarker("Required back-end to feed in gamepad inputs in io.NavInputs[] and set io.BackendFlags |= vsonyp0werBackendFlags_HasGamepad.\n\nRead instructions in vsonyp0wer.cpp for details.");
            vsonyp0wer::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", (unsigned int*)& io.ConfigFlags, vsonyp0werConfigFlags_NavEnableSetMousePos);
            vsonyp0wer::SameLine(); HelpMarker("Instruct navigation to move the mouse cursor. See comment for vsonyp0werConfigFlags_NavEnableSetMousePos.");
            vsonyp0wer::CheckboxFlags("io.ConfigFlags: NoMouse", (unsigned int*)& io.ConfigFlags, vsonyp0werConfigFlags_NoMouse);
            if (io.ConfigFlags & vsonyp0werConfigFlags_NoMouse) // Create a way to restore this flag otherwise we could be stuck completely!
            {
                if (fmodf((float)vsonyp0wer::GetTime(), 0.40f) < 0.20f)
                {
                    vsonyp0wer::SameLine();
                    vsonyp0wer::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (vsonyp0wer::IsKeyPressed(vsonyp0wer::GetKeyIndex(vsonyp0werKey_Space)))
                    io.ConfigFlags &= ~vsonyp0werConfigFlags_NoMouse;
            }
            vsonyp0wer::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", (unsigned int*)& io.ConfigFlags, vsonyp0werConfigFlags_NoMouseCursorChange);
            vsonyp0wer::SameLine(); HelpMarker("Instruct back-end to not alter mouse cursor shape and visibility.");
            vsonyp0wer::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            vsonyp0wer::SameLine(); HelpMarker("Set to false to disable blinking cursor, for users who consider it distracting");
            vsonyp0wer::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            vsonyp0wer::SameLine(); HelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & vsonyp0werBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            vsonyp0wer::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            vsonyp0wer::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            vsonyp0wer::SameLine(); HelpMarker("Instruct Dear vsonyp0wer to render a mouse cursor for you. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            vsonyp0wer::TreePop();
            vsonyp0wer::Separator();
        }

        if (vsonyp0wer::TreeNode("Backend Flags"))
        {
            HelpMarker("Those flags are set by the back-ends (vsonyp0wer_impl_xxx files) to specify their capabilities.");
            vsonyp0werBackendFlags backend_flags = io.BackendFlags; // Make a local copy to avoid modifying actual back-end flags.
            vsonyp0wer::CheckboxFlags("io.BackendFlags: HasGamepad", (unsigned int*)& backend_flags, vsonyp0werBackendFlags_HasGamepad);
            vsonyp0wer::CheckboxFlags("io.BackendFlags: HasMouseCursors", (unsigned int*)& backend_flags, vsonyp0werBackendFlags_HasMouseCursors);
            vsonyp0wer::CheckboxFlags("io.BackendFlags: HasSetMousePos", (unsigned int*)& backend_flags, vsonyp0werBackendFlags_HasSetMousePos);
            vsonyp0wer::TreePop();
            vsonyp0wer::Separator();
        }

        if (vsonyp0wer::TreeNode("Style"))
        {
            vsonyp0wer::ShowStyleEditor();
            vsonyp0wer::TreePop();
            vsonyp0wer::Separator();
        }

        if (vsonyp0wer::TreeNode("Capture/Logging"))
        {
            vsonyp0wer::TextWrapped("The logging API redirects all text output so you can easily capture the content of a window or a block. Tree nodes can be automatically expanded.");
            HelpMarker("Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            vsonyp0wer::LogButtons();
            vsonyp0wer::TextWrapped("You can also call vsonyp0wer::LogText() to output directly to the log without a visual output.");
            if (vsonyp0wer::Button("Copy \"Hello, world!\" to clipboard"))
            {
                vsonyp0wer::LogToClipboard();
                vsonyp0wer::LogText("Hello, world!");
                vsonyp0wer::LogFinish();
            }
            vsonyp0wer::TreePop();
        }
    }

    if (vsonyp0wer::CollapsingHeader("Window options"))
    {
        vsonyp0wer::Checkbox("No titlebar", &no_titlebar); vsonyp0wer::SameLine(150);
        vsonyp0wer::Checkbox("No scrollbar", &no_scrollbar); vsonyp0wer::SameLine(300);
        vsonyp0wer::Checkbox("No menu", &no_menu);
        vsonyp0wer::Checkbox("No move", &no_move); vsonyp0wer::SameLine(150);
        vsonyp0wer::Checkbox("No resize", &no_resize); vsonyp0wer::SameLine(300);
        vsonyp0wer::Checkbox("No collapse", &no_collapse);
        vsonyp0wer::Checkbox("No close", &no_close); vsonyp0wer::SameLine(150);
        vsonyp0wer::Checkbox("No nav", &no_nav); vsonyp0wer::SameLine(300);
        vsonyp0wer::Checkbox("No background", &no_background);
        vsonyp0wer::Checkbox("No bring to front", &no_bring_to_front);
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowColumns();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    vsonyp0wer::End();
}

static void ShowDemoWindowWidgets()
{
    if (!vsonyp0wer::CollapsingHeader("Widgets"))
        return;

    if (vsonyp0wer::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (vsonyp0wer::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            vsonyp0wer::SameLine();
            vsonyp0wer::Text("Thanks for clicking me!");
        }

        static bool check = true;
        vsonyp0wer::Checkbox("checkbox", &check);

        static int e = 0;
        vsonyp0wer::RadioButton("radio a", &e, 0); vsonyp0wer::SameLine();
        vsonyp0wer::RadioButton("radio b", &e, 1); vsonyp0wer::SameLine();
        vsonyp0wer::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                vsonyp0wer::SameLine();
            vsonyp0wer::PushID(i);
            vsonyp0wer::PushStyleColor(vsonyp0werCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            vsonyp0wer::PushStyleColor(vsonyp0werCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            vsonyp0wer::PushStyleColor(vsonyp0werCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            vsonyp0wer::Button("Click");
            vsonyp0wer::PopStyleColor(3);
            vsonyp0wer::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
        vsonyp0wer::AlignTextToFramePadding();
        vsonyp0wer::Text("Hold to repeat:");
        vsonyp0wer::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = vsonyp0wer::GetStyle().ItemInnerSpacing.x;
        vsonyp0wer::PushButtonRepeat(true);
        if (vsonyp0wer::ArrowButton("##left", vsonyp0werDir_Left)) { counter--; }
        vsonyp0wer::SameLine(0.0f, spacing);
        if (vsonyp0wer::ArrowButton("##right", vsonyp0werDir_Right)) { counter++; }
        vsonyp0wer::PopButtonRepeat();
        vsonyp0wer::SameLine();
        vsonyp0wer::Text("%d", counter);

        vsonyp0wer::Text("Hover over me");
        if (vsonyp0wer::IsItemHovered())
            vsonyp0wer::SetTooltip("I am a tooltip");

        vsonyp0wer::SameLine();
        vsonyp0wer::Text("- or me");
        if (vsonyp0wer::IsItemHovered())
        {
            vsonyp0wer::BeginTooltip();
            vsonyp0wer::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            vsonyp0wer::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            vsonyp0wer::EndTooltip();
        }

        vsonyp0wer::Separator();

        vsonyp0wer::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
            static int item_current = 0;
            vsonyp0wer::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            vsonyp0wer::SameLine(); HelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");
        }

        {
            static char str0[128] = "Hello, world!";
            vsonyp0wer::InputText("input text", str0, IM_ARRAYSIZE(str0));
            vsonyp0wer::SameLine(); HelpMarker("USER:\nHold SHIFT or use mouse to select text.\n" "CTRL+Left/Right to word jump.\n" "CTRL+A or double-click to select all.\n" "CTRL+X,CTRL+C,CTRL+V clipboard.\n" "CTRL+Z,CTRL+Y undo/redo.\n" "ESCAPE to revert.\n\nPROGRAMMER:\nYou can use the vsonyp0werInputTextFlags_CallbackResize facility if you need to wire InputText() to a dynamic string type. See misc/cpp/vsonyp0wer_stdlib.h for an example (this is not demonstrated in vsonyp0wer_demo.cpp).");

            static char str1[128] = "";
            vsonyp0wer::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            static int i0 = 123;
            vsonyp0wer::InputInt("input int", &i0);
            vsonyp0wer::SameLine(); HelpMarker("You can apply arithmetic operators +,*,/ on numerical values.\n  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\nUse +- to subtract.\n");

            static float f0 = 0.001f;
            vsonyp0wer::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            vsonyp0wer::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            vsonyp0wer::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            vsonyp0wer::SameLine(); HelpMarker("You can input value using the scientific notation,\n  e.g. \"1e+8\" becomes \"100000000\".\n");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            vsonyp0wer::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            vsonyp0wer::DragInt("drag int", &i1, 1);
            vsonyp0wer::SameLine(); HelpMarker("Click and drag to edit value.\nHold SHIFT/ALT for faster/slower edit.\nDouble-click or CTRL+click to input value.");

            vsonyp0wer::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%");

            static float f1 = 1.00f, f2 = 0.0067f;
            vsonyp0wer::DragFloat("drag float", &f1, 0.005f);
            vsonyp0wer::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            vsonyp0wer::SliderInt("slider int", &i1, -1, 3);
            vsonyp0wer::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            vsonyp0wer::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            vsonyp0wer::SliderFloat("slider float (curve)", &f2, -10.0f, 10.0f, "%.4f", 2.0f);
            static float angle = 0.0f;
            vsonyp0wer::SliderAngle("slider angle", &angle);
        }

        {
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            vsonyp0wer::ColorEdit3("color 1", col1);
            vsonyp0wer::SameLine(); HelpMarker("Click on the colored square to open a color picker.\nClick and hold to use drag and drop.\nRight-click on the colored square to show options.\nCTRL+click on individual component to input value.\n");

            vsonyp0wer::ColorEdit4("color 2", col2);
        }

        {
            // List box
            const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int listbox_item_current = 1;
            vsonyp0wer::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

            //static int listbox_item_current2 = 2;
            //vsonyp0wer::PushItemWidth(-1);
            //vsonyp0wer::ListBox("##listbox2", &listbox_item_current2, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
            //vsonyp0wer::PopItemWidth();
        }

        vsonyp0wer::TreePop();
    }

    // Testing vsonyp0werOnceUponAFrame helper.
    //static vsonyp0werOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        vsonyp0wer::Text("This will be displayed only once.");

    if (vsonyp0wer::TreeNode("Trees"))
    {
        if (vsonyp0wer::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
                if (vsonyp0wer::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    vsonyp0wer::Text("blah blah");
                    vsonyp0wer::SameLine();
                    if (vsonyp0wer::SmallButton("button")) {};
                    vsonyp0wer::TreePop();
                }
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker("This is a more typical looking tree with selectable nodes.\nClick to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static bool align_label_with_current_x_position = false;
            vsonyp0wer::Checkbox("Align label with current X position)", &align_label_with_current_x_position);
            vsonyp0wer::Text("Hello!");
            if (align_label_with_current_x_position)
                vsonyp0wer::Unindent(vsonyp0wer::GetTreeNodeToLabelSpacing());

            static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
            int node_clicked = -1;                // Temporary stohnly of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
            vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_IndentSpacing, vsonyp0wer::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
            for (int i = 0; i < 6; i++)
            {
                // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
                vsonyp0werTreeNodeFlags node_flags = vsonyp0werTreeNodeFlags_OpenOnArrow | vsonyp0werTreeNodeFlags_OpenOnDoubleClick;
                if (selection_mask & (1 << i))
                    node_flags |= vsonyp0werTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = vsonyp0wer::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (vsonyp0wer::IsItemClicked())
                        node_clicked = i;
                    if (node_open)
                    {
                        vsonyp0wer::Text("Blah blah\nBlah Blah");
                        vsonyp0wer::TreePop();
                    }
                } else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf.
                    // Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
                    node_flags |= vsonyp0werTreeNodeFlags_Leaf | vsonyp0werTreeNodeFlags_NoTreePushOnOpen; // vsonyp0werTreeNodeFlags_Bullet
                    vsonyp0wer::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (vsonyp0wer::IsItemClicked())
                        node_clicked = i;
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
                if (vsonyp0wer::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            vsonyp0wer::PopStyleVar();
            if (align_label_with_current_x_position)
                vsonyp0wer::Indent(vsonyp0wer::GetTreeNodeToLabelSpacing());
            vsonyp0wer::TreePop();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        vsonyp0wer::Checkbox("Show 2nd header", &closable_group);
        if (vsonyp0wer::CollapsingHeader("Header"))
        {
            vsonyp0wer::Text("IsItemHovered: %d", vsonyp0wer::IsItemHovered());
            for (int i = 0; i < 5; i++)
                vsonyp0wer::Text("Some content %d", i);
        }
        if (vsonyp0wer::CollapsingHeader("Header with a close button", &closable_group))
        {
            vsonyp0wer::Text("IsItemHovered: %d", vsonyp0wer::IsItemHovered());
            for (int i = 0; i < 5; i++)
                vsonyp0wer::Text("More content %d", i);
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Bullets"))
    {
        vsonyp0wer::BulletText("Bullet point 1");
        vsonyp0wer::BulletText("Bullet point 2\nOn multiple lines");
        vsonyp0wer::Bullet(); vsonyp0wer::Text("Bullet point 3 (two calls)");
        vsonyp0wer::Bullet(); vsonyp0wer::SmallButton("Button");
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Text"))
    {
        if (vsonyp0wer::TreeNode("Colored Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            vsonyp0wer::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
            vsonyp0wer::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
            vsonyp0wer::TextDisabled("Disabled");
            vsonyp0wer::SameLine(); HelpMarker("The TextDisabled color is stored in vsonyp0werStyle.");
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            vsonyp0wer::TextWrapped("This text should automatically wrap on the edge of the window. The current implementation for text wrapping follows simple rules suitable for English and possibly other languages.");
            vsonyp0wer::Spacing();

            static float wrap_width = 200.0f;
            vsonyp0wer::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            vsonyp0wer::Text("Test paragraph 1:");
            ImVec2 pos = vsonyp0wer::GetCursorScreenPos();
            vsonyp0wer::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + vsonyp0wer::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
            vsonyp0wer::PushTextWrapPos(vsonyp0wer::GetCursorPos().x + wrap_width);
            vsonyp0wer::Text("The lazy dog is a good dog. This paragraph is made to fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
            vsonyp0wer::GetWindowDrawList()->AddRect(vsonyp0wer::GetItemRectMin(), vsonyp0wer::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
            vsonyp0wer::PopTextWrapPos();

            vsonyp0wer::Text("Test paragraph 2:");
            pos = vsonyp0wer::GetCursorScreenPos();
            vsonyp0wer::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + wrap_width, pos.y), ImVec2(pos.x + wrap_width + 10, pos.y + vsonyp0wer::GetTextLineHeight()), IM_COL32(255, 0, 255, 255));
            vsonyp0wer::PushTextWrapPos(vsonyp0wer::GetCursorPos().x + wrap_width);
            vsonyp0wer::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");
            vsonyp0wer::GetWindowDrawList()->AddRect(vsonyp0wer::GetItemRectMin(), vsonyp0wer::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
            vsonyp0wer::PopTextWrapPos();

            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font, try Noto, or Arial Unicode, or M+ fonts. Read misc/fonts/README.txt for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. Visual Studio save your file as 'UTF-8 without signature')
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8 CHARACTERS IN THIS SOURCE FILE.
            //   Instead we are encoding a few strings with hexadecimal constants. Don't do this in your application!
            //   Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed, so you can safely copy & paste garbled characters into another application.
            vsonyp0wer::TextWrapped("CJK text will only appears if the font was loaded with the appropriate CJK character ranges. Call io.Font->AddFontFromFileTTF() manually to load extra character ranges. Read misc/fonts/README.txt for details.");
            vsonyp0wer::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            vsonyp0wer::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            vsonyp0wer::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            vsonyp0wer::TreePop();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Images"))
    {
        vsonyp0werIO& io = vsonyp0wer::GetIO();
        vsonyp0wer::TextWrapped("Below we are displaying the font texture (which is the only texture we have access to in this demo). Use the 'ImTextureID' type as stohnly to pass pointers or identifier to your own texture data. Hover the texture for a zoomed view!");

        // Here we are grabbing the font texture because that's the only one we have access to inside the demo code.
        // Remember that ImTextureID is just stohnly for whatever you want it to be, it is essentially a value that will be passed to the render function inside the ImDrawCmd structure.
        // If you use one of the default vsonyp0wer_impl_XXXX.cpp renderer, they all have comments at the top of their file to specify what they expect to be stored in ImTextureID.
        // (for example, the vsonyp0wer_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer. The vsonyp0wer_impl_glfw_gl3.cpp renderer expect a GLuint OpenGL texture identifier etc.)
        // If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers to vsonyp0wer::Image(), and gather width/height through your own functions, etc.
        // Using ShowMetricsWindow() as a "debugger" to inspect the draw data that are being passed to your render will help you debug issues if you are confused about this.
        // Consider using the lower-level ImDrawList::AddImage() API, via vsonyp0wer::GetWindowDrawList()->AddImage().
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;

        vsonyp0wer::Text("%.0fx%.0f", my_tex_w, my_tex_h);
        ImVec2 pos = vsonyp0wer::GetCursorScreenPos();
        vsonyp0wer::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        if (vsonyp0wer::IsItemHovered())
        {
            vsonyp0wer::BeginTooltip();
            float region_sz = 32.0f;
            float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > my_tex_w - region_sz) region_x = my_tex_w - region_sz;
            float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > my_tex_h - region_sz) region_y = my_tex_h - region_sz;
            float zoom = 4.0f;
            vsonyp0wer::Text("Min: (%.2f, %.2f)", region_x, region_y);
            vsonyp0wer::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
            ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
            ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
            vsonyp0wer::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
            vsonyp0wer::EndTooltip();
        }
        vsonyp0wer::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            vsonyp0wer::PushID(i);
            int frame_padding = -1 + i;     // -1 = uses default padding
            if (vsonyp0wer::ImageButton(my_tex_id, ImVec2(32, 32), ImVec2(0, 0), ImVec2(32.0f / my_tex_w, 32 / my_tex_h), frame_padding, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)))
                pressed_count += 1;
            vsonyp0wer::PopID();
            vsonyp0wer::SameLine();
        }
        vsonyp0wer::NewLine();
        vsonyp0wer::Text("Pressed %d times.", pressed_count);
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static vsonyp0werComboFlags flags = 0;
        vsonyp0wer::CheckboxFlags("vsonyp0werComboFlags_PopupAlignLeft", (unsigned int*)& flags, vsonyp0werComboFlags_PopupAlignLeft);
        vsonyp0wer::SameLine(); HelpMarker("Only makes a difference if the popup is larger than the combo");
        if (vsonyp0wer::CheckboxFlags("vsonyp0werComboFlags_NoArrowButton", (unsigned int*)& flags, vsonyp0werComboFlags_NoArrowButton))
            flags &= ~vsonyp0werComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (vsonyp0wer::CheckboxFlags("vsonyp0werComboFlags_NoPreview", (unsigned int*)& flags, vsonyp0werComboFlags_NoPreview))
            flags &= ~vsonyp0werComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // General BeginCombo() API, you have full control over your selection data and display type.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static const char* item_current = items[0];            // Here our selection is a single pointer stored outside the object.
        if (vsonyp0wer::BeginCombo("combo 1", item_current, flags)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                bool is_selected = (item_current == items[n]);
                if (vsonyp0wer::Selectable(items[n], is_selected))
                    item_current = items[n];
                if (is_selected)
                    vsonyp0wer::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
            }
            vsonyp0wer::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        static int item_current_2 = 0;
        vsonyp0wer::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        vsonyp0wer::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };
        static int item_current_4 = 0;
        vsonyp0wer::Combo("combo 4 (function)", &item_current_4, &FuncHolder::ItemGetter, items, IM_ARRAYSIZE(items));

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information. When Selectable() has been clicked is returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in a different manner (in flags within objects, as an external list, etc).
        if (vsonyp0wer::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            vsonyp0wer::Selectable("1. I am selectable", &selection[0]);
            vsonyp0wer::Selectable("2. I am selectable", &selection[1]);
            vsonyp0wer::Text("3. I am not selectable");
            vsonyp0wer::Selectable("4. I am selectable", &selection[3]);
            if (vsonyp0wer::Selectable("5. I am double clickable", selection[4], vsonyp0werSelectableFlags_AllowDoubleClick))
                if (vsonyp0wer::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (vsonyp0wer::Selectable(buf, selected == n))
                    selected = n;
            }
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("Selection State: Multiple Selection"))
        {
            HelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (vsonyp0wer::Selectable(buf, selection[n]))
                {
                    if (!vsonyp0wer::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter and toggle your booleans automatically.
            static bool selected[3] = { false, false, false };
            vsonyp0wer::Selectable("main.c", &selected[0]); vsonyp0wer::SameLine(300); vsonyp0wer::Text(" 2,345 bytes");
            vsonyp0wer::Selectable("Hello.cpp", &selected[1]); vsonyp0wer::SameLine(300); vsonyp0wer::Text("12,345 bytes");
            vsonyp0wer::Selectable("Hello.h", &selected[2]); vsonyp0wer::SameLine(300); vsonyp0wer::Text(" 2,345 bytes");
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("In columns"))
        {
            vsonyp0wer::Columns(3, NULL, false);
            static bool selected[16] = { 0 };
            for (int i = 0; i < 16; i++)
            {
                char label[32]; sprintf(label, "Item %d", i);
                if (vsonyp0wer::Selectable(label, &selected[i])) {}
                vsonyp0wer::NextColumn();
            }
            vsonyp0wer::Columns(1);
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("Grid"))
        {
            static bool selected[4 * 4] = { true, false, false, false, false, true, false, false, false, false, true, false, false, false, false, true };
            for (int i = 0; i < 4 * 4; i++)
            {
                vsonyp0wer::PushID(i);
                if (vsonyp0wer::Selectable("Sailor", &selected[i], 0, ImVec2(50, 50)))
                {
                    // Note: We _unnecessarily_ test for both x/y and i here only to silence some static analyzer. The second part of each test is unnecessary.
                    int x = i % 4;
                    int y = i / 4;
                    if (x > 0) { selected[i - 1] ^= 1; }
                    if (x < 3 && i < 15) { selected[i + 1] ^= 1; }
                    if (y > 0 && i > 3) { selected[i - 4] ^= 1; }
                    if (y < 3 && i < 12) { selected[i + 4] ^= 1; }
                }
                if ((i % 4) < 3) vsonyp0wer::SameLine();
                vsonyp0wer::PopID();
            }
            vsonyp0wer::TreePop();
        }
        if (vsonyp0wer::TreeNode("Alignment"))
        {
            HelpMarker("Alignment applies when a selectable is larger than its text content.\nBy default, Selectables uses style.SelectableTextAlign but it can be overriden on a per-item basis using PushStyleVar().");
            static bool selected[3 * 3] = { true, false, true, false, true, false, true, false, true };
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    ImVec2 alignment = ImVec2((float)x / 2.0f, (float)y / 2.0f);
                    char name[32];
                    sprintf(name, "(%.1f,%.1f)", alignment.x, alignment.y);
                    if (x > 0) vsonyp0wer::SameLine();
                    vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_SelectableTextAlign, alignment);
                    vsonyp0wer::Selectable(name, &selected[3 * y + x], vsonyp0werSelectableFlags_None, ImVec2(80, 80));
                    vsonyp0wer::PopStyleVar();
                }
            }
            vsonyp0wer::TreePop();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Text Input"))
    {
        if (vsonyp0wer::TreeNode("Multi-line Text Input"))
        {
            // Note: we are using a fixed-sized buffer for simplicity here. See vsonyp0werInputTextFlags_CallbackResize
            // and the code in misc/cpp/vsonyp0wer_stdlib.h for how to setup InputText() for dynamically resizing strings.
            static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static vsonyp0werInputTextFlags flags = vsonyp0werInputTextFlags_AllowTabInput;
            HelpMarker("You can use the vsonyp0werInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/vsonyp0wer_stdlib.h for an example. (This is not demonstrated in vsonyp0wer_demo.cpp)");
            vsonyp0wer::CheckboxFlags("vsonyp0werInputTextFlags_ReadOnly", (unsigned int*)& flags, vsonyp0werInputTextFlags_ReadOnly);
            vsonyp0wer::CheckboxFlags("vsonyp0werInputTextFlags_AllowTabInput", (unsigned int*)& flags, vsonyp0werInputTextFlags_AllowTabInput);
            vsonyp0wer::CheckboxFlags("vsonyp0werInputTextFlags_CtrlEnterForNewLine", (unsigned int*)& flags, vsonyp0werInputTextFlags_CtrlEnterForNewLine);
            vsonyp0wer::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, vsonyp0wer::GetTextLineHeight() * 16), flags);
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Filtered Text Input"))
        {
            static char buf1[64] = ""; vsonyp0wer::InputText("default", buf1, 64);
            static char buf2[64] = ""; vsonyp0wer::InputText("decimal", buf2, 64, vsonyp0werInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; vsonyp0wer::InputText("hexadecimal", buf3, 64, vsonyp0werInputTextFlags_CharsHexadecimal | vsonyp0werInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; vsonyp0wer::InputText("uppercase", buf4, 64, vsonyp0werInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; vsonyp0wer::InputText("no blank", buf5, 64, vsonyp0werInputTextFlags_CharsNoBlank);
            struct TextFilters { static int Filtervsonyp0werLetters(vsonyp0werInputTextCallbackData* data) { if (data->EventChar < 256 && strchr("vsonyp0wer", (char)data->EventChar)) return 0; return 1; } };
            static char buf6[64] = ""; vsonyp0wer::InputText("\"vsonyp0wer\" letters", buf6, 64, vsonyp0werInputTextFlags_CallbackCharFilter, TextFilters::Filtervsonyp0werLetters);

            vsonyp0wer::Text("Password input");
            static char bufpass[64] = "password123";
            vsonyp0wer::InputText("password", bufpass, 64, vsonyp0werInputTextFlags_Password | vsonyp0werInputTextFlags_CharsNoBlank);
            vsonyp0wer::SameLine(); HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
            vsonyp0wer::InputTextWithHint("password (w/ hint)", "<password>", bufpass, 64, vsonyp0werInputTextFlags_Password | vsonyp0werInputTextFlags_CharsNoBlank);
            vsonyp0wer::InputText("password (clear)", bufpass, 64, vsonyp0werInputTextFlags_CharsNoBlank);
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Resize Callback"))
        {
            // If you have a custom string type you would typically create a vsonyp0wer::InputText() wrapper than takes your type as input.
            // See misc/cpp/vsonyp0wer_stdlib.h and .cpp for an implementation of this using std::string.
            HelpMarker("Demonstrate using vsonyp0werInputTextFlags_CallbackResize to wire your resizable string type to InputText().\n\nSee misc/cpp/vsonyp0wer_stdlib.h for an implementation of this for std::string.");
            struct Funcs
            {
                static int MyResizeCallback(vsonyp0werInputTextCallbackData* data)
                {
                    if (data->EventFlag == vsonyp0werInputTextFlags_CallbackResize)
                    {
                        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
                        IM_ASSERT(my_str->begin() == data->Buf);
                        my_str->resize(data->BufSize);  // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = my_str->begin();
                    }
                    return 0;
                }

                // Tip: Because vsonyp0wer:: is a namespace you would typicall add your own function into the namespace in your own source files.
                // For example, you may add a function called vsonyp0wer::InputText(const char* label, MyString* my_str).
                static bool MyInputTextMultiline(const char* label, ImVector<char> * my_str, const ImVec2 & size = ImVec2(0, 0), vsonyp0werInputTextFlags flags = 0)
                {
                    IM_ASSERT((flags & vsonyp0werInputTextFlags_CallbackResize) == 0);
                    return vsonyp0wer::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | vsonyp0werInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
                }
            };

            // For this demo we are using ImVector as a string container.
            // Note that because we need to store a terminating zero character, our size/capacity are 1 more than usually reported by a typical string class.
            static ImVector<char> my_str;
            if (my_str.empty())
                my_str.push_back(0);
            Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-1.0f, vsonyp0wer::GetTextLineHeight() * 16));
            vsonyp0wer::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
            vsonyp0wer::TreePop();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        vsonyp0wer::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        vsonyp0wer::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Create a dummy array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
        static float values[90] = { 0 };
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = vsonyp0wer::GetTime();
        while (refresh_time < vsonyp0wer::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }
        vsonyp0wer::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0, 80));
        vsonyp0wer::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        vsonyp0wer::Separator();
        vsonyp0wer::PushItemWidth(100); vsonyp0wer::Combo("func", &func_type, "Sin\0Saw\0"); vsonyp0wer::PopItemWidth();
        vsonyp0wer::SameLine();
        vsonyp0wer::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        vsonyp0wer::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        vsonyp0wer::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        vsonyp0wer::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * vsonyp0wer::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        vsonyp0wer::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        vsonyp0wer::SameLine(0.0f, vsonyp0wer::GetStyle().ItemInnerSpacing.x);
        vsonyp0wer::Text("Progress Bar");

        float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        vsonyp0wer::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Color/Picker Widgets"))
    {
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        vsonyp0wer::Checkbox("With Alpha Preview", &alpha_preview);
        vsonyp0wer::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        vsonyp0wer::Checkbox("With Drag and Drop", &drag_and_drop);
        vsonyp0wer::Checkbox("With Options Menu", &options_menu); vsonyp0wer::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
        vsonyp0wer::Checkbox("With HDR", &hdr); vsonyp0wer::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        int misc_flags = (hdr ? vsonyp0werColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : vsonyp0werColorEditFlags_NoDragDrop) | (alpha_half_preview ? vsonyp0werColorEditFlags_AlphaPreviewHalf : (alpha_preview ? vsonyp0werColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : vsonyp0werColorEditFlags_NoOptions);

        vsonyp0wer::Text("Color widget:");
        vsonyp0wer::SameLine(); HelpMarker("Click on the colored square to open a color picker.\nCTRL+click on individual component to input value.\n");
        vsonyp0wer::ColorEdit3("MyColor##1", (float*)& color, misc_flags);

        vsonyp0wer::Text("Color widget HSV with Alpha:");
        vsonyp0wer::ColorEdit4("MyColor##2", (float*)& color, vsonyp0werColorEditFlags_DisplayHSV | misc_flags);

        vsonyp0wer::Text("Color widget with Float Display:");
        vsonyp0wer::ColorEdit4("MyColor##2f", (float*)& color, vsonyp0werColorEditFlags_Float | misc_flags);

        vsonyp0wer::Text("Color button with Picker:");
        vsonyp0wer::SameLine(); HelpMarker("With the vsonyp0werColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\nWith the vsonyp0werColorEditFlags_NoLabel flag you can pass a non-empty label which will only be used for the tooltip and picker popup.");
        vsonyp0wer::ColorEdit4("MyColor##3", (float*)& color, vsonyp0werColorEditFlags_NoInputs | vsonyp0werColorEditFlags_NoLabel | misc_flags);

        vsonyp0wer::Text("Color button with Custom Picker Popup:");

        // Generate a dummy default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = { };
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                vsonyp0wer::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        static ImVec4 backup_color;
        bool open_popup = vsonyp0wer::ColorButton("MyColor##3b", color, misc_flags);
        vsonyp0wer::SameLine();
        open_popup |= vsonyp0wer::Button("Palette");
        if (open_popup)
        {
            vsonyp0wer::OpenPopup("mypicker");
            backup_color = color;
        }
        if (vsonyp0wer::BeginPopup("mypicker"))
        {
            vsonyp0wer::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            vsonyp0wer::Separator();
            vsonyp0wer::ColorPicker4("##picker", (float*)& color, misc_flags | vsonyp0werColorEditFlags_NoSidePreview | vsonyp0werColorEditFlags_NoSmallPreview);
            vsonyp0wer::SameLine();

            vsonyp0wer::BeginGroup(); // Lock X position
            vsonyp0wer::Text("Current");
            vsonyp0wer::ColorButton("##current", color, vsonyp0werColorEditFlags_NoPicker | vsonyp0werColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            vsonyp0wer::Text("Previous");
            if (vsonyp0wer::ColorButton("##previous", backup_color, vsonyp0werColorEditFlags_NoPicker | vsonyp0werColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            vsonyp0wer::Separator();
            vsonyp0wer::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                vsonyp0wer::PushID(n);
                if ((n % 8) != 0)
                    vsonyp0wer::SameLine(0.0f, vsonyp0wer::GetStyle().ItemSpacing.y);
                if (vsonyp0wer::ColorButton("##palette", saved_palette[n], vsonyp0werColorEditFlags_NoAlpha | vsonyp0werColorEditFlags_NoPicker | vsonyp0werColorEditFlags_NoTooltip, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry
                // (Note that ColorButton is already a drag source by default, unless using vsonyp0werColorEditFlags_NoDragDrop)
                if (vsonyp0wer::BeginDragDropTarget())
                {
                    if (const vsonyp0werPayload * payload = vsonyp0wer::AcceptDragDropPayload(vsonyp0wer_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)& saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const vsonyp0werPayload * payload = vsonyp0wer::AcceptDragDropPayload(vsonyp0wer_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)& saved_palette[n], payload->Data, sizeof(float) * 4);
                    vsonyp0wer::EndDragDropTarget();
                }

                vsonyp0wer::PopID();
            }
            vsonyp0wer::EndGroup();
            vsonyp0wer::EndPopup();
        }

        vsonyp0wer::Text("Color button only:");
        vsonyp0wer::ColorButton("MyColor##3c", *(ImVec4*)& color, misc_flags, ImVec2(80, 80));

        vsonyp0wer::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        vsonyp0wer::Checkbox("With Alpha", &alpha);
        vsonyp0wer::Checkbox("With Alpha Bar", &alpha_bar);
        vsonyp0wer::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            vsonyp0wer::SameLine();
            vsonyp0wer::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                vsonyp0wer::SameLine();
                vsonyp0wer::ColorEdit4("##RefColor", &ref_color_v.x, vsonyp0werColorEditFlags_NoInputs | misc_flags);
            }
        }
        vsonyp0wer::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        vsonyp0wer::SameLine(); HelpMarker("ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
        vsonyp0wer::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        vsonyp0wer::SameLine(); HelpMarker("User can right-click the picker to change mode.");
        vsonyp0werColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= vsonyp0werColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= vsonyp0werColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= vsonyp0werColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= vsonyp0werColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= vsonyp0werColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= vsonyp0werColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= vsonyp0werColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= vsonyp0werColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= vsonyp0werColorEditFlags_DisplayHex;
        vsonyp0wer::ColorPicker4("MyColor##4", (float*)& color, flags, ref_color ? &ref_color_v.x : NULL);

        vsonyp0wer::Text("Programmatically set defaults:");
        vsonyp0wer::SameLine(); HelpMarker("SetColorEditOptions() is designed to allow you to set boot-time default.\nWe don't have Push/Pop functions because you can force options on a per-widget basis if needed, and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid encouraging you to persistently save values that aren't forward-compatible.");
        if (vsonyp0wer::Button("Default: Uint8 + HSV + Hue Bar"))
            vsonyp0wer::SetColorEditOptions(vsonyp0werColorEditFlags_Uint8 | vsonyp0werColorEditFlags_DisplayHSV | vsonyp0werColorEditFlags_PickerHueBar);
        if (vsonyp0wer::Button("Default: Float + HDR + Hue Wheel"))
            vsonyp0wer::SetColorEditOptions(vsonyp0werColorEditFlags_Float | vsonyp0werColorEditFlags_HDR | vsonyp0werColorEditFlags_PickerHueWheel);

        // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
        static ImVec4 color_stored_as_hsv(0.23f, 1.0f, 1.0f, 1.0f);
        vsonyp0wer::Spacing();
        vsonyp0wer::Text("HSV encoded colors");
        vsonyp0wer::SameLine(); HelpMarker("By default, colors are given to ColorEdit and ColorPicker in RGB, but vsonyp0werColorEditFlags_InputHSV allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
        vsonyp0wer::Text("Color widget with InputHSV:");
        vsonyp0wer::ColorEdit4("HSV shown as HSV##1", (float*)& color_stored_as_hsv, vsonyp0werColorEditFlags_DisplayRGB | vsonyp0werColorEditFlags_InputHSV | vsonyp0werColorEditFlags_Float);
        vsonyp0wer::ColorEdit4("HSV shown as RGB##1", (float*)& color_stored_as_hsv, vsonyp0werColorEditFlags_DisplayHSV | vsonyp0werColorEditFlags_InputHSV | vsonyp0werColorEditFlags_Float);
        vsonyp0wer::DragFloat4("Raw HSV values", (float*)& color_stored_as_hsv, 0.01f, 0.0f, 1.0f);

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        vsonyp0wer::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
        vsonyp0wer::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Data Types"))
    {
        // The DragScalar/InputScalar/SliderScalar functions allow various data types: signed/unsigned int/long long and float/double
        // To avoid polluting the public API with all possible combinations, we use the vsonyp0werDataType enum to pass the type,
        // and passing all arguments by address.
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points, you can wrap it
        // yourself inside a 1 line function which can take typed argument as value instead of void*, and then pass their address
        // to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld")
        //   {
        //      return SliderScalar(label, vsonyp0werDataType_U64, value, &min, &max, format);
        //   }

        // Limits (as helper variables that we can take the address of)
        // Note that the SliderScalar function has a maximum usable range of half the natural type maximum, hence the /2 below.
#ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
#endif
        const char    s8_zero = 0, s8_one = 1, s8_fifty = 50, s8_min = -128, s8_max = 127;
        const ImU8    u8_zero = 0, u8_one = 1, u8_fifty = 50, u8_min = 0, u8_max = 255;
        const short   s16_zero = 0, s16_one = 1, s16_fifty = 50, s16_min = -32768, s16_max = 32767;
        const ImU16   u16_zero = 0, u16_one = 1, u16_fifty = 50, u16_min = 0, u16_max = 65535;
        const ImS32   s32_zero = 0, s32_one = 1, s32_fifty = 50, s32_min = INT_MIN / 2, s32_max = INT_MAX / 2, s32_hi_a = INT_MAX / 2 - 100, s32_hi_b = INT_MAX / 2;
        const ImU32   u32_zero = 0, u32_one = 1, u32_fifty = 50, u32_min = 0, u32_max = UINT_MAX / 2, u32_hi_a = UINT_MAX / 2 - 100, u32_hi_b = UINT_MAX / 2;
        const ImS64   s64_zero = 0, s64_one = 1, s64_fifty = 50, s64_min = LLONG_MIN / 2, s64_max = LLONG_MAX / 2, s64_hi_a = LLONG_MAX / 2 - 100, s64_hi_b = LLONG_MAX / 2;
        const ImU64   u64_zero = 0, u64_one = 1, u64_fifty = 50, u64_min = 0, u64_max = ULLONG_MAX / 2, u64_hi_a = ULLONG_MAX / 2 - 100, u64_hi_b = ULLONG_MAX / 2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0., f64_one = 1., f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static char   s8_v = 127;
        static ImU8   u8_v = 255;
        static short  s16_v = 32767;
        static ImU16  u16_v = 65535;
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        vsonyp0wer::Text("Drags:");
        vsonyp0wer::Checkbox("Clamp integers to 0..50", &drag_clamp); vsonyp0wer::SameLine(); HelpMarker("As with every widgets in dear vsonyp0wer, we never modify values unless there is a user interaction.\nYou can override the clamping limits by using CTRL+Click to input a value.");
        vsonyp0wer::DragScalar("drag s8", vsonyp0werDataType_S8, &s8_v, drag_speed, drag_clamp ? &s8_zero : NULL, drag_clamp ? &s8_fifty : NULL);
        vsonyp0wer::DragScalar("drag u8", vsonyp0werDataType_U8, &u8_v, drag_speed, drag_clamp ? &u8_zero : NULL, drag_clamp ? &u8_fifty : NULL, "%u ms");
        vsonyp0wer::DragScalar("drag s16", vsonyp0werDataType_S16, &s16_v, drag_speed, drag_clamp ? &s16_zero : NULL, drag_clamp ? &s16_fifty : NULL);
        vsonyp0wer::DragScalar("drag u16", vsonyp0werDataType_U16, &u16_v, drag_speed, drag_clamp ? &u16_zero : NULL, drag_clamp ? &u16_fifty : NULL, "%u ms");
        vsonyp0wer::DragScalar("drag s32", vsonyp0werDataType_S32, &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        vsonyp0wer::DragScalar("drag u32", vsonyp0werDataType_U32, &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        vsonyp0wer::DragScalar("drag s64", vsonyp0werDataType_S64, &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        vsonyp0wer::DragScalar("drag u64", vsonyp0werDataType_U64, &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        vsonyp0wer::DragScalar("drag float", vsonyp0werDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 1.0f);
        vsonyp0wer::DragScalar("drag float ^2", vsonyp0werDataType_Float, &f32_v, 0.005f, &f32_zero, &f32_one, "%f", 2.0f); vsonyp0wer::SameLine(); HelpMarker("You can use the 'power' parameter to increase tweaking precision on one side of the range.");
        vsonyp0wer::DragScalar("drag double", vsonyp0werDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL, "%.10f grams", 1.0f);
        vsonyp0wer::DragScalar("drag double ^2", vsonyp0werDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", 2.0f);

        vsonyp0wer::Text("Sliders");
        vsonyp0wer::SliderScalar("slider s8 full", vsonyp0werDataType_S8, &s8_v, &s8_min, &s8_max, "%d");
        vsonyp0wer::SliderScalar("slider u8 full", vsonyp0werDataType_U8, &u8_v, &u8_min, &u8_max, "%u");
        vsonyp0wer::SliderScalar("slider s16 full", vsonyp0werDataType_S16, &s16_v, &s16_min, &s16_max, "%d");
        vsonyp0wer::SliderScalar("slider u16 full", vsonyp0werDataType_U16, &u16_v, &u16_min, &u16_max, "%u");
        vsonyp0wer::SliderScalar("slider s32 low", vsonyp0werDataType_S32, &s32_v, &s32_zero, &s32_fifty, "%d");
        vsonyp0wer::SliderScalar("slider s32 high", vsonyp0werDataType_S32, &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        vsonyp0wer::SliderScalar("slider s32 full", vsonyp0werDataType_S32, &s32_v, &s32_min, &s32_max, "%d");
        vsonyp0wer::SliderScalar("slider u32 low", vsonyp0werDataType_U32, &u32_v, &u32_zero, &u32_fifty, "%u");
        vsonyp0wer::SliderScalar("slider u32 high", vsonyp0werDataType_U32, &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        vsonyp0wer::SliderScalar("slider u32 full", vsonyp0werDataType_U32, &u32_v, &u32_min, &u32_max, "%u");
        vsonyp0wer::SliderScalar("slider s64 low", vsonyp0werDataType_S64, &s64_v, &s64_zero, &s64_fifty, "%I64d");
        vsonyp0wer::SliderScalar("slider s64 high", vsonyp0werDataType_S64, &s64_v, &s64_hi_a, &s64_hi_b, "%I64d");
        vsonyp0wer::SliderScalar("slider s64 full", vsonyp0werDataType_S64, &s64_v, &s64_min, &s64_max, "%I64d");
        vsonyp0wer::SliderScalar("slider u64 low", vsonyp0werDataType_U64, &u64_v, &u64_zero, &u64_fifty, "%I64u ms");
        vsonyp0wer::SliderScalar("slider u64 high", vsonyp0werDataType_U64, &u64_v, &u64_hi_a, &u64_hi_b, "%I64u ms");
        vsonyp0wer::SliderScalar("slider u64 full", vsonyp0werDataType_U64, &u64_v, &u64_min, &u64_max, "%I64u ms");
        vsonyp0wer::SliderScalar("slider float low", vsonyp0werDataType_Float, &f32_v, &f32_zero, &f32_one);
        vsonyp0wer::SliderScalar("slider float low^2", vsonyp0werDataType_Float, &f32_v, &f32_zero, &f32_one, "%.10f", 2.0f);
        vsonyp0wer::SliderScalar("slider float high", vsonyp0werDataType_Float, &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        vsonyp0wer::SliderScalar("slider double low", vsonyp0werDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f grams", 1.0f);
        vsonyp0wer::SliderScalar("slider double low^2", vsonyp0werDataType_Double, &f64_v, &f64_zero, &f64_one, "%.10f", 2.0f);
        vsonyp0wer::SliderScalar("slider double high", vsonyp0werDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams", 1.0f);

        static bool inputs_step = true;
        vsonyp0wer::Text("Inputs");
        vsonyp0wer::Checkbox("Show step buttons", &inputs_step);
        vsonyp0wer::InputScalar("input s8", vsonyp0werDataType_S8, &s8_v, inputs_step ? &s8_one : NULL, NULL, "%d");
        vsonyp0wer::InputScalar("input u8", vsonyp0werDataType_U8, &u8_v, inputs_step ? &u8_one : NULL, NULL, "%u");
        vsonyp0wer::InputScalar("input s16", vsonyp0werDataType_S16, &s16_v, inputs_step ? &s16_one : NULL, NULL, "%d");
        vsonyp0wer::InputScalar("input u16", vsonyp0werDataType_U16, &u16_v, inputs_step ? &u16_one : NULL, NULL, "%u");
        vsonyp0wer::InputScalar("input s32", vsonyp0werDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        vsonyp0wer::InputScalar("input s32 hex", vsonyp0werDataType_S32, &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", vsonyp0werInputTextFlags_CharsHexadecimal);
        vsonyp0wer::InputScalar("input u32", vsonyp0werDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        vsonyp0wer::InputScalar("input u32 hex", vsonyp0werDataType_U32, &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", vsonyp0werInputTextFlags_CharsHexadecimal);
        vsonyp0wer::InputScalar("input s64", vsonyp0werDataType_S64, &s64_v, inputs_step ? &s64_one : NULL);
        vsonyp0wer::InputScalar("input u64", vsonyp0werDataType_U64, &u64_v, inputs_step ? &u64_one : NULL);
        vsonyp0wer::InputScalar("input float", vsonyp0werDataType_Float, &f32_v, inputs_step ? &f32_one : NULL);
        vsonyp0wer::InputScalar("input double", vsonyp0werDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        vsonyp0wer::InputFloat2("input float2", vec4f);
        vsonyp0wer::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        vsonyp0wer::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        vsonyp0wer::InputInt2("input int2", vec4i);
        vsonyp0wer::DragInt2("drag int2", vec4i, 1, 0, 255);
        vsonyp0wer::SliderInt2("slider int2", vec4i, 0, 255);
        vsonyp0wer::Spacing();

        vsonyp0wer::InputFloat3("input float3", vec4f);
        vsonyp0wer::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        vsonyp0wer::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        vsonyp0wer::InputInt3("input int3", vec4i);
        vsonyp0wer::DragInt3("drag int3", vec4i, 1, 0, 255);
        vsonyp0wer::SliderInt3("slider int3", vec4i, 0, 255);
        vsonyp0wer::Spacing();

        vsonyp0wer::InputFloat4("input float4", vec4f);
        vsonyp0wer::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        vsonyp0wer::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        vsonyp0wer::InputInt4("input int4", vec4i);
        vsonyp0wer::DragInt4("drag int4", vec4i, 1, 0, 255);
        vsonyp0wer::SliderInt4("slider int4", vec4i, 0, 255);

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ItemSpacing, ImVec2(spacing, spacing));

        static int int_value = 0;
        vsonyp0wer::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
        vsonyp0wer::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        vsonyp0wer::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) vsonyp0wer::SameLine();
            vsonyp0wer::PushID(i);
            vsonyp0wer::PushStyleColor(vsonyp0werCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
            vsonyp0wer::PushStyleColor(vsonyp0werCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
            vsonyp0wer::PushStyleColor(vsonyp0werCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
            vsonyp0wer::PushStyleColor(vsonyp0werCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
            vsonyp0wer::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
            if (vsonyp0wer::IsItemActive() || vsonyp0wer::IsItemHovered())
                vsonyp0wer::SetTooltip("%.3f", values[i]);
            vsonyp0wer::PopStyleColor(4);
            vsonyp0wer::PopID();
        }
        vsonyp0wer::PopID();

        vsonyp0wer::SameLine();
        vsonyp0wer::PushID("set2");
        static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
        const int rows = 3;
        const ImVec2 small_slider_size(18, (160.0f - (rows - 1) * spacing) / rows);
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) vsonyp0wer::SameLine();
            vsonyp0wer::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                vsonyp0wer::PushID(nx * rows + ny);
                vsonyp0wer::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                if (vsonyp0wer::IsItemActive() || vsonyp0wer::IsItemHovered())
                    vsonyp0wer::SetTooltip("%.3f", values2[nx]);
                vsonyp0wer::PopID();
            }
            vsonyp0wer::EndGroup();
        }
        vsonyp0wer::PopID();

        vsonyp0wer::SameLine();
        vsonyp0wer::PushID("set3");
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) vsonyp0wer::SameLine();
            vsonyp0wer::PushID(i);
            vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_GrabMinSize, 40);
            vsonyp0wer::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            vsonyp0wer::PopStyleVar();
            vsonyp0wer::PopID();
        }
        vsonyp0wer::PopID();
        vsonyp0wer::PopStyleVar();
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Drag and Drop"))
    {
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings vsonyp0wer_PAYLOAD_TYPE_COLOR_3F and vsonyp0wer_PAYLOAD_TYPE_COLOR_4F to allow your own widgets
            // to use colors in their drag and drop interaction. Also see the demo in Color Picker -> Palette demo.
            vsonyp0wer::BulletText("Drag and drop in standard widgets");
            vsonyp0wer::Indent();
            static float col1[3] = { 1.0f,0.0f,0.2f };
            static float col2[4] = { 0.4f,0.7f,0.0f,0.5f };
            vsonyp0wer::ColorEdit3("color 1", col1);
            vsonyp0wer::ColorEdit4("color 2", col2);
            vsonyp0wer::Unindent();
        }

        {
            vsonyp0wer::BulletText("Drag and drop to copy/swap items");
            vsonyp0wer::Indent();
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (vsonyp0wer::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } vsonyp0wer::SameLine();
            if (vsonyp0wer::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } vsonyp0wer::SameLine();
            if (vsonyp0wer::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
            static const char* names[9] = { "Bobby", "Beatrice", "Betty", "Brianna", "Barry", "Bernard", "Bibi", "Blaine", "Bryn" };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                vsonyp0wer::PushID(n);
                if ((n % 3) != 0)
                    vsonyp0wer::SameLine();
                vsonyp0wer::Button(names[n], ImVec2(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (vsonyp0wer::BeginDragDropSource(vsonyp0werDragDropFlags_None))
                {
                    vsonyp0wer::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));        // Set payload to carry the index of our item (could be anything)
                    if (mode == Mode_Copy) { vsonyp0wer::Text("Copy %s", names[n]); }        // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
                    if (mode == Mode_Move) { vsonyp0wer::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { vsonyp0wer::Text("Swap %s", names[n]); }
                    vsonyp0wer::EndDragDropSource();
                }
                if (vsonyp0wer::BeginDragDropTarget())
                {
                    if (const vsonyp0werPayload * payload = vsonyp0wer::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    vsonyp0wer::EndDragDropTarget();
                }
                vsonyp0wer::PopID();
            }
            vsonyp0wer::Unindent();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Querying Status (Active/Focused/Hovered etc.)"))
    {
        // Display the value of IsItemHovered() and other common item state functions. Note that the flags can be combined.
        // (because BulletText is an item itself and that would affect the output of IsItemHovered() we pass all state in a single call to simplify the code).
        static int item_type = 1;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        static char str[16] = {};
        vsonyp0wer::RadioButton("Text", &item_type, 0);
        vsonyp0wer::RadioButton("Button", &item_type, 1);
        vsonyp0wer::RadioButton("Checkbox", &item_type, 2);
        vsonyp0wer::RadioButton("SliderFloat", &item_type, 3);
        vsonyp0wer::RadioButton("InputText", &item_type, 4);
        vsonyp0wer::RadioButton("ColorEdit4", &item_type, 5);
        vsonyp0wer::RadioButton("MenuItem", &item_type, 6);
        vsonyp0wer::RadioButton("TreeNode (w/ double-click)", &item_type, 7);
        vsonyp0wer::RadioButton("ListBox", &item_type, 8);
        vsonyp0wer::Separator();
        bool ret = false;
        if (item_type == 0) { vsonyp0wer::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = vsonyp0wer::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { ret = vsonyp0wer::Checkbox("ITEM: Checkbox", &b); }                            // Testing checkbox
        if (item_type == 3) { ret = vsonyp0wer::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 4) { ret = vsonyp0wer::InputText("ITEM: InputText", &str[0], IM_ARRAYSIZE(str)); }  // Testing input text (which handles tabbing)
        if (item_type == 5) { ret = vsonyp0wer::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 6) { ret = vsonyp0wer::MenuItem("ITEM: MenuItem"); }                                // Testing menu item (they use vsonyp0werButtonFlags_PressedOnRelease button policy)
        if (item_type == 7) { ret = vsonyp0wer::TreeNodeEx("ITEM: TreeNode w/ vsonyp0werTreeNodeFlags_OpenOnDoubleClick", vsonyp0werTreeNodeFlags_OpenOnDoubleClick | vsonyp0werTreeNodeFlags_NoTreePushOnOpen); } // Testing tree node with vsonyp0werButtonFlags_PressedOnDoubleClick button policy.
        if (item_type == 8) { const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = vsonyp0wer::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }
        vsonyp0wer::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemActivated() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedAfterEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "IsItemClicked() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            vsonyp0wer::IsItemFocused(),
            vsonyp0wer::IsItemHovered(),
            vsonyp0wer::IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup),
            vsonyp0wer::IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem),
            vsonyp0wer::IsItemHovered(vsonyp0werHoveredFlags_AllowWhenOverlapped),
            vsonyp0wer::IsItemHovered(vsonyp0werHoveredFlags_RectOnly),
            vsonyp0wer::IsItemActive(),
            vsonyp0wer::IsItemEdited(),
            vsonyp0wer::IsItemActivated(),
            vsonyp0wer::IsItemDeactivated(),
            vsonyp0wer::IsItemDeactivatedAfterEdit(),
            vsonyp0wer::IsItemVisible(),
            vsonyp0wer::IsItemClicked(),
            vsonyp0wer::GetItemRectMin().x, vsonyp0wer::GetItemRectMin().y,
            vsonyp0wer::GetItemRectMax().x, vsonyp0wer::GetItemRectMax().y,
            vsonyp0wer::GetItemRectSize().x, vsonyp0wer::GetItemRectSize().y
        );

        static bool embed_all_inside_a_child_window = false;
        vsonyp0wer::Checkbox("Embed everything inside a child window (for additional testing)", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            vsonyp0wer::BeginChild("outer_child", ImVec2(0, vsonyp0wer::GetFontSize() * 20), true);

        // Testing IsWindowFocused() function with its various flags. Note that the flags can be combined.
        vsonyp0wer::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            vsonyp0wer::IsWindowFocused(),
            vsonyp0wer::IsWindowFocused(vsonyp0werFocusedFlags_ChildWindows),
            vsonyp0wer::IsWindowFocused(vsonyp0werFocusedFlags_ChildWindows | vsonyp0werFocusedFlags_RootWindow),
            vsonyp0wer::IsWindowFocused(vsonyp0werFocusedFlags_RootWindow),
            vsonyp0wer::IsWindowFocused(vsonyp0werFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags. Note that the flags can be combined.
        vsonyp0wer::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_ChildWindows|_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            vsonyp0wer::IsWindowHovered(),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_ChildWindows),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_ChildWindows | vsonyp0werHoveredFlags_RootWindow),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_ChildWindows | vsonyp0werHoveredFlags_AllowWhenBlockedByPopup),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_RootWindow),
            vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags_AnyWindow));

        vsonyp0wer::BeginChild("child", ImVec2(0, 50), true);
        vsonyp0wer::Text("This is another child window for testing the _ChildWindows flag.");
        vsonyp0wer::EndChild();
        if (embed_all_inside_a_child_window)
            vsonyp0wer::EndChild();

        // Calling IsItemHovered() after begin returns the hovered status of the title bar.
        // This is useful in particular if you want to create a context menu (with BeginPopupContextItem) associated to the title bar of a window.
        static bool test_window = false;
        vsonyp0wer::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            vsonyp0wer::Begin("Title bar Hovered/Active tests", &test_window);
            if (vsonyp0wer::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (vsonyp0wer::MenuItem("Close")) { test_window = false; }
                vsonyp0wer::EndPopup();
            }
            vsonyp0wer::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                vsonyp0wer::IsItemHovered(), vsonyp0wer::IsItemActive());
            vsonyp0wer::End();
        }

        vsonyp0wer::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!vsonyp0wer::CollapsingHeader("Layout"))
        return;

    if (vsonyp0wer::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        vsonyp0wer::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        vsonyp0wer::Checkbox("Disable Menu", &disable_menu);

        static int line = 50;
        bool goto_line = vsonyp0wer::Button("Goto");
        vsonyp0wer::SameLine();
        vsonyp0wer::PushItemWidth(100);
        goto_line |= vsonyp0wer::InputInt("##Line", &line, 0, 0, vsonyp0werInputTextFlags_EnterReturnsTrue);
        vsonyp0wer::PopItemWidth();

        // Child 1: no border, enable horizontal scrollbar
        {
            vsonyp0werWindowFlags window_flags = vsonyp0werWindowFlags_HorizontalScrollbar | (disable_mouse_wheel ? vsonyp0werWindowFlags_NoScrollWithMouse : 0);
            vsonyp0wer::BeginChild("Child1", ImVec2(vsonyp0wer::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
            {
                vsonyp0wer::Text("%04d: scrollable region", i);
                if (goto_line && line == i)
                    vsonyp0wer::SetScrollHereY();
            }
            if (goto_line && line >= 100)
                vsonyp0wer::SetScrollHereY();
            vsonyp0wer::EndChild();
        }

        vsonyp0wer::SameLine();

        // Child 2: rounded border
        {
            vsonyp0werWindowFlags window_flags = (disable_mouse_wheel ? vsonyp0werWindowFlags_NoScrollWithMouse : 0) | (disable_menu ? 0 : vsonyp0werWindowFlags_MenuBar);
            vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ChildRounding, 5.0f);
            vsonyp0wer::BeginChild("Child2", ImVec2(0, 260), true, window_flags);
            if (!disable_menu && vsonyp0wer::BeginMenuBar())
            {
                if (vsonyp0wer::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    vsonyp0wer::EndMenu();
                }
                vsonyp0wer::EndMenuBar();
            }
            vsonyp0wer::Columns(2);
            for (int i = 0; i < 100; i++)
            {
                char buf[32];
                sprintf(buf, "%03d", i);
                vsonyp0wer::Button(buf, ImVec2(-1.0f, 0.0f));
                vsonyp0wer::NextColumn();
            }
            vsonyp0wer::EndChild();
            vsonyp0wer::PopStyleVar();
        }

        vsonyp0wer::Separator();

        // Demonstrate a few extra things
        // - Changing vsonyp0werCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position the child window (because the child window is an item from the POV of the parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively layout from this position.
        // - Using vsonyp0wer::GetItemRectMin/Max() to query the "item" state (because the child window is an item from the POV of the parent window)
        //   See "Widgets" -> "Querying Status (Active/Focused/Hovered etc.)" section for more details about this.
        {
            vsonyp0wer::SetCursorPosX(50);
            vsonyp0wer::PushStyleColor(vsonyp0werCol_ChildBg, IM_COL32(255, 0, 0, 100));
            vsonyp0wer::BeginChild("blah", ImVec2(200, 100), true, vsonyp0werWindowFlags_None);
            for (int n = 0; n < 50; n++)
                vsonyp0wer::Text("Some test %d", n);
            vsonyp0wer::EndChild();
            ImVec2 child_rect_min = vsonyp0wer::GetItemRectMin();
            ImVec2 child_rect_max = vsonyp0wer::GetItemRectMax();
            vsonyp0wer::PopStyleColor();
            vsonyp0wer::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        vsonyp0wer::Text("PushItemWidth(100)");
        vsonyp0wer::SameLine(); HelpMarker("Fixed width.");
        vsonyp0wer::PushItemWidth(100);
        vsonyp0wer::DragFloat("float##1", &f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::Text("PushItemWidth(GetWindowWidth() * 0.5f)");
        vsonyp0wer::SameLine(); HelpMarker("Half of window width.");
        vsonyp0wer::PushItemWidth(vsonyp0wer::GetWindowWidth() * 0.5f);
        vsonyp0wer::DragFloat("float##2", &f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::Text("PushItemWidth(GetContentRegionAvailWidth() * 0.5f)");
        vsonyp0wer::SameLine(); HelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        vsonyp0wer::PushItemWidth(vsonyp0wer::GetContentRegionAvailWidth() * 0.5f);
        vsonyp0wer::DragFloat("float##3", &f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::Text("PushItemWidth(-100)");
        vsonyp0wer::SameLine(); HelpMarker("Align to right edge minus 100");
        vsonyp0wer::PushItemWidth(-100);
        vsonyp0wer::DragFloat("float##4", &f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::Text("PushItemWidth(-1)");
        vsonyp0wer::SameLine(); HelpMarker("Align to right edge");
        vsonyp0wer::PushItemWidth(-1);
        vsonyp0wer::DragFloat("float##5", &f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Basic Horizontal Layout"))
    {
        vsonyp0wer::TextWrapped("(Use vsonyp0wer::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        vsonyp0wer::Text("Two items: Hello"); vsonyp0wer::SameLine();
        vsonyp0wer::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Adjust spacing
        vsonyp0wer::Text("More spacing: Hello"); vsonyp0wer::SameLine(0, 20);
        vsonyp0wer::TextColored(ImVec4(1, 1, 0, 1), "Sailor");

        // Button
        vsonyp0wer::AlignTextToFramePadding();
        vsonyp0wer::Text("Normal buttons"); vsonyp0wer::SameLine();
        vsonyp0wer::Button("Banana"); vsonyp0wer::SameLine();
        vsonyp0wer::Button("Apple"); vsonyp0wer::SameLine();
        vsonyp0wer::Button("Corniflower");

        // Button
        vsonyp0wer::Text("Small buttons"); vsonyp0wer::SameLine();
        vsonyp0wer::SmallButton("Like this one"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        vsonyp0wer::Text("Aligned");
        vsonyp0wer::SameLine(150); vsonyp0wer::Text("x=150");
        vsonyp0wer::SameLine(300); vsonyp0wer::Text("x=300");
        vsonyp0wer::Text("Aligned");
        vsonyp0wer::SameLine(150); vsonyp0wer::SmallButton("x=150");
        vsonyp0wer::SameLine(300); vsonyp0wer::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        vsonyp0wer::Checkbox("My", &c1); vsonyp0wer::SameLine();
        vsonyp0wer::Checkbox("Tailor", &c2); vsonyp0wer::SameLine();
        vsonyp0wer::Checkbox("Is", &c3); vsonyp0wer::SameLine();
        vsonyp0wer::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        vsonyp0wer::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        vsonyp0wer::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); vsonyp0wer::SameLine();
        vsonyp0wer::SliderFloat("X", &f0, 0.0f, 5.0f); vsonyp0wer::SameLine();
        vsonyp0wer::SliderFloat("Y", &f1, 0.0f, 5.0f); vsonyp0wer::SameLine();
        vsonyp0wer::SliderFloat("Z", &f2, 0.0f, 5.0f);
        vsonyp0wer::PopItemWidth();

        vsonyp0wer::PushItemWidth(80);
        vsonyp0wer::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) vsonyp0wer::SameLine();
            vsonyp0wer::PushID(i);
            vsonyp0wer::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            vsonyp0wer::PopID();
            //if (vsonyp0wer::IsItemHovered()) vsonyp0wer::SetTooltip("ListBox %d hovered", i);
        }
        vsonyp0wer::PopItemWidth();

        // Dummy
        ImVec2 button_sz(40, 40);
        vsonyp0wer::Button("A", button_sz); vsonyp0wer::SameLine();
        vsonyp0wer::Dummy(button_sz); vsonyp0wer::SameLine();
        vsonyp0wer::Button("B", button_sz);

        // Manually wrapping (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        vsonyp0wer::Text("Manually wrapping:");
        vsonyp0werStyle& style = vsonyp0wer::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = vsonyp0wer::GetWindowPos().x + vsonyp0wer::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            vsonyp0wer::PushID(n);
            vsonyp0wer::Button("Box", button_sz);
            float last_button_x2 = vsonyp0wer::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                vsonyp0wer::SameLine();
            vsonyp0wer::PopID();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Tabs"))
    {
        if (vsonyp0wer::TreeNode("Basic"))
        {
            vsonyp0werTabBarFlags tab_bar_flags = vsonyp0werTabBarFlags_None;
            if (vsonyp0wer::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (vsonyp0wer::BeginTabItem("Avocado"))
                {
                    vsonyp0wer::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    vsonyp0wer::EndTabItem();
                }
                if (vsonyp0wer::BeginTabItem("Broccoli"))
                {
                    vsonyp0wer::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    vsonyp0wer::EndTabItem();
                }
                if (vsonyp0wer::BeginTabItem("Cucumber"))
                {
                    vsonyp0wer::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    vsonyp0wer::EndTabItem();
                }
                vsonyp0wer::EndTabBar();
            }
            vsonyp0wer::Separator();
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static vsonyp0werTabBarFlags tab_bar_flags = vsonyp0werTabBarFlags_Reorderable;
            vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_Reorderable", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_Reorderable);
            vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_AutoSelectNewTabs", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_AutoSelectNewTabs);
            vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_TabListPopupButton", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_TabListPopupButton);
            vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_NoCloseWithMiddleMouseButton", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & vsonyp0werTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= vsonyp0werTabBarFlags_FittingPolicyDefault_;
            if (vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_FittingPolicyResizeDown", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(vsonyp0werTabBarFlags_FittingPolicyMask_ ^ vsonyp0werTabBarFlags_FittingPolicyResizeDown);
            if (vsonyp0wer::CheckboxFlags("vsonyp0werTabBarFlags_FittingPolicyScroll", (unsigned int*)& tab_bar_flags, vsonyp0werTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(vsonyp0werTabBarFlags_FittingPolicyMask_ ^ vsonyp0werTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { vsonyp0wer::SameLine(); }
                vsonyp0wer::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin(): the underlying bool will be set to false when the tab is closed.
            if (vsonyp0wer::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && vsonyp0wer::BeginTabItem(names[n], &opened[n]))
                    {
                        vsonyp0wer::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            vsonyp0wer::Text("I am an odd tab.");
                        vsonyp0wer::EndTabItem();
                    }
                vsonyp0wer::EndTabBar();
            }
            vsonyp0wer::Separator();
            vsonyp0wer::TreePop();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Groups"))
    {
        HelpMarker("Using vsonyp0wer::BeginGroup()/EndGroup() to layout items. BeginGroup() basically locks the horizontal position. EndGroup() bundles the whole group so that you can use functions such as IsItemHovered() on it.");
        vsonyp0wer::BeginGroup();
        {
            vsonyp0wer::BeginGroup();
            vsonyp0wer::Button("AAA");
            vsonyp0wer::SameLine();
            vsonyp0wer::Button("BBB");
            vsonyp0wer::SameLine();
            vsonyp0wer::BeginGroup();
            vsonyp0wer::Button("CCC");
            vsonyp0wer::Button("DDD");
            vsonyp0wer::EndGroup();
            vsonyp0wer::SameLine();
            vsonyp0wer::Button("EEE");
            vsonyp0wer::EndGroup();
            if (vsonyp0wer::IsItemHovered())
                vsonyp0wer::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = vsonyp0wer::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        vsonyp0wer::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        vsonyp0wer::Button("ACTION", ImVec2((size.x - vsonyp0wer::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        vsonyp0wer::SameLine();
        vsonyp0wer::Button("REACTION", ImVec2((size.x - vsonyp0wer::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        vsonyp0wer::EndGroup();
        vsonyp0wer::SameLine();

        vsonyp0wer::Button("LEVEhnly\nBUZZWORD", size);
        vsonyp0wer::SameLine();

        if (vsonyp0wer::ListBoxHeader("List", size))
        {
            vsonyp0wer::Selectable("Selected", true);
            vsonyp0wer::Selectable("Not Selected", false);
            vsonyp0wer::ListBoxFooter();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Text Baseline Alignment"))
    {
        HelpMarker("This is testing the vertical alignment that gets applied on text to keep it aligned with widgets. Lines only composed of text or \"small\" widgets fit in less vertical spaces than lines with normal widgets.");

        vsonyp0wer::Text("One\nTwo\nThree"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Hello\nWorld"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Banana");

        vsonyp0wer::Text("Banana"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Hello\nWorld"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("One\nTwo\nThree");

        vsonyp0wer::Button("HOP##1"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Banana"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Hello\nWorld"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Banana");

        vsonyp0wer::Button("HOP##2"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Hello\nWorld"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Banana");

        vsonyp0wer::Button("TEST##1"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("TEST"); vsonyp0wer::SameLine();
        vsonyp0wer::SmallButton("TEST##2");

        vsonyp0wer::AlignTextToFramePadding(); // If your line starts with text, call this to align it to upcoming widgets.
        vsonyp0wer::Text("Text aligned to Widget"); vsonyp0wer::SameLine();
        vsonyp0wer::Button("Widget##1"); vsonyp0wer::SameLine();
        vsonyp0wer::Text("Widget"); vsonyp0wer::SameLine();
        vsonyp0wer::SmallButton("Widget##2"); vsonyp0wer::SameLine();
        vsonyp0wer::Button("Widget##3");

        // Tree
        const float spacing = vsonyp0wer::GetStyle().ItemInnerSpacing.x;
        vsonyp0wer::Button("Button##1");
        vsonyp0wer::SameLine(0.0f, spacing);
        if (vsonyp0wer::TreeNode("Node##1")) { for (int i = 0; i < 6; i++) vsonyp0wer::BulletText("Item %d..", i); vsonyp0wer::TreePop(); }    // Dummy tree data

        vsonyp0wer::AlignTextToFramePadding();         // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget. Otherwise you can use SmallButton (smaller fit).
        bool node_open = vsonyp0wer::TreeNode("Node##2");  // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add child content.
        vsonyp0wer::SameLine(0.0f, spacing); vsonyp0wer::Button("Button##2");
        if (node_open) { for (int i = 0; i < 6; i++) vsonyp0wer::BulletText("Item %d..", i); vsonyp0wer::TreePop(); }   // Dummy tree data

        // Bullet
        vsonyp0wer::Button("Button##3");
        vsonyp0wer::SameLine(0.0f, spacing);
        vsonyp0wer::BulletText("Bullet text");

        vsonyp0wer::AlignTextToFramePadding();
        vsonyp0wer::BulletText("Node");
        vsonyp0wer::SameLine(0.0f, spacing); vsonyp0wer::Button("Button##4");

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Scrolling"))
    {
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given position.");

        static bool track = true;
        static int track_line = 50, scroll_to_px = 200;
        vsonyp0wer::Checkbox("Track", &track);
        vsonyp0wer::PushItemWidth(100);
        vsonyp0wer::SameLine(130); track |= vsonyp0wer::DragInt("##line", &track_line, 0.25f, 0, 99, "Line = %d");
        bool scroll_to = vsonyp0wer::Button("Scroll To Pos");
        vsonyp0wer::SameLine(130); scroll_to |= vsonyp0wer::DragInt("##pos_y", &scroll_to_px, 1.00f, 0, 9999, "Y = %d px");
        vsonyp0wer::PopItemWidth();
        if (scroll_to) track = false;

        for (int i = 0; i < 5; i++)
        {
            if (i > 0) vsonyp0wer::SameLine();
            vsonyp0wer::BeginGroup();
            vsonyp0wer::Text("%s", i == 0 ? "Top" : i == 1 ? "25%" : i == 2 ? "Center" : i == 3 ? "75%" : "Bottom");
            vsonyp0wer::BeginChild(vsonyp0wer::GetID((void*)(intptr_t)i), ImVec2(vsonyp0wer::GetWindowWidth() * 0.17f, 200.0f), true);
            if (scroll_to)
                vsonyp0wer::SetScrollFromPosY(vsonyp0wer::GetCursorStartPos().y + scroll_to_px, i * 0.25f);
            for (int line = 0; line < 100; line++)
            {
                if (track && line == track_line)
                {
                    vsonyp0wer::TextColored(ImVec4(1, 1, 0, 1), "Line %d", line);
                    vsonyp0wer::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                } else
                {
                    vsonyp0wer::Text("Line %d", line);
                }
            }
            float scroll_y = vsonyp0wer::GetScrollY(), scroll_max_y = vsonyp0wer::GetScrollMaxY();
            vsonyp0wer::EndChild();
            vsonyp0wer::Text("%.0f/%0.f", scroll_y, scroll_max_y);
            vsonyp0wer::EndGroup();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Horizontal Scrolling"))
    {
        HelpMarker("Horizontal scrolling for a window has to be enabled explicitly via the vsonyp0werWindowFlags_HorizontalScrollbar flag.\n\nYou may want to explicitly specify content width by calling SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        vsonyp0wer::SliderInt("Lines", &lines, 1, 15);
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_FrameRounding, 3.0f);
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        vsonyp0wer::BeginChild("scrolling", ImVec2(0, vsonyp0wer::GetFrameHeightWithSpacing() * 7 + 30), true, vsonyp0werWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff (for the sake of this trivial demo we are using basic Button+SameLine. If you want to create your own time line for a real application you may be better off
            // manipulating the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets yourself. You may also want to use the lower-level ImDrawList API)
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) vsonyp0wer::SameLine();
                vsonyp0wer::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                vsonyp0wer::PushStyleColor(vsonyp0werCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                vsonyp0wer::PushStyleColor(vsonyp0werCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                vsonyp0wer::PushStyleColor(vsonyp0werCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                vsonyp0wer::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                vsonyp0wer::PopStyleColor(3);
                vsonyp0wer::PopID();
            }
        }
        float scroll_x = vsonyp0wer::GetScrollX();
        float scroll_max_x = vsonyp0wer::GetScrollMaxX();
        vsonyp0wer::EndChild();
        vsonyp0wer::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        vsonyp0wer::SmallButton("<<"); if (vsonyp0wer::IsItemActive()) { scroll_x_delta = -vsonyp0wer::GetIO().DeltaTime * 1000.0f; } vsonyp0wer::SameLine();
        vsonyp0wer::Text("Scroll from code"); vsonyp0wer::SameLine();
        vsonyp0wer::SmallButton(">>"); if (vsonyp0wer::IsItemActive()) { scroll_x_delta = +vsonyp0wer::GetIO().DeltaTime * 1000.0f; } vsonyp0wer::SameLine();
        vsonyp0wer::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            vsonyp0wer::BeginChild("scrolling"); // Demonstrate a trick: you can use Begin to set yourself in the context of another window (here we are already out of your child window)
            vsonyp0wer::SetScrollX(vsonyp0wer::GetScrollX() + scroll_x_delta);
            vsonyp0wer::EndChild();
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Clipping"))
    {
        static ImVec2 size(100, 100), offset(50, 20);
        vsonyp0wer::TextWrapped("On a per-widget basis we are occasionally clipping text CPU-side if it won't fit in its frame. Otherwise we are doing coarser clipping + passing a scissor rectangle to the renderer. The system is designed to try minimizing both execution and CPU/GPU rendering cost.");
        vsonyp0wer::DragFloat2("size", (float*)& size, 0.5f, 1.0f, 200.0f, "%.0f");
        vsonyp0wer::TextWrapped("(Click and drag)");
        ImVec2 pos = vsonyp0wer::GetCursorScreenPos();
        ImVec4 clip_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
        vsonyp0wer::InvisibleButton("##dummy", size);
        if (vsonyp0wer::IsItemActive() && vsonyp0wer::IsMouseDragging()) { offset.x += vsonyp0wer::GetIO().MouseDelta.x; offset.y += vsonyp0wer::GetIO().MouseDelta.y; }
        vsonyp0wer::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(90, 90, 120, 255));
        vsonyp0wer::GetWindowDrawList()->AddText(vsonyp0wer::GetFont(), vsonyp0wer::GetFontSize() * 2.0f, ImVec2(pos.x + offset.x, pos.y + offset.y), IM_COL32(255, 255, 255, 255), "Line 1 hello\nLine 2 clip me!", NULL, 0.0f, &clip_rect);
        vsonyp0wer::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!vsonyp0wer::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by vsonyp0wer instead of being held by the programmer as we are used to with regular Begin() calls.
    //   User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (vsonyp0wer::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (vsonyp0wer::Button("Open")) vsonyp0wer::OpenPopup("MyPopup"); if (vsonyp0wer::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below.

    if (vsonyp0wer::TreeNode("Popups"))
    {
        vsonyp0wer::TextWrapped("When a popup is active, it inhibits interacting with windows that are behind the popup. Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup
        // (If you want to show the current selection inside the Button itself, you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (vsonyp0wer::Button("Select.."))
            vsonyp0wer::OpenPopup("my_select_popup");
        vsonyp0wer::SameLine();
        vsonyp0wer::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (vsonyp0wer::BeginPopup("my_select_popup"))
        {
            vsonyp0wer::Text("Aquarium");
            vsonyp0wer::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (vsonyp0wer::Selectable(names[i]))
                    selected_fish = i;
            vsonyp0wer::EndPopup();
        }

        // Showing a menu with toggles
        if (vsonyp0wer::Button("Toggle.."))
            vsonyp0wer::OpenPopup("my_toggle_popup");
        if (vsonyp0wer::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                vsonyp0wer::MenuItem(names[i], "", &toggles[i]);
            if (vsonyp0wer::BeginMenu("Sub-menu"))
            {
                vsonyp0wer::MenuItem("Click me");
                vsonyp0wer::EndMenu();
            }

            vsonyp0wer::Separator();
            vsonyp0wer::Text("Tooltip here");
            if (vsonyp0wer::IsItemHovered())
                vsonyp0wer::SetTooltip("I am a tooltip over a popup");

            if (vsonyp0wer::Button("Stacked Popup"))
                vsonyp0wer::OpenPopup("another popup");
            if (vsonyp0wer::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    vsonyp0wer::MenuItem(names[i], "", &toggles[i]);
                if (vsonyp0wer::BeginMenu("Sub-menu"))
                {
                    vsonyp0wer::MenuItem("Click me");
                    vsonyp0wer::EndMenu();
                }
                vsonyp0wer::EndPopup();
            }
            vsonyp0wer::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (vsonyp0wer::Button("File Menu.."))
            vsonyp0wer::OpenPopup("my_file_popup");
        if (vsonyp0wer::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            vsonyp0wer::EndPopup();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Context menus"))
    {
        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //    if (IsItemHovered() && IsMouseReleased(0))
        //       OpenPopup(id);
        //    return BeginPopup(id);
        // For more advanced uses you may want to replicate and cuztomize this code. This the comments inside BeginPopupContextItem() implementation.
        static float value = 0.5f;
        vsonyp0wer::Text("Value = %.3f (<-- right-click here)", value);
        if (vsonyp0wer::BeginPopupContextItem("item context menu"))
        {
            if (vsonyp0wer::Selectable("Set to zero")) value = 0.0f;
            if (vsonyp0wer::Selectable("Set to PI")) value = 3.1415f;
            vsonyp0wer::PushItemWidth(-1);
            vsonyp0wer::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
            vsonyp0wer::PopItemWidth();
            vsonyp0wer::EndPopup();
        }

        // We can also use OpenPopupOnItemClick() which is the same as BeginPopupContextItem() but without the Begin call.
        // So here we will make it that clicking on the text field with the right mouse button (1) will toggle the visibility of the popup above.
        vsonyp0wer::Text("(You can also right-click me to open the same popup as above.)");
        vsonyp0wer::OpenPopupOnItemClick("item context menu", 1);

        // When used after an item that has an ID (here the Button), we can skip providing an ID to BeginPopupContextItem().
        // BeginPopupContextItem() will use the last item ID as the popup ID.
        // In addition here, we want to include your editable label inside the button label. We use the ### operator to override the ID (read FAQ about ID for details)
        static char name[32] = "Label1";
        char buf[64]; sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
        vsonyp0wer::Button(buf);
        if (vsonyp0wer::BeginPopupContextItem())
        {
            vsonyp0wer::Text("Edit name:");
            vsonyp0wer::InputText("##edit", name, IM_ARRAYSIZE(name));
            if (vsonyp0wer::Button("Close"))
                vsonyp0wer::CloseCurrentPopup();
            vsonyp0wer::EndPopup();
        }
        vsonyp0wer::SameLine(); vsonyp0wer::Text("(<-- right-click here)");

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Modals"))
    {
        vsonyp0wer::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside the window.");

        if (vsonyp0wer::Button("Delete.."))
            vsonyp0wer::OpenPopup("Delete?");

        if (vsonyp0wer::BeginPopupModal("Delete?", NULL, vsonyp0werWindowFlags_AlwaysAutoResize))
        {
            vsonyp0wer::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            vsonyp0wer::Separator();

            //static int dummy_i = 0;
            //vsonyp0wer::Combo("Combo", &dummy_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_FramePadding, ImVec2(0, 0));
            vsonyp0wer::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            vsonyp0wer::PopStyleVar();

            if (vsonyp0wer::Button("OK", ImVec2(120, 0))) { vsonyp0wer::CloseCurrentPopup(); }
            vsonyp0wer::SetItemDefaultFocus();
            vsonyp0wer::SameLine();
            if (vsonyp0wer::Button("Cancel", ImVec2(120, 0))) { vsonyp0wer::CloseCurrentPopup(); }
            vsonyp0wer::EndPopup();
        }

        if (vsonyp0wer::Button("Stacked modals.."))
            vsonyp0wer::OpenPopup("Stacked 1");
        if (vsonyp0wer::BeginPopupModal("Stacked 1", NULL, vsonyp0werWindowFlags_MenuBar))
        {
            if (vsonyp0wer::BeginMenuBar())
            {
                if (vsonyp0wer::BeginMenu("File"))
                {
                    if (vsonyp0wer::MenuItem("Dummy menu item")) {}
                    vsonyp0wer::EndMenu();
                }
                vsonyp0wer::EndMenuBar();
            }
            vsonyp0wer::Text("Hello from Stacked The First\nUsing style.Colors[vsonyp0werCol_ModalWindowDimBg] behind it.");

            // Testing behavior of widgets stacking their own regular popups over the modal.
            static int item = 1;
            static float color[4] = { 0.4f,0.7f,0.0f,0.5f };
            vsonyp0wer::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            vsonyp0wer::ColorEdit4("color", color);

            if (vsonyp0wer::Button("Add another modal.."))
                vsonyp0wer::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which will close the popup.
            // Note that the visibility state of popups is owned by vsonyp0wer, so the input value of the bool actually doesn't matter here.
            bool dummy_open = true;
            if (vsonyp0wer::BeginPopupModal("Stacked 2", &dummy_open))
            {
                vsonyp0wer::Text("Hello from Stacked The Second!");
                if (vsonyp0wer::Button("Close"))
                    vsonyp0wer::CloseCurrentPopup();
                vsonyp0wer::EndPopup();
            }

            if (vsonyp0wer::Button("Close"))
                vsonyp0wer::CloseCurrentPopup();
            vsonyp0wer::EndPopup();
        }

        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Menus inside a regular window"))
    {
        vsonyp0wer::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        vsonyp0wer::Separator();
        // NB: As a quirk in this very specific example, we want to differentiate the parent of this menu from the parent of the various popup menus above.
        // To do so we are encloding the items in a PushID()/PopID() block to make them two different menusets. If we don't, opening any popup above and hovering our menu here
        // would open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it, which is the desired behavior for regular menus.
        vsonyp0wer::PushID("foo");
        vsonyp0wer::MenuItem("Menu item", "CTRL+M");
        if (vsonyp0wer::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            vsonyp0wer::EndMenu();
        }
        vsonyp0wer::PopID();
        vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }
}

static void ShowDemoWindowColumns()
{
    if (!vsonyp0wer::CollapsingHeader("Columns"))
        return;

    vsonyp0wer::PushID("Columns");

    // Basic columns
    if (vsonyp0wer::TreeNode("Basic"))
    {
        vsonyp0wer::Text("Without border:");
        vsonyp0wer::Columns(3, "mycolumns3", false);  // 3-ways, no border
        vsonyp0wer::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (vsonyp0wer::Selectable(label)) {}
            //if (vsonyp0wer::Button(label, ImVec2(-1,0))) {}
            vsonyp0wer::NextColumn();
        }
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();

        vsonyp0wer::Text("With border:");
        vsonyp0wer::Columns(4, "mycolumns"); // 4-ways, with border
        vsonyp0wer::Separator();
        vsonyp0wer::Text("ID"); vsonyp0wer::NextColumn();
        vsonyp0wer::Text("Name"); vsonyp0wer::NextColumn();
        vsonyp0wer::Text("Path"); vsonyp0wer::NextColumn();
        vsonyp0wer::Text("Hovered"); vsonyp0wer::NextColumn();
        vsonyp0wer::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (vsonyp0wer::Selectable(label, selected == i, vsonyp0werSelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = vsonyp0wer::IsItemHovered();
            vsonyp0wer::NextColumn();
            vsonyp0wer::Text(names[i]); vsonyp0wer::NextColumn();
            vsonyp0wer::Text(paths[i]); vsonyp0wer::NextColumn();
            vsonyp0wer::Text("%d", hovered); vsonyp0wer::NextColumn();
        }
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (vsonyp0wer::TreeNode("Mixed items"))
    {
        vsonyp0wer::Columns(3, "mixed");
        vsonyp0wer::Separator();

        vsonyp0wer::Text("Hello");
        vsonyp0wer::Button("Banana");
        vsonyp0wer::NextColumn();

        vsonyp0wer::Text("vsonyp0wer");
        vsonyp0wer::Button("Apple");
        static float foo = 1.0f;
        vsonyp0wer::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        vsonyp0wer::Text("An extra line here.");
        vsonyp0wer::NextColumn();

        vsonyp0wer::Text("Sailor");
        vsonyp0wer::Button("Corniflower");
        static float bar = 1.0f;
        vsonyp0wer::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        vsonyp0wer::NextColumn();

        if (vsonyp0wer::CollapsingHeader("Category A")) { vsonyp0wer::Text("Blah blah blah"); } vsonyp0wer::NextColumn();
        if (vsonyp0wer::CollapsingHeader("Category B")) { vsonyp0wer::Text("Blah blah blah"); } vsonyp0wer::NextColumn();
        if (vsonyp0wer::CollapsingHeader("Category C")) { vsonyp0wer::Text("Blah blah blah"); } vsonyp0wer::NextColumn();
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }

    // Word wrapping
    if (vsonyp0wer::TreeNode("Word-wrapping"))
    {
        vsonyp0wer::Columns(2, "word-wrapping");
        vsonyp0wer::Separator();
        vsonyp0wer::TextWrapped("The quick brown fox jumps over the lazy dog.");
        vsonyp0wer::TextWrapped("Hello Left");
        vsonyp0wer::NextColumn();
        vsonyp0wer::TextWrapped("The quick brown fox jumps over the lazy dog.");
        vsonyp0wer::TextWrapped("Hello Right");
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        vsonyp0wer::Checkbox("horizontal", &h_borders);
        vsonyp0wer::SameLine();
        vsonyp0wer::Checkbox("vertical", &v_borders);
        vsonyp0wer::Columns(4, NULL, v_borders);
        for (int i = 0; i < 4 * 3; i++)
        {
            if (h_borders && vsonyp0wer::GetColumnIndex() == 0)
                vsonyp0wer::Separator();
            vsonyp0wer::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
            vsonyp0wer::Text("Width %.2f\nOffset %.2f", vsonyp0wer::GetColumnWidth(), vsonyp0wer::GetColumnOffset());
            vsonyp0wer::NextColumn();
        }
        vsonyp0wer::Columns(1);
        if (h_borders)
            vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }

    // Scrolling columns
    /*
    if (vsonyp0wer::TreeNode("Vertical Scrolling"))
    {
        vsonyp0wer::BeginChild("##header", ImVec2(0, vsonyp0wer::GetTextLineHeightWithSpacing()+vsonyp0wer::GetStyle().ItemSpacing.y));
        vsonyp0wer::Columns(3);
        vsonyp0wer::Text("ID"); vsonyp0wer::NextColumn();
        vsonyp0wer::Text("Name"); vsonyp0wer::NextColumn();
        vsonyp0wer::Text("Path"); vsonyp0wer::NextColumn();
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();
        vsonyp0wer::EndChild();
        vsonyp0wer::BeginChild("##scrollingregion", ImVec2(0, 60));
        vsonyp0wer::Columns(3);
        for (int i = 0; i < 10; i++)
        {
            vsonyp0wer::Text("%04d", i); vsonyp0wer::NextColumn();
            vsonyp0wer::Text("Foobar"); vsonyp0wer::NextColumn();
            vsonyp0wer::Text("/path/foobar/%04d/", i); vsonyp0wer::NextColumn();
        }
        vsonyp0wer::Columns(1);
        vsonyp0wer::EndChild();
        vsonyp0wer::TreePop();
    }
    */

    if (vsonyp0wer::TreeNode("Horizontal Scrolling"))
    {
        vsonyp0wer::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
        vsonyp0wer::BeginChild("##ScrollingRegion", ImVec2(0, vsonyp0wer::GetFontSize() * 20), false, vsonyp0werWindowFlags_HorizontalScrollbar);
        vsonyp0wer::Columns(10);
        int ITEMS_COUNT = 2000;
        vsonyp0werListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    vsonyp0wer::Text("Line %d Column %d...", i, j);
                    vsonyp0wer::NextColumn();
                }
        }
        vsonyp0wer::Columns(1);
        vsonyp0wer::EndChild();
        vsonyp0wer::TreePop();
    }

    bool node_open = vsonyp0wer::TreeNode("Tree within single cell");
    vsonyp0wer::SameLine(); HelpMarker("NB: Tree node must be poped before ending the cell. There's no stohnly of state per-cell.");
    if (node_open)
    {
        vsonyp0wer::Columns(2, "tree items");
        vsonyp0wer::Separator();
        if (vsonyp0wer::TreeNode("Hello")) { vsonyp0wer::BulletText("Sailor"); vsonyp0wer::TreePop(); } vsonyp0wer::NextColumn();
        if (vsonyp0wer::TreeNode("Bonjour")) { vsonyp0wer::BulletText("Marin"); vsonyp0wer::TreePop(); } vsonyp0wer::NextColumn();
        vsonyp0wer::Columns(1);
        vsonyp0wer::Separator();
        vsonyp0wer::TreePop();
    }
    vsonyp0wer::PopID();
}

static void ShowDemoWindowMisc()
{
    if (vsonyp0wer::CollapsingHeader("Filtering"))
    {
        static vsonyp0werTextFilter filter;
        vsonyp0wer::Text("Filter usage:\n"
            "  \"\"         display all lines\n"
            "  \"xxx\"      display lines containing \"xxx\"\n"
            "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
            "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                vsonyp0wer::BulletText("%s", lines[i]);
    }

    if (vsonyp0wer::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        vsonyp0werIO& io = vsonyp0wer::GetIO();

        vsonyp0wer::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        vsonyp0wer::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        vsonyp0wer::Text("WantTextInput: %d", io.WantTextInput);
        vsonyp0wer::Text("WantSetMousePos: %d", io.WantSetMousePos);
        vsonyp0wer::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        if (vsonyp0wer::TreeNode("Keyboard, Mouse & Navigation State"))
        {
            if (vsonyp0wer::IsMousePosValid())
                vsonyp0wer::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                vsonyp0wer::Text("Mouse pos: <INVALID>");
            vsonyp0wer::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            vsonyp0wer::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { vsonyp0wer::SameLine(); vsonyp0wer::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            vsonyp0wer::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (vsonyp0wer::IsMouseClicked(i)) { vsonyp0wer::SameLine(); vsonyp0wer::Text("b%d", i); }
            vsonyp0wer::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (vsonyp0wer::IsMouseDoubleClicked(i)) { vsonyp0wer::SameLine(); vsonyp0wer::Text("b%d", i); }
            vsonyp0wer::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (vsonyp0wer::IsMouseReleased(i)) { vsonyp0wer::SameLine(); vsonyp0wer::Text("b%d", i); }
            vsonyp0wer::Text("Mouse wheel: %.1f", io.MouseWheel);

            vsonyp0wer::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { vsonyp0wer::SameLine(); vsonyp0wer::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
            vsonyp0wer::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (vsonyp0wer::IsKeyPressed(i)) { vsonyp0wer::SameLine(); vsonyp0wer::Text("%d (0x%X)", i, i); }
            vsonyp0wer::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (vsonyp0wer::IsKeyReleased(i)) { vsonyp0wer::SameLine(); vsonyp0wer::Text("%d (0x%X)", i, i); }
            vsonyp0wer::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            vsonyp0wer::Text("Chars queue:");    for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; vsonyp0wer::SameLine();  vsonyp0wer::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            vsonyp0wer::Text("NavInputs down:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f) { vsonyp0wer::SameLine(); vsonyp0wer::Text("[%d] %.2f", i, io.NavInputs[i]); }
            vsonyp0wer::Text("NavInputs pressed:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { vsonyp0wer::SameLine(); vsonyp0wer::Text("[%d]", i); }
            vsonyp0wer::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { vsonyp0wer::SameLine(); vsonyp0wer::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

            vsonyp0wer::Button("Hovering me sets the\nkeyboard capture flag");
            if (vsonyp0wer::IsItemHovered())
                vsonyp0wer::CaptureKeyboardFromApp(true);
            vsonyp0wer::SameLine();
            vsonyp0wer::Button("Holding me clears the\nthe keyboard capture flag");
            if (vsonyp0wer::IsItemActive())
                vsonyp0wer::CaptureKeyboardFromApp(false);

            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Tabbing"))
        {
            vsonyp0wer::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "dummy";
            vsonyp0wer::InputText("1", buf, IM_ARRAYSIZE(buf));
            vsonyp0wer::InputText("2", buf, IM_ARRAYSIZE(buf));
            vsonyp0wer::InputText("3", buf, IM_ARRAYSIZE(buf));
            vsonyp0wer::PushAllowKeyboardFocus(false);
            vsonyp0wer::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            //vsonyp0wer::SameLine(); HelpMarker("Use vsonyp0wer::PushAllowKeyboardFocus(bool)\nto disable tabbing through certain widgets.");
            vsonyp0wer::PopAllowKeyboardFocus();
            vsonyp0wer::InputText("5", buf, IM_ARRAYSIZE(buf));
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Focus from code"))
        {
            bool focus_1 = vsonyp0wer::Button("Focus on 1"); vsonyp0wer::SameLine();
            bool focus_2 = vsonyp0wer::Button("Focus on 2"); vsonyp0wer::SameLine();
            bool focus_3 = vsonyp0wer::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) vsonyp0wer::SetKeyboardFocusHere();
            vsonyp0wer::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (vsonyp0wer::IsItemActive()) has_focus = 1;

            if (focus_2) vsonyp0wer::SetKeyboardFocusHere();
            vsonyp0wer::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (vsonyp0wer::IsItemActive()) has_focus = 2;

            vsonyp0wer::PushAllowKeyboardFocus(false);
            if (focus_3) vsonyp0wer::SetKeyboardFocusHere();
            vsonyp0wer::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (vsonyp0wer::IsItemActive()) has_focus = 3;
            vsonyp0wer::PopAllowKeyboardFocus();

            if (has_focus)
                vsonyp0wer::Text("Item with focus: %d", has_focus);
            else
                vsonyp0wer::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (vsonyp0wer::Button("Focus on X")) { focus_ahead = 0; } vsonyp0wer::SameLine();
            if (vsonyp0wer::Button("Focus on Y")) { focus_ahead = 1; } vsonyp0wer::SameLine();
            if (vsonyp0wer::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) vsonyp0wer::SetKeyboardFocusHere(focus_ahead);
            vsonyp0wer::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            vsonyp0wer::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Dragging"))
        {
            vsonyp0wer::TextWrapped("You can use vsonyp0wer::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
                vsonyp0wer::Text("IsMouseDragging(%d):\n  w/ default threshold: %d,\n  w/ zero threshold: %d\n  w/ large threshold: %d",
                    button, vsonyp0wer::IsMouseDragging(button), vsonyp0wer::IsMouseDragging(button, 0.0f), vsonyp0wer::IsMouseDragging(button, 20.0f));

            vsonyp0wer::Button("Drag Me");
            if (vsonyp0wer::IsItemActive())
                vsonyp0wer::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, vsonyp0wer::GetColorU32(vsonyp0werCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold (the default threshold is stored in io.MouseDragThreshold)
            // You can request a lower or higher threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta()
            ImVec2 value_raw = vsonyp0wer::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = vsonyp0wer::GetMouseDragDelta(0);
            ImVec2 mouse_delta = io.MouseDelta;
            vsonyp0wer::Text("GetMouseDragDelta(0):\n  w/ default threshold: (%.1f, %.1f),\n  w/ zero threshold: (%.1f, %.1f)\nMouseDelta: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y, value_raw.x, value_raw.y, mouse_delta.x, mouse_delta.y);
            vsonyp0wer::TreePop();
        }

        if (vsonyp0wer::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "Move", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == vsonyp0werMouseCursor_COUNT);

            vsonyp0wer::Text("Current mouse cursor = %d: %s", vsonyp0wer::GetMouseCursor(), mouse_cursors_names[vsonyp0wer::GetMouseCursor()]);
            vsonyp0wer::Text("Hover to see mouse cursors:");
            vsonyp0wer::SameLine(); HelpMarker("Your application can render a different mouse cursor based on what vsonyp0wer::GetMouseCursor() returns. If software cursor rendering (io.MouseDrawCursor) is set vsonyp0wer will draw the right cursor for you, otherwise your backend needs to handle it.");
            for (int i = 0; i < vsonyp0werMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                vsonyp0wer::Bullet(); vsonyp0wer::Selectable(label, false);
                if (vsonyp0wer::IsItemHovered() || vsonyp0wer::IsItemFocused())
                    vsonyp0wer::SetMouseCursor(i);
            }
            vsonyp0wer::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from vsonyp0wer Demo -> Help -> About
//-----------------------------------------------------------------------------

void vsonyp0wer::ShowAboutWindow(bool* p_open)
{
    if (!vsonyp0wer::Begin("About Dear vsonyp0wer", p_open, vsonyp0werWindowFlags_AlwaysAutoResize))
    {
        vsonyp0wer::End();
        return;
    }
    vsonyp0wer::Text("Dear vsonyp0wer %s", vsonyp0wer::GetVersion());
    vsonyp0wer::Separator();
    vsonyp0wer::Text("By Omar Cornut and all dear vsonyp0wer contributors.");
    vsonyp0wer::Text("Dear vsonyp0wer is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    vsonyp0wer::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        vsonyp0werIO& io = vsonyp0wer::GetIO();
        vsonyp0werStyle& style = vsonyp0wer::GetStyle();

        bool copy_to_clipboard = vsonyp0wer::Button("Copy to clipboard");
        vsonyp0wer::BeginChildFrame(vsonyp0wer::GetID("cfginfos"), ImVec2(0, vsonyp0wer::GetTextLineHeightWithSpacing() * 18), vsonyp0werWindowFlags_NoMove);
        if (copy_to_clipboard)
            vsonyp0wer::LogToClipboard();

        vsonyp0wer::Text("Dear vsonyp0wer %s (%d)", vsonyp0wer_VERSION, vsonyp0wer_VERSION_NUM);
        vsonyp0wer::Separator();
        vsonyp0wer::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        vsonyp0wer::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_WIN32_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_FORMAT_STRING_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_FORMAT_STRING_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_MATH_FUNCTIONS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_MATH_FUNCTIONS");
#endif
#ifdef vsonyp0wer_DISABLE_DEFAULT_ALLOCATORS
        vsonyp0wer::Text("define: vsonyp0wer_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef vsonyp0wer_USE_BGRA_PACKED_COLOR
        vsonyp0wer::Text("define: vsonyp0wer_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        vsonyp0wer::Text("define: _WIN32");
#endif
#ifdef _WIN64
        vsonyp0wer::Text("define: _WIN64");
#endif
#ifdef __linux__
        vsonyp0wer::Text("define: __linux__");
#endif
#ifdef __APPLE__
        vsonyp0wer::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        vsonyp0wer::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef __MINGW32__
        vsonyp0wer::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        vsonyp0wer::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        vsonyp0wer::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        vsonyp0wer::Text("define: __clang_version__=%s", __clang_version__);
#endif
        vsonyp0wer::Separator();
        vsonyp0wer::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        vsonyp0wer::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        vsonyp0wer::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & vsonyp0werConfigFlags_NavEnableKeyboard)        vsonyp0wer::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & vsonyp0werConfigFlags_NavEnableGamepad)         vsonyp0wer::Text(" NavEnableGamepad");
        if (io.ConfigFlags & vsonyp0werConfigFlags_NavEnableSetMousePos)     vsonyp0wer::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & vsonyp0werConfigFlags_NavNoCaptureKeyboard)     vsonyp0wer::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & vsonyp0werConfigFlags_NoMouse)                  vsonyp0wer::Text(" NoMouse");
        if (io.ConfigFlags & vsonyp0werConfigFlags_NoMouseCursorChange)      vsonyp0wer::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         vsonyp0wer::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   vsonyp0wer::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              vsonyp0wer::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            vsonyp0wer::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       vsonyp0wer::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        vsonyp0wer::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & vsonyp0werBackendFlags_HasGamepad)             vsonyp0wer::Text(" HasGamepad");
        if (io.BackendFlags & vsonyp0werBackendFlags_HasMouseCursors)        vsonyp0wer::Text(" HasMouseCursors");
        if (io.BackendFlags & vsonyp0werBackendFlags_HasSetMousePos)         vsonyp0wer::Text(" HasSetMousePos");
        vsonyp0wer::Separator();
        vsonyp0wer::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        vsonyp0wer::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        vsonyp0wer::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        vsonyp0wer::Separator();
        vsonyp0wer::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        vsonyp0wer::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        vsonyp0wer::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        vsonyp0wer::Text("style.FrameRounding: %.2f", style.FrameRounding);
        vsonyp0wer::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        vsonyp0wer::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        vsonyp0wer::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
            vsonyp0wer::LogFinish();
        vsonyp0wer::EndChildFrame();
    }
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string. Useful for quick combo boxes where the choices are known locally.
bool vsonyp0wer::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (vsonyp0wer::Combo(label, &style_idx, "Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: vsonyp0wer::StyleColorsClassic(); break;
        case 1: vsonyp0wer::StyleColorsDark(); break;
        case 2: vsonyp0wer::StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void vsonyp0wer::ShowFontSelector(const char* label)
{
    vsonyp0werIO& io = vsonyp0wer::GetIO();
    ImFont* font_current = vsonyp0wer::GetFont();
    if (vsonyp0wer::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            vsonyp0wer::PushID((void*)font);
            if (vsonyp0wer::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            vsonyp0wer::PopID();
        }
        vsonyp0wer::EndCombo();
    }
    vsonyp0wer::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and documentation in misc/fonts/ for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

void vsonyp0wer::ShowStyleEditor(vsonyp0werStyle * ref)
{
    // You can pass in a reference vsonyp0werStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
    vsonyp0werStyle& style = vsonyp0wer::GetStyle();
    static vsonyp0werStyle ref_saved_style;

    // Default to using internal stohnly as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    vsonyp0wer::PushItemWidth(vsonyp0wer::GetWindowWidth() * 0.50f);

    if (vsonyp0wer::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    vsonyp0wer::ShowFontSelector("Fonts##Selector");

    // Simplified Settings
    if (vsonyp0wer::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool window_border = (style.WindowBorderSize > 0.0f); if (vsonyp0wer::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
    vsonyp0wer::SameLine();
    { bool frame_border = (style.FrameBorderSize > 0.0f); if (vsonyp0wer::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
    vsonyp0wer::SameLine();
    { bool popup_border = (style.PopupBorderSize > 0.0f); if (vsonyp0wer::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

    // Save/Revert button
    if (vsonyp0wer::Button("Save Ref"))
        * ref = ref_saved_style = style;
    vsonyp0wer::SameLine();
    if (vsonyp0wer::Button("Revert Ref"))
        style = *ref;
    vsonyp0wer::SameLine();
    HelpMarker("Save/Revert in local non-persistent stohnly. Default Colors definition are not affected. Use \"Export Colors\" below to save them somewhere.");

    vsonyp0wer::Separator();

    if (vsonyp0wer::BeginTabBar("##tabs", vsonyp0werTabBarFlags_None))
    {
        if (vsonyp0wer::BeginTabItem("Sizes"))
        {
            vsonyp0wer::Text("Main");
            vsonyp0wer::SliderFloat2("WindowPadding", (float*)& style.WindowPadding, 0.0f, 20.0f, "%.0f");
            vsonyp0wer::SliderFloat2("FramePadding", (float*)& style.FramePadding, 0.0f, 20.0f, "%.0f");
            vsonyp0wer::SliderFloat2("ItemSpacing", (float*)& style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            vsonyp0wer::SliderFloat2("ItemInnerSpacing", (float*)& style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            vsonyp0wer::SliderFloat2("TouchExtraPadding", (float*)& style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            vsonyp0wer::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            vsonyp0wer::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            vsonyp0wer::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            vsonyp0wer::Text("Borders");
            vsonyp0wer::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            vsonyp0wer::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            vsonyp0wer::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            vsonyp0wer::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            vsonyp0wer::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            vsonyp0wer::Text("Rounding");
            vsonyp0wer::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            vsonyp0wer::Text("Alignment");
            vsonyp0wer::SliderFloat2("WindowTitleAlign", (float*)& style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            vsonyp0wer::SliderFloat2("ButtonTextAlign", (float*)& style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); vsonyp0wer::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            vsonyp0wer::SliderFloat2("SelectableTextAlign", (float*)& style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); vsonyp0wer::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            vsonyp0wer::Text("Safe Area Padding"); vsonyp0wer::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            vsonyp0wer::SliderFloat2("DisplaySafeAreaPadding", (float*)& style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            vsonyp0wer::EndTabItem();
        }

        if (vsonyp0wer::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (vsonyp0wer::Button("Export Unsaved"))
            {
                if (output_dest == 0)
                    vsonyp0wer::LogToClipboard();
                else
                    vsonyp0wer::LogToTTY();
                vsonyp0wer::LogText("ImVec4* colors = vsonyp0wer::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < vsonyp0werCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = vsonyp0wer::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        vsonyp0wer::LogText("colors[vsonyp0werCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                vsonyp0wer::LogFinish();
            }
            vsonyp0wer::SameLine(); vsonyp0wer::PushItemWidth(120); vsonyp0wer::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); vsonyp0wer::PopItemWidth();
            vsonyp0wer::SameLine(); vsonyp0wer::Checkbox("Only Modified Colors", &output_only_modified);

            static vsonyp0werTextFilter filter;
            filter.Draw("Filter colors", vsonyp0wer::GetFontSize() * 16);

            static vsonyp0werColorEditFlags alpha_flags = 0;
            vsonyp0wer::RadioButton("Opaque", &alpha_flags, 0); vsonyp0wer::SameLine();
            vsonyp0wer::RadioButton("Alpha", &alpha_flags, vsonyp0werColorEditFlags_AlphaPreview); vsonyp0wer::SameLine();
            vsonyp0wer::RadioButton("Both", &alpha_flags, vsonyp0werColorEditFlags_AlphaPreviewHalf); vsonyp0wer::SameLine();
            HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

            vsonyp0wer::BeginChild("##colors", ImVec2(0, 0), true, vsonyp0werWindowFlags_AlwaysVerticalScrollbar | vsonyp0werWindowFlags_AlwaysHorizontalScrollbar | vsonyp0werWindowFlags_NavFlattened);
            vsonyp0wer::PushItemWidth(-160);
            for (int i = 0; i < vsonyp0werCol_COUNT; i++)
            {
                const char* name = vsonyp0wer::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                vsonyp0wer::PushID(i);
                vsonyp0wer::ColorEdit4("##color", (float*)& style.Colors[i], vsonyp0werColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
                    // Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
                    vsonyp0wer::SameLine(0.0f, style.ItemInnerSpacing.x); if (vsonyp0wer::Button("Save")) ref->Colors[i] = style.Colors[i];
                    vsonyp0wer::SameLine(0.0f, style.ItemInnerSpacing.x); if (vsonyp0wer::Button("Revert")) style.Colors[i] = ref->Colors[i];
                }
                vsonyp0wer::SameLine(0.0f, style.ItemInnerSpacing.x);
                vsonyp0wer::TextUnformatted(name);
                vsonyp0wer::PopID();
            }
            vsonyp0wer::PopItemWidth();
            vsonyp0wer::EndChild();

            vsonyp0wer::EndTabItem();
        }

        if (vsonyp0wer::BeginTabItem("Fonts"))
        {
            vsonyp0werIO& io = vsonyp0wer::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and misc/fonts/README.txt for details on font loading.");
            vsonyp0wer::PushItemWidth(120);
            for (int i = 0; i < atlas->Fonts.Size; i++)
            {
                ImFont* font = atlas->Fonts[i];
                vsonyp0wer::PushID(font);
                bool font_details_opened = vsonyp0wer::TreeNode(font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
                vsonyp0wer::SameLine(); if (vsonyp0wer::SmallButton("Set as default")) { io.FontDefault = font; }
                if (font_details_opened)
                {
                    vsonyp0wer::PushFont(font);
                    vsonyp0wer::Text("The quick brown fox jumps over the lazy dog");
                    vsonyp0wer::PopFont();
                    vsonyp0wer::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
                    vsonyp0wer::SameLine(); HelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
                    vsonyp0wer::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
                    vsonyp0wer::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
                    vsonyp0wer::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
                    const float surface_sqrt = sqrtf((float)font->MetricsTotalSurface);
                    vsonyp0wer::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)surface_sqrt, (int)surface_sqrt);
                    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
                        if (const ImFontConfig * cfg = &font->ConfigData[config_i])
                            vsonyp0wer::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
                    if (vsonyp0wer::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
                    {
                        // Display all glyphs of the fonts in separate pages of 256 characters
                        for (int base = 0; base < 0x10000; base += 256)
                        {
                            int count = 0;
                            for (int n = 0; n < 256; n++)
                                count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
                            if (count > 0 && vsonyp0wer::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                            {
                                float cell_size = font->FontSize * 1;
                                float cell_spacing = style.ItemSpacing.y;
                                ImVec2 base_pos = vsonyp0wer::GetCursorScreenPos();
                                ImDrawList* draw_list = vsonyp0wer::GetWindowDrawList();
                                for (int n = 0; n < 256; n++)
                                {
                                    ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                                    ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                                    const ImFontGlyph * glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                                    if (glyph)
                                        font->RenderChar(draw_list, cell_size, cell_p1, vsonyp0wer::GetColorU32(vsonyp0werCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
                                    if (glyph && vsonyp0wer::IsMouseHoveringRect(cell_p1, cell_p2))
                                    {
                                        vsonyp0wer::BeginTooltip();
                                        vsonyp0wer::Text("Codepoint: U+%04X", base + n);
                                        vsonyp0wer::Separator();
                                        vsonyp0wer::Text("AdvanceX: %.1f", glyph->AdvanceX);
                                        vsonyp0wer::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                                        vsonyp0wer::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                                        vsonyp0wer::EndTooltip();
                                    }
                                }
                                vsonyp0wer::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                                vsonyp0wer::TreePop();
                            }
                        }
                        vsonyp0wer::TreePop();
                    }
                    vsonyp0wer::TreePop();
                }
                vsonyp0wer::PopID();
            }
            if (vsonyp0wer::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
            {
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
                vsonyp0wer::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
                vsonyp0wer::TreePop();
            }

            static float window_scale = 1.0f;
            if (vsonyp0wer::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))   // scale only this window
                vsonyp0wer::SetWindowFontScale(window_scale);
            vsonyp0wer::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
            vsonyp0wer::PopItemWidth();

            vsonyp0wer::EndTabItem();
        }

        if (vsonyp0wer::BeginTabItem("Rendering"))
        {
            vsonyp0wer::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); vsonyp0wer::SameLine(); HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
            vsonyp0wer::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            vsonyp0wer::PushItemWidth(100);
            vsonyp0wer::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
            vsonyp0wer::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            vsonyp0wer::PopItemWidth();

            vsonyp0wer::EndTabItem();
        }

        vsonyp0wer::EndTabBar();
    }

    vsonyp0wer::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------

// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window we Begin()-ed into (the window needs the vsonyp0werWindowFlags_MenuBar flag)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
static void ShowExampleAppMainMenuBar()
{
    if (vsonyp0wer::BeginMainMenuBar())
    {
        if (vsonyp0wer::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            vsonyp0wer::EndMenu();
        }
        if (vsonyp0wer::BeginMenu("Edit"))
        {
            if (vsonyp0wer::MenuItem("Undo", "CTRL+Z")) {}
            if (vsonyp0wer::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            vsonyp0wer::Separator();
            if (vsonyp0wer::MenuItem("Cut", "CTRL+X")) {}
            if (vsonyp0wer::MenuItem("Copy", "CTRL+C")) {}
            if (vsonyp0wer::MenuItem("Paste", "CTRL+V")) {}
            vsonyp0wer::EndMenu();
        }
        vsonyp0wer::EndMainMenuBar();
    }
}

// Note that shortcuts are currently provided for display only (future version will add flags to BeginMenu to process shortcuts)
static void ShowExampleMenuFile()
{
    vsonyp0wer::MenuItem("(dummy menu)", NULL, false, false);
    if (vsonyp0wer::MenuItem("New")) {}
    if (vsonyp0wer::MenuItem("Open", "Ctrl+O")) {}
    if (vsonyp0wer::BeginMenu("Open Recent"))
    {
        vsonyp0wer::MenuItem("fish_hat.c");
        vsonyp0wer::MenuItem("fish_hat.inl");
        vsonyp0wer::MenuItem("fish_hat.h");
        if (vsonyp0wer::BeginMenu("More.."))
        {
            vsonyp0wer::MenuItem("Hello");
            vsonyp0wer::MenuItem("Sailor");
            if (vsonyp0wer::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                vsonyp0wer::EndMenu();
            }
            vsonyp0wer::EndMenu();
        }
        vsonyp0wer::EndMenu();
    }
    if (vsonyp0wer::MenuItem("Save", "Ctrl+S")) {}
    if (vsonyp0wer::MenuItem("Save As..")) {}
    vsonyp0wer::Separator();
    if (vsonyp0wer::BeginMenu("Options"))
    {
        static bool enabled = true;
        vsonyp0wer::MenuItem("Enabled", "", &enabled);
        vsonyp0wer::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            vsonyp0wer::Text("Scrolling Text %d", i);
        vsonyp0wer::EndChild();
        static float f = 0.5f;
        static int n = 0;
        static bool b = true;
        vsonyp0wer::SliderFloat("Value", &f, 0.0f, 1.0f);
        vsonyp0wer::InputFloat("Input", &f, 0.1f);
        vsonyp0wer::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        vsonyp0wer::Checkbox("Check", &b);
        vsonyp0wer::EndMenu();
    }
    if (vsonyp0wer::BeginMenu("Colors"))
    {
        float sz = vsonyp0wer::GetTextLineHeight();
        for (int i = 0; i < vsonyp0werCol_COUNT; i++)
        {
            const char* name = vsonyp0wer::GetStyleColorName((vsonyp0werCol)i);
            ImVec2 p = vsonyp0wer::GetCursorScreenPos();
            vsonyp0wer::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), vsonyp0wer::GetColorU32((vsonyp0werCol)i));
            vsonyp0wer::Dummy(ImVec2(sz, sz));
            vsonyp0wer::SameLine();
            vsonyp0wer::MenuItem(name);
        }
        vsonyp0wer::EndMenu();
    }
    if (vsonyp0wer::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (vsonyp0wer::MenuItem("Checked", NULL, true)) {}
    if (vsonyp0wer::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    vsonyp0werTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
        AutoScroll = true;
        ScrollToBottom = true;
        AddLog("Welcome to Dear vsonyp0wer!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char* str) { size_t len = strlen(str) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)str, len); }
    static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
        ScrollToBottom = true;
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
        if (AutoScroll)
            ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open)
    {
        vsonyp0wer::SetNextWindowSize(ImVec2(520, 600), vsonyp0werCond_FirstUseEver);
        if (!vsonyp0wer::Begin(title, p_open))
        {
            vsonyp0wer::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (vsonyp0wer::BeginPopupContextItem())
        {
            if (vsonyp0wer::MenuItem("Close Console"))
                * p_open = false;
            vsonyp0wer::EndPopup();
        }

        vsonyp0wer::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        vsonyp0wer::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

        // TODO: display items starting from the bottom

        if (vsonyp0wer::SmallButton("Add Dummy Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); } vsonyp0wer::SameLine();
        if (vsonyp0wer::SmallButton("Add Dummy Error")) { AddLog("[error] something went wrong"); } vsonyp0wer::SameLine();
        if (vsonyp0wer::SmallButton("Clear")) { ClearLog(); } vsonyp0wer::SameLine();
        bool copy_to_clipboard = vsonyp0wer::SmallButton("Copy"); vsonyp0wer::SameLine();
        if (vsonyp0wer::SmallButton("Scroll to bottom")) ScrollToBottom = true;
        //static float t = 0.0f; if (vsonyp0wer::GetTime() - t > 0.02f) { t = vsonyp0wer::GetTime(); AddLog("Spam %f", t); }

        vsonyp0wer::Separator();

        // Options menu
        if (vsonyp0wer::BeginPopup("Options"))
        {
            if (vsonyp0wer::Checkbox("Auto-scroll", &AutoScroll))
                if (AutoScroll)
                    ScrollToBottom = true;
            vsonyp0wer::EndPopup();
        }

        // Options, Filter
        if (vsonyp0wer::Button("Options"))
            vsonyp0wer::OpenPopup("Options");
        vsonyp0wer::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        vsonyp0wer::Separator();

        const float footer_height_to_reserve = vsonyp0wer::GetStyle().ItemSpacing.y + vsonyp0wer::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
        vsonyp0wer::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, vsonyp0werWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
        if (vsonyp0wer::BeginPopupContextWindow())
        {
            if (vsonyp0wer::Selectable("Clear")) ClearLog();
            vsonyp0wer::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use vsonyp0wer::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the vsonyp0werListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     vsonyp0werListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what vsonyp0werListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            vsonyp0wer::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
            bool pop_color = false;
            if (strstr(item, "[error]")) { vsonyp0wer::PushStyleColor(vsonyp0werCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; } else if (strncmp(item, "# ", 2) == 0) { vsonyp0wer::PushStyleColor(vsonyp0werCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
            vsonyp0wer::TextUnformatted(item);
            if (pop_color)
                vsonyp0wer::PopStyleColor();
        }
        if (copy_to_clipboard)
            vsonyp0wer::LogFinish();
        if (ScrollToBottom)
            vsonyp0wer::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        vsonyp0wer::PopStyleVar();
        vsonyp0wer::EndChild();
        vsonyp0wer::Separator();

        // Command-line
        bool reclaim_focus = false;
        if (vsonyp0wer::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), vsonyp0werInputTextFlags_EnterReturnsTrue | vsonyp0werInputTextFlags_CallbackCompletion | vsonyp0werInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        vsonyp0wer::SetItemDefaultFocus();
        if (reclaim_focus)
            vsonyp0wer::SetKeyboardFocusHere(-1); // Auto focus previous widget

        vsonyp0wer::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        } else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        } else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        } else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On commad input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    static int TextEditCallbackStub(vsonyp0werInputTextCallbackData * data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(vsonyp0werInputTextCallbackData * data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case vsonyp0werInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
            } else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            } else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case vsonyp0werInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == vsonyp0werKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            } else if (data->EventKey == vsonyp0werKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    vsonyp0werTextBuffer     Buf;
    vsonyp0werTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll;
    bool                ScrollToBottom;

    ExampleAppLog()
    {
        AutoScroll = true;
        ScrollToBottom = false;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
        if (AutoScroll)
            ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!vsonyp0wer::Begin(title, p_open))
        {
            vsonyp0wer::End();
            return;
        }

        // Options menu
        if (vsonyp0wer::BeginPopup("Options"))
        {
            if (vsonyp0wer::Checkbox("Auto-scroll", &AutoScroll))
                if (AutoScroll)
                    ScrollToBottom = true;
            vsonyp0wer::EndPopup();
        }

        // Main window
        if (vsonyp0wer::Button("Options"))
            vsonyp0wer::OpenPopup("Options");
        vsonyp0wer::SameLine();
        bool clear = vsonyp0wer::Button("Clear");
        vsonyp0wer::SameLine();
        bool copy = vsonyp0wer::Button("Copy");
        vsonyp0wer::SameLine();
        Filter.Draw("Filter", -100.0f);

        vsonyp0wer::Separator();
        vsonyp0wer::BeginChild("scrolling", ImVec2(0, 0), false, vsonyp0werWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            vsonyp0wer::LogToClipboard();

        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    vsonyp0wer::TextUnformatted(line_start, line_end);
            }
        } else
        {
            // The simplest and easy way to display the entire buffer:
            //   vsonyp0wer::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
            // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
            // Using vsonyp0werListClipper requires A) random access into your data, and B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
            // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
            vsonyp0werListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    vsonyp0wer::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        vsonyp0wer::PopStyleVar();

        if (ScrollToBottom)
            vsonyp0wer::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        vsonyp0wer::EndChild();
        vsonyp0wer::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    vsonyp0wer::SetNextWindowSize(ImVec2(500, 400), vsonyp0werCond_FirstUseEver);
    vsonyp0wer::Begin("Example: Log", p_open);
    if (vsonyp0wer::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            const char* categories[3] = { "info", "warn", "error" };
            const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                vsonyp0wer::GetFrameCount(), categories[counter % IM_ARRAYSIZE(categories)], vsonyp0wer::GetTime(), words[counter % IM_ARRAYSIZE(words)]);
            counter++;
        }
    }
    vsonyp0wer::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    vsonyp0wer::SetNextWindowSize(ImVec2(500, 440), vsonyp0werCond_FirstUseEver);
    if (vsonyp0wer::Begin("Example: Simple layout", p_open, vsonyp0werWindowFlags_MenuBar))
    {
        if (vsonyp0wer::BeginMenuBar())
        {
            if (vsonyp0wer::BeginMenu("File"))
            {
                if (vsonyp0wer::MenuItem("Close"))* p_open = false;
                vsonyp0wer::EndMenu();
            }
            vsonyp0wer::EndMenuBar();
        }

        // left
        static int selected = 0;
        vsonyp0wer::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            char label[128];
            sprintf(label, "MyObject %d", i);
            if (vsonyp0wer::Selectable(label, selected == i))
                selected = i;
        }
        vsonyp0wer::EndChild();
        vsonyp0wer::SameLine();

        // right
        vsonyp0wer::BeginGroup();
        vsonyp0wer::BeginChild("item view", ImVec2(0, -vsonyp0wer::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        vsonyp0wer::Text("MyObject: %d", selected);
        vsonyp0wer::Separator();
        if (vsonyp0wer::BeginTabBar("##Tabs", vsonyp0werTabBarFlags_None))
        {
            if (vsonyp0wer::BeginTabItem("Description"))
            {
                vsonyp0wer::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                vsonyp0wer::EndTabItem();
            }
            if (vsonyp0wer::BeginTabItem("Details"))
            {
                vsonyp0wer::Text("ID: 0123456789");
                vsonyp0wer::EndTabItem();
            }
            vsonyp0wer::EndTabBar();
        }
        vsonyp0wer::EndChild();
        if (vsonyp0wer::Button("Revert")) {}
        vsonyp0wer::SameLine();
        if (vsonyp0wer::Button("Save")) {}
        vsonyp0wer::EndGroup();
    }
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    vsonyp0wer::SetNextWindowSize(ImVec2(430, 450), vsonyp0werCond_FirstUseEver);
    if (!vsonyp0wer::Begin("Example: Property editor", p_open))
    {
        vsonyp0wer::End();
        return;
    }

    HelpMarker("This example shows how you may implement a property editor using two columns.\nAll objects/fields data are dummies here.\nRemember that in many simple cases, you can use vsonyp0wer::SameLine(xxx) to position\nyour cursor horizontally instead of using the Columns() API.");

    vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_FramePadding, ImVec2(2, 2));
    vsonyp0wer::Columns(2);
    vsonyp0wer::Separator();

    struct funcs
    {
        static void ShowDummyObject(const char* prefix, int uid)
        {
            vsonyp0wer::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
            vsonyp0wer::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
            bool node_open = vsonyp0wer::TreeNode("Object", "%s_%u", prefix, uid);
            vsonyp0wer::NextColumn();
            vsonyp0wer::AlignTextToFramePadding();
            vsonyp0wer::Text("my sailor is rich");
            vsonyp0wer::NextColumn();
            if (node_open)
            {
                static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
                for (int i = 0; i < 8; i++)
                {
                    vsonyp0wer::PushID(i); // Use field index as identifier.
                    if (i < 2)
                    {
                        ShowDummyObject("Child", 424242);
                    } else
                    {
                        // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                        vsonyp0wer::AlignTextToFramePadding();
                        vsonyp0wer::TreeNodeEx("Field", vsonyp0werTreeNodeFlags_Leaf | vsonyp0werTreeNodeFlags_NoTreePushOnOpen | vsonyp0werTreeNodeFlags_Bullet, "Field_%d", i);
                        vsonyp0wer::NextColumn();
                        vsonyp0wer::PushItemWidth(-1);
                        if (i >= 5)
                            vsonyp0wer::InputFloat("##value", &dummy_members[i], 1.0f);
                        else
                            vsonyp0wer::DragFloat("##value", &dummy_members[i], 0.01f);
                        vsonyp0wer::PopItemWidth();
                        vsonyp0wer::NextColumn();
                    }
                    vsonyp0wer::PopID();
                }
                vsonyp0wer::TreePop();
            }
            vsonyp0wer::PopID();
        }
    };

    // Iterate dummy objects with dummy members (all the same data)
    for (int obj_i = 0; obj_i < 3; obj_i++)
        funcs::ShowDummyObject("Object", obj_i);

    vsonyp0wer::Columns(1);
    vsonyp0wer::Separator();
    vsonyp0wer::PopStyleVar();
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    vsonyp0wer::SetNextWindowSize(ImVec2(520, 600), vsonyp0werCond_FirstUseEver);
    if (!vsonyp0wer::Begin("Example: Long text display", p_open))
    {
        vsonyp0wer::End();
        return;
    }

    static int test_type = 0;
    static vsonyp0werTextBuffer log;
    static int lines = 0;
    vsonyp0wer::Text("Printing unusually long amount of text.");
    vsonyp0wer::Combo("Test type", &test_type, "Single call to TextUnformatted()\0Multiple calls to Text(), clipped manually\0Multiple calls to Text(), not clipped (slow)\0");
    vsonyp0wer::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (vsonyp0wer::Button("Clear")) { log.clear(); lines = 0; }
    vsonyp0wer::SameLine();
    if (vsonyp0wer::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
        lines += 1000;
    }
    vsonyp0wer::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        vsonyp0wer::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
    {
        // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the vsonyp0werListClipper helper.
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ItemSpacing, ImVec2(0, 0));
        vsonyp0werListClipper clipper(lines);
        while (clipper.Step())
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                vsonyp0wer::Text("%i The quick brown fox jumps over the lazy dog", i);
        vsonyp0wer::PopStyleVar();
        break;
    }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        vsonyp0wer::PushStyleVar(vsonyp0werStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < lines; i++)
            vsonyp0wer::Text("%i The quick brown fox jumps over the lazy dog", i);
        vsonyp0wer::PopStyleVar();
        break;
    }
    vsonyp0wer::EndChild();
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!vsonyp0wer::Begin("Example: Auto-resizing window", p_open, vsonyp0werWindowFlags_AlwaysAutoResize))
    {
        vsonyp0wer::End();
        return;
    }

    static int lines = 10;
    vsonyp0wer::Text("Window will resize every-frame to the size of its content.\nNote that you probably don't want to query the window size to\noutput your content because that would create a feedback loop.");
    vsonyp0wer::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        vsonyp0wer::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints // Helper functions to demonstrate programmatic constraints
    {
        static void Square(vsonyp0werSizeCallbackData* data) { data->DesiredSize = ImVec2(IM_MAX(data->DesiredSize.x, data->DesiredSize.y), IM_MAX(data->DesiredSize.x, data->DesiredSize.y)); }
        static void Step(vsonyp0werSizeCallbackData* data) { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(-1, 0), ImVec2(-1, FLT_MAX));      // Vertical only
    if (type == 1) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(0, -1), ImVec2(FLT_MAX, -1));      // Horizontal only
    if (type == 2) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(400, -1), ImVec2(500, -1));          // Width 400-500
    if (type == 4) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(-1, 400), ImVec2(-1, 500));          // Height 400-500
    if (type == 5) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);                     // Always Square
    if (type == 6) vsonyp0wer::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Fixed Step

    vsonyp0werWindowFlags flags = auto_resize ? vsonyp0werWindowFlags_AlwaysAutoResize : 0;
    if (vsonyp0wer::Begin("Example: Constrained Resize", p_open, flags))
    {
        const char* desc[] =
        {
            "Resize vertical only",
            "Resize horizontal only",
            "Width > 100, Height > 100",
            "Width 400-500",
            "Height 400-500",
            "Custom: Always Square",
            "Custom: Fixed Steps (100)",
        };
        if (vsonyp0wer::Button("200x200")) { vsonyp0wer::SetWindowSize(ImVec2(200, 200)); } vsonyp0wer::SameLine();
        if (vsonyp0wer::Button("500x500")) { vsonyp0wer::SetWindowSize(ImVec2(500, 500)); } vsonyp0wer::SameLine();
        if (vsonyp0wer::Button("800x200")) { vsonyp0wer::SetWindowSize(ImVec2(800, 200)); }
        vsonyp0wer::PushItemWidth(200);
        vsonyp0wer::Combo("Constraint", &type, desc, IM_ARRAYSIZE(desc));
        vsonyp0wer::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        vsonyp0wer::PopItemWidth();
        vsonyp0wer::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            vsonyp0wer::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 0;
    vsonyp0werIO& io = vsonyp0wer::GetIO();
    if (corner != -1)
    {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        vsonyp0wer::SetNextWindowPos(window_pos, vsonyp0werCond_Always, window_pos_pivot);
    }
    vsonyp0wer::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (vsonyp0wer::Begin("Example: Simple overlay", p_open, (corner != -1 ? vsonyp0werWindowFlags_NoMove : 0) | vsonyp0werWindowFlags_NoDecoration | vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_NoFocusOnAppearing | vsonyp0werWindowFlags_NoNav))
    {
        vsonyp0wer::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        vsonyp0wer::Separator();
        if (vsonyp0wer::IsMousePosValid())
            vsonyp0wer::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            vsonyp0wer::Text("Mouse Position: <invalid>");
        if (vsonyp0wer::BeginPopupContextWindow())
        {
            if (vsonyp0wer::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (vsonyp0wer::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (vsonyp0wer::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (vsonyp0wer::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (vsonyp0wer::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && vsonyp0wer::MenuItem("Close")) * p_open = false;
            vsonyp0wer::EndPopup();
        }
    }
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well. Read FAQ section "How can I have multiple widgets with the same label? Can I have widget without a label? (Yes). A primer on the purpose of labels/IDs." for details.
static void ShowExampleAppWindowTitles(bool*)
{
    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    vsonyp0wer::SetNextWindowPos(ImVec2(100, 100), vsonyp0werCond_FirstUseEver);
    vsonyp0wer::Begin("Same title as another window##1");
    vsonyp0wer::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    vsonyp0wer::End();

    vsonyp0wer::SetNextWindowPos(ImVec2(100, 200), vsonyp0werCond_FirstUseEver);
    vsonyp0wer::Begin("Same title as another window##2");
    vsonyp0wer::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    vsonyp0wer::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(vsonyp0wer::GetTime() / 0.25f) & 3], vsonyp0wer::GetFrameCount());
    vsonyp0wer::SetNextWindowPos(ImVec2(100, 300), vsonyp0werCond_FirstUseEver);
    vsonyp0wer::Begin(buf);
    vsonyp0wer::Text("This window has a changing title.");
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    vsonyp0wer::SetNextWindowSize(ImVec2(350, 560), vsonyp0werCond_FirstUseEver);
    if (!vsonyp0wer::Begin("Example: Custom rendering", p_open))
    {
        vsonyp0wer::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc.
    // Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4.
    // vsonyp0wer defines overloaded operators but they are internal to vsonyp0wer.cpp and not exposed outside (to avoid messing with your types)
    // In this example we are not using the maths operators!
    ImDrawList* draw_list = vsonyp0wer::GetWindowDrawList();

    if (vsonyp0wer::BeginTabBar("##TabBar"))
    {
        // Primitives
        if (vsonyp0wer::BeginTabItem("Primitives"))
        {
            static float sz = 36.0f;
            static float thickness = 4.0f;
            static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            vsonyp0wer::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
            vsonyp0wer::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            vsonyp0wer::ColorEdit4("Color", &col.x);
            const ImVec2 p = vsonyp0wer::GetCursorScreenPos();
            const ImU32 col32 = ImColor(col);
            float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6, th); x += sz + spacing;   // Hexagon
                draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 20, th); x += sz + spacing;  // Circle
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 0.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight, th); x += sz + spacing;
                draw_list->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32, th); x += sz + spacing;
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col32, th); x += sz + spacing;                  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col32, th); x += spacing;                       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, th); x += sz + spacing;             // Diagonal line
                draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col32, th);
                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6); x += sz + spacing;     // Hexagon
            draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32); x += sz + spacing;    // Circle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32); x += sz + spacing;
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f); x += sz + spacing;
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight); x += sz + spacing;
            draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32); x += sz + spacing;
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col32); x += sz + spacing;        // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col32); x += spacing + spacing;   // Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col32);          x += sz;                  // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
            vsonyp0wer::Dummy(ImVec2((sz + spacing) * 9.5f, (sz + spacing) * 3));
            vsonyp0wer::EndTabItem();
        }

        if (vsonyp0wer::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static bool adding_line = false;
            if (vsonyp0wer::Button("Clear")) points.clear();
            if (points.Size >= 2) { vsonyp0wer::SameLine(); if (vsonyp0wer::Button("Undo")) { points.pop_back(); points.pop_back(); } }
            vsonyp0wer::Text("Left-click and drag to add lines,\nRight-click to undo");

            // Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered()
            // But you can also draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos().
            // If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max).
            ImVec2 canvas_pos = vsonyp0wer::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
            ImVec2 canvas_size = vsonyp0wer::GetContentRegionAvail();        // Resize canvas to what's available
            if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
            if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
            draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
            draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

            bool adding_preview = false;
            vsonyp0wer::InvisibleButton("canvas", canvas_size);
            ImVec2 mouse_pos_in_canvas = ImVec2(vsonyp0wer::GetIO().MousePos.x - canvas_pos.x, vsonyp0wer::GetIO().MousePos.y - canvas_pos.y);
            if (adding_line)
            {
                adding_preview = true;
                points.push_back(mouse_pos_in_canvas);
                if (!vsonyp0wer::IsMouseDown(0))
                    adding_line = adding_preview = false;
            }
            if (vsonyp0wer::IsItemHovered())
            {
                if (!adding_line && vsonyp0wer::IsMouseClicked(0))
                {
                    points.push_back(mouse_pos_in_canvas);
                    adding_line = true;
                }
                if (vsonyp0wer::IsMouseClicked(1) && !points.empty())
                {
                    adding_line = adding_preview = false;
                    points.pop_back();
                    points.pop_back();
                }
            }
            draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);      // clip lines within the canvas (if we resize it, etc.)
            for (int i = 0; i < points.Size - 1; i += 2)
                draw_list->AddLine(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();
            if (adding_preview)
                points.pop_back();
            vsonyp0wer::EndTabItem();
        }

        if (vsonyp0wer::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            vsonyp0wer::Checkbox("Draw in Background draw list", &draw_bg);
            vsonyp0wer::Checkbox("Draw in Foreground draw list", &draw_fg);
            ImVec2 window_pos = vsonyp0wer::GetWindowPos();
            ImVec2 window_size = vsonyp0wer::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                vsonyp0wer::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 32, 10 + 4);
            if (draw_fg)
                vsonyp0wer::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 32, 10);
            vsonyp0wer::EndTabItem();
        }

        vsonyp0wer::EndTabBar();
    }

    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;           // Document title
    bool        Open;           // Set when the document is open (in this demo, we keep an array of all available documents to simplify the demo)
    bool        OpenPrev;       // Copy of Open from last update.
    bool        Dirty;          // Set when the document has been modified
    bool        WantClose;      // Set when the document
    ImVec4      Color;          // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        Name = name;
        Open = OpenPrev = open;
        Dirty = false;
        WantClose = false;
        Color = color;
    }
    void DoOpen() { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave() { Dirty = false; }

    // Display dummy contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        vsonyp0wer::PushID(doc);
        vsonyp0wer::Text("Document \"%s\"", doc->Name);
        vsonyp0wer::PushStyleColor(vsonyp0werCol_Text, doc->Color);
        vsonyp0wer::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        vsonyp0wer::PopStyleColor();
        if (vsonyp0wer::Button("Modify", ImVec2(100, 0)))
            doc->Dirty = true;
        vsonyp0wer::SameLine();
        if (vsonyp0wer::Button("Save", ImVec2(100, 0)))
            doc->DoSave();
        vsonyp0wer::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        vsonyp0wer::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!vsonyp0wer::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (vsonyp0wer::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (vsonyp0wer::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        vsonyp0wer::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce", true, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant", true, ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot", true, ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato", false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document", false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo, as opposed
// to clicking on the regular tab closing button) and stops being submitted, it will take a frame for the tab bar to notice its absence.
// During this frame there will be a gap in the tab bar, and if the tab that has disappeared was the selected one, the tab bar
// will report no selected tab during the frame. This will effectively give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the vsonyp0werTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments & app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            vsonyp0wer::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    static bool opt_reorderable = true;
    static vsonyp0werTabBarFlags opt_fitting_flags = vsonyp0werTabBarFlags_FittingPolicyDefault_;

    if (!vsonyp0wer::Begin("Example: Documents", p_open, vsonyp0werWindowFlags_MenuBar))
    {
        vsonyp0wer::End();
        return;
    }

    // Menu
    if (vsonyp0wer::BeginMenuBar())
    {
        if (vsonyp0wer::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (vsonyp0wer::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (vsonyp0wer::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                vsonyp0wer::EndMenu();
            }
            if (vsonyp0wer::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (vsonyp0wer::MenuItem("Exit", "Alt+F4")) {}
            vsonyp0wer::EndMenu();
        }
        vsonyp0wer::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            vsonyp0wer::SameLine();
        vsonyp0wer::PushID(doc);
        if (vsonyp0wer::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        vsonyp0wer::PopID();
    }

    vsonyp0wer::Separator();

    // Submit Tab Bar and Tabs
    {
        vsonyp0werTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? vsonyp0werTabBarFlags_Reorderable : 0);
        if (vsonyp0wer::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((vsonyp0wer::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (vsonyp0wer::GetIO().KeyCtrl) vsonyp0wer::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                vsonyp0werTabItemFlags tab_flags = (doc->Dirty ? vsonyp0werTabItemFlags_UnsavedDocument : 0);
                bool visible = vsonyp0wer::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    vsonyp0wer::EndTabItem();
                }
            }

            vsonyp0wer::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        } else
        {
            if (!vsonyp0wer::IsPopupOpen("Save?"))
                vsonyp0wer::OpenPopup("Save?");
            if (vsonyp0wer::BeginPopupModal("Save?"))
            {
                vsonyp0wer::Text("Save change to the following items?");
                vsonyp0wer::PushItemWidth(-1.0f);
                vsonyp0wer::ListBoxHeader("##", close_queue_unsaved_documents, 6);
                for (int n = 0; n < close_queue.Size; n++)
                    if (close_queue[n]->Dirty)
                        vsonyp0wer::Text("%s", close_queue[n]->Name);
                vsonyp0wer::ListBoxFooter();

                if (vsonyp0wer::Button("Yes", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    vsonyp0wer::CloseCurrentPopup();
                }
                vsonyp0wer::SameLine();
                if (vsonyp0wer::Button("No", ImVec2(80, 0)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    vsonyp0wer::CloseCurrentPopup();
                }
                vsonyp0wer::SameLine();
                if (vsonyp0wer::Button("Cancel", ImVec2(80, 0)))
                {
                    close_queue.clear();
                    vsonyp0wer::CloseCurrentPopup();
                }
                vsonyp0wer::EndPopup();
            }
        }
    }

    vsonyp0wer::End();
}

// End of Demo code
#else

void vsonyp0wer::ShowAboutWindow(bool*) {}
void vsonyp0wer::ShowDemoWindow(bool*) {}
void vsonyp0wer::ShowUserGuide() {}
void vsonyp0wer::ShowStyleEditor(vsonyp0werStyle*) {}

#endif



































































// Junk Code By Troll Face & Thaisen's Gen
void RegnGFhcWapsEBOgjZffYPPYcbnNPlYsGAfZysWl5099396() {     int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89799015 = -244543694;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65184970 = -651174978;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw69585979 = -910063869;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw24597509 = 39355808;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw50022537 = -426194560;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw32958132 = 46380666;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5354363 = -464558294;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw36876477 = 37934003;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86945929 = -521445947;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw79513024 = 80043982;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96926915 = 11851621;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw84770774 = -650112736;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw47023441 = -699730970;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw71118811 = -514023268;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85689458 = -818625923;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85587601 = -568656467;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw44049695 = -413493071;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81332252 = -240575574;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76695012 = -355039339;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22078302 = -800217496;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw90813620 = -498342151;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48828736 = -525796419;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60068744 = -676061062;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw30600356 = -735151891;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw83424786 = -873536181;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73169340 = 91811136;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89516955 = -570674213;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96278707 = -984063602;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw21465396 = -986113036;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22195976 = -67945566;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw56495703 = -375228846;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw31468719 = -847223716;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw67016853 = -894783205;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65930110 = -359418114;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw94005994 = -688533098;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw39654487 = -684813934;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw62658833 = 56573271;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23096400 = -326030529;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw78368343 = 36292026;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59436869 = -879091844;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73210868 = -382033680;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74713662 = -719551688;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw99844967 = -657464500;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86760450 = -411956291;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw18911918 = -328458752;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86431767 = -969966396;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw12804696 = 60136283;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85288828 = -584816275;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40521863 = -64707473;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw75092899 = -155390916;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16078811 = -881019593;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51640428 = -7096039;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11615781 = -158915498;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw92372347 = -498608077;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw14285775 = -913666918;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40970279 = -718747275;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5116227 = -975113917;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw38985623 = -74911978;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw41172722 = -87108011;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76853197 = -418005697;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw43441176 = -382945122;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw9075656 = -480494693;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15411081 = 24047039;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64749953 = -353500382;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23017321 = -544727173;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65458197 = -140924663;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17753922 = -755329532;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81093330 = -240312857;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw77112816 = -825490170;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46034972 = -33811990;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22928769 = -525229739;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw20953295 = 12537457;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw2963910 = -176867600;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17258144 = -475947495;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48867434 = -318183817;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16099959 = -778790463;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48983768 = -868331920;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73308293 = -164104771;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11688438 = -306693140;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96993018 = -903569786;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60364644 = -968325148;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4228128 = -985857938;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw55756844 = -819356130;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46372497 = -730722120;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw6117166 = -186925973;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4855276 = -268132807;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw19852938 = -588308218;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74644505 = -296175128;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51644336 = -445751196;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53035715 = -969785824;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw34538261 = -709700018;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23673211 = -868514751;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81923678 = -138922519;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw1515146 = -545702278;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15995693 = -396146723;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64135212 = -901538988;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59302581 = -643598727;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw35095015 = -203964119;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw63743130 = -867229119;    int XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53453721 = -244543694;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89799015 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65184970;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65184970 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw69585979;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw69585979 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw24597509;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw24597509 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw50022537;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw50022537 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw32958132;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw32958132 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5354363;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5354363 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw36876477;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw36876477 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86945929;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86945929 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw79513024;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw79513024 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96926915;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96926915 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw84770774;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw84770774 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw47023441;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw47023441 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw71118811;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw71118811 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85689458;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85689458 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85587601;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85587601 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw44049695;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw44049695 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81332252;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81332252 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76695012;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76695012 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22078302;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22078302 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw90813620;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw90813620 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48828736;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48828736 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60068744;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60068744 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw30600356;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw30600356 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw83424786;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw83424786 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73169340;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73169340 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89516955;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89516955 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96278707;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96278707 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw21465396;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw21465396 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22195976;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22195976 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw56495703;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw56495703 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw31468719;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw31468719 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw67016853;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw67016853 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65930110;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65930110 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw94005994;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw94005994 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw39654487;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw39654487 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw62658833;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw62658833 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23096400;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23096400 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw78368343;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw78368343 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59436869;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59436869 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73210868;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73210868 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74713662;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74713662 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw99844967;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw99844967 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86760450;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86760450 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw18911918;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw18911918 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86431767;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw86431767 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw12804696;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw12804696 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85288828;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw85288828 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40521863;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40521863 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw75092899;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw75092899 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16078811;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16078811 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51640428;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51640428 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11615781;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11615781 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw92372347;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw92372347 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw14285775;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw14285775 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40970279;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw40970279 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5116227;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw5116227 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw38985623;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw38985623 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw41172722;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw41172722 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76853197;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw76853197 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw43441176;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw43441176 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw9075656;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw9075656 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15411081;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15411081 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64749953;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64749953 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23017321;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23017321 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65458197;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw65458197 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17753922;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17753922 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81093330;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81093330 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw77112816;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw77112816 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46034972;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46034972 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22928769;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw22928769 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw20953295;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw20953295 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw2963910;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw2963910 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17258144;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw17258144 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48867434;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48867434 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16099959;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw16099959 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48983768;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw48983768 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73308293;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw73308293 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11688438;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw11688438 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96993018;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw96993018 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60364644;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw60364644 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4228128;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4228128 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw55756844;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw55756844 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46372497;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw46372497 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw6117166;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw6117166 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4855276;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw4855276 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw19852938;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw19852938 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74644505;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw74644505 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51644336;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw51644336 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53035715;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53035715 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw34538261;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw34538261 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23673211;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw23673211 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81923678;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw81923678 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw1515146;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw1515146 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15995693;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw15995693 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64135212;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw64135212 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59302581;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw59302581 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw35095015;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw35095015 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw63743130;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw63743130 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53453721;     XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw53453721 = XyRPUmIfCyZPtQXBPILfRTNlBxmDVohrDtHayADpSHLQLJypdeKqgrTDkgNWAFKORiLzIw89799015;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BiWkjfNGFkuihQpVXIGJIctnXkhEpjeleUxMWOMFzaFLCDWIhHv11561025() {     float rlOrYpiMzyIRVFIEMTdzJwAKhWil13679896 = -629797113;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil801499 = -72712837;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil5308948 = -620909389;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil97044601 = -843908263;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil18504926 = -134231591;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil29581153 = -110553896;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil74090748 = -777626654;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil89393988 = -711766463;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil34466994 = -342937233;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil2189601 = -642159041;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil41864604 = -15184279;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil22362748 = -254348125;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil69325875 = -804416455;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil66500003 = -873381260;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil85284065 = -449333776;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil66299905 = -36138416;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil58504073 = -156869415;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil56595616 = -312395048;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil8868917 = -779087191;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil99130490 = -208525356;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil80140555 = -847515675;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil19536521 = -958471727;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil37684381 = -98430219;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil30123066 = 46300320;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil46163221 = -299086647;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil81338170 = -84822210;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil5078788 = -995339572;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil2287922 = -155157655;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil71373334 = -709578755;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil49632966 = -857757171;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil49284737 = -607750514;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil29794842 = -180204620;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil49224922 = -935301829;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil59392728 = 64335984;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil32762584 = -937126436;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil92340894 = -969119079;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil29143904 = -973426176;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil54480890 = -109391230;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil8479261 = -155146167;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil3484676 = -77184608;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil79001075 = -331894493;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil81251082 = -207950475;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil14921848 = -894074956;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil42353796 = 49853884;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil72717505 = -460004427;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil6792634 = -324190344;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil94231980 = -602045312;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil68200202 = 12975851;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil7133330 = -766228664;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil15862222 = 4943824;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil84881920 = -663949852;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil68332114 = -992070238;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil31587290 = -889946788;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil79862393 = -807536388;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil31420469 = -485359154;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil94143375 = -671325386;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil63117118 = -974282618;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil75185882 = -567209710;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil50881381 = -444821616;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil37166756 = 50590619;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil24502366 = -115214325;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil71802827 = -522469000;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil18020654 = 97812291;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil84834028 = -485180062;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil52904863 = 65591473;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil12069763 = -834979659;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil73137826 = -319046296;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil9933148 = -768752440;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil33737420 = -936254824;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil92943171 = -480214697;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil37156002 = -62712240;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil4023184 = 52521814;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil48116355 = -57248881;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil5384242 = -601902583;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil20129415 = -876630863;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil98889472 = -539565200;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil4614673 = 35603228;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil95330584 = -48284104;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil57405560 = -493695254;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil39370587 = -974896303;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil87106189 = -482776899;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil36878585 = -908315423;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil95154591 = -388928991;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil55511113 = -614522579;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil64751045 = -93807319;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil80952623 = -615680277;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil98207551 = -290257833;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil69362528 = -27765442;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil27972259 = -450304862;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil38619209 = -165801051;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil29223776 = -994836462;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil53958021 = -306216467;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil3599509 = -664569614;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil71312505 = -105736787;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil78982310 = -961970284;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil7198249 = -809425493;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil63230429 = -205762766;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil30087820 = -308894894;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil89448932 = 84262411;    float rlOrYpiMzyIRVFIEMTdzJwAKhWil60647743 = -629797113;     rlOrYpiMzyIRVFIEMTdzJwAKhWil13679896 = rlOrYpiMzyIRVFIEMTdzJwAKhWil801499;     rlOrYpiMzyIRVFIEMTdzJwAKhWil801499 = rlOrYpiMzyIRVFIEMTdzJwAKhWil5308948;     rlOrYpiMzyIRVFIEMTdzJwAKhWil5308948 = rlOrYpiMzyIRVFIEMTdzJwAKhWil97044601;     rlOrYpiMzyIRVFIEMTdzJwAKhWil97044601 = rlOrYpiMzyIRVFIEMTdzJwAKhWil18504926;     rlOrYpiMzyIRVFIEMTdzJwAKhWil18504926 = rlOrYpiMzyIRVFIEMTdzJwAKhWil29581153;     rlOrYpiMzyIRVFIEMTdzJwAKhWil29581153 = rlOrYpiMzyIRVFIEMTdzJwAKhWil74090748;     rlOrYpiMzyIRVFIEMTdzJwAKhWil74090748 = rlOrYpiMzyIRVFIEMTdzJwAKhWil89393988;     rlOrYpiMzyIRVFIEMTdzJwAKhWil89393988 = rlOrYpiMzyIRVFIEMTdzJwAKhWil34466994;     rlOrYpiMzyIRVFIEMTdzJwAKhWil34466994 = rlOrYpiMzyIRVFIEMTdzJwAKhWil2189601;     rlOrYpiMzyIRVFIEMTdzJwAKhWil2189601 = rlOrYpiMzyIRVFIEMTdzJwAKhWil41864604;     rlOrYpiMzyIRVFIEMTdzJwAKhWil41864604 = rlOrYpiMzyIRVFIEMTdzJwAKhWil22362748;     rlOrYpiMzyIRVFIEMTdzJwAKhWil22362748 = rlOrYpiMzyIRVFIEMTdzJwAKhWil69325875;     rlOrYpiMzyIRVFIEMTdzJwAKhWil69325875 = rlOrYpiMzyIRVFIEMTdzJwAKhWil66500003;     rlOrYpiMzyIRVFIEMTdzJwAKhWil66500003 = rlOrYpiMzyIRVFIEMTdzJwAKhWil85284065;     rlOrYpiMzyIRVFIEMTdzJwAKhWil85284065 = rlOrYpiMzyIRVFIEMTdzJwAKhWil66299905;     rlOrYpiMzyIRVFIEMTdzJwAKhWil66299905 = rlOrYpiMzyIRVFIEMTdzJwAKhWil58504073;     rlOrYpiMzyIRVFIEMTdzJwAKhWil58504073 = rlOrYpiMzyIRVFIEMTdzJwAKhWil56595616;     rlOrYpiMzyIRVFIEMTdzJwAKhWil56595616 = rlOrYpiMzyIRVFIEMTdzJwAKhWil8868917;     rlOrYpiMzyIRVFIEMTdzJwAKhWil8868917 = rlOrYpiMzyIRVFIEMTdzJwAKhWil99130490;     rlOrYpiMzyIRVFIEMTdzJwAKhWil99130490 = rlOrYpiMzyIRVFIEMTdzJwAKhWil80140555;     rlOrYpiMzyIRVFIEMTdzJwAKhWil80140555 = rlOrYpiMzyIRVFIEMTdzJwAKhWil19536521;     rlOrYpiMzyIRVFIEMTdzJwAKhWil19536521 = rlOrYpiMzyIRVFIEMTdzJwAKhWil37684381;     rlOrYpiMzyIRVFIEMTdzJwAKhWil37684381 = rlOrYpiMzyIRVFIEMTdzJwAKhWil30123066;     rlOrYpiMzyIRVFIEMTdzJwAKhWil30123066 = rlOrYpiMzyIRVFIEMTdzJwAKhWil46163221;     rlOrYpiMzyIRVFIEMTdzJwAKhWil46163221 = rlOrYpiMzyIRVFIEMTdzJwAKhWil81338170;     rlOrYpiMzyIRVFIEMTdzJwAKhWil81338170 = rlOrYpiMzyIRVFIEMTdzJwAKhWil5078788;     rlOrYpiMzyIRVFIEMTdzJwAKhWil5078788 = rlOrYpiMzyIRVFIEMTdzJwAKhWil2287922;     rlOrYpiMzyIRVFIEMTdzJwAKhWil2287922 = rlOrYpiMzyIRVFIEMTdzJwAKhWil71373334;     rlOrYpiMzyIRVFIEMTdzJwAKhWil71373334 = rlOrYpiMzyIRVFIEMTdzJwAKhWil49632966;     rlOrYpiMzyIRVFIEMTdzJwAKhWil49632966 = rlOrYpiMzyIRVFIEMTdzJwAKhWil49284737;     rlOrYpiMzyIRVFIEMTdzJwAKhWil49284737 = rlOrYpiMzyIRVFIEMTdzJwAKhWil29794842;     rlOrYpiMzyIRVFIEMTdzJwAKhWil29794842 = rlOrYpiMzyIRVFIEMTdzJwAKhWil49224922;     rlOrYpiMzyIRVFIEMTdzJwAKhWil49224922 = rlOrYpiMzyIRVFIEMTdzJwAKhWil59392728;     rlOrYpiMzyIRVFIEMTdzJwAKhWil59392728 = rlOrYpiMzyIRVFIEMTdzJwAKhWil32762584;     rlOrYpiMzyIRVFIEMTdzJwAKhWil32762584 = rlOrYpiMzyIRVFIEMTdzJwAKhWil92340894;     rlOrYpiMzyIRVFIEMTdzJwAKhWil92340894 = rlOrYpiMzyIRVFIEMTdzJwAKhWil29143904;     rlOrYpiMzyIRVFIEMTdzJwAKhWil29143904 = rlOrYpiMzyIRVFIEMTdzJwAKhWil54480890;     rlOrYpiMzyIRVFIEMTdzJwAKhWil54480890 = rlOrYpiMzyIRVFIEMTdzJwAKhWil8479261;     rlOrYpiMzyIRVFIEMTdzJwAKhWil8479261 = rlOrYpiMzyIRVFIEMTdzJwAKhWil3484676;     rlOrYpiMzyIRVFIEMTdzJwAKhWil3484676 = rlOrYpiMzyIRVFIEMTdzJwAKhWil79001075;     rlOrYpiMzyIRVFIEMTdzJwAKhWil79001075 = rlOrYpiMzyIRVFIEMTdzJwAKhWil81251082;     rlOrYpiMzyIRVFIEMTdzJwAKhWil81251082 = rlOrYpiMzyIRVFIEMTdzJwAKhWil14921848;     rlOrYpiMzyIRVFIEMTdzJwAKhWil14921848 = rlOrYpiMzyIRVFIEMTdzJwAKhWil42353796;     rlOrYpiMzyIRVFIEMTdzJwAKhWil42353796 = rlOrYpiMzyIRVFIEMTdzJwAKhWil72717505;     rlOrYpiMzyIRVFIEMTdzJwAKhWil72717505 = rlOrYpiMzyIRVFIEMTdzJwAKhWil6792634;     rlOrYpiMzyIRVFIEMTdzJwAKhWil6792634 = rlOrYpiMzyIRVFIEMTdzJwAKhWil94231980;     rlOrYpiMzyIRVFIEMTdzJwAKhWil94231980 = rlOrYpiMzyIRVFIEMTdzJwAKhWil68200202;     rlOrYpiMzyIRVFIEMTdzJwAKhWil68200202 = rlOrYpiMzyIRVFIEMTdzJwAKhWil7133330;     rlOrYpiMzyIRVFIEMTdzJwAKhWil7133330 = rlOrYpiMzyIRVFIEMTdzJwAKhWil15862222;     rlOrYpiMzyIRVFIEMTdzJwAKhWil15862222 = rlOrYpiMzyIRVFIEMTdzJwAKhWil84881920;     rlOrYpiMzyIRVFIEMTdzJwAKhWil84881920 = rlOrYpiMzyIRVFIEMTdzJwAKhWil68332114;     rlOrYpiMzyIRVFIEMTdzJwAKhWil68332114 = rlOrYpiMzyIRVFIEMTdzJwAKhWil31587290;     rlOrYpiMzyIRVFIEMTdzJwAKhWil31587290 = rlOrYpiMzyIRVFIEMTdzJwAKhWil79862393;     rlOrYpiMzyIRVFIEMTdzJwAKhWil79862393 = rlOrYpiMzyIRVFIEMTdzJwAKhWil31420469;     rlOrYpiMzyIRVFIEMTdzJwAKhWil31420469 = rlOrYpiMzyIRVFIEMTdzJwAKhWil94143375;     rlOrYpiMzyIRVFIEMTdzJwAKhWil94143375 = rlOrYpiMzyIRVFIEMTdzJwAKhWil63117118;     rlOrYpiMzyIRVFIEMTdzJwAKhWil63117118 = rlOrYpiMzyIRVFIEMTdzJwAKhWil75185882;     rlOrYpiMzyIRVFIEMTdzJwAKhWil75185882 = rlOrYpiMzyIRVFIEMTdzJwAKhWil50881381;     rlOrYpiMzyIRVFIEMTdzJwAKhWil50881381 = rlOrYpiMzyIRVFIEMTdzJwAKhWil37166756;     rlOrYpiMzyIRVFIEMTdzJwAKhWil37166756 = rlOrYpiMzyIRVFIEMTdzJwAKhWil24502366;     rlOrYpiMzyIRVFIEMTdzJwAKhWil24502366 = rlOrYpiMzyIRVFIEMTdzJwAKhWil71802827;     rlOrYpiMzyIRVFIEMTdzJwAKhWil71802827 = rlOrYpiMzyIRVFIEMTdzJwAKhWil18020654;     rlOrYpiMzyIRVFIEMTdzJwAKhWil18020654 = rlOrYpiMzyIRVFIEMTdzJwAKhWil84834028;     rlOrYpiMzyIRVFIEMTdzJwAKhWil84834028 = rlOrYpiMzyIRVFIEMTdzJwAKhWil52904863;     rlOrYpiMzyIRVFIEMTdzJwAKhWil52904863 = rlOrYpiMzyIRVFIEMTdzJwAKhWil12069763;     rlOrYpiMzyIRVFIEMTdzJwAKhWil12069763 = rlOrYpiMzyIRVFIEMTdzJwAKhWil73137826;     rlOrYpiMzyIRVFIEMTdzJwAKhWil73137826 = rlOrYpiMzyIRVFIEMTdzJwAKhWil9933148;     rlOrYpiMzyIRVFIEMTdzJwAKhWil9933148 = rlOrYpiMzyIRVFIEMTdzJwAKhWil33737420;     rlOrYpiMzyIRVFIEMTdzJwAKhWil33737420 = rlOrYpiMzyIRVFIEMTdzJwAKhWil92943171;     rlOrYpiMzyIRVFIEMTdzJwAKhWil92943171 = rlOrYpiMzyIRVFIEMTdzJwAKhWil37156002;     rlOrYpiMzyIRVFIEMTdzJwAKhWil37156002 = rlOrYpiMzyIRVFIEMTdzJwAKhWil4023184;     rlOrYpiMzyIRVFIEMTdzJwAKhWil4023184 = rlOrYpiMzyIRVFIEMTdzJwAKhWil48116355;     rlOrYpiMzyIRVFIEMTdzJwAKhWil48116355 = rlOrYpiMzyIRVFIEMTdzJwAKhWil5384242;     rlOrYpiMzyIRVFIEMTdzJwAKhWil5384242 = rlOrYpiMzyIRVFIEMTdzJwAKhWil20129415;     rlOrYpiMzyIRVFIEMTdzJwAKhWil20129415 = rlOrYpiMzyIRVFIEMTdzJwAKhWil98889472;     rlOrYpiMzyIRVFIEMTdzJwAKhWil98889472 = rlOrYpiMzyIRVFIEMTdzJwAKhWil4614673;     rlOrYpiMzyIRVFIEMTdzJwAKhWil4614673 = rlOrYpiMzyIRVFIEMTdzJwAKhWil95330584;     rlOrYpiMzyIRVFIEMTdzJwAKhWil95330584 = rlOrYpiMzyIRVFIEMTdzJwAKhWil57405560;     rlOrYpiMzyIRVFIEMTdzJwAKhWil57405560 = rlOrYpiMzyIRVFIEMTdzJwAKhWil39370587;     rlOrYpiMzyIRVFIEMTdzJwAKhWil39370587 = rlOrYpiMzyIRVFIEMTdzJwAKhWil87106189;     rlOrYpiMzyIRVFIEMTdzJwAKhWil87106189 = rlOrYpiMzyIRVFIEMTdzJwAKhWil36878585;     rlOrYpiMzyIRVFIEMTdzJwAKhWil36878585 = rlOrYpiMzyIRVFIEMTdzJwAKhWil95154591;     rlOrYpiMzyIRVFIEMTdzJwAKhWil95154591 = rlOrYpiMzyIRVFIEMTdzJwAKhWil55511113;     rlOrYpiMzyIRVFIEMTdzJwAKhWil55511113 = rlOrYpiMzyIRVFIEMTdzJwAKhWil64751045;     rlOrYpiMzyIRVFIEMTdzJwAKhWil64751045 = rlOrYpiMzyIRVFIEMTdzJwAKhWil80952623;     rlOrYpiMzyIRVFIEMTdzJwAKhWil80952623 = rlOrYpiMzyIRVFIEMTdzJwAKhWil98207551;     rlOrYpiMzyIRVFIEMTdzJwAKhWil98207551 = rlOrYpiMzyIRVFIEMTdzJwAKhWil69362528;     rlOrYpiMzyIRVFIEMTdzJwAKhWil69362528 = rlOrYpiMzyIRVFIEMTdzJwAKhWil27972259;     rlOrYpiMzyIRVFIEMTdzJwAKhWil27972259 = rlOrYpiMzyIRVFIEMTdzJwAKhWil38619209;     rlOrYpiMzyIRVFIEMTdzJwAKhWil38619209 = rlOrYpiMzyIRVFIEMTdzJwAKhWil29223776;     rlOrYpiMzyIRVFIEMTdzJwAKhWil29223776 = rlOrYpiMzyIRVFIEMTdzJwAKhWil53958021;     rlOrYpiMzyIRVFIEMTdzJwAKhWil53958021 = rlOrYpiMzyIRVFIEMTdzJwAKhWil3599509;     rlOrYpiMzyIRVFIEMTdzJwAKhWil3599509 = rlOrYpiMzyIRVFIEMTdzJwAKhWil71312505;     rlOrYpiMzyIRVFIEMTdzJwAKhWil71312505 = rlOrYpiMzyIRVFIEMTdzJwAKhWil78982310;     rlOrYpiMzyIRVFIEMTdzJwAKhWil78982310 = rlOrYpiMzyIRVFIEMTdzJwAKhWil7198249;     rlOrYpiMzyIRVFIEMTdzJwAKhWil7198249 = rlOrYpiMzyIRVFIEMTdzJwAKhWil63230429;     rlOrYpiMzyIRVFIEMTdzJwAKhWil63230429 = rlOrYpiMzyIRVFIEMTdzJwAKhWil30087820;     rlOrYpiMzyIRVFIEMTdzJwAKhWil30087820 = rlOrYpiMzyIRVFIEMTdzJwAKhWil89448932;     rlOrYpiMzyIRVFIEMTdzJwAKhWil89448932 = rlOrYpiMzyIRVFIEMTdzJwAKhWil60647743;     rlOrYpiMzyIRVFIEMTdzJwAKhWil60647743 = rlOrYpiMzyIRVFIEMTdzJwAKhWil13679896;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZqtDTBCHGpEmnzegcAmBSGAwO95901743() {     long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3257803 = -487019151;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm27727948 = -220493674;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47358518 = -999550287;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55141886 = -691883917;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89276284 = -334313824;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm2312078 = -738826395;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm22042922 = -370718745;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm15531801 = -262995511;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm67294408 = -299248859;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm40719135 = -176613573;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm23009028 = -219880385;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62178159 = -836706872;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm28224277 = -759317733;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm88009261 = -332018796;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90859177 = -369812447;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm21683226 = -663981925;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42387457 = 5956423;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm1081797 = -861752325;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm84385540 = 7020563;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16834360 = 33794084;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37655363 = -161363413;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90995593 = -816458337;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm35278625 = -729006547;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87493658 = -861191999;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49686033 = -70705369;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm78776 = -45243172;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57994422 = -322146413;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm75024241 = -435979722;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68893988 = -410195581;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm24383734 = -700601320;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42762335 = -166545535;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30306490 = -619404741;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3857037 = -803241237;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30917445 = -107039945;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm43826281 = 79052323;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm99413584 = -328521280;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47376005 = -848973574;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93110128 = -696867027;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19502368 = -190917914;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37542306 = -696176758;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm18317489 = -485550543;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm54262294 = -455913164;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49653349 = -142760461;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57674132 = 75995274;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49509004 = -332914828;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm4955745 = 13313756;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68171044 = -278059668;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44223864 = -703784174;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19456146 = -196834116;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34824146 = -310546921;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm73020384 = -121669525;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm38786816 = 45621618;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13843643 = -832851139;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62221520 = -276903427;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58246381 = -113765453;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm12262209 = -670560814;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92449322 = -491487127;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59864860 = -38358289;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm5455853 = -521178549;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89197509 = -189070653;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44317656 = -316679982;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47018680 = -934739024;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46637813 = -852799931;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42910674 = -598647539;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97956799 = 89931962;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92702537 = -600475645;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58321122 = 66534364;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97306832 = -552277788;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44182981 = -311071119;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm91445592 = 58708833;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm74307221 = -815008352;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49277329 = -297176550;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm81579428 = -570834411;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46843234 = -296802679;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm98516870 = -480655373;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm83393069 = -705450249;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41342244 = -573697877;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm77604493 = -705001822;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37984654 = -428277172;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44730289 = 15980875;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31907732 = -767183505;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13770559 = -618362239;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55568096 = -139145607;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34069842 = 351340;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm51363350 = -478931796;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3975520 = -112167154;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16462848 = -786553603;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41635517 = -426337810;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm72671064 = -993274492;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31564072 = -250386863;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6964263 = -837034153;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87511145 = -710615285;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87654275 = -75688479;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30304858 = 98152738;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93224650 = -279496776;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm71298809 = -550811520;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm7624482 = -603113233;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6742675 = -544112922;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59717333 = 86063312;    long uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm56806467 = -487019151;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3257803 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm27727948;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm27727948 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47358518;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47358518 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55141886;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55141886 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89276284;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89276284 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm2312078;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm2312078 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm22042922;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm22042922 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm15531801;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm15531801 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm67294408;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm67294408 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm40719135;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm40719135 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm23009028;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm23009028 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62178159;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62178159 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm28224277;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm28224277 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm88009261;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm88009261 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90859177;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90859177 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm21683226;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm21683226 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42387457;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42387457 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm1081797;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm1081797 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm84385540;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm84385540 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16834360;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16834360 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37655363;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37655363 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90995593;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm90995593 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm35278625;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm35278625 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87493658;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87493658 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49686033;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49686033 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm78776;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm78776 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57994422;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57994422 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm75024241;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm75024241 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68893988;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68893988 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm24383734;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm24383734 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42762335;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42762335 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30306490;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30306490 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3857037;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3857037 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30917445;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30917445 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm43826281;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm43826281 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm99413584;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm99413584 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47376005;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47376005 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93110128;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93110128 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19502368;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19502368 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37542306;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37542306 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm18317489;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm18317489 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm54262294;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm54262294 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49653349;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49653349 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57674132;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm57674132 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49509004;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49509004 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm4955745;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm4955745 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68171044;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm68171044 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44223864;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44223864 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19456146;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm19456146 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34824146;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34824146 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm73020384;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm73020384 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm38786816;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm38786816 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13843643;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13843643 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62221520;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm62221520 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58246381;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58246381 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm12262209;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm12262209 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92449322;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92449322 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59864860;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59864860 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm5455853;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm5455853 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89197509;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm89197509 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44317656;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44317656 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47018680;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm47018680 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46637813;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46637813 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42910674;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm42910674 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97956799;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97956799 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92702537;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm92702537 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58321122;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm58321122 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97306832;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm97306832 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44182981;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44182981 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm91445592;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm91445592 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm74307221;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm74307221 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49277329;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm49277329 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm81579428;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm81579428 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46843234;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm46843234 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm98516870;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm98516870 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm83393069;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm83393069 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41342244;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41342244 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm77604493;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm77604493 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37984654;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm37984654 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44730289;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm44730289 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31907732;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31907732 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13770559;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm13770559 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55568096;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm55568096 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34069842;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm34069842 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm51363350;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm51363350 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3975520;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3975520 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16462848;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm16462848 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41635517;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm41635517 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm72671064;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm72671064 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31564072;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm31564072 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6964263;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6964263 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87511145;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87511145 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87654275;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm87654275 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30304858;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm30304858 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93224650;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm93224650 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm71298809;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm71298809 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm7624482;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm7624482 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6742675;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm6742675 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59717333;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm59717333 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm56806467;     uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm56806467 = uJggqDPAhOrODclgHjgjljhjINCWmkTqwoYrVaQlomGlm3257803;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wwxzGMdZgGSaHCKxUUBWHjYSOkirRETbAmyNT87223576() {     double VHghXShFgyuxrhNRPuNYf10566811 = -884248604;    double VHghXShFgyuxrhNRPuNYf26647358 = -847901534;    double VHghXShFgyuxrhNRPuNYf32773024 = -946742570;    double VHghXShFgyuxrhNRPuNYf53080882 = -770299899;    double VHghXShFgyuxrhNRPuNYf85980252 = 79893351;    double VHghXShFgyuxrhNRPuNYf84453080 = 76585362;    double VHghXShFgyuxrhNRPuNYf83186964 = -235039587;    double VHghXShFgyuxrhNRPuNYf55431881 = -260297300;    double VHghXShFgyuxrhNRPuNYf80195583 = -296314671;    double VHghXShFgyuxrhNRPuNYf87540146 = 90790114;    double VHghXShFgyuxrhNRPuNYf68164167 = -335712339;    double VHghXShFgyuxrhNRPuNYf98704724 = -867256415;    double VHghXShFgyuxrhNRPuNYf48276772 = -159810645;    double VHghXShFgyuxrhNRPuNYf47627896 = -242549773;    double VHghXShFgyuxrhNRPuNYf64238520 = -1325411;    double VHghXShFgyuxrhNRPuNYf25196060 = -201532326;    double VHghXShFgyuxrhNRPuNYf90225618 = -139801372;    double VHghXShFgyuxrhNRPuNYf64697938 = -523555730;    double VHghXShFgyuxrhNRPuNYf80070261 = -860657826;    double VHghXShFgyuxrhNRPuNYf63438398 = -334160895;    double VHghXShFgyuxrhNRPuNYf81796246 = -476231902;    double VHghXShFgyuxrhNRPuNYf19591361 = -865265123;    double VHghXShFgyuxrhNRPuNYf8915961 = -788659507;    double VHghXShFgyuxrhNRPuNYf30848196 = -953866490;    double VHghXShFgyuxrhNRPuNYf75107149 = -572643226;    double VHghXShFgyuxrhNRPuNYf95024946 = -972901055;    double VHghXShFgyuxrhNRPuNYf48564948 = -26895396;    double VHghXShFgyuxrhNRPuNYf9273894 = -99446130;    double VHghXShFgyuxrhNRPuNYf86579137 = 38657771;    double VHghXShFgyuxrhNRPuNYf7878740 = -91841211;    double VHghXShFgyuxrhNRPuNYf37178379 = 95442509;    double VHghXShFgyuxrhNRPuNYf53548683 = -847135129;    double VHghXShFgyuxrhNRPuNYf43235051 = -419492579;    double VHghXShFgyuxrhNRPuNYf67245665 = -616592349;    double VHghXShFgyuxrhNRPuNYf67204695 = 84509438;    double VHghXShFgyuxrhNRPuNYf71559650 = -50264697;    double VHghXShFgyuxrhNRPuNYf62328673 = -356955647;    double VHghXShFgyuxrhNRPuNYf39867649 = -176854946;    double VHghXShFgyuxrhNRPuNYf86892355 = -698957445;    double VHghXShFgyuxrhNRPuNYf60767480 = -320442259;    double VHghXShFgyuxrhNRPuNYf27395790 = -776820075;    double VHghXShFgyuxrhNRPuNYf75531653 = 97299522;    double VHghXShFgyuxrhNRPuNYf89005698 = -128253795;    double VHghXShFgyuxrhNRPuNYf3252164 = -310788430;    double VHghXShFgyuxrhNRPuNYf5304220 = -825765960;    double VHghXShFgyuxrhNRPuNYf65992528 = -540632793;    double VHghXShFgyuxrhNRPuNYf94858240 = -75747048;    double VHghXShFgyuxrhNRPuNYf85264786 = 23382843;    double VHghXShFgyuxrhNRPuNYf13560698 = -943367043;    double VHghXShFgyuxrhNRPuNYf61039509 = -424794852;    double VHghXShFgyuxrhNRPuNYf48229367 = -171888724;    double VHghXShFgyuxrhNRPuNYf47347439 = -302976262;    double VHghXShFgyuxrhNRPuNYf86548481 = -955764765;    double VHghXShFgyuxrhNRPuNYf14003 = 15955247;    double VHghXShFgyuxrhNRPuNYf5427331 = -526192006;    double VHghXShFgyuxrhNRPuNYf90975449 = 81016519;    double VHghXShFgyuxrhNRPuNYf17731397 = 40757973;    double VHghXShFgyuxrhNRPuNYf1924828 = -992876080;    double VHghXShFgyuxrhNRPuNYf77973733 = -97656674;    double VHghXShFgyuxrhNRPuNYf90955305 = 52794405;    double VHghXShFgyuxrhNRPuNYf35888133 = -896519243;    double VHghXShFgyuxrhNRPuNYf73913071 = -35593457;    double VHghXShFgyuxrhNRPuNYf68852743 = -198955071;    double VHghXShFgyuxrhNRPuNYf72316843 = -104473460;    double VHghXShFgyuxrhNRPuNYf50361768 = 95347604;    double VHghXShFgyuxrhNRPuNYf14615484 = -488577210;    double VHghXShFgyuxrhNRPuNYf55469674 = -347763836;    double VHghXShFgyuxrhNRPuNYf81031107 = -543218297;    double VHghXShFgyuxrhNRPuNYf80423201 = -227059212;    double VHghXShFgyuxrhNRPuNYf92678869 = -951060715;    double VHghXShFgyuxrhNRPuNYf62867387 = -844576680;    double VHghXShFgyuxrhNRPuNYf50357970 = -962946427;    double VHghXShFgyuxrhNRPuNYf77805583 = -824598285;    double VHghXShFgyuxrhNRPuNYf19302782 = -440215568;    double VHghXShFgyuxrhNRPuNYf36042609 = -557340821;    double VHghXShFgyuxrhNRPuNYf6264593 = -473531425;    double VHghXShFgyuxrhNRPuNYf30585662 = -637011329;    double VHghXShFgyuxrhNRPuNYf5663798 = -377871078;    double VHghXShFgyuxrhNRPuNYf25543977 = -28100531;    double VHghXShFgyuxrhNRPuNYf9114622 = 67989566;    double VHghXShFgyuxrhNRPuNYf166707 = -797154007;    double VHghXShFgyuxrhNRPuNYf63300162 = 49721761;    double VHghXShFgyuxrhNRPuNYf95713196 = -156079088;    double VHghXShFgyuxrhNRPuNYf25539628 = -536547377;    double VHghXShFgyuxrhNRPuNYf59649373 = -919952488;    double VHghXShFgyuxrhNRPuNYf89830940 = -601581229;    double VHghXShFgyuxrhNRPuNYf67000202 = -891370364;    double VHghXShFgyuxrhNRPuNYf43221049 = -335447826;    double VHghXShFgyuxrhNRPuNYf61818335 = 9599657;    double VHghXShFgyuxrhNRPuNYf76229245 = -996507081;    double VHghXShFgyuxrhNRPuNYf53828254 = 8977330;    double VHghXShFgyuxrhNRPuNYf60403846 = -364079567;    double VHghXShFgyuxrhNRPuNYf61893915 = 20801728;    double VHghXShFgyuxrhNRPuNYf95937049 = -651751851;    double VHghXShFgyuxrhNRPuNYf24879348 = -423923017;    double VHghXShFgyuxrhNRPuNYf53482719 = -641226618;    double VHghXShFgyuxrhNRPuNYf6678910 = -703745407;    double VHghXShFgyuxrhNRPuNYf16688856 = 76219664;    double VHghXShFgyuxrhNRPuNYf52890396 = -306136035;    double VHghXShFgyuxrhNRPuNYf90688735 = -884248604;     VHghXShFgyuxrhNRPuNYf10566811 = VHghXShFgyuxrhNRPuNYf26647358;     VHghXShFgyuxrhNRPuNYf26647358 = VHghXShFgyuxrhNRPuNYf32773024;     VHghXShFgyuxrhNRPuNYf32773024 = VHghXShFgyuxrhNRPuNYf53080882;     VHghXShFgyuxrhNRPuNYf53080882 = VHghXShFgyuxrhNRPuNYf85980252;     VHghXShFgyuxrhNRPuNYf85980252 = VHghXShFgyuxrhNRPuNYf84453080;     VHghXShFgyuxrhNRPuNYf84453080 = VHghXShFgyuxrhNRPuNYf83186964;     VHghXShFgyuxrhNRPuNYf83186964 = VHghXShFgyuxrhNRPuNYf55431881;     VHghXShFgyuxrhNRPuNYf55431881 = VHghXShFgyuxrhNRPuNYf80195583;     VHghXShFgyuxrhNRPuNYf80195583 = VHghXShFgyuxrhNRPuNYf87540146;     VHghXShFgyuxrhNRPuNYf87540146 = VHghXShFgyuxrhNRPuNYf68164167;     VHghXShFgyuxrhNRPuNYf68164167 = VHghXShFgyuxrhNRPuNYf98704724;     VHghXShFgyuxrhNRPuNYf98704724 = VHghXShFgyuxrhNRPuNYf48276772;     VHghXShFgyuxrhNRPuNYf48276772 = VHghXShFgyuxrhNRPuNYf47627896;     VHghXShFgyuxrhNRPuNYf47627896 = VHghXShFgyuxrhNRPuNYf64238520;     VHghXShFgyuxrhNRPuNYf64238520 = VHghXShFgyuxrhNRPuNYf25196060;     VHghXShFgyuxrhNRPuNYf25196060 = VHghXShFgyuxrhNRPuNYf90225618;     VHghXShFgyuxrhNRPuNYf90225618 = VHghXShFgyuxrhNRPuNYf64697938;     VHghXShFgyuxrhNRPuNYf64697938 = VHghXShFgyuxrhNRPuNYf80070261;     VHghXShFgyuxrhNRPuNYf80070261 = VHghXShFgyuxrhNRPuNYf63438398;     VHghXShFgyuxrhNRPuNYf63438398 = VHghXShFgyuxrhNRPuNYf81796246;     VHghXShFgyuxrhNRPuNYf81796246 = VHghXShFgyuxrhNRPuNYf19591361;     VHghXShFgyuxrhNRPuNYf19591361 = VHghXShFgyuxrhNRPuNYf8915961;     VHghXShFgyuxrhNRPuNYf8915961 = VHghXShFgyuxrhNRPuNYf30848196;     VHghXShFgyuxrhNRPuNYf30848196 = VHghXShFgyuxrhNRPuNYf75107149;     VHghXShFgyuxrhNRPuNYf75107149 = VHghXShFgyuxrhNRPuNYf95024946;     VHghXShFgyuxrhNRPuNYf95024946 = VHghXShFgyuxrhNRPuNYf48564948;     VHghXShFgyuxrhNRPuNYf48564948 = VHghXShFgyuxrhNRPuNYf9273894;     VHghXShFgyuxrhNRPuNYf9273894 = VHghXShFgyuxrhNRPuNYf86579137;     VHghXShFgyuxrhNRPuNYf86579137 = VHghXShFgyuxrhNRPuNYf7878740;     VHghXShFgyuxrhNRPuNYf7878740 = VHghXShFgyuxrhNRPuNYf37178379;     VHghXShFgyuxrhNRPuNYf37178379 = VHghXShFgyuxrhNRPuNYf53548683;     VHghXShFgyuxrhNRPuNYf53548683 = VHghXShFgyuxrhNRPuNYf43235051;     VHghXShFgyuxrhNRPuNYf43235051 = VHghXShFgyuxrhNRPuNYf67245665;     VHghXShFgyuxrhNRPuNYf67245665 = VHghXShFgyuxrhNRPuNYf67204695;     VHghXShFgyuxrhNRPuNYf67204695 = VHghXShFgyuxrhNRPuNYf71559650;     VHghXShFgyuxrhNRPuNYf71559650 = VHghXShFgyuxrhNRPuNYf62328673;     VHghXShFgyuxrhNRPuNYf62328673 = VHghXShFgyuxrhNRPuNYf39867649;     VHghXShFgyuxrhNRPuNYf39867649 = VHghXShFgyuxrhNRPuNYf86892355;     VHghXShFgyuxrhNRPuNYf86892355 = VHghXShFgyuxrhNRPuNYf60767480;     VHghXShFgyuxrhNRPuNYf60767480 = VHghXShFgyuxrhNRPuNYf27395790;     VHghXShFgyuxrhNRPuNYf27395790 = VHghXShFgyuxrhNRPuNYf75531653;     VHghXShFgyuxrhNRPuNYf75531653 = VHghXShFgyuxrhNRPuNYf89005698;     VHghXShFgyuxrhNRPuNYf89005698 = VHghXShFgyuxrhNRPuNYf3252164;     VHghXShFgyuxrhNRPuNYf3252164 = VHghXShFgyuxrhNRPuNYf5304220;     VHghXShFgyuxrhNRPuNYf5304220 = VHghXShFgyuxrhNRPuNYf65992528;     VHghXShFgyuxrhNRPuNYf65992528 = VHghXShFgyuxrhNRPuNYf94858240;     VHghXShFgyuxrhNRPuNYf94858240 = VHghXShFgyuxrhNRPuNYf85264786;     VHghXShFgyuxrhNRPuNYf85264786 = VHghXShFgyuxrhNRPuNYf13560698;     VHghXShFgyuxrhNRPuNYf13560698 = VHghXShFgyuxrhNRPuNYf61039509;     VHghXShFgyuxrhNRPuNYf61039509 = VHghXShFgyuxrhNRPuNYf48229367;     VHghXShFgyuxrhNRPuNYf48229367 = VHghXShFgyuxrhNRPuNYf47347439;     VHghXShFgyuxrhNRPuNYf47347439 = VHghXShFgyuxrhNRPuNYf86548481;     VHghXShFgyuxrhNRPuNYf86548481 = VHghXShFgyuxrhNRPuNYf14003;     VHghXShFgyuxrhNRPuNYf14003 = VHghXShFgyuxrhNRPuNYf5427331;     VHghXShFgyuxrhNRPuNYf5427331 = VHghXShFgyuxrhNRPuNYf90975449;     VHghXShFgyuxrhNRPuNYf90975449 = VHghXShFgyuxrhNRPuNYf17731397;     VHghXShFgyuxrhNRPuNYf17731397 = VHghXShFgyuxrhNRPuNYf1924828;     VHghXShFgyuxrhNRPuNYf1924828 = VHghXShFgyuxrhNRPuNYf77973733;     VHghXShFgyuxrhNRPuNYf77973733 = VHghXShFgyuxrhNRPuNYf90955305;     VHghXShFgyuxrhNRPuNYf90955305 = VHghXShFgyuxrhNRPuNYf35888133;     VHghXShFgyuxrhNRPuNYf35888133 = VHghXShFgyuxrhNRPuNYf73913071;     VHghXShFgyuxrhNRPuNYf73913071 = VHghXShFgyuxrhNRPuNYf68852743;     VHghXShFgyuxrhNRPuNYf68852743 = VHghXShFgyuxrhNRPuNYf72316843;     VHghXShFgyuxrhNRPuNYf72316843 = VHghXShFgyuxrhNRPuNYf50361768;     VHghXShFgyuxrhNRPuNYf50361768 = VHghXShFgyuxrhNRPuNYf14615484;     VHghXShFgyuxrhNRPuNYf14615484 = VHghXShFgyuxrhNRPuNYf55469674;     VHghXShFgyuxrhNRPuNYf55469674 = VHghXShFgyuxrhNRPuNYf81031107;     VHghXShFgyuxrhNRPuNYf81031107 = VHghXShFgyuxrhNRPuNYf80423201;     VHghXShFgyuxrhNRPuNYf80423201 = VHghXShFgyuxrhNRPuNYf92678869;     VHghXShFgyuxrhNRPuNYf92678869 = VHghXShFgyuxrhNRPuNYf62867387;     VHghXShFgyuxrhNRPuNYf62867387 = VHghXShFgyuxrhNRPuNYf50357970;     VHghXShFgyuxrhNRPuNYf50357970 = VHghXShFgyuxrhNRPuNYf77805583;     VHghXShFgyuxrhNRPuNYf77805583 = VHghXShFgyuxrhNRPuNYf19302782;     VHghXShFgyuxrhNRPuNYf19302782 = VHghXShFgyuxrhNRPuNYf36042609;     VHghXShFgyuxrhNRPuNYf36042609 = VHghXShFgyuxrhNRPuNYf6264593;     VHghXShFgyuxrhNRPuNYf6264593 = VHghXShFgyuxrhNRPuNYf30585662;     VHghXShFgyuxrhNRPuNYf30585662 = VHghXShFgyuxrhNRPuNYf5663798;     VHghXShFgyuxrhNRPuNYf5663798 = VHghXShFgyuxrhNRPuNYf25543977;     VHghXShFgyuxrhNRPuNYf25543977 = VHghXShFgyuxrhNRPuNYf9114622;     VHghXShFgyuxrhNRPuNYf9114622 = VHghXShFgyuxrhNRPuNYf166707;     VHghXShFgyuxrhNRPuNYf166707 = VHghXShFgyuxrhNRPuNYf63300162;     VHghXShFgyuxrhNRPuNYf63300162 = VHghXShFgyuxrhNRPuNYf95713196;     VHghXShFgyuxrhNRPuNYf95713196 = VHghXShFgyuxrhNRPuNYf25539628;     VHghXShFgyuxrhNRPuNYf25539628 = VHghXShFgyuxrhNRPuNYf59649373;     VHghXShFgyuxrhNRPuNYf59649373 = VHghXShFgyuxrhNRPuNYf89830940;     VHghXShFgyuxrhNRPuNYf89830940 = VHghXShFgyuxrhNRPuNYf67000202;     VHghXShFgyuxrhNRPuNYf67000202 = VHghXShFgyuxrhNRPuNYf43221049;     VHghXShFgyuxrhNRPuNYf43221049 = VHghXShFgyuxrhNRPuNYf61818335;     VHghXShFgyuxrhNRPuNYf61818335 = VHghXShFgyuxrhNRPuNYf76229245;     VHghXShFgyuxrhNRPuNYf76229245 = VHghXShFgyuxrhNRPuNYf53828254;     VHghXShFgyuxrhNRPuNYf53828254 = VHghXShFgyuxrhNRPuNYf60403846;     VHghXShFgyuxrhNRPuNYf60403846 = VHghXShFgyuxrhNRPuNYf61893915;     VHghXShFgyuxrhNRPuNYf61893915 = VHghXShFgyuxrhNRPuNYf95937049;     VHghXShFgyuxrhNRPuNYf95937049 = VHghXShFgyuxrhNRPuNYf24879348;     VHghXShFgyuxrhNRPuNYf24879348 = VHghXShFgyuxrhNRPuNYf53482719;     VHghXShFgyuxrhNRPuNYf53482719 = VHghXShFgyuxrhNRPuNYf6678910;     VHghXShFgyuxrhNRPuNYf6678910 = VHghXShFgyuxrhNRPuNYf16688856;     VHghXShFgyuxrhNRPuNYf16688856 = VHghXShFgyuxrhNRPuNYf52890396;     VHghXShFgyuxrhNRPuNYf52890396 = VHghXShFgyuxrhNRPuNYf90688735;     VHghXShFgyuxrhNRPuNYf90688735 = VHghXShFgyuxrhNRPuNYf10566811;}
// Junk Finished
