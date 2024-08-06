#pragma once

#include "../update/natives.hpp"
#include "../global/global.hpp"

void draw_rect(vec2 pos, vec2 scale, color col);

class menu_option {
public:
	bool selected{false};
	std::string text;

};

class submenu {
public:
	std::string title; // title of it.
	std::vector<menu_option> options;
};

class menu
{
	int main_idx{0}, sub_idx{0};
	std::vector<submenu> submenus_;
public:
	menu() = default;

	bool get_input_just_pressed(int key, bool from_keyboard=true);
	void render();
	void handle_inputs();
	void disable_inputs();
};