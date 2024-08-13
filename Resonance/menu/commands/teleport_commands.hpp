#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_teleport_namespace()
{
    auto ret = std::make_shared<cat_menu_option_t>("Teleport");
    ret->options.push_back(std::make_shared<menu_option_t>("Fuck"));
    return ret;
}