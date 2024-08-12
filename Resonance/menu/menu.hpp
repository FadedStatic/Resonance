#pragma once
#include <d3d11.h>
#include "../global/global.hpp"
#include "../update/natives.hpp"
#include <vector>
#include <Windows.h>
#include "../global/assets.hpp"
#include "../libs/imgui/imgui_impl_win32.h"
#include "../libs/imgui/imgui_impl_dx11.h"
#include "widgets/particle_widget.hpp"

#include <tchar.h>
#include <d3dx11tex.h>

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

static bool INTERNAL_menu_initialized{ false };
class menu_t
{
	particle_widget header_widget_;
	ID3D11Device* device;
	ID3D11ShaderResourceView* Image = nullptr;
	ID3D11DeviceContext* d3d_device_ctx_ptr{};
	IDXGISwapChain* swap_chain_ptr{};
	ID3D11RenderTargetView* render_target_view_ptr{};
public:
	menu_t();

	void set_swapchain(IDXGISwapChain*  _swap_chain_ptr) {swap_chain_ptr=_swap_chain_ptr;}
	void render();
	void initialize();
};