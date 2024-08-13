#pragma once
#include "../menu.hpp"

cat_menu_option_t* create_player_namespace()
{
    auto ret = new cat_menu_option_t("Player");
    ret->options.push_back(new menu_option_t("Godmode"));
    ret->options.push_back(new menu_option_t("Some other shit"));
    return ret;
}