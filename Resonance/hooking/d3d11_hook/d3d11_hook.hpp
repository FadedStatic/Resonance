#pragma once
#include "../../console/console.hpp"
#include "../../libs/minhook/MinHook.h"
#include <Windows.h>
#include <array>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


using d3d11_present_t =  HRESULT(WINAPI*)(
  IDXGISwapChain*, UINT, UINT);

constexpr auto hk_sz = 12;
static std::uint8_t orig_bytes[hk_sz]{ 0 };

static unsigned long oldpf{0};
static ID3D11Device* p_device{ nullptr };
static d3d11_present_t orig_present_addr{nullptr};

static d3d11_present_t present_copy{nullptr};

static void __stdcall stub(IDXGISwapChain* swap_chain, UINT rdx, UINT r9) {
    const auto res = swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&p_device));
    if(FAILED(res))
        p_device = nullptr;
    present_copy(swap_chain, rdx, r9);
}


class d3d11_hook_t {
    bool has_og_present_addr{ false };
public:
    d3d11_hook_t() {
        // Create a dummy device, get swapchain vmt, hook present.
        D3D_FEATURE_LEVEL featLevel;
        // can use designated initializers but it's worth work that i just dont give enough of a fuck about
        DXGI_SWAP_CHAIN_DESC sd{ 0 };
        sd.BufferCount = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.Height = 800;
        sd.BufferDesc.Width = 600;
        sd.BufferDesc.RefreshRate = { 60, 1 };
        sd.OutputWindow = GetForegroundWindow();
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        ID3D11Device* p_device{ nullptr };
        IDXGISwapChain* p_swapchain{ nullptr };

        HRESULT hr = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &p_swapchain, &p_device, &featLevel, nullptr );
        if (FAILED( hr )) {
            console::log<log_severity::error>("Failed to create device & swapchain");
        } else {
            void** vmt = *reinterpret_cast<void***>(p_swapchain);
            constexpr auto present_index = 8;
            orig_present_addr = static_cast<d3d11_present_t>(vmt[present_index]);
            this->has_og_present_addr = true;
            p_swapchain->Release();
            p_device->Release();
        }

    }

    ID3D11Device* hook() {
        if(!this->has_og_present_addr) {
            console::log<log_severity::error>("Failed to retrieve game's D3D11 present function address");
            return nullptr;
        }
        if(MH_Initialize() != MH_OK) {
            console::log<log_severity::error>("Failed to initialize MinHook");
            return nullptr;
        }
        if(MH_CreateHook(orig_present_addr, &stub, reinterpret_cast<void**>(&present_copy)) != MH_OK) {
            console::log<log_severity::error>("Failed to create D3D11 present hook");
            return nullptr;
        }
        if(MH_EnableHook(orig_present_addr) != MH_OK) {
            console::log<log_severity::error>("Failed to enable D3D11 present hook");
            return nullptr;
        }
        console::log<log_severity::success>("Hooked D3D11 present!");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        if(MH_DisableHook(orig_present_addr) != MH_OK) {
            console::log<log_severity::error>("Failed to disable D3D11 present hook");
            return nullptr;
        }
        if(MH_Uninitialize() != MH_OK) {
            console::log<log_severity::error>("Failed to uninitialize MinHook");
            return nullptr;
        }
        console::log<log_severity::success>("Unhooked D3D11 present!");
        console::log<log_severity::success>("Retrieved game's D3D11 device");
        return p_device;
    }



};




