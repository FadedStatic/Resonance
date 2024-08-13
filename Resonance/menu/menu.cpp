#include "commands/player_commands.hpp"

void menu_t::initialize()
{
	INTERNAL_menu_initialized = true;
	header_widget_.setup_circles();
	swap_chain_ptr->GetDevice(__uuidof(ID3D11Device), (void**)&d3d_device_ptr);
	d3d_device_ptr->GetImmediateContext(&d3d_device_ctx_ptr);

	ID3D11Texture2D* rttexture = nullptr;
	if (SUCCEEDED(swap_chain_ptr->GetBuffer(0, IID_PPV_ARGS(&rttexture)))) {
		d3d_device_ptr->CreateRenderTargetView(rttexture, NULL, &render_target_view_ptr);
		if (!render_target_view_ptr)
			return;
		rttexture->Release();
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA("grcWindow", "Grand Theft Auto V"));
	ImGui_ImplDX11_Init(d3d_device_ptr, d3d_device_ctx_ptr);
	ImGui::StyleColorsLight();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	D3DX11_IMAGE_LOAD_INFO info;
	ID3DX11ThreadPump* pump{ nullptr };
	D3DX11CreateShaderResourceViewFromMemory(d3d_device_ptr, logo, sizeof(logo), &info,
		pump, &Image, 0);

	main_font = io.Fonts->AddFontFromMemoryTTF(interFont, 874708, 18);

	auto teleport = new menu_option_t{
"Teleport" };

	global::menu::submenus.push_back(create_player_namespace());
	global::menu::submenus.push_back(teleport);
	global::menu::menu_indexes.push_back(0);
}

menu_t::menu_t()
{
	const auto menu_base_pos = global::menu::pos.load();
	this->header_widget_ = particle_widget(global::menu::pos, { 354, 140 }, 40, 5, 3, global::menu::theme::header, global::menu::theme::header_dots);
}

void menu_t::render(IDXGISwapChain* _swap_chain_ptr)
{
	swap_chain_ptr = _swap_chain_ptr;
	if (!INTERNAL_menu_initialized)
		return initialize();

	if (!global::menu::menu_open)
		return;

	auto menu_base_pos = global::menu::pos.load();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	auto& style = ImGui::GetStyle();

	style.WindowRounding = 5.0f;
	style.WindowPadding = { 0.f, 0.f };
	style.WindowBorderSize = 0.f;
	const auto bg_col = global::menu::theme::bg_col.load();
	style.Colors[ImGuiCol_WindowBg] = bg_col.Value;
	ImGui::SetNextWindowSize({ 354, 433 });
	ImGui::SetNextWindowPos(menu_base_pos);

	ImGui::Begin("mainMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMouseInputs);


	header_widget_.render();
	ImGui::SetCursorPos({ 104, 11 });
	ImGui::Image((PVOID)Image, ImVec2(141, 117));

	ImGui::PushFont(main_font);

	// So, our entrancy depth is dictated by the size of selected vector
	const auto& where_vec = global::menu::menu_indexes.get();
	auto&& menus = global::menu::submenus.get();
	int curr_depth = 0;

	auto&& indexed_menu = menus;
	while (curr_depth++ < static_cast<int>(where_vec.size()) - 2)
	{
		indexed_menu = reinterpret_cast<cat_menu_option_t*>(indexed_menu[where_vec[curr_depth]])->options;
	}

	// Render subheading
	ImGui::GetWindowDrawList()->AddRectFilled({ menu_base_pos.x,menu_base_pos.y + 108 }, { menu_base_pos.x + 354, menu_base_pos.y + 108 + 34 }, global::menu::theme::subheader.load());
	ImGui::GetWindowDrawList()->AddText({ menu_base_pos.x+11,menu_base_pos.y + 114 }, global::menu::theme::subheader_text.load(), "Resonance");
	const auto main_idx = global::menu::menu_indexes.at(0)+1;
	const auto max_idx = indexed_menu.size();
	const auto pos_label = std::vformat("{:d} / {:d}", std::make_format_args(main_idx, max_idx)).c_str();
	const auto text_sz = ImGui::CalcTextSize(pos_label);
	ImGui::GetWindowDrawList()->AddText({ menu_base_pos.x + 343 - text_sz.x,menu_base_pos.y + 114 }, global::menu::theme::active_text.load(), pos_label);

	ImVec2 running_pos{ menu_base_pos.x + 11, menu_base_pos.y + 108 + 34 };
	for (const auto& item : indexed_menu)
	{
		item->render(running_pos);
		running_pos.y += 32;
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void menu_option_t::render(const ImVec2& pos)
{
	ImGui::GetWindowDrawList()->AddText(pos, this->selected ? global::menu::theme::active_text.load() : global::menu::theme::inactive_text.load(), this->name.c_str());
}
