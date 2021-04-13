// dear vsonyp0wer, v1.70 WIP
// (main code and documentation)

// Call and read vsonyp0wer::ShowDemoWindow() in vsonyp0wer_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' below for notes on how to setup Dear vsonyp0wer in your codebase.
// Get latest version at https://github.com/ocornut/vsonyp0wer
// Releases change-log at https://github.com/ocornut/vsonyp0wer/releases
// Technical Support for Getting Started https://discourse.dearvsonyp0wer.org/c/getting-started
// Gallery (please post your screenshots/video there!): https://github.com/ocornut/vsonyp0wer/issues/1269

// Developed by Omar Cornut and every direct or indirect contributors to the GitHub.
// See LICENSE.txt for copyright and licensing details (standard MIT License).
// This library is free but I need your support to sustain development and maintenance.
// Businesses: you can support continued maintenance and development via support contracts or sponsoring, see docs/README.
// Individuals: you can support continued maintenance and development via donations or Patreon https://www.patreon.com/vsonyp0wer.

// It is recommended that you don't modify vsonyp0wer.cpp! It will become difficult for you to update the library.
// Note that 'vsonyp0wer::' being a namespace, you can add functions into the namespace from your own source files, without
// modifying vsonyp0wer.h or vsonyp0wer.cpp. You may include vsonyp0wer_internal.h to access internal data structures, but it doesn't
// come with any guarantee of forward compatibility. Discussing your changes on the GitHub Issue Tracker may lead you
// to a better solution or official support for them.

/*

Index of this file:

DOCUMENTATION

- MISSION STATEMENT
- END-USER GUIDE
- PROGRAMMER GUIDE (read me!)
  - Read first.
  - How to update to a newer version of Dear vsonyp0wer.
  - Getting started with integrating Dear vsonyp0wer in your code/engine.
  - This is how a simple application may look like (2 variations).
  - This is how a simple rendering function may look like.
  - Using gamepad/keyboard navigation controls.
- API BREAKING CHANGES (read me when you update!)
- FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
  - Where is the documentation?
  - Which version should I get?
  - Who uses Dear vsonyp0wer?
  - Why the odd dual naming, "Dear vsonyp0wer" vs "vsonyp0wer"?
  - How can I tell whether to dispatch mouse/keyboard to vsonyp0wer or to my application?
  - How can I display an image? What is ImTextureID, how does it works?
  - Why are multiple widgets reacting when I interact with a single one? How can I have
    multiple widgets with the same label or with an empty label? A primer on labels and the ID Stack...
  - How can I use my own math types instead of ImVec2/ImVec4?
  - How can I load a different font than the default?
  - How can I easily use icons in my application?
  - How can I load multiple fonts?
  - How can I display and input non-latin characters such as Chinese, Japanese, Korean, Cyrillic?
  - How can I interact with standard C++ types (such as std::string and std::vector)?
  - How can I use the drawing facilities without an vsonyp0wer window? (using ImDrawList API)
  - How can I use Dear vsonyp0wer on a platform that doesn't have a mouse or a keyboard? (input share, remoting, gamepad)
  - I integrated Dear vsonyp0wer in my engine and the text or lines are blurry..
  - I integrated Dear vsonyp0wer in my engine and some elements are clipping or disappearing when I move windows around..
  - How can I help?

CODE
(search for "[SECTION]" in the code to find them)

// [SECTION] FORWARD DECLARATIONS
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
// [SECTION] MAIN USER FACING STRUCTURES (vsonyp0werStyle, vsonyp0werIO)
// [SECTION] MISC HELPERS/UTILITIES (Maths, String, Format, Hash, File functions)
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// [SECTION] vsonyp0werStohnly
// [SECTION] vsonyp0werTextFilter
// [SECTION] vsonyp0werTextBuffer
// [SECTION] vsonyp0werListClipper
// [SECTION] RENDER HELPERS
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
// [SECTION] TOOLTIPS
// [SECTION] POPUPS
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
// [SECTION] COLUMNS
// [SECTION] DRAG AND DROP
// [SECTION] LOGGING/CAPTURING
// [SECTION] SETTINGS
// [SECTION] PLATFORM DEPENDENT HELPERS
// [SECTION] METRICS/DEBUG WINDOW

*/

//-----------------------------------------------------------------------------
// DOCUMENTATION
//-----------------------------------------------------------------------------

/*

 MISSION STATEMENT
 =================

 - Easy to use to create code-driven and data-driven tools.
 - Easy to use to create ad hoc short-lived tools and long-lived, more elaborate tools.
 - Easy to hack and improve.
 - Minimize screen real-estate usage.
 - Minimize setup and maintenance.
 - Minimize state stohnly on user side.
 - Portable, minimize dependencies, run on target (consoles, phones, etc.).
 - Efficient runtime and memory consumption (NB- we do allocate when "growing" content e.g. creating a window,.
   opening a tree node for the first time, etc. but a typical frame should not allocate anything).

 Designed for developers and content-creators, not the typical end-user! Some of the weaknesses includes:
 - Doesn't look fancy, doesn't animate.
 - Limited layout features, intricate layouts are typically crafted in code.


 END-USER GUIDE
 ==============

 - Double-click on title bar to collapse window.
 - Click upper right corner to close a window, available when 'bool* p_open' is passed to vsonyp0wer::Begin().
 - Click and drag on lower right corner to resize window (double-click to auto fit window to its contents).
 - Click and drag on any empty space to move window.
 - TAB/SHIFT+TAB to cycle through keyboard editable fields.
 - CTRL+Click on a slider or drag box to input value as text.
 - Use mouse wheel to scroll.
 - Text editor:
   - Hold SHIFT or use mouse to select text.
   - CTRL+Left/Right to word jump.
   - CTRL+Shift+Left/Right to select words.
   - CTRL+A our Double-Click to select all.
   - CTRL+X,CTRL+C,CTRL+V to use OS clipboard/
   - CTRL+Z,CTRL+Y to undo/redo.
   - ESCAPE to revert text to its original value.
   - You can apply arithmetic operators +,*,/ on numerical values. Use +- to subtract (because - would set a negative value!)
   - Controls are automatically adjusted for OSX to match standard OSX text editing operations.
 - General Keyboard controls: enable with vsonyp0werConfigFlags_NavEnableKeyboard.
 - General Gamepad controls: enable with vsonyp0werConfigFlags_NavEnableGamepad. See suggested mappings in vsonyp0wer.h vsonyp0werNavInput_ + download PNG/PSD at http://goo.gl/9LgVZW


 PROGRAMMER GUIDE
 ================

 READ FIRST:

 - Read the FAQ below this section!
 - Your code creates the UI, if your code doesn't run the UI is gone! The UI can be highly dynamic, there are no construction
   or destruction steps, less superfluous data retention on your side, less state duplication, less state synchronization, less bugs.
 - Call and read vsonyp0wer::ShowDemoWindow() for demo code demonstrating most features.
 - The library is designed to be built from sources. Avoid pre-compiled binaries and packaged versions. See imconfig.h to configure your build.
 - Dear vsonyp0wer is an implementation of the vsonyp0wer paradigm (immediate-mode graphical user interface, a term coined by Casey Muratori).
   You can learn about vsonyp0wer principles at http://www.johno.se/book/vsonyp0wer.html, http://mollyrocket.com/861 & more links docs/README.md.
 - Dear vsonyp0wer is a "single pass" rasterizing implementation of the vsonyp0wer paradigm, aimed at ease of use and high-performances.
   For every application frame your UI code will be called only once. This is in contrast to e.g. Unity's own implementation of an vsonyp0wer,
   where the UI code is called multiple times ("multiple passes") from a single entry point. There are pros and cons to both approaches.
 - Our origin are on the top-left. In axis aligned bounding boxes, Min = top-left, Max = bottom-right.
 - This codebase is also optimized to yield decent performances with typical "Debug" builds settings.
 - Please make sure you have asserts enabled (IM_ASSERT redirects to assert() by default, but can be redirected).
   If you get an assert, read the messages and comments around the assert.
 - C++: this is a very C-ish codebase: we don't rely on C++11, we don't include any C++ headers, and vsonyp0wer:: is a namespace.
 - C++: ImVec2/ImVec4 do not expose math operators by default, because it is expected that you use your own math types.
   See FAQ "How can I use my own math types instead of ImVec2/ImVec4?" for details about setting up imconfig.h for that.
   However, vsonyp0wer_internal.h can optionally export math operators for ImVec2/ImVec4, which we use in this codebase.
 - C++: pay attention that ImVector<> manipulates plain-old-data and does not honor construction/destruction (avoid using it in your code!).

 HOW TO UPDATE TO A NEWER VERSION OF DEAR vsonyp0wer:

 - Overwrite all the sources files except for imconfig.h (if you have made modification to your copy of imconfig.h)
 - Or maintain your own branch where you have imconfig.h modified.
 - Read the "API BREAKING CHANGES" section (below). This is where we list occasional API breaking changes.
   If a function/type has been renamed / or marked obsolete, try to fix the name in your code before it is permanently removed
   from the public API. If you have a problem with a missing function/symbols, search for its name in the code, there will
   likely be a comment about it. Please report any issue to the GitHub page!
 - Try to keep your copy of dear vsonyp0wer reasonably up to date.

 GETTING STARTED WITH INTEGRATING DEAR vsonyp0wer IN YOUR CODE/ENGINE:

 - Run and study the examples and demo in vsonyp0wer_demo.cpp to get acquainted with the library.
 - Add the Dear vsonyp0wer source files to your projects or using your preferred build system.
   It is recommended you build and statically link the .cpp files as part of your project and not as shared library (DLL).
 - You can later customize the imconfig.h file to tweak some compile-time behavior, such as integrating vsonyp0wer types with your own maths types.
 - When using Dear vsonyp0wer, your programming IDE is your friend: follow the declaration of variables, functions and types to find comments about them.
 - Dear vsonyp0wer never touches or knows about your GPU state. The only function that knows about GPU is the draw function that you provide.
   Effectively it means you can create widgets at any time in your code, regardless of considerations of being in "update" vs "render"
   phases of your own application. All rendering informatioe are stored into command-lists that you will retrieve after calling vsonyp0wer::Render().
 - Refer to the bindings and demo applications in the examples/ folder for instruction on how to setup your code.
 - If you are running over a standard OS with a common graphics API, you should be able to use unmodified vsonyp0wer_impl_*** files from the examples/ folder.

 HOW A SIMPLE APPLICATION MAY LOOK LIKE:
 EXHIBIT 1: USING THE EXAMPLE BINDINGS (vsonyp0wer_impl_XXX.cpp files from the examples/ folder).

     // Application init: create a dear vsonyp0wer context, setup some options, load fonts
     vsonyp0wer::CreateContext();
     vsonyp0werIO& io = vsonyp0wer::GetIO();
     // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableKeyboard' to enable keyboard controls.
     // TODO: Fill optional fields of the io structure later.
     // TODO: Load TTF/OTF fonts if you don't want to use the default font.

     // Initialize helper Platform and Renderer bindings (here we are using vsonyp0wer_impl_win32 and vsonyp0wer_impl_dx11)
     vsonyp0wer_ImplWin32_Init(hwnd);
     vsonyp0wer_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

     // Application main loop
     while (true)
     {
         // Feed inputs to dear vsonyp0wer, start new frame
         vsonyp0wer_ImplDX11_NewFrame();
         vsonyp0wer_ImplWin32_NewFrame();
         vsonyp0wer::NewFrame();

         // Any application code here
         vsonyp0wer::Text("Hello, world!");

         // Render dear vsonyp0wer into screen
         vsonyp0wer::Render();
         vsonyp0wer_ImplDX11_RenderDrawData(vsonyp0wer::GetDrawData());
         g_pSwapChain->Present(1, 0);
     }

     // Shutdown
     vsonyp0wer_ImplDX11_Shutdown();
     vsonyp0wer_ImplWin32_Shutdown();
     vsonyp0wer::DestroyContext();

 HOW A SIMPLE APPLICATION MAY LOOK LIKE:
 EXHIBIT 2: IMPLEMENTING CUSTOM BINDING / CUSTOM ENGINE.

     // Application init: create a dear vsonyp0wer context, setup some options, load fonts
     vsonyp0wer::CreateContext();
     vsonyp0werIO& io = vsonyp0wer::GetIO();
     // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableKeyboard' to enable keyboard controls.
     // TODO: Fill optional fields of the io structure later.
     // TODO: Load TTF/OTF fonts if you don't want to use the default font.

     // Build and load the texture atlas into a texture
     // (In the examples/ app this is usually done within the vsonyp0wer_ImplXXX_Init() function from one of the demo Renderer)
     int width, height;
     unsigned char* pixels = NULL;
     io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

     // At this point you've got the texture data and you need to upload that your your graphic system:
     // After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
     // This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ below for details about ImTextureID.
     MyTexture* texture = MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA32)
     io.Fonts->TexID = (void*)texture;

     // Application main loop
     while (true)
     {
        // Setup low-level inputs, e.g. on Win32: calling GetKeyboardState(), or write to those fields from your Windows message handlers, etc.
        // (In the examples/ app this is usually done within the vsonyp0wer_ImplXXX_NewFrame() function from one of the demo Platform bindings)
        io.DeltaTime = 1.0f/60.0f;              // set the time elapsed since the previous frame (in seconds)
        io.DisplaySize.x = 1920.0f;             // set the current display width
        io.DisplaySize.y = 1280.0f;             // set the current display height here
        io.MousePos = my_mouse_pos;             // set the mouse position
        io.MouseDown[0] = my_mouse_buttons[0];  // set the mouse button states
        io.MouseDown[1] = my_mouse_buttons[1];

        // Call NewFrame(), after this point you can use vsonyp0wer::* functions anytime
        // (So you want to try calling NewFrame() as early as you can in your mainloop to be able to use vsonyp0wer everywhere)
        vsonyp0wer::NewFrame();

        // Most of your application code here
        vsonyp0wer::Text("Hello, world!");
        MyGameUpdate(); // may use any vsonyp0wer functions, e.g. vsonyp0wer::Begin("My window"); vsonyp0wer::Text("Hello, world!"); vsonyp0wer::End();
        MyGameRender(); // may use any vsonyp0wer functions as well!

        // Render vsonyp0wer, swap buffers
        // (You want to try calling EndFrame/Render as late as you can, to be able to use vsonyp0wer in your own game rendering code)
        vsonyp0wer::EndFrame();
        vsonyp0wer::Render();
        ImDrawData* draw_data = vsonyp0wer::GetDrawData();
        Myvsonyp0werRenderFunction(draw_data);
        SwapBuffers();
     }

     // Shutdown
     vsonyp0wer::DestroyContext();

 HOW A SIMPLE RENDERING FUNCTION MAY LOOK LIKE:

    void void Myvsonyp0werRenderFunction(ImDrawData* draw_data)
    {
       // TODO: Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
       // TODO: Setup viewport covering draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
       // TODO: Setup orthographic projection matrix cover draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize
       // TODO: Setup shader: vertex { float2 pos, float2 uv, u32 color }, fragment shader sample color from 1 texture, multiply by vertex color.
       for (int n = 0; n < draw_data->CmdListsCount; n++)
       {
          const ImDrawList* cmd_list = draw_data->CmdLists[n];
          const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;  // vertex buffer generated by vsonyp0wer
          const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;   // index buffer generated by vsonyp0wer
          for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
          {
             const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
             if (pcmd->UserCallback)
             {
                 pcmd->UserCallback(cmd_list, pcmd);
             }
             else
             {
                 // The texture for the draw call is specified by pcmd->TextureId.
                 // The vast majority of draw calls will use the vsonyp0wer texture atlas, which value you have set yourself during initialization.
                 MyEngineBindTexture((MyTexture*)pcmd->TextureId);

                 // We are using scissoring to clip some objects. All low-level graphics API should supports it.
                 // - If your engine doesn't support scissoring yet, you may ignore this at first. You will get some small glitches
                 //   (some elements visible outside their bounds) but you can fix that once everything else works!
                 // - Clipping coordinates are provided in vsonyp0wer coordinates space (from draw_data->DisplayPos to draw_data->DisplayPos + draw_data->DisplaySize)
                 //   In a single viewport application, draw_data->DisplayPos will always be (0,0) and draw_data->DisplaySize will always be == io.DisplaySize.
                 //   However, in the interest of supporting multi-viewport applications in the future (see 'viewport' branch on github),
                 //   always subtract draw_data->DisplayPos from clipping bounds to convert them to your viewport space.
                 // - Note that pcmd->ClipRect contains Min+Max bounds. Some graphics API may use Min+Max, other may use Min+Size (size being Max-Min)
                 ImVec2 pos = draw_data->DisplayPos;
                 MyEngineScissor((int)(pcmd->ClipRect.x - pos.x), (int)(pcmd->ClipRect.y - pos.y), (int)(pcmd->ClipRect.z - pos.x), (int)(pcmd->ClipRect.w - pos.y));

                 // Render 'pcmd->ElemCount/3' indexed triangles.
                 // By default the indices ImDrawIdx are 16-bits, you can change them to 32-bits in imconfig.h if your engine doesn't support 16-bits indices.
                 MyEngineDrawIndexedTriangles(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer, vtx_buffer);
             }
             idx_buffer += pcmd->ElemCount;
          }
       }
    }

 - The examples/ folders contains many actual implementation of the pseudo-codes above.
 - When calling NewFrame(), the 'io.WantCaptureMouse', 'io.WantCaptureKeyboard' and 'io.WantTextInput' flags are updated.
   They tell you if Dear vsonyp0wer intends to use your inputs. When a flag is set you want to hide the corresponding inputs
   from the rest of your application. In every cases you need to pass on the inputs to vsonyp0wer. Refer to the FAQ for more information.
 - Please read the FAQ below!. Amusingly, it is called a FAQ because people frequently run into the same issues!

 USING GAMEPAD/KEYBOARD NAVIGATION CONTROLS

 - The gamepad/keyboard navigation is fairly functional and keeps being improved.
 - Gamepad support is particularly useful to use dear vsonyp0wer on a console system (e.g. PS4, Switch, XB1) without a mouse!
 - You can ask questions and report issues at https://github.com/ocornut/vsonyp0wer/issues/787
 - The initial focus was to support game controllers, but keyboard is becoming increasingly and decently usable.
 - Gamepad:
    - Set io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableGamepad to enable.
    - Backend: Set io.BackendFlags |= vsonyp0werBackendFlags_HasGamepad + fill the io.NavInputs[] fields before calling NewFrame().
      Note that io.NavInputs[] is cleared by EndFrame().
    - See 'enum vsonyp0werNavInput_' in vsonyp0wer.h for a description of inputs. For each entry of io.NavInputs[], set the following values:
         0.0f= not held. 1.0f= fully held. Pass intermediate 0.0f..1.0f values for analog triggers/sticks.
    - We uses a simple >0.0f test for activation testing, and won't attempt to test for a dead-zone.
      Your code will probably need to transform your raw inputs (such as e.g. remapping your 0.2..0.9 raw input range to 0.0..1.0 vsonyp0wer range, etc.).
    - You can download PNG/PSD files depicting the gamepad controls for common controllers at: http://goo.gl/9LgVZW.
    - If you need to share inputs between your game and the vsonyp0wer parts, the easiest approach is to go all-or-nothing, with a buttons combo
      to toggle the target. Please reach out if you think the game vs navigation input sharing could be improved.
 - Keyboard:
    - Set io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableKeyboard to enable.
      NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
    - When keyboard navigation is active (io.NavActive + vsonyp0werConfigFlags_NavEnableKeyboard), the io.WantCaptureKeyboard flag
      will be set. For more advanced uses, you may want to read from:
       - io.NavActive: true when a window is focused and it doesn't have the vsonyp0werWindowFlags_NoNavInputs flag set.
       - io.NavVisible: true when the navigation cursor is visible (and usually goes false when mouse is used).
       - or query focus information with e.g. IsWindowFocused(vsonyp0werFocusedFlags_AnyWindow), IsItemFocused() etc. functions.
      Please reach out if you think the game vs navigation input sharing could be improved.
 - Mouse:
    - PS4 users: Consider emulating a mouse cursor with DualShock4 touch pad or a spare analog stick as a mouse-emulation fallback.
    - Consoles/Tablet/Phone users: Consider using a Synergy 1.x server (on your PC) + uSynergy.c (on your console/tablet/phone app) to share your PC mouse/keyboard.
    - On a TV/console system where readability may be lower or mouse inputs may be awkward, you may want to set the vsonyp0werConfigFlags_NavEnableSetMousePos flag.
      Enabling vsonyp0werConfigFlags_NavEnableSetMousePos + vsonyp0werBackendFlags_HasSetMousePos instructs dear vsonyp0wer to move your mouse cursor along with navigation movements.
      When enabled, the NewFrame() function may alter 'io.MousePos' and set 'io.WantSetMousePos' to notify you that it wants the mouse cursor to be moved.
      When that happens your back-end NEEDS to move the OS or underlying mouse cursor on the next frame. Some of the binding in examples/ do that.
      (If you set the NavEnableSetMousePos flag but don't honor 'io.WantSetMousePos' properly, vsonyp0wer will misbehave as it will see your mouse as moving back and forth!)
      (In a setup when you may not have easy control over the mouse cursor, e.g. uSynergy.c doesn't expose moving remote mouse cursor, you may want
       to set a boolean to ignore your other external mouse positions until the external source is moved again.)


 API BREAKING CHANGES
 ====================

 Occasionally introducing changes that are breaking the API. We try to make the breakage minor and easy to fix.
 Below is a change-log of API breaking changes only. If you are using one of the functions listed, expect to have to fix some code.
 When you are not sure about a old symbol or function name, try using the Search/Find function of your IDE to look for comments or references in all vsonyp0wer files.
 You can read releases logs https://github.com/ocornut/vsonyp0wer/releases for more details.

 - 2019/03/04 (1.69) - renamed GetOverlayDrawList() to GetForegroundDrawList(). Kept redirection function (will obsolete).
 - 2019/02/26 (1.69) - renamed vsonyp0werColorEditFlags_RGB/vsonyp0werColorEditFlags_HSV/vsonyp0werColorEditFlags_HEX to vsonyp0werColorEditFlags_DisplayRGB/vsonyp0werColorEditFlags_DisplayHSV/vsonyp0werColorEditFlags_DisplayHex. Kept redirection enums (will obsolete).
 - 2019/02/14 (1.68) - made it illegal/assert when io.DisplayTime == 0.0f (with an exception for the first frame). If for some reason your time step calculation gives you a zero value, replace it with a dummy small value!
 - 2019/02/01 (1.68) - removed io.DisplayVisibleMin/DisplayVisibleMax (which were marked obsolete and removed from viewport/docking branch already).
 - 2019/01/06 (1.67) - renamed io.InputCharacters[], marked internal as was always intended. Please don't access directly, and use AddInputCharacter() instead!
 - 2019/01/06 (1.67) - renamed ImFontAtlas::GlyphRangesBuilder to ImFontGlyphRangesBuilder. Keep redirection typedef (will obsolete).
 - 2018/12/20 (1.67) - made it illegal to call Begin("") with an empty string. This somehow half-worked before but had various undesirable side-effects.
 - 2018/12/10 (1.67) - renamed io.ConfigResizeWindowsFromEdges to io.ConfigWindowsResizeFromEdges as we are doing a large pass on configuration flags.
 - 2018/10/12 (1.66) - renamed misc/stl/vsonyp0wer_stl.* to misc/cpp/vsonyp0wer_stdlib.* in prevision for other C++ helper files.
 - 2018/09/28 (1.66) - renamed SetScrollHere() to SetScrollHereY(). Kept redirection function (will obsolete).
 - 2018/09/06 (1.65) - renamed stb_truetype.h to vsdns_truetype.h, stb_textedit.h to vsdns_textedit.h, and stb_rect_pack.h to vsdns_rectpack.h.
                       If you were conveniently using the vsonyp0wer copy of those STB headers in your project you will have to update your include paths.
 - 2018/09/05 (1.65) - renamed io.OptCursorBlink/io.ConfigCursorBlink to io.ConfigInputTextCursorBlink. (#1427)
 - 2018/08/31 (1.64) - added vsonyp0wer_widgets.cpp file, extracted and moved widgets code out of vsonyp0wer.cpp into vsonyp0wer_widgets.cpp. Re-ordered some of the code remaining in vsonyp0wer.cpp.
                       NONE OF THE FUNCTIONS HAVE CHANGED. THE CODE IS SEMANTICALLY 100% IDENTICAL, BUT _EVERY_ FUNCTION HAS BEEN MOVED.
                       Because of this, any local modifications to vsonyp0wer.cpp will likely conflict when you update. Read docs/CHANGELOG.txt for suggestions.
 - 2018/08/22 (1.63) - renamed IsItemDeactivatedAfterChange() to IsItemDeactivatedAfterEdit() for consistency with new IsItemEdited() API. Kept redirection function (will obsolete soonish as IsItemDeactivatedAfterChange() is very recent).
 - 2018/08/21 (1.63) - renamed vsonyp0werTextEditCallback to vsonyp0werInputTextCallback, vsonyp0werTextEditCallbackData to vsonyp0werInputTextCallbackData for consistency. Kept redirection types (will obsolete).
 - 2018/08/21 (1.63) - removed vsonyp0werInputTextCallbackData::ReadOnly since it is a duplication of (vsonyp0werInputTextCallbackData::Flags & vsonyp0werInputTextFlags_ReadOnly).
 - 2018/08/01 (1.63) - removed per-window vsonyp0werWindowFlags_ResizeFromAnySide beta flag in favor of a global io.ConfigResizeWindowsFromEdges [update 1.67 renamed to ConfigWindowsResizeFromEdges] to enable the feature.
 - 2018/08/01 (1.63) - renamed io.OptCursorBlink to io.ConfigCursorBlink [-> io.ConfigInputTextCursorBlink in 1.65], io.OptMacOSXBehaviors to ConfigMacOSXBehaviors for consistency.
 - 2018/07/22 (1.63) - changed vsonyp0wer::GetTime() return value from float to double to avoid accumulating floating point imprecisions over time.
 - 2018/07/08 (1.63) - style: renamed vsonyp0werCol_ModalWindowDarkening to vsonyp0werCol_ModalWindowDimBg for consistency with other features. Kept redirection enum (will obsolete).
 - 2018/06/08 (1.62) - examples: the vsonyp0wer_impl_xxx files have been split to separate platform (Win32, Glfw, SDL2, etc.) from renderer (DX11, OpenGL, Vulkan,  etc.).
                       old binding will still work as is, however prefer using the separated bindings as they will be updated to be multi-viewport conformant.
                       when adopting new bindings follow the main.cpp code of your preferred examples/ folder to know which functions to call.
 - 2018/06/06 (1.62) - renamed GetGlyphRangesChinese() to GetGlyphRangesChineseFull() to distinguish other variants and discouhnly using the full set.
 - 2018/06/06 (1.62) - TreeNodeEx()/TreeNodeBehavior(): the vsonyp0werTreeNodeFlags_CollapsingHeader helper now include the vsonyp0werTreeNodeFlags_NoTreePushOnOpen flag. See Changelog for details.
 - 2018/05/03 (1.61) - DragInt(): the default compile-time format string has been changed from "%.0f" to "%d", as we are not using integers internally any more.
                       If you used DragInt() with custom format strings, make sure you change them to use %d or an integer-compatible format.
                       To honor backward-compatibility, the DragInt() code will currently parse and modify format strings to replace %*f with %d, giving time to users to upgrade their code.
                       If you have vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS enabled, the code will instead assert! You may run a reg-exp search on your codebase for e.g. "DragInt.*%f" to help you find them.
 - 2018/04/28 (1.61) - obsoleted InputFloat() functions taking an optional "int decimal_precision" in favor of an equivalent and more flexible "const char* format",
                       consistent with other functions. Kept redirection functions (will obsolete).
 - 2018/04/09 (1.61) - IM_DELETE() helper function added in 1.60 doesn't clear the input _pointer_ reference, more consistent with expectation and allows passing r-value.
 - 2018/03/20 (1.60) - renamed io.WantMoveMouse to io.WantSetMousePos for consistency and ease of understanding (was added in 1.52, _not_ used by core and only honored by some binding ahead of merging the Nav branch).
 - 2018/03/12 (1.60) - removed vsonyp0werCol_CloseButton, vsonyp0werCol_CloseButtonActive, vsonyp0werCol_CloseButtonHovered as the closing cross uses regular button colors now.
 - 2018/03/08 (1.60) - changed ImFont::DisplayOffset.y to default to 0 instead of +1. Fixed rounding of Ascent/Descent to match TrueType renderer. If you were adding or subtracting to ImFont::DisplayOffset check if your fonts are correctly aligned vertically.
 - 2018/03/03 (1.60) - renamed vsonyp0werStyleVar_Count_ to vsonyp0werStyleVar_COUNT and vsonyp0werMouseCursor_Count_ to vsonyp0werMouseCursor_COUNT for consistency with other public enums.
 - 2018/02/18 (1.60) - BeginDragDropSource(): temporarily removed the optional mouse_button=0 parameter because it is not really usable in many situations at the moment.
 - 2018/02/16 (1.60) - obsoleted the io.RenderDrawListsFn callback, you can call your graphics engine render function after vsonyp0wer::Render(). Use vsonyp0wer::GetDrawData() to retrieve the ImDrawData* to display.
 - 2018/02/07 (1.60) - reorganized context handling to be more explicit,
                       - YOU NOW NEED TO CALL vsonyp0wer::CreateContext() AT THE BEGINNING OF YOUR APP, AND CALL vsonyp0wer::DestroyContext() AT THE END.
                       - removed Shutdown() function, as DestroyContext() serve this purpose.
                       - you may pass a ImFontAtlas* pointer to CreateContext() to share a font atlas between contexts. Otherwise CreateContext() will create its own font atlas instance.
                       - removed allocator parameters from CreateContext(), they are now setup with SetAllocatorFunctions(), and shared by all contexts.
                       - removed the default global context and font atlas instance, which were confusing for users of DLL reloading and users of multiple contexts.
 - 2018/01/31 (1.60) - moved sample TTF files from extra_fonts/ to misc/fonts/. If you loaded files directly from the vsonyp0wer repo you may need to update your paths.
 - 2018/01/11 (1.60) - obsoleted IsAnyWindowHovered() in favor of IsWindowHovered(vsonyp0werHoveredFlags_AnyWindow). Kept redirection function (will obsolete).
 - 2018/01/11 (1.60) - obsoleted IsAnyWindowFocused() in favor of IsWindowFocused(vsonyp0werFocusedFlags_AnyWindow). Kept redirection function (will obsolete).
 - 2018/01/03 (1.60) - renamed vsonyp0werSizeConstraintCallback to vsonyp0werSizeCallback, vsonyp0werSizeConstraintCallbackData to vsonyp0werSizeCallbackData.
 - 2017/12/29 (1.60) - removed CalcItemRectClosestPoint() which was weird and not really used by anyone except demo code. If you need it it's easy to replicate on your side.
 - 2017/12/24 (1.53) - renamed the emblematic ShowTestWindow() function to ShowDemoWindow(). Kept redirection function (will obsolete).
 - 2017/12/21 (1.53) - ImDrawList: renamed style.AntiAliasedShapes to style.AntiAliasedFill for consistency and as a way to explicitly break code that manipulate those flag at runtime. You can now manipulate ImDrawList::Flags
 - 2017/12/21 (1.53) - ImDrawList: removed 'bool anti_aliased = true' final parameter of ImDrawList::AddPolyline() and ImDrawList::AddConvexPolyFilled(). Prefer manipulating ImDrawList::Flags if you need to toggle them during the frame.
 - 2017/12/14 (1.53) - using the vsonyp0werWindowFlags_NoScrollWithMouse flag on a child window forwards the mouse wheel event to the parent window, unless either vsonyp0werWindowFlags_NoInputs or vsonyp0werWindowFlags_NoScrollbar are also set.
 - 2017/12/13 (1.53) - renamed GetItemsLineHeightWithSpacing() to GetFrameHeightWithSpacing(). Kept redirection function (will obsolete).
 - 2017/12/13 (1.53) - obsoleted IsRootWindowFocused() in favor of using IsWindowFocused(vsonyp0werFocusedFlags_RootWindow). Kept redirection function (will obsolete).
                     - obsoleted IsRootWindowOrAnyChildFocused() in favor of using IsWindowFocused(vsonyp0werFocusedFlags_RootAndChildWindows). Kept redirection function (will obsolete).
 - 2017/12/12 (1.53) - renamed vsonyp0werTreeNodeFlags_AllowOverlapMode to vsonyp0werTreeNodeFlags_AllowItemOverlap. Kept redirection enum (will obsolete).
 - 2017/12/10 (1.53) - removed SetNextWindowContentWidth(), prefer using SetNextWindowContentSize(). Kept redirection function (will obsolete).
 - 2017/11/27 (1.53) - renamed vsonyp0werTextBuffer::append() helper to appendf(), appendv() to appendfv(). If you copied the 'Log' demo in your code, it uses appendv() so that needs to be renamed.
 - 2017/11/18 (1.53) - Style, Begin: removed vsonyp0werWindowFlags_ShowBorders window flag. Borders are now fully set up in the vsonyp0werStyle structure (see e.g. style.FrameBorderSize, style.WindowBorderSize). Use vsonyp0wer::ShowStyleEditor() to look them up.
                       Please note that the style system will keep evolving (hopefully stabilizing in Q1 2018), and so custom styles will probably subtly break over time. It is recommended you use the StyleColorsClassic(), StyleColorsDark(), StyleColorsLight() functions.
 - 2017/11/18 (1.53) - Style: removed vsonyp0werCol_ComboBg in favor of combo boxes using vsonyp0werCol_PopupBg for consistency.
 - 2017/11/18 (1.53) - Style: renamed vsonyp0werCol_ChildWindowBg to vsonyp0werCol_ChildBg.
 - 2017/11/18 (1.53) - Style: renamed style.ChildWindowRounding to style.ChildRounding, vsonyp0werStyleVar_ChildWindowRounding to vsonyp0werStyleVar_ChildRounding.
 - 2017/11/02 (1.53) - obsoleted IsRootWindowOrAnyChildHovered() in favor of using IsWindowHovered(vsonyp0werHoveredFlags_RootAndChildWindows);
 - 2017/10/24 (1.52) - renamed vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCS/vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCS to vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS/vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS for consistency.
 - 2017/10/20 (1.52) - changed IsWindowHovered() default parameters behavior to return false if an item is active in another window (e.g. click-dragging item from another window to this window). You can use the newly introduced IsWindowHovered() flags to requests this specific behavior if you need it.
 - 2017/10/20 (1.52) - marked IsItemHoveredRect()/IsMouseHoveringWindow() as obsolete, in favor of using the newly introduced flags for IsItemHovered() and IsWindowHovered(). See https://github.com/ocornut/vsonyp0wer/issues/1382 for details.
                       removed the IsItemRectHovered()/IsWindowRectHovered() names introduced in 1.51 since they were merely more consistent names for the two functions we are now obsoleting.
 - 2017/10/17 (1.52) - marked the old 5-parameters version of Begin() as obsolete (still available). Use SetNextWindowSize()+Begin() instead!
 - 2017/10/11 (1.52) - renamed AlignFirstTextHeightToWidgets() to AlignTextToFramePadding(). Kept inline redirection function (will obsolete).
 - 2017/09/26 (1.52) - renamed ImFont::Glyph to ImFontGlyph. Keep redirection typedef (will obsolete).
 - 2017/09/25 (1.52) - removed SetNextWindowPosCenter() because SetNextWindowPos() now has the optional pivot information to do the same and more. Kept redirection function (will obsolete).
 - 2017/08/25 (1.52) - io.MousePos needs to be set to ImVec2(-FLT_MAX,-FLT_MAX) when mouse is unavailable/missing. Previously ImVec2(-1,-1) was enough but we now accept negative mouse coordinates. In your binding if you need to support unavailable mouse, make sure to replace "io.MousePos = ImVec2(-1,-1)" with "io.MousePos = ImVec2(-FLT_MAX,-FLT_MAX)".
 - 2017/08/22 (1.51) - renamed IsItemHoveredRect() to IsItemRectHovered(). Kept inline redirection function (will obsolete). -> (1.52) use IsItemHovered(vsonyp0werHoveredFlags_RectOnly)!
                     - renamed IsMouseHoveringAnyWindow() to IsAnyWindowHovered() for consistency. Kept inline redirection function (will obsolete).
                     - renamed IsMouseHoveringWindow() to IsWindowRectHovered() for consistency. Kept inline redirection function (will obsolete).
 - 2017/08/20 (1.51) - renamed GetStyleColName() to GetStyleColorName() for consistency.
 - 2017/08/20 (1.51) - added PushStyleColor(vsonyp0werCol idx, ImU32 col) overload, which _might_ cause an "ambiguous call" compilation error if you are using ImColor() with implicit cast. Cast to ImU32 or ImVec4 explicily to fix.
 - 2017/08/15 (1.51) - marked the weird vsonyp0wer_ONCE_UPON_A_FRAME helper macro as obsolete. prefer using the more explicit vsonyp0werOnceUponAFrame.
 - 2017/08/15 (1.51) - changed parameter order for BeginPopupContextWindow() from (const char*,int buttons,bool also_over_items) to (const char*,int buttons,bool also_over_items). Note that most calls relied on default parameters completely.
 - 2017/08/13 (1.51) - renamed vsonyp0werCol_Columns*** to vsonyp0werCol_Separator***. Kept redirection enums (will obsolete).
 - 2017/08/11 (1.51) - renamed vsonyp0werSetCond_*** types and flags to vsonyp0werCond_***. Kept redirection enums (will obsolete).
 - 2017/08/09 (1.51) - removed ValueColor() helpers, they are equivalent to calling Text(label) + SameLine() + ColorButton().
 - 2017/08/08 (1.51) - removed ColorEditMode() and vsonyp0werColorEditMode in favor of vsonyp0werColorEditFlags and parameters to the various Color*() functions. The SetColorEditOptions() allows to initialize default but the user can still change them with right-click context menu.
                     - changed prototype of 'ColorEdit4(const char* label, float col[4], bool show_alpha = true)' to 'ColorEdit4(const char* label, float col[4], vsonyp0werColorEditFlags flags = 0)', where passing flags = 0x01 is a safe no-op (hello dodgy backward compatibility!). - check and run the demo window, under "Color/Picker Widgets", to understand the various new options.
                     - changed prototype of rarely used 'ColorButton(ImVec4 col, bool small_height = false, bool outline_border = true)' to 'ColorButton(const char* desc_id, ImVec4 col, vsonyp0werColorEditFlags flags = 0, ImVec2 size = ImVec2(0,0))'
 - 2017/07/20 (1.51) - removed IsPosHoveringAnyWindow(ImVec2), which was partly broken and misleading. ASSERT + redirect user to io.WantCaptureMouse
 - 2017/05/26 (1.50) - removed ImFontConfig::MergeGlyphCenterV in favor of a more multipurpose ImFontConfig::GlyphOffset.
 - 2017/05/01 (1.50) - renamed ImDrawList::PathFill() (rarely used directly) to ImDrawList::PathFillConvex() for clarity.
 - 2016/11/06 (1.50) - BeginChild(const char*) now applies the stack id to the provided label, consistently with other functions as it should always have been. It shouldn't affect you unless (extremely unlikely) you were appending multiple times to a same child from different locations of the stack id. If that's the case, generate an id with GetId() and use it instead of passing string to BeginChild().
 - 2016/10/15 (1.50) - avoid 'void* user_data' parameter to io.SetClipboardTextFn/io.GetClipboardTextFn pointers. We pass io.ClipboardUserData to it.
 - 2016/09/25 (1.50) - style.WindowTitleAlign is now a ImVec2 (vsonyp0werAlign enum was removed). set to (0.5f,0.5f) for horizontal+vertical centering, (0.0f,0.0f) for upper-left, etc.
 - 2016/07/30 (1.50) - SameLine(x) with x>0.0f is now relative to left of column/group if any, and not always to left of window. This was sort of always the intent and hopefully breakage should be minimal.
 - 2016/05/12 (1.49) - title bar (using vsonyp0werCol_TitleBg/vsonyp0werCol_TitleBgActive colors) isn't rendered over a window background (vsonyp0werCol_WindowBg color) anymore.
                       If your TitleBg/TitleBgActive alpha was 1.0f or you are using the default theme it will not affect you.
                       If your TitleBg/TitleBgActive alpha was <1.0f you need to tweak your custom theme to readjust for the fact that we don't draw a WindowBg background behind the title bar.
                       This helper function will convert an old TitleBg/TitleBgActive color into a new one with the same visual output, given the OLD color and the OLD WindowBg color.
                           ImVec4 ConvertTitleBgCol(const ImVec4& win_bg_col, const ImVec4& title_bg_col)
                           {
                               float new_a = 1.0f - ((1.0f - win_bg_col.w) * (1.0f - title_bg_col.w)), k = title_bg_col.w / new_a;
                               return ImVec4((win_bg_col.x * win_bg_col.w + title_bg_col.x) * k, (win_bg_col.y * win_bg_col.w + title_bg_col.y) * k, (win_bg_col.z * win_bg_col.w + title_bg_col.z) * k, new_a);
                           }
                       If this is confusing, pick the RGB value from title bar from an old screenshot and apply this as TitleBg/TitleBgActive. Or you may just create TitleBgActive from a tweaked TitleBg color.
 - 2016/05/07 (1.49) - removed confusing set of GetInternalState(), GetInternalStateSize(), SetInternalState() functions. Now using CreateContext(), DestroyContext(), GetCurrentContext(), SetCurrentContext().
 - 2016/05/02 (1.49) - renamed SetNextTreeNodeOpened() to SetNextTreeNodeOpen(), no redirection.
 - 2016/05/01 (1.49) - obsoleted old signature of CollapsingHeader(const char* label, const char* str_id = NULL, bool display_frame = true, bool default_open = false) as extra parameters were badly designed and rarely used. You can replace the "default_open = true" flag in new API with CollapsingHeader(label, vsonyp0werTreeNodeFlags_DefaultOpen).
 - 2016/04/26 (1.49) - changed ImDrawList::PushClipRect(ImVec4 rect) to ImDrawList::PushClipRect(Imvec2 min,ImVec2 max,bool intersect_with_current_clip_rect=false). Note that higher-level vsonyp0wer::PushClipRect() is preferable because it will clip at logic/widget level, whereas ImDrawList::PushClipRect() only affect your renderer.
 - 2016/04/03 (1.48) - removed style.WindowFillAlphaDefault setting which was redundant. Bake default BG alpha inside style.Colors[vsonyp0werCol_WindowBg] and all other Bg color values. (ref github issue #337).
 - 2016/04/03 (1.48) - renamed vsonyp0werCol_TooltipBg to vsonyp0werCol_PopupBg, used by popups/menus and tooltips. popups/menus were previously using vsonyp0werCol_WindowBg. (ref github issue #337)
 - 2016/03/21 (1.48) - renamed GetWindowFont() to GetFont(), GetWindowFontSize() to GetFontSize(). Kept inline redirection function (will obsolete).
 - 2016/03/02 (1.48) - InputText() completion/history/always callbacks: if you modify the text buffer manually (without using DeleteChars()/InsertChars() helper) you need to maintain the BufTextLen field. added an assert.
 - 2016/01/23 (1.48) - fixed not honoring exact width passed to PushItemWidth(), previously it would add extra FramePadding.x*2 over that width. if you had manual pixel-perfect alignment in place it might affect you.
 - 2015/12/27 (1.48) - fixed ImDrawList::AddRect() which used to render a rectangle 1 px too large on each axis.
 - 2015/12/04 (1.47) - renamed Color() helpers to ValueColor() - dangerously named, rarely used and probably to be made obsolete.
 - 2015/08/29 (1.45) - with the addition of horizontal scrollbar we made various fixes to inconsistencies with dealing with cursor position.
                       GetCursorPos()/SetCursorPos() functions now include the scrolled amount. It shouldn't affect the majority of users, but take note that SetCursorPosX(100.0f) puts you at +100 from the starting x position which may include scrolling, not at +100 from the window left side.
                       GetContentRegionMax()/GetWindowContentRegionMin()/GetWindowContentRegionMax() functions allow include the scrolled amount. Typically those were used in cases where no scrolling would happen so it may not be a problem, but watch out!
 - 2015/08/29 (1.45) - renamed style.ScrollbarWidth to style.ScrollbarSize
 - 2015/08/05 (1.44) - split vsonyp0wer.cpp into extra files: vsonyp0wer_demo.cpp vsonyp0wer_draw.cpp vsonyp0wer_internal.h that you need to add to your project.
 - 2015/07/18 (1.44) - fixed angles in ImDrawList::PathArcTo(), PathArcToFast() (introduced in 1.43) being off by an extra PI for no justifiable reason
 - 2015/07/14 (1.43) - add new ImFontAtlas::AddFont() API. For the old AddFont***, moved the 'font_no' parameter of ImFontAtlas::AddFont** functions to the ImFontConfig structure.
                       you need to render your textured triangles with bilinear filtering to benefit from sub-pixel positioning of text.
 - 2015/07/08 (1.43) - switched rendering data to use indexed rendering. this is saving a fair amount of CPU/GPU and enables us to get anti-aliasing for a marginal cost.
                       this necessary change will break your rendering function! the fix should be very easy. sorry for that :(
                     - if you are using a vanilla copy of one of the vsonyp0wer_impl_XXXX.cpp provided in the example, you just need to update your copy and you can ignore the rest.
                     - the signature of the io.RenderDrawListsFn handler has changed!
                       old: vsonyp0wer_XXXX_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
                       new: vsonyp0wer_XXXX_RenderDrawLists(ImDrawData* draw_data).
                         parameters: 'cmd_lists' becomes 'draw_data->CmdLists', 'cmd_lists_count' becomes 'draw_data->CmdListsCount'
                         ImDrawList: 'commands' becomes 'CmdBuffer', 'vtx_buffer' becomes 'VtxBuffer', 'IdxBuffer' is new.
                         ImDrawCmd:  'vtx_count' becomes 'ElemCount', 'clip_rect' becomes 'ClipRect', 'user_callback' becomes 'UserCallback', 'texture_id' becomes 'TextureId'.
                     - each ImDrawList now contains both a vertex buffer and an index buffer. For each command, render ElemCount/3 triangles using indices from the index buffer.
                     - if you REALLY cannot render indexed primitives, you can call the draw_data->DeIndexAllBuffers() method to de-index the buffers. This is slow and a waste of CPU/GPU. Prefer using indexed rendering!
                     - refer to code in the examples/ folder or ask on the GitHub if you are unsure of how to upgrade. please upgrade!
 - 2015/07/10 (1.43) - changed SameLine() parameters from int to float.
 - 2015/07/02 (1.42) - renamed SetScrollPosHere() to SetScrollFromCursorPos(). Kept inline redirection function (will obsolete).
 - 2015/07/02 (1.42) - renamed GetScrollPosY() to GetScrollY(). Necessary to reduce confusion along with other scrolling functions, because positions (e.g. cursor position) are not equivalent to scrolling amount.
 - 2015/06/14 (1.41) - changed ImageButton() default bg_col parameter from (0,0,0,1) (black) to (0,0,0,0) (transparent) - makes a difference when texture have transparence
 - 2015/06/14 (1.41) - changed Selectable() API from (label, selected, size) to (label, selected, flags, size). Size override should have been rarely be used. Sorry!
 - 2015/05/31 (1.40) - renamed GetWindowCollapsed() to IsWindowCollapsed() for consistency. Kept inline redirection function (will obsolete).
 - 2015/05/31 (1.40) - renamed IsRectClipped() to IsRectVisible() for consistency. Note that return value is opposite! Kept inline redirection function (will obsolete).
 - 2015/05/27 (1.40) - removed the third 'repeat_if_held' parameter from Button() - sorry! it was rarely used and inconsistent. Use PushButtonRepeat(true) / PopButtonRepeat() to enable repeat on desired buttons.
 - 2015/05/11 (1.40) - changed BeginPopup() API, takes a string identifier instead of a bool. vsonyp0wer needs to manage the open/closed state of popups. Call OpenPopup() to actually set the "open" state of a popup. BeginPopup() returns true if the popup is opened.
 - 2015/05/03 (1.40) - removed style.AutoFitPadding, using style.WindowPadding makes more sense (the default values were already the same).
 - 2015/04/13 (1.38) - renamed IsClipped() to IsRectClipped(). Kept inline redirection function until 1.50.
 - 2015/04/09 (1.38) - renamed ImDrawList::AddArc() to ImDrawList::AddArcFast() for compatibility with future API
 - 2015/04/03 (1.38) - removed vsonyp0werCol_CheckHovered, vsonyp0werCol_CheckActive, replaced with the more general vsonyp0werCol_FrameBgHovered, vsonyp0werCol_FrameBgActive.
 - 2014/04/03 (1.38) - removed support for passing -FLT_MAX..+FLT_MAX as the range for a SliderFloat(). Use DragFloat() or Inputfloat() instead.
 - 2015/03/17 (1.36) - renamed GetItemBoxMin()/GetItemBoxMax()/IsMouseHoveringBox() to GetItemRectMin()/GetItemRectMax()/IsMouseHoveringRect(). Kept inline redirection function until 1.50.
 - 2015/03/15 (1.36) - renamed style.TreeNodeSpacing to style.IndentSpacing, vsonyp0werStyleVar_TreeNodeSpacing to vsonyp0werStyleVar_IndentSpacing
 - 2015/03/13 (1.36) - renamed GetWindowIsFocused() to IsWindowFocused(). Kept inline redirection function until 1.50.
 - 2015/03/08 (1.35) - renamed style.ScrollBarWidth to style.ScrollbarWidth (casing)
 - 2015/02/27 (1.34) - renamed OpenNextNode(bool) to SetNextTreeNodeOpened(bool, vsonyp0werSetCond). Kept inline redirection function until 1.50.
 - 2015/02/27 (1.34) - renamed vsonyp0werSetCondition_*** to vsonyp0werSetCond_***, and _FirstUseThisSession becomes _Once.
 - 2015/02/11 (1.32) - changed text input callback vsonyp0werTextEditCallback return type from void-->int. reserved for future use, return 0 for now.
 - 2015/02/10 (1.32) - renamed GetItemWidth() to CalcItemWidth() to clarify its evolving behavior
 - 2015/02/08 (1.31) - renamed GetTextLineSpacing() to GetTextLineHeightWithSpacing()
 - 2015/02/01 (1.31) - removed IO.MemReallocFn (unused)
 - 2015/01/19 (1.30) - renamed vsonyp0werStohnly::GetIntPtr()/GetFloatPtr() to GetIntRef()/GetIntRef() because Ptr was conflicting with actual pointer stohnly functions.
 - 2015/01/11 (1.30) - big font/image API change! now loads TTF file. allow for multiple fonts. no need for a PNG loader.
              (1.30) - removed GetDefaultFontData(). uses io.Fonts->GetTextureData*() API to retrieve uncompressed pixels.
                       font init:  { const void* png_data; unsigned int png_size; vsonyp0wer::GetDefaultFontData(NULL, NULL, &png_data, &png_size); <..Upload texture to GPU..>; }
                       became:     { unsigned char* pixels; int width, height; io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height); <..Upload texture to GPU>; io.Fonts->TexId = YourTextureIdentifier; }
                       you now have more flexibility to load multiple TTF fonts and manage the texture buffer for internal needs.
                       it is now recommended that you sample the font texture with bilinear interpolation.
              (1.30) - added texture identifier in ImDrawCmd passed to your render function (we can now render images). make sure to set io.Fonts->TexID.
              (1.30) - removed IO.PixelCenterOffset (unnecessary, can be handled in user projection matrix)
              (1.30) - removed vsonyp0wer::IsItemFocused() in favor of vsonyp0wer::IsItemActive() which handles all widgets
 - 2014/12/10 (1.18) - removed SetNewWindowDefaultPos() in favor of new generic API SetNextWindowPos(pos, vsonyp0werSetCondition_FirstUseEver)
 - 2014/11/28 (1.17) - moved IO.Font*** options to inside the IO.Font-> structure (FontYOffset, FontTexUvForWhite, FontBaseScale, FontFallbackGlyph)
 - 2014/11/26 (1.17) - reworked syntax of vsonyp0wer_ONCE_UPON_A_FRAME helper macro to increase compiler compatibility
 - 2014/11/07 (1.15) - renamed IsHovered() to IsItemHovered()
 - 2014/10/02 (1.14) - renamed vsonyp0wer_INCLUDE_vsonyp0wer_USER_CPP to vsonyp0wer_INCLUDE_vsonyp0wer_USER_INL and vsonyp0wer_user.cpp to vsonyp0wer_user.inl (more IDE friendly)
 - 2014/09/25 (1.13) - removed 'text_end' parameter from IO.SetClipboardTextFn (the string is now always zero-terminated for simplicity)
 - 2014/09/24 (1.12) - renamed SetFontScale() to SetWindowFontScale()
 - 2014/09/24 (1.12) - moved IM_MALLOC/IM_REALLOC/IM_FREE preprocessor defines to IO.MemAllocFn/IO.MemReallocFn/IO.MemFreeFn
 - 2014/08/30 (1.09) - removed IO.FontHeight (now computed automatically)
 - 2014/08/30 (1.09) - moved vsonyp0wer_FONT_TEX_UV_FOR_WHITE preprocessor define to IO.FontTexUvForWhite
 - 2014/08/28 (1.09) - changed the behavior of IO.PixelCenterOffset following various rendering fixes


 FREQUENTLY ASKED QUESTIONS (FAQ), TIPS
 ======================================

 Q: Where is the documentation?
 A: This library is poorly documented at the moment and expects of the user to be acquainted with C/C++.
    - Run the examples/ and explore them.
    - See demo code in vsonyp0wer_demo.cpp and particularly the vsonyp0wer::ShowDemoWindow() function.
    - The demo covers most features of Dear vsonyp0wer, so you can read the code and see its output.
    - See documentation and comments at the top of vsonyp0wer.cpp + effectively vsonyp0wer.h.
    - Dozens of standalone example applications using e.g. OpenGL/DirectX are provided in the examples/
      folder to explain how to integrate Dear vsonyp0wer with your own engine/application.
    - Your programming IDE is your friend, find the type or function declaration to find comments
      associated to it.

 Q: Which version should I get?
 A: I occasionally tag Releases (https://github.com/ocornut/vsonyp0wer/releases) but it is generally safe
    and recommended to sync to master/latest. The library is fairly stable and regressions tend to be
    fixed fast when reported. You may also peak at the 'docking' branch which includes:
    - Docking/Merging features (https://github.com/ocornut/vsonyp0wer/issues/2109)
    - Multi-viewport features (https://github.com/ocornut/vsonyp0wer/issues/1542)
    Many projects are using this branch and it is kept in sync with master regularly.

 Q: Who uses Dear vsonyp0wer?
 A: See "Quotes" (https://github.com/ocornut/vsonyp0wer/wiki/Quotes) and
    "Software using Dear vsonyp0wer" (https://github.com/ocornut/vsonyp0wer/wiki/Software-using-dear-vsonyp0wer) Wiki pages
    for a list of games/software which are publicly known to use dear vsonyp0wer. Please add yours if you can!

 Q: Why the odd dual naming, "Dear vsonyp0wer" vs "vsonyp0wer"?
 A: The library started its life as "vsonyp0wer" due to the fact that I didn't give it a proper name when
    when I released 1.0, and had no particular expectation that it would take off. However, the term vsonyp0wer
    (immediate-mode graphical user interface) was coined before and is being used in variety of other
    situations (e.g. Unity uses it own implementation of the vsonyp0wer paradigm).
    To reduce the ambiguity without affecting existing code bases, I have decided on an alternate,
    longer name "Dear vsonyp0wer" that people can use to refer to this specific library.
    Please try to refer to this library as "Dear vsonyp0wer".

 Q: How can I tell whether to dispatch mouse/keyboard to vsonyp0wer or to my application?
 A: You can read the 'io.WantCaptureMouse', 'io.WantCaptureKeyboard' and 'io.WantTextInput' flags from the vsonyp0werIO structure (e.g. if (vsonyp0wer::GetIO().WantCaptureMouse) { ... } )
    - When 'io.WantCaptureMouse' is set, vsonyp0wer wants to use your mouse state, and you may want to discard/hide the inputs from the rest of your application.
    - When 'io.WantCaptureKeyboard' is set, vsonyp0wer wants to use your keyboard state, and you may want to discard/hide the inputs from the rest of your application.
    - When 'io.WantTextInput' is set to may want to notify your OS to popup an on-screen keyboard, if available (e.g. on a mobile phone, or console OS).
    Note: you should always pass your mouse/keyboard inputs to vsonyp0wer, even when the io.WantCaptureXXX flag are set false.
     This is because vsonyp0wer needs to detect that you clicked in the void to unfocus its own windows.
    Note: The 'io.WantCaptureMouse' is more accurate that any attempt to "check if the mouse is hovering a window" (don't do that!).
     It handle mouse dragging correctly (both dragging that started over your application or over an vsonyp0wer window) and handle e.g. modal windows blocking inputs.
     Those flags are updated by vsonyp0wer::NewFrame(). Preferably read the flags after calling NewFrame() if you can afford it, but reading them before is also
     perfectly fine, as the bool toggle fairly rarely. If you have on a touch device, you might find use for an early call to UpdateHoveredWindowAndCaptureFlags().
    Note: Text input widget releases focus on "Return KeyDown", so the subsequent "Return KeyUp" event that your application receive will typically
     have 'io.WantCaptureKeyboard=false'. Depending on your application logic it may or not be inconvenient. You might want to track which key-downs
     were targeted for Dear vsonyp0wer, e.g. with an array of bool, and filter out the corresponding key-ups.)

 Q: How can I display an image? What is ImTextureID, how does it works?
 A: Short explanation:
    - You may use functions such as vsonyp0wer::Image(), vsonyp0wer::ImageButton() or lower-level ImDrawList::AddImage() to emit draw calls that will use your own textures.
    - Actual textures are identified in a way that is up to the user/engine. Those identifiers are stored and passed as ImTextureID (void*) value.
    - Loading image files from the disk and turning them into a texture is not within the scope of Dear vsonyp0wer (for a good reason).
      Please read documentations or tutorials on your graphics API to understand how to display textures on the screen before moving onward.

    Long explanation:
    - Dear vsonyp0wer's job is to create "meshes", defined in a renderer-agnostic format made of draw commands and vertices.
      At the end of the frame those meshes (ImDrawList) will be displayed by your rendering function. They are made up of textured polygons and the code
      to render them is generally fairly short (a few dozen lines). In the examples/ folder we provide functions for popular graphics API (OpenGL, DirectX, etc.).
    - Each rendering function decides on a data type to represent "textures". The concept of what is a "texture" is entirely tied to your underlying engine/graphics API.
      We carry the information to identify a "texture" in the ImTextureID type.
      ImTextureID is nothing more that a void*, aka 4/8 bytes worth of data: just enough to store 1 pointer or 1 integer of your choice.
      Dear vsonyp0wer doesn't know or understand what you are storing in ImTextureID, it merely pass ImTextureID values until they reach your rendering function.
    - In the examples/ bindings, for each graphics API binding we decided on a type that is likely to be a good representation for specifying
      an image from the end-user perspective. This is what the _examples_ rendering functions are using:

         OpenGL:     ImTextureID = GLuint                       (see vsonyp0wer_ImplGlfwGL3_RenderDrawData() function in vsonyp0wer_impl_glfw_gl3.cpp)
         DirectX9:   ImTextureID = LPDIRECT3DTEXTURE9           (see vsonyp0wer_ImplDX9_RenderDrawData()     function in vsonyp0wer_impl_dx9.cpp)
         DirectX11:  ImTextureID = ID3D11ShaderResourceView*    (see vsonyp0wer_ImplDX11_RenderDrawData()    function in vsonyp0wer_impl_dx11.cpp)
         DirectX12:  ImTextureID = D3D12_GPU_DESCRIPTOR_HANDLE  (see vsonyp0wer_ImplDX12_RenderDrawData()    function in vsonyp0wer_impl_dx12.cpp)

      For example, in the OpenGL example binding we store raw OpenGL texture identifier (GLuint) inside ImTextureID.
      Whereas in the DirectX11 example binding we store a pointer to ID3D11ShaderResourceView inside ImTextureID, which is a higher-level structure
      tying together both the texture and information about its format and how to read it.
    - If you have a custom engine built over e.g. OpenGL, instead of passing GLuint around you may decide to use a high-level data type to carry information about
      the texture as well as how to display it (shaders, etc.). The decision of what to use as ImTextureID can always be made better knowing how your codebase
      is designed. If your engine has high-level data types for "textures" and "material" then you may want to use them.
      If you are starting with OpenGL or DirectX or Vulkan and haven't built much of a rendering engine over them, keeping the default ImTextureID
      representation suggested by the example bindings is probably the best choice.
      (Advanced users may also decide to keep a low-level type in ImTextureID, and use ImDrawList callback and pass information to their renderer)

    User code may do:

        // Cast our texture type to ImTextureID / void*
        MyTexture* texture = g_CoffeeTableTexture;
        vsonyp0wer::Image((void*)texture, ImVec2(texture->Width, texture->Height));

    The renderer function called after vsonyp0wer::Render() will receive that same value that the user code passed:

        // Cast ImTextureID / void* stored in the draw command as our texture type
        MyTexture* texture = (MyTexture*)pcmd->TextureId;
        MyEngineBindTexture2D(texture);

    Once you understand this design you will understand that loading image files and turning them into displayable textures is not within the scope of Dear vsonyp0wer.
    This is by design and is actually a good thing, because it means your code has full control over your data types and how you display them.
    If you want to display an image file (e.g. PNG file) into the screen, please refer to documentation and tutorials for the graphics API you are using.

    Here's a simplified OpenGL example using stb_image.h:

        // Use stb_image.h to load a PNG from disk and turn it into raw RGBA pixel data:
        #define STB_IMAGE_IMPLEMENTATION
        #include <stb_image.h>
        [...]
        int my_image_width, my_image_height;
        unsigned char* my_image_data = stbi_load("my_image.png", &my_image_width, &my_image_height, NULL, 4);

        // Turn the RGBA pixel data into an OpenGL texture:
        GLuint my_opengl_texture;
        glGenTextures(1, &my_opengl_texture);
        glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

        // Now that we have an OpenGL texture, assuming our vsonyp0wer rendering function (vsonyp0wer_impl_xxx.cpp file) takes GLuint as ImTextureID, we can display it:
        vsonyp0wer::Image((void*)(intptr_t)my_opengl_texture, ImVec2(my_image_width, my_image_height));

    C/C++ tip: a void* is pointer-sized stohnly. You may safely store any pointer or integer into it by casting your value to ImTextureID / void*, and vice-versa.
    Because both end-points (user code and rendering function) are under your control, you know exactly what is stored inside the ImTextureID / void*.
    Examples:

        GLuint my_tex = XXX;
        void* my_void_ptr;
        my_void_ptr = (void*)(intptr_t)my_tex;                  // cast a GLuint into a void* (we don't take its address! we literally store the value inside the pointer)
        my_tex = (GLuint)(intptr_t)my_void_ptr;                 // cast a void* into a GLuint

        ID3D11ShaderResourceView* my_dx11_srv = XXX;
        void* my_void_ptr;
        my_void_ptr = (void*)my_dx11_srv;                       // cast a ID3D11ShaderResourceView* into an opaque void*
        my_dx11_srv = (ID3D11ShaderResourceView*)my_void_ptr;   // cast a void* into a ID3D11ShaderResourceView*

    Finally, you may call vsonyp0wer::ShowMetricsWindow() to explore/visualize/understand how the ImDrawList are generated.

 Q: Why are multiple widgets reacting when I interact with a single one?
 Q: How can I have multiple widgets with the same label or with an empty label?
 A: A primer on labels and the ID Stack...

    Dear vsonyp0wer internally need to uniquely identify UI elements.
    Elements that are typically not clickable (such as calls to the Text functions) don't need an ID.
    Interactive widgets (such as calls to Button buttons) need a unique ID.
    Unique ID are used internally to track active widgets and occasionally associate state to widgets.
    Unique ID are implicitly built from the hash of multiple elements that identify the "path" to the UI element.

   - Unique ID are often derived from a string label:

       Button("OK");          // Label = "OK",     ID = hash of (..., "OK")
       Button("Cancel");      // Label = "Cancel", ID = hash of (..., "Cancel")

   - ID are uniquely scoped within windows, tree nodes, etc. which all pushes to the ID stack. Having
     two buttons labeled "OK" in different windows or different tree locations is fine.
     We used "..." above to signify whatever was already pushed to the ID stack previously:

       Begin("MyWindow");
       Button("OK");          // Label = "OK",     ID = hash of ("MyWindow", "OK")
       End();
       Begin("MyOtherWindow");
       Button("OK");          // Label = "OK",     ID = hash of ("MyOtherWindow", "OK")
       End();

   - If you have a same ID twice in the same location, you'll have a conflict:

       Button("OK");
       Button("OK");          // ID collision! Interacting with either button will trigger the first one.

     Fear not! this is easy to solve and there are many ways to solve it!

   - Solving ID conflict in a simple/local context:
     When passing a label you can optionally specify extra ID information within string itself.
     Use "##" to pass a complement to the ID that won't be visible to the end-user.
     This helps solving the simple collision cases when you know e.g. at compilation time which items
     are going to be created:

       Begin("MyWindow");
       Button("Play");        // Label = "Play",   ID = hash of ("MyWindow", "Play")
       Button("Play##foo1");  // Label = "Play",   ID = hash of ("MyWindow", "Play##foo1")  // Different from above
       Button("Play##foo2");  // Label = "Play",   ID = hash of ("MyWindow", "Play##foo2")  // Different from above
       End();

   - If you want to completely hide the label, but still need an ID:

       Checkbox("##On", &b);  // Label = "",       ID = hash of (..., "##On")   // No visible label, just a checkbox!

   - Occasionally/rarely you might want change a label while preserving a constant ID. This allows
     you to animate labels. For example you may want to include varying information in a window title bar,
     but windows are uniquely identified by their ID. Use "###" to pass a label that isn't part of ID:

       Button("Hello###ID");  // Label = "Hello",  ID = hash of (..., "###ID")
       Button("World###ID");  // Label = "World",  ID = hash of (..., "###ID")  // Same as above, even though the label looks different

       sprintf(buf, "My game (%f FPS)###MyGame", fps);
       Begin(buf);            // Variable title,   ID = hash of "MyGame"

   - Solving ID conflict in a more general manner:
     Use PushID() / PopID() to create scopes and manipulate the ID stack, as to avoid ID conflicts
     within the same window. This is the most convenient way of distinguishing ID when iterating and
     creating many UI elements programmatically.
     You can push a pointer, a string or an integer value into the ID stack.
     Remember that ID are formed from the concatenation of _everything_ pushed into the ID stack.
     At each level of the stack we store the seed used for items at this level of the ID stack.

     Begin("Window");
       for (int i = 0; i < 100; i++)
       {
         PushID(i);           // Push i to the id tack
         Button("Click");     // Label = "Click",  ID = hash of ("Window", i, "Click")
         PopID();
       }
       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj);
         Button("Click");     // Label = "Click",  ID = hash of ("Window", obj pointer, "Click")
         PopID();
       }
       for (int i = 0; i < 100; i++)
       {
         MyObject* obj = Objects[i];
         PushID(obj->Name);
         Button("Click");     // Label = "Click",  ID = hash of ("Window", obj->Name, "Click")
         PopID();
       }
       End();

   - You can stack multiple prefixes into the ID stack:

       Button("Click");       // Label = "Click",  ID = hash of (..., "Click")
       PushID("node");
       Button("Click");       // Label = "Click",  ID = hash of (..., "node", "Click")
         PushID(my_ptr);
           Button("Click");   // Label = "Click",  ID = hash of (..., "node", my_ptr, "Click")
         PopID();
       PopID();

   - Tree nodes implicitly creates a scope for you by calling PushID().

       Button("Click");       // Label = "Click",  ID = hash of (..., "Click")
       if (TreeNode("node"))  // <-- this function call will do a PushID() for you (unless instructed not to, with a special flag)
       {
         Button("Click");     // Label = "Click",  ID = hash of (..., "node", "Click")
         TreePop();
       }

   - When working with trees, ID are used to preserve the open/close state of each tree node.
     Depending on your use cases you may want to use strings, indices or pointers as ID.
      e.g. when following a single pointer that may change over time, using a static string as ID
       will preserve your node open/closed state when the targeted object change.
      e.g. when displaying a list of objects, using indices or pointers as ID will preserve the
       node open/closed state differently. See what makes more sense in your situation!

 Q: How can I use my own math types instead of ImVec2/ImVec4?
 A: You can edit imconfig.h and setup the IM_VEC2_CLASS_EXTRA/IM_VEC4_CLASS_EXTRA macros to add implicit type conversions.
    This way you'll be able to use your own types everywhere, e.g. passing glm::vec2 to vsonyp0wer functions instead of ImVec2.

 Q: How can I load a different font than the default?
 A: Use the font atlas to load the TTF/OTF file you want:
      vsonyp0werIO& io = vsonyp0wer::GetIO();
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
    Default is ProggyClean.ttf, monospace, rendered at size 13, embedded in dear vsonyp0wer's source code.
    (Tip: monospace fonts are convenient because they allow to facilitate horizontal alignment directly at the string level.)
    (Read the 'misc/fonts/README.txt' file for more details about font loading.)

    New programmers: remember that in C/C++ and most programming languages if you want to use a
    backslash \ within a string literal, you need to write it double backslash "\\":
      io.Fonts->AddFontFromFileTTF("MyDataFolder\MyFontFile.ttf", size_in_pixels);   // WRONG (you are escape the M here!)
      io.Fonts->AddFontFromFileTTF("MyDataFolder\\MyFontFile.ttf", size_in_pixels);  // CORRECT
      io.Fonts->AddFontFromFileTTF("MyDataFolder/MyFontFile.ttf", size_in_pixels);   // ALSO CORRECT

 Q: How can I easily use icons in my application?
 A: The most convenient and practical way is to merge an icon font such as FontAwesome inside you
    main font. Then you can refer to icons within your strings.
    You may want to see ImFontConfig::GlyphMinAdvanceX to make your icon look monospace to facilitate alignment.
    (Read the 'misc/fonts/README.txt' file for more details about icons font loading.)

 Q: How can I load multiple fonts?
 A: Use the font atlas to pack them into a single texture:
    (Read the 'misc/fonts/README.txt' file and the code in ImFontAtlas for more details.)

      vsonyp0werIO& io = vsonyp0wer::GetIO();
      ImFont* font0 = io.Fonts->AddFontDefault();
      ImFont* font1 = io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels);
      ImFont* font2 = io.Fonts->AddFontFromFileTTF("myfontfile2.ttf", size_in_pixels);
      io.Fonts->GetTexDataAsRGBA32() or GetTexDataAsAlpha8()
      // the first loaded font gets used by default
      // use vsonyp0wer::PushFont()/vsonyp0wer::PopFont() to change the font at runtime

      // Options
      ImFontConfig config;
      config.OversampleH = 2;
      config.OversampleV = 1;
      config.GlyphOffset.y -= 1.0f;      // Move everything by 1 pixels up
      config.GlyphExtraSpacing.x = 1.0f; // Increase spacing between characters
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_pixels, &config);

      // Combine multiple fonts into one (e.g. for icon fonts)
      static ImWchar ranges[] = { 0xf000, 0xf3ff, 0 };
      ImFontConfig config;
      config.MergeMode = true;
      io.Fonts->AddFontDefault();
      io.Fonts->AddFontFromFileTTF("fontawesome-webfont.ttf", 16.0f, &config, ranges); // Merge icon font
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_pixels, NULL, &config, io.Fonts->GetGlyphRangesJapanese()); // Merge japanese glyphs

 Q: How can I display and input non-Latin characters such as Chinese, Japanese, Korean, Cyrillic?
 A: When loading a font, pass custom Unicode ranges to specify the glyphs to load.

      // Add default Japanese ranges
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, io.Fonts->GetGlyphRangesJapanese());

      // Or create your own custom ranges (e.g. for a game you can feed your entire game script and only build the characters the game need)
      ImVector<ImWchar> ranges;
      ImFontGlyphRangesBuilder builder;
      builder.AddText("Hello world");                        // Add a string (here "Hello world" contains 7 unique characters)
      builder.AddChar(0x7262);                               // Add a specific character
      builder.AddRanges(io.Fonts->GetGlyphRangesJapanese()); // Add one of the default ranges
      builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)
      io.Fonts->AddFontFromFileTTF("myfontfile.ttf", size_in_pixels, NULL, ranges.Data);

    All your strings needs to use UTF-8 encoding. In C++11 you can encode a string literal in UTF-8
    by using the u8"hello" syntax. Specifying literal in your source code using a local code page
    (such as CP-923 for Japanese or CP-1251 for Cyrillic) will NOT work!
    Otherwise you can convert yourself to UTF-8 or load text data from file already saved as UTF-8.

    Text input: it is up to your application to pass the right character code by calling io.AddInputCharacter().
    The applications in examples/ are doing that.
    Windows: you can use the WM_CHAR or WM_UNICHAR or WM_IME_CHAR message (depending if your app is built using Unicode or MultiByte mode).
    You may also use MultiByteToWideChar() or ToUnicode() to retrieve Unicode codepoints from MultiByte characters or keyboard state.
    Windows: if your language is relying on an Input Method Editor (IME), you copy the HWND of your window to io.ImeWindowHandle in order for
    the default implementation of io.ImeSetInputScreenPosFn() to set your Microsoft IME position correctly.

 Q: How can I interact with standard C++ types (such as std::string and std::vector)?
 A: - Being highly portable (bindings for several languages, frameworks, programming style, obscure or older platforms/compilers),
      and aiming for compatibility & performance suitable for every modern real-time game engines, dear vsonyp0wer does not use
      any of std C++ types. We use raw types (e.g. char* instead of std::string) because they adapt to more use cases.
    - To use vsonyp0wer::InputText() with a std::string or any resizable string class, see misc/cpp/vsonyp0wer_stdlib.h.
    - To use combo boxes and list boxes with std::vector or any other data structure: the BeginCombo()/EndCombo() API
      lets you iterate and submit items yourself, so does the ListBoxHeader()/ListBoxFooter() API.
      Prefer using them over the old and awkward Combo()/ListBox() api.
    - Generally for most high-level types you should be able to access the underlying data type.
      You may write your own one-liner wrappers to facilitate user code (tip: add new functions in vsonyp0wer:: namespace from your code).
    - Dear vsonyp0wer applications often need to make intensive use of strings. It is expected that many of the strings you will pass
      to the API are raw literals (free in C/C++) or allocated in a manner that won't incur a large cost on your application.
      Please bear in mind that using std::string on applications with large amount of UI may incur unsatisfactory performances.
      Modern implementations of std::string often include small-string optimization (which is often a local buffer) but those
      are not configurable and not the same across implementations.
    - If you are finding your UI traversal cost to be too large, make sure your string usage is not leading to excessive amount
      of heap allocations. Consider using literals, statically sized buffers and your own helper functions. A common pattern
      is that you will need to build lots of strings on the fly, and their maximum length can be easily be scoped ahead.
      One possible implementation of a helper to facilitate printf-style building of strings: https://github.com/ocornut/Str
      This is a small helper where you can instance strings with configurable local buffers length. Many game engines will
      provide similar or better string helpers.

 Q: How can I use the drawing facilities without an vsonyp0wer window? (using ImDrawList API)
 A: - You can create a dummy window. Call Begin() with the NoBackground | NoDecoration | NoSavedSettings | NoInputs flags.
      (The vsonyp0werWindowFlags_NoDecoration flag itself is a shortcut for NoTitleBar | NoResize | NoScrollbar | NoCollapse)
      Then you can retrieve the ImDrawList* via GetWindowDrawList() and draw to it in any way you like.
    - You can call vsonyp0wer::GetBackgroundDrawList() or vsonyp0wer::GetForegroundDrawList() and use those draw list to display
      contents behind or over every other vsonyp0wer windows (one bg/fg drawlist per viewport).
    - You can create your own ImDrawList instance. You'll need to initialize them vsonyp0wer::GetDrawListSharedData(), or create
      your own ImDrawListSharedData, and then call your rendered code with your own ImDrawList or ImDrawData data.

 Q: How can I use this without a mouse, without a keyboard or without a screen? (gamepad, input share, remote display)
 A: - You can control Dear vsonyp0wer with a gamepad. Read about navigation in "Using gamepad/keyboard navigation controls".
      (short version: map gamepad inputs into the io.NavInputs[] array + set io.ConfigFlags |= vsonyp0werConfigFlags_NavEnableGamepad)
    - You can share your computer mouse seamlessly with your console/tablet/phone using Synergy (https://symless.com/synergy)
      This is the preferred solution for developer productivity.
      In particular, the "micro-synergy-client" repository (https://github.com/symless/micro-synergy-client) has simple
      and portable source code (uSynergy.c/.h) for a small embeddable client that you can use on any platform to connect
      to your host computer, based on the Synergy 1.x protocol. Make sure you download the Synergy 1 server on your computer.
      Console SDK also sometimes provide equivalent tooling or wrapper for Synergy-like protocols.
    - You may also use a third party solution such as Remote vsonyp0wer (https://github.com/JordiRos/remotevsonyp0wer) which sends
      the vertices to render over the local network, allowing you to use Dear vsonyp0wer even on a screen-less machine.
    - For touch inputs, you can increase the hit box of widgets (via the style.TouchPadding setting) to accommodate
      for the lack of precision of touch inputs, but it is recommended you use a mouse or gamepad to allow optimizing
      for screen real-estate and precision.

 Q: I integrated Dear vsonyp0wer in my engine and the text or lines are blurry..
 A: In your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f).
    Also make sure your orthographic projection matrix and io.DisplaySize matches your actual framebuffer dimension.

 Q: I integrated Dear vsonyp0wer in my engine and some elements are clipping or disappearing when I move windows around..
 A: You are probably mishandling the clipping rectangles in your render function.
    Rectangles provided by vsonyp0wer are defined as (x1=left,y1=top,x2=right,y2=bottom) and NOT as (x1,y1,width,height).

 Q: How can I help?
 A: - If you are experienced with Dear vsonyp0wer and C++, look at the github issues, look at the Wiki, read docs/TODO.txt
      and see how you want to help and can help!
    - Businesses: convince your company to fund development via support contracts/sponsoring! This is among the most useful thing you can do for dear vsonyp0wer.
    - Individuals: you can also become a Patron (http://www.patreon.com/vsonyp0wer) or donate on PayPal! See README.
    - Disclose your usage of dear vsonyp0wer via a dev blog post, a tweet, a screenshot, a mention somewhere etc.
      You may post screenshot or links in the gallery threads (github.com/ocornut/vsonyp0wer/issues/1902). Visuals are ideal as they inspire other programmers.
      But even without visuals, disclosing your use of dear vsonyp0wer help the library grow credibility, and help other teams and programmers with taking decisions.
    - If you have issues or if you need to hack into the library, even if you don't expect any support it is useful that you share your issues (on github or privately).

 - tip: you can call Begin() multiple times with the same name during the same frame, it will keep appending to the same window.
        this is also useful to set yourself in the context of another window (to get/set other settings)
 - tip: you can create widgets without a Begin()/End() block, they will go in an implicit window called "Debug".
 - tip: the vsonyp0werOnceUponAFrame helper will allow run the block of code only once a frame. You can use it to quickly add custom UI in the middle
        of a deep nested inner loop in your code.
 - tip: you can call Render() multiple times (e.g for VR renders).
 - tip: call and read the ShowDemoWindow() code in vsonyp0wer_demo.cpp for more example of how to use vsonyp0wer!

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
#include <stdio.h>      // vsnprintf, sscanf, printf
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

// Debug options
#define vsonyp0wer_DEBUG_NAV_SCORING     0   // Display navigation scoring preview when hovering items. Display last moving direction matches when holding CTRL
#define vsonyp0wer_DEBUG_NAV_RECTS       0   // Display the reference navigation rectangle for each window

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunknown-pragmas"        // warning : unknown warning group '-Wformat-pedantic *'        // not all warnings are known by all clang versions.. so ignoring warnings triggers new warnings on some configuration. great!
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning : format string is not a string literal              // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning : declaration requires an exit-time destructor       // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. vsonyp0wer coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#pragma clang diagnostic ignored "-Wformat-pedantic"        // warning : format specifies type 'void *' but the argument has type 'xxxx *' // unreasonable, would lead to casting every %p arg to void*. probably enabled by -pedantic.
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning : cast to 'void *' from smaller integer type 'int'
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning : zero as null pointer constant              // some standard header variations use #define NULL 0
#endif
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"       // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat"                   // warning: format '%p' expects argument of type 'void*', but argument 6 has type 'vsonyp0werWindow*'
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wformat-nonliteral"        // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wstrict-overflow"          // warning: assuming signed overflow does not occur when assuming that (X - c) > X is always false
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"          // warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif
#endif

// When using CTRL+TAB (or Gamepad Square+L/R) we delay the visual a little in order to reduce visual noise doing a fast switch.
static const float NAV_WINDOWING_HIGHLIGHT_DELAY = 0.20f;    // Time before the highlight and screen dimming starts fading in
static const float NAV_WINDOWING_LIST_APPEAR_DELAY = 0.15f;    // Time before the window list starts to appear

// Window resizing from edges (when io.ConfigWindowsResizeFromEdges = true and vsonyp0werBackendFlags_HasMouseCursors is set in io.BackendFlags by back-end)
static const float WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS = 4.0f;     // Extend outside and inside windows. Affect FindHoveredWindow().
static const float WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER = 0.04f;    // Reduce visual noise by only highlighting the border after a certain time.

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS
//-------------------------------------------------------------------------

static void             SetCurrentWindow(vsonyp0werWindow* window);
static void             FindHoveredWindow();
static vsonyp0werWindow* CreateNewWindow(const char* name, ImVec2 size, vsonyp0werWindowFlags flags);
static void             CheckStacksSize(vsonyp0werWindow* window, bool write);
static ImVec2           CalcNextScrollFromScrollTargetAndClamp(vsonyp0werWindow* window, bool snap_on_edges);

static void             AddDrawListToDrawData(ImVector<ImDrawList*>* out_list, ImDrawList* draw_list);
static void             AddWindowToSortBuffer(ImVector<vsonyp0werWindow*>* out_sorted_windows, vsonyp0werWindow* window);

static ImRect           GetViewportRect();

// Settings
static void* SettingsHandlerWindow_ReadOpen(vsonyp0werContext*, vsonyp0werSettingsHandler*, const char* name);
static void             SettingsHandlerWindow_ReadLine(vsonyp0werContext*, vsonyp0werSettingsHandler*, void* entry, const char* line);
static void             SettingsHandlerWindow_WriteAll(vsonyp0werContext* vsonyp0wer_ctx, vsonyp0werSettingsHandler* handler, vsonyp0werTextBuffer* buf);

// Platform Dependents default implementation for IO functions
static const char* GetClipboardTextFn_DefaultImpl(void* user_data);
static void             SetClipboardTextFn_DefaultImpl(void* user_data, const char* text);
static void             ImeSetInputScreenPosFn_DefaultImpl(int x, int y);

namespace vsonyp0wer
{
	static bool             BeginShadowEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);
	static bool             BeginGroupBoxEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);
	static bool             BeginGroupBoxScrollEx(const char* name, const char* groupboxName, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);
    static bool             BeginChildEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);
	static bool             BeginTabsEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);
	static bool             EndBorderEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags);

    // Navigation
    static void             NavUpdate();
    static void             NavUpdateWindowing();
    static void             NavUpdateWindowingList();
    static void             NavUpdateMoveResult();
    static float            NavUpdatePageUpPageDown(int allowed_dir_flags);
    static inline void      NavUpdateAnyRequestFlag();
    static void             NavProcessItem(vsonyp0werWindow* window, const ImRect& nav_bb, vsonyp0werID id);
    static ImVec2           NavCalcPreferredRefPos();
    static void             NavSaveLastChildNavWindowIntoParent(vsonyp0werWindow* nav_window);
    static vsonyp0werWindow* NavRestoreLastChildNavWindow(vsonyp0werWindow* window);

    // Misc
    static void             UpdateMouseInputs();
    static void             UpdateMouseWheel();
    static void             UpdateManualResize(vsonyp0werWindow* window, const ImVec2& size_auto_fit, int* border_held, int resize_grip_count, ImU32 resize_grip_col[4]);
    static void             RenderOuterBorders(vsonyp0werWindow* window);

}

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------

// Current context pointer. Implicitly used by all Dear vsonyp0wer functions. Always assumed to be != NULL.
// vsonyp0wer::CreateContext() will automatically set this pointer if it is NULL. Change to a different context by calling vsonyp0wer::SetCurrentContext().
// 1) Important: globals are not shared across DLL boundaries! If you use DLLs or any form of hot-reloading: you will need to call
//    SetCurrentContext() (with the pointer you got from CreateContext) from each unique static/DLL boundary, and after each hot-reloading.
//    In your debugger, add Gvsonyp0wer to your watch window and notice how its value changes depending on which location you are currently stepping into.
// 2) Important: Dear vsonyp0wer functions are not thread-safe because of this pointer.
//    If you want thread-safety to allow N threads to access N different contexts, you can:
//    - Change this variable to use thread local stohnly so each thread can refer to a different context, in imconfig.h:
//          struct vsonyp0werContext;
//          extern thread_local vsonyp0werContext* Myvsonyp0werTLS;
//          #define Gvsonyp0wer Myvsonyp0werTLS
//      And then define Myvsonyp0werTLS in one of your cpp file. Note that thread_local is a C++11 keyword, earlier C++ uses compiler-specific keyword.
//    - Future development aim to make this context pointer explicit to all calls. Also read https://github.com/ocornut/vsonyp0wer/issues/586
//    - If you need a finite number of contexts, you may compile and use multiple instances of the vsonyp0wer code from different namespace.
#ifndef Gvsonyp0wer
vsonyp0werContext* Gvsonyp0wer = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// If you use DLL hotreloading you might need to call SetAllocatorFunctions() after reloading code from this file.
// Otherwise, you probably don't want to modify them mid-program, and if you use global/static e.g. ImVector<> instances you may need to keep them accessible during program destruction.
#ifndef vsonyp0wer_DISABLE_DEFAULT_ALLOCATORS
static void* MallocWrapper(size_t size, void* user_data) { IM_UNUSED(user_data); return malloc(size); }
static void    FreeWrapper(void* ptr, void* user_data) { IM_UNUSED(user_data); free(ptr); }
#else
static void* MallocWrapper(size_t size, void* user_data) { IM_UNUSED(user_data); IM_UNUSED(size); IM_ASSERT(0); return NULL; }
static void    FreeWrapper(void* ptr, void* user_data) { IM_UNUSED(user_data); IM_UNUSED(ptr); IM_ASSERT(0); }
#endif

static void* (*GImAllocatorAllocFunc)(size_t size, void* user_data) = MallocWrapper;
static void   (*GImAllocatorFreeFunc)(void* ptr, void* user_data) = FreeWrapper;
static void* GImAllocatorUserData = NULL;

//-----------------------------------------------------------------------------
// [SECTION] MAIN USER FACING STRUCTURES (vsonyp0werStyle, vsonyp0werIO)
//-----------------------------------------------------------------------------

vsonyp0werStyle::vsonyp0werStyle()
{
    Alpha = 1.0f;             // Global alpha applies to everything in vsonyp0wer
    WindowPadding = ImVec2(8, 8);      // Padding within a window
    WindowRounding = 0.0f;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    WindowBorderSize = 1.0f;             // Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    WindowMinSize = ImVec2(32, 32);    // Minimum window size
    WindowTitleAlign = ImVec2(0.0f, 0.5f);// Alignment for title bar text
    ChildRounding = 0.0f;             // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    ChildBorderSize = 1.0f;             // Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    PopupRounding = 0.0f;             // Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
    PopupBorderSize = 1.0f;             // Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    FramePadding = ImVec2(4, 3);      // Padding within a framed rectangle (used by most widgets)
    FrameRounding = 0.0f;             // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    FrameBorderSize = 0.0f;             // Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
    ItemSpacing = ImVec2(8, 4);      // Horizontal and vertical spacing between widgets/lines
    ItemInnerSpacing = ImVec2(4, 4);      // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    TouchExtraPadding = ImVec2(0, 0);      // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    IndentSpacing = 21.0f;            // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    ColumnsMinSpacing = 6.0f;             // Minimum horizontal spacing between two columns
    ScrollbarSize = 7.0f;            // Width of the vertical scrollbar, Height of the horizontal scrollbar
    ScrollbarRounding = 0.0f;             // Radius of grab corners rounding for scrollbar
    GrabMinSize = 10.0f;            // Minimum width/height of a grab box for slider/scrollbar
    GrabRounding = 0.0f;             // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    TabRounding = 0.0f;             // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    TabBorderSize = 0.0f;             // Thickness of border around tabs.
    ButtonTextAlign = ImVec2(0.5f, 0.5f);// Alignment of button text when button is larger than text.
    SelectableTextAlign = ImVec2(0.0f, 0.0f);// Alignment of selectable text when button is larger than text.
    DisplayWindowPadding = ImVec2(19, 19);    // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
    DisplaySafeAreaPadding = ImVec2(3, 3);      // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    MouseCursorScale = 1.0f;             // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    AntiAliasedLines = true;             // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
    AntiAliasedFill = true;             // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    CurveTessellationTol = 1.25f;            // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

    // Default theme
    vsonyp0wer::StyleColorsDark(this);
}

// To scale your entire UI (e.g. if you want your app to use High DPI or generally be DPI aware) you may use this helper function. Scaling the fonts is done separately and is up to you.
// Important: This operation is lossy because we round all sizes to integer. If you need to change your scale multiples, call this over a freshly initialized vsonyp0werStyle structure rather than scaling multiple times.
void vsonyp0werStyle::ScaleAllSizes(float scale_factor)
{
    WindowPadding = ImFloor(WindowPadding * scale_factor);
    WindowRounding = ImFloor(WindowRounding * scale_factor);
    WindowMinSize = ImFloor(WindowMinSize * scale_factor);
    ChildRounding = ImFloor(ChildRounding * scale_factor);
    PopupRounding = ImFloor(PopupRounding * scale_factor);
    FramePadding = ImFloor(FramePadding * scale_factor);
    FrameRounding = ImFloor(FrameRounding * scale_factor);
    ItemSpacing = ImFloor(ItemSpacing * scale_factor);
    ItemInnerSpacing = ImFloor(ItemInnerSpacing * scale_factor);
    TouchExtraPadding = ImFloor(TouchExtraPadding * scale_factor);
    IndentSpacing = ImFloor(IndentSpacing * scale_factor);
    ColumnsMinSpacing = ImFloor(ColumnsMinSpacing * scale_factor);
    ScrollbarSize = ImFloor(ScrollbarSize * scale_factor);
    ScrollbarRounding = ImFloor(ScrollbarRounding * scale_factor);
    GrabMinSize = ImFloor(GrabMinSize * scale_factor);
    GrabRounding = ImFloor(GrabRounding * scale_factor);
    TabRounding = ImFloor(TabRounding * scale_factor);
    DisplayWindowPadding = ImFloor(DisplayWindowPadding * scale_factor);
    DisplaySafeAreaPadding = ImFloor(DisplaySafeAreaPadding * scale_factor);
    MouseCursorScale = ImFloor(MouseCursorScale * scale_factor);
}

vsonyp0werIO::vsonyp0werIO()
{
    // Most fields are initialized with zero
    memset(this, 0, sizeof(*this));

    // Settings
    ConfigFlags = vsonyp0werConfigFlags_None;
    BackendFlags = vsonyp0werBackendFlags_None;
    DisplaySize = ImVec2(-1.0f, -1.0f);
    DeltaTime = 1.0f / 60.0f;
    IniSavingRate = 5.0f;
    IniFilename = "vsonyp0wer.ini";
    LogFilename = "vsonyp0wer_log.txt";
    MouseDoubleClickTime = 0.30f;
    MouseDoubleClickMaxDist = 6.0f;
    for (int i = 0; i < vsonyp0werKey_COUNT; i++)
        KeyMap[i] = -1;
    KeyRepeatDelay = 0.250f;
    KeyRepeatRate = 0.050f;
    UserData = NULL;

    Fonts = NULL;
    FontGlobalScale = 1.0f;
    FontDefault = NULL;
    FontAllowUserScaling = false;
    DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Miscellaneous options
    MouseDrawCursor = false;
#ifdef __APPLE__
    ConfigMacOSXBehaviors = true;  // Set Mac OS X style defaults based on __APPLE__ compile time flag
#else
    ConfigMacOSXBehaviors = false;
#endif
    ConfigInputTextCursorBlink = true;
    ConfigWindowsResizeFromEdges = true;
    ConfigWindowsMoveFromTitleBarOnly = false;

    // Platform Functions
    BackendPlatformName = BackendRendererName = NULL;
    BackendPlatformUserData = BackendRendererUserData = BackendLanguageUserData = NULL;
    GetClipboardTextFn = GetClipboardTextFn_DefaultImpl;   // Platform dependent default implementations
    SetClipboardTextFn = SetClipboardTextFn_DefaultImpl;
    ClipboardUserData = NULL;
    ImeSetInputScreenPosFn = ImeSetInputScreenPosFn_DefaultImpl;
    ImeWindowHandle = NULL;

#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    RenderDrawListsFn = NULL;
#endif

    // Input (NB: we already have memset zero the entire structure!)
    MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    MousePosPrev = ImVec2(-FLT_MAX, -FLT_MAX);
    MouseDragThreshold = 6.0f;
    for (int i = 0; i < IM_ARRAYSIZE(MouseDownDuration); i++) MouseDownDuration[i] = MouseDownDurationPrev[i] = -1.0f;
    for (int i = 0; i < IM_ARRAYSIZE(KeysDownDuration); i++) KeysDownDuration[i] = KeysDownDurationPrev[i] = -1.0f;
    for (int i = 0; i < IM_ARRAYSIZE(NavInputsDownDuration); i++) NavInputsDownDuration[i] = -1.0f;
}

// Pass in translated ASCII characters for text input.
// - with glfw you can get those from the callback set in glfwSetCharCallback()
// - on Windows you can get those using ToAscii+keyboard state, or via the WM_CHAR message
void vsonyp0werIO::AddInputCharacter(ImWchar c)
{
    InputQueueCharacters.push_back(c);
}

void vsonyp0werIO::AddInputCharactersUTF8(const char* utf8_chars)
{
    while (*utf8_chars != 0)
    {
        unsigned int c = 0;
        utf8_chars += ImTextCharFromUtf8(&c, utf8_chars, NULL);
        if (c > 0 && c <= 0xFFFF)
            InputQueueCharacters.push_back((ImWchar)c);
    }
}

void vsonyp0werIO::ClearInputCharacters()
{
    InputQueueCharacters.resize(0);
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (Maths, String, Format, Hash, File functions)
//-----------------------------------------------------------------------------

ImVec2 ImLineClosestPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & p)
{
    ImVec2 ap = p - a;
    ImVec2 ab_dir = b - a;
    float dot = ap.x * ab_dir.x + ap.y * ab_dir.y;
    if (dot < 0.0f)
        return a;
    float ab_len_sqr = ab_dir.x * ab_dir.x + ab_dir.y * ab_dir.y;
    if (dot > ab_len_sqr)
        return b;
    return a + ab_dir * dot / ab_len_sqr;
}

bool ImTriangleContainsPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p)
{
    bool b1 = ((p.x - b.x) * (a.y - b.y) - (p.y - b.y) * (a.x - b.x)) < 0.0f;
    bool b2 = ((p.x - c.x) * (b.y - c.y) - (p.y - c.y) * (b.x - c.x)) < 0.0f;
    bool b3 = ((p.x - a.x) * (c.y - a.y) - (p.y - a.y) * (c.x - a.x)) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

void ImTriangleBarycentricCoords(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p, float& out_u, float& out_v, float& out_w)
{
    ImVec2 v0 = b - a;
    ImVec2 v1 = c - a;
    ImVec2 v2 = p - a;
    const float denom = v0.x * v1.y - v1.x * v0.y;
    out_v = (v2.x * v1.y - v1.x * v2.y) / denom;
    out_w = (v0.x * v2.y - v2.x * v0.y) / denom;
    out_u = 1.0f - out_v - out_w;
}

ImVec2 ImTriangleClosestPoint(const ImVec2 & a, const ImVec2 & b, const ImVec2 & c, const ImVec2 & p)
{
    ImVec2 proj_ab = ImLineClosestPoint(a, b, p);
    ImVec2 proj_bc = ImLineClosestPoint(b, c, p);
    ImVec2 proj_ca = ImLineClosestPoint(c, a, p);
    float dist2_ab = ImLengthSqr(p - proj_ab);
    float dist2_bc = ImLengthSqr(p - proj_bc);
    float dist2_ca = ImLengthSqr(p - proj_ca);
    float m = ImMin(dist2_ab, ImMin(dist2_bc, dist2_ca));
    if (m == dist2_ab)
        return proj_ab;
    if (m == dist2_bc)
        return proj_bc;
    return proj_ca;
}

// Consider using _stricmp/_strnicmp under Windows or strcasecmp/strncasecmp. We don't actually use either ImStricmp/ImStrnicmp in the codebase any more.
int ImStricmp(const char* str1, const char* str2)
{
    int d;
    while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; }
    return d;
}

int ImStrnicmp(const char* str1, const char* str2, size_t count)
{
    int d = 0;
    while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; count--; }
    return d;
}

void ImStrncpy(char* dst, const char* src, size_t count)
{
    if (count < 1)
        return;
    if (count > 1)
        strncpy(dst, src, count - 1);
    dst[count - 1] = 0;
}

char* ImStrdup(const char* str)
{
    size_t len = strlen(str);
    void* buf = IM_ALLOC(len + 1);
    return (char*)memcpy(buf, (const void*)str, len + 1);
}

char* ImStrdupcpy(char* dst, size_t * p_dst_size, const char* src)
{
    size_t dst_buf_size = p_dst_size ? *p_dst_size : strlen(dst) + 1;
    size_t src_size = strlen(src) + 1;
    if (dst_buf_size < src_size)
    {
        IM_FREE(dst);
        dst = (char*)IM_ALLOC(src_size);
        if (p_dst_size)
            * p_dst_size = src_size;
    }
    return (char*)memcpy(dst, (const void*)src, src_size);
}

const char* ImStrchrRange(const char* str, const char* str_end, char c)
{
    const char* p = (const char*)memchr(str, (int)c, str_end - str);
    return p;
}

int ImStrlenW(const ImWchar * str)
{
    //return (int)wcslen((const wchar_t*)str);  // FIXME-OPT: Could use this when wchar_t are 16-bits
    int n = 0;
    while (*str++) n++;
    return n;
}

// Find end-of-line. Return pointer will point to either first \n, either str_end.
const char* ImStreolRange(const char* str, const char* str_end)
{
    const char* p = (const char*)memchr(str, '\n', str_end - str);
    return p ? p : str_end;
}

const ImWchar * ImStrbolW(const ImWchar * buf_mid_line, const ImWchar * buf_begin) // find beginning-of-line
{
    while (buf_mid_line > buf_begin && buf_mid_line[-1] != '\n')
        buf_mid_line--;
    return buf_mid_line;
}

const char* ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end)
{
    if (!needle_end)
        needle_end = needle + strlen(needle);

    const char un0 = (char)toupper(*needle);
    while ((!haystack_end && *haystack) || (haystack_end && haystack < haystack_end))
    {
        if (toupper(*haystack) == un0)
        {
            const char* b = needle + 1;
            for (const char* a = haystack + 1; b < needle_end; a++, b++)
                if (toupper(*a) != toupper(*b))
                    break;
            if (b == needle_end)
                return haystack;
        }
        haystack++;
    }
    return NULL;
}

// Trim str by offsetting contents when there's leading data + writing a \0 at the trailing position. We use this in situation where the cost is negligible.
void ImStrTrimBlanks(char* buf)
{
    char* p = buf;
    while (p[0] == ' ' || p[0] == '\t')     // Leading blanks
        p++;
    char* p_start = p;
    while (*p != 0)                         // Find end of string
        p++;
    while (p > p_start && (p[-1] == ' ' || p[-1] == '\t'))  // Trailing blanks
        p--;
    if (p_start != buf)                     // Copy memory if we had leading blanks
        memmove(buf, p_start, p - p_start);
    buf[p - p_start] = 0;                   // Zero terminate
}

// A) MSVC version appears to return -1 on overflow, whereas glibc appears to return total count (which may be >= buf_size).
// Ideally we would test for only one of those limits at runtime depending on the behavior the vsnprintf(), but trying to deduct it at compile time sounds like a pandora can of worm.
// B) When buf==NULL vsnprintf() will return the output size.
#ifndef vsonyp0wer_DISABLE_FORMAT_STRING_FUNCTIONS

//#define vsonyp0wer_USE_STB_SPRINTF
#ifdef vsonyp0wer_USE_STB_SPRINTF
#define STB_SPRINTF_IMPLEMENTATION
#include "vsdns_sprintf.h"
#endif

#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf _vsnprintf
#endif

int ImFormatString(char* buf, size_t buf_size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#ifdef vsonyp0wer_USE_STB_SPRINTF
    int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#else
    int w = vsnprintf(buf, buf_size, fmt, args);
#endif
    va_end(args);
    if (buf == NULL)
        return w;
    if (w == -1 || w >= (int)buf_size)
        w = (int)buf_size - 1;
    buf[w] = 0;
    return w;
}

int ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args)
{
#ifdef vsonyp0wer_USE_STB_SPRINTF
    int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#else
    int w = vsnprintf(buf, buf_size, fmt, args);
#endif
    if (buf == NULL)
        return w;
    if (w == -1 || w >= (int)buf_size)
        w = (int)buf_size - 1;
    buf[w] = 0;
    return w;
}
#endif // #ifdef vsonyp0wer_DISABLE_FORMAT_STRING_FUNCTIONS

// CRC32 needs a 1KB lookup table (not cache friendly)
// Although the code to generate the table is simple and shorter than the table itself, using a const table allows us to easily:
// - avoid an unnecessary branch/memory tap, - keep the ImHashXXX functions usable by static constructors, - make it thread-safe.
static const ImU32 GCrc32LookupTable[256] =
{
    0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
    0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
    0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
    0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
    0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
    0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
    0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
    0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
    0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
    0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
    0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
    0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
    0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
    0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
    0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
    0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
};

// Known size hash
// It is ok to call ImHashData on a string with known length but the ### operator won't be supported.
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
ImU32 ImHashData(const void* data_p, size_t data_size, ImU32 seed)
{
    ImU32 crc = ~seed;
    const unsigned char* data = (const unsigned char*)data_p;
    const ImU32* crc32_lut = GCrc32LookupTable;
    while (data_size-- != 0)
        crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *data++];
    return ~crc;
}

// Zero-terminated string hash, with support for ### to reset back to seed value
// We support a syntax of "label###id" where only "###id" is included in the hash, and only "label" gets displayed.
// Because this syntax is rarely used we are optimizing for the common case.
// - If we reach ### in the string we discard the hash so far and reset to the seed.
// - We don't do 'current += 2; continue;' after handling ### to keep the code smaller/faster (measured ~10% diff in Debug build)
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do proper measurements.
ImU32 ImHashStr(const char* data_p, size_t data_size, ImU32 seed)
{
    seed = ~seed;
    ImU32 crc = seed;
    const unsigned char* data = (const unsigned char*)data_p;
    const ImU32* crc32_lut = GCrc32LookupTable;
    if (data_size != 0)
    {
        while (data_size-- != 0)
        {
            unsigned char c = *data++;
            if (c == '#' && data_size >= 2 && data[0] == '#' && data[1] == '#')
                crc = seed;
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    } else
    {
        while (unsigned char c = *data++)
        {
            if (c == '#' && data[0] == '#' && data[1] == '#')
                crc = seed;
            crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
        }
    }
    return ~crc;
}

FILE* ImFileOpen(const char* filename, const char* mode)
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__GNUC__)
    // We need a fopen() wrapper because MSVC/Windows fopen doesn't handle UTF-8 filenames. Converting both strings from UTF-8 to wchar format (using a single allocation, because we can)
    const int filename_wsize = ImTextCountCharsFromUtf8(filename, NULL) + 1;
    const int mode_wsize = ImTextCountCharsFromUtf8(mode, NULL) + 1;
    ImVector<ImWchar> buf;
    buf.resize(filename_wsize + mode_wsize);
    ImTextStrFromUtf8(&buf[0], filename_wsize, filename, NULL);
    ImTextStrFromUtf8(&buf[filename_wsize], mode_wsize, mode, NULL);
    return _wfopen((wchar_t*)& buf[0], (wchar_t*)& buf[filename_wsize]);
#else
    return fopen(filename, mode);
#endif
}

// Load file content into memory
// Memory allocated with IM_ALLOC(), must be freed by user using IM_FREE() == vsonyp0wer::MemFree()
void* ImFileLoadToMemory(const char* filename, const char* file_open_mode, size_t * out_file_size, int padding_bytes)
{
    IM_ASSERT(filename && file_open_mode);
    if (out_file_size)
        * out_file_size = 0;

    FILE* f;
    if ((f = ImFileOpen(filename, file_open_mode)) == NULL)
        return NULL;

    long file_size_signed;
    if (fseek(f, 0, SEEK_END) || (file_size_signed = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
    {
        fclose(f);
        return NULL;
    }

    size_t file_size = (size_t)file_size_signed;
    void* file_data = IM_ALLOC(file_size + padding_bytes);
    if (file_data == NULL)
    {
        fclose(f);
        return NULL;
    }
    if (fread(file_data, 1, file_size, f) != file_size)
    {
        fclose(f);
        IM_FREE(file_data);
        return NULL;
    }
    if (padding_bytes > 0)
        memset((void*)(((char*)file_data) + file_size), 0, (size_t)padding_bytes);

    fclose(f);
    if (out_file_size)
        * out_file_size = file_size;

    return file_data;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

// Convert UTF-8 to 32-bits character, process single character input.
// Based on stb_from_utf8() from github.com/nothings/stb/
// We handle UTF-8 decoding error by skipping forward.
int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)in_text;
    if (!(*str & 0x80))
    {
        c = (unsigned int)(*str++);
        *out_char = c;
        return 1;
    }
    if ((*str & 0xe0) == 0xc0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 2) return 1;
        if (*str < 0xc2) return 2;
        c = (unsigned int)((*str++ & 0x1f) << 6);
        if ((*str & 0xc0) != 0x80) return 2;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 2;
    }
    if ((*str & 0xf0) == 0xe0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 3) return 1;
        if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
        if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x0f) << 12);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 3;
        c += (*str++ & 0x3f);
        *out_char = c;
        return 3;
    }
    if ((*str & 0xf8) == 0xf0)
    {
        *out_char = 0xFFFD; // will be invalid but not end of string
        if (in_text_end && in_text_end - (const char*)str < 4) return 1;
        if (*str > 0xf4) return 4;
        if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
        if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80) return 4;
        c += (*str++ & 0x3f);
        // utf-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800) return 4;
        *out_char = c;
        return 4;
    }
    *out_char = 0;
    return 0;
}

int ImTextStrFromUtf8(ImWchar * buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_text_remaining)
{
    ImWchar* buf_out = buf;
    ImWchar* buf_end = buf + buf_size;
    while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
        if (c < 0x10000)    // FIXME: Losing characters that don't fit in 2 bytes
            * buf_out++ = (ImWchar)c;
    }
    *buf_out = 0;
    if (in_text_remaining)
        * in_text_remaining = in_text;
    return (int)(buf_out - buf);
}

int ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end)
{
    int char_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c;
        in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
        if (c == 0)
            break;
        if (c < 0x10000)
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
        buf[0] = (char)(0xc0 + (c >> 6));
        buf[1] = (char)(0x80 + (c & 0x3f));
        return 2;
    }
    if (c >= 0xdc00 && c < 0xe000)
    {
        return 0;
    }
    if (c >= 0xd800 && c < 0xdc00)
    {
        if (buf_size < 4) return 0;
        buf[0] = (char)(0xf0 + (c >> 18));
        buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
        buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[3] = (char)(0x80 + ((c) & 0x3f));
        return 4;
    }
    //else if (c < 0x10000)
    {
        if (buf_size < 3) return 0;
        buf[0] = (char)(0xe0 + (c >> 12));
        buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[2] = (char)(0x80 + ((c) & 0x3f));
        return 3;
    }
}

// Not optimal but we very rarely use this function.
int ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end)
{
    unsigned int dummy = 0;
    return ImTextCharFromUtf8(&dummy, in_text, in_text_end);
}

static inline int ImTextCountUtf8BytesFromChar(unsigned int c)
{
    if (c < 0x80) return 1;
    if (c < 0x800) return 2;
    if (c >= 0xdc00 && c < 0xe000) return 0;
    if (c >= 0xd800 && c < 0xdc00) return 4;
    return 3;
}

int ImTextStrToUtf8(char* buf, int buf_size, const ImWchar * in_text, const ImWchar * in_text_end)
{
    char* buf_out = buf;
    const char* buf_end = buf + buf_size;
    while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c = (unsigned int)(*in_text++);
        if (c < 0x80)
            * buf_out++ = (char)c;
        else
            buf_out += ImTextCharToUtf8(buf_out, (int)(buf_end - buf_out - 1), c);
    }
    *buf_out = 0;
    return (int)(buf_out - buf);
}

int ImTextCountUtf8BytesFromStr(const ImWchar * in_text, const ImWchar * in_text_end)
{
    int bytes_count = 0;
    while ((!in_text_end || in_text < in_text_end) && *in_text)
    {
        unsigned int c = (unsigned int)(*in_text++);
        if (c < 0x80)
            bytes_count++;
        else
            bytes_count += ImTextCountUtf8BytesFromChar(c);
    }
    return bytes_count;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILTIES (Color functions)
// Note: The Convert functions are early design which are not consistent with other API.
//-----------------------------------------------------------------------------

ImVec4 vsonyp0wer::ColorConvertU32ToFloat4(ImU32 in)
{
    float s = 1.0f / 255.0f;
    return ImVec4(
        ((in >> IM_COL32_R_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_G_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_B_SHIFT) & 0xFF) * s,
        ((in >> IM_COL32_A_SHIFT) & 0xFF) * s);
}

ImU32 vsonyp0wer::ColorConvertFloat4ToU32(const ImVec4 & in)
{
    ImU32 out;
    out = ((ImU32)IM_F32_TO_INT8_SAT(in.x)) << IM_COL32_R_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.y)) << IM_COL32_G_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.z)) << IM_COL32_B_SHIFT;
    out |= ((ImU32)IM_F32_TO_INT8_SAT(in.w)) << IM_COL32_A_SHIFT;
    return out;
}

// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam p592
// Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void vsonyp0wer::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
    float K = 0.f;
    if (g < b)
    {
        ImSwap(g, b);
        K = -1.f;
    }
    if (r < g)
    {
        ImSwap(r, g);
        K = -2.f / 6.f - K;
    }

    const float chroma = r - (g < b ? g : b);
    out_h = ImFabs(K + (g - b) / (6.f * chroma + 1e-20f));
    out_s = chroma / (r + 1e-20f);
    out_v = r;
}

// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]), from Foley & van Dam p593
// also http://en.wikipedia.org/wiki/HSL_and_HSV
void vsonyp0wer::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
    if (s == 0.0f)
    {
        // gray
        out_r = out_g = out_b = v;
        return;
    }

    h = ImFmod(h, 1.0f) / (60.0f / 360.0f);
    int   i = (int)h;
    float f = h - (float)i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
    case 0: out_r = v; out_g = t; out_b = p; break;
    case 1: out_r = q; out_g = v; out_b = p; break;
    case 2: out_r = p; out_g = v; out_b = t; break;
    case 3: out_r = p; out_g = q; out_b = v; break;
    case 4: out_r = t; out_g = p; out_b = v; break;
    case 5: default: out_r = v; out_g = p; out_b = q; break;
    }
}

ImU32 vsonyp0wer::GetColorU32(vsonyp0werCol idx, float alpha_mul)
{
    vsonyp0werStyle& style = Gvsonyp0wer->Style;
    ImVec4 c = style.Colors[idx];
    c.w *= style.Alpha * alpha_mul;
    return ColorConvertFloat4ToU32(c);
}

ImU32 vsonyp0wer::GetColorU32(const ImVec4 & col)
{
    vsonyp0werStyle& style = Gvsonyp0wer->Style;
    ImVec4 c = col;
    c.w *= style.Alpha;
    return ColorConvertFloat4ToU32(c);
}

const ImVec4& vsonyp0wer::GetStyleColorVec4(vsonyp0werCol idx)
{
    vsonyp0werStyle& style = Gvsonyp0wer->Style;
    return style.Colors[idx];
}

ImU32 vsonyp0wer::GetColorU32(ImU32 col)
{
    float style_alpha = Gvsonyp0wer->Style.Alpha;
    if (style_alpha >= 1.0f)
        return col;
    ImU32 a = (col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT;
    a = (ImU32)(a * style_alpha); // We don't need to clamp 0..255 because Style.Alpha is in 0..1 range.
    return (col & ~IM_COL32_A_MASK) | (a << IM_COL32_A_SHIFT);
}

//-----------------------------------------------------------------------------
// [SECTION] vsonyp0werStohnly
// Helper: Key->value stohnly
//-----------------------------------------------------------------------------

// std::lower_bound but without the bullshit
static vsonyp0werStohnly::Pair * LowerBound(ImVector<vsonyp0werStohnly::Pair> & data, vsonyp0werID key)
{
    vsonyp0werStohnly::Pair* first = data.Data;
    vsonyp0werStohnly::Pair* last = data.Data + data.Size;
    size_t count = (size_t)(last - first);
    while (count > 0)
    {
        size_t count2 = count >> 1;
        vsonyp0werStohnly::Pair* mid = first + count2;
        if (mid->key < key)
        {
            first = ++mid;
            count -= count2 + 1;
        } else
        {
            count = count2;
        }
    }
    return first;
}

// For quicker full rebuild of a stohnly (instead of an incremental one), you may add all your contents and then sort once.
void vsonyp0werStohnly::BuildSortByKey()
{
    struct StaticFunc
    {
        static int vsonyp0wer_CDECL PairCompareByID(const void* lhs, const void* rhs)
        {
            // We can't just do a subtraction because qsort uses signed integers and subtracting our ID doesn't play well with that.
            if (((const Pair*)lhs)->key > ((const Pair*)rhs)->key) return +1;
            if (((const Pair*)lhs)->key < ((const Pair*)rhs)->key) return -1;
            return 0;
        }
    };
    if (Data.Size > 1)
        ImQsort(Data.Data, (size_t)Data.Size, sizeof(Pair), StaticFunc::PairCompareByID);
}

int vsonyp0werStohnly::GetInt(vsonyp0werID key, int default_val) const
{
    vsonyp0werStohnly::Pair* it = LowerBound(const_cast<ImVector<vsonyp0werStohnly::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_i;
}

bool vsonyp0werStohnly::GetBool(vsonyp0werID key, bool default_val) const
{
    return GetInt(key, default_val ? 1 : 0) != 0;
}

float vsonyp0werStohnly::GetFloat(vsonyp0werID key, float default_val) const
{
    vsonyp0werStohnly::Pair* it = LowerBound(const_cast<ImVector<vsonyp0werStohnly::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return default_val;
    return it->val_f;
}

void* vsonyp0werStohnly::GetVoidPtr(vsonyp0werID key) const
{
    vsonyp0werStohnly::Pair* it = LowerBound(const_cast<ImVector<vsonyp0werStohnly::Pair>&>(Data), key);
    if (it == Data.end() || it->key != key)
        return NULL;
    return it->val_p;
}

// References are only valid until a new value is added to the stohnly. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
int* vsonyp0werStohnly::GetIntRef(vsonyp0werID key, int default_val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_i;
}

bool* vsonyp0werStohnly::GetBoolRef(vsonyp0werID key, bool default_val)
{
    return (bool*)GetIntRef(key, default_val ? 1 : 0);
}

float* vsonyp0werStohnly::GetFloatRef(vsonyp0werID key, float default_val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_f;
}

void** vsonyp0werStohnly::GetVoidPtrRef(vsonyp0werID key, void* default_val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
        it = Data.insert(it, Pair(key, default_val));
    return &it->val_p;
}

// FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too bad because it only happens on explicit interaction (maximum one a frame)
void vsonyp0werStohnly::SetInt(vsonyp0werID key, int val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_i = val;
}

void vsonyp0werStohnly::SetBool(vsonyp0werID key, bool val)
{
    SetInt(key, val ? 1 : 0);
}

void vsonyp0werStohnly::SetFloat(vsonyp0werID key, float val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_f = val;
}

void vsonyp0werStohnly::SetVoidPtr(vsonyp0werID key, void* val)
{
    vsonyp0werStohnly::Pair* it = LowerBound(Data, key);
    if (it == Data.end() || it->key != key)
    {
        Data.insert(it, Pair(key, val));
        return;
    }
    it->val_p = val;
}

void vsonyp0werStohnly::SetAllInt(int v)
{
    for (int i = 0; i < Data.Size; i++)
        Data[i].val_i = v;
}

//-----------------------------------------------------------------------------
// [SECTION] vsonyp0werTextFilter
//-----------------------------------------------------------------------------

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
vsonyp0werTextFilter::vsonyp0werTextFilter(const char* default_filter)
{
    if (default_filter)
    {
        ImStrncpy(InputBuf, default_filter, IM_ARRAYSIZE(InputBuf));
        Build();
    } else
    {
        InputBuf[0] = 0;
        CountGrep = 0;
    }
}

bool vsonyp0werTextFilter::Draw(const char* label, float width)
{
    if (width != 0.0f)
        vsonyp0wer::PushItemWidth(width);
    bool value_changed = vsonyp0wer::InputText(label, InputBuf, IM_ARRAYSIZE(InputBuf));
    if (width != 0.0f)
        vsonyp0wer::PopItemWidth();
    if (value_changed)
        Build();
    return value_changed;
}

void vsonyp0werTextFilter::TextRange::split(char separator, ImVector<TextRange> * out) const
{
    out->resize(0);
    const char* wb = b;
    const char* we = wb;
    while (we < e)
    {
        if (*we == separator)
        {
            out->push_back(TextRange(wb, we));
            wb = we + 1;
        }
        we++;
    }
    if (wb != we)
        out->push_back(TextRange(wb, we));
}

void vsonyp0werTextFilter::Build()
{
    Filters.resize(0);
    TextRange input_range(InputBuf, InputBuf + strlen(InputBuf));
    input_range.split(',', &Filters);

    CountGrep = 0;
    for (int i = 0; i != Filters.Size; i++)
    {
        TextRange& f = Filters[i];
        while (f.b < f.e && ImCharIsBlankA(f.b[0]))
            f.b++;
        while (f.e > f.b && ImCharIsBlankA(f.e[-1]))
            f.e--;
        if (f.empty())
            continue;
        if (Filters[i].b[0] != '-')
            CountGrep += 1;
    }
}

bool vsonyp0werTextFilter::PassFilter(const char* text, const char* text_end) const
{
    if (Filters.empty())
        return true;

    if (text == NULL)
        text = "";

    for (int i = 0; i != Filters.Size; i++)
    {
        const TextRange& f = Filters[i];
        if (f.empty())
            continue;
        if (f.b[0] == '-')
        {
            // Subtract
            if (ImStristr(text, text_end, f.begin() + 1, f.end()) != NULL)
                return false;
        } else
        {
            // Grep
            if (ImStristr(text, text_end, f.begin(), f.end()) != NULL)
                return true;
        }
    }

    // Implicit * grep
    if (CountGrep == 0)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
// [SECTION] vsonyp0werTextBuffer
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to 2013 doesn't have it.
#ifndef va_copy
#if defined(__GNUC__) || defined(__clang__)
#define va_copy(dest, src) __builtin_va_copy(dest, src)
#else
#define va_copy(dest, src) (dest = src)
#endif
#endif

char vsonyp0werTextBuffer::EmptyString[1] = { 0 };

void vsonyp0werTextBuffer::append(const char* str, const char* str_end)
{
    int len = str_end ? (int)(str_end - str) : (int)strlen(str);

    // Add zero-terminator the first time
    const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
    const int needed_sz = write_off + len;
    if (write_off + len >= Buf.Capacity)
    {
        int new_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
    }

    Buf.resize(needed_sz);
    memcpy(&Buf[write_off - 1], str, (size_t)len);
    Buf[write_off - 1 + len] = 0;
}

void vsonyp0werTextBuffer::appendf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    appendfv(fmt, args);
    va_end(args);
}

// Helper: Text buffer for logging/accumulating text
void vsonyp0werTextBuffer::appendfv(const char* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    int len = ImFormatStringV(NULL, 0, fmt, args);         // FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
    if (len <= 0)
    {
        va_end(args_copy);
        return;
    }

    // Add zero-terminator the first time
    const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
    const int needed_sz = write_off + len;
    if (write_off + len >= Buf.Capacity)
    {
        int new_capacity = Buf.Capacity * 2;
        Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
    }

    Buf.resize(needed_sz);
    ImFormatStringV(&Buf[write_off - 1], (size_t)len + 1, fmt, args_copy);
    va_end(args_copy);
}

//-----------------------------------------------------------------------------
// [SECTION] vsonyp0werListClipper
// This is currently not as flexible/powerful as it should be, needs some rework (see TODO)
//-----------------------------------------------------------------------------

static void SetCursorPosYAndSetupDummyPrevLine(float pos_y, float line_height)
{
    // Set cursor position and a few other things so that SetScrollHereY() and Columns() can work when seeking cursor.
    // FIXME: It is problematic that we have to do that here, because custom/equivalent end-user code would stumble on the same issue.
    // The clipper should probably have a 4th step to display the last item in a regular manner.
    vsonyp0wer::SetCursorPosY(pos_y);
    vsonyp0werWindow* window = vsonyp0wer::GetCurrentWindow();
    window->DC.CursorPosPrevLine.y = window->DC.CursorPos.y - line_height;      // Setting those fields so that SetScrollHereY() can properly function after the end of our clipper usage.
    window->DC.PrevLineSize.y = (line_height - Gvsonyp0wer->Style.ItemSpacing.y);    // If we end up needing more accurate data (to e.g. use SameLine) we may as well make the clipper have a fourth step to let user process and display the last item in their list.
    if (window->DC.CurrentColumns)
        window->DC.CurrentColumns->LineMinY = window->DC.CursorPos.y;           // Setting this so that cell Y position are set properly
}

// Use case A: Begin() called from constructor with items_height<0, then called again from Sync() in StepNo 1
// Use case B: Begin() called from constructor with items_height>0
// FIXME-LEGACY: Ideally we should remove the Begin/End functions but they are part of the legacy API we still support. This is why some of the code in Step() calling Begin() and reassign some fields, spaghetti style.
void vsonyp0werListClipper::Begin(int count, float items_height)
{
    StartPosY = vsonyp0wer::GetCursorPosY();
    ItemsHeight = items_height;
    ItemsCount = count;
    StepNo = 0;
    DisplayEnd = DisplayStart = -1;
    if (ItemsHeight > 0.0f)
    {
        vsonyp0wer::CalcListClipping(ItemsCount, ItemsHeight, &DisplayStart, &DisplayEnd); // calculate how many to clip/display
        if (DisplayStart > 0)
            SetCursorPosYAndSetupDummyPrevLine(StartPosY + DisplayStart * ItemsHeight, ItemsHeight); // advance cursor
        StepNo = 2;
    }
}

void vsonyp0werListClipper::End()
{
    if (ItemsCount < 0)
        return;
    // In theory here we should assert that vsonyp0wer::GetCursorPosY() == StartPosY + DisplayEnd * ItemsHeight, but it feels saner to just seek at the end and not assert/crash the user.
    if (ItemsCount < INT_MAX)
        SetCursorPosYAndSetupDummyPrevLine(StartPosY + ItemsCount * ItemsHeight, ItemsHeight); // advance cursor
    ItemsCount = -1;
    StepNo = 3;
}

bool vsonyp0werListClipper::Step()
{
    if (ItemsCount == 0 || vsonyp0wer::GetCurrentWindowRead()->SkipItems)
    {
        ItemsCount = -1;
        return false;
    }
    if (StepNo == 0) // Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height.
    {
        DisplayStart = 0;
        DisplayEnd = 1;
        StartPosY = vsonyp0wer::GetCursorPosY();
        StepNo = 1;
        return true;
    }
    if (StepNo == 1) // Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
    {
        if (ItemsCount == 1) { ItemsCount = -1; return false; }
        float items_height = vsonyp0wer::GetCursorPosY() - StartPosY;
        IM_ASSERT(items_height > 0.0f);   // If this triggers, it means Item 0 hasn't moved the cursor vertically
        Begin(ItemsCount - 1, items_height);
        DisplayStart++;
        DisplayEnd++;
        StepNo = 3;
        return true;
    }
    if (StepNo == 2) // Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user still call Step(). Does nothing and switch to Step 3.
    {
        IM_ASSERT(DisplayStart >= 0 && DisplayEnd >= 0);
        StepNo = 3;
        return true;
    }
    if (StepNo == 3) // Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
        End();
    return false;
}

//-----------------------------------------------------------------------------
// [SECTION] RENDER HELPERS
// Those (internal) functions are currently quite a legacy mess - their signature and behavior will change.
// Also see vsonyp0wer_draw.cpp for some more which have been reworked to not rely on vsonyp0wer:: state.
//-----------------------------------------------------------------------------

const char* vsonyp0wer::FindRenderedTextEnd(const char* text, const char* text_end)
{
    const char* text_display_end = text;
    if (!text_end)
        text_end = (const char*)-1;

    while (text_display_end < text_end && *text_display_end != '\0' && (text_display_end[0] != '#' || text_display_end[1] != '#'))
        text_display_end++;
    return text_display_end;
}

// Internal vsonyp0wer functions to render text
// RenderText***() functions calls ImDrawList::AddText() calls ImBitmapFont::RenderText()
void vsonyp0wer::RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    // Hide anything after a '##' string
    const char* text_display_end;
    if (hide_text_after_hash)
    {
        text_display_end = FindRenderedTextEnd(text, text_end);
    } else
    {
        if (!text_end)
            text_end = text + strlen(text); // FIXME-OPT
        text_display_end = text_end;
    }

    if (text != text_display_end)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(vsonyp0werCol_Text), text, text_display_end);
        if (g.LogEnabled)
            LogRenderedText(&pos, text, text_display_end);
    }
}

void vsonyp0wer::RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    if (!text_end)
        text_end = text + strlen(text); // FIXME-OPT

    if (text != text_end)
    {
        window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(vsonyp0werCol_Text), text, text_end, wrap_width);
        if (g.LogEnabled)
            LogRenderedText(&pos, text, text_end);
    }
}

// Default clip_rect uses (pos_min,pos_max)
// Handle clipping on CPU immediately (vs typically let the GPU clip the triangles that are overlapping the clipping rectangle edges)
void vsonyp0wer::RenderTextClippedEx(ImDrawList * draw_list, const ImVec2 & pos_min, const ImVec2 & pos_max, const char* text, const char* text_display_end, const ImVec2 * text_size_if_known, const ImVec2 & align, const ImRect * clip_rect)
{
    // Perform CPU side clipping for single clipped element to avoid using scissor state
    ImVec2 pos = pos_min;
    const ImVec2 text_size = text_size_if_known ? *text_size_if_known : CalcTextSize(text, text_display_end, false, 0.0f);

    const ImVec2* clip_min = clip_rect ? &clip_rect->Min : &pos_min;
    const ImVec2* clip_max = clip_rect ? &clip_rect->Max : &pos_max;
    bool need_clipping = (pos.x + text_size.x >= clip_max->x) || (pos.y + text_size.y >= clip_max->y);
    if (clip_rect) // If we had no explicit clipping rectangle then pos==clip_min
        need_clipping |= (pos.x < clip_min->x) || (pos.y < clip_min->y);

    // Align whole block. We should defer that to the better rendering function when we'll have support for individual line alignment.
    if (align.x > 0.0f) pos.x = ImMax(pos.x, pos.x + (pos_max.x - pos.x - text_size.x) * align.x);
    if (align.y > 0.0f) pos.y = ImMax(pos.y, pos.y + (pos_max.y - pos.y - text_size.y) * align.y);

    // Render
    if (need_clipping)
    {
        ImVec4 fine_clip_rect(clip_min->x, clip_min->y, clip_max->x, clip_max->y);
        draw_list->AddText(NULL, 0.0f, pos, GetColorU32(vsonyp0werCol_Text), text, text_display_end, 0.0f, &fine_clip_rect);
    } else
    {
        draw_list->AddText(NULL, 0.0f, pos, GetColorU32(vsonyp0werCol_Text), text, text_display_end, 0.0f, NULL);
    }
}

void vsonyp0wer::RenderTextClipped(const ImVec2 & pos_min, const ImVec2 & pos_max, const char* text, const char* text_end, const ImVec2 * text_size_if_known, const ImVec2 & align, const ImRect * clip_rect)
{
    // Hide anything after a '##' string
    const char* text_display_end = FindRenderedTextEnd(text, text_end);
    const int text_len = (int)(text_display_end - text);
    if (text_len == 0)
        return;

    vsonyp0werContext & g = *Gvsonyp0wer;
    vsonyp0werWindow * window = g.CurrentWindow;
    RenderTextClippedEx(window->DrawList, pos_min, pos_max, text, text_display_end, text_size_if_known, align, clip_rect);
    if (g.LogEnabled)
        LogRenderedText(&pos_min, text, text_display_end);
}

// Render a rectangle shaped with optional rounding and borders
void vsonyp0wer::RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
    const float border_size = g.Style.FrameBorderSize;
    if (border && border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(vsonyp0werCol_BorderShadow), rounding, ImDrawCornerFlags_All, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(vsonyp0werCol_Border), rounding, ImDrawCornerFlags_All, border_size);
    }
}

void vsonyp0wer::RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(vsonyp0werCol_BorderShadow), rounding, ImDrawCornerFlags_All, border_size);
        window->DrawList->AddRect(p_min, p_max, GetColorU32(vsonyp0werCol_Border), rounding, ImDrawCornerFlags_All, border_size);
    }
}

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void vsonyp0wer::RenderArrow(ImVec2 p_min, vsonyp0werDir dir, float scale)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    const float h = g.FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = p_min + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 a, b, c;
    switch (dir)
    {
    case vsonyp0werDir_Up:
    case vsonyp0werDir_Down:
        if (dir == vsonyp0werDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.750f) * r;
        b = ImVec2(-0.866f, -0.750f) * r;
        c = ImVec2(+0.866f, -0.750f) * r;
        break;
    case vsonyp0werDir_Left:
    case vsonyp0werDir_Right:
        if (dir == vsonyp0werDir_Left) r = -r;
        a = ImVec2(+0.750f, +0.000f) * r;
        b = ImVec2(-0.750f, +0.866f) * r;
        c = ImVec2(-0.750f, -0.866f) * r;
        break;
    case vsonyp0werDir_None:
    case vsonyp0werDir_COUNT:
        IM_ASSERT(0);
        break;
    }

    g.CurrentWindow->DrawList->AddTriangleFilled(center + a, center + b, center + c, GetColorU32(vsonyp0werCol_Text));
}

void vsonyp0wer::RenderBullet(ImVec2 pos)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    window->DrawList->AddCircleFilled(pos, g.FontSize * 0.20f, GetColorU32(vsonyp0werCol_Text), 8);
}

void vsonyp0wer::RenderCheckMark(ImVec2 pos, ImU32 col, float sz)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    float thickness = ImMax(sz / 5.0f, 1.0f);
    sz -= thickness * 0.5f;
    pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

    float third = sz / 3.0f;
    float bx = pos.x + third;
    float by = pos.y + sz - third * 0.5f;
    window->DrawList->PathLineTo(ImVec2(bx - third, by - third));
    window->DrawList->PathLineTo(ImVec2(bx, by));
    window->DrawList->PathLineTo(ImVec2(bx + third * 2, by - third * 2));
    window->DrawList->PathStroke(col, false, thickness);
}

void vsonyp0wer::RenderNavHighlight(const ImRect & bb, vsonyp0werID id, vsonyp0werNavHighlightFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (id != g.NavId)
        return;
    if (g.NavDisableHighlight && !(flags & vsonyp0werNavHighlightFlags_AlwaysDraw))
        return;
    vsonyp0werWindow * window = g.CurrentWindow;
    if (window->DC.NavHideHighlightOneFrame)
        return;

    float rounding = (flags & vsonyp0werNavHighlightFlags_NoRounding) ? 0.0f : g.Style.FrameRounding;
    ImRect display_rect = bb;
    display_rect.ClipWith(window->ClipRect);
    if (flags & vsonyp0werNavHighlightFlags_TypeDefault)
    {
        const float THICKNESS = 2.0f;
        const float DISTANCE = 3.0f + THICKNESS * 0.5f;
        display_rect.Expand(ImVec2(DISTANCE, DISTANCE));
        bool fully_visible = window->ClipRect.Contains(display_rect);
        if (!fully_visible)
            window->DrawList->PushClipRect(display_rect.Min, display_rect.Max);
        window->DrawList->AddRect(display_rect.Min + ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f), display_rect.Max - ImVec2(THICKNESS * 0.5f, THICKNESS * 0.5f), GetColorU32(vsonyp0werCol_NavHighlight), rounding, ImDrawCornerFlags_All, THICKNESS);
        if (!fully_visible)
            window->DrawList->PopClipRect();
    }
    if (flags & vsonyp0werNavHighlightFlags_TypeThin)
    {
        window->DrawList->AddRect(display_rect.Min, display_rect.Max, GetColorU32(vsonyp0werCol_NavHighlight), rounding, ~0, 1.0f);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
//-----------------------------------------------------------------------------

// vsonyp0werWindow is mostly a dumb struct. It merely has a constructor and a few helper methods
vsonyp0werWindow::vsonyp0werWindow(vsonyp0werContext * context, const char* name)
    : DrawListInst(&context->DrawListSharedData)
{
    Name = ImStrdup(name);
    ID = ImHashStr(name, 0);
    IDStack.push_back(ID);
    Flags = vsonyp0werWindowFlags_None;
    Pos = ImVec2(0.0f, 0.0f);
    Size = SizeFull = ImVec2(0.0f, 0.0f);
    SizeContents = SizeContentsExplicit = ImVec2(0.0f, 0.0f);
    WindowPadding = ImVec2(0.0f, 0.0f);
    WindowRounding = 0.0f;
    WindowBorderSize = 0.0f;
    NameBufLen = (int)strlen(name) + 1;
    MoveId = GetID("#MOVE");
    ChildId = 0;
    Scroll = ImVec2(0.0f, 0.0f);
    ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);
    ScrollTargetCenterRatio = ImVec2(0.5f, 0.5f);
    ScrollbarSizes = ImVec2(0.0f, 0.0f);
    ScrollbarX = ScrollbarY = false;
    Active = WasActive = false;
    WriteAccessed = false;
    Collapsed = false;
    WantCollapseToggle = false;
    SkipItems = false;
    Appearing = false;
    Hidden = false;
    HasCloseButton = false;
    ResizeBorderHeld = -1;
    BeginCount = 0;
    BeginOrderWithinParent = -1;
    BeginOrderWithinContext = -1;
    PopupId = 0;
    AutoFitFramesX = AutoFitFramesY = -1;
    AutoFitOnlyGrows = false;
    AutoFitChildAxises = 0x00;
    AutoPosLastDirection = vsonyp0werDir_None;
    HiddenFramesCanSkipItems = HiddenFramesCannotSkipItems = 0;
    SetWindowPosAllowFlags = SetWindowSizeAllowFlags = SetWindowCollapsedAllowFlags = vsonyp0werCond_Always | vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing;
    SetWindowPosVal = SetWindowPosPivot = ImVec2(FLT_MAX, FLT_MAX);

    LastFrameActive = -1;
    ItemWidthDefault = 0.0f;
    FontWindowScale = 1.0f;
    SettingsIdx = -1;

    DrawList = &DrawListInst;
    DrawList->_OwnerName = Name;
    ParentWindow = NULL;
    RootWindow = NULL;
    RootWindowForTitleBarHighlight = NULL;
    RootWindowForNav = NULL;

    NavLastIds[0] = NavLastIds[1] = 0;
    NavRectRel[0] = NavRectRel[1] = ImRect();
    NavLastChildNavWindow = NULL;
}

vsonyp0werWindow::~vsonyp0werWindow()
{
    IM_ASSERT(DrawList == &DrawListInst);
    IM_DELETE(Name);
    for (int i = 0; i != ColumnsStohnly.Size; i++)
        ColumnsStohnly[i].~vsonyp0werColumns();
}

vsonyp0werID vsonyp0werWindow::GetID(const char* str, const char* str_end)
{
    vsonyp0werID seed = IDStack.back();
    vsonyp0werID id = ImHashStr(str, str_end ? (str_end - str) : 0, seed);
    vsonyp0wer::KeepAliveID(id);
    return id;
}

vsonyp0werID vsonyp0werWindow::GetID(const void* ptr)
{
    vsonyp0werID seed = IDStack.back();
    vsonyp0werID id = ImHashData(&ptr, sizeof(void*), seed);
    vsonyp0wer::KeepAliveID(id);
    return id;
}

vsonyp0werID vsonyp0werWindow::GetIDNoKeepAlive(const char* str, const char* str_end)
{
    vsonyp0werID seed = IDStack.back();
    return ImHashStr(str, str_end ? (str_end - str) : 0, seed);
}

vsonyp0werID vsonyp0werWindow::GetIDNoKeepAlive(const void* ptr)
{
    vsonyp0werID seed = IDStack.back();
    return ImHashData(&ptr, sizeof(void*), seed);
}

// This is only used in rare/specific situations to manufacture an ID out of nowhere.
vsonyp0werID vsonyp0werWindow::GetIDFromRectangle(const ImRect & r_abs)
{
    vsonyp0werID seed = IDStack.back();
    const int r_rel[4] = { (int)(r_abs.Min.x - Pos.x), (int)(r_abs.Min.y - Pos.y), (int)(r_abs.Max.x - Pos.x), (int)(r_abs.Max.y - Pos.y) };
    vsonyp0werID id = ImHashData(&r_rel, sizeof(r_rel), seed);
    vsonyp0wer::KeepAliveID(id);
    return id;
}

static void SetCurrentWindow(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.CurrentWindow = window;
    if (window)
        g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

void vsonyp0wer::SetNavID(vsonyp0werID id, int nav_layer)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.NavWindow);
    IM_ASSERT(nav_layer == 0 || nav_layer == 1);
    g.NavId = id;
    g.NavWindow->NavLastIds[nav_layer] = id;
}

void vsonyp0wer::SetNavIDWithRectRel(vsonyp0werID id, int nav_layer, const ImRect & rect_rel)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    SetNavID(id, nav_layer);
    g.NavWindow->NavRectRel[nav_layer] = rect_rel;
    g.NavMousePosDirty = true;
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = true;
}

void vsonyp0wer::SetActiveID(vsonyp0werID id, vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.ActiveIdIsJustActivated = (g.ActiveId != id);
    if (g.ActiveIdIsJustActivated)
    {
        g.ActiveIdTimer = 0.0f;
        g.ActiveIdHasBeenPressed = false;
        g.ActiveIdHasBeenEdited = false;
        if (id != 0)
        {
            g.LastActiveId = id;
            g.LastActiveIdTimer = 0.0f;
        }
    }
    g.ActiveId = id;
    g.ActiveIdAllowNavDirFlags = 0;
    g.ActiveIdBlockNavInputFlags = 0;
    g.ActiveIdAllowOverlap = false;
    g.ActiveIdWindow = window;
    if (id)
    {
        g.ActiveIdIsAlive = id;
        g.ActiveIdSource = (g.NavActivateId == id || g.NavInputId == id || g.NavJustTabbedId == id || g.NavJustMovedToId == id) ? vsonyp0werInputSource_Nav : vsonyp0werInputSource_Mouse;
    }
}

// FIXME-NAV: The existence of SetNavID/SetNavIDWithRectRel/SetFocusID is incredibly messy and confusing and needs some explanation or refactoring.
void vsonyp0wer::SetFocusID(vsonyp0werID id, vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(id != 0);

    // Assume that SetFocusID() is called in the context where its NavLayer is the current layer, which is the case everywhere we call it.
    const vsonyp0werNavLayer nav_layer = window->DC.NavLayerCurrent;
    if (g.NavWindow != window)
        g.NavInitRequest = false;
    g.NavId = id;
    g.NavWindow = window;
    g.NavLayer = nav_layer;
    window->NavLastIds[nav_layer] = id;
    if (window->DC.LastItemId == id)
        window->NavRectRel[nav_layer] = ImRect(window->DC.LastItemRect.Min - window->Pos, window->DC.LastItemRect.Max - window->Pos);

    if (g.ActiveIdSource == vsonyp0werInputSource_Nav)
        g.NavDisableMouseHover = true;
    else
        g.NavDisableHighlight = true;
}

void vsonyp0wer::ClearActiveID()
{
    SetActiveID(0, NULL);
}

void vsonyp0wer::SetHoveredID(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.HoveredId = id;
    g.HoveredIdAllowOverlap = false;
    if (id != 0 && g.HoveredIdPreviousFrame != id)
        g.HoveredIdTimer = g.HoveredIdNotActiveTimer = 0.0f;
}

vsonyp0werID vsonyp0wer::GetHoveredID()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.HoveredId ? g.HoveredId : g.HoveredIdPreviousFrame;
}

void vsonyp0wer::KeepAliveID(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.ActiveId == id)
        g.ActiveIdIsAlive = id;
    if (g.ActiveIdPreviousFrame == id)
        g.ActiveIdPreviousFrameIsAlive = true;
}

void vsonyp0wer::MarkItemEdited(vsonyp0werID id)
{
    // This marking is solely to be able to provide info for IsItemDeactivatedAfterEdit().
    // ActiveId might have been released by the time we call this (as in the typical press/release button behavior) but still need need to fill the data.
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.ActiveId == id || g.ActiveId == 0 || g.DragDropActive);
    IM_UNUSED(id); // Avoid unused variable warnings when asserts are compiled out.
    //IM_ASSERT(g.CurrentWindow->DC.LastItemId == id);
    g.ActiveIdHasBeenEdited = true;
    g.CurrentWindow->DC.LastItemStatusFlags |= vsonyp0werItemStatusFlags_Edited;
}

static inline bool IsWindowContentHoverable(vsonyp0werWindow * window, vsonyp0werHoveredFlags flags)
{
    // An active popup disable hovering on other windows (apart from its own children)
    // FIXME-OPT: This could be cached/stored within the window.
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NavWindow)
        if (vsonyp0werWindow * focused_root_window = g.NavWindow->RootWindow)
            if (focused_root_window->WasActive && focused_root_window != window->RootWindow)
            {
                // For the purpose of those flags we differentiate "standard popup" from "modal popup"
                // NB: The order of those two tests is important because Modal windows are also Popups.
                if (focused_root_window->Flags & vsonyp0werWindowFlags_Modal)
                    return false;
                if ((focused_root_window->Flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werHoveredFlags_AllowWhenBlockedByPopup))
                    return false;
            }

    return true;
}

// Advance cursor given item size for layout.
void vsonyp0wer::ItemSize(const ImVec2 & size, float text_offset_y)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    // Always align ourselves on pixel boundaries
    const float line_height = ImMax(window->DC.CurrentLineSize.y, size.y);
    const float text_base_offset = ImMax(window->DC.CurrentLineTextBaseOffset, text_offset_y);
    //if (g.IO.KeyAlt) window->DrawList->AddRect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(size.x, line_height), IM_COL32(255,0,0,200)); // [DEBUG]
    window->DC.CursorPosPrevLine = ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y);
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
    window->DC.CursorPos.y = (float)(int)(window->DC.CursorPos.y + line_height + g.Style.ItemSpacing.y);
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPosPrevLine.x);
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y - g.Style.ItemSpacing.y);
    //if (g.IO.KeyAlt) window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.0f, IM_COL32(255,0,0,255), 4); // [DEBUG]

    window->DC.PrevLineSize.y = line_height;
    window->DC.PrevLineTextBaseOffset = text_base_offset;
    window->DC.CurrentLineSize.y = window->DC.CurrentLineTextBaseOffset = 0.0f;

    // Horizontal layout mode
    if (window->DC.LayoutType == vsonyp0werLayoutType_Horizontal)
        SameLine();
}

void vsonyp0wer::ItemSize(const ImRect & bb, float text_offset_y)
{
    ItemSize(bb.GetSize(), text_offset_y);
}

// Declare item bounding box for clipping and interaction.
// Note that the size can be different than the one provided to ItemSize(). Typically, widgets that spread over available surface
// declare their minimum size requirement to ItemSize() and then use a larger region for drawing/interaction, which is passed to ItemAdd().
bool vsonyp0wer::ItemAdd(const ImRect & bb, vsonyp0werID id, const ImRect * nav_bb_arg)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    if (id != 0)
    {
        // Navigation processing runs prior to clipping early-out
        //  (a) So that NavInitRequest can be honored, for newly opened windows to select a default widget
        //  (b) So that we can scroll up/down past clipped items. This adds a small O(N) cost to regular navigation requests unfortunately, but it is still limited to one window.
        //      it may not scale very well for windows with ten of thousands of item, but at least NavMoveRequest is only set on user interaction, aka maximum once a frame.
        //      We could early out with "if (is_clipped && !g.NavInitRequest) return false;" but when we wouldn't be able to reach unclipped widgets. This would work if user had explicit scrolling control (e.g. mapped on a stick)
        window->DC.NavLayerActiveMaskNext |= window->DC.NavLayerCurrentMask;
        if (g.NavId == id || g.NavAnyRequest)
            if (g.NavWindow->RootWindowForNav == window->RootWindowForNav)
                if (window == g.NavWindow || ((window->Flags | g.NavWindow->Flags) & vsonyp0werWindowFlags_NavFlattened))
                    NavProcessItem(window, nav_bb_arg ? *nav_bb_arg : bb, id);
    }

    window->DC.LastItemId = id;
    window->DC.LastItemRect = bb;
    window->DC.LastItemStatusFlags = vsonyp0werItemStatusFlags_None;

#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
    if (id != 0)
        vsonyp0wer_TEST_ENGINE_ITEM_ADD(nav_bb_arg ? *nav_bb_arg : bb, id);
#endif

    // Clipping test
    const bool is_clipped = IsClippedEx(bb, id, false);
    if (is_clipped)
        return false;
    //if (g.IO.KeyAlt) window->DrawList->AddRect(bb.Min, bb.Max, IM_COL32(255,255,0,120)); // [DEBUG]

    // We need to calculate this now to take account of the current clipping rectangle (as items like Selectable may change them)
    if (IsMouseHoveringRect(bb.Min, bb.Max))
        window->DC.LastItemStatusFlags |= vsonyp0werItemStatusFlags_HoveredRect;
    return true;
}

// This is roughly matching the behavior of internal-facing ItemHoverable()
// - we allow hovering to be true when ActiveId==window->MoveID, so that clicking on non-interactive items such as a Text() item still returns true with IsItemHovered()
// - this should work even for non-interactive items that have no ID, so we cannot use LastItemId
bool vsonyp0wer::IsItemHovered(vsonyp0werHoveredFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (g.NavDisableMouseHover && !g.NavDisableHighlight)
        return IsItemFocused();

    // Test for bounding box overlap, as updated as ItemAdd()
    if (!(window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_HoveredRect))
        return false;
    IM_ASSERT((flags & (vsonyp0werHoveredFlags_RootWindow | vsonyp0werHoveredFlags_ChildWindows)) == 0);   // Flags not supported by this function

    // Test if we are hovering the right window (our window could be behind another window)
    // [2017/10/16] Reverted commit 344d48be3 and testing RootWindow instead. I believe it is correct to NOT test for RootWindow but this leaves us unable to use IsItemHovered() after EndChild() itself.
    // Until a solution is found I believe reverting to the test from 2017/09/27 is safe since this was the test that has been running for a long while.
    //if (g.HoveredWindow != window)
    //    return false;
    if (g.HoveredRootWindow != window->RootWindow && !(flags & vsonyp0werHoveredFlags_AllowWhenOverlapped))
        return false;

    // Test if another item is active (e.g. being dragged)
    if (!(flags & vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem))
        if (g.ActiveId != 0 && g.ActiveId != window->DC.LastItemId && !g.ActiveIdAllowOverlap && g.ActiveId != window->MoveId)
            return false;

    // Test if interactions on this window are blocked by an active popup or modal
    if (!IsWindowContentHoverable(window, flags))
        return false;

    // Test if the item is disabled
    if ((window->DC.ItemFlags & vsonyp0werItemFlags_Disabled) && !(flags & vsonyp0werHoveredFlags_AllowWhenDisabled))
        return false;

    // Special handling for the dummy item after Begin() which represent the title bar or tab.
    // When the window is collapsed (SkipItems==true) that last item will never be overwritten so we need to detect the case.
    if (window->DC.LastItemId == window->MoveId && window->WriteAccessed)
        return false;
    return true;
}

// Internal facing ItemHoverable() used when submitting widgets. Differs slightly from IsItemHovered().
bool vsonyp0wer::ItemHoverable(const ImRect & bb, vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.HoveredId != 0 && g.HoveredId != id && !g.HoveredIdAllowOverlap)
        return false;

    vsonyp0werWindow * window = g.CurrentWindow;
    if (g.HoveredWindow != window)
        return false;
    if (g.ActiveId != 0 && g.ActiveId != id && !g.ActiveIdAllowOverlap)
        return false;
    if (!IsMouseHoveringRect(bb.Min, bb.Max))
        return false;
    if (g.NavDisableMouseHover || !IsWindowContentHoverable(window, vsonyp0werHoveredFlags_None))
        return false;
    if (window->DC.ItemFlags & vsonyp0werItemFlags_Disabled)
        return false;

    SetHoveredID(id);
    return true;
}

bool vsonyp0wer::IsClippedEx(const ImRect & bb, vsonyp0werID id, bool clip_even_when_logged)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (!bb.Overlaps(window->ClipRect))
        if (id == 0 || id != g.ActiveId)
            if (clip_even_when_logged || !g.LogEnabled)
                return true;
    return false;
}

// Process TAB/Shift+TAB. Be mindful that this function may _clear_ the ActiveID when tabbing out.
bool vsonyp0wer::FocusableItemRegister(vsonyp0werWindow * window, vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Increment counters
    const bool is_tab_stop = (window->DC.ItemFlags & (vsonyp0werItemFlags_NoTabStop | vsonyp0werItemFlags_Disabled)) == 0;
    window->DC.FocusCounterAll++;
    if (is_tab_stop)
        window->DC.FocusCounterTab++;

    // Process TAB/Shift-TAB to tab *OUT* of the currently focused item.
    // (Note that we can always TAB out of a widget that doesn't allow tabbing in)
    if (g.ActiveId == id && g.FocusTabPressed && !(g.ActiveIdBlockNavInputFlags & (1 << vsonyp0werNavInput_KeyTab_)) && g.FocusRequestNextWindow == NULL)
    {
        g.FocusRequestNextWindow = window;
        g.FocusRequestNextCounterTab = window->DC.FocusCounterTab + (g.IO.KeyShift ? (is_tab_stop ? -1 : 0) : +1); // Modulo on index will be applied at the end of frame once we've got the total counter of items.
    }

    // Handle focus requests
    if (g.FocusRequestCurrWindow == window)
    {
        if (window->DC.FocusCounterAll == g.FocusRequestCurrCounterAll)
            return true;
        if (is_tab_stop && window->DC.FocusCounterTab == g.FocusRequestCurrCounterTab)
        {
            g.NavJustTabbedId = id;
            return true;
        }

        // If another item is about to be focused, we clear our own active id
        if (g.ActiveId == id)
            ClearActiveID();
    }

    return false;
}

void vsonyp0wer::FocusableItemUnregister(vsonyp0werWindow * window)
{
    window->DC.FocusCounterAll--;
    window->DC.FocusCounterTab--;
}

float vsonyp0wer::CalcWrapWidthForPos(const ImVec2 & pos, float wrap_pos_x)
{
    if (wrap_pos_x < 0.0f)
        return 0.0f;

    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    if (wrap_pos_x == 0.0f)
        wrap_pos_x = GetContentRegionMaxScreen().x;
    else if (wrap_pos_x > 0.0f)
        wrap_pos_x += window->Pos.x - window->Scroll.x; // wrap_pos_x is provided is window local space

    return ImMax(wrap_pos_x - pos.x, 1.0f);
}

// IM_ALLOC() == vsonyp0wer::MemAlloc()
void* vsonyp0wer::MemAlloc(size_t size)
{
    if (vsonyp0werContext * ctx = Gvsonyp0wer)
        ctx->IO.MetricsActiveAllocations++;
    return GImAllocatorAllocFunc(size, GImAllocatorUserData);
}

// IM_FREE() == vsonyp0wer::MemFree()
void vsonyp0wer::MemFree(void* ptr)
{
    if (ptr)
        if (vsonyp0werContext * ctx = Gvsonyp0wer)
            ctx->IO.MetricsActiveAllocations--;
    return GImAllocatorFreeFunc(ptr, GImAllocatorUserData);
}

const char* vsonyp0wer::GetClipboardText()
{
    return Gvsonyp0wer->IO.GetClipboardTextFn ? Gvsonyp0wer->IO.GetClipboardTextFn(Gvsonyp0wer->IO.ClipboardUserData) : "";
}

void vsonyp0wer::SetClipboardText(const char* text)
{
    if (Gvsonyp0wer->IO.SetClipboardTextFn)
        Gvsonyp0wer->IO.SetClipboardTextFn(Gvsonyp0wer->IO.ClipboardUserData, text);
}

const char* vsonyp0wer::GetVersion()
{
    return vsonyp0wer_VERSION;
}

// Internal state access - if you want to share vsonyp0wer state between modules (e.g. DLL) or allocate it yourself
// Note that we still point to some static data and members (such as GFontAtlas), so the state instance you end up using will point to the static data within its module
vsonyp0werContext* vsonyp0wer::GetCurrentContext()
{
    return Gvsonyp0wer;
}

void vsonyp0wer::SetCurrentContext(vsonyp0werContext * ctx)
{
#ifdef vsonyp0wer_SET_CURRENT_CONTEXT_FUNC
    vsonyp0wer_SET_CURRENT_CONTEXT_FUNC(ctx); // For custom thread-based hackery you may want to have control over this.
#else
    Gvsonyp0wer = ctx;
#endif
}

// Helper function to verify that the type sizes are matching between the calling file's compilation unit and vsonyp0wer.cpp's compilation unit
// If the user has inconsistent compilation settings, vsonyp0wer configuration #define, packing pragma, etc. you may see different structures from what vsonyp0wer.cpp sees which is highly problematic.
bool vsonyp0wer::DebugCheckVersionAndDataLayout(const char* version, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_vert)
{
    bool error = false;
    if (strcmp(version, vsonyp0wer_VERSION) != 0) { error = true; IM_ASSERT(strcmp(version, vsonyp0wer_VERSION) == 0 && "Mismatched version string!"); }
    if (sz_io != sizeof(vsonyp0werIO)) { error = true; IM_ASSERT(sz_io == sizeof(vsonyp0werIO) && "Mismatched struct layout!"); }
    if (sz_style != sizeof(vsonyp0werStyle)) { error = true; IM_ASSERT(sz_style == sizeof(vsonyp0werStyle) && "Mismatched struct layout!"); }
    if (sz_vec2 != sizeof(ImVec2)) { error = true; IM_ASSERT(sz_vec2 == sizeof(ImVec2) && "Mismatched struct layout!"); }
    if (sz_vec4 != sizeof(ImVec4)) { error = true; IM_ASSERT(sz_vec4 == sizeof(ImVec4) && "Mismatched struct layout!"); }
    if (sz_vert != sizeof(ImDrawVert)) { error = true; IM_ASSERT(sz_vert == sizeof(ImDrawVert) && "Mismatched struct layout!"); }
    return !error;
}

void vsonyp0wer::SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data)
{
    GImAllocatorAllocFunc = alloc_func;
    GImAllocatorFreeFunc = free_func;
    GImAllocatorUserData = user_data;
}

vsonyp0werContext* vsonyp0wer::CreateContext(ImFontAtlas * shared_font_atlas)
{
    vsonyp0werContext* ctx = IM_NEW(vsonyp0werContext)(shared_font_atlas);
    if (Gvsonyp0wer == NULL)
        SetCurrentContext(ctx);
    Initialize(ctx);
    return ctx;
}

void vsonyp0wer::DestroyContext(vsonyp0werContext * ctx)
{
    if (ctx == NULL)
        ctx = Gvsonyp0wer;
    Shutdown(ctx);
    if (Gvsonyp0wer == ctx)
        SetCurrentContext(NULL);
    IM_DELETE(ctx);
}

vsonyp0werIO & vsonyp0wer::GetIO()
{
    IM_ASSERT(Gvsonyp0wer != NULL && "No current context. Did you call vsonyp0wer::CreateContext() and vsonyp0wer::SetCurrentContext() ?");
    return Gvsonyp0wer->IO;
}

vsonyp0werStyle& vsonyp0wer::GetStyle()
{
    IM_ASSERT(Gvsonyp0wer != NULL && "No current context. Did you call vsonyp0wer::CreateContext() and vsonyp0wer::SetCurrentContext() ?");
    return Gvsonyp0wer->Style;
}

// Same value as passed to the old io.RenderDrawListsFn function. Valid after Render() and until the next call to NewFrame()
ImDrawData* vsonyp0wer::GetDrawData()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.DrawData.Valid ? &g.DrawData : NULL;
}

double vsonyp0wer::GetTime()
{
    return Gvsonyp0wer->Time;
}

int vsonyp0wer::GetFrameCount()
{
    return Gvsonyp0wer->FrameCount;
}

ImDrawList* vsonyp0wer::GetBackgroundDrawList()
{
    return &Gvsonyp0wer->BackgroundDrawList;
}

static ImDrawList* GetForegroundDrawList(vsonyp0werWindow*)
{
    // This seemingly unnecessary wrapper simplifies compatibility between the 'master' and 'docking' branches.
    return &Gvsonyp0wer->ForegroundDrawList;
}

ImDrawList* vsonyp0wer::GetForegroundDrawList()
{
    return &Gvsonyp0wer->ForegroundDrawList;
}

ImDrawListSharedData* vsonyp0wer::GetDrawListSharedData()
{
    return &Gvsonyp0wer->DrawListSharedData;
}

void vsonyp0wer::StartMouseMovingWindow(vsonyp0werWindow * window)
{
    // Set ActiveId even if the _NoMove flag is set. Without it, dragging away from a window with _NoMove would activate hover on other windows.
    // We _also_ call this when clicking in a window empty space when io.ConfigWindowsMoveFromTitleBarOnly is set, but clear g.MovingWindow afterward.
    // This is because we want ActiveId to be set even when the window is not permitted to move.
    vsonyp0werContext& g = *Gvsonyp0wer;
    FocusWindow(window);
    SetActiveID(window->MoveId, window);
    g.NavDisableHighlight = true;
    g.ActiveIdClickOffset = g.IO.MousePos - window->RootWindow->Pos;

    bool can_move_window = true;
    if ((window->Flags & vsonyp0werWindowFlags_NoMove) || (window->RootWindow->Flags & vsonyp0werWindowFlags_NoMove))
        can_move_window = false;
    if (can_move_window)
        g.MovingWindow = window;
}

// Handle mouse moving window
// Note: moving window with the navigation keys (Square + d-pad / CTRL+TAB + Arrows) are processed in NavUpdateWindowing()
void vsonyp0wer::UpdateMouseMovingWindowNewFrame()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.MovingWindow != NULL)
    {
        // We actually want to move the root window. g.MovingWindow == window we clicked on (could be a child window).
        // We track it to preserve Focus and so that generally ActiveIdWindow == MovingWindow and ActiveId == MovingWindow->MoveId for consistency.
        KeepAliveID(g.ActiveId);
        IM_ASSERT(g.MovingWindow && g.MovingWindow->RootWindow);
        vsonyp0werWindow* moving_window = g.MovingWindow->RootWindow;
        if (g.IO.MouseDown[0] && IsMousePosValid(&g.IO.MousePos))
        {
            ImVec2 pos = g.IO.MousePos - g.ActiveIdClickOffset;
            if (moving_window->Pos.x != pos.x || moving_window->Pos.y != pos.y)
            {
                MarkIniSettingsDirty(moving_window);
                SetWindowPos(moving_window, pos, vsonyp0werCond_Always);
            }
            FocusWindow(g.MovingWindow);
        } else
        {
            ClearActiveID();
            g.MovingWindow = NULL;
        }
    } else
    {
        // When clicking/dragging from a window that has the _NoMove flag, we still set the ActiveId in order to prevent hovering others.
        if (g.ActiveIdWindow && g.ActiveIdWindow->MoveId == g.ActiveId)
        {
            KeepAliveID(g.ActiveId);
            if (!g.IO.MouseDown[0])
                ClearActiveID();
        }
    }
}

// Initiate moving window, handle left-click and right-click focus
void vsonyp0wer::UpdateMouseMovingWindowEndFrame()
{
    // Initiate moving window
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.ActiveId != 0 || g.HoveredId != 0)
        return;

    // Unless we just made a window/popup appear
    if (g.NavWindow && g.NavWindow->Appearing)
        return;

    // Click to focus window and start moving (after we're done with all our widgets)
    if (g.IO.MouseClicked[0])
    {
        if (g.HoveredRootWindow != NULL)
        {
            StartMouseMovingWindow(g.HoveredWindow);
            if (g.IO.ConfigWindowsMoveFromTitleBarOnly && !(g.HoveredRootWindow->Flags & vsonyp0werWindowFlags_NoTitleBar))
                if (!g.HoveredRootWindow->TitleBarRect().Contains(g.IO.MouseClickedPos[0]))
                    g.MovingWindow = NULL;
        } else if (g.NavWindow != NULL && GetFrontMostPopupModal() == NULL)
        {
            // Clicking on void disable focus
            FocusWindow(NULL);
        }
    }

    // With right mouse button we close popups without changing focus
    // (The left mouse button path calls FocusWindow which will lead NewFrame->ClosePopupsOverWindow to trigger)
    if (g.IO.MouseClicked[1])
    {
        // Find the top-most window between HoveredWindow and the front most Modal Window.
        // This is where we can trim the popup stack.
        vsonyp0werWindow* modal = GetFrontMostPopupModal();
        bool hovered_window_above_modal = false;
        if (modal == NULL)
            hovered_window_above_modal = true;
        for (int i = g.Windows.Size - 1; i >= 0 && hovered_window_above_modal == false; i--)
        {
            vsonyp0werWindow* window = g.Windows[i];
            if (window == modal)
                break;
            if (window == g.HoveredWindow)
                hovered_window_above_modal = true;
        }
        ClosePopupsOverWindow(hovered_window_above_modal ? g.HoveredWindow : modal);
    }
}

static bool IsWindowActiveAndVisible(vsonyp0werWindow * window)
{
    return (window->Active) && (!window->Hidden);
}

static void vsonyp0wer::UpdateMouseInputs()
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Round mouse position to avoid spreading non-rounded position (e.g. UpdateManualResize doesn't support them well)
    if (IsMousePosValid(&g.IO.MousePos))
        g.IO.MousePos = g.LastValidMousePos = ImFloor(g.IO.MousePos);

    // If mouse just appeared or disappeared (usually denoted by -FLT_MAX components) we cancel out movement in MouseDelta
    if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MousePosPrev))
        g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
    else
        g.IO.MouseDelta = ImVec2(0.0f, 0.0f);
    if (g.IO.MouseDelta.x != 0.0f || g.IO.MouseDelta.y != 0.0f)
        g.NavDisableMouseHover = false;

    g.IO.MousePosPrev = g.IO.MousePos;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        g.IO.MouseClicked[i] = g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] < 0.0f;
        g.IO.MouseReleased[i] = !g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] >= 0.0f;
        g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
        g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ? (g.IO.MouseDownDuration[i] < 0.0f ? 0.0f : g.IO.MouseDownDuration[i] + g.IO.DeltaTime) : -1.0f;
        g.IO.MouseDoubleClicked[i] = false;
        if (g.IO.MouseClicked[i])
        {
            if ((float)(g.Time - g.IO.MouseClickedTime[i]) < g.IO.MouseDoubleClickTime)
            {
                ImVec2 delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ? (g.IO.MousePos - g.IO.MouseClickedPos[i]) : ImVec2(0.0f, 0.0f);
                if (ImLengthSqr(delta_from_click_pos) < g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
                    g.IO.MouseDoubleClicked[i] = true;
                g.IO.MouseClickedTime[i] = -FLT_MAX;    // so the third click isn't turned into a double-click
            } else
            {
                g.IO.MouseClickedTime[i] = g.Time;
            }
            g.IO.MouseClickedPos[i] = g.IO.MousePos;
            g.IO.MouseDragMaxDistanceAbs[i] = ImVec2(0.0f, 0.0f);
            g.IO.MouseDragMaxDistanceSqr[i] = 0.0f;
        } else if (g.IO.MouseDown[i])
        {
            // Maintain the maximum distance we reaching from the initial click position, which is used with dragging threshold
            ImVec2 delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ? (g.IO.MousePos - g.IO.MouseClickedPos[i]) : ImVec2(0.0f, 0.0f);
            g.IO.MouseDragMaxDistanceSqr[i] = ImMax(g.IO.MouseDragMaxDistanceSqr[i], ImLengthSqr(delta_from_click_pos));
            g.IO.MouseDragMaxDistanceAbs[i].x = ImMax(g.IO.MouseDragMaxDistanceAbs[i].x, delta_from_click_pos.x < 0.0f ? -delta_from_click_pos.x : delta_from_click_pos.x);
            g.IO.MouseDragMaxDistanceAbs[i].y = ImMax(g.IO.MouseDragMaxDistanceAbs[i].y, delta_from_click_pos.y < 0.0f ? -delta_from_click_pos.y : delta_from_click_pos.y);
        }
        if (g.IO.MouseClicked[i]) // Clicking any mouse button reactivate mouse hovering which may have been deactivated by gamepad/keyboard navigation
            g.NavDisableMouseHover = false;
    }
}

void vsonyp0wer::UpdateMouseWheel()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!g.HoveredWindow || g.HoveredWindow->Collapsed)
        return;
    if (g.IO.MouseWheel == 0.0f && g.IO.MouseWheelH == 0.0f)
        return;
    vsonyp0werWindow * window = g.HoveredWindow;

    // Zoom / Scale window
    // FIXME-OBSOLETE: This is an old feature, it still works but pretty much nobody is using it and may be best redesigned.
    if (g.IO.MouseWheel != 0.0f && g.IO.KeyCtrl && g.IO.FontAllowUserScaling)
    {
        const float new_font_scale = ImClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);
        const float scale = new_font_scale / window->FontWindowScale;
        window->FontWindowScale = new_font_scale;
        if (!(window->Flags & vsonyp0werWindowFlags_ChildWindow))
        {
            const ImVec2 offset = window->Size * (1.0f - scale) * (g.IO.MousePos - window->Pos) / window->Size;
            window->Pos = ImFloor(window->Pos + offset);
            window->Size = ImFloor(window->Size * scale);
            window->SizeFull = ImFloor(window->SizeFull * scale);
        }
        return;
    }

    // Mouse wheel scrolling
    // If a child window has the vsonyp0werWindowFlags_NoScrollWithMouse flag, we give a chance to scroll its parent (unless either vsonyp0werWindowFlags_NoInputs or vsonyp0werWindowFlags_NoScrollbar are also set).
    while ((window->Flags & vsonyp0werWindowFlags_ChildWindow) && (window->Flags & vsonyp0werWindowFlags_NoScrollWithMouse) && !(window->Flags & vsonyp0werWindowFlags_NoScrollbar) && !(window->Flags & vsonyp0werWindowFlags_NoMouseInputs) && window->ParentWindow)
        window = window->ParentWindow;
    const bool scroll_allowed = !(window->Flags & vsonyp0werWindowFlags_NoScrollWithMouse) && !(window->Flags & vsonyp0werWindowFlags_NoMouseInputs);
    if (scroll_allowed && (g.IO.MouseWheel != 0.0f || g.IO.MouseWheelH != 0.0f) && !g.IO.KeyCtrl)
    {
        ImVec2 max_step = (window->ContentsRegionRect.GetSize() + window->WindowPadding * 2.0f) * 0.67f;

        // Vertical Mouse Wheel Scrolling (hold Shift to scroll horizontally)
        if (g.IO.MouseWheel != 0.0f && !g.IO.KeyShift)
        {
            float scroll_step = ImFloor(ImMin(5 * window->CalcFontSize(), max_step.y));
            SetWindowScrollY(window, window->Scroll.y - g.IO.MouseWheel * scroll_step);
        } else if (g.IO.MouseWheel != 0.0f && g.IO.KeyShift)
        {
            float scroll_step = ImFloor(ImMin(2 * window->CalcFontSize(), max_step.x));
            SetWindowScrollX(window, window->Scroll.x - g.IO.MouseWheel * scroll_step);
        }

        // Horizontal Mouse Wheel Scrolling (for hardware that supports it)
        if (g.IO.MouseWheelH != 0.0f && !g.IO.KeyShift)
        {
            float scroll_step = ImFloor(ImMin(2 * window->CalcFontSize(), max_step.x));
            SetWindowScrollX(window, window->Scroll.x - g.IO.MouseWheelH * scroll_step);
        }
    }
}

// The reason this is exposed in vsonyp0wer_internal.h is: on touch-based system that don't have hovering, we want to dispatch inputs to the right target (vsonyp0wer vs vsonyp0wer+app)
void vsonyp0wer::UpdateHoveredWindowAndCaptureFlags()
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Find the window hovered by mouse:
    // - Child windows can extend beyond the limit of their parent so we need to derive HoveredRootWindow from HoveredWindow.
    // - When moving a window we can skip the search, which also conveniently bypasses the fact that window->WindowRectClipped is lagging as this point of the frame.
    // - We also support the moved window toggling the NoInputs flag after moving has started in order to be able to detect windows below it, which is useful for e.g. docking mechanisms.
    FindHoveredWindow();

    // Modal windows prevents cursor from hovering behind them.
    vsonyp0werWindow* modal_window = GetFrontMostPopupModal();
    if (modal_window)
        if (g.HoveredRootWindow && !IsWindowChildOf(g.HoveredRootWindow, modal_window))
            g.HoveredRootWindow = g.HoveredWindow = NULL;

    // Disabled mouse?
    if (g.IO.ConfigFlags & vsonyp0werConfigFlags_NoMouse)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // We track click ownership. When clicked outside of a window the click is owned by the application and won't report hovering nor request capture even while dragging over our windows afterward.
    int mouse_earliest_button_down = -1;
    bool mouse_any_down = false;
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.MouseDown); i++)
    {
        if (g.IO.MouseClicked[i])
            g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (!g.OpenPopupStack.empty());
        mouse_any_down |= g.IO.MouseDown[i];
        if (g.IO.MouseDown[i])
            if (mouse_earliest_button_down == -1 || g.IO.MouseClickedTime[i] < g.IO.MouseClickedTime[mouse_earliest_button_down])
                mouse_earliest_button_down = i;
    }
    const bool mouse_avail_to_vsonyp0wer = (mouse_earliest_button_down == -1) || g.IO.MouseDownOwned[mouse_earliest_button_down];

    // If mouse was first clicked outside of vsonyp0wer bounds we also cancel out hovering.
    // FIXME: For patterns of drag and drop across OS windows, we may need to rework/remove this test (first committed 311c0ca9 on 2015/02)
    const bool mouse_dragging_extern_payload = g.DragDropActive && (g.DragDropSourceFlags & vsonyp0werDragDropFlags_SourceExtern) != 0;
    if (!mouse_avail_to_vsonyp0wer && !mouse_dragging_extern_payload)
        g.HoveredWindow = g.HoveredRootWindow = NULL;

    // Update io.WantCaptureMouse for the user application (true = dispatch mouse info to vsonyp0wer, false = dispatch mouse info to vsonyp0wer + app)
    if (g.WantCaptureMouseNextFrame != -1)
        g.IO.WantCaptureMouse = (g.WantCaptureMouseNextFrame != 0);
    else
        g.IO.WantCaptureMouse = (mouse_avail_to_vsonyp0wer && (g.HoveredWindow != NULL || mouse_any_down)) || (!g.OpenPopupStack.empty());

    // Update io.WantCaptureKeyboard for the user application (true = dispatch keyboard info to vsonyp0wer, false = dispatch keyboard info to vsonyp0wer + app)
    if (g.WantCaptureKeyboardNextFrame != -1)
        g.IO.WantCaptureKeyboard = (g.WantCaptureKeyboardNextFrame != 0);
    else
        g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (modal_window != NULL);
    if (g.IO.NavActive && (g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableKeyboard) && !(g.IO.ConfigFlags & vsonyp0werConfigFlags_NavNoCaptureKeyboard))
        g.IO.WantCaptureKeyboard = true;

    // Update io.WantTextInput flag, this is to allow systems without a keyboard (e.g. mobile, hand-held) to show a software keyboard if possible
    g.IO.WantTextInput = (g.WantTextInputNextFrame != -1) ? (g.WantTextInputNextFrame != 0) : false;
}

void vsonyp0wer::NewFrame()
{
    IM_ASSERT(Gvsonyp0wer != NULL && "No current context. Did you call vsonyp0wer::CreateContext() and vsonyp0wer::SetCurrentContext() ?");
    vsonyp0werContext & g = *Gvsonyp0wer;

#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
    vsonyp0werTestEngineHook_PreNewFrame(&g);
#endif

    // Check user data
    // (We pass an error message in the assert expression to make it visible to programmers who are not using a debugger, as most assert handlers display their argument)
    IM_ASSERT(g.Initialized);
    IM_ASSERT((g.IO.DeltaTime > 0.0f || g.FrameCount == 0) && "Need a positive DeltaTime!");
    IM_ASSERT((g.FrameCount == 0 || g.FrameCountEnded == g.FrameCount) && "Forgot to call Render() or EndFrame() at the end of the previous frame?");
    IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f && "Invalid DisplaySize value!");
    IM_ASSERT(g.IO.Fonts->Fonts.Size > 0 && "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8() ?");
    IM_ASSERT(g.IO.Fonts->Fonts[0]->IsLoaded() && "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8() ?");
    IM_ASSERT(g.Style.CurveTessellationTol > 0.0f && "Invalid style setting!");
    IM_ASSERT(g.Style.Alpha >= 0.0f && g.Style.Alpha <= 1.0f && "Invalid style setting. Alpha cannot be negative (allows us to avoid a few clamps in color computations)!");
    IM_ASSERT(g.Style.WindowMinSize.x >= 1.0f && g.Style.WindowMinSize.y >= 1.0f && "Invalid style setting.");
    for (int n = 0; n < vsonyp0werKey_COUNT; n++)
        IM_ASSERT(g.IO.KeyMap[n] >= -1 && g.IO.KeyMap[n] < IM_ARRAYSIZE(g.IO.KeysDown) && "io.KeyMap[] contains an out of bound value (need to be 0..512, or -1 for unmapped key)");

    // Perform simple check: required key mapping (we intentionally do NOT check all keys to not pressure user into setting up everything, but Space is required and was only recently added in 1.60 WIP)
    if (g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableKeyboard)
        IM_ASSERT(g.IO.KeyMap[vsonyp0werKey_Space] != -1 && "vsonyp0werKey_Space is not mapped, required for keyboard navigation.");

    // Perform simple check: the beta io.ConfigWindowsResizeFromEdges option requires back-end to honor mouse cursor changes and set the vsonyp0werBackendFlags_HasMouseCursors flag accordingly.
    if (g.IO.ConfigWindowsResizeFromEdges && !(g.IO.BackendFlags & vsonyp0werBackendFlags_HasMouseCursors))
        g.IO.ConfigWindowsResizeFromEdges = false;

    // Load settings on first frame (if not explicitly loaded manually before)
    if (!g.SettingsLoaded)
    {
        IM_ASSERT(g.SettingsWindows.empty());
        if (g.IO.IniFilename)
            LoadIniSettingsFromDisk(g.IO.IniFilename);
        g.SettingsLoaded = true;
    }

    // Save settings (with a delay after the last modification, so we don't spam disk too much)
    if (g.SettingsDirtyTimer > 0.0f)
    {
        g.SettingsDirtyTimer -= g.IO.DeltaTime;
        if (g.SettingsDirtyTimer <= 0.0f)
        {
            if (g.IO.IniFilename != NULL)
                SaveIniSettingsToDisk(g.IO.IniFilename);
            else
                g.IO.WantSaveIniSettings = true;  // Let user know they can call SaveIniSettingsToMemory(). user will need to clear io.WantSaveIniSettings themselves.
            g.SettingsDirtyTimer = 0.0f;
        }
    }

    g.Time += g.IO.DeltaTime;
    g.FrameScopeActive = true;
    g.FrameCount += 1;
    g.TooltipOverrideCount = 0;
    g.WindowsActiveCount = 0;

    // Setup current font and draw list shared data
    g.IO.Fonts->Locked = true;
    SetCurrentFont(GetDefaultFont());
    IM_ASSERT(g.Font->IsLoaded());
    g.DrawListSharedData.ClipRectFullscreen = ImVec4(0.0f, 0.0f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
    g.DrawListSharedData.CurveTessellationTol = g.Style.CurveTessellationTol;

    g.BackgroundDrawList.Clear();
    g.BackgroundDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.BackgroundDrawList.PushClipRectFullScreen();
    g.BackgroundDrawList.Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);

    g.ForegroundDrawList.Clear();
    g.ForegroundDrawList.PushTextureID(g.IO.Fonts->TexID);
    g.ForegroundDrawList.PushClipRectFullScreen();
    g.ForegroundDrawList.Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);

    // Mark rendering data as invalid to prevent user who may have a handle on it to use it.
    g.DrawData.Clear();

    // Drag and drop keep the source ID alive so even if the source disappear our state is consistent
    if (g.DragDropActive && g.DragDropPayload.SourceId == g.ActiveId)
        KeepAliveID(g.DragDropPayload.SourceId);

    // Clear reference to active widget if the widget isn't alive anymore
    if (!g.HoveredIdPreviousFrame)
        g.HoveredIdTimer = 0.0f;
    if (!g.HoveredIdPreviousFrame || (g.HoveredId && g.ActiveId == g.HoveredId))
        g.HoveredIdNotActiveTimer = 0.0f;
    if (g.HoveredId)
        g.HoveredIdTimer += g.IO.DeltaTime;
    if (g.HoveredId && g.ActiveId != g.HoveredId)
        g.HoveredIdNotActiveTimer += g.IO.DeltaTime;
    g.HoveredIdPreviousFrame = g.HoveredId;
    g.HoveredId = 0;
    g.HoveredIdAllowOverlap = false;
    if (g.ActiveIdIsAlive != g.ActiveId && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
        ClearActiveID();
    if (g.ActiveId)
        g.ActiveIdTimer += g.IO.DeltaTime;
    g.LastActiveIdTimer += g.IO.DeltaTime;
    g.ActiveIdPreviousFrame = g.ActiveId;
    g.ActiveIdPreviousFrameWindow = g.ActiveIdWindow;
    g.ActiveIdPreviousFrameHasBeenEdited = g.ActiveIdHasBeenEdited;
    g.ActiveIdIsAlive = 0;
    g.ActiveIdPreviousFrameIsAlive = false;
    g.ActiveIdIsJustActivated = false;
    if (g.ScalarAsInputTextId && g.ActiveId != g.ScalarAsInputTextId)
        g.ScalarAsInputTextId = 0;

    // Drag and drop
    g.DragDropAcceptIdPrev = g.DragDropAcceptIdCurr;
    g.DragDropAcceptIdCurr = 0;
    g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
    g.DragDropWithinSourceOrTarget = false;

    // Update keyboard input state
    memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.KeysDown); i++)
        g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.0f ? 0.0f : g.IO.KeysDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Update gamepad/keyboard directional navigation
    NavUpdate();

    // Update mouse input state
    UpdateMouseInputs();

    // Calculate frame-rate for the user, as a purely luxurious feature
    g.FramerateSecPerFrameAccum += g.IO.DeltaTime - g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
    g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
    g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) % IM_ARRAYSIZE(g.FramerateSecPerFrame);
    g.IO.Framerate = (g.FramerateSecPerFrameAccum > 0.0f) ? (1.0f / (g.FramerateSecPerFrameAccum / (float)IM_ARRAYSIZE(g.FramerateSecPerFrame))) : FLT_MAX;

    // Handle user moving window with mouse (at the beginning of the frame to avoid input lag or sheering)
    UpdateMouseMovingWindowNewFrame();
    UpdateHoveredWindowAndCaptureFlags();

    // Background darkening/whitening
    if (GetFrontMostPopupModal() != NULL || (g.NavWindowingTarget != NULL && g.NavWindowingHighlightAlpha > 0.0f))
        g.DimBgRatio = ImMin(g.DimBgRatio + g.IO.DeltaTime * 6.0f, 1.0f);
    else
        g.DimBgRatio = ImMax(g.DimBgRatio - g.IO.DeltaTime * 10.0f, 0.0f);

    g.MouseCursor = vsonyp0werMouseCursor_Arrow;
    g.WantCaptureMouseNextFrame = g.WantCaptureKeyboardNextFrame = g.WantTextInputNextFrame = -1;
    g.PlatformImePos = ImVec2(1.0f, 1.0f); // OS Input Method Editor showing on top-left of our window by default

    // Mouse wheel scrolling, scale
    UpdateMouseWheel();

    // Pressing TAB activate widget focus
    g.FocusTabPressed = (g.NavWindow && g.NavWindow->Active && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs) && !g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_Tab));
    if (g.ActiveId == 0 && g.FocusTabPressed)
    {
        // Note that SetKeyboardFocusHere() sets the Next fields mid-frame. To be consistent we also
        // manipulate the Next fields even, even though they will be turned into Curr fields by the code below.
        g.FocusRequestNextWindow = g.NavWindow;
        g.FocusRequestNextCounterAll = INT_MAX;
        if (g.NavId != 0 && g.NavIdTabCounter != INT_MAX)
            g.FocusRequestNextCounterTab = g.NavIdTabCounter + 1 + (g.IO.KeyShift ? -1 : 1);
        else
            g.FocusRequestNextCounterTab = g.IO.KeyShift ? -1 : 0;
    }

    // Turn queued focus request into current one
    g.FocusRequestCurrWindow = NULL;
    g.FocusRequestCurrCounterAll = g.FocusRequestCurrCounterTab = INT_MAX;
    if (g.FocusRequestNextWindow != NULL)
    {
        vsonyp0werWindow* window = g.FocusRequestNextWindow;
        g.FocusRequestCurrWindow = window;
        if (g.FocusRequestNextCounterAll != INT_MAX && window->DC.FocusCounterAll != -1)
            g.FocusRequestCurrCounterAll = ImModPositive(g.FocusRequestNextCounterAll, window->DC.FocusCounterAll + 1);
        if (g.FocusRequestNextCounterTab != INT_MAX && window->DC.FocusCounterTab != -1)
            g.FocusRequestCurrCounterTab = ImModPositive(g.FocusRequestNextCounterTab, window->DC.FocusCounterTab + 1);
        g.FocusRequestNextWindow = NULL;
        g.FocusRequestNextCounterAll = g.FocusRequestNextCounterTab = INT_MAX;
    }

    g.NavIdTabCounter = INT_MAX;

    // Mark all windows as not visible
    IM_ASSERT(g.WindowsFocusOrder.Size == g.Windows.Size);
    for (int i = 0; i != g.Windows.Size; i++)
    {
        vsonyp0werWindow* window = g.Windows[i];
        window->WasActive = window->Active;
        window->BeginCount = 0;
        window->Active = false;
        window->WriteAccessed = false;
    }

    // Closing the focused window restore focus to the first active root window in descending z-order
    if (g.NavWindow && !g.NavWindow->WasActive)
        FocusPreviousWindowIgnoringOne(NULL);

    // No window should be open at the beginning of the frame.
    // But in order to allow the user to call NewFrame() multiple times without calling Render(), we are doing an explicit clear.
    g.CurrentWindowStack.resize(0);
    g.BeginPopupStack.resize(0);
    ClosePopupsOverWindow(g.NavWindow);

    // Create implicit/fallback window - which we will only render it if the user has added something to it.
    // We don't use "Debug" to avoid colliding with user trying to create a "Debug" window with custom flags.
    // This fallback is particularly important as it avoid vsonyp0wer:: calls from crashing.
    SetNextWindowSize(ImVec2(400, 400), vsonyp0werCond_FirstUseEver);
    Begin("Debug##Default");
    g.FrameScopePushedImplicitWindow = true;

#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
    vsonyp0werTestEngineHook_PostNewFrame(&g);
#endif
}

void vsonyp0wer::Initialize(vsonyp0werContext * context)
{
    vsonyp0werContext& g = *context;
    IM_ASSERT(!g.Initialized && !g.SettingsLoaded);

    // Add .ini handle for vsonyp0werWindow type
    vsonyp0werSettingsHandler ini_handler;
    ini_handler.TypeName = "Window";
    ini_handler.TypeHash = ImHashStr("Window", 0);
    ini_handler.ReadOpenFn = SettingsHandlerWindow_ReadOpen;
    ini_handler.ReadLineFn = SettingsHandlerWindow_ReadLine;
    ini_handler.WriteAllFn = SettingsHandlerWindow_WriteAll;
    g.SettingsHandlers.push_back(ini_handler);

    g.Initialized = true;
}

// This function is merely here to free heap allocations.
void vsonyp0wer::Shutdown(vsonyp0werContext * context)
{
    // The fonts atlas can be used prior to calling NewFrame(), so we clear it even if g.Initialized is FALSE (which would happen if we never called NewFrame)
    vsonyp0werContext& g = *context;
    if (g.IO.Fonts && g.FontAtlasOwnedByContext)
    {
        g.IO.Fonts->Locked = false;
        IM_DELETE(g.IO.Fonts);
    }
    g.IO.Fonts = NULL;

    // Cleanup of other data are conditional on actually having initialized vsonyp0wer.
    if (!g.Initialized)
        return;

    // Save settings (unless we haven't attempted to load them: CreateContext/DestroyContext without a call to NewFrame shouldn't save an empty file)
    if (g.SettingsLoaded && g.IO.IniFilename != NULL)
    {
        vsonyp0werContext* backup_context = Gvsonyp0wer;
        SetCurrentContext(context);
        SaveIniSettingsToDisk(g.IO.IniFilename);
        SetCurrentContext(backup_context);
    }

    // Clear everything else
    for (int i = 0; i < g.Windows.Size; i++)
        IM_DELETE(g.Windows[i]);
    g.Windows.clear();
    g.WindowsFocusOrder.clear();
    g.WindowsSortBuffer.clear();
    g.CurrentWindow = NULL;
    g.CurrentWindowStack.clear();
    g.WindowsById.Clear();
    g.NavWindow = NULL;
    g.HoveredWindow = g.HoveredRootWindow = NULL;
    g.ActiveIdWindow = g.ActiveIdPreviousFrameWindow = NULL;
    g.MovingWindow = NULL;
    g.ColorModifiers.clear();
    g.StyleModifiers.clear();
    g.FontStack.clear();
    g.OpenPopupStack.clear();
    g.BeginPopupStack.clear();
    g.DrawDataBuilder.ClearFreeMemory();
    g.BackgroundDrawList.ClearFreeMemory();
    g.ForegroundDrawList.ClearFreeMemory();
    g.PrivateClipboard.clear();
    g.InputTextState.ClearFreeMemory();

    for (int i = 0; i < g.SettingsWindows.Size; i++)
        IM_DELETE(g.SettingsWindows[i].Name);
    g.SettingsWindows.clear();
    g.SettingsHandlers.clear();

    if (g.LogFile && g.LogFile != stdout)
    {
        fclose(g.LogFile);
        g.LogFile = NULL;
    }
    g.LogBuffer.clear();

    g.Initialized = false;
}

// FIXME: Add a more explicit sort order in the window structure.
static int vsonyp0wer_CDECL ChildWindowComparer(const void* lhs, const void* rhs)
{
    const vsonyp0werWindow* const a = *(const vsonyp0werWindow * const*)lhs;
    const vsonyp0werWindow* const b = *(const vsonyp0werWindow * const*)rhs;
    if (int d = (a->Flags & vsonyp0werWindowFlags_Popup) - (b->Flags & vsonyp0werWindowFlags_Popup))
        return d;
    if (int d = (a->Flags & vsonyp0werWindowFlags_Tooltip) - (b->Flags & vsonyp0werWindowFlags_Tooltip))
        return d;
    return (a->BeginOrderWithinParent - b->BeginOrderWithinParent);
}

static void AddWindowToSortBuffer(ImVector<vsonyp0werWindow*> * out_sorted_windows, vsonyp0werWindow * window)
{
    out_sorted_windows->push_back(window);
    if (window->Active)
    {
        int count = window->DC.ChildWindows.Size;
        if (count > 1)
            ImQsort(window->DC.ChildWindows.begin(), (size_t)count, sizeof(vsonyp0werWindow*), ChildWindowComparer);
        for (int i = 0; i < count; i++)
        {
            vsonyp0werWindow* child = window->DC.ChildWindows[i];
            if (child->Active)
                AddWindowToSortBuffer(out_sorted_windows, child);
        }
    }
}

static void AddDrawListToDrawData(ImVector<ImDrawList*> * out_list, ImDrawList * draw_list)
{
    if (draw_list->CmdBuffer.empty())
        return;

    // Remove trailing command if unused
    ImDrawCmd& last_cmd = draw_list->CmdBuffer.back();
    if (last_cmd.ElemCount == 0 && last_cmd.UserCallback == NULL)
    {
        draw_list->CmdBuffer.pop_back();
        if (draw_list->CmdBuffer.empty())
            return;
    }

    // Draw list sanity check. Detect mismatch between PrimReserve() calls and incrementing _VtxCurrentIdx, _VtxWritePtr etc. May trigger for you if you are using PrimXXX functions incorrectly.
    IM_ASSERT(draw_list->VtxBuffer.Size == 0 || draw_list->_VtxWritePtr == draw_list->VtxBuffer.Data + draw_list->VtxBuffer.Size);
    IM_ASSERT(draw_list->IdxBuffer.Size == 0 || draw_list->_IdxWritePtr == draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size);
    IM_ASSERT((int)draw_list->_VtxCurrentIdx == draw_list->VtxBuffer.Size);

    // Check that draw_list doesn't use more vertices than indexable (default ImDrawIdx = unsigned short = 2 bytes = 64K vertices per ImDrawList = per window)
    // If this assert triggers because you are drawing lots of stuff manually:
    // A) Make sure you are coarse clipping, because ImDrawList let all your vertices pass. You can use the Metrics window to inspect draw list contents.
    // B) If you need/want meshes with more than 64K vertices, uncomment the '#define ImDrawIdx unsigned int' line in imconfig.h to set the index size to 4 bytes.
    //    You'll need to handle the 4-bytes indices to your renderer. For example, the OpenGL example code detect index size at compile-time by doing:
    //      glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
    //    Your own engine or render API may use different parameters or function calls to specify index sizes. 2 and 4 bytes indices are generally supported by most API.
    // C) If for some reason you cannot use 4 bytes indices or don't want to, a workaround is to call BeginChild()/EndChild() before reaching the 64K limit to split your draw commands in multiple draw lists.
    if (sizeof(ImDrawIdx) == 2)
        IM_ASSERT(draw_list->_VtxCurrentIdx < (1 << 16) && "Too many vertices in ImDrawList using 16-bit indices. Read comment above");

    out_list->push_back(draw_list);
}

static void AddWindowToDrawData(ImVector<ImDrawList*> * out_render_list, vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.IO.MetricsRenderWindows++;
    AddDrawListToDrawData(out_render_list, window->DrawList);
    for (int i = 0; i < window->DC.ChildWindows.Size; i++)
    {
        vsonyp0werWindow* child = window->DC.ChildWindows[i];
        if (IsWindowActiveAndVisible(child)) // clipped children may have been marked not active
            AddWindowToDrawData(out_render_list, child);
    }
}

// Layer is locked for the root window, however child windows may use a different viewport (e.g. extruding menu)
static void AddRootWindowToDrawData(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (window->Flags & vsonyp0werWindowFlags_Tooltip)
        AddWindowToDrawData(&g.DrawDataBuilder.Layers[1], window);
    else
        AddWindowToDrawData(&g.DrawDataBuilder.Layers[0], window);
}

void ImDrawDataBuilder::FlattenIntoSingleLayer()
{
    int n = Layers[0].Size;
    int size = n;
    for (int i = 1; i < IM_ARRAYSIZE(Layers); i++)
        size += Layers[i].Size;
    Layers[0].resize(size);
    for (int layer_n = 1; layer_n < IM_ARRAYSIZE(Layers); layer_n++)
    {
        ImVector<ImDrawList*>& layer = Layers[layer_n];
        if (layer.empty())
            continue;
        memcpy(&Layers[0][n], &layer[0], layer.Size * sizeof(ImDrawList*));
        n += layer.Size;
        layer.resize(0);
    }
}

static void SetupDrawData(ImVector<ImDrawList*> * draw_lists, ImDrawData * draw_data)
{
    vsonyp0werIO& io = vsonyp0wer::GetIO();
    draw_data->Valid = true;
    draw_data->CmdLists = (draw_lists->Size > 0) ? draw_lists->Data : NULL;
    draw_data->CmdListsCount = draw_lists->Size;
    draw_data->TotalVtxCount = draw_data->TotalIdxCount = 0;
    draw_data->DisplayPos = ImVec2(0.0f, 0.0f);
    draw_data->DisplaySize = io.DisplaySize;
    draw_data->FramebufferScale = io.DisplayFramebufferScale;
    for (int n = 0; n < draw_lists->Size; n++)
    {
        draw_data->TotalVtxCount += draw_lists->Data[n]->VtxBuffer.Size;
        draw_data->TotalIdxCount += draw_lists->Data[n]->IdxBuffer.Size;
    }
}

// When using this function it is sane to ensure that float are perfectly rounded to integer values, to that e.g. (int)(max.x-min.x) in user's render produce correct result.
void vsonyp0wer::PushClipRect(const ImVec2 & clip_rect_min, const ImVec2 & clip_rect_max, bool intersect_with_current_clip_rect)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DrawList->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

void vsonyp0wer::PopClipRect()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DrawList->PopClipRect();
    window->ClipRect = window->DrawList->_ClipRectStack.back();
}

// This is normally called by Render(). You may want to call it directly if you want to avoid calling Render() but the gain will be very minimal.
void vsonyp0wer::EndFrame()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.Initialized);
    if (g.FrameCountEnded == g.FrameCount)          // Don't process EndFrame() multiple times.
        return;
    IM_ASSERT(g.FrameScopeActive && "Forgot to call vsonyp0wer::NewFrame()?");

    // Notify OS when our Input Method Editor cursor has moved (e.g. CJK inputs using Microsoft IME)
    if (g.IO.ImeSetInputScreenPosFn && (g.PlatformImeLastPos.x == FLT_MAX || ImLengthSqr(g.PlatformImeLastPos - g.PlatformImePos) > 0.0001f))
    {
        g.IO.ImeSetInputScreenPosFn((int)g.PlatformImePos.x, (int)g.PlatformImePos.y);
        g.PlatformImeLastPos = g.PlatformImePos;
    }

    // Report when there is a mismatch of Begin/BeginChild vs End/EndChild calls. Important: Remember that the Begin/BeginChild API requires you
    // to always call End/EndChild even if Begin/BeginChild returns false! (this is unfortunately inconsistent with most other Begin* API).
    if (g.CurrentWindowStack.Size != 1)
    {
        if (g.CurrentWindowStack.Size > 1)
        {
            IM_ASSERT(g.CurrentWindowStack.Size == 1 && "Mismatched Begin/BeginChild vs End/EndChild calls: did you forget to call End/EndChild?");
            while (g.CurrentWindowStack.Size > 1) // FIXME-ERRORHANDLING
                End();
        } else
        {
            IM_ASSERT(g.CurrentWindowStack.Size == 1 && "Mismatched Begin/BeginChild vs End/EndChild calls: did you call End/EndChild too much?");
        }
    }

    // Hide implicit/fallback "Debug" window if it hasn't been used
    g.FrameScopePushedImplicitWindow = false;
    if (g.CurrentWindow && !g.CurrentWindow->WriteAccessed)
        g.CurrentWindow->Active = false;
    End();

    // Show CTRL+TAB list window
    if (g.NavWindowingTarget)
        NavUpdateWindowingList();

    // Drag and Drop: Elapse payload (if delivered, or if source stops being submitted)
    if (g.DragDropActive)
    {
        bool is_delivered = g.DragDropPayload.Delivery;
        bool is_elapsed = (g.DragDropPayload.DataFrameCount + 1 < g.FrameCount) && ((g.DragDropSourceFlags & vsonyp0werDragDropFlags_SourceAutoExpirePayload) || !IsMouseDown(g.DragDropMouseButton));
        if (is_delivered || is_elapsed)
            ClearDragDrop();
    }

    // Drag and Drop: Fallback for source tooltip. This is not ideal but better than nothing.
    if (g.DragDropActive && g.DragDropSourceFrameCount < g.FrameCount)
    {
        g.DragDropWithinSourceOrTarget = true;
        SetTooltip("...");
        g.DragDropWithinSourceOrTarget = false;
    }

    // End frame
    g.FrameScopeActive = false;
    g.FrameCountEnded = g.FrameCount;

    // Initiate moving window + handle left-click and right-click focus
    UpdateMouseMovingWindowEndFrame();

    // Sort the window list so that all child windows are after their parent
    // We cannot do that on FocusWindow() because childs may not exist yet
    g.WindowsSortBuffer.resize(0);
    g.WindowsSortBuffer.reserve(g.Windows.Size);
    for (int i = 0; i != g.Windows.Size; i++)
    {
        vsonyp0werWindow* window = g.Windows[i];
        if (window->Active && (window->Flags & vsonyp0werWindowFlags_ChildWindow))       // if a child is active its parent will add it
            continue;
        AddWindowToSortBuffer(&g.WindowsSortBuffer, window);
    }

    // This usually assert if there is a mismatch between the vsonyp0werWindowFlags_ChildWindow / ParentWindow values and DC.ChildWindows[] in parents, aka we've done something wrong.
    IM_ASSERT(g.Windows.Size == g.WindowsSortBuffer.Size);
    g.Windows.swap(g.WindowsSortBuffer);
    g.IO.MetricsActiveWindows = g.WindowsActiveCount;

    // Unlock font atlas
    g.IO.Fonts->Locked = false;

    // Clear Input data for next frame
    g.IO.MouseWheel = g.IO.MouseWheelH = 0.0f;
    g.IO.InputQueueCharacters.resize(0);
    memset(g.IO.NavInputs, 0, sizeof(g.IO.NavInputs));
}

void vsonyp0wer::Render()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.Initialized);

    if (g.FrameCountEnded != g.FrameCount)
        EndFrame();
    g.FrameCountRendered = g.FrameCount;

    // Gather ImDrawList to render (for each active window)
    g.IO.MetricsRenderVertices = g.IO.MetricsRenderIndices = g.IO.MetricsRenderWindows = 0;
    g.DrawDataBuilder.Clear();
    if (!g.BackgroundDrawList.VtxBuffer.empty())
        AddDrawListToDrawData(&g.DrawDataBuilder.Layers[0], &g.BackgroundDrawList);

    vsonyp0werWindow * windows_to_render_front_most[2];
    windows_to_render_front_most[0] = (g.NavWindowingTarget && !(g.NavWindowingTarget->Flags & vsonyp0werWindowFlags_NoBringToFrontOnFocus)) ? g.NavWindowingTarget->RootWindow : NULL;
    windows_to_render_front_most[1] = g.NavWindowingTarget ? g.NavWindowingList : NULL;
    for (int n = 0; n != g.Windows.Size; n++)
    {
        vsonyp0werWindow* window = g.Windows[n];
        if (IsWindowActiveAndVisible(window) && (window->Flags & vsonyp0werWindowFlags_ChildWindow) == 0 && window != windows_to_render_front_most[0] && window != windows_to_render_front_most[1])
            AddRootWindowToDrawData(window);
    }
    for (int n = 0; n < IM_ARRAYSIZE(windows_to_render_front_most); n++)
        if (windows_to_render_front_most[n] && IsWindowActiveAndVisible(windows_to_render_front_most[n])) // NavWindowingTarget is always temporarily displayed as the front-most window
            AddRootWindowToDrawData(windows_to_render_front_most[n]);
    g.DrawDataBuilder.FlattenIntoSingleLayer();

    // Draw software mouse cursor if requested
    if (g.IO.MouseDrawCursor)
        RenderMouseCursor(&g.ForegroundDrawList, g.IO.MousePos, g.Style.MouseCursorScale, g.MouseCursor);

    if (!g.ForegroundDrawList.VtxBuffer.empty())
        AddDrawListToDrawData(&g.DrawDataBuilder.Layers[0], &g.ForegroundDrawList);

    // Setup ImDrawData structure for end-user
    SetupDrawData(&g.DrawDataBuilder.Layers[0], &g.DrawData);
    g.IO.MetricsRenderVertices = g.DrawData.TotalVtxCount;
    g.IO.MetricsRenderIndices = g.DrawData.TotalIdxCount;

    // (Legacy) Call the Render callback function. The current prefer way is to let the user retrieve GetDrawData() and call the render function themselves.
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
    if (g.DrawData.CmdListsCount > 0 && g.IO.RenderDrawListsFn != NULL)
        g.IO.RenderDrawListsFn(&g.DrawData);
#endif
}

// Calculate text size. Text can be multi-line. Optionally ignore text after a ## marker.
// CalcTextSize("") should return ImVec2(0.0f, Gvsonyp0wer->FontSize)
ImVec2 vsonyp0wer::CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    const char* text_display_end;
    if (hide_text_after_double_hash)
        text_display_end = FindRenderedTextEnd(text, text_end);      // Hide anything after a '##' string
    else
        text_display_end = text_end;

    ImFont* font = g.Font;
    const float font_size = g.FontSize;
    if (text == text_display_end)
        return ImVec2(0.0f, font_size);
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

    // Round
    text_size.x = (float)(int)(text_size.x + 0.95f);

    return text_size;
}

// Helper to calculate coarse clipping of large list of evenly sized items.
// NB: Prefer using the vsonyp0werListClipper higher-level helper if you can! Read comments and instructions there on how those use this sort of pattern.
// NB: 'items_count' is only used to clamp the result, if you don't know your count you can use INT_MAX
void vsonyp0wer::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (g.LogEnabled)
    {
        // If logging is active, do not perform any clipping
        *out_items_display_start = 0;
        *out_items_display_end = items_count;
        return;
    }
    if (window->SkipItems)
    {
        *out_items_display_start = *out_items_display_end = 0;
        return;
    }

    // We create the union of the ClipRect and the NavScoringRect which at worst should be 1 page away from ClipRect
    ImRect unclipped_rect = window->ClipRect;
    if (g.NavMoveRequest)
        unclipped_rect.Add(g.NavScoringRectScreen);

    const ImVec2 pos = window->DC.CursorPos;
    int start = (int)((unclipped_rect.Min.y - pos.y) / items_height);
    int end = (int)((unclipped_rect.Max.y - pos.y) / items_height);

    // When performing a navigation request, ensure we have one item extra in the direction we are moving to
    if (g.NavMoveRequest && g.NavMoveClipDir == vsonyp0werDir_Up)
        start--;
    if (g.NavMoveRequest && g.NavMoveClipDir == vsonyp0werDir_Down)
        end++;

    start = ImClamp(start, 0, items_count);
    end = ImClamp(end + 1, start, items_count);
    *out_items_display_start = start;
    *out_items_display_end = end;
}

// Find window given position, search front-to-back
// FIXME: Note that we have an inconsequential lag here: OuterRectClipped is updated in Begin(), so windows moved programatically
// with SetWindowPos() and not SetNextWindowPos() will have that rectangle lagging by a frame at the time FindHoveredWindow() is
// called, aka before the next Begin(). Moving window isn't affected.
static void FindHoveredWindow()
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    vsonyp0werWindow* hovered_window = NULL;
    if (g.MovingWindow && !(g.MovingWindow->Flags & vsonyp0werWindowFlags_NoMouseInputs))
        hovered_window = g.MovingWindow;

    ImVec2 padding_regular = g.Style.TouchExtraPadding;
    ImVec2 padding_for_resize_from_edges = g.IO.ConfigWindowsResizeFromEdges ? ImMax(g.Style.TouchExtraPadding, ImVec2(WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS, WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS)) : padding_regular;
    for (int i = g.Windows.Size - 1; i >= 0; i--)
    {
        vsonyp0werWindow* window = g.Windows[i];
        if (!window->Active || window->Hidden)
            continue;
        if (window->Flags & vsonyp0werWindowFlags_NoMouseInputs)
            continue;

        // Using the clipped AABB, a child window will typically be clipped by its parent (not always)
        ImRect bb(window->OuterRectClipped);
        if ((window->Flags & vsonyp0werWindowFlags_ChildWindow) || (window->Flags & vsonyp0werWindowFlags_NoResize))
            bb.Expand(padding_regular);
        else
            bb.Expand(padding_for_resize_from_edges);
        if (!bb.Contains(g.IO.MousePos))
            continue;

        // Those seemingly unnecessary extra tests are because the code here is a little different in viewport/docking branches.
        if (hovered_window == NULL)
            hovered_window = window;
        if (hovered_window)
            break;
    }

    g.HoveredWindow = hovered_window;
    g.HoveredRootWindow = g.HoveredWindow ? g.HoveredWindow->RootWindow : NULL;

}

// Test if mouse cursor is hovering given rectangle
// NB- Rectangle is clipped by our current clip setting
// NB- Expand the rectangle to be generous on imprecise inputs systems (g.Style.TouchExtraPadding)
bool vsonyp0wer::IsMouseHoveringRect(const ImVec2 & r_min, const ImVec2 & r_max, bool clip)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Clip
    ImRect rect_clipped(r_min, r_max);
    if (clip)
        rect_clipped.ClipWith(g.CurrentWindow->ClipRect);

    // Expand for touch input
    const ImRect rect_for_touch(rect_clipped.Min - g.Style.TouchExtraPadding, rect_clipped.Max + g.Style.TouchExtraPadding);
    if (!rect_for_touch.Contains(g.IO.MousePos))
        return false;
    return true;
}

int vsonyp0wer::GetKeyIndex(vsonyp0werKey vsonyp0wer_key)
{
    IM_ASSERT(vsonyp0wer_key >= 0 && vsonyp0wer_key < vsonyp0werKey_COUNT);
    return Gvsonyp0wer->IO.KeyMap[vsonyp0wer_key];
}

// Note that vsonyp0wer doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your back-end/engine stored them into io.KeysDown[]!
bool vsonyp0wer::IsKeyDown(int user_key_index)
{
    if (user_key_index < 0) return false;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(Gvsonyp0wer->IO.KeysDown));
    return Gvsonyp0wer->IO.KeysDown[user_key_index];
}

int vsonyp0wer::CalcTypematicPressedRepeatAmount(float t, float t_prev, float repeat_delay, float repeat_rate)
{
    if (t == 0.0f)
        return 1;
    if (t <= repeat_delay || repeat_rate <= 0.0f)
        return 0;
    const int count = (int)((t - repeat_delay) / repeat_rate) - (int)((t_prev - repeat_delay) / repeat_rate);
    return (count > 0) ? count : 0;
}

int vsonyp0wer::GetKeyPressedAmount(int key_index, float repeat_delay, float repeat_rate)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (key_index < 0)
        return 0;
    IM_ASSERT(key_index >= 0 && key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    const float t = g.IO.KeysDownDuration[key_index];
    return CalcTypematicPressedRepeatAmount(t, t - g.IO.DeltaTime, repeat_delay, repeat_rate);
}

bool vsonyp0wer::IsKeyPressed(int user_key_index, bool repeat)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (user_key_index < 0)
        return false;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    const float t = g.IO.KeysDownDuration[user_key_index];
    if (t == 0.0f)
        return true;
    if (repeat && t > g.IO.KeyRepeatDelay)
        return GetKeyPressedAmount(user_key_index, g.IO.KeyRepeatDelay, g.IO.KeyRepeatRate) > 0;
    return false;
}

bool vsonyp0wer::IsKeyReleased(int user_key_index)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (user_key_index < 0) return false;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    return g.IO.KeysDownDurationPrev[user_key_index] >= 0.0f && !g.IO.KeysDown[user_key_index];
}

bool vsonyp0wer::IsMouseDown(int button)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDown[button];
}

bool vsonyp0wer::IsAnyMouseDown()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int n = 0; n < IM_ARRAYSIZE(g.IO.MouseDown); n++)
        if (g.IO.MouseDown[n])
            return true;
    return false;
}

bool vsonyp0wer::IsMouseClicked(int button, bool repeat)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    const float t = g.IO.MouseDownDuration[button];
    if (t == 0.0f)
        return true;

    if (repeat && t > g.IO.KeyRepeatDelay)
    {
        float delay = g.IO.KeyRepeatDelay, rate = g.IO.KeyRepeatRate;
        if ((ImFmod(t - delay, rate) > rate * 0.5f) != (ImFmod(t - delay - g.IO.DeltaTime, rate) > rate * 0.5f))
            return true;
    }

    return false;
}

bool vsonyp0wer::IsMouseReleased(int button)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseReleased[button];
}

bool vsonyp0wer::IsMouseDoubleClicked(int button)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    return g.IO.MouseDoubleClicked[button];
}

bool vsonyp0wer::IsMouseDragging(int button, float lock_threshold)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (!g.IO.MouseDown[button])
        return false;
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    return g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold;
}

ImVec2 vsonyp0wer::GetMousePos()
{
    return Gvsonyp0wer->IO.MousePos;
}

// NB: prefer to call right after BeginPopup(). At the time Selectable/MenuItem is activated, the popup is already closed!
ImVec2 vsonyp0wer::GetMousePosOnOpeningCurrentPopup()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.BeginPopupStack.Size > 0)
        return g.OpenPopupStack[g.BeginPopupStack.Size - 1].OpenMousePos;
    return g.IO.MousePos;
}

// We typically use ImVec2(-FLT_MAX,-FLT_MAX) to denote an invalid mouse position.
bool vsonyp0wer::IsMousePosValid(const ImVec2 * mouse_pos)
{
    // The assert is only to silence a false-positive in XCode Static Analysis.
    // Because Gvsonyp0wer is not dereferenced in every code path, the static analyzer assume that it may be NULL (which it doesn't for other functions).
    IM_ASSERT(Gvsonyp0wer != NULL);
    const float MOUSE_INVALID = -256000.0f;
    ImVec2 p = mouse_pos ? *mouse_pos : Gvsonyp0wer->IO.MousePos;
    return p.x >= MOUSE_INVALID && p.y >= MOUSE_INVALID;
}

// Return the delta from the initial clicking position while the mouse button is clicked or was just released.
// This is locked and return 0.0f until the mouse moves past a distance threshold at least once.
// NB: This is only valid if IsMousePosValid(). Back-ends in theory should always keep mouse position valid when dragging even outside the client window.
ImVec2 vsonyp0wer::GetMouseDragDelta(int button, float lock_threshold)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    if (lock_threshold < 0.0f)
        lock_threshold = g.IO.MouseDragThreshold;
    if (g.IO.MouseDown[button] || g.IO.MouseReleased[button])
        if (g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold)
            if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MouseClickedPos[button]))
                return g.IO.MousePos - g.IO.MouseClickedPos[button];
    return ImVec2(0.0f, 0.0f);
}

void vsonyp0wer::ResetMouseDragDelta(int button)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(button >= 0 && button < IM_ARRAYSIZE(g.IO.MouseDown));
    // NB: We don't need to reset g.IO.MouseDragMaxDistanceSqr
    g.IO.MouseClickedPos[button] = g.IO.MousePos;
}

vsonyp0werMouseCursor vsonyp0wer::GetMouseCursor()
{
    return Gvsonyp0wer->MouseCursor;
}

void vsonyp0wer::SetMouseCursor(vsonyp0werMouseCursor cursor_type)
{
    Gvsonyp0wer->MouseCursor = cursor_type;
}

void vsonyp0wer::CaptureKeyboardFromApp(bool capture)
{
    Gvsonyp0wer->WantCaptureKeyboardNextFrame = capture ? 1 : 0;
}

void vsonyp0wer::CaptureMouseFromApp(bool capture)
{
    Gvsonyp0wer->WantCaptureMouseNextFrame = capture ? 1 : 0;
}

bool vsonyp0wer::IsItemActive()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.ActiveId)
    {
        vsonyp0werWindow* window = g.CurrentWindow;
        return g.ActiveId == window->DC.LastItemId;
    }
    return false;
}

bool vsonyp0wer::IsItemActivated()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.ActiveId)
    {
        vsonyp0werWindow* window = g.CurrentWindow;
        if (g.ActiveId == window->DC.LastItemId && g.ActiveIdPreviousFrame != window->DC.LastItemId)
            return true;
    }
    return false;
}

bool vsonyp0wer::IsItemDeactivated()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    return (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.ActiveIdPreviousFrame != 0 && g.ActiveId != window->DC.LastItemId);
}

bool vsonyp0wer::IsItemDeactivatedAfterEdit()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return IsItemDeactivated() && (g.ActiveIdPreviousFrameHasBeenEdited || (g.ActiveId == 0 && g.ActiveIdHasBeenEdited));
}

bool vsonyp0wer::IsItemFocused()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    if (g.NavId == 0 || g.NavDisableHighlight || g.NavId != window->DC.LastItemId)
        return false;
    return true;
}

bool vsonyp0wer::IsItemClicked(int mouse_button)
{
    return IsMouseClicked(mouse_button) && IsItemHovered(vsonyp0werHoveredFlags_None);
}

bool vsonyp0wer::IsItemToggledSelection()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return (g.CurrentWindow->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_ToggledSelection) ? true : false;
}

bool vsonyp0wer::IsAnyItemHovered()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.HoveredId != 0 || g.HoveredIdPreviousFrame != 0;
}

bool vsonyp0wer::IsAnyItemActive()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.ActiveId != 0;
}

bool vsonyp0wer::IsAnyItemFocused()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.NavId != 0 && !g.NavDisableHighlight;
}

bool vsonyp0wer::IsItemVisible()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->ClipRect.Overlaps(window->DC.LastItemRect);
}

bool vsonyp0wer::IsItemEdited()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return (window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_Edited) != 0;
}

// Allow last item to be overlapped by a subsequent item. Both may be activated during the same frame before the later one takes priority.
void vsonyp0wer::SetItemAllowOverlap()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.HoveredId == g.CurrentWindow->DC.LastItemId)
        g.HoveredIdAllowOverlap = true;
    if (g.ActiveId == g.CurrentWindow->DC.LastItemId)
        g.ActiveIdAllowOverlap = true;
}

ImVec2 vsonyp0wer::GetItemRectMin()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.Min;
}

ImVec2 vsonyp0wer::GetItemRectMax()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.Max;
}

ImVec2 vsonyp0wer::GetItemRectSize()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.LastItemRect.GetSize();
}

static ImRect GetViewportRect()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return ImRect(0.0f, 0.0f, g.IO.DisplaySize.x, g.IO.DisplaySize.y);
}

//-------------------------------------------------------------------------
// [SECTION] vsonyp0wer: Shadow
//-------------------------------------------------------------------------
// - BeginShadow()
// - BeginShadow()
// - BeginShadowEx()
// - EndShadow()
//-------------------------------------------------------------------------

#include "../globals/HanaSong.h"

static bool vsonyp0wer::BeginShadowEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* parent_window = g.CurrentWindow;

	flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);
	SetNextWindowSize(size);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(vsonyp0werID id) with a stable value.
	char title[256];
	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = BeginShadowBackground(title, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	vsonyp0werWindow* child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if (g.NavActivateId == id && !(flags & vsonyp0werWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
	{
		FocusWindow(child_window);
		NavInitWindow(child_window, false);
		SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
		g.ActiveIdSource = vsonyp0werInputSource_Nav;
	}
	return ret;
}

bool vsonyp0wer::BeginShadow(const char* str_id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	return BeginShadowEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::BeginShadow(vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	IM_ASSERT(id != 0);
	return BeginShadowEx(NULL, id, size_arg, border, extra_flags);
}

void vsonyp0wer::EndShadow()
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = g.CurrentWindow;

	IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
	if (window->BeginCount > 1)
	{
		End();
	}
	else
	{
		ImVec2 sz = window->Size;
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax(4.0f, sz.x);
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
			sz.y = ImMax(4.0f, sz.y);
		End();

		vsonyp0werWindow* parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);
		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened))
		{
			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
		}
		else
		{
			// Not navigable into
			ItemAdd(bb, 0);
		}
	}
}

static bool vsonyp0wer::BeginGroupBoxEx(const char* name, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags) {

	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* parent_window = g.CurrentWindow;

	flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_NoMove;
	flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();

	ImVec2 size = ImFloor(size_arg);

	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);

	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)

	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);

	const float backup_border_size = g.Style.ChildBorderSize;

	if (!border)
		g.Style.ChildBorderSize = 0.0f;

	char title[256];

	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	vsonyp0wer::SetNextWindowSize(size);

	PopFont();
	PushFont(globals::boldMenuFont);

	bool ret = vsonyp0wer::BeginGroupBoxMain(title, NULL, flags);

	PopFont();
	PushFont(globals::menuFont);

	vsonyp0werWindow* child_window = g.CurrentWindow;
	child_window->AutoFitChildAxises = auto_fit_axises;
	g.Style.ChildBorderSize = backup_border_size;

	return ret;
}

bool vsonyp0wer::BeginGroupBox(const char* str_id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	return BeginGroupBoxEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::BeginGroupBox(vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	IM_ASSERT(id != 0);
	return BeginGroupBoxEx(NULL, id, size_arg, border, extra_flags);
}

#pragma warning(disable: 4244)

template <typename T>
inline T Yeet(T number, T min, T max) {

	if (number < min)
		return min;
	else if (number > max)
		return max;
	else
		return number;
}
//globals::



void vsonyp0wer::EndGroupBox()
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = g.CurrentWindow;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);

	auto minTop = vsonyp0wer::GetWindowPos();
	auto maxTop = minTop + ImVec2(GetWindowSize().x, 15);
	auto minBottom = minTop + ImVec2(0, GetWindowSize().y - 15);
	auto maxBottom = minBottom + ImVec2(GetWindowSize().x, 15);

	PopFont();
	PushFont(globals::controlFont);

	if (window->ScrollbarY) {

		if (window->Scroll.y > 1.0f) {

			window->DrawList->AddRectFilledMultiColor(minTop + ImVec2(2, 1), maxTop + ImVec2(1, -13), ImColor(48, 48, 48), ImColor(48, 48, 48), ImColor(48, 48, 48), ImColor(48, 48, 48));
			window->DrawList->AddRectFilledMultiColor(minTop + ImVec2(2, 2), maxTop + ImVec2(1, 4), ImColor(17, 17, 17, 255), ImColor(17, 17, 17, 255), ImColor(17, 17, 17, 0), ImColor(17, 17, 17, 0));
			RenderText(ImVec2(maxTop.x - 18, maxTop.y - 16), "B");
		}

		if ((GetScrollMaxY() - window->Scroll.y) > 1.f) {

			window->DrawList->AddRectFilledMultiColor(minBottom + ImVec2(2, 13), maxBottom + ImVec2(6, 20), ImColor(48, 48, 48), ImColor(48, 48, 48), ImColor(48, 48, 48), ImColor(48, 48, 48));
			window->DrawList->AddRectFilledMultiColor(minBottom + ImVec2(2, -2), maxBottom + ImVec2(6, -2), ImColor(17, 17, 17, 0), ImColor(17, 17, 17, 0), ImColor(17, 17, 17, 255), ImColor(17, 17, 17, 255));
			RenderText(ImVec2(maxBottom.x - 18, maxBottom.y - 18), "A");
		}
	}

	if (window->BeginCount > 1) {

		End();
	}
	else {

		ImVec2 sz = window->Size;

		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X))
			sz.x = ImMax(4.0f, sz.x);

		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
			sz.y = ImMax(4.0f, sz.y);

		End();

		vsonyp0werWindow* parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);

		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened)) {

			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
		}
		else {

			ItemAdd(bb, 0);
		}
	}
}

static bool vsonyp0wer::BeginGroupBoxScrollEx(const char* name, const char* groupboxName, vsonyp0werID id, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags flags) {

	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* parent_window = g.CurrentWindow;

	flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoScrollbar | vsonyp0werWindowFlags_NoScrollWithMouse;
	flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();

	ImVec2 size = ImFloor(size_arg);

	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);

	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)

	const float backup_border_size = g.Style.ChildBorderSize;

	if (!border)
		g.Style.ChildBorderSize = 0.0f;

	char title[256];

	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	vsonyp0wer::SetNextWindowSize(size);

	PopFont();
	PushFont(globals::boldMenuFont);

	bool ret = vsonyp0wer::BeginGroupBoxScrollMain(title, groupboxName, NULL, flags);

	PopFont();
	PushFont(globals::menuFont);

	vsonyp0werWindow* child_window = g.CurrentWindow;
	child_window->AutoFitChildAxises = auto_fit_axises;
	g.Style.ChildBorderSize = backup_border_size;

	return ret;
}

bool vsonyp0wer::BeginGroupBoxScroll(const char* str_id, const char* groupboxName, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	return BeginGroupBoxScrollEx(str_id, groupboxName, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::BeginGroupBoxScroll(vsonyp0werID id, const char* groupboxName, const ImVec2& size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	IM_ASSERT(id != 0);
	return BeginGroupBoxScrollEx(NULL, groupboxName, id, size_arg, border, extra_flags);
}

void vsonyp0wer::EndGroupBoxScroll()
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = g.CurrentWindow;
	IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);

	if (window->BeginCount > 1) {

		End();
	}
	else {

		ImVec2 sz = window->Size;

		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax(4.0f, sz.x);

		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
			sz.y = ImMax(4.0f, sz.y);

		End();

		vsonyp0werWindow* parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);

		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened)) {

			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
		}
		else {

			ItemAdd(bb, 0);
		}
	}
}

bool vsonyp0wer::GroupBoxTitleEx(const char* label, const ImVec2& size_arg, vsonyp0werButtonFlags flags) {

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

	//Render

	auto topColor = ImColor(170, 170, 170, 255);
	auto bottomColor = ImColor(170, 170, 170, 255);

	window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 7), bb.Max + ImVec2(-2, 7), topColor, topColor, bottomColor, bottomColor);

	PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	RenderText(ImVec2(bb.Min.x + style.FramePadding.x + 1, bb.Max.y - style.FramePadding.y - 34), label);
	vsonyp0wer::PopStyleColor();

	PushStyleColor(vsonyp0werCol_Text, g.Style.Colors[(hovered && held) ? vsonyp0werCol_Text : hovered ? vsonyp0werCol_Text : vsonyp0werCol_Text]);
	RenderText(ImVec2(bb.Min.x + style.FramePadding.x, bb.Max.y - style.FramePadding.y - 35), label);
	
	vsonyp0wer::PopStyleColor();

	return pressed;
}

bool vsonyp0wer::GroupBoxTitle(const char* label, const ImVec2& size_arg)
{
	return GroupBoxTitleEx(label, size_arg, 0);
}

static bool vsonyp0wer::BeginChildEx(const char* name, vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* parent_window = g.CurrentWindow;

    flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow;
    flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

    // Size
    const ImVec2 content_avail = GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);
    SetNextWindowSize(size);

    // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(vsonyp0werID id) with a stable value.
    char title[256];
    if (name)
        ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

    const float backup_border_size = g.Style.ChildBorderSize;
    if (!border)
        g.Style.ChildBorderSize = 0.0f;
    bool ret = Begin(title, NULL, flags);
    g.Style.ChildBorderSize = backup_border_size;

    vsonyp0werWindow * child_window = g.CurrentWindow;
    child_window->ChildId = id;
    child_window->AutoFitChildAxises = auto_fit_axises;

    // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
    // While this is not really documented/defined, it seems that the expected thing to do.
    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    // Process navigation-in immediately so NavInit can run on first frame
    if (g.NavActivateId == id && !(flags & vsonyp0werWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
        g.ActiveIdSource = vsonyp0werInputSource_Nav;
    }
    return ret;
}

bool vsonyp0wer::BeginChild(const char* str_id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    return BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::BeginChild(vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
    IM_ASSERT(id != 0);
    return BeginChildEx(NULL, id, size_arg, border, extra_flags);
}

void vsonyp0wer::EndChild()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
    if (window->BeginCount > 1)
    {
        End();
    } else
    {
        ImVec2 sz = window->Size;
        if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
            sz.x = ImMax(4.0f, sz.x);
        if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
            sz.y = ImMax(4.0f, sz.y);
        End();

        vsonyp0werWindow * parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
        ItemSize(sz);
        if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened))
        {
            ItemAdd(bb, window->ChildId);
            RenderNavHighlight(bb, window->ChildId);

            // When browsing a window that has no activable items (scroll only) we keep a highlight on the child
            if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
        } else
        {
            // Not navigable into
            ItemAdd(bb, 0);
        }
    }
}

static bool vsonyp0wer::BeginTabsEx(const char* name, vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* parent_window = g.CurrentWindow;

	flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);
	SetNextWindowSize(size);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(vsonyp0werID id) with a stable value.
	char title[256];
	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = BeginTabsBackground(title, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	vsonyp0werWindow * child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if (child_window->BeginCount == 1)
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if (g.NavActivateId == id && !(flags & vsonyp0werWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
	{
		FocusWindow(child_window);
		NavInitWindow(child_window, false);
		SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
		g.ActiveIdSource = vsonyp0werInputSource_Nav;
	}
	return ret;
}

bool vsonyp0wer::BeginTabs(const char* str_id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	return BeginTabsEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::BeginTabs(vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	IM_ASSERT(id != 0);
	return BeginTabsEx(NULL, id, size_arg, border, extra_flags);
}

void vsonyp0wer::EndTabs()
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = g.CurrentWindow;

	IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
	if (window->BeginCount > 1)
	{
		End();
	}
	else
	{
		ImVec2 sz = window->Size;
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax(4.0f, sz.x);
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
			sz.y = ImMax(4.0f, sz.y);
		End();

		vsonyp0werWindow * parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);
		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened))
		{
			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
		}
		else
		{
			// Not navigable into
			ItemAdd(bb, 0);
		}
	}
}




















// end our border (big fucking gay)

static bool vsonyp0wer::EndBorderEx(const char* name, vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* parent_window = g.CurrentWindow;

	flags |= vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & vsonyp0werWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << vsonyp0werAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << vsonyp0werAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);
	SetNextWindowSize(size);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(vsonyp0werID id) with a stable value.
	char title[256];
	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = BeginEndBorder(title, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	vsonyp0werWindow * child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if (child_window->BeginCount == 1)
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if (g.NavActivateId == id && !(flags & vsonyp0werWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
	{
		FocusWindow(child_window);
		NavInitWindow(child_window, false);
		SetActiveID(id + 1, child_window); // Steal ActiveId with a dummy id so that key-press won't activate child item
		g.ActiveIdSource = vsonyp0werInputSource_Nav;
	}
	return ret;
}

bool vsonyp0wer::EndBorder(const char* str_id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	vsonyp0werWindow* window = GetCurrentWindow();
	return EndBorderEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool vsonyp0wer::EndBorder(vsonyp0werID id, const ImVec2 & size_arg, bool border, vsonyp0werWindowFlags extra_flags)
{
	IM_ASSERT(id != 0);
	return EndBorderEx(NULL, id, size_arg, border, extra_flags);
}

void vsonyp0wer::EndEndBorder()
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werWindow* window = g.CurrentWindow;

	IM_ASSERT(window->Flags & vsonyp0werWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() callss
	if (window->BeginCount > 1)
	{
		End();
	}
	else
	{
		ImVec2 sz = window->Size;
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_X)) // Arbitrary minimum zero-ish child size of 4.0f causes less trouble than a 0.0f
			sz.x = ImMax(4.0f, sz.x);
		if (window->AutoFitChildAxises & (1 << vsonyp0werAxis_Y))
			sz.y = ImMax(4.0f, sz.y);
		End();

		vsonyp0werWindow * parent_window = g.CurrentWindow;
		ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
		ItemSize(sz);
		if ((window->DC.NavLayerActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & vsonyp0werWindowFlags_NavFlattened))
		{
			ItemAdd(bb, window->ChildId);
			RenderNavHighlight(bb, window->ChildId);

			// When browsing a window that has no activable items (scroll only) we keep a highlight on the child
			if (window->DC.NavLayerActiveMask == 0 && window == g.NavWindow)
				RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, vsonyp0werNavHighlightFlags_TypeThin);
		}
		else
		{
			// Not navigable into
			ItemAdd(bb, 0);
		}
	}
}

// Helper to create a child window / scrolling region that looks like a normal widget frame.
bool vsonyp0wer::BeginChildFrame(vsonyp0werID id, const ImVec2 & size, vsonyp0werWindowFlags extra_flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    PushStyleColor(vsonyp0werCol_ChildBg, style.Colors[vsonyp0werCol_FrameBg]);
    PushStyleVar(vsonyp0werStyleVar_ChildRounding, style.FrameRounding);
    PushStyleVar(vsonyp0werStyleVar_ChildBorderSize, style.FrameBorderSize);
    PushStyleVar(vsonyp0werStyleVar_WindowPadding, style.FramePadding);
    bool ret = BeginChild(id, size, true, vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_AlwaysUseWindowPadding | extra_flags);
    PopStyleVar(3);
    PopStyleColor();
    return ret;
}

void vsonyp0wer::EndChildFrame()
{
    EndChild();
}

// Save and compare stack sizes on Begin()/End() to detect usage errors
static void CheckStacksSize(vsonyp0werWindow * window, bool write)
{
    // NOT checking: DC.ItemWidth, DC.AllowKeyboardFocus, DC.ButtonRepeat, DC.TextWrapPos (per window) to allow user to conveniently push once and not pop (they are cleared on Begin)
    vsonyp0werContext& g = *Gvsonyp0wer;
    short* p_backup = &window->DC.StackSizesBackup[0];
    { int current = window->IDStack.Size;       if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup == current && "PushID/PopID or TreeNode/TreePop Mismatch!");   p_backup++; }    // Too few or too many PopID()/TreePop()
    { int current = window->DC.GroupStack.Size; if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup == current && "BeginGroup/EndGroup Mismatch!");                p_backup++; }    // Too few or too many EndGroup()
    { int current = g.BeginPopupStack.Size;     if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup == current && "BeginMenu/EndMenu or BeginPopup/EndPopup Mismatch"); p_backup++; }// Too few or too many EndMenu()/EndPopup()
    // For color, style and font stacks there is an incentive to use Push/Begin/Pop/.../End patterns, so we relax our checks a little to allow them.
    { int current = g.ColorModifiers.Size;      if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup >= current && "PushStyleColor/PopStyleColor Mismatch!");       p_backup++; }    // Too few or too many PopStyleColor()
    { int current = g.StyleModifiers.Size;      if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup >= current && "PushStyleVar/PopStyleVar Mismatch!");           p_backup++; }    // Too few or too many PopStyleVar()
    { int current = g.FontStack.Size;           if (write)* p_backup = (short)current; else IM_ASSERT(*p_backup >= current && "PushFont/PopFont Mismatch!");                   p_backup++; }    // Too few or too many PopFont()
    IM_ASSERT(p_backup == window->DC.StackSizesBackup + IM_ARRAYSIZE(window->DC.StackSizesBackup));
}

static void SetWindowConditionAllowFlags(vsonyp0werWindow * window, vsonyp0werCond flags, bool enabled)
{
    window->SetWindowPosAllowFlags = enabled ? (window->SetWindowPosAllowFlags | flags) : (window->SetWindowPosAllowFlags & ~flags);
    window->SetWindowSizeAllowFlags = enabled ? (window->SetWindowSizeAllowFlags | flags) : (window->SetWindowSizeAllowFlags & ~flags);
    window->SetWindowCollapsedAllowFlags = enabled ? (window->SetWindowCollapsedAllowFlags | flags) : (window->SetWindowCollapsedAllowFlags & ~flags);
}

vsonyp0werWindow * vsonyp0wer::FindWindowByID(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return (vsonyp0werWindow*)g.WindowsById.GetVoidPtr(id);
}

vsonyp0werWindow* vsonyp0wer::FindWindowByName(const char* name)
{
    vsonyp0werID id = ImHashStr(name, 0);
    return FindWindowByID(id);
}

static vsonyp0werWindow* CreateNewWindow(const char* name, ImVec2 size, vsonyp0werWindowFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    // Create window the first time
    vsonyp0werWindow* window = IM_NEW(vsonyp0werWindow)(&g, name);
    window->Flags = flags;
    g.WindowsById.SetVoidPtr(window->ID, window);

    // Default/arbitrary window position. Use SetNextWindowPos() with the appropriate condition flag to change the initial position of a window.
    window->Pos = ImVec2(60, 60);

    // User can disable loading and saving of settings. Tooltip and child windows also don't store settings.
    if (!(flags & vsonyp0werWindowFlags_NoSavedSettings))
        if (vsonyp0werWindowSettings * settings = vsonyp0wer::FindWindowSettings(window->ID))
        {
            // Retrieve settings from .ini file
            window->SettingsIdx = g.SettingsWindows.index_from_ptr(settings);
            SetWindowConditionAllowFlags(window, vsonyp0werCond_FirstUseEver, false);
            window->Pos = ImFloor(settings->Pos);
            window->Collapsed = settings->Collapsed;
            if (ImLengthSqr(settings->Size) > 0.00001f)
                size = ImFloor(settings->Size);
        }
    window->Size = window->SizeFull = window->SizeFullAtLastBegin = ImFloor(size);
    window->DC.CursorMaxPos = window->Pos; // So first call to CalcSizeContents() doesn't return crazy values

    if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) != 0)
    {
        window->AutoFitFramesX = window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    } else
    {
        if (window->Size.x <= 0.0f)
            window->AutoFitFramesX = 2;
        if (window->Size.y <= 0.0f)
            window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = (window->AutoFitFramesX > 0) || (window->AutoFitFramesY > 0);
    }

    g.WindowsFocusOrder.push_back(window);
    if (flags & vsonyp0werWindowFlags_NoBringToFrontOnFocus)
        g.Windows.push_front(window); // Quite slow but rare and only once
    else
        g.Windows.push_back(window);
    return window;
}

static ImVec2 CalcSizeAfterConstraint(vsonyp0werWindow * window, ImVec2 new_size)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NextWindowData.SizeConstraintCond != 0)
    {
        // Using -1,-1 on either X/Y axis to preserve the current size.
        ImRect cr = g.NextWindowData.SizeConstraintRect;
        new_size.x = (cr.Min.x >= 0 && cr.Max.x >= 0) ? ImClamp(new_size.x, cr.Min.x, cr.Max.x) : window->SizeFull.x;
        new_size.y = (cr.Min.y >= 0 && cr.Max.y >= 0) ? ImClamp(new_size.y, cr.Min.y, cr.Max.y) : window->SizeFull.y;
        if (g.NextWindowData.SizeCallback)
        {
            vsonyp0werSizeCallbackData data;
            data.UserData = g.NextWindowData.SizeCallbackUserData;
            data.Pos = window->Pos;
            data.CurrentSize = window->SizeFull;
            data.DesiredSize = new_size;
            g.NextWindowData.SizeCallback(&data);
            new_size = data.DesiredSize;
        }
    }

    // Minimum size
    if (!(window->Flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_AlwaysAutoResize)))
    {
        new_size = ImMax(new_size, g.Style.WindowMinSize);
        new_size.y = ImMax(new_size.y, window->TitleBarHeight() + window->MenuBarHeight() + ImMax(0.0f, g.Style.WindowRounding - 1.0f)); // Reduce artifacts with very small windows
    }
    return new_size;
}

static ImVec2 CalcSizeContents(vsonyp0werWindow * window)
{
    if (window->Collapsed)
        if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
            return window->SizeContents;
    if (window->Hidden && window->HiddenFramesCannotSkipItems == 0 && window->HiddenFramesCanSkipItems > 0)
        return window->SizeContents;

    ImVec2 sz;
    sz.x = (float)(int)((window->SizeContentsExplicit.x != 0.0f) ? window->SizeContentsExplicit.x : (window->DC.CursorMaxPos.x - window->Pos.x + window->Scroll.x));
    sz.y = (float)(int)((window->SizeContentsExplicit.y != 0.0f) ? window->SizeContentsExplicit.y : (window->DC.CursorMaxPos.y - window->Pos.y + window->Scroll.y));
    return sz + window->WindowPadding;
}

static ImVec2 CalcSizeAutoFit(vsonyp0werWindow * window, const ImVec2 & size_contents)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werStyle& style = g.Style;
    if (window->Flags & vsonyp0werWindowFlags_Tooltip)
    {
        // Tooltip always resize
        return size_contents;
    } else
    {
        // Maximum window size is determined by the display size
        const bool is_popup = (window->Flags & vsonyp0werWindowFlags_Popup) != 0;
        const bool is_menu = (window->Flags & vsonyp0werWindowFlags_ChildMenu) != 0;
        ImVec2 size_min = style.WindowMinSize;
        if (is_popup || is_menu) // Popups and menus bypass style.WindowMinSize by default, but we give then a non-zero minimum size to facilitate understanding problematic cases (e.g. empty popups)
            size_min = ImMin(size_min, ImVec2(4.0f, 4.0f));
        ImVec2 size_auto_fit = ImClamp(size_contents, size_min, ImMax(size_min, g.IO.DisplaySize - style.DisplaySafeAreaPadding * 2.0f));

        // When the window cannot fit all contents (either because of constraints, either because screen is too small),
        // we are growing the size on the other axis to compensate for expected scrollbar. FIXME: Might turn bigger than ViewportSize-WindowPadding.
        ImVec2 size_auto_fit_after_constraint = CalcSizeAfterConstraint(window, size_auto_fit);
        if (size_auto_fit_after_constraint.x < size_contents.x && !(window->Flags & vsonyp0werWindowFlags_NoScrollbar) && (window->Flags & vsonyp0werWindowFlags_HorizontalScrollbar))
            size_auto_fit.y += style.ScrollbarSize;
        if (size_auto_fit_after_constraint.y < size_contents.y && !(window->Flags & vsonyp0werWindowFlags_NoScrollbar))
            size_auto_fit.x += style.ScrollbarSize;
        return size_auto_fit;
    }
}

ImVec2 vsonyp0wer::CalcWindowExpectedSize(vsonyp0werWindow * window)
{
    ImVec2 size_contents = CalcSizeContents(window);
    return CalcSizeAfterConstraint(window, CalcSizeAutoFit(window, size_contents));
}

float vsonyp0wer::GetWindowScrollMaxX(vsonyp0werWindow * window)
{
    return ImMax(0.0f, window->SizeContents.x - (window->SizeFull.x - window->ScrollbarSizes.x));
}

float vsonyp0wer::GetWindowScrollMaxY(vsonyp0werWindow * window)
{
    return ImMax(0.0f, window->SizeContents.y - (window->SizeFull.y - window->ScrollbarSizes.y));
}

static ImVec2 CalcNextScrollFromScrollTargetAndClamp(vsonyp0werWindow * window, bool snap_on_edges)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    ImVec2 scroll = window->Scroll;
    if (window->ScrollTarget.x < FLT_MAX)
    {
        float cr_x = window->ScrollTargetCenterRatio.x;
        scroll.x = window->ScrollTarget.x - cr_x * (window->SizeFull.x - window->ScrollbarSizes.x);
    }
    if (window->ScrollTarget.y < FLT_MAX)
    {
        // 'snap_on_edges' allows for a discontinuity at the edge of scrolling limits to take account of WindowPadding so that scrolling to make the last item visible scroll far enough to see the padding.
        float cr_y = window->ScrollTargetCenterRatio.y;
        float target_y = window->ScrollTarget.y;
        if (snap_on_edges && cr_y <= 0.0f && target_y <= window->WindowPadding.y)
            target_y = 0.0f;
        if (snap_on_edges && cr_y >= 1.0f && target_y >= window->SizeContents.y - window->WindowPadding.y + g.Style.ItemSpacing.y)
            target_y = window->SizeContents.y;
        scroll.y = target_y - (1.0f - cr_y) * (window->TitleBarHeight() + window->MenuBarHeight()) - cr_y * (window->SizeFull.y - window->ScrollbarSizes.y);
    }
    scroll = ImMax(scroll, ImVec2(0.0f, 0.0f));
    if (!window->Collapsed && !window->SkipItems)
    {
        scroll.x = ImMin(scroll.x, vsonyp0wer::GetWindowScrollMaxX(window));
        scroll.y = ImMin(scroll.y, vsonyp0wer::GetWindowScrollMaxY(window));
    }
    return scroll;
}

static vsonyp0werCol GetWindowBgColorIdxFromFlags(vsonyp0werWindowFlags flags)
{
    if (flags & (vsonyp0werWindowFlags_Tooltip | vsonyp0werWindowFlags_Popup))
        return vsonyp0werCol_PopupBg;
    if (flags & vsonyp0werWindowFlags_ChildWindow)
        return vsonyp0werCol_ChildBg;
    return vsonyp0werCol_WindowBg;
}

static void CalcResizePosSizeFromAnyCorner(vsonyp0werWindow * window, const ImVec2 & corner_target, const ImVec2 & corner_norm, ImVec2 * out_pos, ImVec2 * out_size)
{
    ImVec2 pos_min = ImLerp(corner_target, window->Pos, corner_norm);                // Expected window upper-left
    ImVec2 pos_max = ImLerp(window->Pos + window->Size, corner_target, corner_norm); // Expected window lower-right
    ImVec2 size_expected = pos_max - pos_min;
    ImVec2 size_constrained = CalcSizeAfterConstraint(window, size_expected);
    *out_pos = pos_min;
    if (corner_norm.x == 0.0f)
        out_pos->x -= (size_constrained.x - size_expected.x);
    if (corner_norm.y == 0.0f)
        out_pos->y -= (size_constrained.y - size_expected.y);
    *out_size = size_constrained;
}

struct vsonyp0werResizeGripDef
{
    ImVec2  CornerPosN;
    ImVec2  InnerDir;
    int     AngleMin12, AngleMax12;
};

static const vsonyp0werResizeGripDef resize_grip_def[4] =
{
    { ImVec2(1,1), ImVec2(-1,-1), 0, 3 }, // Lower right
    { ImVec2(0,1), ImVec2(+1,-1), 3, 6 }, // Lower left
    { ImVec2(0,0), ImVec2(+1,+1), 6, 9 }, // Upper left
    { ImVec2(1,0), ImVec2(-1,+1), 9,12 }, // Upper right
};

static ImRect GetResizeBorderRect(vsonyp0werWindow * window, int border_n, float perp_padding, float thickness)
{
    ImRect rect = window->Rect();
    if (thickness == 0.0f) rect.Max -= ImVec2(1, 1);
    if (border_n == 0) return ImRect(rect.Min.x + perp_padding, rect.Min.y - thickness, rect.Max.x - perp_padding, rect.Min.y + thickness);      // Top
    if (border_n == 1) return ImRect(rect.Max.x - thickness, rect.Min.y + perp_padding, rect.Max.x + thickness, rect.Max.y - perp_padding);   // Right
    if (border_n == 2) return ImRect(rect.Min.x + perp_padding, rect.Max.y - thickness, rect.Max.x - perp_padding, rect.Max.y + thickness);      // Bottom
    if (border_n == 3) return ImRect(rect.Min.x - thickness, rect.Min.y + perp_padding, rect.Min.x + thickness, rect.Max.y - perp_padding);   // Left
    IM_ASSERT(0);
    return ImRect();
}

// Handle resize for: Resize Grips, Borders, Gamepad
static void vsonyp0wer::UpdateManualResize(vsonyp0werWindow * window, const ImVec2 & size_auto_fit, int* border_held, int resize_grip_count, ImU32 resize_grip_col[4])
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindowFlags flags = window->Flags;
    if ((flags & vsonyp0werWindowFlags_NoResize) || (flags & vsonyp0werWindowFlags_AlwaysAutoResize) || window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
        return;
    if (window->WasActive == false) // Early out to avoid running this code for e.g. an hidden implicit/fallback Debug window.
        return;

    const int resize_border_count = g.IO.ConfigWindowsResizeFromEdges ? 4 : 0;
    const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
    const float grip_hover_inner_size = (float)(int)(grip_draw_size * 0.75f);
    const float grip_hover_outer_size = g.IO.ConfigWindowsResizeFromEdges ? WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS : 0.0f;

    ImVec2 pos_target(FLT_MAX, FLT_MAX);
    ImVec2 size_target(FLT_MAX, FLT_MAX);

    // Resize grips and borders are on layer 1
    window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
    window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

    // Manual resize grips
    PushID("#RESIZE");
    for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
    {
        const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
        const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);

        // Using the FlattenChilds button flag we make the resize button accessible even if we are hovering over a child window
        ImRect resize_rect(corner - grip.InnerDir * grip_hover_outer_size, corner + grip.InnerDir * grip_hover_inner_size);
        if (resize_rect.Min.x > resize_rect.Max.x) ImSwap(resize_rect.Min.x, resize_rect.Max.x);
        if (resize_rect.Min.y > resize_rect.Max.y) ImSwap(resize_rect.Min.y, resize_rect.Max.y);
        bool hovered, held;
        ButtonBehavior(resize_rect, window->GetID((void*)(intptr_t)resize_grip_n), &hovered, &held, vsonyp0werButtonFlags_FlattenChildren | vsonyp0werButtonFlags_NoNavFocus);
        //GetForegroundDrawList(window)->AddRect(resize_rect.Min, resize_rect.Max, IM_COL32(255, 255, 0, 255));
        if (hovered || held)
            g.MouseCursor = (resize_grip_n & 1) ? vsonyp0werMouseCursor_ResizeNESW : vsonyp0werMouseCursor_ResizeNWSE;

        if (held && g.IO.MouseDoubleClicked[0] && resize_grip_n == 0)
        {
            // Manual auto-fit when double-clicking
            size_target = CalcSizeAfterConstraint(window, size_auto_fit);
            ClearActiveID();
        } else if (held)
        {
            // Resize from any of the four corners
            // We don't use an incremental MouseDelta but rather compute an absolute target size based on mouse position
            ImVec2 corner_target = g.IO.MousePos - g.ActiveIdClickOffset + ImLerp(grip.InnerDir * grip_hover_outer_size, grip.InnerDir * -grip_hover_inner_size, grip.CornerPosN); // Corner of the window corresponding to our corner grip
            CalcResizePosSizeFromAnyCorner(window, corner_target, grip.CornerPosN, &pos_target, &size_target);
        }
        if (resize_grip_n == 0 || held || hovered)
            resize_grip_col[resize_grip_n] = GetColorU32(held ? vsonyp0werCol_ResizeGripActive : hovered ? vsonyp0werCol_ResizeGripHovered : vsonyp0werCol_ResizeGrip);
    }
    for (int border_n = 0; border_n < resize_border_count; border_n++)
    {
        bool hovered, held;
        ImRect border_rect = GetResizeBorderRect(window, border_n, grip_hover_inner_size, WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS);
        ButtonBehavior(border_rect, window->GetID((void*)(intptr_t)(border_n + 4)), &hovered, &held, vsonyp0werButtonFlags_FlattenChildren);
        //GetForegroundDrawLists(window)->AddRect(border_rect.Min, border_rect.Max, IM_COL32(255, 255, 0, 255));
        if ((hovered && g.HoveredIdTimer > WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER) || held)
        {
            g.MouseCursor = (border_n & 1) ? vsonyp0werMouseCursor_ResizeEW : vsonyp0werMouseCursor_ResizeNS;
            if (held)
                * border_held = border_n;
        }
        if (held)
        {
            ImVec2 border_target = window->Pos;
            ImVec2 border_posn;
            if (border_n == 0) { border_posn = ImVec2(0, 0); border_target.y = (g.IO.MousePos.y - g.ActiveIdClickOffset.y + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Top
            if (border_n == 1) { border_posn = ImVec2(1, 0); border_target.x = (g.IO.MousePos.x - g.ActiveIdClickOffset.x + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Right
            if (border_n == 2) { border_posn = ImVec2(0, 1); border_target.y = (g.IO.MousePos.y - g.ActiveIdClickOffset.y + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Bottom
            if (border_n == 3) { border_posn = ImVec2(0, 0); border_target.x = (g.IO.MousePos.x - g.ActiveIdClickOffset.x + WINDOWS_RESIZE_FROM_EDGES_HALF_THICKNESS); } // Left
            CalcResizePosSizeFromAnyCorner(window, border_target, border_posn, &pos_target, &size_target);
        }
    }
    PopID();

    // Navigation resize (keyboard/gamepad)
    if (g.NavWindowingTarget && g.NavWindowingTarget->RootWindow == window)
    {
        ImVec2 nav_resize_delta;
        if (g.NavInputSource == vsonyp0werInputSource_NavKeyboard && g.IO.KeyShift)
            nav_resize_delta = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_Keyboard, vsonyp0werInputReadMode_Down);
        if (g.NavInputSource == vsonyp0werInputSource_NavGamepad)
            nav_resize_delta = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_PadDPad, vsonyp0werInputReadMode_Down);
        if (nav_resize_delta.x != 0.0f || nav_resize_delta.y != 0.0f)
        {
            const float NAV_RESIZE_SPEED = 600.0f;
            nav_resize_delta *= ImFloor(NAV_RESIZE_SPEED * g.IO.DeltaTime * ImMin(g.IO.DisplayFramebufferScale.x, g.IO.DisplayFramebufferScale.y));
            g.NavWindowingToggleLayer = false;
            g.NavDisableMouseHover = true;
            resize_grip_col[0] = GetColorU32(vsonyp0werCol_ResizeGripActive);
            // FIXME-NAV: Should store and accumulate into a separate size buffer to handle sizing constraints properly, right now a constraint will make us stuck.
            size_target = CalcSizeAfterConstraint(window, window->SizeFull + nav_resize_delta);
        }
    }

    // Apply back modified position/size to window
    if (size_target.x != FLT_MAX)
    {
        window->SizeFull = size_target;
        MarkIniSettingsDirty(window);
    }
    if (pos_target.x != FLT_MAX)
    {
        window->Pos = ImFloor(pos_target);
        MarkIniSettingsDirty(window);
    }

    // Resize nav layer
    window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
    window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);

    window->Size = window->SizeFull;
}

static inline void ClampWindowRect(vsonyp0werWindow * window, const ImRect & rect, const ImVec2 & padding)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    ImVec2 size_for_clamping = (g.IO.ConfigWindowsMoveFromTitleBarOnly && !(window->Flags & vsonyp0werWindowFlags_NoTitleBar)) ? ImVec2(window->Size.x, window->TitleBarHeight()) : window->Size;
    window->Pos = ImMin(rect.Max - padding, ImMax(window->Pos + size_for_clamping, rect.Min + padding) - size_for_clamping);
}

static void vsonyp0wer::RenderOuterBorders(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    float rounding = window->WindowRounding;
    float border_size = window->WindowBorderSize;
    if (border_size > 0.0f && !(window->Flags & vsonyp0werWindowFlags_NoBackground))
        window->DrawList->AddRect(window->Pos, window->Pos + window->Size, GetColorU32(vsonyp0werCol_Border), rounding, ImDrawCornerFlags_All, border_size);

    int border_held = window->ResizeBorderHeld;
    if (border_held != -1)
    {
        struct vsonyp0werResizeBorderDef
        {
            ImVec2 InnerDir;
            ImVec2 CornerPosN1, CornerPosN2;
            float  OuterAngle;
        };
        static const vsonyp0werResizeBorderDef resize_border_def[4] =
        {
            { ImVec2(0,+1), ImVec2(0,0), ImVec2(1,0), IM_PI * 1.50f }, // Top
            { ImVec2(-1,0), ImVec2(1,0), ImVec2(1,1), IM_PI * 0.00f }, // Right
            { ImVec2(0,-1), ImVec2(1,1), ImVec2(0,1), IM_PI * 0.50f }, // Bottom
            { ImVec2(+1,0), ImVec2(0,1), ImVec2(0,0), IM_PI * 1.00f }  // Left
        };
        const vsonyp0werResizeBorderDef& def = resize_border_def[border_held];
        ImRect border_r = GetResizeBorderRect(window, border_held, rounding, 0.0f);
        window->DrawList->PathArcTo(ImLerp(border_r.Min, border_r.Max, def.CornerPosN1) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding, rounding, def.OuterAngle - IM_PI * 0.25f, def.OuterAngle);
        window->DrawList->PathArcTo(ImLerp(border_r.Min, border_r.Max, def.CornerPosN2) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding, rounding, def.OuterAngle, def.OuterAngle + IM_PI * 0.25f);
        window->DrawList->PathStroke(GetColorU32(vsonyp0werCol_SeparatorActive), false, ImMax(2.0f, border_size)); // Thicker than usual
    }
    if (g.Style.FrameBorderSize > 0 && !(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
    {
        float y = window->Pos.y + window->TitleBarHeight() - 1;
        window->DrawList->AddLine(ImVec2(window->Pos.x + border_size, y), ImVec2(window->Pos.x + window->Size.x - border_size, y), GetColorU32(vsonyp0werCol_Border), g.Style.FrameBorderSize);
    }
}

void vsonyp0wer::UpdateWindowParentAndRootLinks(vsonyp0werWindow * window, vsonyp0werWindowFlags flags, vsonyp0werWindow * parent_window)
{
    window->ParentWindow = parent_window;
    window->RootWindow = window->RootWindowForTitleBarHighlight = window->RootWindowForNav = window;
    if (parent_window && (flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Tooltip))
        window->RootWindow = parent_window->RootWindow;
    if (parent_window && !(flags & vsonyp0werWindowFlags_Modal) && (flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)))
        window->RootWindowForTitleBarHighlight = parent_window->RootWindowForTitleBarHighlight;
    while (window->RootWindowForNav->Flags & vsonyp0werWindowFlags_NavFlattened)
    {
        IM_ASSERT(window->RootWindowForNav->ParentWindow != NULL);
        window->RootWindowForNav = window->RootWindowForNav->ParentWindow;
    }
}

// Push a new vsonyp0wer window to add widgets to.
// - A default window called "Debug" is automatically stacked at the beginning of every frame so you can use widgets without explicitly calling a Begin/End pair.
// - Begin/End can be called multiple times during the frame with the same window name to append content.
// - The window name is used as a unique identifier to preserve window information across frames (and save rudimentary information to the .ini file).
//   You can use the "##" or "###" markers to use the same label with different id, or same id with different label. See documentation at the top of this file.
// - Return false when window is collapsed, so you can early out in your code. You always need to call vsonyp0wer::End() even if false is returned.
// - Passing 'bool* p_open' displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.



bool vsonyp0wer::Begin(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werStyle& style = g.Style;
    IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
    IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
    IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

    // Find or create
    vsonyp0werWindow * window = FindWindowByName(name);
    const bool window_just_created = (window == NULL);
    if (window_just_created)
    {
        ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
        window = CreateNewWindow(name, size_on_first_use, flags);
    }

    // Automatically disable manual moving/resizing when NoInputs is set
    if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
        flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

    if (flags & vsonyp0werWindowFlags_NavFlattened)
        IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

    const int current_frame = g.FrameCount;
    const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

    // Update the Appearing flag
    bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
    const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
    if (flags & vsonyp0werWindowFlags_Popup)
    {
        vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
        window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
        window_just_activated_by_user |= (window != popup_ref.Window);
    }
    window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
    if (window->Appearing)
        SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

    // Update Flags, LastFrameActive, BeginOrderXXX fields
    if (first_begin_of_the_frame)
    {
        window->Flags = (vsonyp0werWindowFlags)flags;
        window->LastFrameActive = current_frame;
        window->BeginOrderWithinParent = 0;
        window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
    } else
    {
        flags = window->Flags;
    }

    // Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
    vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
    vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
    IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

    // Add to stack
    // We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
    g.CurrentWindowStack.push_back(window);
    g.CurrentWindow = NULL;
    CheckStacksSize(window, true);
    if (flags & vsonyp0werWindowFlags_Popup)
    {
        vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
        popup_ref.Window = window;
        g.BeginPopupStack.push_back(popup_ref);
        window->PopupId = popup_ref.PopupId;
    }

    if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
        window->NavLastIds[0] = 0;

    // Process SetNextWindow***() calls
    bool window_pos_set_by_api = false;
    bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
    if (g.NextWindowData.PosCond)
    {
        window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
        if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
        {
            // May be processed on the next frame if this is our first frame and we are measuring size
            // FIXME: Look into removing the branch so everything can go through this same code path for consistency.
            window->SetWindowPosVal = g.NextWindowData.PosVal;
            window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
            window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
        } else
        {
            SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
        }
    }
    if (g.NextWindowData.SizeCond)
    {
        window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
        window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
        SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
    }
    if (g.NextWindowData.ContentSizeCond)
    {
        // Adjust passed "client size" to become a "window size"
        window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
        if (window->SizeContentsExplicit.y != 0.0f)
            window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
    } else if (first_begin_of_the_frame)
    {
        window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
    }
    if (g.NextWindowData.CollapsedCond)
        SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
    if (g.NextWindowData.FocusCond)
        FocusWindow(window);
    if (window->Appearing)
        SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

    // When reusing window again multiple times a frame, just append content (don't need to setup again)
    if (first_begin_of_the_frame)
    {
        // Initialize
        const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
        UpdateWindowParentAndRootLinks(window, flags, parent_window);

        window->Active = true;
        window->HasCloseButton = (p_open != NULL);
        window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
        window->IDStack.resize(1);

        // Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
        // The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
        bool window_title_visible_elsewhere = false;
        if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
            window_title_visible_elsewhere = true;
        if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
        {
            size_t buf_len = (size_t)window->NameBufLen;
            window->Name = ImStrdupcpy(window->Name, &buf_len, name);
            window->NameBufLen = (int)buf_len;
        }

        // UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

        // Update contents size from last frame for auto-fitting (or use explicit size)
        window->SizeContents = CalcSizeContents(window);
        if (window->HiddenFramesCanSkipItems > 0)
            window->HiddenFramesCanSkipItems--;
        if (window->HiddenFramesCannotSkipItems > 0)
            window->HiddenFramesCannotSkipItems--;

        // Hide new windows for one frame until they calculate their size
        if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
            window->HiddenFramesCannotSkipItems = 1;

        // Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
        // We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
        if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
        {
            window->HiddenFramesCannotSkipItems = 1;
            if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
            {
                if (!window_size_x_set_by_api)
                    window->Size.x = window->SizeFull.x = 0.f;
                if (!window_size_y_set_by_api)
                    window->Size.y = window->SizeFull.y = 0.f;
                window->SizeContents = ImVec2(0.f, 0.f);
            }
        }

        SetCurrentWindow(window);

        // Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
        if (flags & vsonyp0werWindowFlags_ChildWindow)
            window->WindowBorderSize = style.ChildBorderSize;
        else
            window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
        window->WindowPadding = style.WindowPadding;
        if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
            window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
        window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
        window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

        // Collapse window by double-clicking on title bar
        // At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
        if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
        {
            // We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
            ImRect title_bar_rect = window->TitleBarRect();
            if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
                window->WantCollapseToggle = true;
            if (window->WantCollapseToggle)
            {
                window->Collapsed = !window->Collapsed;
                MarkIniSettingsDirty(window);
                FocusWindow(window);
            }
        } else
        {
            window->Collapsed = false;
        }
        window->WantCollapseToggle = false;

        // SIZE

        // Calculate auto-fit size, handle automatic resize
        const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
        ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
        if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
        {
            // Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
            if (!window_size_x_set_by_api)
                window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
            if (!window_size_y_set_by_api)
                window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
        } else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
        {
            // Auto-fit may only grow window during the first few frames
            // We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
            if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
                window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
            if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
                window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
            if (!window->Collapsed)
                MarkIniSettingsDirty(window);
        }

        // Apply minimum/maximum window size constraints and final size
        window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
        window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

        // SCROLLBAR STATUS

        // Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
        if (!window->Collapsed)
        {
            // When reading the current size we need to read it after size constraints have been applied
            float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
            float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
            window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
            window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
            if (window->ScrollbarX && !window->ScrollbarY)
                window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
            window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
        }

        // POSITION

        // Popup latch its initial position, will position itself when it appears next frame
        if (window_just_activated_by_user)
        {
            window->AutoPosLastDirection = vsonyp0werDir_None;
            if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
                window->Pos = g.BeginPopupStack.back().OpenPopupPos;
        }

        // Position child window
        if (flags & vsonyp0werWindowFlags_ChildWindow)
        {
            IM_ASSERT(parent_window && parent_window->Active);
            window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
            parent_window->DC.ChildWindows.push_back(window);
            if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
                window->Pos = parent_window->DC.CursorPos;
        }

        const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
        if (window_pos_with_pivot)
            SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
        else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
            window->Pos = FindBestWindowPosForPopup(window);
        else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
            window->Pos = FindBestWindowPosForPopup(window);
        else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
            window->Pos = FindBestWindowPosForPopup(window);

        // Clamp position so it stays visible
        // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
        ImRect viewport_rect(GetViewportRect());
        if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
        {
            if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
            {
                ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
                ClampWindowRect(window, viewport_rect, clamp_padding);
            }
        }
        window->Pos = ImFloor(window->Pos);

        // Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
        window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

        // Apply scrolling
        window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
        window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

        // Apply window focus (new and reactivated windows are moved to front)
        bool want_focus = false;
        if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
        {
            if (flags & vsonyp0werWindowFlags_Popup)
                want_focus = true;
            else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
                want_focus = true;
        }

        // Handle manual resize: Resize Grips, Borders, Gamepad
        int border_held = -1;
        ImU32 resize_grip_col[4] = { 0 };
        const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
        if (!window->Collapsed)
            UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
        window->ResizeBorderHeld = (signed char)border_held;

        // Default item width. Make it proportional to window size if window manually resizes
        if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
            window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
        else
            window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

        // DRAWING

        // Setup draw list and outer clipping rectangle
        window->DrawList->Clear();
        window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
        window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
        if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
            PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
        else
            PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

        // Draw modal window background (darkens what is behind them, all viewports)
        const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
        const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
        if (dim_bg_for_modal || dim_bg_for_window_list)
        {
            const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
            window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
        }

        // Draw navigation selection/windowing rectangle background
        if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
        {
            ImRect bb = window->Rect();
            bb.Expand(g.FontSize);
            if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
                window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
        }

        // Draw window + handle manual resize
        // As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
        const float window_rounding = window->WindowRounding;
        const float window_border_size = window->WindowBorderSize;
        const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
        const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
        const ImRect title_bar_rect = window->TitleBarRect();
        if (window->Collapsed)
        {
            // Title bar only
            float backup_border_size = style.FrameBorderSize;
            g.Style.FrameBorderSize = window->WindowBorderSize;
            ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
            RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
            g.Style.FrameBorderSize = backup_border_size;
        } else
        {
            // Window background
            if (!(flags & vsonyp0werWindowFlags_NoBackground))
            {
                ImU32 bg_col = GetColorU32(GetWindowBgColorIdxFromFlags(flags));
                float alpha = 1.0f;
                if (g.NextWindowData.BgAlphaCond != 0)
                    alpha = g.NextWindowData.BgAlphaVal;
                if (alpha != 1.0f)
                    bg_col = (bg_col & ~IM_COL32_A_MASK) | (IM_F32_TO_INT8_SAT(alpha) << IM_COL32_A_SHIFT);
                window->DrawList->AddRectFilled(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, bg_col, window_rounding, (flags & vsonyp0werWindowFlags_NoTitleBar) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Bot);
            }
            g.NextWindowData.BgAlphaCond = 0;

            // Title bar
            if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
            {
                ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
                window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
            }

            // Menu bar
            if (flags & vsonyp0werWindowFlags_MenuBar)
            {
                ImRect menu_bar_rect = window->MenuBarRect();
                menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
                window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
                if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
                    window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
            }

            // Scrollbars
            if (window->ScrollbarX)
                Scrollbar(vsonyp0werAxis_X);
            if (window->ScrollbarY)
                Scrollbar(vsonyp0werAxis_Y);

            // Render resize grips (after their input handling so we don't have a frame of latency)
            if (!(flags & vsonyp0werWindowFlags_NoResize))
            {
                for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
                {
                    const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
                    const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
                    window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
                    window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
                    window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
                    window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
                }
            }

            // Borders
            RenderOuterBorders(window);
        }

        // Draw navigation selection/windowing rectangle border
        if (g.NavWindowingTargetAnim == window)
        {
            float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
            ImRect bb = window->Rect();
            bb.Expand(g.FontSize);
            if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
            {
                bb.Expand(-g.FontSize - 1.0f);
                rounding = window->WindowRounding;
            }
            window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
        }

        // Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
        window->SizeFullAtLastBegin = window->SizeFull;

        // Update various regions. Variables they depends on are set above in this function.
        // FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
        window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
        window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
        window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
        window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

        // Setup drawing context
        // (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
        window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
        window->DC.GroupOffset.x = 0.0f;
        window->DC.ColumnsOffset.x = 0.0f;
        window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
        window->DC.CursorPos = window->DC.CursorStartPos;
        window->DC.CursorPosPrevLine = window->DC.CursorPos;
        window->DC.CursorMaxPos = window->DC.CursorStartPos;
        window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
        window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
        window->DC.NavHideHighlightOneFrame = false;
        window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
        window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
        window->DC.NavLayerActiveMaskNext = 0x00;
        window->DC.MenuBarAppending = false;
        window->DC.ChildWindows.resize(0);
        window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
        window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
        window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
        window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
        window->DC.ItemWidth = window->ItemWidthDefault;
        window->DC.TextWrapPos = -1.0f; // disabled
        window->DC.ItemFlagsStack.resize(0);
        window->DC.ItemWidthStack.resize(0);
        window->DC.TextWrapPosStack.resize(0);
        window->DC.CurrentColumns = NULL;
        window->DC.TreeDepth = 0;
        window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
        window->DC.StateStohnly = &window->StateStohnly;
        window->DC.GroupStack.resize(0);
        window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

        if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
        {
            window->DC.ItemFlags = parent_window->DC.ItemFlags;
            window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
        }

        if (window->AutoFitFramesX > 0)
            window->AutoFitFramesX--;
        if (window->AutoFitFramesY > 0)
            window->AutoFitFramesY--;

        // Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
        if (want_focus)
        {
            FocusWindow(window);
            NavInitWindow(window, false);
        }

        // Title bar
        if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
        {
            // Close & collapse button are on layer 1 (same as menus) and don't default focus
            const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
            window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
            window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
            window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

            // Collapse button
            if (!(flags & vsonyp0werWindowFlags_NoCollapse))
                if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
                    window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

            // Close button
            if (p_open != NULL)
            {
                const float rad = g.FontSize * 0.5f;
                if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
                    * p_open = false;
            }

            window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
            window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
            window->DC.ItemFlags = item_flags_backup;

            // Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
            // FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
            const char* UNSAVED_DOCUMENT_MARKER = "*";
            float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
            ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
            ImRect text_r = title_bar_rect;
            float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
            float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
            if (style.WindowTitleAlign.x > 0.0f)
                pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
            text_r.Min.x += pad_left;
            text_r.Max.x -= pad_right;
            ImRect clip_rect = text_r;
            clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
            RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
            if (flags & vsonyp0werWindowFlags_UnsavedDocument)
            {
                ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
                ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
                RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
            }
        }

        // Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
        window->OuterRectClipped = window->Rect();
        window->OuterRectClipped.ClipWith(window->ClipRect);

        // Pressing CTRL+C while holding on a window copy its content to the clipboard
        // This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
        // Maybe we can support CTRL+C on every element?
        /*
        if (g.ActiveId == move_id)
            if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
                LogToClipboard();
        */

        // Inner rectangle
        // We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
        // Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
        window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
        window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
        window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
        window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
        //window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

        // Inner clipping rectangle
        // Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
        window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
        window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
        window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
        window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

        // We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
        // This is useful to allow creating context menus on title bar only, etc.
        window->DC.LastItemId = window->MoveId;
        window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
        window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
        if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
            vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
    } else
    {
        // Append
        SetCurrentWindow(window);
    }

    PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

    // Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
    if (first_begin_of_the_frame)
        window->WriteAccessed = false;

    window->BeginCount++;
    g.NextWindowData.Clear();

    if (flags & vsonyp0werWindowFlags_ChildWindow)
    {
        // Child window can be out of sight and have "negative" clip windows.
        // Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
        IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
        if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
            if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
                window->HiddenFramesCanSkipItems = 1;

        // Completely hide along with parent or if parent is collapsed
        if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
            window->HiddenFramesCanSkipItems = 1;
    }

    // Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
    if (style.Alpha <= 0.0f)
        window->HiddenFramesCanSkipItems = 1;

    // Update the Hidden flag
    window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

    // Update the SkipItems flag, used to early out of all items functions (no layout required)
    bool skip_items = false;
    if (window->Collapsed || !window->Active || window->Hidden)
        if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
            skip_items = true;
    window->SkipItems = skip_items;

    return !skip_items;
}

bool vsonyp0wer::BeginColorPickerBackground(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			// Window background
			auto backgroundColor = ImColor(40, 40, 40, 255);
			auto borderColor = ImColor(10, 10, 10, 255);
			auto innerBorderColor = ImColor(60, 60, 60, 255);
			window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size + ImVec2(1, 1), backgroundColor, backgroundColor, backgroundColor, backgroundColor);
			window->DrawList->AddRect(window->Pos + ImVec2(1, window->TitleBarHeight() + 1), window->Pos + window->Size, innerBorderColor, 0, false, 0.4);
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size + ImVec2(1, 1), borderColor, 0, false, 0.4);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginTabsBackground(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			// Window background   outerBorderColor
			auto backgroundColor = ImColor(12, 12, 12);
			auto outerBorderColor = ImColor(48, 48, 48);
			auto innerBorderColor = ImColor(0, 0, 0);
				
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight() - 1), window->Pos + window->Size - ImVec2(0, 530), outerBorderColor, 0, false, 0.4);
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight() - 1), window->Pos + window->Size - ImVec2(1, 530), innerBorderColor, 0, false, 0.4);
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight() - 1), window->Pos + window->Size - ImVec2(2, 530), backgroundColor, 0, false, 0.4);
			//window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(-2, window->TitleBarHeight() - 1), window->Pos + window->Size, backgroundColor, backgroundColor, backgroundColor, backgroundColor);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginEndBorder(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			// Window background
			auto backgroundColor = GetColorU32(vsonyp0werCol_WindowBg);
			auto innerBorderColor = ImColor(255, 255, 255);

			//window->DrawList->AddRectFilled(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, backgroundColor, 0.f);
			window->DrawList->AddRect(window->Pos + ImVec2(1, window->TitleBarHeight() + 1), window->Pos + window->Size, innerBorderColor, 0, false, 1.f);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginMenuBackground(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			// Window background
			auto backgroundColor = GetColorU32(vsonyp0werCol_WindowBg);
			auto innerBorderColor = ImColor(60, 60, 60);

			window->DrawList->AddRectFilled(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, backgroundColor, 0.f);
			window->DrawList->AddRect(window->Pos + ImVec2(1, window->TitleBarHeight() + 1), window->Pos + window->Size - ImVec2(1, 1), innerBorderColor, 0, false, 0.4);

			window->DrawList->AddRect(window->Pos + ImVec2(5, window->TitleBarHeight() + 5), window->Pos + window->Size - ImVec2(5, 5), innerBorderColor, 0, false, 0.4);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginShadowBackground(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			// Window background
			auto topColor = ImColor(40, 40, 40, 255);
			auto bottomColor = ImColor(49, 49, 49, 255);
			window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, topColor, topColor, bottomColor, bottomColor);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginGroupBoxMain(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
		const float grip_draw_size = (float)(int)ImMax(9 * 1.00f, window->WindowRounding + 0.0f + g.FontSize * 0.0f);//mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			auto backgroundColor = ImColor(17, 17, 17);
			auto innerBorderColor = ImColor(48, 48, 48);
			auto outerBorderColor = ImColor(10, 10, 10);

			window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
			window->DrawList->AddRect(window->Pos + ImVec2(1, window->TitleBarHeight() + 1), window->Pos + window->Size - ImVec2(1, 1), innerBorderColor, 0, false, 0.4);
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, outerBorderColor, 0, false, 0.4);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
                    window->DrawList->PathArcTo(ImVec2(corner.x - 1, corner.y - 1), 0, 1, 1); // * (window_rounding + window_border_size)
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginGroupBoxScrollMain(const char* name, const char* groupboxName, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			auto backgroundColor = ImColor(17, 17, 17);
			auto backgroundColor2 = ImColor(17, 17, 170);
			auto innerBorderColor = ImColor(48, 48, 48);
			auto outerBorderColor = ImColor(10, 10, 10);

			//window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
			//window->DrawList->AddRect(window->Pos + ImVec2(1, window->TitleBarHeight() + 1), window->Pos + window->Size - ImVec2(1, 1), innerBorderColor, 0, false, 0.4);
			//window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight()), window->Pos + window->Size, outerBorderColor, 0, false, 0.4);

			const vsonyp0werID id = window->GetID(groupboxName);
			const ImVec2 labelSize = CalcTextSize(groupboxName, NULL, true);
			ImVec2 pos = window->DC.CursorPos;
			ImVec2 size = CalcItemSize(ImVec2(window->Size.x, window->Size.y), labelSize.x + style.FramePadding.x * 2.0f, labelSize.y + style.FramePadding.y * 2.0f);
			const ImRect bb(pos + ImVec2(5, 0), pos + ImVec2(labelSize.x + 14, 0));
			ItemSize(bb, style.FramePadding.y);

			window->DrawList->AddRectFilledMultiColor(bb.Min - ImVec2(0, 6), bb.Max + ImVec2(-2, -2), backgroundColor, backgroundColor, backgroundColor, backgroundColor);

			PushColor(vsonyp0werCol_Text, vsonyp0werCol_TextShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			RenderTextClipped(window->Pos + ImVec2(15, window->TitleBarHeight() - 5), window->Pos + window->Size - ImVec2(8, 5), groupboxName, NULL, NULL, ImVec2(0, 0));
			PopStyleColor();

			PushColor(vsonyp0werCol_Text, vsonyp0werCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			RenderTextClipped(window->Pos + ImVec2(14, window->TitleBarHeight() - 6), window->Pos + window->Size - ImVec2(8, 5), groupboxName, NULL, NULL, ImVec2(0, 0));
			PopStyleColor();

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

bool vsonyp0wer::BeginComboBackground(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
	vsonyp0werContext& g = *Gvsonyp0wer;
	const vsonyp0werStyle& style = g.Style;
	IM_ASSERT(name != NULL && name[0] != '\0');     // Window name required
	IM_ASSERT(g.FrameScopeActive);                  // Forgot to call vsonyp0wer::NewFrame()
	IM_ASSERT(g.FrameCountEnded != g.FrameCount);   // Called vsonyp0wer::Render() or vsonyp0wer::EndFrame() and haven't called vsonyp0wer::NewFrame() again yet

	// Find or create
	vsonyp0werWindow * window = FindWindowByName(name);
	const bool window_just_created = (window == NULL);
	if (window_just_created)
	{
		ImVec2 size_on_first_use = (g.NextWindowData.SizeCond != 0) ? g.NextWindowData.SizeVal : ImVec2(0.0f, 0.0f); // Any condition flag will do since we are creating a new window here.
		window = CreateNewWindow(name, size_on_first_use, flags);
	}

	// Automatically disable manual moving/resizing when NoInputs is set
	if ((flags & vsonyp0werWindowFlags_NoInputs) == vsonyp0werWindowFlags_NoInputs)
		flags |= vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize;

	if (flags & vsonyp0werWindowFlags_NavFlattened)
		IM_ASSERT(flags & vsonyp0werWindowFlags_ChildWindow);

	const int current_frame = g.FrameCount;
	const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);

	// Update the Appearing flag
	bool window_just_activated_by_user = (window->LastFrameActive < current_frame - 1);   // Not using !WasActive because the implicit "Debug" window would always toggle off->on
	const bool window_just_appearing_after_hidden_for_resize = (window->HiddenFramesCannotSkipItems > 0);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		window_just_activated_by_user |= (window->PopupId != popup_ref.PopupId); // We recycle popups so treat window as activated if popup id changed
		window_just_activated_by_user |= (window != popup_ref.Window);
	}
	window->Appearing = (window_just_activated_by_user || window_just_appearing_after_hidden_for_resize);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, true);

	// Update Flags, LastFrameActive, BeginOrderXXX fields
	if (first_begin_of_the_frame)
	{
		window->Flags = (vsonyp0werWindowFlags)flags;
		window->LastFrameActive = current_frame;
		window->BeginOrderWithinParent = 0;
		window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
	}
	else
	{
		flags = window->Flags;
	}

	// Parent window is latched only on the first call to Begin() of the frame, so further append-calls can be done from a different window stack
	vsonyp0werWindow* parent_window_in_stack = g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back();
	vsonyp0werWindow* parent_window = first_begin_of_the_frame ? ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Popup)) ? parent_window_in_stack : NULL) : window->ParentWindow;
	IM_ASSERT(parent_window != NULL || !(flags & vsonyp0werWindowFlags_ChildWindow));

	// Add to stack
	// We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set, then will call SetCurrentWindow()
	g.CurrentWindowStack.push_back(window);
	g.CurrentWindow = NULL;
	CheckStacksSize(window, true);
	if (flags & vsonyp0werWindowFlags_Popup)
	{
		vsonyp0werPopupRef& popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
		popup_ref.Window = window;
		g.BeginPopupStack.push_back(popup_ref);
		window->PopupId = popup_ref.PopupId;
	}

	if (window_just_appearing_after_hidden_for_resize && !(flags & vsonyp0werWindowFlags_ChildWindow))
		window->NavLastIds[0] = 0;

	// Process SetNextWindow***() calls
	bool window_pos_set_by_api = false;
	bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
	if (g.NextWindowData.PosCond)
	{
		window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
		if (window_pos_set_by_api && ImLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f)
		{
			// May be processed on the next frame if this is our first frame and we are measuring size
			// FIXME: Look into removing the branch so everything can go through this same code path for consistency.
			window->SetWindowPosVal = g.NextWindowData.PosVal;
			window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
			window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
		}
		else
		{
			SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
		}
	}
	if (g.NextWindowData.SizeCond)
	{
		window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.x > 0.0f);
		window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) != 0 && (g.NextWindowData.SizeVal.y > 0.0f);
		SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
	}
	if (g.NextWindowData.ContentSizeCond)
	{
		// Adjust passed "client size" to become a "window size"
		window->SizeContentsExplicit = g.NextWindowData.ContentSizeVal;
		if (window->SizeContentsExplicit.y != 0.0f)
			window->SizeContentsExplicit.y += window->TitleBarHeight() + window->MenuBarHeight();
	}
	else if (first_begin_of_the_frame)
	{
		window->SizeContentsExplicit = ImVec2(0.0f, 0.0f);
	}
	if (g.NextWindowData.CollapsedCond)
		SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
	if (g.NextWindowData.FocusCond)
		FocusWindow(window);
	if (window->Appearing)
		SetWindowConditionAllowFlags(window, vsonyp0werCond_Appearing, false);

	// When reusing window again multiple times a frame, just append content (don't need to setup again)
	if (first_begin_of_the_frame)
	{
		// Initialize
		const bool window_is_child_tooltip = (flags & vsonyp0werWindowFlags_ChildWindow) && (flags & vsonyp0werWindowFlags_Tooltip); // FIXME-WIP: Undocumented behavior of Child+Tooltip for pinned tooltip (#1345)
		UpdateWindowParentAndRootLinks(window, flags, parent_window);

		window->Active = true;
		window->HasCloseButton = (p_open != NULL);
		window->ClipRect = ImVec4(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
		window->IDStack.resize(1);

		// Update stored window name when it changes (which can _only_ happen with the "###" operator, so the ID would stay unchanged).
		// The title bar always display the 'name' parameter, so we only update the string stohnly if it needs to be visible to the end-user elsewhere.
		bool window_title_visible_elsewhere = false;
		if (g.NavWindowingList != NULL && (window->Flags & vsonyp0werWindowFlags_NoNavFocus) == 0)   // Window titles visible when using CTRL+TAB
			window_title_visible_elsewhere = true;
		if (window_title_visible_elsewhere && !window_just_created && strcmp(name, window->Name) != 0)
		{
			size_t buf_len = (size_t)window->NameBufLen;
			window->Name = ImStrdupcpy(window->Name, &buf_len, name);
			window->NameBufLen = (int)buf_len;
		}

		// UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

		// Update contents size from last frame for auto-fitting (or use explicit size)
		window->SizeContents = CalcSizeContents(window);
		if (window->HiddenFramesCanSkipItems > 0)
			window->HiddenFramesCanSkipItems--;
		if (window->HiddenFramesCannotSkipItems > 0)
			window->HiddenFramesCannotSkipItems--;

		// Hide new windows for one frame until they calculate their size
		if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
			window->HiddenFramesCannotSkipItems = 1;

		// Hide popup/tooltip window when re-opening while we measure size (because we recycle the windows)
		// We reset Size/SizeContents for reappearing popups/tooltips early in this function, so further code won't be tempted to use the old size.
		if (window_just_activated_by_user && (flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) != 0)
		{
			window->HiddenFramesCannotSkipItems = 1;
			if (flags & vsonyp0werWindowFlags_AlwaysAutoResize)
			{
				if (!window_size_x_set_by_api)
					window->Size.x = window->SizeFull.x = 0.f;
				if (!window_size_y_set_by_api)
					window->Size.y = window->SizeFull.y = 0.f;
				window->SizeContents = ImVec2(0.f, 0.f);
			}
		}

		SetCurrentWindow(window);

		// Lock border size and padding for the frame (so that altering them doesn't cause inconsistencies)
		if (flags & vsonyp0werWindowFlags_ChildWindow)
			window->WindowBorderSize = style.ChildBorderSize;
		else
			window->WindowBorderSize = ((flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Tooltip)) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupBorderSize : style.WindowBorderSize;
		window->WindowPadding = style.WindowPadding;
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & (vsonyp0werWindowFlags_AlwaysUseWindowPadding | vsonyp0werWindowFlags_Popup)) && window->WindowBorderSize == 0.0f)
			window->WindowPadding = ImVec2(0.0f, (flags & vsonyp0werWindowFlags_MenuBar) ? style.WindowPadding.y : 0.0f);
		window->DC.MenuBarOffset.x = ImMax(ImMax(window->WindowPadding.x, style.ItemSpacing.x), g.NextWindowData.MenuBarOffsetMinVal.x);
		window->DC.MenuBarOffset.y = g.NextWindowData.MenuBarOffsetMinVal.y;

		// Collapse window by double-clicking on title bar
		// At this point we don't have a clipping rectangle setup yet, so we can use the title bar area for hit detection and drawing
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar) && !(flags & vsonyp0werWindowFlags_NoCollapse))
		{
			// We don't use a regular button+id to test for double-click on title bar (mostly due to legacy reason, could be fixed), so verify that we don't have items over the title bar.
			ImRect title_bar_rect = window->TitleBarRect();
			if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 && IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) && g.IO.MouseDoubleClicked[0])
				window->WantCollapseToggle = true;
			if (window->WantCollapseToggle)
			{
				window->Collapsed = !window->Collapsed;
				MarkIniSettingsDirty(window);
				FocusWindow(window);
			}
		}
		else
		{
			window->Collapsed = false;
		}
		window->WantCollapseToggle = false;

		// SIZE

		// Calculate auto-fit size, handle automatic resize
		const ImVec2 size_auto_fit = CalcSizeAutoFit(window, window->SizeContents);
		ImVec2 size_full_modified(FLT_MAX, FLT_MAX);
		if ((flags & vsonyp0werWindowFlags_AlwaysAutoResize) && !window->Collapsed)
		{
			// Using SetNextWindowSize() overrides vsonyp0werWindowFlags_AlwaysAutoResize, so it can be used on tooltips/popups, etc.
			if (!window_size_x_set_by_api)
				window->SizeFull.x = size_full_modified.x = size_auto_fit.x;
			if (!window_size_y_set_by_api)
				window->SizeFull.y = size_full_modified.y = size_auto_fit.y;
		}
		else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
		{
			// Auto-fit may only grow window during the first few frames
			// We still process initial auto-fit on collapsed windows to get a window width, but otherwise don't honor vsonyp0werWindowFlags_AlwaysAutoResize when collapsed.
			if (!window_size_x_set_by_api && window->AutoFitFramesX > 0)
				window->SizeFull.x = size_full_modified.x = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.x, size_auto_fit.x) : size_auto_fit.x;
			if (!window_size_y_set_by_api && window->AutoFitFramesY > 0)
				window->SizeFull.y = size_full_modified.y = window->AutoFitOnlyGrows ? ImMax(window->SizeFull.y, size_auto_fit.y) : size_auto_fit.y;
			if (!window->Collapsed)
				MarkIniSettingsDirty(window);
		}

		// Apply minimum/maximum window size constraints and final size
		window->SizeFull = CalcSizeAfterConstraint(window, window->SizeFull);
		window->Size = window->Collapsed && !(flags & vsonyp0werWindowFlags_ChildWindow) ? window->TitleBarRect().GetSize() : window->SizeFull;

		// SCROLLBAR STATUS

		// Update scrollbar status (based on the Size that was effective during last frame or the auto-resized Size).
		if (!window->Collapsed)
		{
			// When reading the current size we need to read it after size constraints have been applied
			float size_x_for_scrollbars = size_full_modified.x != FLT_MAX ? window->SizeFull.x : window->SizeFullAtLastBegin.x;
			float size_y_for_scrollbars = size_full_modified.y != FLT_MAX ? window->SizeFull.y : window->SizeFullAtLastBegin.y;
			window->ScrollbarY = (flags & vsonyp0werWindowFlags_AlwaysVerticalScrollbar) || ((window->SizeContents.y > size_y_for_scrollbars) && !(flags & vsonyp0werWindowFlags_NoScrollbar));
			window->ScrollbarX = (flags & vsonyp0werWindowFlags_AlwaysHorizontalScrollbar) || ((window->SizeContents.x > size_x_for_scrollbars - (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) && !(flags & vsonyp0werWindowFlags_NoScrollbar) && (flags & vsonyp0werWindowFlags_HorizontalScrollbar));
			if (window->ScrollbarX && !window->ScrollbarY)
				window->ScrollbarY = (window->SizeContents.y > size_y_for_scrollbars - style.ScrollbarSize) && !(flags & vsonyp0werWindowFlags_NoScrollbar);
			window->ScrollbarSizes = ImVec2(window->ScrollbarY ? style.ScrollbarSize : 0.0f, window->ScrollbarX ? style.ScrollbarSize : 0.0f);
		}

		// POSITION

		// Popup latch its initial position, will position itself when it appears next frame
		if (window_just_activated_by_user)
		{
			window->AutoPosLastDirection = vsonyp0werDir_None;
			if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api)
				window->Pos = g.BeginPopupStack.back().OpenPopupPos;
		}

		// Position child window
		if (flags & vsonyp0werWindowFlags_ChildWindow)
		{
			IM_ASSERT(parent_window && parent_window->Active);
			window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
			parent_window->DC.ChildWindows.push_back(window);
			if (!(flags & vsonyp0werWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
				window->Pos = parent_window->DC.CursorPos;
		}

		const bool window_pos_with_pivot = (window->SetWindowPosVal.x != FLT_MAX && window->HiddenFramesCannotSkipItems == 0);
		if (window_pos_with_pivot)
			SetWindowPos(window, ImMax(style.DisplaySafeAreaPadding, window->SetWindowPosVal - window->SizeFull * window->SetWindowPosPivot), 0); // Position given a pivot (e.g. for centering)
		else if ((flags & vsonyp0werWindowFlags_ChildMenu) != 0)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Popup) != 0 && !window_pos_set_by_api && window_just_appearing_after_hidden_for_resize)
			window->Pos = FindBestWindowPosForPopup(window);
		else if ((flags & vsonyp0werWindowFlags_Tooltip) != 0 && !window_pos_set_by_api && !window_is_child_tooltip)
			window->Pos = FindBestWindowPosForPopup(window);

		// Clamp position so it stays visible
		// Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
		ImRect viewport_rect(GetViewportRect());
		if (!window_pos_set_by_api && !(flags & vsonyp0werWindowFlags_ChildWindow) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
		{
			if (g.IO.DisplaySize.x > 0.0f && g.IO.DisplaySize.y > 0.0f) // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports zero-sized window when initializing or minimizing.
			{
				ImVec2 clamp_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
				ClampWindowRect(window, viewport_rect, clamp_padding);
			}
		}
		window->Pos = ImFloor(window->Pos);

		// Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
		window->WindowRounding = (flags & vsonyp0werWindowFlags_ChildWindow) ? style.ChildRounding : ((flags & vsonyp0werWindowFlags_Popup) && !(flags & vsonyp0werWindowFlags_Modal)) ? style.PopupRounding : style.WindowRounding;

		// Apply scrolling
		window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window, true);
		window->ScrollTarget = ImVec2(FLT_MAX, FLT_MAX);

		// Apply window focus (new and reactivated windows are moved to front)
		bool want_focus = false;
		if (window_just_activated_by_user && !(flags & vsonyp0werWindowFlags_NoFocusOnAppearing))
		{
			if (flags & vsonyp0werWindowFlags_Popup)
				want_focus = true;
			else if ((flags & (vsonyp0werWindowFlags_ChildWindow | vsonyp0werWindowFlags_Tooltip)) == 0)
				want_focus = true;
		}

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = { 0 };
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // 4
        const float grip_draw_size = (float)(int)ImMax(9 * 1.0f, window->WindowRounding + 0.0f + g.FontSize * 0.f); // mrtvi ugao
		if (!window->Collapsed)
			UpdateManualResize(window, size_auto_fit, &border_held, resize_grip_count, &resize_grip_col[0]);
		window->ResizeBorderHeld = (signed char)border_held;

		// Default item width. Make it proportional to window size if window manually resizes
		if (window->Size.x > 0.0f && !(flags & vsonyp0werWindowFlags_Tooltip) && !(flags & vsonyp0werWindowFlags_AlwaysAutoResize))
			window->ItemWidthDefault = (float)(int)(window->Size.x * 0.65f);
		else
			window->ItemWidthDefault = (float)(int)(g.FontSize * 16.0f);

		// DRAWING

		// Setup draw list and outer clipping rectangle
		window->DrawList->Clear();
		window->DrawList->Flags = (g.Style.AntiAliasedLines ? ImDrawListFlags_AntiAliasedLines : 0) | (g.Style.AntiAliasedFill ? ImDrawListFlags_AntiAliasedFill : 0);
		window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
		if ((flags & vsonyp0werWindowFlags_ChildWindow) && !(flags & vsonyp0werWindowFlags_Popup) && !window_is_child_tooltip)
			PushClipRect(parent_window->ClipRect.Min, parent_window->ClipRect.Max, true);
		else
			PushClipRect(viewport_rect.Min, viewport_rect.Max, true);

		// Draw modal window background (darkens what is behind them, all viewports)
		const bool dim_bg_for_modal = (flags & vsonyp0werWindowFlags_Modal) && window == GetFrontMostPopupModal() && window->HiddenFramesCannotSkipItems <= 0;
		const bool dim_bg_for_window_list = g.NavWindowingTargetAnim && (window == g.NavWindowingTargetAnim->RootWindow);
		if (dim_bg_for_modal || dim_bg_for_window_list)
		{
			const ImU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? vsonyp0werCol_ModalWindowDimBg : vsonyp0werCol_NavWindowingDimBg, g.DimBgRatio);
			window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
		}

		// Draw navigation selection/windowing rectangle background
		if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim)
		{
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (!bb.Contains(viewport_rect)) // Avoid drawing if the window covers all the viewport anyway
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f), g.Style.WindowRounding);
		}

		// Draw window + handle manual resize
		// As we highlight the title bar when want_focus is set, multiple reappearing windows will have have their title bar highlighted on their reappearing frame.
		const float window_rounding = window->WindowRounding;
		const float window_border_size = window->WindowBorderSize;
		const vsonyp0werWindow* window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget : g.NavWindow;
		const bool title_bar_is_highlight = want_focus || (window_to_highlight && window->RootWindowForTitleBarHighlight == window_to_highlight->RootWindowForTitleBarHighlight);
		const ImRect title_bar_rect = window->TitleBarRect();
		if (window->Collapsed)
		{
			// Title bar only
			float backup_border_size = style.FrameBorderSize;
			g.Style.FrameBorderSize = window->WindowBorderSize;
			ImU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBgCollapsed);
			RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
			g.Style.FrameBorderSize = backup_border_size;
		}
		else
		{
			auto borderColor = ImColor(10, 10, 10);
			auto backgroundColor = ImColor(35, 35, 35);

			window->DrawList->AddRectFilledMultiColor(window->Pos + ImVec2(0, window->TitleBarHeight() + 2), window->Pos + window->Size, backgroundColor, backgroundColor, backgroundColor, backgroundColor);
			window->DrawList->AddRect(window->Pos + ImVec2(0, window->TitleBarHeight() + 1), window->Pos + window->Size, borderColor, 0, false, 0.4);

			// Title bar
			if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
			{
				ImU32 title_bar_col = GetColorU32(title_bar_is_highlight ? vsonyp0werCol_TitleBgActive : vsonyp0werCol_TitleBg);
				window->DrawList->AddRectFilled(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, window_rounding, ImDrawCornerFlags_Top);
			}

			// Menu bar
			if (flags & vsonyp0werWindowFlags_MenuBar)
			{
				ImRect menu_bar_rect = window->MenuBarRect();
				menu_bar_rect.ClipWith(window->Rect());  // Soft clipping, in particular child window don't have minimum size covering the menu bar so this is useful for them.
				window->DrawList->AddRectFilled(menu_bar_rect.Min + ImVec2(window_border_size, 0), menu_bar_rect.Max - ImVec2(window_border_size, 0), GetColorU32(vsonyp0werCol_MenuBarBg), (flags & vsonyp0werWindowFlags_NoTitleBar) ? window_rounding : 0.0f, ImDrawCornerFlags_Top);
				if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max.y < window->Pos.y + window->Size.y)
					window->DrawList->AddLine(menu_bar_rect.GetBL(), menu_bar_rect.GetBR(), GetColorU32(vsonyp0werCol_Border), style.FrameBorderSize);
			}

			// Scrollbars
			if (window->ScrollbarX)
				Scrollbar(vsonyp0werAxis_X);
			if (window->ScrollbarY)
				Scrollbar(vsonyp0werAxis_Y);

			// Render resize grips (after their input handling so we don't have a frame of latency)
			if (!(flags & vsonyp0werWindowFlags_NoResize))
			{
				for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
				{
					const vsonyp0werResizeGripDef& grip = resize_grip_def[resize_grip_n];
					const ImVec2 corner = ImLerp(window->Pos, window->Pos + window->Size, grip.CornerPosN);
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(window_border_size, grip_draw_size) : ImVec2(grip_draw_size, window_border_size)));
					window->DrawList->PathLineTo(corner + grip.InnerDir * ((resize_grip_n & 1) ? ImVec2(grip_draw_size, window_border_size) : ImVec2(window_border_size, grip_draw_size)));
					window->DrawList->PathArcToFast(ImVec2(corner.x + grip.InnerDir.x * (window_rounding + window_border_size), corner.y + grip.InnerDir.y * (window_rounding + window_border_size)), window_rounding, grip.AngleMin12, grip.AngleMax12);
					window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
				}
			}

			// Borders
			RenderOuterBorders(window);
		}

		// Draw navigation selection/windowing rectangle border
		if (g.NavWindowingTargetAnim == window)
		{
			float rounding = ImMax(window->WindowRounding, g.Style.WindowRounding);
			ImRect bb = window->Rect();
			bb.Expand(g.FontSize);
			if (bb.Contains(viewport_rect)) // If a window fits the entire viewport, adjust its highlight inward
			{
				bb.Expand(-g.FontSize - 1.0f);
				rounding = window->WindowRounding;
			}
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(vsonyp0werCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha), rounding, ~0, 3.0f);
		}

		// Store a backup of SizeFull which we will use next frame to decide if we need scrollbars.
		window->SizeFullAtLastBegin = window->SizeFull;

		// Update various regions. Variables they depends on are set above in this function.
		// FIXME: window->ContentsRegionRect.Max is currently very misleading / partly faulty, but some BeginChild() patterns relies on it.
		window->ContentsRegionRect.Min.x = window->Pos.x - window->Scroll.x + window->WindowPadding.x;
		window->ContentsRegionRect.Min.y = window->Pos.y - window->Scroll.y + window->WindowPadding.y + window->TitleBarHeight() + window->MenuBarHeight();
		window->ContentsRegionRect.Max.x = window->Pos.x - window->Scroll.x - window->WindowPadding.x + (window->SizeContentsExplicit.x != 0.0f ? window->SizeContentsExplicit.x : (window->Size.x - window->ScrollbarSizes.x));
		window->ContentsRegionRect.Max.y = window->Pos.y - window->Scroll.y - window->WindowPadding.y + (window->SizeContentsExplicit.y != 0.0f ? window->SizeContentsExplicit.y : (window->Size.y - window->ScrollbarSizes.y));

		// Setup drawing context
		// (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant to hold transient data only. Nowadays difference between window-> and window->DC-> is dubious.)
		window->DC.Indent.x = 0.0f + window->WindowPadding.x - window->Scroll.x;
		window->DC.GroupOffset.x = 0.0f;
		window->DC.ColumnsOffset.x = 0.0f;
		window->DC.CursorStartPos = window->Pos + ImVec2(window->DC.Indent.x + window->DC.ColumnsOffset.x, window->TitleBarHeight() + window->MenuBarHeight() + window->WindowPadding.y - window->Scroll.y);
		window->DC.CursorPos = window->DC.CursorStartPos;
		window->DC.CursorPosPrevLine = window->DC.CursorPos;
		window->DC.CursorMaxPos = window->DC.CursorStartPos;
		window->DC.CurrentLineSize = window->DC.PrevLineSize = ImVec2(0.0f, 0.0f);
		window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;
		window->DC.NavHideHighlightOneFrame = false;
		window->DC.NavHasScroll = (GetWindowScrollMaxY(window) > 0.0f);
		window->DC.NavLayerActiveMask = window->DC.NavLayerActiveMaskNext;
		window->DC.NavLayerActiveMaskNext = 0x00;
		window->DC.MenuBarAppending = false;
		window->DC.ChildWindows.resize(0);
		window->DC.LayoutType = vsonyp0werLayoutType_Vertical;
		window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType : vsonyp0werLayoutType_Vertical;
		window->DC.FocusCounterAll = window->DC.FocusCounterTab = -1;
		window->DC.ItemFlags = parent_window ? parent_window->DC.ItemFlags : vsonyp0werItemFlags_Default_;
		window->DC.ItemWidth = window->ItemWidthDefault;
		window->DC.TextWrapPos = -1.0f; // disabled
		window->DC.ItemFlagsStack.resize(0);
		window->DC.ItemWidthStack.resize(0);
		window->DC.TextWrapPosStack.resize(0);
		window->DC.CurrentColumns = NULL;
		window->DC.TreeDepth = 0;
		window->DC.TreeDepthMayJumpToParentOnPop = 0x00;
		window->DC.StateStohnly = &window->StateStohnly;
		window->DC.GroupStack.resize(0);
		window->MenuColumns.Update(3, style.ItemSpacing.x, window_just_activated_by_user);

		if ((flags & vsonyp0werWindowFlags_ChildWindow) && (window->DC.ItemFlags != parent_window->DC.ItemFlags))
		{
			window->DC.ItemFlags = parent_window->DC.ItemFlags;
			window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
		}

		if (window->AutoFitFramesX > 0)
			window->AutoFitFramesX--;
		if (window->AutoFitFramesY > 0)
			window->AutoFitFramesY--;

		// Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial navigation reference rectangle can start around there)
		if (want_focus)
		{
			FocusWindow(window);
			NavInitWindow(window, false);
		}

		// Title bar
		if (!(flags & vsonyp0werWindowFlags_NoTitleBar))
		{
			// Close & collapse button are on layer 1 (same as menus) and don't default focus
			const vsonyp0werItemFlags item_flags_backup = window->DC.ItemFlags;
			window->DC.ItemFlags |= vsonyp0werItemFlags_NoNavDefaultFocus;
			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Menu;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Menu);

			// Collapse button
			if (!(flags & vsonyp0werWindowFlags_NoCollapse))
				if (CollapseButton(window->GetID("#COLLAPSE"), window->Pos))
					window->WantCollapseToggle = true; // Defer collapsing to next frame as we are too far in the Begin() function

			// Close button
			if (p_open != NULL)
			{
				const float rad = g.FontSize * 0.5f;
				if (CloseButton(window->GetID("#CLOSE"), ImVec2(window->Pos.x + window->Size.x - style.FramePadding.x - rad, window->Pos.y + style.FramePadding.y + rad), rad + 1))
					* p_open = false;
			}

			window->DC.NavLayerCurrent = vsonyp0werNavLayer_Main;
			window->DC.NavLayerCurrentMask = (1 << vsonyp0werNavLayer_Main);
			window->DC.ItemFlags = item_flags_backup;

			// Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved document" marker)
			// FIXME: Refactor text alignment facilities along with RenderText helpers, this is too much code..
			const char* UNSAVED_DOCUMENT_MARKER = "*";
			float marker_size_x = (flags & vsonyp0werWindowFlags_UnsavedDocument) ? CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false).x : 0.0f;
			ImVec2 text_size = CalcTextSize(name, NULL, true) + ImVec2(marker_size_x, 0.0f);
			ImRect text_r = title_bar_rect;
			float pad_left = (flags & vsonyp0werWindowFlags_NoCollapse) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			float pad_right = (p_open == NULL) ? style.FramePadding.x : (style.FramePadding.x + g.FontSize + style.ItemInnerSpacing.x);
			if (style.WindowTitleAlign.x > 0.0f)
				pad_right = ImLerp(pad_right, pad_left, style.WindowTitleAlign.x);
			text_r.Min.x += pad_left;
			text_r.Max.x -= pad_right;
			ImRect clip_rect = text_r;
			clip_rect.Max.x = window->Pos.x + window->Size.x - (p_open ? title_bar_rect.GetHeight() - 3 : style.FramePadding.x); // Match the size of CloseButton()
			RenderTextClipped(text_r.Min, text_r.Max, name, NULL, &text_size, style.WindowTitleAlign, &clip_rect);
			if (flags & vsonyp0werWindowFlags_UnsavedDocument)
			{
				ImVec2 marker_pos = ImVec2(ImMax(text_r.Min.x, text_r.Min.x + (text_r.GetWidth() - text_size.x) * style.WindowTitleAlign.x) + text_size.x, text_r.Min.y) + ImVec2(2 - marker_size_x, 0.0f);
				ImVec2 off = ImVec2(0.0f, (float)(int)(-g.FontSize * 0.25f));
				RenderTextClipped(marker_pos + off, text_r.Max + off, UNSAVED_DOCUMENT_MARKER, NULL, NULL, ImVec2(0, style.WindowTitleAlign.y), &clip_rect);
			}
		}

		// Save clipped aabb so we can access it in constant-time in FindHoveredWindow()
		window->OuterRectClipped = window->Rect();
		window->OuterRectClipped.ClipWith(window->ClipRect);

		// Pressing CTRL+C while holding on a window copy its content to the clipboard
		// This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another Begin/End pair - so we need to work that out and add better logging scope.
		// Maybe we can support CTRL+C on every element?
		/*
		if (g.ActiveId == move_id)
			if (g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_C))
				LogToClipboard();
		*/

		// Inner rectangle
		// We set this up after processing the resize grip so that our clip rectangle doesn't lag by a frame
		// Note that if our window is collapsed we will end up with an inverted (~null) clipping rectangle which is the correct behavior.
		window->InnerMainRect.Min.x = title_bar_rect.Min.x + window->WindowBorderSize;
		window->InnerMainRect.Min.y = title_bar_rect.Max.y + window->MenuBarHeight() + (((flags & vsonyp0werWindowFlags_MenuBar) || !(flags & vsonyp0werWindowFlags_NoTitleBar)) ? style.FrameBorderSize : window->WindowBorderSize);
		window->InnerMainRect.Max.x = window->Pos.x + window->Size.x - window->ScrollbarSizes.x - window->WindowBorderSize;
		window->InnerMainRect.Max.y = window->Pos.y + window->Size.y - window->ScrollbarSizes.y - window->WindowBorderSize;
		//window->DrawList->AddRect(window->InnerRect.Min, window->InnerRect.Max, IM_COL32_WHITE);

		// Inner clipping rectangle
		// Force round operator last to ensure that e.g. (int)(max.x-min.x) in user's render code produce correct result.
		window->InnerClipRect.Min.x = ImFloor(0.5f + window->InnerMainRect.Min.x + ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Min.y = ImFloor(0.5f + window->InnerMainRect.Min.y);
		window->InnerClipRect.Max.x = ImFloor(0.5f + window->InnerMainRect.Max.x - ImMax(0.0f, ImFloor(window->WindowPadding.x * 0.5f - window->WindowBorderSize)));
		window->InnerClipRect.Max.y = ImFloor(0.5f + window->InnerMainRect.Max.y);

		// We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and IsItemActive() to be usable after Begin().
		// This is useful to allow creating context menus on title bar only, etc.
		window->DC.LastItemId = window->MoveId;
		window->DC.LastItemStatusFlags = IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ? vsonyp0werItemStatusFlags_HoveredRect : 0;
		window->DC.LastItemRect = title_bar_rect;
#ifdef vsonyp0wer_ENABLE_TEST_ENGINE
		if (!(window->Flags & vsonyp0werWindowFlags_NoTitleBar))
			vsonyp0wer_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
	}
	else
	{
		// Append
		SetCurrentWindow(window);
	}

	PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

	// Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag to stay false when the default "Debug" window is unused)
	if (first_begin_of_the_frame)
		window->WriteAccessed = false;

	window->BeginCount++;
	g.NextWindowData.Clear();

	if (flags & vsonyp0werWindowFlags_ChildWindow)
	{
		// Child window can be out of sight and have "negative" clip windows.
		// Mark them as collapsed so commands are skipped earlier (we can't manually collapse them because they have no title bar).
		IM_ASSERT((flags & vsonyp0werWindowFlags_NoTitleBar) != 0);
		if (!(flags & vsonyp0werWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
			if (window->OuterRectClipped.Min.x >= window->OuterRectClipped.Max.x || window->OuterRectClipped.Min.y >= window->OuterRectClipped.Max.y)
				window->HiddenFramesCanSkipItems = 1;

		// Completely hide along with parent or if parent is collapsed
		if (parent_window && (parent_window->Collapsed || parent_window->Hidden))
			window->HiddenFramesCanSkipItems = 1;
	}

	// Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and inconsistent but has been there for a long while (may remove at some point)
	if (style.Alpha <= 0.0f)
		window->HiddenFramesCanSkipItems = 1;

	// Update the Hidden flag
	window->Hidden = (window->HiddenFramesCanSkipItems > 0) || (window->HiddenFramesCannotSkipItems > 0);

	// Update the SkipItems flag, used to early out of all items functions (no layout required)
	bool skip_items = false;
	if (window->Collapsed || !window->Active || window->Hidden)
		if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 && window->HiddenFramesCannotSkipItems <= 0)
			skip_items = true;
	window->SkipItems = skip_items;

	return !skip_items;
}

// Old Begin() API with 5 parameters, avoid calling this version directly! Use SetNextWindowSize()/SetNextWindowBgAlpha() + Begin() instead.
#ifndef vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS
bool vsonyp0wer::Begin(const char* name, bool* p_open, const ImVec2 & size_first_use, float bg_alpha_override, vsonyp0werWindowFlags flags)
{
    // Old API feature: we could pass the initial window size as a parameter. This was misleading because it only had an effect if the window didn't have data in the .ini file.
    if (size_first_use.x != 0.0f || size_first_use.y != 0.0f)
        SetNextWindowSize(size_first_use, vsonyp0werCond_FirstUseEver);

    // Old API feature: override the window background alpha with a parameter.
    if (bg_alpha_override >= 0.0f)
        SetNextWindowBgAlpha(bg_alpha_override);

    return Begin(name, p_open, flags);
}
#endif // vsonyp0wer_DISABLE_OBSOLETE_FUNCTIONS

void vsonyp0wer::End()
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    if (g.CurrentWindowStack.Size <= 1 && g.FrameScopePushedImplicitWindow)
    {
        IM_ASSERT(g.CurrentWindowStack.Size > 1 && "Calling End() too many times!");
        return; // FIXME-ERRORHANDLING
    }
    IM_ASSERT(g.CurrentWindowStack.Size > 0);

    vsonyp0werWindow* window = g.CurrentWindow;

    if (window->DC.CurrentColumns != NULL)
        EndColumns();
    PopClipRect();   // Inner window clip rectangle

    // Stop logging
    if (!(window->Flags & vsonyp0werWindowFlags_ChildWindow))    // FIXME: add more options for scope of logging
        LogFinish();

    // Pop from window stack
    g.CurrentWindowStack.pop_back();
    if (window->Flags & vsonyp0werWindowFlags_Popup)
        g.BeginPopupStack.pop_back();
    CheckStacksSize(window, false);
    SetCurrentWindow(g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back());
}

void vsonyp0wer::BringWindowToFocusFront(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.WindowsFocusOrder.back() == window)
        return;
    for (int i = g.WindowsFocusOrder.Size - 2; i >= 0; i--) // We can ignore the front most window
        if (g.WindowsFocusOrder[i] == window)
        {
            memmove(&g.WindowsFocusOrder[i], &g.WindowsFocusOrder[i + 1], (size_t)(g.WindowsFocusOrder.Size - i - 1) * sizeof(vsonyp0werWindow*));
            g.WindowsFocusOrder[g.WindowsFocusOrder.Size - 1] = window;
            break;
        }
}

void vsonyp0wer::BringWindowToDisplayFront(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* current_front_window = g.Windows.back();
    if (current_front_window == window || current_front_window->RootWindow == window)
        return;
    for (int i = g.Windows.Size - 2; i >= 0; i--) // We can ignore the front most window
        if (g.Windows[i] == window)
        {
            memmove(&g.Windows[i], &g.Windows[i + 1], (size_t)(g.Windows.Size - i - 1) * sizeof(vsonyp0werWindow*));
            g.Windows[g.Windows.Size - 1] = window;
            break;
        }
}

void vsonyp0wer::BringWindowToDisplayBack(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.Windows[0] == window)
        return;
    for (int i = 0; i < g.Windows.Size; i++)
        if (g.Windows[i] == window)
        {
            memmove(&g.Windows[1], &g.Windows[0], (size_t)i * sizeof(vsonyp0werWindow*));
            g.Windows[0] = window;
            break;
        }
}

// Moving window to front of display and set focus (which happens to be back of our sorted list)
void vsonyp0wer::FocusWindow(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    if (g.NavWindow != window)
    {
        g.NavWindow = window;
        if (window && g.NavDisableMouseHover)
            g.NavMousePosDirty = true;
        g.NavInitRequest = false;
        g.NavId = window ? window->NavLastIds[0] : 0; // Restore NavId
        g.NavIdIsAlive = false;
        g.NavLayer = vsonyp0werNavLayer_Main;
        //vsonyp0wer_DEBUG_LOG("FocusWindow(\"%s\")\n", window ? window->Name : NULL);
    }

    // Passing NULL allow to disable keyboard focus
    if (!window)
        return;

    // Move the root window to the top of the pile
    if (window->RootWindow)
        window = window->RootWindow;

    // Steal focus on active widgets
    if (window->Flags & vsonyp0werWindowFlags_Popup) // FIXME: This statement should be unnecessary. Need further testing before removing it..
        if (g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != window)
            ClearActiveID();

    // Bring to front
    BringWindowToFocusFront(window);
    if (!(window->Flags & vsonyp0werWindowFlags_NoBringToFrontOnFocus))
        BringWindowToDisplayFront(window);
}

void vsonyp0wer::FocusPreviousWindowIgnoringOne(vsonyp0werWindow * ignore_window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int i = g.WindowsFocusOrder.Size - 1; i >= 0; i--)
    {
        // We may later decide to test for different NoXXXInputs based on the active navigation input (mouse vs nav) but that may feel more confusing to the user.
        vsonyp0werWindow* window = g.WindowsFocusOrder[i];
        if (window != ignore_window && window->WasActive && !(window->Flags & vsonyp0werWindowFlags_ChildWindow))
            if ((window->Flags & (vsonyp0werWindowFlags_NoMouseInputs | vsonyp0werWindowFlags_NoNavInputs)) != (vsonyp0werWindowFlags_NoMouseInputs | vsonyp0werWindowFlags_NoNavInputs))
            {
                vsonyp0werWindow* focus_window = NavRestoreLastChildNavWindow(window);
                FocusWindow(focus_window);
                return;
            }
    }
}

void vsonyp0wer::PushItemWidth(float item_width)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.ItemWidth = (item_width == 0.0f ? window->ItemWidthDefault : item_width);
    window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);
}

void vsonyp0wer::PushMultiItemsWidths(int components, float w_full)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    const vsonyp0werStyle& style = Gvsonyp0wer->Style;
    if (w_full <= 0.0f)
        w_full = CalcItemWidth();
    const float w_item_one = ImMax(1.0f, (float)(int)((w_full - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
    const float w_item_last = ImMax(1.0f, (float)(int)(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for (int i = 0; i < components - 1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

void vsonyp0wer::PopItemWidth()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.ItemWidthStack.pop_back();
    window->DC.ItemWidth = window->DC.ItemWidthStack.empty() ? window->ItemWidthDefault : window->DC.ItemWidthStack.back();
}

// Calculate default item width given value passed to PushItemWidth()
float vsonyp0wer::CalcItemWidth()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    float w = window->DC.ItemWidth;
    if (w < 0.0f)
    {
        float region_max_x = GetContentRegionMaxScreen().x;
        w = ImMax(1.0f, region_max_x - window->DC.CursorPos.x + w);
    }
    w = (float)(int)w;
    return w;
}

// [Internal] Calculate full item size given user provided 'size' parameter and default width/height. Default width is often == CalcItemWidth().
// Those two functions CalcItemWidth vs CalcItemSize are awkwardly named because they are not fully symmetrical.
// Note that only CalcItemWidth() is publicly exposed.
// The 4.0f here may be changed to match CalcItemWidth() and/or BeginChild() (right now we have a mismatch which is harmless but undesirable)
ImVec2 vsonyp0wer::CalcItemSize(ImVec2 size, float default_w, float default_h)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;

    ImVec2 region_max;
    if (size.x < 0.0f || size.y < 0.0f)
        region_max = GetContentRegionMaxScreen();

    if (size.x == 0.0f)
        size.x = default_w;
    else if (size.x < 0.0f)
        size.x = ImMax(4.0f, region_max.x - window->DC.CursorPos.x + size.x);

    if (size.y == 0.0f)
        size.y = default_h;
    else if (size.y < 0.0f)
        size.y = ImMax(4.0f, region_max.y - window->DC.CursorPos.y + size.y);

    return size;
}

void vsonyp0wer::SetCurrentFont(ImFont * font)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(font && font->IsLoaded());    // Font Atlas not created. Did you call io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
    IM_ASSERT(font->Scale > 0.0f);
    g.Font = font;
    g.FontBaseSize = ImMax(1.0f, g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale);
    g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.0f;

    ImFontAtlas* atlas = g.Font->ContainerAtlas;
    g.DrawListSharedData.TexUvWhitePixel = atlas->TexUvWhitePixel;
    g.DrawListSharedData.Font = g.Font;
    g.DrawListSharedData.FontSize = g.FontSize;
}

void vsonyp0wer::PushFont(ImFont * font)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!font)
        font = GetDefaultFont();
    SetCurrentFont(font);
    g.FontStack.push_back(font);
    g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void  vsonyp0wer::PopFont()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.CurrentWindow->DrawList->PopTextureID();
    g.FontStack.pop_back();
    SetCurrentFont(g.FontStack.empty() ? GetDefaultFont() : g.FontStack.back());
}

void vsonyp0wer::PushItemFlag(vsonyp0werItemFlags option, bool enabled)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (enabled)
        window->DC.ItemFlags |= option;
    else
        window->DC.ItemFlags &= ~option;
    window->DC.ItemFlagsStack.push_back(window->DC.ItemFlags);
}

void vsonyp0wer::PopItemFlag()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.ItemFlagsStack.pop_back();
    window->DC.ItemFlags = window->DC.ItemFlagsStack.empty() ? vsonyp0werItemFlags_Default_ : window->DC.ItemFlagsStack.back();
}

// FIXME: Look into renaming this once we have settled the new Focus/Activation/TabStop system.
void vsonyp0wer::PushAllowKeyboardFocus(bool allow_keyboard_focus)
{
    PushItemFlag(vsonyp0werItemFlags_NoTabStop, !allow_keyboard_focus);
}

void vsonyp0wer::PopAllowKeyboardFocus()
{
    PopItemFlag();
}

void vsonyp0wer::PushButtonRepeat(bool repeat)
{
    PushItemFlag(vsonyp0werItemFlags_ButtonRepeat, repeat);
}

void vsonyp0wer::PopButtonRepeat()
{
    PopItemFlag();
}

void vsonyp0wer::PushTextWrapPos(float wrap_pos_x)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.TextWrapPos = wrap_pos_x;
    window->DC.TextWrapPosStack.push_back(wrap_pos_x);
}

void vsonyp0wer::PopTextWrapPos()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.TextWrapPosStack.pop_back();
    window->DC.TextWrapPos = window->DC.TextWrapPosStack.empty() ? -1.0f : window->DC.TextWrapPosStack.back();
}

// FIXME: This may incur a round-trip (if the end user got their data from a float4) but eventually we aim to store the in-flight colors as ImU32
void vsonyp0wer::PushStyleColor(vsonyp0werCol idx, ImU32 col)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style.Colors[idx] = ColorConvertU32ToFloat4(col);
}

void vsonyp0wer::PushStyleColor(vsonyp0werCol idx, const ImVec4 & col)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werColorMod backup;
    backup.Col = idx;
    backup.BackupValue = g.Style.Colors[idx];
    g.ColorModifiers.push_back(backup);
    g.Style.Colors[idx] = col;
}

void vsonyp0wer::PushColor(vsonyp0werCol idx, vsonyp0werCol idx2, const ImVec4& col) {

	vsonyp0werContext& g = *Gvsonyp0wer;
	vsonyp0werColorMod backup;
	backup.Col = idx;
	backup.BackupValue = g.Style.Colors[idx];
	g.ColorModifiers.push_back(backup);
	g.Style.Colors[idx] = g.Style.Colors[idx2];
}

void vsonyp0wer::PopStyleColor(int count)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    while (count > 0)
    {
        vsonyp0werColorMod& backup = g.ColorModifiers.back();
        g.Style.Colors[backup.Col] = backup.BackupValue;
        g.ColorModifiers.pop_back();
        count--;
    }
}

struct vsonyp0werStyleVarInfo
{
    vsonyp0werDataType   Type;
    ImU32           Count;
    ImU32           Offset;
    void* GetVarPtr(vsonyp0werStyle* style) const { return (void*)((unsigned char*)style + Offset); }
};

static const vsonyp0werStyleVarInfo GStyleVarInfo[] =
{
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, Alpha) },               // vsonyp0werStyleVar_Alpha
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, WindowPadding) },       // vsonyp0werStyleVar_WindowPadding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, WindowRounding) },      // vsonyp0werStyleVar_WindowRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, WindowBorderSize) },    // vsonyp0werStyleVar_WindowBorderSize
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, WindowMinSize) },       // vsonyp0werStyleVar_WindowMinSize
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, WindowTitleAlign) },    // vsonyp0werStyleVar_WindowTitleAlign
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ChildRounding) },       // vsonyp0werStyleVar_ChildRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ChildBorderSize) },     // vsonyp0werStyleVar_ChildBorderSize
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, PopupRounding) },       // vsonyp0werStyleVar_PopupRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, PopupBorderSize) },     // vsonyp0werStyleVar_PopupBorderSize
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, FramePadding) },        // vsonyp0werStyleVar_FramePadding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, FrameRounding) },       // vsonyp0werStyleVar_FrameRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, FrameBorderSize) },     // vsonyp0werStyleVar_FrameBorderSize
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ItemSpacing) },         // vsonyp0werStyleVar_ItemSpacing
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ItemInnerSpacing) },    // vsonyp0werStyleVar_ItemInnerSpacing
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, IndentSpacing) },       // vsonyp0werStyleVar_IndentSpacing
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ScrollbarSize) },       // vsonyp0werStyleVar_ScrollbarSize
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ScrollbarRounding) },   // vsonyp0werStyleVar_ScrollbarRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, GrabMinSize) },         // vsonyp0werStyleVar_GrabMinSize
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, GrabRounding) },        // vsonyp0werStyleVar_GrabRounding
    { vsonyp0werDataType_Float, 1, (ImU32)IM_OFFSETOF(vsonyp0werStyle, TabRounding) },         // vsonyp0werStyleVar_TabRounding
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, ButtonTextAlign) },     // vsonyp0werStyleVar_ButtonTextAlign
    { vsonyp0werDataType_Float, 2, (ImU32)IM_OFFSETOF(vsonyp0werStyle, SelectableTextAlign) }, // vsonyp0werStyleVar_SelectableTextAlign
};

static const vsonyp0werStyleVarInfo* GetStyleVarInfo(vsonyp0werStyleVar idx)
{
    IM_ASSERT(idx >= 0 && idx < vsonyp0werStyleVar_COUNT);
    IM_ASSERT(IM_ARRAYSIZE(GStyleVarInfo) == vsonyp0werStyleVar_COUNT);
    return &GStyleVarInfo[idx];
}

void vsonyp0wer::PushStyleVar(vsonyp0werStyleVar idx, float val)
{
    const vsonyp0werStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == vsonyp0werDataType_Float && var_info->Count == 1)
    {
        vsonyp0werContext& g = *Gvsonyp0wer;
        float* pvar = (float*)var_info->GetVarPtr(&g.Style);
        g.StyleModifiers.push_back(vsonyp0werStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0); // Called function with wrong-type? Variable is not a float.
}

void vsonyp0wer::PushStyleVar(vsonyp0werStyleVar idx, const ImVec2 & val)
{
    const vsonyp0werStyleVarInfo* var_info = GetStyleVarInfo(idx);
    if (var_info->Type == vsonyp0werDataType_Float && var_info->Count == 2)
    {
        vsonyp0werContext& g = *Gvsonyp0wer;
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&g.Style);
        g.StyleModifiers.push_back(vsonyp0werStyleMod(idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0); // Called function with wrong-type? Variable is not a ImVec2.
}

void vsonyp0wer::PopStyleVar(int count)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    while (count > 0)
    {
        // We avoid a generic memcpy(data, &backup.Backup.., GDataTypeSize[info->Type] * info->Count), the overhead in Debug is not worth it.
        vsonyp0werStyleMod& backup = g.StyleModifiers.back();
        const vsonyp0werStyleVarInfo* info = GetStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&g.Style);
        if (info->Type == vsonyp0werDataType_Float && info->Count == 1) { ((float*)data)[0] = backup.BackupFloat[0]; } else if (info->Type == vsonyp0werDataType_Float && info->Count == 2) { ((float*)data)[0] = backup.BackupFloat[0]; ((float*)data)[1] = backup.BackupFloat[1]; }
        g.StyleModifiers.pop_back();
        count--;
    }
}

const char* vsonyp0wer::GetStyleColorName(vsonyp0werCol idx)
{
    // Create switch-case from enum with regexp: vsonyp0werCol_{.*}, --> case vsonyp0werCol_\1: return "\1";
    switch (idx)
    {
    case vsonyp0werCol_Text: return "Text";
    case vsonyp0werCol_TextDisabled: return "TextDisabled";
    case vsonyp0werCol_WindowBg: return "WindowBg";
    case vsonyp0werCol_ChildBg: return "ChildBg";
    case vsonyp0werCol_PopupBg: return "PopupBg";
    case vsonyp0werCol_Border: return "Border";
    case vsonyp0werCol_BorderShadow: return "BorderShadow";
    case vsonyp0werCol_FrameBg: return "FrameBg";
    case vsonyp0werCol_FrameBgHovered: return "FrameBgHovered";
    case vsonyp0werCol_FrameBgActive: return "FrameBgActive";
    case vsonyp0werCol_TitleBg: return "TitleBg";
    case vsonyp0werCol_TitleBgActive: return "TitleBgActive";
    case vsonyp0werCol_TitleBgCollapsed: return "TitleBgCollapsed";
    case vsonyp0werCol_MenuBarBg: return "MenuBarBg";
    case vsonyp0werCol_ScrollbarBg: return "ScrollbarBg";
    case vsonyp0werCol_ScrollbarGrab: return "ScrollbarGrab";
    case vsonyp0werCol_ScrollbarGrabHovered: return "ScrollbarGrabHovered";
    case vsonyp0werCol_ScrollbarGrabActive: return "ScrollbarGrabActive";
    case vsonyp0werCol_CheckMark: return "CheckMark";
    case vsonyp0werCol_SliderGrab: return "SliderGrab";
    case vsonyp0werCol_SliderGrabActive: return "SliderGrabActive";
    case vsonyp0werCol_Button: return "Button";
    case vsonyp0werCol_ButtonHovered: return "ButtonHovered";
    case vsonyp0werCol_ButtonActive: return "ButtonActive";
    case vsonyp0werCol_Header: return "Header";
    case vsonyp0werCol_HeaderHovered: return "HeaderHovered";
    case vsonyp0werCol_HeaderActive: return "HeaderActive";
    case vsonyp0werCol_Separator: return "Separator";
    case vsonyp0werCol_SeparatorHovered: return "SeparatorHovered";
    case vsonyp0werCol_SeparatorActive: return "SeparatorActive";
    case vsonyp0werCol_ResizeGrip: return "ResizeGrip";
    case vsonyp0werCol_ResizeGripHovered: return "ResizeGripHovered";
    case vsonyp0werCol_ResizeGripActive: return "ResizeGripActive";
    case vsonyp0werCol_Tab: return "Tab";
    case vsonyp0werCol_TabHovered: return "TabHovered";
    case vsonyp0werCol_TabActive: return "TabActive";
    case vsonyp0werCol_TabUnfocused: return "TabUnfocused";
    case vsonyp0werCol_TabUnfocusedActive: return "TabUnfocusedActive";
    case vsonyp0werCol_PlotLines: return "PlotLines";
    case vsonyp0werCol_PlotLinesHovered: return "PlotLinesHovered";
    case vsonyp0werCol_PlotHistogram: return "PlotHistogram";
    case vsonyp0werCol_PlotHistogramHovered: return "PlotHistogramHovered";
    case vsonyp0werCol_TextSelectedBg: return "TextSelectedBg";
    case vsonyp0werCol_DragDropTarget: return "DragDropTarget";
    case vsonyp0werCol_NavHighlight: return "NavHighlight";
    case vsonyp0werCol_NavWindowingHighlight: return "NavWindowingHighlight";
    case vsonyp0werCol_NavWindowingDimBg: return "NavWindowingDimBg";
    case vsonyp0werCol_ModalWindowDimBg: return "ModalWindowDimBg";
    }
    IM_ASSERT(0);
    return "Unknown";
}

bool vsonyp0wer::IsWindowChildOf(vsonyp0werWindow * window, vsonyp0werWindow * potential_parent)
{
    if (window->RootWindow == potential_parent)
        return true;
    while (window != NULL)
    {
        if (window == potential_parent)
            return true;
        window = window->ParentWindow;
    }
    return false;
}

bool vsonyp0wer::IsWindowHovered(vsonyp0werHoveredFlags flags)
{
    IM_ASSERT((flags & vsonyp0werHoveredFlags_AllowWhenOverlapped) == 0);   // Flags not supported by this function
    vsonyp0werContext & g = *Gvsonyp0wer;

    if (flags & vsonyp0werHoveredFlags_AnyWindow)
    {
        if (g.HoveredWindow == NULL)
            return false;
    } else
    {
        switch (flags & (vsonyp0werHoveredFlags_RootWindow | vsonyp0werHoveredFlags_ChildWindows))
        {
        case vsonyp0werHoveredFlags_RootWindow | vsonyp0werHoveredFlags_ChildWindows:
            if (g.HoveredRootWindow != g.CurrentWindow->RootWindow)
                return false;
            break;
        case vsonyp0werHoveredFlags_RootWindow:
            if (g.HoveredWindow != g.CurrentWindow->RootWindow)
                return false;
            break;
        case vsonyp0werHoveredFlags_ChildWindows:
            if (g.HoveredWindow == NULL || !IsWindowChildOf(g.HoveredWindow, g.CurrentWindow))
                return false;
            break;
        default:
            if (g.HoveredWindow != g.CurrentWindow)
                return false;
            break;
        }
    }

    if (!IsWindowContentHoverable(g.HoveredWindow, flags))
        return false;
    if (!(flags & vsonyp0werHoveredFlags_AllowWhenBlockedByActiveItem))
        if (g.ActiveId != 0 && !g.ActiveIdAllowOverlap && g.ActiveId != g.HoveredWindow->MoveId)
            return false;
    return true;
}

bool vsonyp0wer::IsWindowFocused(vsonyp0werFocusedFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    if (flags & vsonyp0werFocusedFlags_AnyWindow)
        return g.NavWindow != NULL;

    IM_ASSERT(g.CurrentWindow);     // Not inside a Begin()/End()
    switch (flags & (vsonyp0werFocusedFlags_RootWindow | vsonyp0werFocusedFlags_ChildWindows))
    {
    case vsonyp0werFocusedFlags_RootWindow | vsonyp0werFocusedFlags_ChildWindows:
        return g.NavWindow && g.NavWindow->RootWindow == g.CurrentWindow->RootWindow;
    case vsonyp0werFocusedFlags_RootWindow:
        return g.NavWindow == g.CurrentWindow->RootWindow;
    case vsonyp0werFocusedFlags_ChildWindows:
        return g.NavWindow&& IsWindowChildOf(g.NavWindow, g.CurrentWindow);
    default:
        return g.NavWindow == g.CurrentWindow;
    }
}

// Can we focus this window with CTRL+TAB (or PadMenu + PadFocusPrev/PadFocusNext)
// Note that NoNavFocus makes the window not reachable with CTRL+TAB but it can still be focused with mouse or programmaticaly.
// If you want a window to never be focused, you may use the e.g. NoInputs flag.
bool vsonyp0wer::IsWindowNavFocusable(vsonyp0werWindow * window)
{
    return window->Active&& window == window->RootWindow && !(window->Flags & vsonyp0werWindowFlags_NoNavFocus);
}

float vsonyp0wer::GetWindowWidth()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->Size.x;
}

float vsonyp0wer::GetWindowHeight()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->Size.y;
}

ImVec2 vsonyp0wer::GetWindowPos()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    return window->Pos;
}

void vsonyp0wer::SetWindowScrollX(vsonyp0werWindow * window, float new_scroll_x)
{
    window->DC.CursorMaxPos.x += window->Scroll.x; // SizeContents is generally computed based on CursorMaxPos which is affected by scroll position, so we need to apply our change to it.
    window->Scroll.x = new_scroll_x;
    window->DC.CursorMaxPos.x -= window->Scroll.x;
}

void vsonyp0wer::SetWindowScrollY(vsonyp0werWindow * window, float new_scroll_y)
{
    window->DC.CursorMaxPos.y += window->Scroll.y; // SizeContents is generally computed based on CursorMaxPos which is affected by scroll position, so we need to apply our change to it.
    window->Scroll.y = new_scroll_y;
    window->DC.CursorMaxPos.y -= window->Scroll.y;
}

void vsonyp0wer::SetWindowPos(vsonyp0werWindow * window, const ImVec2 & pos, vsonyp0werCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowPosAllowFlags & cond) == 0)
        return;

    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    window->SetWindowPosAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);
    window->SetWindowPosVal = ImVec2(FLT_MAX, FLT_MAX);

    // Set
    const ImVec2 old_pos = window->Pos;
    window->Pos = ImFloor(pos);
    window->DC.CursorPos += (window->Pos - old_pos);    // As we happen to move the window while it is being appended to (which is a bad idea - will smear) let's at least offset the cursor
    window->DC.CursorMaxPos += (window->Pos - old_pos); // And more importantly we need to adjust this so size calculation doesn't get affected.
}

void vsonyp0wer::SetWindowPos(const ImVec2 & pos, vsonyp0werCond cond)
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    SetWindowPos(window, pos, cond);
}

void vsonyp0wer::SetWindowPos(const char* name, const ImVec2 & pos, vsonyp0werCond cond)
{
    if (vsonyp0werWindow * window = FindWindowByName(name))
        SetWindowPos(window, pos, cond);
}

ImVec2 vsonyp0wer::GetWindowSize()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->Size;
}

void vsonyp0wer::SetWindowSize(vsonyp0werWindow * window, const ImVec2 & size, vsonyp0werCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowSizeAllowFlags & cond) == 0)
        return;

    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    window->SetWindowSizeAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);

    // Set
    if (size.x > 0.0f)
    {
        window->AutoFitFramesX = 0;
        window->SizeFull.x = ImFloor(size.x);
    } else
    {
        window->AutoFitFramesX = 2;
        window->AutoFitOnlyGrows = false;
    }
    if (size.y > 0.0f)
    {
        window->AutoFitFramesY = 0;
        window->SizeFull.y = ImFloor(size.y);
    } else
    {
        window->AutoFitFramesY = 2;
        window->AutoFitOnlyGrows = false;
    }
}

void vsonyp0wer::SetWindowSize(const ImVec2 & size, vsonyp0werCond cond)
{
    SetWindowSize(Gvsonyp0wer->CurrentWindow, size, cond);
}

void vsonyp0wer::SetWindowSize(const char* name, const ImVec2 & size, vsonyp0werCond cond)
{
    if (vsonyp0werWindow * window = FindWindowByName(name))
        SetWindowSize(window, size, cond);
}

void vsonyp0wer::SetWindowCollapsed(vsonyp0werWindow * window, bool collapsed, vsonyp0werCond cond)
{
    // Test condition (NB: bit 0 is always true) and clear flags for next time
    if (cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
        return;
    window->SetWindowCollapsedAllowFlags &= ~(vsonyp0werCond_Once | vsonyp0werCond_FirstUseEver | vsonyp0werCond_Appearing);

    // Set
    window->Collapsed = collapsed;
}

void vsonyp0wer::SetWindowCollapsed(bool collapsed, vsonyp0werCond cond)
{
    SetWindowCollapsed(Gvsonyp0wer->CurrentWindow, collapsed, cond);
}

bool vsonyp0wer::IsWindowCollapsed()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->Collapsed;
}

bool vsonyp0wer::IsWindowAppearing()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->Appearing;
}

void vsonyp0wer::SetWindowCollapsed(const char* name, bool collapsed, vsonyp0werCond cond)
{
    if (vsonyp0werWindow * window = FindWindowByName(name))
        SetWindowCollapsed(window, collapsed, cond);
}

void vsonyp0wer::SetWindowFocus()
{
    FocusWindow(Gvsonyp0wer->CurrentWindow);
}

void vsonyp0wer::SetWindowFocus(const char* name)
{
    if (name)
    {
        if (vsonyp0werWindow * window = FindWindowByName(name))
            FocusWindow(window);
    } else
    {
        FocusWindow(NULL);
    }
}

void vsonyp0wer::SetNextWindowPos(const ImVec2 & pos, vsonyp0werCond cond, const ImVec2 & pivot)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = pivot;
    g.NextWindowData.PosCond = cond ? cond : vsonyp0werCond_Always;
}

void vsonyp0wer::SetNextWindowSize(const ImVec2 & size, vsonyp0werCond cond)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.SizeVal = size;
    g.NextWindowData.SizeCond = cond ? cond : vsonyp0werCond_Always;
}

void vsonyp0wer::SetNextWindowSizeConstraints(const ImVec2 & size_min, const ImVec2 & size_max, vsonyp0werSizeCallback custom_callback, void* custom_callback_user_data)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NextWindowData.SizeConstraintCond = vsonyp0werCond_Always;
    g.NextWindowData.SizeConstraintRect = ImRect(size_min, size_max);
    g.NextWindowData.SizeCallback = custom_callback;
    g.NextWindowData.SizeCallbackUserData = custom_callback_user_data;
}

void vsonyp0wer::SetNextWindowContentSize(const ImVec2 & size)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NextWindowData.ContentSizeVal = size;  // In Begin() we will add the size of window decorations (title bar, menu etc.) to that to form a SizeContents value.
    g.NextWindowData.ContentSizeCond = vsonyp0werCond_Always;
}

void vsonyp0wer::SetNextWindowCollapsed(bool collapsed, vsonyp0werCond cond)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    g.NextWindowData.CollapsedVal = collapsed;
    g.NextWindowData.CollapsedCond = cond ? cond : vsonyp0werCond_Always;
}

void vsonyp0wer::SetNextWindowFocus()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NextWindowData.FocusCond = vsonyp0werCond_Always;   // Using a Cond member for consistency (may transition all of them to single flag set for fast Clear() op)
}

void vsonyp0wer::SetNextWindowBgAlpha(float alpha)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NextWindowData.BgAlphaVal = alpha;
    g.NextWindowData.BgAlphaCond = vsonyp0werCond_Always; // Using a Cond member for consistency (may transition all of them to single flag set for fast Clear() op)
}

// FIXME: This is in window space (not screen space!)
ImVec2 vsonyp0wer::GetContentRegionMax()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    ImVec2 mx = window->ContentsRegionRect.Max - window->Pos;
    if (window->DC.CurrentColumns)
        mx.x = GetColumnOffset(window->DC.CurrentColumns->Current + 1) - window->WindowPadding.x;
    return mx;
}

// [Internal] Absolute coordinate. Saner. This is not exposed until we finishing refactoring work rect features.
ImVec2 vsonyp0wer::GetContentRegionMaxScreen()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    ImVec2 mx = window->ContentsRegionRect.Max;
    if (window->DC.CurrentColumns)
        mx.x = window->Pos.x + GetColumnOffset(window->DC.CurrentColumns->Current + 1) - window->WindowPadding.x;
    return mx;
}

ImVec2 vsonyp0wer::GetContentRegionAvail()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return GetContentRegionMaxScreen() - window->DC.CursorPos;
}

float vsonyp0wer::GetContentRegionAvailWidth()
{
    return GetContentRegionAvail().x;
}

// In window space (not screen space!)
ImVec2 vsonyp0wer::GetWindowContentRegionMin()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->ContentsRegionRect.Min - window->Pos;
}

ImVec2 vsonyp0wer::GetWindowContentRegionMax()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->ContentsRegionRect.Max - window->Pos;
}

float vsonyp0wer::GetWindowContentRegionWidth()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->ContentsRegionRect.GetWidth();
}

float vsonyp0wer::GetTextLineHeight()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize;
}

float vsonyp0wer::GetTextLineHeightWithSpacing()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize + g.Style.ItemSpacing.y;
}

float vsonyp0wer::GetFrameHeight()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize + g.Style.FramePadding.y * 2.0f;
}

float vsonyp0wer::GetFrameHeightWithSpacing()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.FontSize + g.Style.FramePadding.y * 2.0f + g.Style.ItemSpacing.y;
}

ImDrawList* vsonyp0wer::GetWindowDrawList()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    return window->DrawList;
}

ImFont* vsonyp0wer::GetFont()
{
    return Gvsonyp0wer->Font;
}

float vsonyp0wer::GetFontSize()
{
    return Gvsonyp0wer->FontSize;
}

ImVec2 vsonyp0wer::GetFontTexUvWhitePixel()
{
    return Gvsonyp0wer->DrawListSharedData.TexUvWhitePixel;
}

void vsonyp0wer::SetWindowFontScale(float scale)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();
    window->FontWindowScale = scale;
    g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

// User generally sees positions in window coordinates. Internally we store CursorPos in absolute screen coordinates because it is more convenient.
// Conversion happens as we pass the value to user, but it makes our naming convention confusing because GetCursorPos() == (DC.CursorPos - window.Pos). May want to rename 'DC.CursorPos'.
ImVec2 vsonyp0wer::GetCursorPos()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos - window->Pos + window->Scroll;
}

float vsonyp0wer::GetCursorPosX()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.x - window->Pos.x + window->Scroll.x;
}

float vsonyp0wer::GetCursorPosY()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos.y - window->Pos.y + window->Scroll.y;
}

void vsonyp0wer::SetCursorPos(const ImVec2 & local_pos)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

void vsonyp0wer::SetCursorPosX(float x)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + x;
    window->DC.CursorMaxPos.x = ImMax(window->DC.CursorMaxPos.x, window->DC.CursorPos.x);
}

void vsonyp0wer::SetCursorPosY(float y)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.CursorPos.y = window->Pos.y - window->Scroll.y + y;
    window->DC.CursorMaxPos.y = ImMax(window->DC.CursorMaxPos.y, window->DC.CursorPos.y);
}

ImVec2 vsonyp0wer::GetCursorStartPos()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CursorStartPos - window->Pos;
}

ImVec2 vsonyp0wer::GetCursorScreenPos()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CursorPos;
}

void vsonyp0wer::SetCursorScreenPos(const ImVec2 & pos)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.CursorPos = pos;
    window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

float vsonyp0wer::GetScrollX()
{
    return Gvsonyp0wer->CurrentWindow->Scroll.x;
}

float vsonyp0wer::GetScrollY()
{
    return Gvsonyp0wer->CurrentWindow->Scroll.y;
}

float vsonyp0wer::GetScrollMaxX()
{
    return GetWindowScrollMaxX(Gvsonyp0wer->CurrentWindow);
}

float vsonyp0wer::GetScrollMaxY()
{
    return GetWindowScrollMaxY(Gvsonyp0wer->CurrentWindow);
}

void vsonyp0wer::SetScrollX(float scroll_x)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->ScrollTarget.x = scroll_x;
    window->ScrollTargetCenterRatio.x = 0.0f;
}

void vsonyp0wer::SetScrollY(float scroll_y)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    window->ScrollTarget.y = scroll_y + window->TitleBarHeight() + window->MenuBarHeight(); // title bar height canceled out when using ScrollTargetRelY
    window->ScrollTargetCenterRatio.y = 0.0f;
}

void vsonyp0wer::SetScrollFromPosY(float local_y, float center_y_ratio)
{
    // We store a target position so centering can occur on the next frame when we are guaranteed to have a known window size
    vsonyp0werWindow* window = GetCurrentWindow();
    IM_ASSERT(center_y_ratio >= 0.0f && center_y_ratio <= 1.0f);
    window->ScrollTarget.y = (float)(int)(local_y + window->Scroll.y);
    window->ScrollTargetCenterRatio.y = center_y_ratio;
}

// center_y_ratio: 0.0f top of last item, 0.5f vertical center of last item, 1.0f bottom of last item.
void vsonyp0wer::SetScrollHereY(float center_y_ratio)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    float target_y = window->DC.CursorPosPrevLine.y - window->Pos.y; // Top of last item, in window space
    target_y += (window->DC.PrevLineSize.y * center_y_ratio) + (Gvsonyp0wer->Style.ItemSpacing.y * (center_y_ratio - 0.5f) * 2.0f); // Precisely aim above, in the middle or below the last line.
    SetScrollFromPosY(target_y, center_y_ratio);
}

void vsonyp0wer::ActivateItem(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NavNextActivateId = id;
}

void vsonyp0wer::SetKeyboardFocusHere(int offset)
{
    IM_ASSERT(offset >= -1);    // -1 is allowed but not below
    vsonyp0werContext & g = *Gvsonyp0wer;
    vsonyp0werWindow * window = g.CurrentWindow;
    g.FocusRequestNextWindow = window;
    g.FocusRequestNextCounterAll = window->DC.FocusCounterAll + 1 + offset;
    g.FocusRequestNextCounterTab = INT_MAX;
}

void vsonyp0wer::SetItemDefaultFocus()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (!window->Appearing)
        return;
    if (g.NavWindow == window->RootWindowForNav && (g.NavInitRequest || g.NavInitResultId != 0) && g.NavLayer == g.NavWindow->DC.NavLayerCurrent)
    {
        g.NavInitRequest = false;
        g.NavInitResultId = g.NavWindow->DC.LastItemId;
        g.NavInitResultRectRel = ImRect(g.NavWindow->DC.LastItemRect.Min - g.NavWindow->Pos, g.NavWindow->DC.LastItemRect.Max - g.NavWindow->Pos);
        NavUpdateAnyRequestFlag();
        if (!IsItemVisible())
            SetScrollHereY();
    }
}

void vsonyp0wer::SetStateStohnly(vsonyp0werStohnly * tree)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->DC.StateStohnly = tree ? tree : &window->StateStohnly;
}

vsonyp0werStohnly* vsonyp0wer::GetStateStohnly()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->DC.StateStohnly;
}

void vsonyp0wer::PushID(const char* str_id)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->IDStack.push_back(window->GetIDNoKeepAlive(str_id));
}

void vsonyp0wer::PushID(const char* str_id_begin, const char* str_id_end)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->IDStack.push_back(window->GetIDNoKeepAlive(str_id_begin, str_id_end));
}

void vsonyp0wer::PushID(const void* ptr_id)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->IDStack.push_back(window->GetIDNoKeepAlive(ptr_id));
}

void vsonyp0wer::PushID(int int_id)
{
    const void* ptr_id = (void*)(intptr_t)int_id;
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->IDStack.push_back(window->GetIDNoKeepAlive(ptr_id));
}

void vsonyp0wer::PopID()
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    window->IDStack.pop_back();
}

vsonyp0werID vsonyp0wer::GetID(const char* str_id)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->GetID(str_id);
}

vsonyp0werID vsonyp0wer::GetID(const char* str_id_begin, const char* str_id_end)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->GetID(str_id_begin, str_id_end);
}

vsonyp0werID vsonyp0wer::GetID(const void* ptr_id)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->GetID(ptr_id);
}

bool vsonyp0wer::IsRectVisible(const ImVec2 & size)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->ClipRect.Overlaps(ImRect(window->DC.CursorPos, window->DC.CursorPos + size));
}

bool vsonyp0wer::IsRectVisible(const ImVec2 & rect_min, const ImVec2 & rect_max)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    return window->ClipRect.Overlaps(ImRect(rect_min, rect_max));
}

// Lock horizontal starting position + capture group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
void vsonyp0wer::BeginGroup()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();

    window->DC.GroupStack.resize(window->DC.GroupStack.Size + 1);
    vsonyp0werGroupData & group_data = window->DC.GroupStack.back();
    group_data.BackupCursorPos = window->DC.CursorPos;
    group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
    group_data.BackupIndent = window->DC.Indent;
    group_data.BackupGroupOffset = window->DC.GroupOffset;
    group_data.BackupCurrentLineSize = window->DC.CurrentLineSize;
    group_data.BackupCurrentLineTextBaseOffset = window->DC.CurrentLineTextBaseOffset;
    group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
    group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
    group_data.AdvanceCursor = true;

    window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
    window->DC.Indent = window->DC.GroupOffset;
    window->DC.CursorMaxPos = window->DC.CursorPos;
    window->DC.CurrentLineSize = ImVec2(0.0f, 0.0f);
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX; // To enforce Log carriage return
}

void vsonyp0wer::EndGroup()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();
    IM_ASSERT(!window->DC.GroupStack.empty());  // Mismatched BeginGroup()/EndGroup() calls

    vsonyp0werGroupData& group_data = window->DC.GroupStack.back();

    ImRect group_bb(group_data.BackupCursorPos, window->DC.CursorMaxPos);
    group_bb.Max = ImMax(group_bb.Min, group_bb.Max);

    window->DC.CursorPos = group_data.BackupCursorPos;
    window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
    window->DC.Indent = group_data.BackupIndent;
    window->DC.GroupOffset = group_data.BackupGroupOffset;
    window->DC.CurrentLineSize = group_data.BackupCurrentLineSize;
    window->DC.CurrentLineTextBaseOffset = group_data.BackupCurrentLineTextBaseOffset;
    if (g.LogEnabled)
        g.LogLinePosY = -FLT_MAX; // To enforce Log carriage return

    if (group_data.AdvanceCursor)
    {
        window->DC.CurrentLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrentLineTextBaseOffset);      // FIXME: Incorrect, we should grab the base offset from the *first line* of the group but it is hard to obtain now.
        ItemSize(group_bb.GetSize(), 0.0f);
        ItemAdd(group_bb, 0);
    }

    // If the current ActiveId was declared within the boundary of our group, we copy it to LastItemId so IsItemActive(), IsItemDeactivated() etc. will be functional on the entire group.
    // It would be be neater if we replaced window.DC.LastItemId by e.g. 'bool LastItemIsActive', but would put a little more burden on individual widgets.
    // (and if you grep for LastItemId you'll notice it is only used in that context.
    if ((group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId) // && g.ActiveIdWindow->RootWindow == window->RootWindow)
        window->DC.LastItemId = g.ActiveId;
    else if (!group_data.BackupActiveIdPreviousFrameIsAlive && g.ActiveIdPreviousFrameIsAlive) // && g.ActiveIdPreviousFrameWindow->RootWindow == window->RootWindow)
        window->DC.LastItemId = g.ActiveIdPreviousFrame;
    window->DC.LastItemRect = group_bb;

    window->DC.GroupStack.pop_back();

    //window->DrawList->AddRect(group_bb.Min, group_bb.Max, IM_COL32(255,0,255,255));   // [Debug]
}

// Gets back to previous line and continue with horizontal layout
//      offset_from_start_x == 0 : follow right after previous item
//      offset_from_start_x != 0 : align to specified x position (relative to window/group left)
//      spacing_w < 0            : use default spacing if pos_x == 0, no spacing if pos_x != 0
//      spacing_w >= 0           : enforce spacing amount
void vsonyp0wer::SameLine(float offset_from_start_x, float spacing_w)
{
    vsonyp0werWindow* window = GetCurrentWindow();

    if (window->SkipItems)
        return;

    vsonyp0werContext& g = *Gvsonyp0wer;

    if (offset_from_start_x != 0.0f) {

        if (spacing_w < 0.0f)
			spacing_w = 0.0f;

        window->DC.CursorPos.x = window->Pos.x - window->Scroll.x + offset_from_start_x + spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }
	else {

        if (spacing_w < 0.0f)
			spacing_w = g.Style.ItemSpacing.x;

        window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + spacing_w;
        window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    }

    window->DC.CurrentLineSize = window->DC.PrevLineSize;
    window->DC.CurrentLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

void vsonyp0wer::Indent(float indent_w)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.Indent.x += (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

void vsonyp0wer::Unindent(float indent_w)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();
    window->DC.Indent.x -= (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
    window->DC.CursorPos.x = window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

//-----------------------------------------------------------------------------
// [SECTION] TOOLTIPS
//-----------------------------------------------------------------------------

void vsonyp0wer::BeginTooltip()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.DragDropWithinSourceOrTarget)
    {
        // The default tooltip position is a little offset to give space to see the context menu (it's also clamped within the current viewport/monitor)
        // In the context of a dragging tooltip we try to reduce that offset and we enforce following the cursor.
        // Whatever we do we want to call SetNextWindowPos() to enforce a tooltip position and disable clipping the tooltip without our display area, like regular tooltip do.
        //ImVec2 tooltip_pos = g.IO.MousePos - g.ActiveIdClickOffset - g.Style.WindowPadding;
        ImVec2 tooltip_pos = g.IO.MousePos + ImVec2(16 * g.Style.MouseCursorScale, 8 * g.Style.MouseCursorScale);
        SetNextWindowPos(tooltip_pos);
        SetNextWindowBgAlpha(g.Style.Colors[vsonyp0werCol_PopupBg].w * 0.60f);
        //PushStyleVar(vsonyp0werStyleVar_Alpha, g.Style.Alpha * 0.60f); // This would be nice but e.g ColorButton with checkboard has issue with transparent colors :(
        BeginTooltipEx(0, true);
    } else
    {
        BeginTooltipEx(0, false);
    }
}

// Not exposed publicly as BeginTooltip() because bool parameters are evil. Let's see if other needs arise first.
void vsonyp0wer::BeginTooltipEx(vsonyp0werWindowFlags extra_flags, bool override_previous_tooltip)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
	ImVec2 tooltip_pos = g.IO.MousePos + ImVec2(0 * g.Style.MouseCursorScale, -18 * g.Style.MouseCursorScale);
	SetNextWindowPos(tooltip_pos);
	SetNextWindowBgAlpha(0.8f);
	g.Style.WindowRounding = 4.f;
	g.Style.WindowPadding = ImVec2(0, 0.5);
    char window_name[16];
    ImFormatString(window_name, IM_ARRAYSIZE(window_name), "##Tooltip_%02d", g.TooltipOverrideCount);

	if (override_previous_tooltip) {

		if (vsonyp0werWindow * window = FindWindowByName(window_name)) {

			if (window->Active) {

				// Hide previous tooltip from being displayed. We can't easily "reset" the content of a window so we create a new one.			
				window->Hidden = true;
				window->HiddenFramesCanSkipItems = 1;
				ImFormatString(window_name, IM_ARRAYSIZE(window_name), "##Tooltip_%02d", ++g.TooltipOverrideCount);
			}
		}
	}
            
    vsonyp0werWindowFlags flags = vsonyp0werWindowFlags_Tooltip | vsonyp0werWindowFlags_NoInputs | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoSavedSettings | vsonyp0werWindowFlags_AlwaysAutoResize;
    Begin(window_name, NULL, flags | extra_flags);
	g.Style.WindowPadding = ImVec2(4, 4);
	g.Style.WindowRounding = 0.f;
}

void vsonyp0wer::EndTooltip()
{
    IM_ASSERT(GetCurrentWindowRead()->Flags & vsonyp0werWindowFlags_Tooltip);   // Mismatched BeginTooltip()/EndTooltip() calls
    End();
}

void vsonyp0wer::SetTooltipV(const char* fmt, va_list args)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.DragDropWithinSourceOrTarget)
        BeginTooltip();
    else
        BeginTooltipEx(0, true);
    TextV(fmt, args);
    EndTooltip();
}

void vsonyp0wer::SetTooltip(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SetTooltipV(fmt, args);
    va_end(args);
}

//-----------------------------------------------------------------------------
// [SECTION] POPUPS
//-----------------------------------------------------------------------------

bool vsonyp0wer::IsPopupOpen(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.OpenPopupStack.Size > g.BeginPopupStack.Size&& g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == id;
}

bool vsonyp0wer::IsPopupOpen(const char* str_id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.OpenPopupStack.Size > g.BeginPopupStack.Size&& g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == g.CurrentWindow->GetID(str_id);
}

vsonyp0werWindow* vsonyp0wer::GetFrontMostPopupModal()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int n = g.OpenPopupStack.Size - 1; n >= 0; n--)
        if (vsonyp0werWindow * popup = g.OpenPopupStack.Data[n].Window)
            if (popup->Flags & vsonyp0werWindowFlags_Modal)
                return popup;
    return NULL;
}

void vsonyp0wer::OpenPopup(const char* str_id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    OpenPopupEx(g.CurrentWindow->GetID(str_id));
}

// Mark popup as open (toggle toward open state).
// Popups are closed when user click outside, or activate a pressable item, or CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block.
// Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
// One open popup per level of the popup hierarchy (NB: when assigning we reset the Window member of vsonyp0werPopupRef to NULL)
void vsonyp0wer::OpenPopupEx(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* parent_window = g.CurrentWindow;
    int current_stack_size = g.BeginPopupStack.Size;
    vsonyp0werPopupRef popup_ref; // Tagged as new ref as Window will be set back to NULL if we write this into OpenPopupStack.
    popup_ref.PopupId = id;
    popup_ref.Window = NULL;
    popup_ref.ParentWindow = parent_window;
    popup_ref.OpenFrameCount = g.FrameCount;
    popup_ref.OpenParentId = parent_window->IDStack.back();
    popup_ref.OpenPopupPos = NavCalcPreferredRefPos();
    popup_ref.OpenMousePos = IsMousePosValid(&g.IO.MousePos) ? g.IO.MousePos : popup_ref.OpenPopupPos;

    //vsonyp0wer_DEBUG_LOG("OpenPopupEx(0x%08X)\n", g.FrameCount, id);
    if (g.OpenPopupStack.Size < current_stack_size + 1)
    {
        g.OpenPopupStack.push_back(popup_ref);
    } else
    {
        // Gently handle the user mistakenly calling OpenPopup() every frame. It is a programming mistake! However, if we were to run the regular code path, the ui
        // would become completely unusable because the popup will always be in hidden-while-calculating-size state _while_ claiming focus. Which would be a very confusing
        // situation for the programmer. Instead, we silently allow the popup to proceed, it will keep reappearing and the programming error will be more obvious to understand.
        if (g.OpenPopupStack[current_stack_size].PopupId == id && g.OpenPopupStack[current_stack_size].OpenFrameCount == g.FrameCount - 1)
        {
            g.OpenPopupStack[current_stack_size].OpenFrameCount = popup_ref.OpenFrameCount;
        } else
        {
            // Close child popups if any, then flag popup for open/reopen
            g.OpenPopupStack.resize(current_stack_size + 1);
            g.OpenPopupStack[current_stack_size] = popup_ref;
        }

        // When reopening a popup we first refocus its parent, otherwise if its parent is itself a popup it would get closed by ClosePopupsOverWindow().
        // This is equivalent to what ClosePopupToLevel() does.
        //if (g.OpenPopupStack[current_stack_size].PopupId == id)
        //    FocusWindow(parent_window);
    }
}

bool vsonyp0wer::OpenPopupOnItemClick(const char* str_id, int mouse_button)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    if (IsMouseReleased(mouse_button) && IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup))
    {
        vsonyp0werID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
        IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
        OpenPopupEx(id);
        return true;
    }
    return false;
}

void vsonyp0wer::ClosePopupsOverWindow(vsonyp0werWindow * ref_window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.OpenPopupStack.empty())
        return;

    // When popups are stacked, clicking on a lower level popups puts focus back to it and close popups above it.
    // Don't close our own child popup windows.
    int popup_count_to_keep = 0;
    if (ref_window)
    {
        // Find the highest popup which is a descendant of the reference window (generally reference window = NavWindow)
        for (; popup_count_to_keep < g.OpenPopupStack.Size; popup_count_to_keep++)
        {
            vsonyp0werPopupRef& popup = g.OpenPopupStack[popup_count_to_keep];
            if (!popup.Window)
                continue;
            IM_ASSERT((popup.Window->Flags & vsonyp0werWindowFlags_Popup) != 0);
            if (popup.Window->Flags & vsonyp0werWindowFlags_ChildWindow)
                continue;

            // Trim the stack if popups are not direct descendant of the reference window (which is often the NavWindow)
            bool popup_or_descendent_has_focus = false;
            for (int m = popup_count_to_keep; m < g.OpenPopupStack.Size && !popup_or_descendent_has_focus; m++)
                if (g.OpenPopupStack[m].Window && g.OpenPopupStack[m].Window->RootWindow == ref_window->RootWindow)
                    popup_or_descendent_has_focus = true;
            if (!popup_or_descendent_has_focus)
                break;
        }
    }
    if (popup_count_to_keep < g.OpenPopupStack.Size) // This test is not required but it allows to set a convenient breakpoint on the statement below
    {
        //vsonyp0wer_DEBUG_LOG("ClosePopupsOverWindow(%s) -> ClosePopupToLevel(%d)\n", ref_window->Name, popup_count_to_keep);
        ClosePopupToLevel(popup_count_to_keep, false);
    }
}

void vsonyp0wer::ClosePopupToLevel(int remaining, bool apply_focus_to_window_under)
{
    IM_ASSERT(remaining >= 0);
    vsonyp0werContext & g = *Gvsonyp0wer;
    vsonyp0werWindow * focus_window = (remaining > 0) ? g.OpenPopupStack[remaining - 1].Window : g.OpenPopupStack[0].ParentWindow;
    g.OpenPopupStack.resize(remaining);

    // FIXME: This code is faulty and we may want to eventually to replace or remove the 'apply_focus_to_window_under=true' path completely.
    // Instead of using g.OpenPopupStack[remaining-1].Window etc. we should find the highest root window that is behind the popups we are closing.
    // The current code will set focus to the parent of the popup window which is incorrect.
    // It rarely manifested until now because UpdateMouseMovingWindowNewFrame() would call FocusWindow() again on the clicked window,
    // leading to a chain of focusing A (clicked window) then B (parent window of the popup) then A again.
    // However if the clicked window has the _NoMove flag set we would be left with B focused.
    // For now, we have disabled this path when called from ClosePopupsOverWindow() because the users of ClosePopupsOverWindow() don't need to alter focus anyway,
    // but we should inspect and fix this properly.
    if (apply_focus_to_window_under)
    {
        if (g.NavLayer == 0)
            focus_window = NavRestoreLastChildNavWindow(focus_window);
        FocusWindow(focus_window);
    }
}

// Close the popup we have begin-ed into.
void vsonyp0wer::CloseCurrentPopup()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    int popup_idx = g.BeginPopupStack.Size - 1;
    if (popup_idx < 0 || popup_idx >= g.OpenPopupStack.Size || g.BeginPopupStack[popup_idx].PopupId != g.OpenPopupStack[popup_idx].PopupId)
        return;

    // Closing a menu closes its top-most parent popup (unless a modal)
    while (popup_idx > 0)
    {
        vsonyp0werWindow* popup_window = g.OpenPopupStack[popup_idx].Window;
        vsonyp0werWindow* parent_popup_window = g.OpenPopupStack[popup_idx - 1].Window;
        bool close_parent = false;
        if (popup_window && (popup_window->Flags & vsonyp0werWindowFlags_ChildMenu))
            if (parent_popup_window == NULL || !(parent_popup_window->Flags & vsonyp0werWindowFlags_Modal))
                close_parent = true;
        if (!close_parent)
            break;
        popup_idx--;
    }
    //vsonyp0wer_DEBUG_LOG("CloseCurrentPopup %d -> %d\n", g.BeginPopupStack.Size - 1, popup_idx);
    ClosePopupToLevel(popup_idx, true);

    // A common pattern is to close a popup when selecting a menu item/selectable that will open another window.
    // To improve this usage pattern, we avoid nav highlight for a single frame in the parent window.
    // Similarly, we could avoid mouse hover highlight in this window but it is less visually problematic.
    if (vsonyp0werWindow * window = g.NavWindow)
        window->DC.NavHideHighlightOneFrame = true;
}

bool vsonyp0wer::BeginPopupEx(vsonyp0werID id, vsonyp0werWindowFlags extra_flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!IsPopupOpen(id))
    {
        g.NextWindowData.Clear(); // We behave like Begin() and need to consume those values
        return false;
    }

    char name[20];
    if (extra_flags & vsonyp0werWindowFlags_ChildMenu)
        ImFormatString(name, IM_ARRAYSIZE(name), "##Menu_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth
    else
        ImFormatString(name, IM_ARRAYSIZE(name), "##Popup_%08x", id); // Not recycling, so we can close/open during the same frame

    bool is_open = BeginColorPickerBackground(name, NULL, extra_flags | vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_NoMove);
    if (!is_open) // NB: Begin can return false when the popup is completely clipped (e.g. zero size display)
        EndPopup();

    return is_open;
}

bool vsonyp0wer::BeginPopup(const char* str_id, vsonyp0werWindowFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size) // Early out for performance
    {
        g.NextWindowData.Clear(); // We behave like Begin() and need to consume those values
        return false;
    }
    flags |= vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings;
    return BeginPopupEx(g.CurrentWindow->GetID(str_id), flags);
}

// If 'p_open' is specified for a modal popup window, the popup will have a regular close button which will close the popup.
// Note that popup visibility status is owned by vsonyp0wer (and manipulated with e.g. OpenPopup) so the actual value of *p_open is meaningless here.
bool vsonyp0wer::BeginPopupModal(const char* name, bool* p_open, vsonyp0werWindowFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    const vsonyp0werID id = window->GetID(name);
    if (!IsPopupOpen(id))
    {
        g.NextWindowData.Clear(); // We behave like Begin() and need to consume those values
        return false;
    }

    // Center modal windows by default
    // FIXME: Should test for (PosCond & window->SetWindowPosAllowFlags) with the upcoming window.
    if (g.NextWindowData.PosCond == 0)
        SetNextWindowPos(g.IO.DisplaySize * 0.5f, vsonyp0werCond_Appearing, ImVec2(0.5f, 0.5f));

    flags |= vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_Modal | vsonyp0werWindowFlags_NoCollapse | vsonyp0werWindowFlags_NoSavedSettings;
    const bool is_open = Begin(name, p_open, flags);
    if (!is_open || (p_open && !*p_open)) // NB: is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
    {
        EndPopup();
        if (is_open)
            ClosePopupToLevel(g.BeginPopupStack.Size, true);
        return false;
    }
    return is_open;
}

void vsonyp0wer::EndPopup()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.CurrentWindow->Flags & vsonyp0werWindowFlags_Popup);  // Mismatched BeginPopup()/EndPopup() calls
    IM_ASSERT(g.BeginPopupStack.Size > 0);

    // Make all menus and popups wrap around for now, may need to expose that policy.
    NavMoveRequestTryWrapping(g.CurrentWindow, vsonyp0werNavMoveFlags_LoopY);

    End();
}

// This is a helper to handle the simplest case of associating one named popup to one given widget.
// You may want to handle this on user side if you have specific needs (e.g. tweaking IsItemHovered() parameters).
// You can pass a NULL str_id to use the identifier of the last item.
bool vsonyp0wer::BeginPopupContextItem(const char* str_id, int mouse_button)
{
    vsonyp0werWindow* window = Gvsonyp0wer->CurrentWindow;
    vsonyp0werID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
    IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
    if (IsMouseReleased(mouse_button) && IsItemHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup))
        OpenPopupEx(id);
    return BeginPopupEx(id, vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings);
}

bool vsonyp0wer::BeginPopupContextWindow(const char* str_id, int mouse_button, bool also_over_items)
{
    if (!str_id)
        str_id = "window_context";
    vsonyp0werID id = Gvsonyp0wer->CurrentWindow->GetID(str_id);
    if (IsMouseReleased(mouse_button) && IsWindowHovered(vsonyp0werHoveredFlags_AllowWhenBlockedByPopup))
        if (also_over_items || !IsAnyItemHovered())
            OpenPopupEx(id);
    return BeginPopupEx(id, vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings);
}

bool vsonyp0wer::BeginPopupContextVoid(const char* str_id, int mouse_button)
{
    if (!str_id)
        str_id = "void_context";
    vsonyp0werID id = Gvsonyp0wer->CurrentWindow->GetID(str_id);
    if (IsMouseReleased(mouse_button) && !IsWindowHovered(vsonyp0werHoveredFlags_AnyWindow))
        OpenPopupEx(id);
    return BeginPopupEx(id, vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoSavedSettings);
}

// r_avoid = the rectangle to avoid (e.g. for tooltip it is a rectangle around the mouse cursor which we want to avoid. for popups it's a small point around the cursor.)
// r_outer = the visible area rectangle, minus safe area padding. If our popup size won't fit because of safe area padding we ignore it.
ImVec2 vsonyp0wer::FindBestWindowPosForPopupEx(const ImVec2 & ref_pos, const ImVec2 & size, vsonyp0werDir * last_dir, const ImRect & r_outer, const ImRect & r_avoid, vsonyp0werPopupPositionPolicy policy)
{
    ImVec2 base_pos_clamped = ImClamp(ref_pos, r_outer.Min, r_outer.Max - size);
    //GetForegroundDrawList()->AddRect(r_avoid.Min, r_avoid.Max, IM_COL32(255,0,0,255));
    //GetForegroundDrawList()->AddRect(r_outer.Min, r_outer.Max, IM_COL32(0,255,0,255));

    // Combo Box policy (we want a connecting edge)
    if (policy == vsonyp0werPopupPositionPolicy_ComboBox)
    {
        const vsonyp0werDir dir_prefered_order[vsonyp0werDir_COUNT] = { vsonyp0werDir_Down, vsonyp0werDir_Right, vsonyp0werDir_Left, vsonyp0werDir_Up };
        for (int n = (*last_dir != vsonyp0werDir_None) ? -1 : 0; n < vsonyp0werDir_COUNT; n++)
        {
            const vsonyp0werDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
            if (n != -1 && dir == *last_dir) // Already tried this direction?
                continue;
            ImVec2 pos;
            if (dir == vsonyp0werDir_Down)  pos = ImVec2(r_avoid.Min.x, r_avoid.Max.y);          // Below, Toward Right (default)
            if (dir == vsonyp0werDir_Right) pos = ImVec2(r_avoid.Min.x, r_avoid.Min.y - size.y); // Above, Toward Right
            if (dir == vsonyp0werDir_Left)  pos = ImVec2(r_avoid.Max.x - size.x, r_avoid.Max.y); // Below, Toward Left
            if (dir == vsonyp0werDir_Up)    pos = ImVec2(r_avoid.Max.x - size.x, r_avoid.Min.y - size.y); // Above, Toward Left
            if (!r_outer.Contains(ImRect(pos, pos + size)))
                continue;
            *last_dir = dir;
            return pos;
        }
    }

    // Default popup policy
    const vsonyp0werDir dir_prefered_order[vsonyp0werDir_COUNT] = { vsonyp0werDir_Right, vsonyp0werDir_Down, vsonyp0werDir_Up, vsonyp0werDir_Left };
    for (int n = (*last_dir != vsonyp0werDir_None) ? -1 : 0; n < vsonyp0werDir_COUNT; n++)
    {
        const vsonyp0werDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
        if (n != -1 && dir == *last_dir) // Already tried this direction?
            continue;
        float avail_w = (dir == vsonyp0werDir_Left ? r_avoid.Min.x : r_outer.Max.x) - (dir == vsonyp0werDir_Right ? r_avoid.Max.x : r_outer.Min.x);
        float avail_h = (dir == vsonyp0werDir_Up ? r_avoid.Min.y : r_outer.Max.y) - (dir == vsonyp0werDir_Down ? r_avoid.Max.y : r_outer.Min.y);
        if (avail_w < size.x || avail_h < size.y)
            continue;
        ImVec2 pos;
        pos.x = (dir == vsonyp0werDir_Left) ? r_avoid.Min.x - size.x : (dir == vsonyp0werDir_Right) ? r_avoid.Max.x : base_pos_clamped.x;
        pos.y = (dir == vsonyp0werDir_Up) ? r_avoid.Min.y - size.y : (dir == vsonyp0werDir_Down) ? r_avoid.Max.y : base_pos_clamped.y;
        *last_dir = dir;
        return pos;
    }

    // Fallback, try to keep within display
    *last_dir = vsonyp0werDir_None;
    ImVec2 pos = ref_pos;
    pos.x = ImMax(ImMin(pos.x + size.x, r_outer.Max.x) - size.x, r_outer.Min.x);
    pos.y = ImMax(ImMin(pos.y + size.y, r_outer.Max.y) - size.y, r_outer.Min.y);
    return pos;
}

ImRect vsonyp0wer::GetWindowAllowedExtentRect(vsonyp0werWindow * window)
{
    IM_UNUSED(window);
    ImVec2 padding = Gvsonyp0wer->Style.DisplaySafeAreaPadding;
    ImRect r_screen = GetViewportRect();
    r_screen.Expand(ImVec2((r_screen.GetWidth() > padding.x * 2) ? -padding.x : 0.0f, (r_screen.GetHeight() > padding.y * 2) ? -padding.y : 0.0f));
    return r_screen;
}

ImVec2 vsonyp0wer::FindBestWindowPosForPopup(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    ImRect r_outer = GetWindowAllowedExtentRect(window);
    if (window->Flags & vsonyp0werWindowFlags_ChildMenu)
    {
        // Child menus typically request _any_ position within the parent menu item, and then we move the new menu outside the parent bounds.
        // This is how we end up with child menus appearing (most-commonly) on the right of the parent menu.
        IM_ASSERT(g.CurrentWindow == window);
        vsonyp0werWindow * parent_window = g.CurrentWindowStack[g.CurrentWindowStack.Size - 2];
        float horizontal_overlap = g.Style.ItemInnerSpacing.x; // We want some overlap to convey the relative depth of each menu (currently the amount of overlap is hard-coded to style.ItemSpacing.x).
        ImRect r_avoid;
        if (parent_window->DC.MenuBarAppending)
            r_avoid = ImRect(-FLT_MAX, parent_window->Pos.y + parent_window->TitleBarHeight(), FLT_MAX, parent_window->Pos.y + parent_window->TitleBarHeight() + parent_window->MenuBarHeight());
        else
            r_avoid = ImRect(parent_window->Pos.x + horizontal_overlap, -FLT_MAX, parent_window->Pos.x + parent_window->Size.x - horizontal_overlap - parent_window->ScrollbarSizes.x, FLT_MAX);
        return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
    }
    if (window->Flags & vsonyp0werWindowFlags_Popup)
    {
        ImRect r_avoid = ImRect(window->Pos.x - 1, window->Pos.y - 1, window->Pos.x + 1, window->Pos.y + 1);
        return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
    }
    if (window->Flags & vsonyp0werWindowFlags_Tooltip)
    {
        // Position tooltip (always follows mouse)
        float sc = g.Style.MouseCursorScale;
        ImVec2 ref_pos = NavCalcPreferredRefPos();
        ImRect r_avoid;
        if (!g.NavDisableHighlight && g.NavDisableMouseHover && !(g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableSetMousePos))
            r_avoid = ImRect(ref_pos.x - 16, ref_pos.y - 8, ref_pos.x + 16, ref_pos.y + 8);
        else
            r_avoid = ImRect(ref_pos.x - 16, ref_pos.y - 8, ref_pos.x + 24 * sc, ref_pos.y + 24 * sc); // FIXME: Hard-coded based on mouse cursor shape expectation. Exact dimension not very important.
        ImVec2 pos = FindBestWindowPosForPopupEx(ref_pos, window->Size, &window->AutoPosLastDirection, r_outer, r_avoid);
        if (window->AutoPosLastDirection == vsonyp0werDir_None)
            pos = ref_pos + ImVec2(2, 2); // If there's not enough room, for tooltip we prefer avoiding the cursor at all cost even if it means that part of the tooltip won't be visible.
        return pos;
    }
    IM_ASSERT(0);
    return window->Pos;
}


//-----------------------------------------------------------------------------
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
//-----------------------------------------------------------------------------

vsonyp0werDir ImGetDirQuadrantFromDelta(float dx, float dy)
{
    if (ImFabs(dx) > ImFabs(dy))
        return (dx > 0.0f) ? vsonyp0werDir_Right : vsonyp0werDir_Left;
    return (dy > 0.0f) ? vsonyp0werDir_Down : vsonyp0werDir_Up;
}

static float inline NavScoreItemDistInterval(float a0, float a1, float b0, float b1)
{
    if (a1 < b0)
        return a1 - b0;
    if (b1 < a0)
        return a0 - b1;
    return 0.0f;
}

static void inline NavClampRectToVisibleAreaForMoveDir(vsonyp0werDir move_dir, ImRect & r, const ImRect & clip_rect)
{
    if (move_dir == vsonyp0werDir_Left || move_dir == vsonyp0werDir_Right)
    {
        r.Min.y = ImClamp(r.Min.y, clip_rect.Min.y, clip_rect.Max.y);
        r.Max.y = ImClamp(r.Max.y, clip_rect.Min.y, clip_rect.Max.y);
    } else
    {
        r.Min.x = ImClamp(r.Min.x, clip_rect.Min.x, clip_rect.Max.x);
        r.Max.x = ImClamp(r.Max.x, clip_rect.Min.x, clip_rect.Max.x);
    }
}

// Scoring function for directional navigation. Based on https://gist.github.com/rygorous/6981057
static bool NavScoreItem(vsonyp0werNavMoveResult * result, ImRect cand)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    if (g.NavLayer != window->DC.NavLayerCurrent)
        return false;

    const ImRect & curr = g.NavScoringRectScreen; // Current modified source rect (NB: we've applied Max.x = Min.x in NavUpdate() to inhibit the effect of having varied item width)
    g.NavScoringCount++;

    // When entering through a NavFlattened border, we consider child window items as fully clipped for scoring
    if (window->ParentWindow == g.NavWindow)
    {
        IM_ASSERT((window->Flags | g.NavWindow->Flags) & vsonyp0werWindowFlags_NavFlattened);
        if (!window->ClipRect.Contains(cand))
            return false;
        cand.ClipWithFull(window->ClipRect); // This allows the scored item to not overlap other candidates in the parent window
    }

    // We perform scoring on items bounding box clipped by the current clipping rectangle on the other axis (clipping on our movement axis would give us equal scores for all clipped items)
    // For example, this ensure that items in one column are not reached when moving vertically from items in another column.
    NavClampRectToVisibleAreaForMoveDir(g.NavMoveClipDir, cand, window->ClipRect);

    // Compute distance between boxes
    // FIXME-NAV: Introducing biases for vertical navigation, needs to be removed.
    float dbx = NavScoreItemDistInterval(cand.Min.x, cand.Max.x, curr.Min.x, curr.Max.x);
    float dby = NavScoreItemDistInterval(ImLerp(cand.Min.y, cand.Max.y, 0.2f), ImLerp(cand.Min.y, cand.Max.y, 0.8f), ImLerp(curr.Min.y, curr.Max.y, 0.2f), ImLerp(curr.Min.y, curr.Max.y, 0.8f)); // Scale down on Y to keep using box-distance for vertically touching items
    if (dby != 0.0f && dbx != 0.0f)
        dbx = (dbx / 1000.0f) + ((dbx > 0.0f) ? +1.0f : -1.0f);
    float dist_box = ImFabs(dbx) + ImFabs(dby);

    // Compute distance between centers (this is off by a factor of 2, but we only compare center distances with each other so it doesn't matter)
    float dcx = (cand.Min.x + cand.Max.x) - (curr.Min.x + curr.Max.x);
    float dcy = (cand.Min.y + cand.Max.y) - (curr.Min.y + curr.Max.y);
    float dist_center = ImFabs(dcx) + ImFabs(dcy); // L1 metric (need this for our connectedness guarantee)

    // Determine which quadrant of 'curr' our candidate item 'cand' lies in based on distance
    vsonyp0werDir quadrant;
    float dax = 0.0f, day = 0.0f, dist_axial = 0.0f;
    if (dbx != 0.0f || dby != 0.0f)
    {
        // For non-overlapping boxes, use distance between boxes
        dax = dbx;
        day = dby;
        dist_axial = dist_box;
        quadrant = ImGetDirQuadrantFromDelta(dbx, dby);
    } else if (dcx != 0.0f || dcy != 0.0f)
    {
        // For overlapping boxes with different centers, use distance between centers
        dax = dcx;
        day = dcy;
        dist_axial = dist_center;
        quadrant = ImGetDirQuadrantFromDelta(dcx, dcy);
    } else
    {
        // Degenerate case: two overlapping buttons with same center, break ties arbitrarily (note that LastItemId here is really the _previous_ item order, but it doesn't matter)
        quadrant = (window->DC.LastItemId < g.NavId) ? vsonyp0werDir_Left : vsonyp0werDir_Right;
    }

#if vsonyp0wer_DEBUG_NAV_SCORING
    char buf[128];
    if (vsonyp0wer::IsMouseHoveringRect(cand.Min, cand.Max))
    {
        ImFormatString(buf, IM_ARRAYSIZE(buf), "dbox (%.2f,%.2f->%.4f)\ndcen (%.2f,%.2f->%.4f)\nd (%.2f,%.2f->%.4f)\nnav %c, quadrant %c", dbx, dby, dist_box, dcx, dcy, dist_center, dax, day, dist_axial, "WENS"[g.NavMoveDir], "WENS"[quadrant]);
        ImDrawList* draw_list = vsonyp0wer::GetForegroundDrawList(window);
        draw_list->AddRect(curr.Min, curr.Max, IM_COL32(255, 200, 0, 100));
        draw_list->AddRect(cand.Min, cand.Max, IM_COL32(255, 255, 0, 200));
        draw_list->AddRectFilled(cand.Max - ImVec2(4, 4), cand.Max + vsonyp0wer::CalcTextSize(buf) + ImVec2(4, 4), IM_COL32(40, 0, 0, 150));
        draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Max, ~0U, buf);
    } else if (g.IO.KeyCtrl) // Hold to preview score in matching quadrant. Press C to rotate.
    {
        if (vsonyp0wer::IsKeyPressedMap(vsonyp0werKey_C)) { g.NavMoveDirLast = (vsonyp0werDir)((g.NavMoveDirLast + 1) & 3); g.IO.KeysDownDuration[g.IO.KeyMap[vsonyp0werKey_C]] = 0.01f; }
        if (quadrant == g.NavMoveDir)
        {
            ImFormatString(buf, IM_ARRAYSIZE(buf), "%.0f/%.0f", dist_box, dist_center);
            ImDrawList* draw_list = vsonyp0wer::GetForegroundDrawList(window);
            draw_list->AddRectFilled(cand.Min, cand.Max, IM_COL32(255, 0, 0, 200));
            draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Min, IM_COL32(255, 255, 255, 255), buf);
        }
    }
#endif

    // Is it in the quadrant we're interesting in moving to?
    bool new_best = false;
    if (quadrant == g.NavMoveDir)
    {
        // Does it beat the current best candidate?
        if (dist_box < result->DistBox)
        {
            result->DistBox = dist_box;
            result->DistCenter = dist_center;
            return true;
        }
        if (dist_box == result->DistBox)
        {
            // Try using distance between center points to break ties
            if (dist_center < result->DistCenter)
            {
                result->DistCenter = dist_center;
                new_best = true;
            } else if (dist_center == result->DistCenter)
            {
                // Still tied! we need to be extra-careful to make sure everything gets linked properly. We consistently break ties by symbolically moving "later" items
                // (with higher index) to the right/downwards by an infinitesimal amount since we the current "best" button already (so it must have a lower index),
                // this is fairly easy. This rule ensures that all buttons with dx==dy==0 will end up being linked in order of appearance along the x axis.
                if (((g.NavMoveDir == vsonyp0werDir_Up || g.NavMoveDir == vsonyp0werDir_Down) ? dby : dbx) < 0.0f) // moving bj to the right/down decreases distance
                    new_best = true;
            }
        }
    }

    // Axial check: if 'curr' has no link at all in some direction and 'cand' lies roughly in that direction, add a tentative link. This will only be kept if no "real" matches
    // are found, so it only augments the graph produced by the above method using extra links. (important, since it doesn't guarantee strong connectedness)
    // This is just to avoid buttons having no links in a particular direction when there's a suitable neighbor. you get good graphs without this too.
    // 2017/09/29: FIXME: This now currently only enabled inside menu bars, ideally we'd disable it everywhere. Menus in particular need to catch failure. For general navigation it feels awkward.
    // Disabling it may lead to disconnected graphs when nodes are very spaced out on different axis. Perhaps consider offering this as an option?
    if (result->DistBox == FLT_MAX && dist_axial < result->DistAxial)  // Check axial match
        if (g.NavLayer == 1 && !(g.NavWindow->Flags & vsonyp0werWindowFlags_ChildMenu))
            if ((g.NavMoveDir == vsonyp0werDir_Left && dax < 0.0f) || (g.NavMoveDir == vsonyp0werDir_Right && dax > 0.0f) || (g.NavMoveDir == vsonyp0werDir_Up && day < 0.0f) || (g.NavMoveDir == vsonyp0werDir_Down && day > 0.0f))
            {
                result->DistAxial = dist_axial;
                new_best = true;
            }

    return new_best;
}

// We get there when either NavId == id, or when g.NavAnyRequest is set (which is updated by NavUpdateAnyRequestFlag above)
static void vsonyp0wer::NavProcessItem(vsonyp0werWindow * window, const ImRect & nav_bb, const vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    //if (!g.IO.NavActive)  // [2017/10/06] Removed this possibly redundant test but I am not sure of all the side-effects yet. Some of the feature here will need to work regardless of using a _NoNavInputs flag.
    //    return;

    const vsonyp0werItemFlags item_flags = window->DC.ItemFlags;
    const ImRect nav_bb_rel(nav_bb.Min - window->Pos, nav_bb.Max - window->Pos);

    // Process Init Request
    if (g.NavInitRequest && g.NavLayer == window->DC.NavLayerCurrent)
    {
        // Even if 'vsonyp0werItemFlags_NoNavDefaultFocus' is on (typically collapse/close button) we record the first ResultId so they can be used as a fallback
        if (!(item_flags & vsonyp0werItemFlags_NoNavDefaultFocus) || g.NavInitResultId == 0)
        {
            g.NavInitResultId = id;
            g.NavInitResultRectRel = nav_bb_rel;
        }
        if (!(item_flags & vsonyp0werItemFlags_NoNavDefaultFocus))
        {
            g.NavInitRequest = false; // Found a match, clear request
            NavUpdateAnyRequestFlag();
        }
    }

    // Process Move Request (scoring for navigation)
    // FIXME-NAV: Consider policy for double scoring (scoring from NavScoringRectScreen + scoring from a rect wrapped according to current wrapping policy)
    if ((g.NavId != id || (g.NavMoveRequestFlags & vsonyp0werNavMoveFlags_AllowCurrentNavId)) && !(item_flags & (vsonyp0werItemFlags_Disabled | vsonyp0werItemFlags_NoNav)))
    {
        vsonyp0werNavMoveResult* result = (window == g.NavWindow) ? &g.NavMoveResultLocal : &g.NavMoveResultOther;
#if vsonyp0wer_DEBUG_NAV_SCORING
        // [DEBUG] Score all items in NavWindow at all times
        if (!g.NavMoveRequest)
            g.NavMoveDir = g.NavMoveDirLast;
        bool new_best = NavScoreItem(result, nav_bb) && g.NavMoveRequest;
#else
        bool new_best = g.NavMoveRequest && NavScoreItem(result, nav_bb);
#endif
        if (new_best)
        {
            result->ID = id;
            result->SelectScopeId = g.MultiSelectScopeId;
            result->Window = window;
            result->RectRel = nav_bb_rel;
        }

        const float VISIBLE_RATIO = 0.70f;
        if ((g.NavMoveRequestFlags & vsonyp0werNavMoveFlags_AlsoScoreVisibleSet) && window->ClipRect.Overlaps(nav_bb))
            if (ImClamp(nav_bb.Max.y, window->ClipRect.Min.y, window->ClipRect.Max.y) - ImClamp(nav_bb.Min.y, window->ClipRect.Min.y, window->ClipRect.Max.y) >= (nav_bb.Max.y - nav_bb.Min.y) * VISIBLE_RATIO)
                if (NavScoreItem(&g.NavMoveResultLocalVisibleSet, nav_bb))
                {
                    result = &g.NavMoveResultLocalVisibleSet;
                    result->ID = id;
                    result->SelectScopeId = g.MultiSelectScopeId;
                    result->Window = window;
                    result->RectRel = nav_bb_rel;
                }
    }

    // Update window-relative bounding box of navigated item
    if (g.NavId == id)
    {
        g.NavWindow = window;                                           // Always refresh g.NavWindow, because some operations such as FocusItem() don't have a window.
        g.NavLayer = window->DC.NavLayerCurrent;
        g.NavIdIsAlive = true;
        g.NavIdTabCounter = window->DC.FocusCounterTab;
        window->NavRectRel[window->DC.NavLayerCurrent] = nav_bb_rel;    // Store item bounding box (relative to window position)
    }
}

bool vsonyp0wer::NavMoveRequestButNoResultYet()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.NavMoveRequest&& g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0;
}

void vsonyp0wer::NavMoveRequestCancel()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NavMoveRequest = false;
    NavUpdateAnyRequestFlag();
}

void vsonyp0wer::NavMoveRequestForward(vsonyp0werDir move_dir, vsonyp0werDir clip_dir, const ImRect & bb_rel, vsonyp0werNavMoveFlags move_flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.NavMoveRequestForward == vsonyp0werNavForward_None);
    vsonyp0wer::NavMoveRequestCancel();
    g.NavMoveDir = move_dir;
    g.NavMoveClipDir = clip_dir;
    g.NavMoveRequestForward = vsonyp0werNavForward_ForwardQueued;
    g.NavMoveRequestFlags = move_flags;
    g.NavWindow->NavRectRel[g.NavLayer] = bb_rel;
}

void vsonyp0wer::NavMoveRequestTryWrapping(vsonyp0werWindow * window, vsonyp0werNavMoveFlags move_flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NavWindow != window || !NavMoveRequestButNoResultYet() || g.NavMoveRequestForward != vsonyp0werNavForward_None || g.NavLayer != 0)
        return;
    IM_ASSERT(move_flags != 0); // No points calling this with no wrapping
    ImRect bb_rel = window->NavRectRel[0];

    vsonyp0werDir clip_dir = g.NavMoveDir;
    if (g.NavMoveDir == vsonyp0werDir_Left && (move_flags & (vsonyp0werNavMoveFlags_WrapX | vsonyp0werNavMoveFlags_LoopX)))
    {
        bb_rel.Min.x = bb_rel.Max.x = ImMax(window->SizeFull.x, window->SizeContents.x) - window->Scroll.x;
        if (move_flags & vsonyp0werNavMoveFlags_WrapX) { bb_rel.TranslateY(-bb_rel.GetHeight()); clip_dir = vsonyp0werDir_Up; }
        NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == vsonyp0werDir_Right && (move_flags & (vsonyp0werNavMoveFlags_WrapX | vsonyp0werNavMoveFlags_LoopX)))
    {
        bb_rel.Min.x = bb_rel.Max.x = -window->Scroll.x;
        if (move_flags & vsonyp0werNavMoveFlags_WrapX) { bb_rel.TranslateY(+bb_rel.GetHeight()); clip_dir = vsonyp0werDir_Down; }
        NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == vsonyp0werDir_Up && (move_flags & (vsonyp0werNavMoveFlags_WrapY | vsonyp0werNavMoveFlags_LoopY)))
    {
        bb_rel.Min.y = bb_rel.Max.y = ImMax(window->SizeFull.y, window->SizeContents.y) - window->Scroll.y;
        if (move_flags & vsonyp0werNavMoveFlags_WrapY) { bb_rel.TranslateX(-bb_rel.GetWidth()); clip_dir = vsonyp0werDir_Left; }
        NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == vsonyp0werDir_Down && (move_flags & (vsonyp0werNavMoveFlags_WrapY | vsonyp0werNavMoveFlags_LoopY)))
    {
        bb_rel.Min.y = bb_rel.Max.y = -window->Scroll.y;
        if (move_flags & vsonyp0werNavMoveFlags_WrapY) { bb_rel.TranslateX(+bb_rel.GetWidth()); clip_dir = vsonyp0werDir_Right; }
        NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
}

// FIXME: This could be replaced by updating a frame number in each window when (window == NavWindow) and (NavLayer == 0).
// This way we could find the last focused window among our children. It would be much less confusing this way?
static void vsonyp0wer::NavSaveLastChildNavWindowIntoParent(vsonyp0werWindow * nav_window)
{
    vsonyp0werWindow* parent_window = nav_window;
    while (parent_window && (parent_window->Flags & vsonyp0werWindowFlags_ChildWindow) != 0 && (parent_window->Flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_ChildMenu)) == 0)
        parent_window = parent_window->ParentWindow;
    if (parent_window && parent_window != nav_window)
        parent_window->NavLastChildNavWindow = nav_window;
}

// Restore the last focused child.
// Call when we are expected to land on the Main Layer (0) after FocusWindow()
static vsonyp0werWindow * vsonyp0wer::NavRestoreLastChildNavWindow(vsonyp0werWindow * window)
{
    return window->NavLastChildNavWindow ? window->NavLastChildNavWindow : window;
}

static void NavRestoreLayer(vsonyp0werNavLayer layer)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NavLayer = layer;
    if (layer == 0)
        g.NavWindow = vsonyp0wer::NavRestoreLastChildNavWindow(g.NavWindow);
    if (layer == 0 && g.NavWindow->NavLastIds[0] != 0)
        vsonyp0wer::SetNavIDWithRectRel(g.NavWindow->NavLastIds[0], layer, g.NavWindow->NavRectRel[0]);
    else
        vsonyp0wer::NavInitWindow(g.NavWindow, true);
}

static inline void vsonyp0wer::NavUpdateAnyRequestFlag()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.NavAnyRequest = g.NavMoveRequest || g.NavInitRequest || (vsonyp0wer_DEBUG_NAV_SCORING && g.NavWindow != NULL);
    if (g.NavAnyRequest)
        IM_ASSERT(g.NavWindow != NULL);
}

// This needs to be called before we submit any widget (aka in or before Begin)
void vsonyp0wer::NavInitWindow(vsonyp0werWindow * window, bool force_reinit)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(window == g.NavWindow);
    bool init_for_nav = false;
    if (!(window->Flags & vsonyp0werWindowFlags_NoNavInputs))
        if (!(window->Flags & vsonyp0werWindowFlags_ChildWindow) || (window->Flags & vsonyp0werWindowFlags_Popup) || (window->NavLastIds[0] == 0) || force_reinit)
            init_for_nav = true;
    if (init_for_nav)
    {
        SetNavID(0, g.NavLayer);
        g.NavInitRequest = true;
        g.NavInitRequestFromMove = false;
        g.NavInitResultId = 0;
        g.NavInitResultRectRel = ImRect();
        NavUpdateAnyRequestFlag();
    } else
    {
        g.NavId = window->NavLastIds[0];
    }
}

static ImVec2 vsonyp0wer::NavCalcPreferredRefPos()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NavDisableHighlight || !g.NavDisableMouseHover || !g.NavWindow)
    {
        // Mouse (we need a fallback in case the mouse becomes invalid after being used)
        if (IsMousePosValid(&g.IO.MousePos))
            return g.IO.MousePos;
        return g.LastValidMousePos;
    } else
    {
        // When navigation is active and mouse is disabled, decide on an arbitrary position around the bottom left of the currently navigated item.
        const ImRect& rect_rel = g.NavWindow->NavRectRel[g.NavLayer];
        ImVec2 pos = g.NavWindow->Pos + ImVec2(rect_rel.Min.x + ImMin(g.Style.FramePadding.x * 4, rect_rel.GetWidth()), rect_rel.Max.y - ImMin(g.Style.FramePadding.y, rect_rel.GetHeight()));
        ImRect visible_rect = GetViewportRect();
        return ImFloor(ImClamp(pos, visible_rect.Min, visible_rect.Max));   // ImFloor() is important because non-integer mouse position application in back-end might be lossy and result in undesirable non-zero delta.
    }
}

float vsonyp0wer::GetNavInputAmount(vsonyp0werNavInput n, vsonyp0werInputReadMode mode)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (mode == vsonyp0werInputReadMode_Down)
        return g.IO.NavInputs[n];                         // Instant, read analog input (0.0f..1.0f, as provided by user)

    const float t = g.IO.NavInputsDownDuration[n];
    if (t < 0.0f && mode == vsonyp0werInputReadMode_Released)  // Return 1.0f when just released, no repeat, ignore analog input.
        return (g.IO.NavInputsDownDurationPrev[n] >= 0.0f ? 1.0f : 0.0f);
    if (t < 0.0f)
        return 0.0f;
    if (mode == vsonyp0werInputReadMode_Pressed)               // Return 1.0f when just pressed, no repeat, ignore analog input.
        return (t == 0.0f) ? 1.0f : 0.0f;
    if (mode == vsonyp0werInputReadMode_Repeat)
        return (float)CalcTypematicPressedRepeatAmount(t, t - g.IO.DeltaTime, g.IO.KeyRepeatDelay * 0.80f, g.IO.KeyRepeatRate * 0.80f);
    if (mode == vsonyp0werInputReadMode_RepeatSlow)
        return (float)CalcTypematicPressedRepeatAmount(t, t - g.IO.DeltaTime, g.IO.KeyRepeatDelay * 1.00f, g.IO.KeyRepeatRate * 2.00f);
    if (mode == vsonyp0werInputReadMode_RepeatFast)
        return (float)CalcTypematicPressedRepeatAmount(t, t - g.IO.DeltaTime, g.IO.KeyRepeatDelay * 0.80f, g.IO.KeyRepeatRate * 0.30f);
    return 0.0f;
}

ImVec2 vsonyp0wer::GetNavInputAmount2d(vsonyp0werNavDirSourceFlags dir_sources, vsonyp0werInputReadMode mode, float slow_factor, float fast_factor)
{
    ImVec2 delta(0.0f, 0.0f);
    if (dir_sources & vsonyp0werNavDirSourceFlags_Keyboard)
        delta += ImVec2(GetNavInputAmount(vsonyp0werNavInput_KeyRight_, mode) - GetNavInputAmount(vsonyp0werNavInput_KeyLeft_, mode), GetNavInputAmount(vsonyp0werNavInput_KeyDown_, mode) - GetNavInputAmount(vsonyp0werNavInput_KeyUp_, mode));
    if (dir_sources & vsonyp0werNavDirSourceFlags_PadDPad)
        delta += ImVec2(GetNavInputAmount(vsonyp0werNavInput_DpadRight, mode) - GetNavInputAmount(vsonyp0werNavInput_DpadLeft, mode), GetNavInputAmount(vsonyp0werNavInput_DpadDown, mode) - GetNavInputAmount(vsonyp0werNavInput_DpadUp, mode));
    if (dir_sources & vsonyp0werNavDirSourceFlags_PadLStick)
        delta += ImVec2(GetNavInputAmount(vsonyp0werNavInput_LStickRight, mode) - GetNavInputAmount(vsonyp0werNavInput_LStickLeft, mode), GetNavInputAmount(vsonyp0werNavInput_LStickDown, mode) - GetNavInputAmount(vsonyp0werNavInput_LStickUp, mode));
    if (slow_factor != 0.0f && IsNavInputDown(vsonyp0werNavInput_TweakSlow))
        delta *= slow_factor;
    if (fast_factor != 0.0f && IsNavInputDown(vsonyp0werNavInput_TweakFast))
        delta *= fast_factor;
    return delta;
}

// Scroll to keep newly navigated item fully into view
// NB: We modify rect_rel by the amount we scrolled for, so it is immediately updated.
static void NavScrollToBringItemIntoView(vsonyp0werWindow * window, const ImRect & item_rect)
{
    ImRect window_rect(window->InnerMainRect.Min - ImVec2(1, 1), window->InnerMainRect.Max + ImVec2(1, 1));
    //GetForegroundDrawList(window)->AddRect(window_rect.Min, window_rect.Max, IM_COL32_WHITE); // [DEBUG]
    if (window_rect.Contains(item_rect))
        return;

    vsonyp0werContext & g = *Gvsonyp0wer;
    if (window->ScrollbarX && item_rect.Min.x < window_rect.Min.x)
    {
        window->ScrollTarget.x = item_rect.Min.x - window->Pos.x + window->Scroll.x - g.Style.ItemSpacing.x;
        window->ScrollTargetCenterRatio.x = 0.0f;
    } else if (window->ScrollbarX && item_rect.Max.x >= window_rect.Max.x)
    {
        window->ScrollTarget.x = item_rect.Max.x - window->Pos.x + window->Scroll.x + g.Style.ItemSpacing.x;
        window->ScrollTargetCenterRatio.x = 1.0f;
    }
    if (item_rect.Min.y < window_rect.Min.y)
    {
        window->ScrollTarget.y = item_rect.Min.y - window->Pos.y + window->Scroll.y - g.Style.ItemSpacing.y;
        window->ScrollTargetCenterRatio.y = 0.0f;
    } else if (item_rect.Max.y >= window_rect.Max.y)
    {
        window->ScrollTarget.y = item_rect.Max.y - window->Pos.y + window->Scroll.y + g.Style.ItemSpacing.y;
        window->ScrollTargetCenterRatio.y = 1.0f;
    }
}

static void vsonyp0wer::NavUpdate()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.IO.WantSetMousePos = false;
#if 0
    if (g.NavScoringCount > 0) vsonyp0wer_DEBUG_LOG("NavScoringCount %d for '%s' layer %d (Init:%d, Move:%d)\n", g.FrameCount, g.NavScoringCount, g.NavWindow ? g.NavWindow->Name : "NULL", g.NavLayer, g.NavInitRequest || g.NavInitResultId != 0, g.NavMoveRequest);
#endif

    // Set input source as Gamepad when buttons are pressed before we map Keyboard (some features differs when used with Gamepad vs Keyboard)
    bool nav_keyboard_active = (g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableKeyboard) != 0;
    bool nav_gamepad_active = (g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableGamepad) != 0 && (g.IO.BackendFlags & vsonyp0werBackendFlags_HasGamepad) != 0;
    if (nav_gamepad_active)
        if (g.IO.NavInputs[vsonyp0werNavInput_Activate] > 0.0f || g.IO.NavInputs[vsonyp0werNavInput_Input] > 0.0f || g.IO.NavInputs[vsonyp0werNavInput_Cancel] > 0.0f || g.IO.NavInputs[vsonyp0werNavInput_Menu] > 0.0f)
            g.NavInputSource = vsonyp0werInputSource_NavGamepad;

    // Update Keyboard->Nav inputs mapping
    if (nav_keyboard_active)
    {
#define NAV_MAP_KEY(_KEY, _NAV_INPUT) if (IsKeyDown(g.IO.KeyMap[_KEY])) { g.IO.NavInputs[_NAV_INPUT] = 1.0f; g.NavInputSource = vsonyp0werInputSource_NavKeyboard; }
        NAV_MAP_KEY(vsonyp0werKey_Space, vsonyp0werNavInput_Activate);
        NAV_MAP_KEY(vsonyp0werKey_Enter, vsonyp0werNavInput_Input);
        NAV_MAP_KEY(vsonyp0werKey_Escape, vsonyp0werNavInput_Cancel);
        NAV_MAP_KEY(vsonyp0werKey_LeftArrow, vsonyp0werNavInput_KeyLeft_);
        NAV_MAP_KEY(vsonyp0werKey_RightArrow, vsonyp0werNavInput_KeyRight_);
        NAV_MAP_KEY(vsonyp0werKey_UpArrow, vsonyp0werNavInput_KeyUp_);
        NAV_MAP_KEY(vsonyp0werKey_DownArrow, vsonyp0werNavInput_KeyDown_);
        NAV_MAP_KEY(vsonyp0werKey_Tab, vsonyp0werNavInput_KeyTab_);
        if (g.IO.KeyCtrl)
            g.IO.NavInputs[vsonyp0werNavInput_TweakSlow] = 1.0f;
        if (g.IO.KeyShift)
            g.IO.NavInputs[vsonyp0werNavInput_TweakFast] = 1.0f;
        if (g.IO.KeyAlt && !g.IO.KeyCtrl) // AltGR is Alt+Ctrl, also even on keyboards without AltGR we don't want Alt+Ctrl to open menu.
            g.IO.NavInputs[vsonyp0werNavInput_KeyMenu_] = 1.0f;
#undef NAV_MAP_KEY
    }
    memcpy(g.IO.NavInputsDownDurationPrev, g.IO.NavInputsDownDuration, sizeof(g.IO.NavInputsDownDuration));
    for (int i = 0; i < IM_ARRAYSIZE(g.IO.NavInputs); i++)
        g.IO.NavInputsDownDuration[i] = (g.IO.NavInputs[i] > 0.0f) ? (g.IO.NavInputsDownDuration[i] < 0.0f ? 0.0f : g.IO.NavInputsDownDuration[i] + g.IO.DeltaTime) : -1.0f;

    // Process navigation init request (select first/default focus)
    if (g.NavInitResultId != 0 && (!g.NavDisableHighlight || g.NavInitRequestFromMove))
    {
        // Apply result from previous navigation init request (will typically select the first item, unless SetItemDefaultFocus() has been called)
        IM_ASSERT(g.NavWindow);
        if (g.NavInitRequestFromMove)
            SetNavIDWithRectRel(g.NavInitResultId, g.NavLayer, g.NavInitResultRectRel);
        else
            SetNavID(g.NavInitResultId, g.NavLayer);
        g.NavWindow->NavRectRel[g.NavLayer] = g.NavInitResultRectRel;
    }
    g.NavInitRequest = false;
    g.NavInitRequestFromMove = false;
    g.NavInitResultId = 0;
    g.NavJustMovedToId = 0;

    // Process navigation move request
    if (g.NavMoveRequest)
        NavUpdateMoveResult();

    // When a forwarded move request failed, we restore the highlight that we disabled during the forward frame
    if (g.NavMoveRequestForward == vsonyp0werNavForward_ForwardActive)
    {
        IM_ASSERT(g.NavMoveRequest);
        if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0)
            g.NavDisableHighlight = false;
        g.NavMoveRequestForward = vsonyp0werNavForward_None;
    }

    // Apply application mouse position movement, after we had a chance to process move request result.
    if (g.NavMousePosDirty && g.NavIdIsAlive)
    {
        // Set mouse position given our knowledge of the navigated item position from last frame
        if ((g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableSetMousePos) && (g.IO.BackendFlags & vsonyp0werBackendFlags_HasSetMousePos))
        {
            if (!g.NavDisableHighlight && g.NavDisableMouseHover && g.NavWindow)
            {
                g.IO.MousePos = g.IO.MousePosPrev = NavCalcPreferredRefPos();
                g.IO.WantSetMousePos = true;
            }
        }
        g.NavMousePosDirty = false;
    }
    g.NavIdIsAlive = false;
    g.NavJustTabbedId = 0;
    IM_ASSERT(g.NavLayer == 0 || g.NavLayer == 1);

    // Store our return window (for returning from Layer 1 to Layer 0) and clear it as soon as we step back in our own Layer 0
    if (g.NavWindow)
        NavSaveLastChildNavWindowIntoParent(g.NavWindow);
    if (g.NavWindow && g.NavWindow->NavLastChildNavWindow != NULL && g.NavLayer == 0)
        g.NavWindow->NavLastChildNavWindow = NULL;

    // Update CTRL+TAB and Windowing features (hold Square to move/resize/etc.)
    NavUpdateWindowing();

    // Set output flags for user application
    g.IO.NavActive = (nav_keyboard_active || nav_gamepad_active) && g.NavWindow && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs);
    g.IO.NavVisible = (g.IO.NavActive && g.NavId != 0 && !g.NavDisableHighlight) || (g.NavWindowingTarget != NULL);

    // Process NavCancel input (to close a popup, get back to parent, clear focus)
    if (IsNavInputPressed(vsonyp0werNavInput_Cancel, vsonyp0werInputReadMode_Pressed))
    {
        if (g.ActiveId != 0)
        {
            if (!(g.ActiveIdBlockNavInputFlags & (1 << vsonyp0werNavInput_Cancel)))
                ClearActiveID();
        } else if (g.NavWindow && (g.NavWindow->Flags & vsonyp0werWindowFlags_ChildWindow) && !(g.NavWindow->Flags & vsonyp0werWindowFlags_Popup) && g.NavWindow->ParentWindow)
        {
            // Exit child window
            vsonyp0werWindow* child_window = g.NavWindow;
            vsonyp0werWindow* parent_window = g.NavWindow->ParentWindow;
            IM_ASSERT(child_window->ChildId != 0);
            FocusWindow(parent_window);
            SetNavID(child_window->ChildId, 0);
            g.NavIdIsAlive = false;
            if (g.NavDisableMouseHover)
                g.NavMousePosDirty = true;
        } else if (g.OpenPopupStack.Size > 0)
        {
            // Close open popup/menu
            if (!(g.OpenPopupStack.back().Window->Flags & vsonyp0werWindowFlags_Modal))
                ClosePopupToLevel(g.OpenPopupStack.Size - 1, true);
        } else if (g.NavLayer != 0)
        {
            // Leave the "menu" layer
            NavRestoreLayer(vsonyp0werNavLayer_Main);
        } else
        {
            // Clear NavLastId for popups but keep it for regular child window so we can leave one and come back where we were
            if (g.NavWindow && ((g.NavWindow->Flags & vsonyp0werWindowFlags_Popup) || !(g.NavWindow->Flags & vsonyp0werWindowFlags_ChildWindow)))
                g.NavWindow->NavLastIds[0] = 0;
            g.NavId = 0;
        }
    }

    // Process manual activation request
    g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId = 0;
    if (g.NavId != 0 && !g.NavDisableHighlight && !g.NavWindowingTarget && g.NavWindow && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs))
    {
        bool activate_down = IsNavInputDown(vsonyp0werNavInput_Activate);
        bool activate_pressed = activate_down && IsNavInputPressed(vsonyp0werNavInput_Activate, vsonyp0werInputReadMode_Pressed);
        if (g.ActiveId == 0 && activate_pressed)
            g.NavActivateId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_down)
            g.NavActivateDownId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_pressed)
            g.NavActivatePressedId = g.NavId;
        if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && IsNavInputPressed(vsonyp0werNavInput_Input, vsonyp0werInputReadMode_Pressed))
            g.NavInputId = g.NavId;
    }
    if (g.NavWindow && (g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs))
        g.NavDisableHighlight = true;
    if (g.NavActivateId != 0)
        IM_ASSERT(g.NavActivateDownId == g.NavActivateId);
    g.NavMoveRequest = false;

    // Process programmatic activation request
    if (g.NavNextActivateId != 0)
        g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId = g.NavNextActivateId;
    g.NavNextActivateId = 0;

    // Initiate directional inputs request
    const int allowed_dir_flags = (g.ActiveId == 0) ? ~0 : g.ActiveIdAllowNavDirFlags;
    if (g.NavMoveRequestForward == vsonyp0werNavForward_None)
    {
        g.NavMoveDir = vsonyp0werDir_None;
        g.NavMoveRequestFlags = vsonyp0werNavMoveFlags_None;
        if (g.NavWindow && !g.NavWindowingTarget && allowed_dir_flags && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs))
        {
            if ((allowed_dir_flags & (1 << vsonyp0werDir_Left)) && IsNavInputPressedAnyOfTwo(vsonyp0werNavInput_DpadLeft, vsonyp0werNavInput_KeyLeft_, vsonyp0werInputReadMode_Repeat)) g.NavMoveDir = vsonyp0werDir_Left;
            if ((allowed_dir_flags & (1 << vsonyp0werDir_Right)) && IsNavInputPressedAnyOfTwo(vsonyp0werNavInput_DpadRight, vsonyp0werNavInput_KeyRight_, vsonyp0werInputReadMode_Repeat)) g.NavMoveDir = vsonyp0werDir_Right;
            if ((allowed_dir_flags & (1 << vsonyp0werDir_Up)) && IsNavInputPressedAnyOfTwo(vsonyp0werNavInput_DpadUp, vsonyp0werNavInput_KeyUp_, vsonyp0werInputReadMode_Repeat)) g.NavMoveDir = vsonyp0werDir_Up;
            if ((allowed_dir_flags & (1 << vsonyp0werDir_Down)) && IsNavInputPressedAnyOfTwo(vsonyp0werNavInput_DpadDown, vsonyp0werNavInput_KeyDown_, vsonyp0werInputReadMode_Repeat)) g.NavMoveDir = vsonyp0werDir_Down;
        }
        g.NavMoveClipDir = g.NavMoveDir;
    } else
    {
        // Forwarding previous request (which has been modified, e.g. wrap around menus rewrite the requests with a starting rectangle at the other side of the window)
        // (Preserve g.NavMoveRequestFlags, g.NavMoveClipDir which were set by the NavMoveRequestForward() function)
        IM_ASSERT(g.NavMoveDir != vsonyp0werDir_None && g.NavMoveClipDir != vsonyp0werDir_None);
        IM_ASSERT(g.NavMoveRequestForward == vsonyp0werNavForward_ForwardQueued);
        g.NavMoveRequestForward = vsonyp0werNavForward_ForwardActive;
    }

    // Update PageUp/PageDown scroll
    float nav_scoring_rect_offset_y = 0.0f;
    if (nav_keyboard_active)
        nav_scoring_rect_offset_y = NavUpdatePageUpPageDown(allowed_dir_flags);

    // If we initiate a movement request and have no current NavId, we initiate a InitDefautRequest that will be used as a fallback if the direction fails to find a match
    if (g.NavMoveDir != vsonyp0werDir_None)
    {
        g.NavMoveRequest = true;
        g.NavMoveDirLast = g.NavMoveDir;
    }
    if (g.NavMoveRequest && g.NavId == 0)
    {
        g.NavInitRequest = g.NavInitRequestFromMove = true;
        g.NavInitResultId = 0;
        g.NavDisableHighlight = false;
    }
    NavUpdateAnyRequestFlag();

    // Scrolling
    if (g.NavWindow && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs) && !g.NavWindowingTarget)
    {
        // *Fallback* manual-scroll with Nav directional keys when window has no navigable item
        vsonyp0werWindow* window = g.NavWindow;
        const float scroll_speed = ImFloor(window->CalcFontSize() * 100 * g.IO.DeltaTime + 0.5f); // We need round the scrolling speed because sub-pixel scroll isn't reliably supported.
        if (window->DC.NavLayerActiveMask == 0x00 && window->DC.NavHasScroll && g.NavMoveRequest)
        {
            if (g.NavMoveDir == vsonyp0werDir_Left || g.NavMoveDir == vsonyp0werDir_Right)
                SetWindowScrollX(window, ImFloor(window->Scroll.x + ((g.NavMoveDir == vsonyp0werDir_Left) ? -1.0f : +1.0f) * scroll_speed));
            if (g.NavMoveDir == vsonyp0werDir_Up || g.NavMoveDir == vsonyp0werDir_Down)
                SetWindowScrollY(window, ImFloor(window->Scroll.y + ((g.NavMoveDir == vsonyp0werDir_Up) ? -1.0f : +1.0f) * scroll_speed));
        }

        // *Normal* Manual scroll with NavScrollXXX keys
        // Next movement request will clamp the NavId reference rectangle to the visible area, so navigation will resume within those bounds.
        ImVec2 scroll_dir = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_PadLStick, vsonyp0werInputReadMode_Down, 1.0f / 10.0f, 10.0f);
        if (scroll_dir.x != 0.0f && window->ScrollbarX)
        {
            SetWindowScrollX(window, ImFloor(window->Scroll.x + scroll_dir.x * scroll_speed));
            g.NavMoveFromClampedRefRect = true;
        }
        if (scroll_dir.y != 0.0f)
        {
            SetWindowScrollY(window, ImFloor(window->Scroll.y + scroll_dir.y * scroll_speed));
            g.NavMoveFromClampedRefRect = true;
        }
    }

    // Reset search results
    g.NavMoveResultLocal.Clear();
    g.NavMoveResultLocalVisibleSet.Clear();
    g.NavMoveResultOther.Clear();

    // When we have manually scrolled (without using navigation) and NavId becomes out of bounds, we project its bounding box to the visible area to restart navigation within visible items
    if (g.NavMoveRequest && g.NavMoveFromClampedRefRect && g.NavLayer == 0)
    {
        vsonyp0werWindow* window = g.NavWindow;
        ImRect window_rect_rel(window->InnerMainRect.Min - window->Pos - ImVec2(1, 1), window->InnerMainRect.Max - window->Pos + ImVec2(1, 1));
        if (!window_rect_rel.Contains(window->NavRectRel[g.NavLayer]))
        {
            float pad = window->CalcFontSize() * 0.5f;
            window_rect_rel.Expand(ImVec2(-ImMin(window_rect_rel.GetWidth(), pad), -ImMin(window_rect_rel.GetHeight(), pad))); // Terrible approximation for the intent of starting navigation from first fully visible item
            window->NavRectRel[g.NavLayer].ClipWith(window_rect_rel);
            g.NavId = 0;
        }
        g.NavMoveFromClampedRefRect = false;
    }

    // For scoring we use a single segment on the left side our current item bounding box (not touching the edge to avoid box overlap with zero-spaced items)
    ImRect nav_rect_rel = (g.NavWindow && !g.NavWindow->NavRectRel[g.NavLayer].IsInverted()) ? g.NavWindow->NavRectRel[g.NavLayer] : ImRect(0, 0, 0, 0);
    g.NavScoringRectScreen = g.NavWindow ? ImRect(g.NavWindow->Pos + nav_rect_rel.Min, g.NavWindow->Pos + nav_rect_rel.Max) : GetViewportRect();
    g.NavScoringRectScreen.TranslateY(nav_scoring_rect_offset_y);
    g.NavScoringRectScreen.Min.x = ImMin(g.NavScoringRectScreen.Min.x + 1.0f, g.NavScoringRectScreen.Max.x);
    g.NavScoringRectScreen.Max.x = g.NavScoringRectScreen.Min.x;
    IM_ASSERT(!g.NavScoringRectScreen.IsInverted()); // Ensure if we have a finite, non-inverted bounding box here will allows us to remove extraneous ImFabs() calls in NavScoreItem().
    //GetForegroundDrawList()->AddRect(g.NavScoringRectScreen.Min, g.NavScoringRectScreen.Max, IM_COL32(255,200,0,255)); // [DEBUG]
    g.NavScoringCount = 0;
#if vsonyp0wer_DEBUG_NAV_RECTS
    if (g.NavWindow)
    {
        ImDrawList* draw_list = GetForegroundDrawList(g.NavWindow);
        if (1) { for (int layer = 0; layer < 2; layer++) draw_list->AddRect(g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Min, g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Max, IM_COL32(255, 200, 0, 255)); } // [DEBUG]
        if (1) { ImU32 col = (!g.NavWindow->Hidden) ? IM_COL32(255, 0, 255, 255) : IM_COL32(255, 0, 0, 255); ImVec2 p = NavCalcPreferredRefPos(); char buf[32]; ImFormatString(buf, 32, "%d", g.NavLayer); draw_list->AddCircleFilled(p, 3.0f, col); draw_list->AddText(NULL, 13.0f, p + ImVec2(8, -4), col, buf); }
    }
#endif
}

// Apply result from previous frame navigation directional move request
static void vsonyp0wer::NavUpdateMoveResult()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0)
    {
        // In a situation when there is no results but NavId != 0, re-enable the Navigation highlight (because g.NavId is not considered as a possible result)
        if (g.NavId != 0)
        {
            g.NavDisableHighlight = false;
            g.NavDisableMouseHover = true;
        }
        return;
    }

    // Select which result to use
    vsonyp0werNavMoveResult* result = (g.NavMoveResultLocal.ID != 0) ? &g.NavMoveResultLocal : &g.NavMoveResultOther;

    // PageUp/PageDown behavior first jumps to the bottom/top mostly visible item, _otherwise_ use the result from the previous/next page.
    if (g.NavMoveRequestFlags & vsonyp0werNavMoveFlags_AlsoScoreVisibleSet)
        if (g.NavMoveResultLocalVisibleSet.ID != 0 && g.NavMoveResultLocalVisibleSet.ID != g.NavId)
            result = &g.NavMoveResultLocalVisibleSet;

    // Maybe entering a flattened child from the outside? In this case solve the tie using the regular scoring rules.
    if (result != &g.NavMoveResultOther && g.NavMoveResultOther.ID != 0 && g.NavMoveResultOther.Window->ParentWindow == g.NavWindow)
        if ((g.NavMoveResultOther.DistBox < result->DistBox) || (g.NavMoveResultOther.DistBox == result->DistBox && g.NavMoveResultOther.DistCenter < result->DistCenter))
            result = &g.NavMoveResultOther;
    IM_ASSERT(g.NavWindow && result->Window);

    // Scroll to keep newly navigated item fully into view.
    if (g.NavLayer == 0)
    {
        ImRect rect_abs = ImRect(result->RectRel.Min + result->Window->Pos, result->RectRel.Max + result->Window->Pos);
        NavScrollToBringItemIntoView(result->Window, rect_abs);

        // Estimate upcoming scroll so we can offset our result position so mouse position can be applied immediately after in NavUpdate()
        ImVec2 next_scroll = CalcNextScrollFromScrollTargetAndClamp(result->Window, false);
        ImVec2 delta_scroll = result->Window->Scroll - next_scroll;
        result->RectRel.Translate(delta_scroll);

        // Also scroll parent window to keep us into view if necessary (we could/should technically recurse back the whole the parent hierarchy).
        if (result->Window->Flags & vsonyp0werWindowFlags_ChildWindow)
            NavScrollToBringItemIntoView(result->Window->ParentWindow, ImRect(rect_abs.Min + delta_scroll, rect_abs.Max + delta_scroll));
    }

    ClearActiveID();
    g.NavWindow = result->Window;
    if (g.NavId != result->ID)
    {
        // Don't set NavJustMovedToId if just landed on the same spot (which may happen with vsonyp0werNavMoveFlags_AllowCurrentNavId)
        g.NavJustMovedToId = result->ID;
        g.NavJustMovedToMultiSelectScopeId = result->SelectScopeId;
    }
    SetNavIDWithRectRel(result->ID, g.NavLayer, result->RectRel);
    g.NavMoveFromClampedRefRect = false;
}

static float vsonyp0wer::NavUpdatePageUpPageDown(int allowed_dir_flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.NavMoveDir == vsonyp0werDir_None && g.NavWindow && !(g.NavWindow->Flags & vsonyp0werWindowFlags_NoNavInputs) && !g.NavWindowingTarget && g.NavLayer == 0)
    {
        vsonyp0werWindow* window = g.NavWindow;
        bool page_up_held = IsKeyDown(g.IO.KeyMap[vsonyp0werKey_PageUp]) && (allowed_dir_flags & (1 << vsonyp0werDir_Up));
        bool page_down_held = IsKeyDown(g.IO.KeyMap[vsonyp0werKey_PageDown]) && (allowed_dir_flags & (1 << vsonyp0werDir_Down));
        if (page_up_held != page_down_held) // If either (not both) are pressed
        {
            if (window->DC.NavLayerActiveMask == 0x00 && window->DC.NavHasScroll)
            {
                // Fallback manual-scroll when window has no navigable item
                if (IsKeyPressed(g.IO.KeyMap[vsonyp0werKey_PageUp], true))
                    SetWindowScrollY(window, window->Scroll.y - window->InnerClipRect.GetHeight());
                else if (IsKeyPressed(g.IO.KeyMap[vsonyp0werKey_PageDown], true))
                    SetWindowScrollY(window, window->Scroll.y + window->InnerClipRect.GetHeight());
            } else
            {
                const ImRect& nav_rect_rel = window->NavRectRel[g.NavLayer];
                const float page_offset_y = ImMax(0.0f, window->InnerClipRect.GetHeight() - window->CalcFontSize() * 1.0f + nav_rect_rel.GetHeight());
                float nav_scoring_rect_offset_y = 0.0f;
                if (IsKeyPressed(g.IO.KeyMap[vsonyp0werKey_PageUp], true))
                {
                    nav_scoring_rect_offset_y = -page_offset_y;
                    g.NavMoveDir = vsonyp0werDir_Down; // Because our scoring rect is offset, we intentionally request the opposite direction (so we can always land on the last item)
                    g.NavMoveClipDir = vsonyp0werDir_Up;
                    g.NavMoveRequestFlags = vsonyp0werNavMoveFlags_AllowCurrentNavId | vsonyp0werNavMoveFlags_AlsoScoreVisibleSet;
                } else if (IsKeyPressed(g.IO.KeyMap[vsonyp0werKey_PageDown], true))
                {
                    nav_scoring_rect_offset_y = +page_offset_y;
                    g.NavMoveDir = vsonyp0werDir_Up; // Because our scoring rect is offset, we intentionally request the opposite direction (so we can always land on the last item)
                    g.NavMoveClipDir = vsonyp0werDir_Down;
                    g.NavMoveRequestFlags = vsonyp0werNavMoveFlags_AllowCurrentNavId | vsonyp0werNavMoveFlags_AlsoScoreVisibleSet;
                }
                return nav_scoring_rect_offset_y;
            }
        }
    }
    return 0.0f;
}

static int FindWindowFocusIndex(vsonyp0werWindow * window) // FIXME-OPT O(N)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int i = g.WindowsFocusOrder.Size - 1; i >= 0; i--)
        if (g.WindowsFocusOrder[i] == window)
            return i;
    return -1;
}

static vsonyp0werWindow * FindWindowNavFocusable(int i_start, int i_stop, int dir) // FIXME-OPT O(N)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int i = i_start; i >= 0 && i < g.WindowsFocusOrder.Size && i != i_stop; i += dir)
        if (vsonyp0wer::IsWindowNavFocusable(g.WindowsFocusOrder[i]))
            return g.WindowsFocusOrder[i];
    return NULL;
}

static void NavUpdateWindowingHighlightWindow(int focus_change_dir)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.NavWindowingTarget);
    if (g.NavWindowingTarget->Flags & vsonyp0werWindowFlags_Modal)
        return;

    const int i_current = FindWindowFocusIndex(g.NavWindowingTarget);
    vsonyp0werWindow* window_target = FindWindowNavFocusable(i_current + focus_change_dir, -INT_MAX, focus_change_dir);
    if (!window_target)
        window_target = FindWindowNavFocusable((focus_change_dir < 0) ? (g.WindowsFocusOrder.Size - 1) : 0, i_current, focus_change_dir);
    if (window_target) // Don't reset windowing target if there's a single window in the list
        g.NavWindowingTarget = g.NavWindowingTargetAnim = window_target;
    g.NavWindowingToggleLayer = false;
}

// Windowing management mode
// Keyboard: CTRL+Tab (change focus/move/resize), Alt (toggle menu layer)
// Gamepad:  Hold Menu/Square (change focus/move/resize), Tap Menu/Square (toggle menu layer)
static void vsonyp0wer::NavUpdateWindowing()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* apply_focus_window = NULL;
    bool apply_toggle_layer = false;

    vsonyp0werWindow* modal_window = GetFrontMostPopupModal();
    if (modal_window != NULL)
    {
        g.NavWindowingTarget = NULL;
        return;
    }

    // Fade out
    if (g.NavWindowingTargetAnim && g.NavWindowingTarget == NULL)
    {
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha - g.IO.DeltaTime * 10.0f, 0.0f);
        if (g.DimBgRatio <= 0.0f && g.NavWindowingHighlightAlpha <= 0.0f)
            g.NavWindowingTargetAnim = NULL;
    }

    // Start CTRL-TAB or Square+L/R window selection
    bool start_windowing_with_gamepad = !g.NavWindowingTarget && IsNavInputPressed(vsonyp0werNavInput_Menu, vsonyp0werInputReadMode_Pressed);
    bool start_windowing_with_keyboard = !g.NavWindowingTarget && g.IO.KeyCtrl && IsKeyPressedMap(vsonyp0werKey_Tab) && (g.IO.ConfigFlags & vsonyp0werConfigFlags_NavEnableKeyboard);
    if (start_windowing_with_gamepad || start_windowing_with_keyboard)
        if (vsonyp0werWindow * window = g.NavWindow ? g.NavWindow : FindWindowNavFocusable(g.WindowsFocusOrder.Size - 1, -INT_MAX, -1))
        {
            g.NavWindowingTarget = g.NavWindowingTargetAnim = window;
            g.NavWindowingTimer = g.NavWindowingHighlightAlpha = 0.0f;
            g.NavWindowingToggleLayer = start_windowing_with_keyboard ? false : true;
            g.NavInputSource = start_windowing_with_keyboard ? vsonyp0werInputSource_NavKeyboard : vsonyp0werInputSource_NavGamepad;
        }

    // Gamepad update
    g.NavWindowingTimer += g.IO.DeltaTime;
    if (g.NavWindowingTarget && g.NavInputSource == vsonyp0werInputSource_NavGamepad)
    {
        // Highlight only appears after a brief time holding the button, so that a fast tap on PadMenu (to toggle NavLayer) doesn't add visual noise
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha, ImSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f));

        // Select window to focus
        const int focus_change_dir = (int)IsNavInputPressed(vsonyp0werNavInput_FocusPrev, vsonyp0werInputReadMode_RepeatSlow) - (int)IsNavInputPressed(vsonyp0werNavInput_FocusNext, vsonyp0werInputReadMode_RepeatSlow);
        if (focus_change_dir != 0)
        {
            NavUpdateWindowingHighlightWindow(focus_change_dir);
            g.NavWindowingHighlightAlpha = 1.0f;
        }

        // Single press toggles NavLayer, long press with L/R apply actual focus on release (until then the window was merely rendered front-most)
        if (!IsNavInputDown(vsonyp0werNavInput_Menu))
        {
            g.NavWindowingToggleLayer &= (g.NavWindowingHighlightAlpha < 1.0f); // Once button was held long enough we don't consider it a tap-to-toggle-layer press anymore.
            if (g.NavWindowingToggleLayer && g.NavWindow)
                apply_toggle_layer = true;
            else if (!g.NavWindowingToggleLayer)
                apply_focus_window = g.NavWindowingTarget;
            g.NavWindowingTarget = NULL;
        }
    }

    // Keyboard: Focus
    if (g.NavWindowingTarget && g.NavInputSource == vsonyp0werInputSource_NavKeyboard)
    {
        // Visuals only appears after a brief time after pressing TAB the first time, so that a fast CTRL+TAB doesn't add visual noise
        g.NavWindowingHighlightAlpha = ImMax(g.NavWindowingHighlightAlpha, ImSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f)); // 1.0f
        if (IsKeyPressedMap(vsonyp0werKey_Tab, true))
            NavUpdateWindowingHighlightWindow(g.IO.KeyShift ? +1 : -1);
        if (!g.IO.KeyCtrl)
            apply_focus_window = g.NavWindowingTarget;
    }

    // Keyboard: Press and Release ALT to toggle menu layer
    // FIXME: We lack an explicit IO variable for "is the vsonyp0wer window focused", so compare mouse validity to detect the common case of back-end clearing releases all keys on ALT-TAB
    if (IsNavInputPressed(vsonyp0werNavInput_KeyMenu_, vsonyp0werInputReadMode_Pressed))
        g.NavWindowingToggleLayer = true;
    if ((g.ActiveId == 0 || g.ActiveIdAllowOverlap) && g.NavWindowingToggleLayer && IsNavInputPressed(vsonyp0werNavInput_KeyMenu_, vsonyp0werInputReadMode_Released))
        if (IsMousePosValid(&g.IO.MousePos) == IsMousePosValid(&g.IO.MousePosPrev))
            apply_toggle_layer = true;

    // Move window
    if (g.NavWindowingTarget && !(g.NavWindowingTarget->Flags & vsonyp0werWindowFlags_NoMove))
    {
        ImVec2 move_delta;
        if (g.NavInputSource == vsonyp0werInputSource_NavKeyboard && !g.IO.KeyShift)
            move_delta = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_Keyboard, vsonyp0werInputReadMode_Down);
        if (g.NavInputSource == vsonyp0werInputSource_NavGamepad)
            move_delta = GetNavInputAmount2d(vsonyp0werNavDirSourceFlags_PadLStick, vsonyp0werInputReadMode_Down);
        if (move_delta.x != 0.0f || move_delta.y != 0.0f)
        {
            const float NAV_MOVE_SPEED = 800.0f;
            const float move_speed = ImFloor(NAV_MOVE_SPEED * g.IO.DeltaTime * ImMin(g.IO.DisplayFramebufferScale.x, g.IO.DisplayFramebufferScale.y)); // FIXME: Doesn't code variable framerate very well
            g.NavWindowingTarget->RootWindow->Pos += move_delta * move_speed;
            g.NavDisableMouseHover = true;
            MarkIniSettingsDirty(g.NavWindowingTarget);
        }
    }

    // Apply final focus
    if (apply_focus_window && (g.NavWindow == NULL || apply_focus_window != g.NavWindow->RootWindow))
    {
        g.NavDisableHighlight = false;
        g.NavDisableMouseHover = true;
        apply_focus_window = NavRestoreLastChildNavWindow(apply_focus_window);
        ClosePopupsOverWindow(apply_focus_window);
        ClearActiveID();
        FocusWindow(apply_focus_window);
        if (apply_focus_window->NavLastIds[0] == 0)
            NavInitWindow(apply_focus_window, false);

        // If the window only has a menu layer, select it directly
        if (apply_focus_window->DC.NavLayerActiveMask == (1 << vsonyp0werNavLayer_Menu))
            g.NavLayer = vsonyp0werNavLayer_Menu;
    }
    if (apply_focus_window)
        g.NavWindowingTarget = NULL;

    // Apply menu/layer toggle
    if (apply_toggle_layer && g.NavWindow)
    {
        // Move to parent menu if necessary
        vsonyp0werWindow* new_nav_window = g.NavWindow;
        while (new_nav_window->ParentWindow
            && (new_nav_window->DC.NavLayerActiveMask & (1 << vsonyp0werNavLayer_Menu)) == 0
            && (new_nav_window->Flags & vsonyp0werWindowFlags_ChildWindow) != 0
            && (new_nav_window->Flags & (vsonyp0werWindowFlags_Popup | vsonyp0werWindowFlags_ChildMenu)) == 0)
            new_nav_window = new_nav_window->ParentWindow;
        if (new_nav_window != g.NavWindow)
        {
            vsonyp0werWindow* old_nav_window = g.NavWindow;
            FocusWindow(new_nav_window);
            new_nav_window->NavLastChildNavWindow = old_nav_window;
        }
        g.NavDisableHighlight = false;
        g.NavDisableMouseHover = true;

        // When entering a regular menu bar with the Alt key, we always reinitialize the navigation ID.
        const vsonyp0werNavLayer new_nav_layer = (g.NavWindow->DC.NavLayerActiveMask & (1 << vsonyp0werNavLayer_Menu)) ? (vsonyp0werNavLayer)((int)g.NavLayer ^ 1) : vsonyp0werNavLayer_Main;
        NavRestoreLayer(new_nav_layer);
    }
}

// Window has already passed the IsWindowNavFocusable()
static const char* GetFallbackWindowNameForWindowingList(vsonyp0werWindow * window)
{
    if (window->Flags & vsonyp0werWindowFlags_Popup)
        return "(Popup)";
    if ((window->Flags & vsonyp0werWindowFlags_MenuBar) && strcmp(window->Name, "##MainMenuBar") == 0)
        return "(Main menu bar)";
    return "(Untitled)";
}

// Overlay displayed when using CTRL+TAB. Called by EndFrame().
void vsonyp0wer::NavUpdateWindowingList()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.NavWindowingTarget != NULL);

    if (g.NavWindowingTimer < NAV_WINDOWING_LIST_APPEAR_DELAY)
        return;

    if (g.NavWindowingList == NULL)
        g.NavWindowingList = FindWindowByName("###NavWindowingList");
    SetNextWindowSizeConstraints(ImVec2(g.IO.DisplaySize.x * 0.20f, g.IO.DisplaySize.y * 0.20f), ImVec2(FLT_MAX, FLT_MAX));
    SetNextWindowPos(g.IO.DisplaySize * 0.5f, vsonyp0werCond_Always, ImVec2(0.5f, 0.5f));
    PushStyleVar(vsonyp0werStyleVar_WindowPadding, g.Style.WindowPadding * 2.0f);
    Begin("###NavWindowingList", NULL, vsonyp0werWindowFlags_NoTitleBar | vsonyp0werWindowFlags_NoFocusOnAppearing | vsonyp0werWindowFlags_NoResize | vsonyp0werWindowFlags_NoMove | vsonyp0werWindowFlags_NoInputs | vsonyp0werWindowFlags_AlwaysAutoResize | vsonyp0werWindowFlags_NoSavedSettings);
    for (int n = g.WindowsFocusOrder.Size - 1; n >= 0; n--)
    {
        vsonyp0werWindow* window = g.WindowsFocusOrder[n];
        if (!IsWindowNavFocusable(window))
            continue;
        const char* label = window->Name;
        if (label == FindRenderedTextEnd(label))
            label = GetFallbackWindowNameForWindowingList(window);
        Selectable(label, g.NavWindowingTarget == window);
    }
    End();
    PopStyleVar();
}

//-----------------------------------------------------------------------------
// [SECTION] COLUMNS
// In the current version, Columns are very weak. Needs to be replaced with a more full-featured system.
//-----------------------------------------------------------------------------

void vsonyp0wer::NextColumn()
{
    vsonyp0werWindow* window = GetCurrentWindow();
    if (window->SkipItems || window->DC.CurrentColumns == NULL)
        return;

    vsonyp0werContext & g = *Gvsonyp0wer;
    vsonyp0werColumns * columns = window->DC.CurrentColumns;

    if (columns->Count == 1)
    {
        window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
        IM_ASSERT(columns->Current == 0);
        return;
    }

    PopItemWidth();
    PopClipRect();

    columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
    if (++columns->Current < columns->Count)
    {
        // New column (columns 1+ cancels out IndentX)
        window->DC.ColumnsOffset.x = GetColumnOffset(columns->Current) - window->DC.Indent.x + g.Style.ItemSpacing.x;
        window->DrawList->ChannelsSetCurrent(columns->Current);
    } else
    {
        // New row/line
        window->DC.ColumnsOffset.x = 0.0f;
        window->DrawList->ChannelsSetCurrent(0);
        columns->Current = 0;
        columns->LineMinY = columns->LineMaxY;
    }
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
    window->DC.CursorPos.y = columns->LineMinY;
    window->DC.CurrentLineSize = ImVec2(0.0f, 0.0f);
    window->DC.CurrentLineTextBaseOffset = 0.0f;

    PushColumnClipRect();
    PushItemWidth(GetColumnWidth() * 0.65f);  // FIXME-COLUMNS: Move on columns setup
}

int vsonyp0wer::GetColumnIndex()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CurrentColumns ? window->DC.CurrentColumns->Current : 0;
}

int vsonyp0wer::GetColumnsCount()
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    return window->DC.CurrentColumns ? window->DC.CurrentColumns->Count : 1;
}

static float OffsetNormToPixels(const vsonyp0werColumns * columns, float offset_norm)
{
    return offset_norm * (columns->MaxX - columns->MinX);
}

static float PixelsToOffsetNorm(const vsonyp0werColumns * columns, float offset)
{
    return offset / (columns->MaxX - columns->MinX);
}

static const float COLUMNS_HIT_RECT_HALF_WIDTH = 4.0f;

static float GetDraggedColumnOffset(vsonyp0werColumns * columns, int column_index)
{
    // Active (dragged) column always follow mouse. The reason we need this is that dragging a column to the right edge of an auto-resizing
    // window creates a feedback loop because we store normalized positions. So while dragging we enforce absolute positioning.
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    IM_ASSERT(column_index > 0); // We are not supposed to drag column 0.
    IM_ASSERT(g.ActiveId == columns->ID + vsonyp0werID(column_index));

    float x = g.IO.MousePos.x - g.ActiveIdClickOffset.x + COLUMNS_HIT_RECT_HALF_WIDTH - window->Pos.x;
    x = ImMax(x, vsonyp0wer::GetColumnOffset(column_index - 1) + g.Style.ColumnsMinSpacing);
    if ((columns->Flags & vsonyp0werColumnsFlags_NoPreserveWidths))
        x = ImMin(x, vsonyp0wer::GetColumnOffset(column_index + 1) - g.Style.ColumnsMinSpacing);

    return x;
}

float vsonyp0wer::GetColumnOffset(int column_index)
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    IM_ASSERT(column_index < columns->Columns.Size);

    const float t = columns->Columns[column_index].OffsetNorm;
    const float x_offset = ImLerp(columns->MinX, columns->MaxX, t);
    return x_offset;
}

static float GetColumnWidthEx(vsonyp0werColumns * columns, int column_index, bool before_resize = false)
{
    if (column_index < 0)
        column_index = columns->Current;

    float offset_norm;
    if (before_resize)
        offset_norm = columns->Columns[column_index + 1].OffsetNormBeforeResize - columns->Columns[column_index].OffsetNormBeforeResize;
    else
        offset_norm = columns->Columns[column_index + 1].OffsetNorm - columns->Columns[column_index].OffsetNorm;
    return OffsetNormToPixels(columns, offset_norm);
}

float vsonyp0wer::GetColumnWidth(int column_index)
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    return OffsetNormToPixels(columns, columns->Columns[column_index + 1].OffsetNorm - columns->Columns[column_index].OffsetNorm);
}

void vsonyp0wer::SetColumnOffset(int column_index, float offset)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    IM_ASSERT(column_index < columns->Columns.Size);

    const bool preserve_width = !(columns->Flags & vsonyp0werColumnsFlags_NoPreserveWidths) && (column_index < columns->Count - 1);
    const float width = preserve_width ? GetColumnWidthEx(columns, column_index, columns->IsBeingResized) : 0.0f;

    if (!(columns->Flags & vsonyp0werColumnsFlags_NoForceWithinWindow))
        offset = ImMin(offset, columns->MaxX - g.Style.ColumnsMinSpacing * (columns->Count - column_index));
    columns->Columns[column_index].OffsetNorm = PixelsToOffsetNorm(columns, offset - columns->MinX);

    if (preserve_width)
        SetColumnOffset(column_index + 1, offset + ImMax(g.Style.ColumnsMinSpacing, width));
}

void vsonyp0wer::SetColumnWidth(int column_index, float width)
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    if (column_index < 0)
        column_index = columns->Current;
    SetColumnOffset(column_index + 1, GetColumnOffset(column_index) + width);
}

void vsonyp0wer::PushColumnClipRect(int column_index)
{
    vsonyp0werWindow* window = GetCurrentWindowRead();
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    if (column_index < 0)
        column_index = columns->Current;

    vsonyp0werColumnData* column = &columns->Columns[column_index];
    PushClipRect(column->ClipRect.Min, column->ClipRect.Max, false);
}

vsonyp0werColumns* vsonyp0wer::FindOrCreateColumns(vsonyp0werWindow * window, vsonyp0werID id)
{
    // We have few columns per window so for now we don't need bother much with turning this into a faster lookup.
    for (int n = 0; n < window->ColumnsStohnly.Size; n++)
        if (window->ColumnsStohnly[n].ID == id)
            return &window->ColumnsStohnly[n];

    window->ColumnsStohnly.push_back(vsonyp0werColumns());
    vsonyp0werColumns * columns = &window->ColumnsStohnly.back();
    columns->ID = id;
    return columns;
}

vsonyp0werID vsonyp0wer::GetColumnsID(const char* str_id, int columns_count)
{
    vsonyp0werWindow* window = GetCurrentWindow();

    // Differentiate column ID with an arbitrary prefix for cases where users name their columns set the same as another widget.
    // In addition, when an identifier isn't explicitly provided we include the number of columns in the hash to make it uniquer.
    PushID(0x11223347 + (str_id ? 0 : columns_count));
    vsonyp0werID id = window->GetID(str_id ? str_id : "columns");
    PopID();

    return id;
}

void vsonyp0wer::BeginColumns(const char* str_id, int columns_count, vsonyp0werColumnsFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();

    IM_ASSERT(columns_count >= 1);
    IM_ASSERT(window->DC.CurrentColumns == NULL); // Nested columns are currently not supported

    vsonyp0werID id = GetColumnsID(str_id, columns_count);

    // Acquire stohnly for the columns set
    vsonyp0werColumns * columns = FindOrCreateColumns(window, id);
    IM_ASSERT(columns->ID == id);
    columns->Current = 0;
    columns->Count = columns_count;
    columns->Flags = flags;
    window->DC.CurrentColumns = columns;

    // Set state for first column
    const float content_region_width = (window->SizeContentsExplicit.x != 0.0f) ? (window->SizeContentsExplicit.x) : (window->InnerClipRect.Max.x - window->Pos.x);
    columns->MinX = window->DC.Indent.x - g.Style.ItemSpacing.x; // Lock our horizontal range
    columns->MaxX = ImMax(content_region_width - window->Scroll.x, columns->MinX + 1.0f);
    columns->BackupCursorPosY = window->DC.CursorPos.y;
    columns->BackupCursorMaxPosX = window->DC.CursorMaxPos.x;
    columns->LineMinY = columns->LineMaxY = window->DC.CursorPos.y;
    window->DC.ColumnsOffset.x = 0.0f;
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);

    // Clear data if columns count changed
    if (columns->Columns.Size != 0 && columns->Columns.Size != columns_count + 1)
        columns->Columns.resize(0);

    // Initialize defaults
    columns->IsFirstFrame = (columns->Columns.Size == 0);
    if (columns->Columns.Size == 0)
    {
        columns->Columns.reserve(columns_count + 1);
        for (int n = 0; n < columns_count + 1; n++)
        {
            vsonyp0werColumnData column;
            column.OffsetNorm = n / (float)columns_count;
            columns->Columns.push_back(column);
        }
    }

    for (int n = 0; n < columns_count; n++)
    {
        // Compute clipping rectangle
        vsonyp0werColumnData* column = &columns->Columns[n];
        float clip_x1 = ImFloor(0.5f + window->Pos.x + GetColumnOffset(n));
        float clip_x2 = ImFloor(0.5f + window->Pos.x + GetColumnOffset(n + 1) - 1.0f);
        column->ClipRect = ImRect(clip_x1, -FLT_MAX, clip_x2, +FLT_MAX);
        column->ClipRect.ClipWith(window->ClipRect);
    }

    if (columns->Count > 1)
    {
        window->DrawList->ChannelsSplit(columns->Count);
        PushColumnClipRect();
    }
    PushItemWidth(GetColumnWidth() * 0.65f);
}

void vsonyp0wer::EndColumns()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = GetCurrentWindow();
    vsonyp0werColumns* columns = window->DC.CurrentColumns;
    IM_ASSERT(columns != NULL);

    PopItemWidth();
    if (columns->Count > 1)
    {
        PopClipRect();
        window->DrawList->ChannelsMerge();
    }

    columns->LineMaxY = ImMax(columns->LineMaxY, window->DC.CursorPos.y);
    window->DC.CursorPos.y = columns->LineMaxY;
    if (!(columns->Flags & vsonyp0werColumnsFlags_GrowParentContentsSize))
        window->DC.CursorMaxPos.x = columns->BackupCursorMaxPosX;  // Restore cursor max pos, as columns don't grow parent

    // Draw columns borders and handle resize
    bool is_being_resized = false;
    if (!(columns->Flags & vsonyp0werColumnsFlags_NoBorder) && !window->SkipItems)
    {
        // We clip Y boundaries CPU side because very long triangles are mishandled by some GPU drivers.
        const float y1 = ImMax(columns->BackupCursorPosY, window->ClipRect.Min.y);
        const float y2 = ImMin(window->DC.CursorPos.y, window->ClipRect.Max.y);
        int dragging_column = -1;
        for (int n = 1; n < columns->Count; n++)
        {
            vsonyp0werColumnData* column = &columns->Columns[n];
            float x = window->Pos.x + GetColumnOffset(n);
            const vsonyp0werID column_id = columns->ID + vsonyp0werID(n);
            const float column_hit_hw = COLUMNS_HIT_RECT_HALF_WIDTH;
            const ImRect column_hit_rect(ImVec2(x - column_hit_hw, y1), ImVec2(x + column_hit_hw, y2));
            KeepAliveID(column_id);
            if (IsClippedEx(column_hit_rect, column_id, false))
                continue;

            bool hovered = false, held = false;
            if (!(columns->Flags & vsonyp0werColumnsFlags_NoResize))
            {
                ButtonBehavior(column_hit_rect, column_id, &hovered, &held);
                if (hovered || held)
                    g.MouseCursor = vsonyp0werMouseCursor_ResizeEW;
                if (held && !(column->Flags & vsonyp0werColumnsFlags_NoResize))
                    dragging_column = n;
            }

            // Draw column
            const ImU32 col = GetColorU32(held ? vsonyp0werCol_SeparatorActive : hovered ? vsonyp0werCol_SeparatorHovered : vsonyp0werCol_Separator);
            const float xi = (float)(int)x;
            window->DrawList->AddLine(ImVec2(xi, y1 + 1.0f), ImVec2(xi, y2), col);
        }

        // Apply dragging after drawing the column lines, so our rendered lines are in sync with how items were displayed during the frame.
        if (dragging_column != -1)
        {
            if (!columns->IsBeingResized)
                for (int n = 0; n < columns->Count + 1; n++)
                    columns->Columns[n].OffsetNormBeforeResize = columns->Columns[n].OffsetNorm;
            columns->IsBeingResized = is_being_resized = true;
            float x = GetDraggedColumnOffset(columns, dragging_column);
            SetColumnOffset(dragging_column, x);
        }
    }
    columns->IsBeingResized = is_being_resized;

    window->DC.CurrentColumns = NULL;
    window->DC.ColumnsOffset.x = 0.0f;
    window->DC.CursorPos.x = (float)(int)(window->Pos.x + window->DC.Indent.x + window->DC.ColumnsOffset.x);
}

// [2018-03: This is currently the only public API, while we are working on making BeginColumns/EndColumns user-facing]
void vsonyp0wer::Columns(int columns_count, const char* id, bool border)
{
    vsonyp0werWindow* window = GetCurrentWindow();
    IM_ASSERT(columns_count >= 1);

    vsonyp0werColumnsFlags flags = (border ? 0 : vsonyp0werColumnsFlags_NoBorder);
    //flags |= vsonyp0werColumnsFlags_NoPreserveWidths; // NB: Legacy behavior
    vsonyp0werColumns * columns = window->DC.CurrentColumns;
    if (columns != NULL && columns->Count == columns_count && columns->Flags == flags)
        return;

    if (columns != NULL)
        EndColumns();

    if (columns_count != 1)
        BeginColumns(id, columns_count, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] DRAG AND DROP
//-----------------------------------------------------------------------------

void vsonyp0wer::ClearDragDrop()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.DragDropActive = false;
    g.DragDropPayload.Clear();
    g.DragDropAcceptFlags = vsonyp0werDragDropFlags_None;
    g.DragDropAcceptIdCurr = g.DragDropAcceptIdPrev = 0;
    g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
    g.DragDropAcceptFrameCount = -1;

    g.DragDropPayloadBufHeap.clear();
    memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
}

// Call when current ID is active.
// When this returns true you need to: a) call SetDragDropPayload() exactly once, b) you may render the payload visual/description, c) call EndDragDropSource()
bool vsonyp0wer::BeginDragDropSource(vsonyp0werDragDropFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    bool source_drag_active = false;
    vsonyp0werID source_id = 0;
    vsonyp0werID source_parent_id = 0;
    int mouse_button = 0;
    if (!(flags & vsonyp0werDragDropFlags_SourceExtern))
    {
        source_id = window->DC.LastItemId;
        if (source_id != 0 && g.ActiveId != source_id) // Early out for most common case
            return false;
        if (g.IO.MouseDown[mouse_button] == false)
            return false;

        if (source_id == 0)
        {
            // If you want to use BeginDragDropSource() on an item with no unique identifier for interaction, such as Text() or Image(), you need to:
            // A) Read the explanation below, B) Use the vsonyp0werDragDropFlags_SourceAllowNullID flag, C) Swallow your programmer pride.
            if (!(flags & vsonyp0werDragDropFlags_SourceAllowNullID))
            {
                IM_ASSERT(0);
                return false;
            }

            // Magic fallback (=somehow reprehensible) to handle items with no assigned ID, e.g. Text(), Image()
            // We build a throwaway ID based on current ID stack + relative AABB of items in window.
            // THE IDENTIFIER WON'T SURVIVE ANY REPOSITIONING OF THE WIDGET, so if your widget moves your dragging operation will be canceled.
            // We don't need to maintain/call ClearActiveID() as releasing the button will early out this function and trigger !ActiveIdIsAlive.
            bool is_hovered = (window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_HoveredRect) != 0;
            if (!is_hovered && (g.ActiveId == 0 || g.ActiveIdWindow != window))
                return false;
            source_id = window->DC.LastItemId = window->GetIDFromRectangle(window->DC.LastItemRect);
            if (is_hovered)
                SetHoveredID(source_id);
            if (is_hovered && g.IO.MouseClicked[mouse_button])
            {
                SetActiveID(source_id, window);
                FocusWindow(window);
            }
            if (g.ActiveId == source_id) // Allow the underlying widget to display/return hovered during the mouse release frame, else we would get a flicker.
                g.ActiveIdAllowOverlap = is_hovered;
        } else
        {
            g.ActiveIdAllowOverlap = false;
        }
        if (g.ActiveId != source_id)
            return false;
        source_parent_id = window->IDStack.back();
        source_drag_active = IsMouseDragging(mouse_button);
    } else
    {
        window = NULL;
        source_id = ImHashStr("#SourceExtern", 0);
        source_drag_active = true;
    }

    if (source_drag_active)
    {
        if (!g.DragDropActive)
        {
            IM_ASSERT(source_id != 0);
            ClearDragDrop();
            vsonyp0werPayload & payload = g.DragDropPayload;
            payload.SourceId = source_id;
            payload.SourceParentId = source_parent_id;
            g.DragDropActive = true;
            g.DragDropSourceFlags = flags;
            g.DragDropMouseButton = mouse_button;
        }
        g.DragDropSourceFrameCount = g.FrameCount;
        g.DragDropWithinSourceOrTarget = true;

        if (!(flags & vsonyp0werDragDropFlags_SourceNoPreviewTooltip))
        {
            // Target can request the Source to not display its tooltip (we use a dedicated flag to make this request explicit)
            // We unfortunately can't just modify the source flags and skip the call to BeginTooltip, as caller may be emitting contents.
            BeginTooltip();
            if (g.DragDropAcceptIdPrev && (g.DragDropAcceptFlags & vsonyp0werDragDropFlags_AcceptNoPreviewTooltip))
            {
                vsonyp0werWindow* tooltip_window = g.CurrentWindow;
                tooltip_window->SkipItems = true;
                tooltip_window->HiddenFramesCanSkipItems = 1;
            }
        }

        if (!(flags & vsonyp0werDragDropFlags_SourceNoDisableHover) && !(flags & vsonyp0werDragDropFlags_SourceExtern))
            window->DC.LastItemStatusFlags &= ~vsonyp0werItemStatusFlags_HoveredRect;

        return true;
    }
    return false;
}

void vsonyp0wer::EndDragDropSource()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.DragDropActive);
    IM_ASSERT(g.DragDropWithinSourceOrTarget && "Not after a BeginDragDropSource()?");

    if (!(g.DragDropSourceFlags & vsonyp0werDragDropFlags_SourceNoPreviewTooltip))
        EndTooltip();

    // Discard the drag if have not called SetDragDropPayload()
    if (g.DragDropPayload.DataFrameCount == -1)
        ClearDragDrop();
    g.DragDropWithinSourceOrTarget = false;
}

// Use 'cond' to choose to submit payload on drag start or every frame
bool vsonyp0wer::SetDragDropPayload(const char* type, const void* data, size_t data_size, vsonyp0werCond cond)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werPayload& payload = g.DragDropPayload;
    if (cond == 0)
        cond = vsonyp0werCond_Always;

    IM_ASSERT(type != NULL);
    IM_ASSERT(strlen(type) < IM_ARRAYSIZE(payload.DataType) && "Payload type can be at most 32 characters long");
    IM_ASSERT((data != NULL && data_size > 0) || (data == NULL && data_size == 0));
    IM_ASSERT(cond == vsonyp0werCond_Always || cond == vsonyp0werCond_Once);
    IM_ASSERT(payload.SourceId != 0);                               // Not called between BeginDragDropSource() and EndDragDropSource()

    if (cond == vsonyp0werCond_Always || payload.DataFrameCount == -1)
    {
        // Copy payload
        ImStrncpy(payload.DataType, type, IM_ARRAYSIZE(payload.DataType));
        g.DragDropPayloadBufHeap.resize(0);
        if (data_size > sizeof(g.DragDropPayloadBufLocal))
        {
            // Store in heap
            g.DragDropPayloadBufHeap.resize((int)data_size);
            payload.Data = g.DragDropPayloadBufHeap.Data;
            memcpy(payload.Data, data, data_size);
        } else if (data_size > 0)
        {
            // Store locally
            memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
            payload.Data = g.DragDropPayloadBufLocal;
            memcpy(payload.Data, data, data_size);
        } else
        {
            payload.Data = NULL;
        }
        payload.DataSize = (int)data_size;
    }
    payload.DataFrameCount = g.FrameCount;

    return (g.DragDropAcceptFrameCount == g.FrameCount) || (g.DragDropAcceptFrameCount == g.FrameCount - 1);
}

bool vsonyp0wer::BeginDragDropTargetCustom(const ImRect & bb, vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!g.DragDropActive)
        return false;

    vsonyp0werWindow* window = g.CurrentWindow;
    if (g.HoveredWindow == NULL || window->RootWindow != g.HoveredWindow->RootWindow)
        return false;
    IM_ASSERT(id != 0);
    if (!IsMouseHoveringRect(bb.Min, bb.Max) || (id == g.DragDropPayload.SourceId))
        return false;
    if (window->SkipItems)
        return false;

    IM_ASSERT(g.DragDropWithinSourceOrTarget == false);
    g.DragDropTargetRect = bb;
    g.DragDropTargetId = id;
    g.DragDropWithinSourceOrTarget = true;
    return true;
}

// We don't use BeginDragDropTargetCustom() and duplicate its code because:
// 1) we use LastItemRectHoveredRect which handles items that pushes a temporarily clip rectangle in their code. Calling BeginDragDropTargetCustom(LastItemRect) would not handle them.
// 2) and it's faster. as this code may be very frequently called, we want to early out as fast as we can.
// Also note how the HoveredWindow test is positioned differently in both functions (in both functions we optimize for the cheapest early out case)
bool vsonyp0wer::BeginDragDropTarget()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!g.DragDropActive)
        return false;

    vsonyp0werWindow* window = g.CurrentWindow;
    if (!(window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_HoveredRect))
        return false;
    if (g.HoveredWindow == NULL || window->RootWindow != g.HoveredWindow->RootWindow)
        return false;

    const ImRect & display_rect = (window->DC.LastItemStatusFlags & vsonyp0werItemStatusFlags_HasDisplayRect) ? window->DC.LastItemDisplayRect : window->DC.LastItemRect;
    vsonyp0werID id = window->DC.LastItemId;
    if (id == 0)
        id = window->GetIDFromRectangle(display_rect);
    if (g.DragDropPayload.SourceId == id)
        return false;

    IM_ASSERT(g.DragDropWithinSourceOrTarget == false);
    g.DragDropTargetRect = display_rect;
    g.DragDropTargetId = id;
    g.DragDropWithinSourceOrTarget = true;
    return true;
}

bool vsonyp0wer::IsDragDropPayloadBeingAccepted()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.DragDropActive&& g.DragDropAcceptIdPrev != 0;
}

const vsonyp0werPayload* vsonyp0wer::AcceptDragDropPayload(const char* type, vsonyp0werDragDropFlags flags)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    vsonyp0werPayload& payload = g.DragDropPayload;
    IM_ASSERT(g.DragDropActive);                        // Not called between BeginDragDropTarget() and EndDragDropTarget() ?
    IM_ASSERT(payload.DataFrameCount != -1);            // Forgot to call EndDragDropTarget() ?
    if (type != NULL && !payload.IsDataType(type))
        return NULL;

    // Accept smallest drag target bounding box, this allows us to nest drag targets conveniently without ordering constraints.
    // NB: We currently accept NULL id as target. However, overlapping targets requires a unique ID to function!
    const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
    ImRect r = g.DragDropTargetRect;
    float r_surface = r.GetWidth() * r.GetHeight();
    if (r_surface < g.DragDropAcceptIdCurrRectSurface)
    {
        g.DragDropAcceptFlags = flags;
        g.DragDropAcceptIdCurr = g.DragDropTargetId;
        g.DragDropAcceptIdCurrRectSurface = r_surface;
    }

    // Render default drop visuals
    payload.Preview = was_accepted_previously;
    flags |= (g.DragDropSourceFlags & vsonyp0werDragDropFlags_AcceptNoDrawDefaultRect); // Source can also inhibit the preview (useful for external sources that lives for 1 frame)
    if (!(flags & vsonyp0werDragDropFlags_AcceptNoDrawDefaultRect) && payload.Preview)
    {
        // FIXME-DRAG: Settle on a proper default visuals for drop target.
        r.Expand(3.5f);
        bool push_clip_rect = !window->ClipRect.Contains(r);
        if (push_clip_rect) window->DrawList->PushClipRect(r.Min - ImVec2(1, 1), r.Max + ImVec2(1, 1));
        window->DrawList->AddRect(r.Min, r.Max, GetColorU32(vsonyp0werCol_DragDropTarget), 0.0f, ~0, 2.0f);
        if (push_clip_rect) window->DrawList->PopClipRect();
    }

    g.DragDropAcceptFrameCount = g.FrameCount;
    payload.Delivery = was_accepted_previously && !IsMouseDown(g.DragDropMouseButton); // For extern drag sources affecting os window focus, it's easier to just test !IsMouseDown() instead of IsMouseReleased()
    if (!payload.Delivery && !(flags & vsonyp0werDragDropFlags_AcceptBeforeDelivery))
        return NULL;

    return &payload;
}

const vsonyp0werPayload* vsonyp0wer::GetDragDropPayload()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.DragDropActive ? &g.DragDropPayload : NULL;
}

// We don't really use/need this now, but added it for the sake of consistency and because we might need it later.
void vsonyp0wer::EndDragDropTarget()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.DragDropActive);
    IM_ASSERT(g.DragDropWithinSourceOrTarget);
    g.DragDropWithinSourceOrTarget = false;
}


//-----------------------------------------------------------------------------
// [SECTION] LOGGING/CAPTURING
//-----------------------------------------------------------------------------
// All text output from the interface can be captured into tty/file/clipboard.
// By default, tree nodes are automatically opened during logging.
//-----------------------------------------------------------------------------

// Pass text data straight to log (without being displayed)
void vsonyp0wer::LogText(const char* fmt, ...)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!g.LogEnabled)
        return;

    va_list args;
    va_start(args, fmt);
    if (g.LogFile)
        vfprintf(g.LogFile, fmt, args);
    else
        g.LogBuffer.appendfv(fmt, args);
    va_end(args);
}

// Internal version that takes a position to decide on newline placement and pad items according to their depth.
// We split text into individual lines to add current tree level padding
void vsonyp0wer::LogRenderedText(const ImVec2 * ref_pos, const char* text, const char* text_end)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;

    if (!text_end)
        text_end = FindRenderedTextEnd(text, text_end);

    const bool log_new_line = ref_pos && (ref_pos->y > g.LogLinePosY + 1);
    if (ref_pos)
        g.LogLinePosY = ref_pos->y;
    if (log_new_line)
        g.LogLineFirstItem = true;

    const char* text_remaining = text;
    if (g.LogDepthRef > window->DC.TreeDepth)  // Re-adjust padding if we have popped out of our starting depth
        g.LogDepthRef = window->DC.TreeDepth;
    const int tree_depth = (window->DC.TreeDepth - g.LogDepthRef);
    for (;;)
    {
        // Split the string. Each new line (after a '\n') is followed by spacing corresponding to the current depth of our log entry.
        // We don't add a trailing \n to allow a subsequent item on the same line to be captured.
        const char* line_start = text_remaining;
        const char* line_end = ImStreolRange(line_start, text_end);
        const bool is_first_line = (line_start == text);
        const bool is_last_line = (line_end == text_end);
        if (!is_last_line || (line_start != line_end))
        {
            const int char_count = (int)(line_end - line_start);
            if (log_new_line || !is_first_line)
                LogText(IM_NEWLINE "%*s%.*s", tree_depth * 4, "", char_count, line_start);
            else if (g.LogLineFirstItem)
                LogText("%*s%.*s", tree_depth * 4, "", char_count, line_start);
            else
                LogText(" %.*s", char_count, line_start);
            g.LogLineFirstItem = false;
        } else if (log_new_line)
        {
            // An empty "" string at a different Y position should output a carriage return.
            LogText(IM_NEWLINE);
            break;
        }

        if (is_last_line)
            break;
        text_remaining = line_end + 1;
    }
}

// Start logging/capturing text output
void vsonyp0wer::LogBegin(vsonyp0werLogType type, int auto_open_depth)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    vsonyp0werWindow* window = g.CurrentWindow;
    IM_ASSERT(g.LogEnabled == false);
    IM_ASSERT(g.LogFile == NULL);
    IM_ASSERT(g.LogBuffer.empty());
    g.LogEnabled = true;
    g.LogType = type;
    g.LogDepthRef = window->DC.TreeDepth;
    g.LogDepthToExpand = ((auto_open_depth >= 0) ? auto_open_depth : g.LogDepthToExpandDefault);
    g.LogLinePosY = FLT_MAX;
    g.LogLineFirstItem = true;
}

void vsonyp0wer::LogToTTY(int auto_open_depth)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.LogEnabled)
        return;
    LogBegin(vsonyp0werLogType_TTY, auto_open_depth);
    g.LogFile = stdout;
}

// Start logging/capturing text output to given file
void vsonyp0wer::LogToFile(int auto_open_depth, const char* filename)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.LogEnabled)
        return;

    // FIXME: We could probably open the file in text mode "at", however note that clipboard/buffer logging will still
    // be subject to outputting OS-incompatible carriage return if within strings the user doesn't use IM_NEWLINE.
    // By opening the file in binary mode "ab" we have consistent output everywhere.
    if (!filename)
        filename = g.IO.LogFilename;
    if (!filename || !filename[0])
        return;
    FILE * f = ImFileOpen(filename, "ab");
    if (f == NULL)
    {
        IM_ASSERT(0);
        return;
    }

    LogBegin(vsonyp0werLogType_File, auto_open_depth);
    g.LogFile = f;
}

// Start logging/capturing text output to clipboard
void vsonyp0wer::LogToClipboard(int auto_open_depth)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.LogEnabled)
        return;
    LogBegin(vsonyp0werLogType_Clipboard, auto_open_depth);
}

void vsonyp0wer::LogToBuffer(int auto_open_depth)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.LogEnabled)
        return;
    LogBegin(vsonyp0werLogType_Buffer, auto_open_depth);
}

void vsonyp0wer::LogFinish()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!g.LogEnabled)
        return;

    LogText(IM_NEWLINE);
    switch (g.LogType)
    {
    case vsonyp0werLogType_TTY:
        fflush(g.LogFile);
        break;
    case vsonyp0werLogType_File:
        fclose(g.LogFile);
        break;
    case vsonyp0werLogType_Buffer:
        break;
    case vsonyp0werLogType_Clipboard:
        if (!g.LogBuffer.empty())
            SetClipboardText(g.LogBuffer.begin());
        break;
    case vsonyp0werLogType_None:
        IM_ASSERT(0);
        break;
    }

    g.LogEnabled = false;
    g.LogType = vsonyp0werLogType_None;
    g.LogFile = NULL;
    g.LogBuffer.clear();
}

// Helper to display logging buttons
// FIXME-OBSOLETE: We should probably obsolete this and let the user have their own helper (this is one of the oldest function alive!)
void vsonyp0wer::LogButtons()
{
    vsonyp0werContext& g = *Gvsonyp0wer;

    PushID("LogButtons");
    const bool log_to_tty = Button("Log To TTY"); SameLine();
    const bool log_to_file = Button("Log To File"); SameLine();
    const bool log_to_clipboard = Button("Log To Clipboard"); SameLine();
    PushItemWidth(80.0f);
    PushAllowKeyboardFocus(false);
    SliderInt("Default Depth", &g.LogDepthToExpandDefault, 0, 9, NULL);
    PopAllowKeyboardFocus();
    PopItemWidth();
    PopID();

    // Start logging at the end of the function so that the buttons don't appear in the log
    if (log_to_tty)
        LogToTTY();
    if (log_to_file)
        LogToFile();
    if (log_to_clipboard)
        LogToClipboard();
}

//-----------------------------------------------------------------------------
// [SECTION] SETTINGS
//-----------------------------------------------------------------------------

void vsonyp0wer::MarkIniSettingsDirty()
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (g.SettingsDirtyTimer <= 0.0f)
        g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

void vsonyp0wer::MarkIniSettingsDirty(vsonyp0werWindow * window)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    if (!(window->Flags & vsonyp0werWindowFlags_NoSavedSettings))
        if (g.SettingsDirtyTimer <= 0.0f)
            g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

vsonyp0werWindowSettings* vsonyp0wer::CreateNewWindowSettings(const char* name)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.SettingsWindows.push_back(vsonyp0werWindowSettings());
    vsonyp0werWindowSettings* settings = &g.SettingsWindows.back();
    settings->Name = ImStrdup(name);
    settings->ID = ImHashStr(name, 0);
    return settings;
}

vsonyp0werWindowSettings* vsonyp0wer::FindWindowSettings(vsonyp0werID id)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    for (int i = 0; i != g.SettingsWindows.Size; i++)
        if (g.SettingsWindows[i].ID == id)
            return &g.SettingsWindows[i];
    return NULL;
}

vsonyp0werWindowSettings * vsonyp0wer::FindOrCreateWindowSettings(const char* name)
{
    if (vsonyp0werWindowSettings * settings = FindWindowSettings(ImHashStr(name, 0)))
        return settings;
    return CreateNewWindowSettings(name);
}

void vsonyp0wer::LoadIniSettingsFromDisk(const char* ini_filename)
{
    size_t file_data_size = 0;
    char* file_data = (char*)ImFileLoadToMemory(ini_filename, "rb", &file_data_size);
    if (!file_data)
        return;
    LoadIniSettingsFromMemory(file_data, (size_t)file_data_size);
    IM_FREE(file_data);
}

vsonyp0werSettingsHandler* vsonyp0wer::FindSettingsHandler(const char* type_name)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    const vsonyp0werID type_hash = ImHashStr(type_name, 0);
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
        if (g.SettingsHandlers[handler_n].TypeHash == type_hash)
            return &g.SettingsHandlers[handler_n];
    return NULL;
}

// Zero-tolerance, no error reporting, cheap .ini parsing
void vsonyp0wer::LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    IM_ASSERT(g.Initialized);
    IM_ASSERT(g.SettingsLoaded == false && g.FrameCount == 0);

    // For user convenience, we allow passing a non zero-terminated string (hence the ini_size parameter).
    // For our convenience and to make the code simpler, we'll also write zero-terminators within the buffer. So let's create a writable copy..
    if (ini_size == 0)
        ini_size = strlen(ini_data);
    char* buf = (char*)IM_ALLOC(ini_size + 1);
    char* buf_end = buf + ini_size;
    memcpy(buf, ini_data, ini_size);
    buf[ini_size] = 0;

    void* entry_data = NULL;
    vsonyp0werSettingsHandler * entry_handler = NULL;

    char* line_end = NULL;
    for (char* line = buf; line < buf_end; line = line_end + 1)
    {
        // Skip new lines markers, then find end of the line
        while (*line == '\n' || *line == '\r')
            line++;
        line_end = line;
        while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
            line_end++;
        line_end[0] = 0;
        if (line[0] == ';')
            continue;
        if (line[0] == '[' && line_end > line && line_end[-1] == ']')
        {
            // Parse "[Type][Name]". Note that 'Name' can itself contains [] characters, which is acceptable with the current format and parsing code.
            line_end[-1] = 0;
            const char* name_end = line_end - 1;
            const char* type_start = line + 1;
            char* type_end = (char*)(intptr_t)ImStrchrRange(type_start, name_end, ']');
            const char* name_start = type_end ? ImStrchrRange(type_end + 1, name_end, '[') : NULL;
            if (!type_end || !name_start)
            {
                name_start = type_start; // Import legacy entries that have no type
                type_start = "Window";
            } else
            {
                *type_end = 0; // Overwrite first ']'
                name_start++;  // Skip second '['
            }
            entry_handler = FindSettingsHandler(type_start);
            entry_data = entry_handler ? entry_handler->ReadOpenFn(&g, entry_handler, name_start) : NULL;
        } else if (entry_handler != NULL && entry_data != NULL)
        {
            // Let type handler parse the line
            entry_handler->ReadLineFn(&g, entry_handler, entry_data, line);
        }
    }
    IM_FREE(buf);
    g.SettingsLoaded = true;
}

void vsonyp0wer::SaveIniSettingsToDisk(const char* ini_filename)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.SettingsDirtyTimer = 0.0f;
    if (!ini_filename)
        return;

    size_t ini_data_size = 0;
    const char* ini_data = SaveIniSettingsToMemory(&ini_data_size);
    FILE* f = ImFileOpen(ini_filename, "wt");
    if (!f)
        return;
    fwrite(ini_data, sizeof(char), ini_data_size, f);
    fclose(f);
}

// Call registered handlers (e.g. SettingsHandlerWindow_WriteAll() + custom handlers) to write their stuff into a text buffer
const char* vsonyp0wer::SaveIniSettingsToMemory(size_t * out_size)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.SettingsDirtyTimer = 0.0f;
    g.SettingsIniData.Buf.resize(0);
    g.SettingsIniData.Buf.push_back(0);
    for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    {
        vsonyp0werSettingsHandler* handler = &g.SettingsHandlers[handler_n];
        handler->WriteAllFn(&g, handler, &g.SettingsIniData);
    }
    if (out_size)
        * out_size = (size_t)g.SettingsIniData.size();
    return g.SettingsIniData.c_str();
}

static void* SettingsHandlerWindow_ReadOpen(vsonyp0werContext*, vsonyp0werSettingsHandler*, const char* name)
{
    vsonyp0werWindowSettings* settings = vsonyp0wer::FindWindowSettings(ImHashStr(name, 0));
    if (!settings)
        settings = vsonyp0wer::CreateNewWindowSettings(name);
    return (void*)settings;
}

static void SettingsHandlerWindow_ReadLine(vsonyp0werContext*, vsonyp0werSettingsHandler*, void* entry, const char* line)
{
    vsonyp0werWindowSettings* settings = (vsonyp0werWindowSettings*)entry;
    float x, y;
    int i;
    if (sscanf(line, "Pos=%f,%f", &x, &y) == 2)         settings->Pos = ImVec2(x, y);
    else if (sscanf(line, "Size=%f,%f", &x, &y) == 2)   settings->Size = ImMax(ImVec2(x, y), Gvsonyp0wer->Style.WindowMinSize);
    else if (sscanf(line, "Collapsed=%d", &i) == 1)     settings->Collapsed = (i != 0);
}

static void SettingsHandlerWindow_WriteAll(vsonyp0werContext * vsonyp0wer_ctx, vsonyp0werSettingsHandler * handler, vsonyp0werTextBuffer * buf)
{
    // Gather data from windows that were active during this session
    // (if a window wasn't opened in this session we preserve its settings)
    vsonyp0werContext& g = *vsonyp0wer_ctx;
    for (int i = 0; i != g.Windows.Size; i++)
    {
        vsonyp0werWindow* window = g.Windows[i];
        if (window->Flags & vsonyp0werWindowFlags_NoSavedSettings)
            continue;

        vsonyp0werWindowSettings* settings = (window->SettingsIdx != -1) ? &g.SettingsWindows[window->SettingsIdx] : vsonyp0wer::FindWindowSettings(window->ID);
        if (!settings)
        {
            settings = vsonyp0wer::CreateNewWindowSettings(window->Name);
            window->SettingsIdx = g.SettingsWindows.index_from_ptr(settings);
        }
        IM_ASSERT(settings->ID == window->ID);
        settings->Pos = window->Pos;
        settings->Size = window->SizeFull;
        settings->Collapsed = window->Collapsed;
    }

    // Write to text buffer
    buf->reserve(buf->size() + g.SettingsWindows.Size * 96); // ballpark reserve
    for (int i = 0; i != g.SettingsWindows.Size; i++)
    {
        const vsonyp0werWindowSettings* settings = &g.SettingsWindows[i];
        if (settings->Pos.x == FLT_MAX)
            continue;
        const char* name = settings->Name;
        if (const char* p = strstr(name, "###"))  // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
            name = p;
        buf->appendf("[%s][%s]\n", handler->TypeName, name);
        buf->appendf("Pos=%d,%d\n", (int)settings->Pos.x, (int)settings->Pos.y);
        buf->appendf("Size=%d,%d\n", (int)settings->Size.x, (int)settings->Size.y);
        buf->appendf("Collapsed=%d\n", settings->Collapsed);
        buf->appendf("\n");
    }
}


//-----------------------------------------------------------------------------
// [SECTION] VIEWPORTS, PLATFORM WINDOWS
//-----------------------------------------------------------------------------

// (this section is filled in the 'docking' branch)


//-----------------------------------------------------------------------------
// [SECTION] DOCKING
//-----------------------------------------------------------------------------

// (this section is filled in the 'docking' branch)


//-----------------------------------------------------------------------------
// [SECTION] PLATFORM DEPENDENT HELPERS
//-----------------------------------------------------------------------------

#if defined(_WIN32) && !defined(_WINDOWS_) && !defined(vsonyp0wer_DISABLE_WIN32_FUNCTIONS) && (!defined(vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS) || !defined(vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS))
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef __MINGW32__
#include <Windows.h>
#else
#include <windows.h>
#endif
#endif

// Win32 API clipboard implementation
#if defined(_WIN32) && !defined(vsonyp0wer_DISABLE_WIN32_FUNCTIONS) && !defined(vsonyp0wer_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS)

#ifdef _MSC_VER
#pragma comment(lib, "user32")
#endif

static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    static ImVector<char> buf_local;
    buf_local.clear();
    if (!::OpenClipboard(NULL))
        return NULL;
    HANDLE wbuf_handle = ::GetClipboardData(CF_UNICODETEXT);
    if (wbuf_handle == NULL)
    {
        ::CloseClipboard();
        return NULL;
    }
    if (ImWchar * wbuf_global = (ImWchar*)::GlobalLock(wbuf_handle))
    {
        int buf_len = ImTextCountUtf8BytesFromStr(wbuf_global, NULL) + 1;
        buf_local.resize(buf_len);
        ImTextStrToUtf8(buf_local.Data, buf_len, wbuf_global, NULL);
    }
    ::GlobalUnlock(wbuf_handle);
    ::CloseClipboard();
    return buf_local.Data;
}

static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    if (!::OpenClipboard(NULL))
        return;
    const int wbuf_length = ImTextCountCharsFromUtf8(text, NULL) + 1;
    HGLOBAL wbuf_handle = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(ImWchar));
    if (wbuf_handle == NULL)
    {
        ::CloseClipboard();
        return;
    }
    ImWchar* wbuf_global = (ImWchar*)::GlobalLock(wbuf_handle);
    ImTextStrFromUtf8(wbuf_global, wbuf_length, text, NULL);
    ::GlobalUnlock(wbuf_handle);
    ::EmptyClipboard();
    if (::SetClipboardData(CF_UNICODETEXT, wbuf_handle) == NULL)
        ::GlobalFree(wbuf_handle);
    ::CloseClipboard();
}

#else

// Local vsonyp0wer-only clipboard implementation, if user hasn't defined better clipboard handlers
static const char* GetClipboardTextFn_DefaultImpl(void*)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    return g.PrivateClipboard.empty() ? NULL : g.PrivateClipboard.begin();
}

// Local vsonyp0wer-only clipboard implementation, if user hasn't defined better clipboard handlers
static void SetClipboardTextFn_DefaultImpl(void*, const char* text)
{
    vsonyp0werContext& g = *Gvsonyp0wer;
    g.PrivateClipboard.clear();
    const char* text_end = text + strlen(text);
    g.PrivateClipboard.resize((int)(text_end - text) + 1);
    memcpy(&g.PrivateClipboard[0], text, (size_t)(text_end - text));
    g.PrivateClipboard[(int)(text_end - text)] = 0;
}

#endif

// Win32 API IME support (for Asian languages, etc.)
#if defined(_WIN32) && !defined(__GNUC__) && !defined(vsonyp0wer_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS)

#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif

static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y)
{
    // Notify OS Input Method Editor of text input position
    if (HWND hwnd = (HWND)Gvsonyp0wer->IO.ImeWindowHandle)
        if (HIMC himc = ::ImmGetContext(hwnd))
        {
            COMPOSITIONFORM cf;
            cf.ptCurrentPos.x = x;
            cf.ptCurrentPos.y = y;
            cf.dwStyle = CFS_FORCE_POSITION;
            ::ImmSetCompositionWindow(himc, &cf);
            ::ImmReleaseContext(hwnd, himc);
        }
}

#else

static void ImeSetInputScreenPosFn_DefaultImpl(int, int) {}

#endif

//-----------------------------------------------------------------------------
// [SECTION] METRICS/DEBUG WINDOW
//-----------------------------------------------------------------------------

void vsonyp0wer::ShowMetricsWindow(bool* p_open)
{
    if (!vsonyp0wer::Begin("vsonyp0wer Metrics", p_open))
    {
        vsonyp0wer::End();
        return;
    }

    enum { RT_OuterRect, RT_OuterRectClipped, RT_InnerMainRect, RT_InnerClipRect, RT_ContentsRegionRect, RT_ContentsFullRect };
    static bool show_windows_begin_order = false;
    static bool show_windows_rects = false;
    static int  show_windows_rect_type = RT_ContentsRegionRect;
    static bool show_drawcmd_clip_rects = true;

    vsonyp0werIO& io = vsonyp0wer::GetIO();
    vsonyp0wer::Text("Dear vsonyp0wer %s", vsonyp0wer::GetVersion());
    vsonyp0wer::Text("Application avehnly %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    vsonyp0wer::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
    vsonyp0wer::Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
    vsonyp0wer::Text("%d active allocations", io.MetricsActiveAllocations);
    vsonyp0wer::Separator();

    struct Funcs
    {
        static void NodeDrawList(vsonyp0werWindow* window, ImDrawList* draw_list, const char* label)
        {
            bool node_open = vsonyp0wer::TreeNode(draw_list, "%s: '%s' %d vtx, %d indices, %d cmds", label, draw_list->_OwnerName ? draw_list->_OwnerName : "", draw_list->VtxBuffer.Size, draw_list->IdxBuffer.Size, draw_list->CmdBuffer.Size);
            if (draw_list == vsonyp0wer::GetWindowDrawList())
            {
                vsonyp0wer::SameLine();
                vsonyp0wer::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "CURRENTLY APPENDING"); // Can't display stats for active draw list! (we don't have the data double-buffered)
                if (node_open) vsonyp0wer::TreePop();
                return;
            }

            ImDrawList* fg_draw_list = GetForegroundDrawList(window); // Render additional visuals into the top-most draw list
            if (window && IsItemHovered())
                fg_draw_list->AddRect(window->Pos, window->Pos + window->Size, IM_COL32(255, 255, 0, 255));
            if (!node_open)
                return;

            int elem_offset = 0;
            for (const ImDrawCmd* pcmd = draw_list->CmdBuffer.begin(); pcmd < draw_list->CmdBuffer.end(); elem_offset += pcmd->ElemCount, pcmd++)
            {
                if (pcmd->UserCallback == NULL && pcmd->ElemCount == 0)
                    continue;
                if (pcmd->UserCallback)
                {
                    vsonyp0wer::BulletText("Callback %p, user_data %p", pcmd->UserCallback, pcmd->UserCallbackData);
                    continue;
                }
                ImDrawIdx* idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data : NULL;
                bool pcmd_node_open = vsonyp0wer::TreeNode((void*)(pcmd - draw_list->CmdBuffer.begin()), "Draw %4d %s vtx, tex 0x%p, clip_rect (%4.0f,%4.0f)-(%4.0f,%4.0f)", pcmd->ElemCount, draw_list->IdxBuffer.Size > 0 ? "indexed" : "non-indexed", pcmd->TextureId, pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w);
                if (show_drawcmd_clip_rects && fg_draw_list && vsonyp0wer::IsItemHovered())
                {
                    ImRect clip_rect = pcmd->ClipRect;
                    ImRect vtxs_rect;
                    for (int i = elem_offset; i < elem_offset + (int)pcmd->ElemCount; i++)
                        vtxs_rect.Add(draw_list->VtxBuffer[idx_buffer ? idx_buffer[i] : i].pos);
                    clip_rect.Floor(); fg_draw_list->AddRect(clip_rect.Min, clip_rect.Max, IM_COL32(255, 255, 0, 255));
                    vtxs_rect.Floor(); fg_draw_list->AddRect(vtxs_rect.Min, vtxs_rect.Max, IM_COL32(255, 0, 255, 255));
                }
                if (!pcmd_node_open)
                    continue;

                // Display individual triangles/vertices. Hover on to get the corresponding triangle highlighted.
                vsonyp0werListClipper clipper(pcmd->ElemCount / 3); // Manually coarse clip our print out of individual vertices to save CPU, only items that may be visible.
                while (clipper.Step())
                    for (int prim = clipper.DisplayStart, idx_i = elem_offset + clipper.DisplayStart * 3; prim < clipper.DisplayEnd; prim++)
                    {
                        char buf[300];
                        char* buf_p = buf, * buf_end = buf + IM_ARRAYSIZE(buf);
                        ImVec2 triangles_pos[3];
                        for (int n = 0; n < 3; n++, idx_i++)
                        {
                            int vtx_i = idx_buffer ? idx_buffer[idx_i] : idx_i;
                            ImDrawVert& v = draw_list->VtxBuffer[vtx_i];
                            triangles_pos[n] = v.pos;
                            buf_p += ImFormatString(buf_p, buf_end - buf_p, "%s %04d: pos (%8.2f,%8.2f), uv (%.6f,%.6f), col %08X\n",
                                (n == 0) ? "idx" : "   ", idx_i, v.pos.x, v.pos.y, v.uv.x, v.uv.y, v.col);
                        }
                        vsonyp0wer::Selectable(buf, false);
                        if (fg_draw_list && vsonyp0wer::IsItemHovered())
                        {
                            ImDrawListFlags backup_flags = fg_draw_list->Flags;
                            fg_draw_list->Flags &= ~ImDrawListFlags_AntiAliasedLines; // Disable AA on triangle outlines at is more readable for very large and thin triangles.
                            fg_draw_list->AddPolyline(triangles_pos, 3, IM_COL32(255, 255, 0, 255), true, 1.0f);
                            fg_draw_list->Flags = backup_flags;
                        }
                    }
                vsonyp0wer::TreePop();
            }
            vsonyp0wer::TreePop();
        }

        static void NodeColumns(const vsonyp0werColumns * columns)
        {
            if (!vsonyp0wer::TreeNode((void*)(uintptr_t)columns->ID, "Columns Id: 0x%08X, Count: %d, Flags: 0x%04X", columns->ID, columns->Count, columns->Flags))
                return;
            vsonyp0wer::BulletText("Width: %.1f (MinX: %.1f, MaxX: %.1f)", columns->MaxX - columns->MinX, columns->MinX, columns->MaxX);
            for (int column_n = 0; column_n < columns->Columns.Size; column_n++)
                vsonyp0wer::BulletText("Column %02d: OffsetNorm %.3f (= %.1f px)", column_n, columns->Columns[column_n].OffsetNorm, OffsetNormToPixels(columns, columns->Columns[column_n].OffsetNorm));
            vsonyp0wer::TreePop();
        }

        static void NodeWindows(ImVector<vsonyp0werWindow*> & windows, const char* label)
        {
            if (!vsonyp0wer::TreeNode(label, "%s (%d)", label, windows.Size))
                return;
            for (int i = 0; i < windows.Size; i++)
                Funcs::NodeWindow(windows[i], "Window");
            vsonyp0wer::TreePop();
        }

        static void NodeWindow(vsonyp0werWindow * window, const char* label)
        {
            if (!vsonyp0wer::TreeNode(window, "%s '%s', %d @ 0x%p", label, window->Name, window->Active || window->WasActive, window))
                return;
            vsonyp0werWindowFlags flags = window->Flags;
            NodeDrawList(window, window->DrawList, "DrawList");
            vsonyp0wer::BulletText("Pos: (%.1f,%.1f), Size: (%.1f,%.1f), SizeContents (%.1f,%.1f)", window->Pos.x, window->Pos.y, window->Size.x, window->Size.y, window->SizeContents.x, window->SizeContents.y);
            vsonyp0wer::BulletText("Flags: 0x%08X (%s%s%s%s%s%s%s%s%s..)", flags,
                (flags & vsonyp0werWindowFlags_ChildWindow) ? "Child " : "", (flags & vsonyp0werWindowFlags_Tooltip) ? "Tooltip " : "", (flags & vsonyp0werWindowFlags_Popup) ? "Popup " : "",
                (flags & vsonyp0werWindowFlags_Modal) ? "Modal " : "", (flags & vsonyp0werWindowFlags_ChildMenu) ? "ChildMenu " : "", (flags & vsonyp0werWindowFlags_NoSavedSettings) ? "NoSavedSettings " : "",
                (flags & vsonyp0werWindowFlags_NoMouseInputs) ? "NoMouseInputs" : "", (flags & vsonyp0werWindowFlags_NoNavInputs) ? "NoNavInputs" : "", (flags & vsonyp0werWindowFlags_AlwaysAutoResize) ? "AlwaysAutoResize" : "");
            vsonyp0wer::BulletText("Scroll: (%.2f/%.2f,%.2f/%.2f)", window->Scroll.x, GetWindowScrollMaxX(window), window->Scroll.y, GetWindowScrollMaxY(window));
            vsonyp0wer::BulletText("Active: %d/%d, WriteAccessed: %d, BeginOrderWithinContext: %d", window->Active, window->WasActive, window->WriteAccessed, (window->Active || window->WasActive) ? window->BeginOrderWithinContext : -1);
            vsonyp0wer::BulletText("Appearing: %d, Hidden: %d (CanSkip %d Cannot %d), SkipItems: %d", window->Appearing, window->Hidden, window->HiddenFramesCanSkipItems, window->HiddenFramesCannotSkipItems, window->SkipItems);
            vsonyp0wer::BulletText("NavLastIds: 0x%08X,0x%08X, NavLayerActiveMask: %X", window->NavLastIds[0], window->NavLastIds[1], window->DC.NavLayerActiveMask);
            vsonyp0wer::BulletText("NavLastChildNavWindow: %s", window->NavLastChildNavWindow ? window->NavLastChildNavWindow->Name : "NULL");
            if (!window->NavRectRel[0].IsInverted())
                vsonyp0wer::BulletText("NavRectRel[0]: (%.1f,%.1f)(%.1f,%.1f)", window->NavRectRel[0].Min.x, window->NavRectRel[0].Min.y, window->NavRectRel[0].Max.x, window->NavRectRel[0].Max.y);
            else
                vsonyp0wer::BulletText("NavRectRel[0]: <None>");
            if (window->RootWindow != window) NodeWindow(window->RootWindow, "RootWindow");
            if (window->ParentWindow != NULL) NodeWindow(window->ParentWindow, "ParentWindow");
            if (window->DC.ChildWindows.Size > 0) NodeWindows(window->DC.ChildWindows, "ChildWindows");
            if (window->ColumnsStohnly.Size > 0 && vsonyp0wer::TreeNode("Columns", "Columns sets (%d)", window->ColumnsStohnly.Size))
            {
                for (int n = 0; n < window->ColumnsStohnly.Size; n++)
                    NodeColumns(&window->ColumnsStohnly[n]);
                vsonyp0wer::TreePop();
            }
            vsonyp0wer::BulletText("Stohnly: %d bytes", window->StateStohnly.Data.Size * (int)sizeof(vsonyp0werStohnly::Pair));
            vsonyp0wer::TreePop();
        }

        static void NodeTabBar(vsonyp0werTabBar * tab_bar)
        {
            // Standalone tab bars (not associated to docking/windows functionality) currently hold no discernible strings.
            char buf[256];
            char* p = buf;
            const char* buf_end = buf + IM_ARRAYSIZE(buf);
            ImFormatString(p, buf_end - p, "TabBar (%d tabs)%s", tab_bar->Tabs.Size, (tab_bar->PrevFrameVisible < vsonyp0wer::GetFrameCount() - 2) ? " *Inactive*" : "");
            if (vsonyp0wer::TreeNode(tab_bar, "%s", buf))
            {
                for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++)
                {
                    const vsonyp0werTabItem* tab = &tab_bar->Tabs[tab_n];
                    vsonyp0wer::PushID(tab);
                    if (vsonyp0wer::SmallButton("<")) { TabBarQueueChangeTabOrder(tab_bar, tab, -1); } vsonyp0wer::SameLine(0, 2);
                    if (vsonyp0wer::SmallButton(">")) { TabBarQueueChangeTabOrder(tab_bar, tab, +1); } vsonyp0wer::SameLine();
                    vsonyp0wer::Text("%02d%c Tab 0x%08X", tab_n, (tab->ID == tab_bar->SelectedTabId) ? '*' : ' ', tab->ID);
                    vsonyp0wer::PopID();
                }
                vsonyp0wer::TreePop();
            }
        }
    };

    // Access private state, we are going to display the draw lists from last frame
    vsonyp0werContext& g = *Gvsonyp0wer;
    Funcs::NodeWindows(g.Windows, "Windows");
    if (vsonyp0wer::TreeNode("DrawList", "Active DrawLists (%d)", g.DrawDataBuilder.Layers[0].Size))
    {
        for (int i = 0; i < g.DrawDataBuilder.Layers[0].Size; i++)
            Funcs::NodeDrawList(NULL, g.DrawDataBuilder.Layers[0][i], "DrawList");
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Popups", "Popups (%d)", g.OpenPopupStack.Size))
    {
        for (int i = 0; i < g.OpenPopupStack.Size; i++)
        {
            vsonyp0werWindow* window = g.OpenPopupStack[i].Window;
            vsonyp0wer::BulletText("PopupID: %08x, Window: '%s'%s%s", g.OpenPopupStack[i].PopupId, window ? window->Name : "NULL", window && (window->Flags & vsonyp0werWindowFlags_ChildWindow) ? " ChildWindow" : "", window && (window->Flags & vsonyp0werWindowFlags_ChildMenu) ? " ChildMenu" : "");
        }
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("TabBars", "Tab Bars (%d)", g.TabBars.Data.Size))
    {
        for (int n = 0; n < g.TabBars.Data.Size; n++)
            Funcs::NodeTabBar(g.TabBars.GetByIndex(n));
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Internal state"))
    {
        const char* input_source_names[] = { "None", "Mouse", "Nav", "NavKeyboard", "NavGamepad" }; IM_ASSERT(IM_ARRAYSIZE(input_source_names) == vsonyp0werInputSource_COUNT);
        vsonyp0wer::Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
        vsonyp0wer::Text("HoveredRootWindow: '%s'", g.HoveredRootWindow ? g.HoveredRootWindow->Name : "NULL");
        vsonyp0wer::Text("HoveredId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d", g.HoveredId, g.HoveredIdPreviousFrame, g.HoveredIdTimer, g.HoveredIdAllowOverlap); // Data is "in-flight" so depending on when the Metrics window is called we may see current frame information or not
        vsonyp0wer::Text("ActiveId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d, Source: %s", g.ActiveId, g.ActiveIdPreviousFrame, g.ActiveIdTimer, g.ActiveIdAllowOverlap, input_source_names[g.ActiveIdSource]);
        vsonyp0wer::Text("ActiveIdWindow: '%s'", g.ActiveIdWindow ? g.ActiveIdWindow->Name : "NULL");
        vsonyp0wer::Text("MovingWindow: '%s'", g.MovingWindow ? g.MovingWindow->Name : "NULL");
        vsonyp0wer::Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
        vsonyp0wer::Text("NavId: 0x%08X, NavLayer: %d", g.NavId, g.NavLayer);
        vsonyp0wer::Text("NavInputSource: %s", input_source_names[g.NavInputSource]);
        vsonyp0wer::Text("NavActive: %d, NavVisible: %d", g.IO.NavActive, g.IO.NavVisible);
        vsonyp0wer::Text("NavActivateId: 0x%08X, NavInputId: 0x%08X", g.NavActivateId, g.NavInputId);
        vsonyp0wer::Text("NavDisableHighlight: %d, NavDisableMouseHover: %d", g.NavDisableHighlight, g.NavDisableMouseHover);
        vsonyp0wer::Text("NavWindowingTarget: '%s'", g.NavWindowingTarget ? g.NavWindowingTarget->Name : "NULL");
        vsonyp0wer::Text("DragDrop: %d, SourceId = 0x%08X, Payload \"%s\" (%d bytes)", g.DragDropActive, g.DragDropPayload.SourceId, g.DragDropPayload.DataType, g.DragDropPayload.DataSize);
        vsonyp0wer::TreePop();
    }

    if (vsonyp0wer::TreeNode("Tools"))
    {
        vsonyp0wer::Checkbox("Show windows begin order", &show_windows_begin_order);
        vsonyp0wer::Checkbox("Show windows rectangles", &show_windows_rects);
        vsonyp0wer::SameLine();
        vsonyp0wer::PushItemWidth(vsonyp0wer::GetFontSize() * 12);
        show_windows_rects |= vsonyp0wer::Combo("##rects_type", &show_windows_rect_type, "OuterRect\0" "OuterRectClipped\0" "InnerMainRect\0" "InnerClipRect\0" "ContentsRegionRect\0");
        vsonyp0wer::PopItemWidth();
        vsonyp0wer::Checkbox("Show clipping rectangle when hovering ImDrawCmd node", &show_drawcmd_clip_rects);
        vsonyp0wer::TreePop();
    }

    if (show_windows_rects || show_windows_begin_order)
    {
        for (int n = 0; n < g.Windows.Size; n++)
        {
            vsonyp0werWindow* window = g.Windows[n];
            if (!window->WasActive)
                continue;
            ImDrawList* draw_list = GetForegroundDrawList(window);
            if (show_windows_rects)
            {
                ImRect r;
                if (show_windows_rect_type == RT_OuterRect) { r = window->Rect(); } else if (show_windows_rect_type == RT_OuterRectClipped) { r = window->OuterRectClipped; } else if (show_windows_rect_type == RT_InnerMainRect) { r = window->InnerMainRect; } else if (show_windows_rect_type == RT_InnerClipRect) { r = window->InnerClipRect; } else if (show_windows_rect_type == RT_ContentsRegionRect) { r = window->ContentsRegionRect; }
                draw_list->AddRect(r.Min, r.Max, IM_COL32(255, 0, 128, 255));
            }
            if (show_windows_begin_order && !(window->Flags & vsonyp0werWindowFlags_ChildWindow))
            {
                char buf[32];
                ImFormatString(buf, IM_ARRAYSIZE(buf), "%d", window->BeginOrderWithinContext);
                float font_size = vsonyp0wer::GetFontSize();
                draw_list->AddRectFilled(window->Pos, window->Pos + ImVec2(font_size, font_size), IM_COL32(200, 100, 100, 255));
                draw_list->AddText(window->Pos, IM_COL32(255, 255, 255, 255), buf);
            }
        }
    }
    vsonyp0wer::End();
}

//-----------------------------------------------------------------------------

// Include vsonyp0wer_user.inl at the end of vsonyp0wer.cpp to access private data/functions that aren't exposed.
// Prefer just including vsonyp0wer_internal.h from your code rather than using this define. If a declaration is missing from vsonyp0wer_internal.h add it or request it on the github.
#ifdef vsonyp0wer_INCLUDE_vsonyp0wer_USER_INL
#include "vsonyp0wer_user.inl"
#endif

//-----------------------------------------------------------------------------



































































// Junk Code By Troll Face & Thaisen's Gen
void fRMdxaYGnsSeqQGEramAxKhyiCAMjVELiqRotbYQ47622986() {     int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4059217 = -552041443;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5829965 = -977002489;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv15350195 = -883896484;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv14288418 = -558546235;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv22507281 = -396603845;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92601501 = -391880626;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70992545 = 10636148;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv13055789 = -160579629;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv64423537 = -980502712;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50210254 = -134805834;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70352234 = 24160718;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60769173 = -228340530;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33815948 = -926962081;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91692750 = -612555616;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45463174 = -474204798;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv10262571 = -54995447;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96063423 = 14112675;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29096371 = -627868714;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77715411 = -952997116;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11519322 = -889955619;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv85944990 = -233746565;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv1126525 = -369654580;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33412806 = -888662798;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31283801 = -177150161;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv62491373 = -612546444;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33964079 = -184206227;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv49514578 = -23798920;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61533589 = -700046570;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92129745 = -736845054;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv90970046 = -892539716;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv82996532 = -988680848;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv28139803 = 11794401;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55576133 = -578331367;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42686190 = -276400256;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9910456 = -165635456;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv41328641 = -574817493;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60082947 = -487151917;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29557458 = -151517921;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv95185539 = -645340969;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv98439578 = -847133863;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5604581 = -281329587;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2921010 = 58662930;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91299353 = -492719266;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31146508 = -974104030;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv53857218 = -847405316;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45916546 = 86366836;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv93220440 = -236855834;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92337159 = -532079841;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv44266694 = -352729489;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5033820 = -236125341;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv21789546 = -956510808;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv25786845 = -470186655;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55586776 = -850698397;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv7687222 = -406783740;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61677995 = -538160770;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2932692 = -82386863;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72417158 = 11660309;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv84066393 = -606746324;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv51797044 = -945999792;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv88543202 = -112397618;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv43086923 = -268081707;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9458957 = -289317282;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv20926043 = -423734575;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv73453490 = 12037003;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv67213722 = -146124987;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42212431 = -987633684;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5193041 = -650009163;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91129758 = -550561826;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81782294 = -346920160;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4134534 = -899387305;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50179624 = -567843530;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv66505966 = -834369405;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33910831 = -982527746;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv79275832 = -5863254;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5914742 = -508626032;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv83023980 = -192409496;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9827172 = -876935315;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2266298 = -914558768;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77426582 = -329744845;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv16574828 = -598913281;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv40743638 = -947350394;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57177418 = -491719079;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17266895 = -247317081;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv87095926 = -400719713;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv69180501 = -936028908;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57209687 = -418494193;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72553027 = -137507202;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv47888911 = -71547628;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81008194 = -738239486;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6977764 = 16751407;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv68911482 = -486477803;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv76016553 = -880405593;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77760413 = -205518130;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6642338 = -432943351;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55352656 = -479052156;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96145624 = -992012305;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81994966 = -517602495;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17845863 = -404756270;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv63932786 = -727979044;    int QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11644788 = -552041443;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4059217 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5829965;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5829965 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv15350195;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv15350195 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv14288418;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv14288418 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv22507281;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv22507281 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92601501;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92601501 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70992545;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70992545 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv13055789;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv13055789 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv64423537;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv64423537 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50210254;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50210254 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70352234;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv70352234 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60769173;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60769173 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33815948;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33815948 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91692750;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91692750 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45463174;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45463174 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv10262571;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv10262571 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96063423;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96063423 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29096371;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29096371 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77715411;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77715411 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11519322;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11519322 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv85944990;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv85944990 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv1126525;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv1126525 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33412806;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33412806 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31283801;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31283801 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv62491373;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv62491373 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33964079;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33964079 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv49514578;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv49514578 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61533589;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61533589 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92129745;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92129745 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv90970046;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv90970046 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv82996532;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv82996532 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv28139803;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv28139803 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55576133;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55576133 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42686190;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42686190 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9910456;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9910456 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv41328641;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv41328641 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60082947;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv60082947 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29557458;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv29557458 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv95185539;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv95185539 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv98439578;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv98439578 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5604581;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5604581 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2921010;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2921010 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91299353;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91299353 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31146508;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv31146508 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv53857218;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv53857218 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45916546;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv45916546 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv93220440;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv93220440 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92337159;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv92337159 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv44266694;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv44266694 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5033820;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5033820 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv21789546;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv21789546 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv25786845;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv25786845 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55586776;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55586776 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv7687222;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv7687222 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61677995;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv61677995 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2932692;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2932692 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72417158;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72417158 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv84066393;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv84066393 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv51797044;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv51797044 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv88543202;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv88543202 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv43086923;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv43086923 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9458957;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9458957 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv20926043;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv20926043 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv73453490;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv73453490 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv67213722;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv67213722 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42212431;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv42212431 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5193041;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5193041 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91129758;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv91129758 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81782294;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81782294 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4134534;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4134534 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50179624;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv50179624 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv66505966;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv66505966 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33910831;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv33910831 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv79275832;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv79275832 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5914742;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv5914742 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv83023980;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv83023980 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9827172;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv9827172 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2266298;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv2266298 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77426582;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77426582 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv16574828;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv16574828 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv40743638;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv40743638 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57177418;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57177418 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17266895;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17266895 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv87095926;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv87095926 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv69180501;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv69180501 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57209687;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv57209687 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72553027;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv72553027 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv47888911;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv47888911 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81008194;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81008194 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6977764;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6977764 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv68911482;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv68911482 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv76016553;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv76016553 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77760413;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv77760413 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6642338;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv6642338 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55352656;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv55352656 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96145624;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv96145624 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81994966;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv81994966 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17845863;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv17845863 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv63932786;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv63932786 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11644788;     QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv11644788 = QxGcQaNfiqZruguVjOvvZLWSnGvAySLPCVkzDFhHoUetvNQCvwEfgcqEaopJfNhQeJoYwv4059217;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void MxInSztTXDFULTVEpIqrEjvooXGuXmqYJPWkAPkNXErWpnpkJoG11891222() {     float cRDecCTyuszvlMhLvwlDZXcTArYz59019027 = -64793670;    float cRDecCTyuszvlMhLvwlDZXcTArYz92376196 = -635743148;    float cRDecCTyuszvlMhLvwlDZXcTArYz45889513 = -192653389;    float cRDecCTyuszvlMhLvwlDZXcTArYz10689521 = -658346682;    float cRDecCTyuszvlMhLvwlDZXcTArYz52893357 = -930151673;    float cRDecCTyuszvlMhLvwlDZXcTArYz50212188 = -780835871;    float cRDecCTyuszvlMhLvwlDZXcTArYz21537380 = -180270449;    float cRDecCTyuszvlMhLvwlDZXcTArYz29432936 = 84624452;    float cRDecCTyuszvlMhLvwlDZXcTArYz29432747 = -9729932;    float cRDecCTyuszvlMhLvwlDZXcTArYz16197129 = -164643;    float cRDecCTyuszvlMhLvwlDZXcTArYz83573914 = -449299779;    float cRDecCTyuszvlMhLvwlDZXcTArYz73889712 = -903402399;    float cRDecCTyuszvlMhLvwlDZXcTArYz37361475 = 77465375;    float cRDecCTyuszvlMhLvwlDZXcTArYz27377793 = -506430733;    float cRDecCTyuszvlMhLvwlDZXcTArYz232102 = -504924997;    float cRDecCTyuszvlMhLvwlDZXcTArYz51096917 = -738774502;    float cRDecCTyuszvlMhLvwlDZXcTArYz14525070 = -602884157;    float cRDecCTyuszvlMhLvwlDZXcTArYz11999562 = -645902203;    float cRDecCTyuszvlMhLvwlDZXcTArYz28076586 = -11257910;    float cRDecCTyuszvlMhLvwlDZXcTArYz53569698 = -475183661;    float cRDecCTyuszvlMhLvwlDZXcTArYz25635592 = 10101103;    float cRDecCTyuszvlMhLvwlDZXcTArYz46580198 = -202019503;    float cRDecCTyuszvlMhLvwlDZXcTArYz38092946 = 29355360;    float cRDecCTyuszvlMhLvwlDZXcTArYz72344805 = -135579386;    float cRDecCTyuszvlMhLvwlDZXcTArYz78853294 = -352867048;    float cRDecCTyuszvlMhLvwlDZXcTArYz56671300 = 10680646;    float cRDecCTyuszvlMhLvwlDZXcTArYz73310445 = -741295006;    float cRDecCTyuszvlMhLvwlDZXcTArYz84442445 = -756072782;    float cRDecCTyuszvlMhLvwlDZXcTArYz44154104 = -975404194;    float cRDecCTyuszvlMhLvwlDZXcTArYz31287427 = -242430577;    float cRDecCTyuszvlMhLvwlDZXcTArYz30991887 = -445971223;    float cRDecCTyuszvlMhLvwlDZXcTArYz83527088 = -160529853;    float cRDecCTyuszvlMhLvwlDZXcTArYz67021469 = -580728431;    float cRDecCTyuszvlMhLvwlDZXcTArYz65019672 = -132224939;    float cRDecCTyuszvlMhLvwlDZXcTArYz80616465 = -978577101;    float cRDecCTyuszvlMhLvwlDZXcTArYz47842542 = -800889229;    float cRDecCTyuszvlMhLvwlDZXcTArYz66380783 = -122652934;    float cRDecCTyuszvlMhLvwlDZXcTArYz11421331 = -230724888;    float cRDecCTyuszvlMhLvwlDZXcTArYz93023208 = -485878982;    float cRDecCTyuszvlMhLvwlDZXcTArYz51371173 = -28307695;    float cRDecCTyuszvlMhLvwlDZXcTArYz10897342 = -307288233;    float cRDecCTyuszvlMhLvwlDZXcTArYz30274086 = -894210470;    float cRDecCTyuszvlMhLvwlDZXcTArYz84205025 = -512699892;    float cRDecCTyuszvlMhLvwlDZXcTArYz92591295 = -33430894;    float cRDecCTyuszvlMhLvwlDZXcTArYz96751494 = -800746230;    float cRDecCTyuszvlMhLvwlDZXcTArYz91887000 = -67445400;    float cRDecCTyuszvlMhLvwlDZXcTArYz5456062 = 43731450;    float cRDecCTyuszvlMhLvwlDZXcTArYz20156475 = -618133133;    float cRDecCTyuszvlMhLvwlDZXcTArYz83448952 = -430262336;    float cRDecCTyuszvlMhLvwlDZXcTArYz91065981 = -442061768;    float cRDecCTyuszvlMhLvwlDZXcTArYz23027751 = -585289357;    float cRDecCTyuszvlMhLvwlDZXcTArYz93497221 = -859150003;    float cRDecCTyuszvlMhLvwlDZXcTArYz75307635 = 57914661;    float cRDecCTyuszvlMhLvwlDZXcTArYz32696907 = -343569754;    float cRDecCTyuszvlMhLvwlDZXcTArYz86255630 = -946349751;    float cRDecCTyuszvlMhLvwlDZXcTArYz12438830 = -862774168;    float cRDecCTyuszvlMhLvwlDZXcTArYz54283251 = -565098508;    float cRDecCTyuszvlMhLvwlDZXcTArYz73544708 = 42925996;    float cRDecCTyuszvlMhLvwlDZXcTArYz31836227 = -205479634;    float cRDecCTyuszvlMhLvwlDZXcTArYz96222057 = -840832320;    float cRDecCTyuszvlMhLvwlDZXcTArYz76901743 = 60459134;    float cRDecCTyuszvlMhLvwlDZXcTArYz37094934 = -424197667;    float cRDecCTyuszvlMhLvwlDZXcTArYz85278831 = 60028646;    float cRDecCTyuszvlMhLvwlDZXcTArYz98145320 = -767299356;    float cRDecCTyuszvlMhLvwlDZXcTArYz85205241 = -554193420;    float cRDecCTyuszvlMhLvwlDZXcTArYz46827 = -188769926;    float cRDecCTyuszvlMhLvwlDZXcTArYz6868244 = -222673968;    float cRDecCTyuszvlMhLvwlDZXcTArYz72341802 = -790309687;    float cRDecCTyuszvlMhLvwlDZXcTArYz46761328 = -527853632;    float cRDecCTyuszvlMhLvwlDZXcTArYz52389559 = -704035768;    float cRDecCTyuszvlMhLvwlDZXcTArYz84716133 = -516121568;    float cRDecCTyuszvlMhLvwlDZXcTArYz3103740 = -272159269;    float cRDecCTyuszvlMhLvwlDZXcTArYz18976354 = -60023221;    float cRDecCTyuszvlMhLvwlDZXcTArYz76705412 = -982950215;    float cRDecCTyuszvlMhLvwlDZXcTArYz42672356 = -67895428;    float cRDecCTyuszvlMhLvwlDZXcTArYz95361506 = -95688428;    float cRDecCTyuszvlMhLvwlDZXcTArYz62375173 = -689319611;    float cRDecCTyuszvlMhLvwlDZXcTArYz45501651 = -937213747;    float cRDecCTyuszvlMhLvwlDZXcTArYz75593310 = -334833156;    float cRDecCTyuszvlMhLvwlDZXcTArYz86966293 = -185421649;    float cRDecCTyuszvlMhLvwlDZXcTArYz51215239 = 66949196;    float cRDecCTyuszvlMhLvwlDZXcTArYz53153971 = -23161873;    float cRDecCTyuszvlMhLvwlDZXcTArYz993494 = -225810447;    float cRDecCTyuszvlMhLvwlDZXcTArYz53088122 = -433342427;    float cRDecCTyuszvlMhLvwlDZXcTArYz8259676 = -657141220;    float cRDecCTyuszvlMhLvwlDZXcTArYz37494665 = -586821221;    float cRDecCTyuszvlMhLvwlDZXcTArYz8219453 = -118444515;    float cRDecCTyuszvlMhLvwlDZXcTArYz34324562 = -137158677;    float cRDecCTyuszvlMhLvwlDZXcTArYz78764042 = -185875188;    float cRDecCTyuszvlMhLvwlDZXcTArYz68177635 = -15802934;    float cRDecCTyuszvlMhLvwlDZXcTArYz93559291 = -135790721;    float cRDecCTyuszvlMhLvwlDZXcTArYz92836075 = -65578931;    float cRDecCTyuszvlMhLvwlDZXcTArYz79585103 = 74754746;    float cRDecCTyuszvlMhLvwlDZXcTArYz96801151 = -645046663;    float cRDecCTyuszvlMhLvwlDZXcTArYz74469430 = 11233171;    float cRDecCTyuszvlMhLvwlDZXcTArYz73802408 = -883090567;    float cRDecCTyuszvlMhLvwlDZXcTArYz44995254 = -854239117;    float cRDecCTyuszvlMhLvwlDZXcTArYz86059704 = -745400536;    float cRDecCTyuszvlMhLvwlDZXcTArYz7386054 = -479237474;    float cRDecCTyuszvlMhLvwlDZXcTArYz96704668 = -64793670;     cRDecCTyuszvlMhLvwlDZXcTArYz59019027 = cRDecCTyuszvlMhLvwlDZXcTArYz92376196;     cRDecCTyuszvlMhLvwlDZXcTArYz92376196 = cRDecCTyuszvlMhLvwlDZXcTArYz45889513;     cRDecCTyuszvlMhLvwlDZXcTArYz45889513 = cRDecCTyuszvlMhLvwlDZXcTArYz10689521;     cRDecCTyuszvlMhLvwlDZXcTArYz10689521 = cRDecCTyuszvlMhLvwlDZXcTArYz52893357;     cRDecCTyuszvlMhLvwlDZXcTArYz52893357 = cRDecCTyuszvlMhLvwlDZXcTArYz50212188;     cRDecCTyuszvlMhLvwlDZXcTArYz50212188 = cRDecCTyuszvlMhLvwlDZXcTArYz21537380;     cRDecCTyuszvlMhLvwlDZXcTArYz21537380 = cRDecCTyuszvlMhLvwlDZXcTArYz29432936;     cRDecCTyuszvlMhLvwlDZXcTArYz29432936 = cRDecCTyuszvlMhLvwlDZXcTArYz29432747;     cRDecCTyuszvlMhLvwlDZXcTArYz29432747 = cRDecCTyuszvlMhLvwlDZXcTArYz16197129;     cRDecCTyuszvlMhLvwlDZXcTArYz16197129 = cRDecCTyuszvlMhLvwlDZXcTArYz83573914;     cRDecCTyuszvlMhLvwlDZXcTArYz83573914 = cRDecCTyuszvlMhLvwlDZXcTArYz73889712;     cRDecCTyuszvlMhLvwlDZXcTArYz73889712 = cRDecCTyuszvlMhLvwlDZXcTArYz37361475;     cRDecCTyuszvlMhLvwlDZXcTArYz37361475 = cRDecCTyuszvlMhLvwlDZXcTArYz27377793;     cRDecCTyuszvlMhLvwlDZXcTArYz27377793 = cRDecCTyuszvlMhLvwlDZXcTArYz232102;     cRDecCTyuszvlMhLvwlDZXcTArYz232102 = cRDecCTyuszvlMhLvwlDZXcTArYz51096917;     cRDecCTyuszvlMhLvwlDZXcTArYz51096917 = cRDecCTyuszvlMhLvwlDZXcTArYz14525070;     cRDecCTyuszvlMhLvwlDZXcTArYz14525070 = cRDecCTyuszvlMhLvwlDZXcTArYz11999562;     cRDecCTyuszvlMhLvwlDZXcTArYz11999562 = cRDecCTyuszvlMhLvwlDZXcTArYz28076586;     cRDecCTyuszvlMhLvwlDZXcTArYz28076586 = cRDecCTyuszvlMhLvwlDZXcTArYz53569698;     cRDecCTyuszvlMhLvwlDZXcTArYz53569698 = cRDecCTyuszvlMhLvwlDZXcTArYz25635592;     cRDecCTyuszvlMhLvwlDZXcTArYz25635592 = cRDecCTyuszvlMhLvwlDZXcTArYz46580198;     cRDecCTyuszvlMhLvwlDZXcTArYz46580198 = cRDecCTyuszvlMhLvwlDZXcTArYz38092946;     cRDecCTyuszvlMhLvwlDZXcTArYz38092946 = cRDecCTyuszvlMhLvwlDZXcTArYz72344805;     cRDecCTyuszvlMhLvwlDZXcTArYz72344805 = cRDecCTyuszvlMhLvwlDZXcTArYz78853294;     cRDecCTyuszvlMhLvwlDZXcTArYz78853294 = cRDecCTyuszvlMhLvwlDZXcTArYz56671300;     cRDecCTyuszvlMhLvwlDZXcTArYz56671300 = cRDecCTyuszvlMhLvwlDZXcTArYz73310445;     cRDecCTyuszvlMhLvwlDZXcTArYz73310445 = cRDecCTyuszvlMhLvwlDZXcTArYz84442445;     cRDecCTyuszvlMhLvwlDZXcTArYz84442445 = cRDecCTyuszvlMhLvwlDZXcTArYz44154104;     cRDecCTyuszvlMhLvwlDZXcTArYz44154104 = cRDecCTyuszvlMhLvwlDZXcTArYz31287427;     cRDecCTyuszvlMhLvwlDZXcTArYz31287427 = cRDecCTyuszvlMhLvwlDZXcTArYz30991887;     cRDecCTyuszvlMhLvwlDZXcTArYz30991887 = cRDecCTyuszvlMhLvwlDZXcTArYz83527088;     cRDecCTyuszvlMhLvwlDZXcTArYz83527088 = cRDecCTyuszvlMhLvwlDZXcTArYz67021469;     cRDecCTyuszvlMhLvwlDZXcTArYz67021469 = cRDecCTyuszvlMhLvwlDZXcTArYz65019672;     cRDecCTyuszvlMhLvwlDZXcTArYz65019672 = cRDecCTyuszvlMhLvwlDZXcTArYz80616465;     cRDecCTyuszvlMhLvwlDZXcTArYz80616465 = cRDecCTyuszvlMhLvwlDZXcTArYz47842542;     cRDecCTyuszvlMhLvwlDZXcTArYz47842542 = cRDecCTyuszvlMhLvwlDZXcTArYz66380783;     cRDecCTyuszvlMhLvwlDZXcTArYz66380783 = cRDecCTyuszvlMhLvwlDZXcTArYz11421331;     cRDecCTyuszvlMhLvwlDZXcTArYz11421331 = cRDecCTyuszvlMhLvwlDZXcTArYz93023208;     cRDecCTyuszvlMhLvwlDZXcTArYz93023208 = cRDecCTyuszvlMhLvwlDZXcTArYz51371173;     cRDecCTyuszvlMhLvwlDZXcTArYz51371173 = cRDecCTyuszvlMhLvwlDZXcTArYz10897342;     cRDecCTyuszvlMhLvwlDZXcTArYz10897342 = cRDecCTyuszvlMhLvwlDZXcTArYz30274086;     cRDecCTyuszvlMhLvwlDZXcTArYz30274086 = cRDecCTyuszvlMhLvwlDZXcTArYz84205025;     cRDecCTyuszvlMhLvwlDZXcTArYz84205025 = cRDecCTyuszvlMhLvwlDZXcTArYz92591295;     cRDecCTyuszvlMhLvwlDZXcTArYz92591295 = cRDecCTyuszvlMhLvwlDZXcTArYz96751494;     cRDecCTyuszvlMhLvwlDZXcTArYz96751494 = cRDecCTyuszvlMhLvwlDZXcTArYz91887000;     cRDecCTyuszvlMhLvwlDZXcTArYz91887000 = cRDecCTyuszvlMhLvwlDZXcTArYz5456062;     cRDecCTyuszvlMhLvwlDZXcTArYz5456062 = cRDecCTyuszvlMhLvwlDZXcTArYz20156475;     cRDecCTyuszvlMhLvwlDZXcTArYz20156475 = cRDecCTyuszvlMhLvwlDZXcTArYz83448952;     cRDecCTyuszvlMhLvwlDZXcTArYz83448952 = cRDecCTyuszvlMhLvwlDZXcTArYz91065981;     cRDecCTyuszvlMhLvwlDZXcTArYz91065981 = cRDecCTyuszvlMhLvwlDZXcTArYz23027751;     cRDecCTyuszvlMhLvwlDZXcTArYz23027751 = cRDecCTyuszvlMhLvwlDZXcTArYz93497221;     cRDecCTyuszvlMhLvwlDZXcTArYz93497221 = cRDecCTyuszvlMhLvwlDZXcTArYz75307635;     cRDecCTyuszvlMhLvwlDZXcTArYz75307635 = cRDecCTyuszvlMhLvwlDZXcTArYz32696907;     cRDecCTyuszvlMhLvwlDZXcTArYz32696907 = cRDecCTyuszvlMhLvwlDZXcTArYz86255630;     cRDecCTyuszvlMhLvwlDZXcTArYz86255630 = cRDecCTyuszvlMhLvwlDZXcTArYz12438830;     cRDecCTyuszvlMhLvwlDZXcTArYz12438830 = cRDecCTyuszvlMhLvwlDZXcTArYz54283251;     cRDecCTyuszvlMhLvwlDZXcTArYz54283251 = cRDecCTyuszvlMhLvwlDZXcTArYz73544708;     cRDecCTyuszvlMhLvwlDZXcTArYz73544708 = cRDecCTyuszvlMhLvwlDZXcTArYz31836227;     cRDecCTyuszvlMhLvwlDZXcTArYz31836227 = cRDecCTyuszvlMhLvwlDZXcTArYz96222057;     cRDecCTyuszvlMhLvwlDZXcTArYz96222057 = cRDecCTyuszvlMhLvwlDZXcTArYz76901743;     cRDecCTyuszvlMhLvwlDZXcTArYz76901743 = cRDecCTyuszvlMhLvwlDZXcTArYz37094934;     cRDecCTyuszvlMhLvwlDZXcTArYz37094934 = cRDecCTyuszvlMhLvwlDZXcTArYz85278831;     cRDecCTyuszvlMhLvwlDZXcTArYz85278831 = cRDecCTyuszvlMhLvwlDZXcTArYz98145320;     cRDecCTyuszvlMhLvwlDZXcTArYz98145320 = cRDecCTyuszvlMhLvwlDZXcTArYz85205241;     cRDecCTyuszvlMhLvwlDZXcTArYz85205241 = cRDecCTyuszvlMhLvwlDZXcTArYz46827;     cRDecCTyuszvlMhLvwlDZXcTArYz46827 = cRDecCTyuszvlMhLvwlDZXcTArYz6868244;     cRDecCTyuszvlMhLvwlDZXcTArYz6868244 = cRDecCTyuszvlMhLvwlDZXcTArYz72341802;     cRDecCTyuszvlMhLvwlDZXcTArYz72341802 = cRDecCTyuszvlMhLvwlDZXcTArYz46761328;     cRDecCTyuszvlMhLvwlDZXcTArYz46761328 = cRDecCTyuszvlMhLvwlDZXcTArYz52389559;     cRDecCTyuszvlMhLvwlDZXcTArYz52389559 = cRDecCTyuszvlMhLvwlDZXcTArYz84716133;     cRDecCTyuszvlMhLvwlDZXcTArYz84716133 = cRDecCTyuszvlMhLvwlDZXcTArYz3103740;     cRDecCTyuszvlMhLvwlDZXcTArYz3103740 = cRDecCTyuszvlMhLvwlDZXcTArYz18976354;     cRDecCTyuszvlMhLvwlDZXcTArYz18976354 = cRDecCTyuszvlMhLvwlDZXcTArYz76705412;     cRDecCTyuszvlMhLvwlDZXcTArYz76705412 = cRDecCTyuszvlMhLvwlDZXcTArYz42672356;     cRDecCTyuszvlMhLvwlDZXcTArYz42672356 = cRDecCTyuszvlMhLvwlDZXcTArYz95361506;     cRDecCTyuszvlMhLvwlDZXcTArYz95361506 = cRDecCTyuszvlMhLvwlDZXcTArYz62375173;     cRDecCTyuszvlMhLvwlDZXcTArYz62375173 = cRDecCTyuszvlMhLvwlDZXcTArYz45501651;     cRDecCTyuszvlMhLvwlDZXcTArYz45501651 = cRDecCTyuszvlMhLvwlDZXcTArYz75593310;     cRDecCTyuszvlMhLvwlDZXcTArYz75593310 = cRDecCTyuszvlMhLvwlDZXcTArYz86966293;     cRDecCTyuszvlMhLvwlDZXcTArYz86966293 = cRDecCTyuszvlMhLvwlDZXcTArYz51215239;     cRDecCTyuszvlMhLvwlDZXcTArYz51215239 = cRDecCTyuszvlMhLvwlDZXcTArYz53153971;     cRDecCTyuszvlMhLvwlDZXcTArYz53153971 = cRDecCTyuszvlMhLvwlDZXcTArYz993494;     cRDecCTyuszvlMhLvwlDZXcTArYz993494 = cRDecCTyuszvlMhLvwlDZXcTArYz53088122;     cRDecCTyuszvlMhLvwlDZXcTArYz53088122 = cRDecCTyuszvlMhLvwlDZXcTArYz8259676;     cRDecCTyuszvlMhLvwlDZXcTArYz8259676 = cRDecCTyuszvlMhLvwlDZXcTArYz37494665;     cRDecCTyuszvlMhLvwlDZXcTArYz37494665 = cRDecCTyuszvlMhLvwlDZXcTArYz8219453;     cRDecCTyuszvlMhLvwlDZXcTArYz8219453 = cRDecCTyuszvlMhLvwlDZXcTArYz34324562;     cRDecCTyuszvlMhLvwlDZXcTArYz34324562 = cRDecCTyuszvlMhLvwlDZXcTArYz78764042;     cRDecCTyuszvlMhLvwlDZXcTArYz78764042 = cRDecCTyuszvlMhLvwlDZXcTArYz68177635;     cRDecCTyuszvlMhLvwlDZXcTArYz68177635 = cRDecCTyuszvlMhLvwlDZXcTArYz93559291;     cRDecCTyuszvlMhLvwlDZXcTArYz93559291 = cRDecCTyuszvlMhLvwlDZXcTArYz92836075;     cRDecCTyuszvlMhLvwlDZXcTArYz92836075 = cRDecCTyuszvlMhLvwlDZXcTArYz79585103;     cRDecCTyuszvlMhLvwlDZXcTArYz79585103 = cRDecCTyuszvlMhLvwlDZXcTArYz96801151;     cRDecCTyuszvlMhLvwlDZXcTArYz96801151 = cRDecCTyuszvlMhLvwlDZXcTArYz74469430;     cRDecCTyuszvlMhLvwlDZXcTArYz74469430 = cRDecCTyuszvlMhLvwlDZXcTArYz73802408;     cRDecCTyuszvlMhLvwlDZXcTArYz73802408 = cRDecCTyuszvlMhLvwlDZXcTArYz44995254;     cRDecCTyuszvlMhLvwlDZXcTArYz44995254 = cRDecCTyuszvlMhLvwlDZXcTArYz86059704;     cRDecCTyuszvlMhLvwlDZXcTArYz86059704 = cRDecCTyuszvlMhLvwlDZXcTArYz7386054;     cRDecCTyuszvlMhLvwlDZXcTArYz7386054 = cRDecCTyuszvlMhLvwlDZXcTArYz96704668;     cRDecCTyuszvlMhLvwlDZXcTArYz96704668 = cRDecCTyuszvlMhLvwlDZXcTArYz59019027;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fteRQztUZdZyhZjWMeqlrwdnv74636034() {     long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC32326923 = -399513634;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC78328424 = -544580711;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75211988 = -482473365;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC80854897 = -690663435;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC94385950 = 40173224;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99477198 = -712996227;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37822781 = -90340211;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29696780 = -382881865;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29739526 = -949878185;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC4192738 = -120853319;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87286768 = -534809071;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89625216 = -579422055;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC15460814 = -798609672;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89415975 = 30401920;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64814363 = -388388120;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3729719 = -957950012;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC42421104 = -779323611;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC47562479 = -409039431;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24141421 = -595817127;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73307449 = -337132686;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC70820485 = -267746783;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40745075 = -563169516;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC67984659 = -171657863;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87207984 = -701383836;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90535510 = -876122678;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11830586 = -484501415;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC77596437 = -211177557;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64374441 = -214369350;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC914453 = -554146606;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC65496652 = 17550273;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72421229 = -936771030;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24108168 = -757105435;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50325861 = -911937714;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71986266 = -831015784;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC76925449 = -537375025;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC81948276 = -58922160;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72435107 = -218888250;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30514680 = -722081223;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29348438 = -114528194;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC52745027 = -984515038;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27125609 = -23040511;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46927362 = -554406746;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91453312 = 45340603;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC95875252 = -29268793;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC17512192 = -526411308;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC93889024 = -632928321;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84876301 = -272648191;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC59538402 = -488253700;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10987592 = 3111922;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC25855814 = -702543062;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37997676 = -630226424;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC910386 = -291365185;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498042 = -4868047;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50738369 = -957726130;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3815549 = -34583717;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91581847 = -836344119;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10343766 = -272922849;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC88004004 = -781089529;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90319387 = -814540757;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82555364 = -475325362;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC21880762 = -401818671;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73448339 = -875970861;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC28782328 = -828735259;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64242873 = -867428459;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31771509 = -184082290;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC63178601 = -777703636;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC39299355 = -667484341;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC43474548 = -967593889;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC12490526 = -432223056;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82866086 = -229465960;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31294611 = -639061763;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11906424 = 42757611;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18214042 = -194511238;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71396393 = -611302090;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46181840 = -214092176;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC23893123 = -713340037;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC49291763 = -508510119;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72109406 = -42389071;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC69695792 = -74972528;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96646485 = -143194357;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC26954285 = -111853225;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18058035 = -722923858;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53386849 = -117481273;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75058639 = -851603544;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498976 = -352223303;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71510843 = -545405845;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96610126 = -652237389;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99587492 = -954211585;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC68170717 = -696432067;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC85343602 = -701030906;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71604511 = -785999312;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84431103 = -437798721;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40195293 = -907540467;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46793073 = -639202833;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30864266 = -482696368;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53677270 = -147069650;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18145035 = -725671487;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27210439 = -87230939;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64103744 = -845186504;    long cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC54333591 = -399513634;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC32326923 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC78328424;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC78328424 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75211988;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75211988 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC80854897;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC80854897 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC94385950;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC94385950 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99477198;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99477198 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37822781;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37822781 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29696780;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29696780 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29739526;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29739526 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC4192738;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC4192738 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87286768;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87286768 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89625216;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89625216 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC15460814;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC15460814 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89415975;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC89415975 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64814363;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64814363 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3729719;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3729719 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC42421104;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC42421104 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC47562479;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC47562479 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24141421;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24141421 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73307449;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73307449 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC70820485;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC70820485 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40745075;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40745075 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC67984659;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC67984659 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87207984;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC87207984 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90535510;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90535510 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11830586;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11830586 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC77596437;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC77596437 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64374441;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64374441 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC914453;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC914453 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC65496652;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC65496652 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72421229;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72421229 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24108168;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC24108168 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50325861;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50325861 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71986266;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71986266 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC76925449;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC76925449 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC81948276;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC81948276 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72435107;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72435107 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30514680;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30514680 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29348438;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC29348438 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC52745027;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC52745027 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27125609;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27125609 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46927362;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46927362 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91453312;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91453312 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC95875252;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC95875252 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC17512192;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC17512192 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC93889024;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC93889024 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84876301;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84876301 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC59538402;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC59538402 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10987592;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10987592 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC25855814;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC25855814 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37997676;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC37997676 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC910386;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC910386 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498042;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498042 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50738369;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC50738369 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3815549;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC3815549 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91581847;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC91581847 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10343766;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC10343766 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC88004004;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC88004004 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90319387;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC90319387 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82555364;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82555364 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC21880762;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC21880762 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73448339;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC73448339 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC28782328;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC28782328 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64242873;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64242873 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31771509;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31771509 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC63178601;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC63178601 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC39299355;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC39299355 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC43474548;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC43474548 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC12490526;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC12490526 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82866086;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC82866086 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31294611;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC31294611 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11906424;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC11906424 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18214042;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18214042 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71396393;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71396393 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46181840;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46181840 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC23893123;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC23893123 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC49291763;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC49291763 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72109406;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC72109406 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC69695792;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC69695792 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96646485;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96646485 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC26954285;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC26954285 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18058035;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18058035 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53386849;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53386849 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75058639;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC75058639 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498976;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27498976 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71510843;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71510843 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96610126;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC96610126 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99587492;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC99587492 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC68170717;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC68170717 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC85343602;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC85343602 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71604511;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC71604511 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84431103;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC84431103 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40195293;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC40195293 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46793073;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC46793073 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30864266;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC30864266 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53677270;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC53677270 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18145035;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC18145035 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27210439;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC27210439 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64103744;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC64103744 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC54333591;     cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC54333591 = cbWnMojwZlhwVknRquVhFYojoiIGwmjPqVoYWgNpqCIoC32326923;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void NSGgJnVxscfmHNNvyVPpjugEjMqWqfzGFBumo65957867() {     double vKRVOtKxzoXHGxHpBhTlB39635931 = -796743087;    double vKRVOtKxzoXHGxHpBhTlB77247833 = -71988571;    double vKRVOtKxzoXHGxHpBhTlB60626494 = -429665647;    double vKRVOtKxzoXHGxHpBhTlB78793894 = -769079417;    double vKRVOtKxzoXHGxHpBhTlB91089918 = -645619601;    double vKRVOtKxzoXHGxHpBhTlB81618201 = -997584471;    double vKRVOtKxzoXHGxHpBhTlB98966823 = 45338948;    double vKRVOtKxzoXHGxHpBhTlB69596860 = -380183653;    double vKRVOtKxzoXHGxHpBhTlB42640701 = -946943996;    double vKRVOtKxzoXHGxHpBhTlB51013750 = -953449632;    double vKRVOtKxzoXHGxHpBhTlB32441908 = -650641024;    double vKRVOtKxzoXHGxHpBhTlB26151782 = -609971597;    double vKRVOtKxzoXHGxHpBhTlB35513310 = -199102585;    double vKRVOtKxzoXHGxHpBhTlB49034610 = -980129058;    double vKRVOtKxzoXHGxHpBhTlB38193706 = -19901085;    double vKRVOtKxzoXHGxHpBhTlB7242553 = -495500413;    double vKRVOtKxzoXHGxHpBhTlB90259266 = -925081407;    double vKRVOtKxzoXHGxHpBhTlB11178622 = -70842836;    double vKRVOtKxzoXHGxHpBhTlB19826142 = -363495517;    double vKRVOtKxzoXHGxHpBhTlB19911488 = -705087666;    double vKRVOtKxzoXHGxHpBhTlB14961369 = -582615272;    double vKRVOtKxzoXHGxHpBhTlB69340843 = -611976302;    double vKRVOtKxzoXHGxHpBhTlB41621995 = -231310823;    double vKRVOtKxzoXHGxHpBhTlB30562523 = -794058326;    double vKRVOtKxzoXHGxHpBhTlB15956627 = -278060536;    double vKRVOtKxzoXHGxHpBhTlB6776758 = -312159298;    double vKRVOtKxzoXHGxHpBhTlB68166963 = 84073459;    double vKRVOtKxzoXHGxHpBhTlB98624093 = -977835758;    double vKRVOtKxzoXHGxHpBhTlB18599602 = -105293254;    double vKRVOtKxzoXHGxHpBhTlB48991658 = -473689618;    double vKRVOtKxzoXHGxHpBhTlB66837273 = -674782986;    double vKRVOtKxzoXHGxHpBhTlB47350361 = -984835823;    double vKRVOtKxzoXHGxHpBhTlB89703875 = -528189056;    double vKRVOtKxzoXHGxHpBhTlB8314487 = -240568187;    double vKRVOtKxzoXHGxHpBhTlB303864 = -531917910;    double vKRVOtKxzoXHGxHpBhTlB54094342 = -880665577;    double vKRVOtKxzoXHGxHpBhTlB87387775 = -826870322;    double vKRVOtKxzoXHGxHpBhTlB77272200 = -202069141;    double vKRVOtKxzoXHGxHpBhTlB96738425 = -622567725;    double vKRVOtKxzoXHGxHpBhTlB75970201 = -608780539;    double vKRVOtKxzoXHGxHpBhTlB36203910 = -314310043;    double vKRVOtKxzoXHGxHpBhTlB68196722 = -1194060;    double vKRVOtKxzoXHGxHpBhTlB30805662 = 59847268;    double vKRVOtKxzoXHGxHpBhTlB41453284 = -416052497;    double vKRVOtKxzoXHGxHpBhTlB73307407 = 80737560;    double vKRVOtKxzoXHGxHpBhTlB54925808 = -86874870;    double vKRVOtKxzoXHGxHpBhTlB11563498 = -70335571;    double vKRVOtKxzoXHGxHpBhTlB579325 = -861086683;    double vKRVOtKxzoXHGxHpBhTlB5092144 = -743421005;    double vKRVOtKxzoXHGxHpBhTlB52071177 = -816790994;    double vKRVOtKxzoXHGxHpBhTlB13206659 = -680445624;    double vKRVOtKxzoXHGxHpBhTlB9471010 = -639963065;    double vKRVOtKxzoXHGxHpBhTlB202881 = -127781674;    double vKRVOtKxzoXHGxHpBhTlB88530850 = -664867456;    double vKRVOtKxzoXHGxHpBhTlB50996498 = -447010270;    double vKRVOtKxzoXHGxHpBhTlB70295087 = -84766786;    double vKRVOtKxzoXHGxHpBhTlB35625839 = -840677748;    double vKRVOtKxzoXHGxHpBhTlB30063972 = -635607321;    double vKRVOtKxzoXHGxHpBhTlB62837268 = -391018882;    double vKRVOtKxzoXHGxHpBhTlB84313160 = -233460304;    double vKRVOtKxzoXHGxHpBhTlB13451238 = -981657931;    double vKRVOtKxzoXHGxHpBhTlB342731 = 23174705;    double vKRVOtKxzoXHGxHpBhTlB50997258 = -174890400;    double vKRVOtKxzoXHGxHpBhTlB93649043 = -373254379;    double vKRVOtKxzoXHGxHpBhTlB84176476 = -178666647;    double vKRVOtKxzoXHGxHpBhTlB85091547 = -665805201;    double vKRVOtKxzoXHGxHpBhTlB36447907 = 18217459;    double vKRVOtKxzoXHGxHpBhTlB27198823 = -958534398;    double vKRVOtKxzoXHGxHpBhTlB48730746 = -348211148;    double vKRVOtKxzoXHGxHpBhTlB84099363 = -139235508;    double vKRVOtKxzoXHGxHpBhTlB19854777 = -668630091;    double vKRVOtKxzoXHGxHpBhTlB12987066 = -623012266;    double vKRVOtKxzoXHGxHpBhTlB14440196 = -448275112;    double vKRVOtKxzoXHGxHpBhTlB43855940 = -754714978;    double vKRVOtKxzoXHGxHpBhTlB83707577 = -290777623;    double vKRVOtKxzoXHGxHpBhTlB46764647 = -481421212;    double vKRVOtKxzoXHGxHpBhTlB38535181 = -571823571;    double vKRVOtKxzoXHGxHpBhTlB168712 = -815258327;    double vKRVOtKxzoXHGxHpBhTlB57255116 = -774795887;    double vKRVOtKxzoXHGxHpBhTlB61030818 = -91185666;    double vKRVOtKxzoXHGxHpBhTlB95213259 = -141823727;    double vKRVOtKxzoXHGxHpBhTlB67587639 = -54839858;    double vKRVOtKxzoXHGxHpBhTlB93531949 = -134414753;    double vKRVOtKxzoXHGxHpBhTlB66528425 = -288502261;    double vKRVOtKxzoXHGxHpBhTlB35784999 = -793243995;    double vKRVOtKxzoXHGxHpBhTlB57366264 = 65180079;    double vKRVOtKxzoXHGxHpBhTlB47147481 = -757054150;    double vKRVOtKxzoXHGxHpBhTlB1173025 = -863321600;    double vKRVOtKxzoXHGxHpBhTlB57317988 = -793557917;    double vKRVOtKxzoXHGxHpBhTlB30008776 = -347151125;    double vKRVOtKxzoXHGxHpBhTlB18468503 = 60012171;    double vKRVOtKxzoXHGxHpBhTlB57323804 = -91263002;    double vKRVOtKxzoXHGxHpBhTlB14434933 = -811050260;    double vKRVOtKxzoXHGxHpBhTlB12425265 = -289107421;    double vKRVOtKxzoXHGxHpBhTlB62518963 = -627122608;    double vKRVOtKxzoXHGxHpBhTlB35861179 = -237484749;    double vKRVOtKxzoXHGxHpBhTlB17199464 = -826303661;    double vKRVOtKxzoXHGxHpBhTlB37156619 = -566898353;    double vKRVOtKxzoXHGxHpBhTlB57276807 = -137385851;    double vKRVOtKxzoXHGxHpBhTlB88215860 = -796743087;     vKRVOtKxzoXHGxHpBhTlB39635931 = vKRVOtKxzoXHGxHpBhTlB77247833;     vKRVOtKxzoXHGxHpBhTlB77247833 = vKRVOtKxzoXHGxHpBhTlB60626494;     vKRVOtKxzoXHGxHpBhTlB60626494 = vKRVOtKxzoXHGxHpBhTlB78793894;     vKRVOtKxzoXHGxHpBhTlB78793894 = vKRVOtKxzoXHGxHpBhTlB91089918;     vKRVOtKxzoXHGxHpBhTlB91089918 = vKRVOtKxzoXHGxHpBhTlB81618201;     vKRVOtKxzoXHGxHpBhTlB81618201 = vKRVOtKxzoXHGxHpBhTlB98966823;     vKRVOtKxzoXHGxHpBhTlB98966823 = vKRVOtKxzoXHGxHpBhTlB69596860;     vKRVOtKxzoXHGxHpBhTlB69596860 = vKRVOtKxzoXHGxHpBhTlB42640701;     vKRVOtKxzoXHGxHpBhTlB42640701 = vKRVOtKxzoXHGxHpBhTlB51013750;     vKRVOtKxzoXHGxHpBhTlB51013750 = vKRVOtKxzoXHGxHpBhTlB32441908;     vKRVOtKxzoXHGxHpBhTlB32441908 = vKRVOtKxzoXHGxHpBhTlB26151782;     vKRVOtKxzoXHGxHpBhTlB26151782 = vKRVOtKxzoXHGxHpBhTlB35513310;     vKRVOtKxzoXHGxHpBhTlB35513310 = vKRVOtKxzoXHGxHpBhTlB49034610;     vKRVOtKxzoXHGxHpBhTlB49034610 = vKRVOtKxzoXHGxHpBhTlB38193706;     vKRVOtKxzoXHGxHpBhTlB38193706 = vKRVOtKxzoXHGxHpBhTlB7242553;     vKRVOtKxzoXHGxHpBhTlB7242553 = vKRVOtKxzoXHGxHpBhTlB90259266;     vKRVOtKxzoXHGxHpBhTlB90259266 = vKRVOtKxzoXHGxHpBhTlB11178622;     vKRVOtKxzoXHGxHpBhTlB11178622 = vKRVOtKxzoXHGxHpBhTlB19826142;     vKRVOtKxzoXHGxHpBhTlB19826142 = vKRVOtKxzoXHGxHpBhTlB19911488;     vKRVOtKxzoXHGxHpBhTlB19911488 = vKRVOtKxzoXHGxHpBhTlB14961369;     vKRVOtKxzoXHGxHpBhTlB14961369 = vKRVOtKxzoXHGxHpBhTlB69340843;     vKRVOtKxzoXHGxHpBhTlB69340843 = vKRVOtKxzoXHGxHpBhTlB41621995;     vKRVOtKxzoXHGxHpBhTlB41621995 = vKRVOtKxzoXHGxHpBhTlB30562523;     vKRVOtKxzoXHGxHpBhTlB30562523 = vKRVOtKxzoXHGxHpBhTlB15956627;     vKRVOtKxzoXHGxHpBhTlB15956627 = vKRVOtKxzoXHGxHpBhTlB6776758;     vKRVOtKxzoXHGxHpBhTlB6776758 = vKRVOtKxzoXHGxHpBhTlB68166963;     vKRVOtKxzoXHGxHpBhTlB68166963 = vKRVOtKxzoXHGxHpBhTlB98624093;     vKRVOtKxzoXHGxHpBhTlB98624093 = vKRVOtKxzoXHGxHpBhTlB18599602;     vKRVOtKxzoXHGxHpBhTlB18599602 = vKRVOtKxzoXHGxHpBhTlB48991658;     vKRVOtKxzoXHGxHpBhTlB48991658 = vKRVOtKxzoXHGxHpBhTlB66837273;     vKRVOtKxzoXHGxHpBhTlB66837273 = vKRVOtKxzoXHGxHpBhTlB47350361;     vKRVOtKxzoXHGxHpBhTlB47350361 = vKRVOtKxzoXHGxHpBhTlB89703875;     vKRVOtKxzoXHGxHpBhTlB89703875 = vKRVOtKxzoXHGxHpBhTlB8314487;     vKRVOtKxzoXHGxHpBhTlB8314487 = vKRVOtKxzoXHGxHpBhTlB303864;     vKRVOtKxzoXHGxHpBhTlB303864 = vKRVOtKxzoXHGxHpBhTlB54094342;     vKRVOtKxzoXHGxHpBhTlB54094342 = vKRVOtKxzoXHGxHpBhTlB87387775;     vKRVOtKxzoXHGxHpBhTlB87387775 = vKRVOtKxzoXHGxHpBhTlB77272200;     vKRVOtKxzoXHGxHpBhTlB77272200 = vKRVOtKxzoXHGxHpBhTlB96738425;     vKRVOtKxzoXHGxHpBhTlB96738425 = vKRVOtKxzoXHGxHpBhTlB75970201;     vKRVOtKxzoXHGxHpBhTlB75970201 = vKRVOtKxzoXHGxHpBhTlB36203910;     vKRVOtKxzoXHGxHpBhTlB36203910 = vKRVOtKxzoXHGxHpBhTlB68196722;     vKRVOtKxzoXHGxHpBhTlB68196722 = vKRVOtKxzoXHGxHpBhTlB30805662;     vKRVOtKxzoXHGxHpBhTlB30805662 = vKRVOtKxzoXHGxHpBhTlB41453284;     vKRVOtKxzoXHGxHpBhTlB41453284 = vKRVOtKxzoXHGxHpBhTlB73307407;     vKRVOtKxzoXHGxHpBhTlB73307407 = vKRVOtKxzoXHGxHpBhTlB54925808;     vKRVOtKxzoXHGxHpBhTlB54925808 = vKRVOtKxzoXHGxHpBhTlB11563498;     vKRVOtKxzoXHGxHpBhTlB11563498 = vKRVOtKxzoXHGxHpBhTlB579325;     vKRVOtKxzoXHGxHpBhTlB579325 = vKRVOtKxzoXHGxHpBhTlB5092144;     vKRVOtKxzoXHGxHpBhTlB5092144 = vKRVOtKxzoXHGxHpBhTlB52071177;     vKRVOtKxzoXHGxHpBhTlB52071177 = vKRVOtKxzoXHGxHpBhTlB13206659;     vKRVOtKxzoXHGxHpBhTlB13206659 = vKRVOtKxzoXHGxHpBhTlB9471010;     vKRVOtKxzoXHGxHpBhTlB9471010 = vKRVOtKxzoXHGxHpBhTlB202881;     vKRVOtKxzoXHGxHpBhTlB202881 = vKRVOtKxzoXHGxHpBhTlB88530850;     vKRVOtKxzoXHGxHpBhTlB88530850 = vKRVOtKxzoXHGxHpBhTlB50996498;     vKRVOtKxzoXHGxHpBhTlB50996498 = vKRVOtKxzoXHGxHpBhTlB70295087;     vKRVOtKxzoXHGxHpBhTlB70295087 = vKRVOtKxzoXHGxHpBhTlB35625839;     vKRVOtKxzoXHGxHpBhTlB35625839 = vKRVOtKxzoXHGxHpBhTlB30063972;     vKRVOtKxzoXHGxHpBhTlB30063972 = vKRVOtKxzoXHGxHpBhTlB62837268;     vKRVOtKxzoXHGxHpBhTlB62837268 = vKRVOtKxzoXHGxHpBhTlB84313160;     vKRVOtKxzoXHGxHpBhTlB84313160 = vKRVOtKxzoXHGxHpBhTlB13451238;     vKRVOtKxzoXHGxHpBhTlB13451238 = vKRVOtKxzoXHGxHpBhTlB342731;     vKRVOtKxzoXHGxHpBhTlB342731 = vKRVOtKxzoXHGxHpBhTlB50997258;     vKRVOtKxzoXHGxHpBhTlB50997258 = vKRVOtKxzoXHGxHpBhTlB93649043;     vKRVOtKxzoXHGxHpBhTlB93649043 = vKRVOtKxzoXHGxHpBhTlB84176476;     vKRVOtKxzoXHGxHpBhTlB84176476 = vKRVOtKxzoXHGxHpBhTlB85091547;     vKRVOtKxzoXHGxHpBhTlB85091547 = vKRVOtKxzoXHGxHpBhTlB36447907;     vKRVOtKxzoXHGxHpBhTlB36447907 = vKRVOtKxzoXHGxHpBhTlB27198823;     vKRVOtKxzoXHGxHpBhTlB27198823 = vKRVOtKxzoXHGxHpBhTlB48730746;     vKRVOtKxzoXHGxHpBhTlB48730746 = vKRVOtKxzoXHGxHpBhTlB84099363;     vKRVOtKxzoXHGxHpBhTlB84099363 = vKRVOtKxzoXHGxHpBhTlB19854777;     vKRVOtKxzoXHGxHpBhTlB19854777 = vKRVOtKxzoXHGxHpBhTlB12987066;     vKRVOtKxzoXHGxHpBhTlB12987066 = vKRVOtKxzoXHGxHpBhTlB14440196;     vKRVOtKxzoXHGxHpBhTlB14440196 = vKRVOtKxzoXHGxHpBhTlB43855940;     vKRVOtKxzoXHGxHpBhTlB43855940 = vKRVOtKxzoXHGxHpBhTlB83707577;     vKRVOtKxzoXHGxHpBhTlB83707577 = vKRVOtKxzoXHGxHpBhTlB46764647;     vKRVOtKxzoXHGxHpBhTlB46764647 = vKRVOtKxzoXHGxHpBhTlB38535181;     vKRVOtKxzoXHGxHpBhTlB38535181 = vKRVOtKxzoXHGxHpBhTlB168712;     vKRVOtKxzoXHGxHpBhTlB168712 = vKRVOtKxzoXHGxHpBhTlB57255116;     vKRVOtKxzoXHGxHpBhTlB57255116 = vKRVOtKxzoXHGxHpBhTlB61030818;     vKRVOtKxzoXHGxHpBhTlB61030818 = vKRVOtKxzoXHGxHpBhTlB95213259;     vKRVOtKxzoXHGxHpBhTlB95213259 = vKRVOtKxzoXHGxHpBhTlB67587639;     vKRVOtKxzoXHGxHpBhTlB67587639 = vKRVOtKxzoXHGxHpBhTlB93531949;     vKRVOtKxzoXHGxHpBhTlB93531949 = vKRVOtKxzoXHGxHpBhTlB66528425;     vKRVOtKxzoXHGxHpBhTlB66528425 = vKRVOtKxzoXHGxHpBhTlB35784999;     vKRVOtKxzoXHGxHpBhTlB35784999 = vKRVOtKxzoXHGxHpBhTlB57366264;     vKRVOtKxzoXHGxHpBhTlB57366264 = vKRVOtKxzoXHGxHpBhTlB47147481;     vKRVOtKxzoXHGxHpBhTlB47147481 = vKRVOtKxzoXHGxHpBhTlB1173025;     vKRVOtKxzoXHGxHpBhTlB1173025 = vKRVOtKxzoXHGxHpBhTlB57317988;     vKRVOtKxzoXHGxHpBhTlB57317988 = vKRVOtKxzoXHGxHpBhTlB30008776;     vKRVOtKxzoXHGxHpBhTlB30008776 = vKRVOtKxzoXHGxHpBhTlB18468503;     vKRVOtKxzoXHGxHpBhTlB18468503 = vKRVOtKxzoXHGxHpBhTlB57323804;     vKRVOtKxzoXHGxHpBhTlB57323804 = vKRVOtKxzoXHGxHpBhTlB14434933;     vKRVOtKxzoXHGxHpBhTlB14434933 = vKRVOtKxzoXHGxHpBhTlB12425265;     vKRVOtKxzoXHGxHpBhTlB12425265 = vKRVOtKxzoXHGxHpBhTlB62518963;     vKRVOtKxzoXHGxHpBhTlB62518963 = vKRVOtKxzoXHGxHpBhTlB35861179;     vKRVOtKxzoXHGxHpBhTlB35861179 = vKRVOtKxzoXHGxHpBhTlB17199464;     vKRVOtKxzoXHGxHpBhTlB17199464 = vKRVOtKxzoXHGxHpBhTlB37156619;     vKRVOtKxzoXHGxHpBhTlB37156619 = vKRVOtKxzoXHGxHpBhTlB57276807;     vKRVOtKxzoXHGxHpBhTlB57276807 = vKRVOtKxzoXHGxHpBhTlB88215860;     vKRVOtKxzoXHGxHpBhTlB88215860 = vKRVOtKxzoXHGxHpBhTlB39635931;}
// Junk Finished
