#pragma once
#include "../../console/console.hpp"
#include "../../libs/minhook/MinHook.hpp"
#include <Windows.h>
#include <array>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <thread>

using d3d11_present_t =  HRESULT(WINAPI*)(
  IDXGISwapChain*, UINT, UINT);

static d3d11_present_t orig_present_addr{nullptr};
static d3d11_present_t present_copy{nullptr};

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

    void hook_swapchain(std::uintptr_t* callback) {
        if(!this->has_og_present_addr) {
            console::log<log_severity::error>("Failed to retrieve game's D3D11 present function address");
            return;
        }
        if(MH_Initialize() != MH_OK) {
            console::log<log_severity::error>("Failed to initialize MinHook");
            return;
        }
        if(const auto status = MH_CreateHook(orig_present_addr, callback, reinterpret_cast<void**>(&present_copy)); status != MH_OK) {
            console::log<log_severity::error>("Failed to create D3D11 present hook: %02llX %02llX %02llX %02X", orig_present_addr, &callback, &present_copy, status);
            return;
        }
        if(MH_EnableHook(orig_present_addr) != MH_OK) {
            console::log<log_severity::error>("Failed to enable D3D11 present hook");
            return;
        }
        console::log<log_severity::success>("Hooked D3D11 present!");
    }

    ~d3d11_hook_t() {
        if(MH_DisableHook(orig_present_addr) != MH_OK) {
            console::log<log_severity::error>("Failed to disable D3D11 present hook");
            return;
        }
        if(MH_Uninitialize() != MH_OK) {
            console::log<log_severity::error>("Failed to uninitialize MinHook");
            return;
        }
        console::log<log_severity::success>("Unhooked D3D11 present!");
    }
};
