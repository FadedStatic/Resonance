#include "menu_handler.hpp"

bool menu_handler_t::get_input_just_pressed(int key, bool from_keyboard) {
    return from_keyboard ? (GetAsyncKeyState(key) & 0xFFFF) == 0x8001 : false;
}

void menu_handler_t::handle_inputs() {
    if (get_input_just_pressed(VK_F9))
        global::menu::menu_open = !global::menu::menu_open;

    if (get_input_just_pressed(VK_END))
        global::menu::menu_exit = true;

    if (!INTERNAL_menu_initialized)
        return;

    if (global::menu::menu_open)
        disable_inputs();

    const auto& where_vec = global::menu::menu_indexes.get();
    auto&& menus = global::menu::submenus.get();
    int  curr_depth = 0;
    auto&& indexed_menu = menus;
    while (curr_depth++ < static_cast<int>(where_vec.size()) - 2)
    {
        indexed_menu = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[where_vec[curr_depth]])->options;
    }

    auto local_idx = global::menu::menu_indexes.at(global::menu::menu_indexes.size() - 1);

    if (get_input_just_pressed(VK_DOWN))
        global::menu::menu_indexes.set_back(
            local_idx + 2 > indexed_menu.size() ? 0 : local_idx + 1
        );
    else if (get_input_just_pressed(VK_UP))
        global::menu::menu_indexes.set_back(
            local_idx == 0 ? indexed_menu.size() - 1 : local_idx - 1
        );
    else if (get_input_just_pressed(VK_RETURN)) {
        if (const auto derived = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[local_idx]); derived != nullptr)
            global::menu::menu_indexes.push_back(0);
        else if(indexed_menu[local_idx]->cb)
            indexed_menu[local_idx]->cb(indexed_menu[local_idx]);
    }
    else if (get_input_just_pressed(VK_BACK))
    {
        if (where_vec.size() > 1)
            global::menu::menu_indexes.pop_back();
    }
}

void menu_handler_t::disable_inputs() {
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_ACCEPT);
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_CANCEL);
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_DOWN);
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_UP);
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_LEFT);
    PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_RIGHT);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_SELECT_NEXT_WEAPON, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_RIGHT, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_DOWN, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_LEFT, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_UP, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_ARREST, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_CONTEXT, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_BLOCK, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_UP, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_HUD_SPECIAL, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_CIN_CAM, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_VEH_CIN_CAM, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_HEADLIGHT, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_LEFT, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_DOWN, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RDOWN, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RIGHT, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_CHARACTER_WHEEL, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_RADIO_WHEEL, true);
    PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_CANCEL, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_LIGHT, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_HEAVY, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_TREVOR, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MICHAEL, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_FRANKLIN, true);
    PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, true);
}
