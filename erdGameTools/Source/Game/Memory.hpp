#pragma once

#include "../Common.hpp"

namespace ERD::Game {

// 安全读取游戏内存。
// 这里直接 memcpy，是因为当前 DLL 已经在游戏进程内，不需要 ReadProcessMemory。
inline bool ReadMemory(uintptr_t address, void* output, std::size_t size) {
    __try {
        std::memcpy(output, reinterpret_cast<const void*>(address), size);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

// 安全写入游戏内存。
inline bool WriteMemory(uintptr_t address, const void* input, std::size_t size) {
    __try {
        std::memcpy(reinterpret_cast<void*>(address), input, size);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

template <typename T>
bool ReadValue(uintptr_t address, T& output) {
    return ReadMemory(address, &output, sizeof(T));
}

template <typename T>
bool WriteValue(uintptr_t address, const T& input) {
    return WriteMemory(address, &input, sizeof(T));
}

// 解析多级指针链。
// 这类逻辑正是很多 CE 指针项在 DLL 里最常见的落地方式。
inline bool ResolvePointerChain(uintptr_t base_object,
                                const std::vector<uintptr_t>& pointer_offsets,
                                uintptr_t final_offset,
                                uintptr_t& result) {
    uintptr_t current = base_object;
    for (uintptr_t offset : pointer_offsets) {
        uintptr_t next = 0;
        if (!ReadValue(current + offset, next) || next == 0) {
            return false;
        }
        current = next;
    }

    result = current + final_offset;
    return true;
}

// 修改 1 字节里的某一位标志。
inline bool SetBitFlag(uintptr_t address, int bit_index, bool enabled) {
    if (bit_index < 0 || bit_index > 7) {
        return false;
    }

    std::uint8_t value = 0;
    if (!ReadValue(address, value)) {
        return false;
    }

    const std::uint8_t mask = static_cast<std::uint8_t>(1u << bit_index);
    const std::uint8_t new_value = enabled ? static_cast<std::uint8_t>(value | mask)
                                           : static_cast<std::uint8_t>(value & ~mask);
    if (new_value == value) {
        return true;
    }

    return WriteValue(address, new_value);
}

}  // namespace ERD::Game
