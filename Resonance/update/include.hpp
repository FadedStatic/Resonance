#pragma once
#include <cstdint>

#pragma pack(push, 8)
template <typename _Ty, class _CounterType = std::uint16_t>
struct at_array_t {
	at_array_t() {
		data = nullptr;
		size = 0;
		capacity = 0;
	}
	at_array_t(_Ty* data_ptr, _CounterType  _size, _CounterType _cap) : data(data_ptr),size(_size),capacity(_cap) {}

	_Ty& operator[](const _CounterType index) {
		return data[index];
	}
	const _Ty& operator[](const _CounterType index) const {
		return data[index];
	}
	_Ty* begin() {
		return &data[0];
	}
	_Ty* end() {
		return &data[size-1];
	}
	const _Ty* begin() const {
		return &data[0];
	}
	const _Ty* end() const {
		return &data[size-1];
	}
	const _CounterType size() const {
		return size;
	}
	const _CounterType capacity() const {
		return capacity;
	}
	_Ty* data;
	_CounterType size, capacity;
};

struct scrThread {
	char pad_0[0x8]; // 0x0
	struct {

	} m_ctx;
};