#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_player_namespace()
{
    auto ret = std::make_shared<cat_menu_option_t>("Player");
    ret->options.push_back(std::make_shared<menu_option_t>("Players"));
    return ret;
}