#pragma once
#include "menu.hpp"

bool fuck_cb(const std::shared_ptr<menu_option_t>& menu_ctx)
{
    std::cout << "FUCK!\r\n";
    return true;
}

bool fuck2_cb(const std::shared_ptr<menu_option_t>& menu_ctx)
{
    std::cout << "FUCK2!\r\n";
    return true;
}

std::shared_ptr<cat_menu_option_t> create_teleport_namespace()
{
    auto ret = std::make_shared<cat_menu_option_t>("Teleport");
    ret->options.push_back(std::make_shared<menu_option_t>("Fuck", fuck_cb));
    ret->options.push_back(std::make_shared<menu_option_t>("Fuck2", fuck2_cb));
    ret->options.push_back(std::make_shared<menu_option_t>("Fuck3"));
    return ret;
}