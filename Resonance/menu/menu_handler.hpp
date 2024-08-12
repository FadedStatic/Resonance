#pragma once
#include "../global/global.hpp"
#include "../update/natives.hpp"
#include <vector>
#include <Windows.h>

// The purpose of this class is to modify global variables and call natives on every execution.
class menu_handler_t {
public:

    [[nodiscard]] static bool get_input_just_pressed(int key, bool from_keyboard=true);
    void handle_inputs();
    void disable_inputs();
};