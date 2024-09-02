#pragma once
#include <array>

#include "commands/misc_commands.hpp"
#include "commands/online_commands.hpp"
#include "commands/player_commands.hpp"
#include "commands/settings.hpp"
#include "commands/teleport_commands.hpp"
#include "commands/vehspawner_commands.hpp"
#include "commands/world_commands.hpp"

namespace init {
    std::array<std::function<std::shared_ptr<cat_menu_option_t>()>, 7> namespaces{
        &create_player_namespace,
        &create_teleport_namespace,
        &create_world_namespace,
        &create_vehicle_spawner_namespace,
        &create_misc_namespace,
        &create_online_namespace,
        &create_settings
    };
}
