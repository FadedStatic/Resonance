#include "menu_handler.hpp"

bool menu_handler_t::get_input_just_pressed(int key, bool from_keyboard) {
    return from_keyboard ? (GetAsyncKeyState(key) & 0xFFFF) == 0x8001 : false;
}

void menu_handler_t::handle_inputs() {
    if (get_input_just_pressed(VK_F9))
        menu_t::menu_open = !menu_t::menu_open;

    if (get_input_just_pressed(VK_END))
        menu_t::menu_exit = true;

    if (!INTERNAL_menu_initialized)
        return;

    if (menu_t::menu_open)
        disable_inputs();
    else return;
   

    const auto& where_vec = this->menu.menu_indexes;
    auto indexed_menu = this->menu.submenus;
    for (std::size_t i = 1; i < where_vec.size(); i++)
        if (const auto thing = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[where_vec[i - 1]]); thing!=nullptr)
            indexed_menu = thing->options;

    auto local_idx = this->menu.menu_indexes.at(this->menu.menu_indexes.size() - 1);
    
    if (get_input_just_pressed(VK_DOWN))
    {
    	const auto new_idx = local_idx + 2 > indexed_menu.size() ? 0 : local_idx + 1;
        this->menu.menu_indexes[this->menu.menu_indexes.size() - 1] = new_idx;
        indexed_menu.at(local_idx)->selected = false;
        indexed_menu.at(new_idx)->selected = true;
	}
    else if (get_input_just_pressed(VK_UP)) {
        const auto new_idx = local_idx == 0 ? indexed_menu.size() - 1 : local_idx - 1;
        this->menu.menu_indexes[this->menu.menu_indexes.size() - 1] = new_idx;

        indexed_menu.at(local_idx)->selected = false;
        indexed_menu.at(new_idx)->selected = true;

    }
    else if (get_input_just_pressed(VK_RETURN)) {
        if (const auto derived = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[local_idx]); derived != nullptr) {
            this->menu.menu_indexes.push_back(0);
            derived->options.at(0)->selected = true;
        }
        else if (indexed_menu[local_idx]->cb) {
            indexed_menu[local_idx]->cb(indexed_menu[local_idx]);
            return;
        }
        else
        {
            return;
        }
    }
    else if (get_input_just_pressed(VK_BACK))
    {
        if (this->menu.menu_indexes.size() > 1)
            this->menu.menu_indexes.pop_back();
        indexed_menu[local_idx]->selected = false;
    }
    else
    {
        return;
    }
    global::menu::move_circles = true;
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
