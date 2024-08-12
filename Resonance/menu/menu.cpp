#include "menu.hpp"

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
	d3d_device_ctx_ptr->OMSetRenderTargets(1, &render_target_view_ptr, NULL);
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}