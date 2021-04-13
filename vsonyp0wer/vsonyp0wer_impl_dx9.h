// dear vsonyp0wer: Renderer for DirectX9
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID in vsonyp0wer.cpp.

// You can copy and use unmodified vsonyp0wer_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear vsonyp0wer, read examples/README.txt and read the documentation at the top of vsonyp0wer.cpp.
// https://github.com/ocornut/vsonyp0wer

#pragma once

struct IDirect3DDevice9;

vsonyp0wer_IMPL_API bool     vsonyp0wer_ImplDX9_Init(IDirect3DDevice9* device);
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplDX9_Shutdown();
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplDX9_NewFrame();
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplDX9_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing vsonyp0wer state.
vsonyp0wer_IMPL_API void     vsonyp0wer_ImplDX9_InvalidateDeviceObjects();
vsonyp0wer_IMPL_API bool     vsonyp0wer_ImplDX9_CreateDeviceObjects();
