#pragma once
#include <cstdint>
#include <Windows.h>


#pragma pack(push, 8)
template <typename _Ty, class _CounterType = std::uint16_t>
struct at_array_t {
	at_array_t(_Ty* data_ptr, _CounterType  _size, _CounterType _cap) : data(data_ptr),sz(_size),cap(_cap) {}

	at_array_t(const void* data_ptr) {
		auto* data_ptr_ = reinterpret_cast<const at_array_t*>(data_ptr);
		data = data_ptr_->data;
		sz = data_ptr_->sz;
		cap = data_ptr_->cap;
	}
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
		return &data[sz-1];
	}
	const _Ty* begin() const {
		return &data[0];
	}
	const _Ty* end() const {
		return &data[sz-1];
	}
	const _CounterType size() const {
		return sz;
	}
	const _CounterType capacity() const {
		return cap;
	}
	_Ty* data{ nullptr };
	_CounterType sz{ 0 }, cap{0};
};
#pragma pack(pop)

enum states : std::uint32_t
{
	idle,
	awake,
	off,
};
struct scrThread {
	std::uintptr_t** vtable_ptr; // 0x0, vtable ptr

	std::uint32_t m_thread_id; // 0x8
	std::uint32_t m_script_hash; // 0x10
	states m_state; // 0x18

};


#pragma pack(push, 1)
class scr_vector_t
{
public:
	scr_vector_t() = default;

	scr_vector_t(float x, float y, float z) :
		x(x), y(y), z(z)
	{}
public:
	float x{};
private:
	char m_padding1[0x04];
public:
	float y{};
private:
	char m_padding2[0x04];
public:
	float z{};
private:
	char m_padding3[0x04];
};
#pragma pack(pop)

using Void = void;
using Any = std::uint32_t;
using Hash = std::uint32_t;
using Entity = std::uint64_t;
using Player = std::int32_t;
using Ped = Entity;
using Vehicle = Entity;
using Cam = std::int32_t;
using Object = Entity;
using Pickup = Object;
using Blip = std::int32_t;
using Camera = Entity;
using ScrHandle = Entity;
using FireId = Entity;
using Interior = Entity;
using Vector3 = scr_vector_t;