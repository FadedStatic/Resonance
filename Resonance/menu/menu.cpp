#include "namespaces.hpp"


std::atomic_bool menu_t::menu_open = false;
std::atomic_bool menu_t::menu_exit = false;

void menu_t::initialize() {
    INTERNAL_menu_initialized = true;
    header_widget_.setup_circles();
    selected_widget_.setup_circles();
    swap_chain_ptr->GetDevice(__uuidof(ID3D11Device), (void **) &d3d_device_ptr);
    d3d_device_ptr->GetImmediateContext(&d3d_device_ctx_ptr);

    ID3D11Texture2D *rttexture = nullptr;
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
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    D3DX11_IMAGE_LOAD_INFO info;
    ID3DX11ThreadPump *pump{nullptr};
    D3DX11CreateShaderResourceViewFromMemory(d3d_device_ptr, logo, sizeof(logo), &info,
                                             pump, &Image, 0);

    main_font = io.Fonts->AddFontFromMemoryTTF(interFont, 874708, 18);


    this->menu_indexes.push_back(0);
    this->submenus.reserve(8);
    for (const auto &init_func: init::namespaces)
        this->submenus.push_back(init_func());
    auto &this_wrapper = *this;
    this->submenus.push_back(std::make_shared<menu_option_t>(
        "Exit", [&](const std::shared_ptr<menu_option_t> &menu_ctx) -> bool {
            this_wrapper.menu_exit = true;
            return true;
        }
    ));
    this->submenus.at(0)->selected = true;
}

menu_t::menu_t() {
    this->header_widget_ = particle_widget(
        this->pos, {354, 140}, 40, 5, 3, global::menu::theme::header, global::menu::theme::header_dots
    );
    this->selected_widget_ = particle_widget<true>(
        {this->pos.x + 4, this->pos.y + 144}, {346, 32}, 15, 5, 3, global::menu::theme::subheader,
        global::menu::theme::header_dots
    );
}

void menu_t::render(IDXGISwapChain *_swap_chain_ptr) {
    swap_chain_ptr = _swap_chain_ptr;
    if (!INTERNAL_menu_initialized)
        return initialize();

    if (!menu_t::menu_open)
        return;

    const auto &menu_base_pos = this->pos;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    auto &style = ImGui::GetStyle();

    style.WindowRounding = 5.0f;
    style.WindowPadding = {0.f, 0.f};
    style.WindowBorderSize = 0.f;
    const auto bg_col = global::menu::theme::bg_col.load();
    style.Colors[ImGuiCol_WindowBg] = bg_col.Value;
    ImGui::SetNextWindowSize({354, 433});
    ImGui::SetNextWindowPos(menu_base_pos);

    ImGui::Begin("mainMenu", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMouseInputs);


    header_widget_.render();
    ImGui::SetCursorPos({104, 11});
    ImGui::Image((PVOID) Image, ImVec2(141, 117));

    ImGui::PushFont(main_font);

    // So, our entrancy depth is dictated by the size of selected vector
    const auto &where_vec = this->menu_indexes;
    auto indexed_menu = this->submenus;
    std::string menu_title{"Resonance"};
    for (std::size_t i = 1; i < where_vec.size(); i++) {
        menu_title = indexed_menu.at(where_vec[i - 1])->name;
        indexed_menu = std::dynamic_pointer_cast<cat_menu_option_t>(indexed_menu[where_vec[i - 1]])->options;
    }

    // Render subheading
    const auto main_idx = this->menu_indexes.at(this->menu_indexes.size() - 1) + 1;
    ImGui::GetWindowDrawList()->AddRectFilled({menu_base_pos.x, menu_base_pos.y + 108},
                                              {menu_base_pos.x + 354, menu_base_pos.y + 108 + 34},
                                              global::menu::theme::subheader.load());
    ImGui::GetWindowDrawList()->AddText({menu_base_pos.x + 11, menu_base_pos.y + 114},
                                        global::menu::theme::subheader_text.load(), menu_title.c_str());
    const auto max_idx = indexed_menu.size();
    const auto pos_label = std::vformat("{:d} / {:d}", std::make_format_args(main_idx, max_idx)).c_str();
    const auto text_sz = ImGui::CalcTextSize(pos_label);
    ImGui::GetWindowDrawList()->AddText({menu_base_pos.x + 343 - text_sz.x, menu_base_pos.y + 114},
                                        global::menu::theme::active_text.load(), pos_label);

    if (global::menu::move_circles) {
        selected_widget_.move_widget(main_idx - 1);
        global::menu::move_circles = false;
    }

    selected_widget_.render();

    ImVec2 running_pos{menu_base_pos.x + 11, menu_base_pos.y + 146};
    for (const auto &item: indexed_menu) {
        item->render(running_pos);
        running_pos.y += 36;
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void menu_option_t::render(const ImVec2 &pos) {
    ImGui::GetWindowDrawList()->AddText({pos.x, pos.y + 7},
                                        this->selected
                                            ? global::menu::theme::active_text.load()
                                            : global::menu::theme::inactive_text.load(), this->name.c_str());
}

void checkbox_menu_option_t::render(const ImVec2 &pos) {
    ImGui::GetWindowDrawList()->AddText({pos.x, pos.y + 7},
                                        this->selected
                                            ? global::menu::theme::active_text.load()
                                            : global::menu::theme::inactive_text.load(), this->name.c_str());
    if (!this->checked)
        ImGui::GetWindowDrawList()->AddRect({pos.x + 346 - 31, pos.y + 8}, {pos.x + 346 - 15, pos.y + 24}, 0xFFFFFFFF,
                                            5, 0, 1.5);
    else
        ImGui::GetWindowDrawList()->AddRectFilled({pos.x + 346 - 31, pos.y + 8}, {pos.x + 346 - 15, pos.y + 24},
                                                  0xFFFFFFFF, 5);
}

void multi_option_t::render(const ImVec2 &pos) {
    menu_option_t::render(pos);

    bool right_enabled = this->idx != this->options.size() - 1;
    ImGui::GetWindowDrawList()->AddLine({pos.x + 325, pos.y + 11}, {pos.x + (346 - 15), pos.y + 17},
                                        right_enabled ? 0xFFFFFFFF : 0x91FFFFFF, 1.5);
    ImGui::GetWindowDrawList()->AddLine({pos.x + 325, pos.y + 21}, {pos.x + 346 - 15, pos.y + 15},
                                        right_enabled ? 0xFFFFFFFF : 0x91FFFFFF, 1.5);

    const auto &menu_opt = this->options[this->idx];
    const auto text_sz = ImGui::CalcTextSize(menu_opt.first.c_str());

    ImGui::GetWindowDrawList()->AddText({pos.x + 317 - text_sz.x, pos.y + 7}, global::menu::theme::active_text.load(),
                                        menu_opt.first.c_str());

    bool left_enabled = this->idx != 0;
    ImGui::GetWindowDrawList()->AddLine({pos.x + 311 - 8 - text_sz.x - 3, pos.y + 16},
                                        {pos.x + 311 - text_sz.x - 5, pos.y + 11},
                                        left_enabled ? 0xFFFFFFFF : 0x91FFFFFF, 1.5);
    ImGui::GetWindowDrawList()->AddLine({pos.x + 311 - 8 - text_sz.x - 3, pos.y + 16},
                                        {pos.x + 311 - text_sz.x - 5, pos.y + 21},
                                        left_enabled ? 0xFFFFFFFF : 0x91FFFFFF, 1.5);
}
