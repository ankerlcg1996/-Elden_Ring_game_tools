#include "EventFlags.hpp"

#include "Memory.hpp"

namespace ERD::Game {

bool IsDlcOwned(const SingletonRegistry& singletons, int dlc_index) {
    const uintptr_t cs_dlc = singletons.GetObjectPointer("CsDlc");
    if (cs_dlc == 0) {
        return false;
    }

    std::uint8_t owned = 0;
    return ReadValue(cs_dlc + 0x10 + dlc_index, owned) && owned == 1;
}

bool SetEventFlag(const SingletonRegistry& singletons, std::uint32_t flag_id, bool enabled) {
    const uintptr_t virtual_memory_flag = singletons.GetObjectPointer("CSFD4VirtualMemoryFlag");
    if (virtual_memory_flag == 0) {
        return false;
    }

    // Event Flag 按 block 存在树结构里，先定位 block，再定位 block 内部位偏移。
    std::int32_t block_mod = 0;
    if (!ReadValue(virtual_memory_flag + 0x1C, block_mod) || block_mod <= 0) {
        return false;
    }

    const std::int32_t block = static_cast<std::int32_t>(flag_id / block_mod);
    const std::int32_t index = static_cast<std::int32_t>(flag_id - (block * block_mod));

    uintptr_t tree = 0;
    if (!ReadValue(virtual_memory_flag + 0x38, tree) || tree == 0) {
        return false;
    }

    uintptr_t node = 0;
    if (!ReadValue(tree + 0x8, node) || node == 0) {
        return false;
    }

    while (true) {
        std::uint8_t is_nil = 0;
        if (!ReadValue(node + 0x19, is_nil) || is_nil != 0) {
            return false;
        }

        std::int32_t node_block = 0;
        if (!ReadValue(node + 0x20, node_block)) {
            return false;
        }
        if (node_block == block) {
            break;
        }

        uintptr_t next = 0;
        if (node_block < block) {
            if (!ReadValue(node + 0x10, next) || next == 0) {
                return false;
            }
        } else {
            if (!ReadValue(node + 0x0, next) || next == 0) {
                return false;
            }
        }
        node = next;
    }

    // 某些 block 是直接指针，某些 block 是索引，需要再解一层。
    std::int32_t address_mode = 0;
    uintptr_t block_address = 0;
    if (!ReadValue(node + 0x28, address_mode) || !ReadValue(node + 0x30, block_address)) {
        return false;
    }

    if (address_mode == 1) {
        std::int32_t block_size = 0;
        uintptr_t blocks = 0;
        if (!ReadValue(virtual_memory_flag + 0x20, block_size) ||
            !ReadValue(virtual_memory_flag + 0x28, blocks)) {
            return false;
        }

        const std::uint32_t block_index = static_cast<std::uint32_t>(block_address & 0xFFFFFFFFu);
        block_address = blocks + static_cast<uintptr_t>(block_index) * static_cast<uintptr_t>(block_size);
    } else if (address_mode != 2 || block_address == 0) {
        return false;
    }

    const std::int32_t byte_index = index / 8;
    const int bit_index = 7 - (index - (byte_index * 8));
    return SetBitFlag(block_address + byte_index, bit_index, enabled);
}

}  // namespace ERD::Game
