// dear vsonyp0wer: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core vsonyp0wer)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= vsonyp0werConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. vsonyp0wer::IsKeyPressed(VK_SPACE).
// Missing features:
//  [ ] Platform: Gamepad support (best leaving it to user application to fill io.NavInputs[] with gamepad inputs from their source of choice).

#pragma once

vsonyp0wer_IMPL_API bool     vsonyp0wer_ImplWin32_Init(void* hwnd);
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplWin32_Shutdown();
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplWin32_NewFrame();

// Handler for Win32 messages, update mouse/keyboard data.
// You may or not need this for your implementation, but it can serve as reference for handling inputs.
// Intentionally commented out to avoid dragging dependencies on <windows.h> types. You can copy the extern declaration in your code.
/*
vsonyp0wer_IMPL_API LRESULT  vsonyp0wer_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/
