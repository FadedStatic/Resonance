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
#include <functional>
#include <tchar.h>
#include <d3dx11tex.h>
#include <atomic>
#include <array>

class menu_option_t {
public:
	bool selected{ false };
	std::string name;
	std::string description;
	std::function<menu_option_callback> cb{};
	virtual ~menu_option_t() = default;
	void render(const ImVec2& pos);

	menu_option_t(const std::string_view name_) noexcept : name{ name_ } { }
	menu_option_t(const std::string_view name_,  const std::function<menu_option_callback>& cb_)  : name(name_), cb(cb_) {}

};

class cat_menu_option_t : public menu_option_t {
public:
	std::vector<std::shared_ptr<menu_option_t>> options;
	cat_menu_option_t(const std::string_view name) noexcept : menu_option_t{ name } { }

};

inline bool INTERNAL_menu_initialized{ false };
class menu_t
{
	particle_widget<false> header_widget_;
	particle_widget<true> selected_widget_;
	ID3D11Device* d3d_device_ptr{};
	ID3D11ShaderResourceView* Image = nullptr;
	ID3D11DeviceContext* d3d_device_ctx_ptr{};
	IDXGISwapChain* swap_chain_ptr{};
	ImFont* main_font;
	ID3D11RenderTargetView* render_target_view_ptr{};

public:
	std::vector<std::shared_ptr<menu_option_t>> submenus{};
	std::vector<std::uint32_t> menu_indexes{}; // so the neat thing about this is we just pop_back and emplace_back if wee are eentering the new sections of the menu, sign me up for infinite reentrancy hah
	static std::atomic_bool menu_open;
	static std::atomic_bool menu_exit;
	
	ImVec2 pos{ ImVec2{ 35,35 } };

	menu_t();

	void render(IDXGISwapChain* _swap_chain_ptr);
	void initialize();
};