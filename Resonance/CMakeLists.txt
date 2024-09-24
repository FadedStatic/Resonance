cmake_minimum_required (VERSION 3.24)
project(Resonance)
include_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include")
link_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64")

add_library(Resonance SHARED
        "main/main.cpp" "menu/menu.cpp" "hooking/hooking.cpp"
        console/console.cpp
        console/console.hpp
        hooking/hooking.hpp
        global/global.hpp
        invoker/invoker.hpp
        menu/menu.hpp
        update/natives.hpp
        update/update.hpp
        libs/minhook/buffer.cpp
        libs/minhook/buffer.hpp
        libs/minhook/hook.cpp
        libs/minhook/MinHook.hpp
        libs/minhook/trampoline.cpp
        libs/minhook/trampoline.hpp
        libs/minhook/hde/hde32.cpp
        libs/minhook/hde/hde32.hpp
        libs/minhook/hde/hde64.cpp
        libs/minhook/hde/hde64.hpp
        libs/minhook/hde/pstdint.hpp
        libs/minhook/hde/table32.hpp
        libs/minhook/hde/table64.hpp
        hooking/d3d11_hook/d3d11hook.hpp
        libs/imgui/imconfig.h
        libs/imgui/imgui.cpp
        libs/imgui/imgui.h
        libs/imgui/imgui_demo.cpp
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_impl_dx11.cpp
        libs/imgui/imgui_impl_dx11.h
        libs/imgui/imgui_impl_win32.cpp
        libs/imgui/imgui_impl_win32.h
        libs/imgui/imgui_internal.h
        libs/imgui/imgui_stdlib.cpp
        libs/imgui/imgui_stdlib.h
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imstb_rectpack.h
        libs/imgui/imstb_textedit.h
        libs/imgui/imstb_truetype.h
        menu/widgets/particle_widget.hpp
        global/assets.hpp
        menu/menu_handler/menu_handler.hpp
        "menu/menu_handler/menu_handler.cpp"
        menu/commands/player_commands.hpp
        menu/commands/teleport_commands.hpp
        menu/commands/misc_commands.hpp
        menu/commands/online_commands.hpp
        menu/commands/settings.hpp
        menu/commands/vehspawner_commands.hpp
        menu/commands/world_commands.hpp
        menu/namespaces.hpp
        update/globals.hpp)

target_link_libraries(Resonance d3dx11 d3d11)
set_property(TARGET Resonance PROPERTY CXX_STANDARD 23)