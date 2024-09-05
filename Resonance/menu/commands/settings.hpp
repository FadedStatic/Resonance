#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_settings() {
    std::map<const std::string_view, std::function<menu_option_callback> > settings
    {
        {
            "test", [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                std::cout << "test1\n";
                return true;
            }
        },
        {
            "test2", [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                std::cout << "test2\n";
                return true;
            }
        },
    };

    auto ret = std::make_shared<cat_menu_option_t>("Settings");
    for (const auto &[name, fn]: settings)
        ret->options.emplace_back(
            std::make_shared<menu_option_t>(name, fn)
        );

    return ret;
}
