#include "menu.hpp"

void menu_t::initialize()
{
	INTERNAL_menu_initialized = true;
	header_widget_.setup_circles();
	device->GetImmediateContext(&d3d_device_ctx_ptr);
	ID3D11Texture2D* rttexture = nullptr;
	if (SUCCEEDED(swap_chain_ptr->GetBuffer(0, IID_PPV_ARGS(&rttexture)))) {
		device->CreateRenderTargetView(rttexture, NULL, &render_target_view_ptr);
		if (!render_target_view_ptr)
			return;
		rttexture->Release();
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindow(L"Progman", L"Program Manager"));
	ImGui_ImplDX11_Init(d3d_device_ptr, d3d_device_ctx_ptr);
	ImGui::StyleColorsLight();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	/*D3DX11_IMAGE_LOAD_INFO info;
	ID3DX11ThreadPump* pump{ nullptr };
	D3DX11CreateShaderResourceViewFromMemory(device, logo, sizeof(logo), &info,
		pump, &Image, 0);*/
}

menu_t::menu_t()
{
	const auto menu_base_pos = global::menu::pos.load();
	this->header_widget_ = particle_widget(global::menu::pos, { 354, 140 }, 40, 5, 3, global::menu::theme::header, global::menu::theme::header_dots);
}

void menu_t::render()
{
	if (!INTERNAL_menu_initialized)
		return initialize();

	if (get_input_just_pressed(VK_F9))
		global::menu::menu_open = !global::menu::menu_open;

	if (!global::menu::menu_open)
		return;

	handle_inputs();

	auto menu_base_pos = global::menu::pos.load();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	auto& style = ImGui::GetStyle();

	style.WindowRounding = 5.0f;
	style.WindowPadding = {0.f, 0.f};
	style.WindowBorderSize = 0.f;
	const auto bg_col = global::menu::theme::bg_col.load();
	style.Colors[ImGuiCol_WindowBg] = bg_col.Value;
	ImGui::SetNextWindowSize({ 354, 433 });
	ImGui::SetNextWindowPos(menu_base_pos);

	ImGui::Begin("mainMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMouseInputs);


	header_widget_.render();
	ImGui::SetCursorPos({104, 11});
	/*ImGui::Image((PVOID)Image, ImVec2(141, 117));*/



	ImGui::End();

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void menu_t::handle_inputs() {
	disable_inputs();
}

bool menu_t::get_input_just_pressed(int key, bool from_keyboard) {
	return from_keyboard ? (GetAsyncKeyState(key) & 0xFFFF) == 0x8001 : false;
}

void menu_t::disable_inputs() {
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