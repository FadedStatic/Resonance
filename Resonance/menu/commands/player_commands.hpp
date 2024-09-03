#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_player_namespace() {
    std::unordered_map<std::string, std::tuple<men_ty, std::function<menu_option_callback>, std::function<
        menu_option_callback>, bool, std::vector<std::pair<std::string, std::string> > > > player_callbacks =
    {
        {
            "Godmode", {
                men_ty::check, [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::GET_PLAYER_PED(-1), true);
                    return true;
                },
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::GET_PLAYER_PED(-1), false);
                    return true;
                },
                false, {}
            }
        },
        {
            "Wanted Level", {
                men_ty::multi, [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    const auto &real_ctx = std::dynamic_pointer_cast<multi_option_t>(ctx);
                    PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(),
                                                    std::stoi(real_ctx->options[real_ctx->idx].second), false);
                    PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), false);
                    return true;
                },
                nullptr, false, {
                    {"0", "0"},
                    {"1", "1"},
                    {"2", "2"},
                    {"3", "3"},
                    {"4", "4"},
                    {"5", "5"}
                }
            }
        }
    };

    auto ret = std::make_shared<cat_menu_option_t>("Player");
    for (const auto &[name, pr]: player_callbacks) {
        switch (const auto &[ty, fn, cb, def_en, opts] = pr; ty) {
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
                    std::make_shared<multi_option_t>(name, fn, opts)
                );
                break;
        }
    }

    return ret;
}
