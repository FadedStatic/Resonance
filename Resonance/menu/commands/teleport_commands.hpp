#pragma once
#include "../menu.hpp"

std::shared_ptr<cat_menu_option_t> create_teleport_namespace() {
    std::unordered_map<std::string, std::any> player_callbacks_2 = {
        {
            "Waypoint",
            menu_option_reg_t{
                [](const std::shared_ptr<menu_option_t> &ctx) -> bool {

                    const auto blip = HUD::GET_FIRST_BLIP_INFO_ID(8); // waypoint blip id
                    if (!blip)
                        return false;

                    const auto waypoint = HUD::GET_BLIP_COORDS(blip);
                    PED::SET_PED_COORDS_KEEP_VEHICLE(PLAYER::PLAYER_PED_ID(), waypoint.x, waypoint.y, waypoint.z + 5);
                    return true;
                },
            }
        },
{
    "Objectivee",
    menu_option_reg_t{
        [](const std::shared_ptr<menu_option_t> &ctx) -> bool {

            const auto blip = HUD::GET_FIRST_BLIP_INFO_ID(1); // waypoint blip id
            if (!blip)
                return false;

            const auto waypoint = HUD::GET_BLIP_COORDS(blip);
            PED::SET_PED_COORDS_KEEP_VEHICLE(PLAYER::PLAYER_PED_ID(), waypoint.x, waypoint.y, waypoint.z + 5);
            return true;
        },
    }
},
    };

    auto ret = std::make_shared<cat_menu_option_t>("Teleport");
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
