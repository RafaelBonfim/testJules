#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <fstream>
#include "hooks.h"
#include "minhook/include/MinHook.h"

// Function types
typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(WINAPI* DrawIndexed_t)(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);

// Pointers to original functions
Present_t oPresent = nullptr;
DrawIndexed_t oDrawIndexed = nullptr;

// Detour functions
HRESULT WINAPI detourPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool init = false;
    if (!init)
    {
        // One-time initialization code here if needed
        std::ofstream("hook.log", std::ios::app) << "Present hook called for the first time." << std::endl;
        init = true;
    }

    // Call original Present function
    return oPresent(pSwapChain, SyncInterval, Flags);
}

// --- Interactive Capture Logic ---
// We use a static variable to track the key state to only capture once per press.
static bool g_insert_was_down = false;

void CheckForCapture(ID3D11DeviceContext* pContext)
{
    if (GetAsyncKeyState(VK_INSERT) & 0x8000) // Is the INSERT key currently down?
    {
        if (!g_insert_was_down) // Was it up before?
        {
            g_insert_was_down = true; // Mark it as down

            // This is the moment we capture the shader
            ID3D11PixelShader* pPixelShader = nullptr;
            pContext->PSGetShader(&pPixelShader, NULL, 0);

            if (pPixelShader != nullptr)
            {
                std::ofstream("hook.log", std::ios::app)
                    << "SHADER CAPTURADO: " << pPixelShader
                    << std::endl;

                pPixelShader->Release();
            }
        }
    }
    else
    {
        g_insert_was_down = false; // Key has been released
    }
}
// --- End of Interactive Capture Logic ---


void WINAPI detourDrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
    // Check if the user wants to capture the current shader
    CheckForCapture(pContext);

    // Call original DrawIndexed function
    return oDrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
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
    void** pContextVTable = *(void***)pContext;

    // Get the original function pointers from the vtable
    oPresent = (Present_t)pSwapChainVTable[8];
    oDrawIndexed = (DrawIndexed_t)pContextVTable[12];

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
    if (MH_CreateHook(oDrawIndexed, &detourDrawIndexed, (void**)&oDrawIndexed) != MH_OK) {
        std::ofstream("hook.log") << "Failed to create hook for DrawIndexed." << std::endl;
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
