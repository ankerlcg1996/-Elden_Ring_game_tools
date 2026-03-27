#pragma once

#include "../Common.hpp"

#include <cwchar>

#include "SingletonRegistry.hpp"

namespace ERD::Game {

// 这里不再依赖外部 tga 头文件，而是直接按新版 CT 里已经验证过的
// Regulation / ParamTable 布局读取参数表。这样工程可以独立编译。

struct ParamTable {
    std::byte reserved0[0x0A];
    std::int16_t num_rows = 0;
};

struct MenuCommonParam {
    float soloPlayDeath_ToFadeOutTime = 0.0f;
    float partyGhostDeath_ToFadeOutTime = 0.0f;
};

struct SpEffectParam {
    std::byte reserved0[0x141];
    std::int8_t saveCategory = 0;
    std::byte reserved1[0x156 - 0x142];
    std::uint16_t stateInfo = 0;
};

struct EquipMtrlSetParam {
    std::int32_t materialId01 = 0;
    std::byte reserved0[0x20 - 0x04];
    std::int8_t itemNum01 = 0;
};

struct CalcCorrectGraph {
    float stageMaxVal0 = 0.0f;
    float stageMaxVal1 = 0.0f;
    float stageMaxVal2 = 0.0f;
    float stageMaxVal3 = 0.0f;
    float stageMaxVal4 = 0.0f;
    float stageMaxGrowVal0 = 0.0f;
    float stageMaxGrowVal1 = 0.0f;
    float stageMaxGrowVal2 = 0.0f;
    float stageMaxGrowVal3 = 0.0f;
    float stageMaxGrowVal4 = 0.0f;
};

// 这些类型在当前工程里只拿来做“遍历某张 Param 表”的标签类型，
// 真正字段读写都走 CT 对应偏移，因此保留为占位结构即可。
struct ShopLineupParam {
    std::byte reserved[1];
};

struct EquipParamWeapon {
    std::byte reserved[1];
};

struct EquipParamProtector {
    std::byte reserved[1];
};

struct EquipParamAccessory {
    std::byte reserved[1];
};

struct EquipParamGoods {
    std::byte reserved[1];
};

struct EquipParamGem {
    std::byte reserved[1];
};

struct MagicParam {
    std::byte reserved[1];
};

struct NpcParam {
    std::byte reserved[1];
};

struct LockCamParam {
    float camDistTarget = 0.0f;
    std::byte reserved0[0x14 - sizeof(float)];
    float camFovY = 0.0f;
};

namespace detail {

inline constexpr std::size_t kRegulationParamListBeginOffset = 0x18;
inline constexpr std::size_t kRegulationParamListEndOffset = 0x20;
inline constexpr std::size_t kParamEntryNameOffset = 0x18;
inline constexpr std::size_t kParamEntryInlineLengthOffset = 0x28;
inline constexpr std::size_t kParamEntryHeaderOffset = 0x80;
inline constexpr std::size_t kParamHeaderTableOffset = 0x80;
inline constexpr std::size_t kTableRowCountOffset = 0x0A;
inline constexpr std::size_t kTableRowsOffset = 0x40;
inline constexpr std::size_t kTableRowStride = 0x18;
inline constexpr std::size_t kTableRowIdOffset = 0x00;
inline constexpr std::size_t kTableRowDataOffset = 0x08;

inline const wchar_t* ResolveParamName(const std::uint8_t* param_entry) {
    const std::int32_t inline_length =
        *reinterpret_cast<const std::int32_t*>(param_entry + kParamEntryInlineLengthOffset);
    if (inline_length > 7) {
        return *reinterpret_cast<const wchar_t* const*>(param_entry + kParamEntryNameOffset);
    }
    return reinterpret_cast<const wchar_t*>(param_entry + kParamEntryNameOffset);
}

inline std::uint8_t* ResolveParamTable(const std::uint8_t* param_entry) {
    const auto* header =
        *reinterpret_cast<std::uint8_t* const*>(param_entry + kParamEntryHeaderOffset);
    if (header == nullptr) {
        return nullptr;
    }

    return *reinterpret_cast<std::uint8_t* const*>(header + kParamHeaderTableOffset);
}

inline std::uint8_t* GetRowData(ParamTable* table, int row_index) {
    auto* table_bytes = reinterpret_cast<std::uint8_t*>(table);
    const auto data_offset = *reinterpret_cast<std::int32_t*>(
        table_bytes + kTableRowsOffset + row_index * kTableRowStride + kTableRowDataOffset
    );
    return table_bytes + data_offset;
}

inline std::uint32_t GetRowId(const ParamTable* table, int row_index) {
    const auto* table_bytes = reinterpret_cast<const std::uint8_t*>(table);
    return *reinterpret_cast<const std::uint32_t*>(
        table_bytes + kTableRowsOffset + row_index * kTableRowStride + kTableRowIdOffset
    );
}

}  // namespace detail

template <typename T>
ParamTable* FindParamTable(const SingletonRegistry& singletons, const wchar_t* param_name) {
    const uintptr_t regulation_manager = singletons.GetObjectPointer("CSRegulationManager");
    if (regulation_manager == 0) {
        return nullptr;
    }

    const auto* regulation_bytes = reinterpret_cast<const std::uint8_t*>(regulation_manager);
    const auto start = *reinterpret_cast<const std::uint8_t* const*>(
        regulation_bytes + detail::kRegulationParamListBeginOffset
    );
    const auto end = *reinterpret_cast<const std::uint8_t* const*>(
        regulation_bytes + detail::kRegulationParamListEndOffset
    );
    if (start == nullptr || end == nullptr || end < start) {
        return nullptr;
    }

    const std::size_t num_params =
        static_cast<std::size_t>(end - start) / sizeof(std::uint64_t);
    for (std::size_t i = 0; i < num_params; ++i) {
        const auto* param_entry =
            *reinterpret_cast<const std::uint8_t* const*>(start + i * sizeof(std::uint64_t));
        if (param_entry == nullptr) {
            continue;
        }

        const wchar_t* current_name = detail::ResolveParamName(param_entry);
        if (current_name == nullptr || std::wcscmp(current_name, param_name) != 0) {
            continue;
        }

        return reinterpret_cast<ParamTable*>(detail::ResolveParamTable(param_entry));
    }

    return nullptr;
}

// 按 param 名和 row id 从 RegulationManager 中取出指定行。
template <typename T>
T* FindParamRow(const SingletonRegistry& singletons, const wchar_t* param_name, std::uint64_t row_id) {
    ParamTable* table = FindParamTable<T>(singletons, param_name);
    if (table == nullptr) {
        return nullptr;
    }

    for (int row_index = 0; row_index < table->num_rows; ++row_index) {
        if (detail::GetRowId(table, row_index) == row_id) {
            return reinterpret_cast<T*>(detail::GetRowData(table, row_index));
        }
    }

    return nullptr;
}

// 遍历某张 param 表的所有行。
template <typename T, typename Fn>
bool ForEachParamRow(const SingletonRegistry& singletons, const wchar_t* param_name, Fn&& fn) {
    ParamTable* table = FindParamTable<T>(singletons, param_name);
    if (table == nullptr) {
        return false;
    }

    for (int row_index = 0; row_index < table->num_rows; ++row_index) {
        auto* row = reinterpret_cast<T*>(detail::GetRowData(table, row_index));
        if (row != nullptr) {
            fn(static_cast<std::uint64_t>(detail::GetRowId(table, row_index)), row);
        }
    }

    return true;
}

}  // namespace ERD::Game

namespace ERD {

using Game::CalcCorrectGraph;
using Game::EquipMtrlSetParam;
using Game::EquipParamAccessory;
using Game::EquipParamGem;
using Game::EquipParamGoods;
using Game::EquipParamProtector;
using Game::EquipParamWeapon;
using Game::MenuCommonParam;
using Game::MagicParam;
using Game::NpcParam;
using Game::LockCamParam;
using Game::ShopLineupParam;
using Game::SpEffectParam;

}  // namespace ERD
