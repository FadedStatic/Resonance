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
        update/update.hpp)
set_property(TARGET Resonance PROPERTY CXX_STANDARD 23)