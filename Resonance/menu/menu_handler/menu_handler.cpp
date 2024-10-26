#include "menu_handler.hpp"

bool menu_handler_t::get_input_just_pressed(int key, bool from_keyboard) {
    return from_keyboard ? (GetAsyncKeyState(key) & 0xFFFF) == 0x8001 : false;
}

void menu_handler_t::handle_inputs() {
    if (get_input_just_pressed(VK_F9))
        menu_t::menu_open = !menu_t::menu_open;

    if (get_input_just_pressed(VK_END))
        menu_t::menu_exit = true;

    for (const auto &[first,second]: jobs)
        if (second)
            first->recur_cb(first);

    if (!INTERNAL_menu_initialized)
        return;

    if (menu_t::menu_open)
        disable_inputs();
    else return;

    BYTE keyState[256];
    const auto &where_vec = this->menu.menu_indexes;
    auto indexed_menu = this->menu.submenus;
    for (std::size_t i = 1; i < where_vec.size(); i++)
        if (const auto thing = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[where_vec[i - 1].first]);
            thing != nullptr)
            indexed_menu = thing->options;

    auto local_idx = this->menu.menu_indexes.at(this->menu.menu_indexes.size() - 1).first;

    if (get_input_just_pressed(VK_DOWN)) {
        const auto new_idx = local_idx + 2 > indexed_menu.size() ? 0 : local_idx + 1;
        if (!new_idx)
            this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second = 0;
        else if ((local_idx + 1 - this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second) > 7)
            this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second++;
        this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].first = new_idx;
        indexed_menu.at(local_idx)->selected = false;
        indexed_menu.at(new_idx)->selected = true;
    } else if (get_input_just_pressed(VK_UP)) {
        const auto new_idx = local_idx == 0 ? indexed_menu.size() - 1 : local_idx - 1;
        if (!local_idx)
            this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second = new_idx-7;
        else if(local_idx == this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second) // this logic is correct and i have no fucking idea why
            this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].second--;

        this->menu.menu_indexes[this->menu.menu_indexes.size() - 1].first = new_idx;

        indexed_menu.at(local_idx)->selected = false;
        indexed_menu.at(new_idx)->selected = true;
    } else if (get_input_just_pressed(VK_RETURN)) {
        if (const auto derived = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[local_idx]);
            derived != nullptr) {
            this->menu.menu_indexes.push_back({0, 0});
            derived->options.at(0)->selected = true;
        } else if (const auto checkbox_derived = std::dynamic_pointer_cast<
            checkbox_menu_option_t>(indexed_menu[local_idx]); checkbox_derived != nullptr) {
            if (checkbox_derived->recur_cb)
                jobs.insert_or_assign(checkbox_derived, !checkbox_derived->checked);

            if (checkbox_derived->cb)
                checkbox_derived->cb(checkbox_derived);

            checkbox_derived->checked = !checkbox_derived->checked;
            return;
        } else if (indexed_menu[local_idx]->cb) {
            indexed_menu[local_idx]->cb(indexed_menu[local_idx]);
            return;
        } else {
            return;
        }
    } else if (get_input_just_pressed(VK_LEFT)) {
        if (const auto derived = std::dynamic_pointer_cast<multi_option_t>(indexed_menu[local_idx]); derived != nullptr)
            if ((derived->idx - 1) >= 0)
                derived->idx--;
    } else if (get_input_just_pressed(VK_RIGHT) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, VK_RIGHT)) {
        if (const auto derived = std::dynamic_pointer_cast<multi_option_t>(indexed_menu[local_idx]); derived != nullptr)
            if ((derived->idx + 1) < derived->options.size())
                derived->idx++;
    } else if (get_input_just_pressed(VK_BACK)) {
        if (this->menu.menu_indexes.size() > 1)
            this->menu.menu_indexes.pop_back();
        indexed_menu[local_idx]->selected = false;
    } else {
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
