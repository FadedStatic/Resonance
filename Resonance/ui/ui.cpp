#include "include.hpp"

void menu::render()
{

	if (!global::menu_gen::menu_open)
		return;
	// Draw Header
	draw_rect({ 0.7f, 0.32f }, { 0.23f, 0.09f }, color(255, 255, 255, 255));
}

void draw_rect(vec2 pos, vec2 scale, const color col)
{
	GRAPHICS::DRAW_RECT(pos.a + scale.a / 2, pos.b + scale.b / 2, scale.a, scale.b, col.r, col.g, col.b, col.a, 0);
}
