cmake_minimum_required (VERSION 3.24)
project(Resonance)
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
        hooking/d3d11_hook/d3d11_hook.hpp)

find_library(D3D11_LIBRARY d3d11.lib     PATHS     "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64"     DOC "The directory where d3d11.lib resides")
find_library(D3DX11_LIBRARY d3dx11.lib     PATHS     "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64"     DOC "The directory where d3dx11.lib resides")
find_library(MinHook MinHook.x64.lib     PATHS     "libs\\minhook"     DOC "MinHook lib")

target_link_libraries(Resonance "${D3D11_LIBRARY}" "${D3DX11_LIBRARY}" "${MinHook}")
set_property(TARGET Resonance PROPERTY CXX_STANDARD 23)