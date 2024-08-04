#pragma once
#include <shared_mutex>
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
namespace global {
	template<typename _Ty>
	struct shared_var {
		std::shared_mutex obj_mutex;
		_Ty object;

		_Ty get() {
			std::shared_lock<std::shared_mutex> lock(this->obj_mutex);
			return object;
		}

		void set(const _Ty obj) {
			std::unique_lock<std::shared_mutex> lock(this->obj_mutex);
			object = obj;
		}

		void operator=(const _Ty obj) {
			set(obj);
		}

		operator _Ty() {
			return get();
		}

		shared_var(_Ty obj) : object(obj) {}
	};

	namespace menu_gen
	{
		extern shared_var<bool> menu_open;
		extern shared_var<bool> menu_exit;
	}

}