#pragma once
#include <shared_mutex>

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

		shared_var(_Ty obj) : object(obj) {}
	};

	namespace menu_gen
	{
		shared_var<bool> menu_open{ false };
	}
}