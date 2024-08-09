#pragma once

#include "../update/natives.hpp"
#include "../global/global.hpp"

void draw_rect(vec2 pos, vec2 scale, color col);
class menu_option_t {
public:
	bool selected{ false };
	std::string name;
	std::string description;

	menu_option_t(const std::string_view name_) noexcept : name{name_} { }

};

class cat_menu_option_t : public menu_option_t {
public:
	std::vector<menu_option_t*> options;
	cat_menu_option_t(const std::string_view name) noexcept : menu_option_t{name} { }
};
class menu_t
{
	int main_idx{0}, sub_idx{0};
	std::vector<menu_option_t*> submenus_;
public:
	menu_t() = default;

	[[nodiscard]] static bool get_input_just_pressed(int key, bool from_keyboard=true);
	static void render();
	static void handle_inputs();
	static void disable_inputs();
};