#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_player_namespace()
{
    std::unordered_map<std::string, std::tuple<men_ty, std::function<menu_option_callback>, std::function<menu_option_callback>, bool, std::vector<std::pair<std::string,std::string>>>> player_callbacks =
    {
        { "test", {men_ty::reg, [](const std::shared_ptr<menu_option_t>& ctx) -> bool {std::cout << "test1\n"; return true; }, nullptr, false, {}}},
        { "test2", {men_ty::reg, [](const std::shared_ptr<menu_option_t>& ctx) -> bool {std::cout << "test2\n"; return true; }, nullptr, false, {}}},
        {"Godmode", {men_ty::check, [](const std::shared_ptr<menu_option_t>& ctx) -> bool {std::cout << "Godmode!\r\n"; return true;  }, [](const std::shared_ptr<menu_option_t>& ctx) -> bool { return true; }, false, {}}},
        { "Wanted Level", {men_ty::multi, [](const std::shared_ptr<menu_option_t>& ctx) -> bool { return true; }, nullptr, false,{
                {"Poop", "Deck"},
                {"Fuckus", "Niggus"},
            } } }
    };

    auto ret = std::make_shared<cat_menu_option_t>("Player");
    for (const auto& [name, pr] : player_callbacks) {
        switch (const auto& [ty, fn, cb, def_en, opts] = pr; ty)
        {
            case men_ty::check:
                ret->options.emplace_back(
                    std::make_shared<checkbox_menu_option_t>(name, fn, cb, def_en)
                );
            break;
            case men_ty::reg:
                ret->options.emplace_back(
                    std::make_shared<menu_option_t>(name, fn)
                );
            break;
            case men_ty::multi:
                ret->options.emplace_back(
                    std::make_shared<multi_option_t>(name, opts)
                );
            break;
        }
    }

    return ret;
}