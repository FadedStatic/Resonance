#pragma once

#include "../update/natives.hpp"
#include "../global/global.hpp"

struct color
{
	int r, g, b, a;
	explicit color(const std::uint32_t col)
	{
		r = (col >> 24) & 0xFF;
		g = (col >> 16) & 0xFF;
		b = (col >> 8) & 0xFF;
		a = col & 0xFF;
	}

	explicit color(const int r, const int g, const int b, const int a) : r(r), g(g), b(b), a(a) {}
};

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