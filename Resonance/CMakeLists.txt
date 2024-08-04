cmake_minimum_required (VERSION 3.24)
project(Resonance)

add_library(Resonance SHARED "main/main.cpp" "ui/ui.cpp" "input/input.cpp" "hooking/hooking.cpp" "global/include.cpp")
set_property(TARGET Resonance PROPERTY CXX_STANDARD 23)