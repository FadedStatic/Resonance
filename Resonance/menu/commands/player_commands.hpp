#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_player_namespace() {
    std::unordered_map<std::string, std::any> player_callbacks_2 = {
        {
            "Godmode",
            checkbox_menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::GET_PLAYER_PED(-1), true);
                    return true;
                },
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::GET_PLAYER_PED(-1), false);
                    return true;
                },
                false
            }
        },
        {
            "Wanted Level",
            multi_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    const auto &real_ctx = std::dynamic_pointer_cast<multi_option_t>(ctx);
                    PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(),
                                                    std::stoi(real_ctx->options[real_ctx->idx].second), false);
                    PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), false);
                    return true;
                },
                {
                    {"0", "0"},
                    {"1", "1"},
                    {"2", "2"},
                    {"3", "3"},
                    {"4", "4"},
                    {"5", "5"}
                }
            }
        },
        {
            "No Ragdoll",
            checkbox_menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    PED::SET_PED_CAN_RAGDOLL(PLAYER::GET_PLAYER_PED(-1), false);
                    PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::GET_PLAYER_PED(-1), false);
                    PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::GET_PLAYER_PED(-1), false);

                    return true;
                },
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    const auto &real_ctx = std::dynamic_pointer_cast<checkbox_menu_option_t>(ctx);
                    PED::SET_PED_CAN_RAGDOLL(PLAYER::GET_PLAYER_PED(-1), real_ctx->checked);
                    PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::GET_PLAYER_PED(-1), real_ctx->checked);
                    PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(PLAYER::GET_PLAYER_PED(-1), real_ctx->checked);
                    return true;
                }
            }
        },
        {
        "Suicide",
            menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    PED::APPLY_DAMAGE_TO_PED(PLAYER::PLAYER_PED_ID(), 30000, true, 0, 0);
                    return true;
                }
            }
        },
        {
            "Skydive",
            menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    Vector3 waypoint = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

                    PED::SET_PED_COORDS_KEEP_VEHICLE(PLAYER::PLAYER_PED_ID(), waypoint.x, waypoint.y, waypoint.z + 200);
                    // 200 seems like a good amount

                    return true;
                },
            }
        },
        {
            "Homelander", checkbox_menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {
                    if (ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(PLAYER::PLAYER_PED_ID()) < 3) {
                        Vector3 waypoint = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

                        PED::SET_PED_COORDS_KEEP_VEHICLE(PLAYER::PLAYER_PED_ID(), waypoint.x, waypoint.y,
                                                         waypoint.z + 60);
                    }
                    return true;
                },
                nullptr,
                false
            }
        },
    };

    auto ret = std::make_shared<cat_menu_option_t>("Player");
    for (const auto &[name, pr]: player_callbacks_2) {
        if (!pr.has_value())
            continue;
        const std::type_info &ty = pr.type();
        if (ty == typeid(menu_option_reg_t))
            ret->options.emplace_back(
                std::make_shared<menu_option_t>(name, std::any_cast<menu_option_reg_t>(pr).cb)
            );
        else if (ty == typeid(checkbox_menu_option_reg_t)) {
            const auto [recur_cb, cb, checked] = std::any_cast<checkbox_menu_option_reg_t>(pr);
            ret->options.emplace_back(
                std::make_shared<checkbox_menu_option_t>(name, recur_cb, cb, checked)
            );
        } else if (ty == typeid(multi_option_reg_t))
            ret->options.emplace_back(
                std::make_shared<multi_option_t>(name, std::any_cast<multi_option_reg_t>(pr).cb,
                                                 std::any_cast<multi_option_reg_t>(pr).options)
            );
        else if (ty == typeid(cat_menu_option_reg_t))
            ret->options.emplace_back(
                std::make_shared<cat_menu_option_t>(name, std::any_cast<cat_menu_option_reg_t>(pr).options)
            );
    }

    return ret;
}
