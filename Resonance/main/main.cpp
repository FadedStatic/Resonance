#include <iostream>
#include <thread>
#include <Windows.h>
#include "../hooking/hooking.hpp"
#include "../menu/menu.hpp"
#include "../hooking/d3d11_hook/d3d11hook.hpp"
#include "../menu/menu_handler/menu_handler.hpp"

menu_t main_menu;
menu_handler_t menu_handler{main_menu};

using scr_thread_run_t = std::uint32_t(__thiscall*)(void *self, int ops);

std::uintptr_t *orig_scr_thread_run_addr{nullptr};
std::once_flag flag;

std::uint32_t __stdcall scr_callback(void *_this, int ops) {
    std::call_once(flag, [&]() {
        /*constexpr Hash weapon_hash = -1238556825;
        constexpr auto vehicle_hash = 1093792632u;

        const auto local_ped = PLAYER::GET_PLAYER_PED(-1);
        const auto coords = ENTITY::GET_ENTITY_COORDS(local_ped, true);
        console::log<log_severity::info>("Player entity coordinates: { X: %f, Y: %f, Z: %f } ", coords.x, coords.y, coords.z);

        const auto local_player = PLAYER::GET_PLAYER_INDEX();
        PLAYER::SET_PLAYER_INVINCIBLE(local_player, true);
        PLAYER::SET_PLAYER_WANTED_LEVEL(local_player, 0, false);
        PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(local_player, false);
        WEAPON::GIVE_WEAPON_TO_PED(local_ped, weapon_hash, 9999999, false, true);*/

    });

    menu_handler.handle_inputs();
    return reinterpret_cast<scr_thread_run_t>(orig_scr_thread_run_addr)(_this, ops);
}

static void __stdcall dx_callback(IDXGISwapChain *swap_chain, UINT rdx, UINT r9) {
    main_menu.render(swap_chain);
    present_copy(swap_chain, rdx, r9);
}

void main(HMODULE dll) {
    console console{};
    console::log<log_severity::info>("Base address: %llX", global::base);
    const auto threads = at_array_t<scrThread *>(
        *reinterpret_cast<at_array_t<scrThread *> *>(global::base + global::native_resolver::sm_threads));

    scrThread *persistent_thread{nullptr};

    for (const auto &thread: threads) {
        if (thread->m_script_hash == 0x5700179C) {
            console::log<log_severity::success>("Main Persistent Thread Hash: %X, Thread ID: %X, scrThread address: %p",
                                                thread->m_script_hash, thread->m_thread_id, thread);
            persistent_thread = thread;
            break;
        }
        console::log<log_severity::info>("Thread hash: %X", thread->m_script_hash);
    }
    d3d11_hook_t dxhk{};
    dxhk.hook_swapchain(reinterpret_cast<std::uintptr_t *>(&dx_callback));
    hk_scr_thread_run_t hk{
        persistent_thread, reinterpret_cast<std::uintptr_t *>(&scr_callback), orig_scr_thread_run_addr
    };
    while (!menu_t::menu_exit) {
    }
    FreeConsole();
    FreeLibrary(dll);
}

int __stdcall DllMain(HMODULE calling_module, unsigned long reason_for_call, void *reserved) {
    switch (reason_for_call) {
        case DLL_PROCESS_ATTACH:
            std::thread{main, calling_module}.detach();
            break;
    }

    return 1;
}
