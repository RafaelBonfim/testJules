#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <fstream>
#include "hooks.h"
#include "minhook/include/MinHook.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Function types
typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc;

// Pointers to original functions
Present_t oPresent = nullptr;

// State
static bool show_menu = true;
static bool imgui_init = false;
static HWND window = NULL;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (show_menu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

// Detour functions
HRESULT WINAPI detourPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!imgui_init)
    {
        ID3D11Device* pDevice = NULL;
        ID3D11DeviceContext* pContext = NULL;
        pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
        pDevice->GetImmediateContext(&pContext);

        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        window = sd.OutputWindow;

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

        imgui_init = true;
        std::ofstream("hook.log", std::ios::app) << "ImGui initialized." << std::endl;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        show_menu = !show_menu;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (show_menu)
    {
        ImGui::Begin("ImGui Menu");
        ImGui::Text("Hello, world!");
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void Hook()
{
    // Create a dummy device and swap chain to get the vtable
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* pSwapChain;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, NULL, &pContext);

    if (FAILED(hr))
    {
        std::ofstream("hook.log") << "Failed to create D3D11 device." << std::endl;
        return;
    }

    // Get the vtable addresses
    void** pSwapChainVTable = *(void***)pSwapChain;
    oPresent = (Present_t)pSwapChainVTable[8];

    // Release the dummy device and swap chain
    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();

    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
    {
        std::ofstream("hook.log") << "MinHook initialization failed." << std::endl;
        return;
    }

    // Create hooks
    if (MH_CreateHook(oPresent, &detourPresent, (void**)&oPresent) != MH_OK) {
        std::ofstream("hook.log") << "Failed to create hook for Present." << std::endl;
        return;
    }

    // Enable hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        std::ofstream("hook.log") << "Failed to enable hooks." << std::endl;
        return;
    }

    std::ofstream("hook.log") << "Hooks created and enabled successfully." << std::endl;
}

void Unhook()
{
    std::ofstream("hook.log", std::ios::app) << "Unhooking..." << std::endl;
    if (oWndProc)
        SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

    if (imgui_init)
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    // Open a console for debugging purposes if needed
    // AllocConsole();
    // FILE* f;
    // freopen_s(&f, "CONOUT$", "w", stdout);

    std::ofstream("hook.log") << "DLL Injected. Starting hook thread." << std::endl;
    Hook();

    // Wait for an exit signal (e.g., key press) to unhook
    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        Sleep(100);
    }

    Unhook();
    // fclose(f);
    // FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}
