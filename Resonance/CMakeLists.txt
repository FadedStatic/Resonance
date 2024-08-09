cmake_minimum_required (VERSION 3.24)
project(Resonance)
include_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include")
link_directories("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64")
add_library(Resonance SHARED
        "main/main.cpp" "menu/menu.cpp" "input/input.cpp" "hooking/hooking.cpp"
        console/console.cpp
        console/console.hpp
        hooking/hooking.hpp
        global/global.hpp
        input/input.hpp
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
        hooking/d3d11_hook/d3d11hook.hpp)

target_link_libraries(Resonance d3dx11 d3d11)
set_property(TARGET Resonance PROPERTY CXX_STANDARD 23)