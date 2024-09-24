#pragma once

#include "../global/global.hpp"

struct script_global {
    script_global(int idx) {
        static const auto base = reinterpret_cast<std::uint64_t>(GetModuleHandleA(nullptr));
        m_handle = &(base + global::native_resolver::global_cache)[idx >> 18 & 0x3F][idx & 0x3FFFF];
    }

    script_global(void* ptr)
        : m_handle(ptr)
    {}

    script_global at(int index) const {
        return script_global((void**)(m_handle) + index);
    }

    script_global at(int index, int size) const {
        return at(1 + (index * size));
    }

    template<typename T>
    T* get() {
        return (T*)(m_handle);
    }

    template<typename T>
    T& as() {
        return *get<T>();
    }

private:
    void* m_handle{};
};