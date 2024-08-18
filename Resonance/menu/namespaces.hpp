#pragma once
#include <array>

#include "commands/misc_commands.hpp"
#include "commands/online_commands.hpp"
#include "commands/player_commands.hpp"
#include "commands/settings.hpp"
#include "commands/teleport_commands.hpp"
#include "commands/vehspawner_commands.hpp"
#include "commands/world_commands.hpp"

std::vector<std::shared_ptr<menu_option_t>> create_namespaces() {
    auto ret = std::vector<std::shared_ptr<menu_option_t>>();
    for (
        constexpr std::array namespaces
        = {

            create_player_namespace,
            create_teleport_namespace,
            create_world_namespace,
            create_vehicle_spawner_namespace,
            create_misc_namespace,
            create_online_namespace,
            create_settings
        };
        const auto& fn : namespaces)
    {
        ret.push_back(fn());
    }

    return ret;
}