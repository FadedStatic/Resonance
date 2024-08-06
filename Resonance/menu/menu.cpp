#include "menu.hpp"

void menu::render()
{
	if (get_input_just_pressed(VK_F9))
		global::menu::menu_open = !global::menu::menu_open;

	if (!global::menu::menu_open)
		return;

	handle_inputs();

	const auto menu_base_pos = global::menu::pos.load();
	// Draw Heaader
	draw_rect(menu_base_pos, { 0.175f, 0.12f }, global::menu::theme::header);

	// Draw subheading

	draw_rect({menu_base_pos.a, menu_base_pos.b+0.12f}, {0.175f, 0.05f}, global::menu::theme::subheader);
}

void menu::handle_inputs() {
	disable_inputs();


}

// DO NOT make this static under  any circumstances. it is  dumb to do so
bool menu::get_input_just_pressed(int key, bool from_keyboard) {
	return from_keyboard ? (GetAsyncKeyState(key) & 0xFFFF) == 0x8001 : false;
}

void draw_rect(vec2 pos, vec2 scale, const color col)
{
	GRAPHICS::DRAW_RECT(pos.a + scale.a / 2, pos.b + scale.b / 2, scale.a, scale.b, col.r, col.g, col.b, col.a, 0);
}

void menu::disable_inputs() {
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_ACCEPT);
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_CANCEL);
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_DOWN);
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_UP);
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_LEFT);
	PAD::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_RIGHT);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_SELECT_NEXT_WEAPON, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_RIGHT, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_DOWN, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_LEFT, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_FRONTEND_UP, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_ARREST, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_CONTEXT, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_BLOCK, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_UP, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_HUD_SPECIAL, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_CIN_CAM, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_VEH_CIN_CAM, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_HEADLIGHT, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_LEFT, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_DOWN, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RDOWN, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RIGHT, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_CHARACTER_WHEEL, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_VEH_RADIO_WHEEL, true);
	PAD::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_CANCEL, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_LIGHT, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_HEAVY, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_TREVOR, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MICHAEL, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_FRANKLIN, true);
	PAD::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, true);
}