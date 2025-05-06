#include "stdafx.h"
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>
#include <exception>
#include <FunctionHook.h>
#include <d3d9.h>

static void ImGuiMenu() {
    
}

// imgui implementation below, keep cwe logic above here

extern IDirect3DDevice9* cwe_device;

FunctionHook<void> BeginScene(0x00424740);
FunctionHook<void> FuncBeforeEndScene(0x44C260);
StdcallFunctionHook<LRESULT, HWND, UINT, WPARAM, LPARAM> WndProcFunc(0x00401810);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return WndProcFunc.Original(hWnd, uMsg, wParam, lParam);
}

static void ImGuiDraw() {
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiMenu();

    ImGui::EndFrame();
}

static void BeginSceneHook() {
    ImGuiDraw();
    BeginScene.Original();
}

static void BeforeEndSceneHook() {
    FuncBeforeEndScene.Original();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Init() {
    BeginScene.Hook(BeginSceneHook);
    FuncBeforeEndScene.Hook(BeforeEndSceneHook);
    WndProcFunc.Hook(WndProcHook);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(*(HWND*)0x01933EA8);
	ImGui_ImplDX9_Init(cwe_device);
}