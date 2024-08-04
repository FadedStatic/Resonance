#pragma once

#include "../update/natives.hpp"
#include "../global/include.hpp"

struct vec2
{
	float a, b;
};


void draw_rect(vec2 pos, vec2 scale, const color col);
class menu
{
public:
	menu() = default;

	void render();
};