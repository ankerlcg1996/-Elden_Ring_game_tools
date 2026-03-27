#pragma once

#include "../Common.hpp"

#include "Params.hpp"
#include "SingletonRegistry.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace ERD::Game {

struct ParamHeaderRow {
    std::uint32_t row_id = 0;
    std::uint32_t pad0x4 = 0;
    std::uint32_t param_offset = 0;
    std::uint32_t pad0xC = 0;
    std::uint32_t string_offset = 0;
    std::uint32_t pad0x14 = 0;
};
static_assert(sizeof(ParamHeaderRow) == 0x18);

struct ParamHeader {
    std::uint32_t string_offset = 0;
    std::uint8_t reserved0x4[0x6]{};
    std::uint16_t row_count = 0;
    std::uint8_t reserved0xC[0x4]{};
    std::uint64_t param_type_offset = 0;
    std::uint8_t reserved0x18[0x18]{};
    std::uint32_t data_offset = 0;
    std::uint8_t reserved0x34[0xC]{};
    ParamHeaderRow table;
};
static_assert(sizeof(ParamHeader) == 0x58);

namespace detail {

inline const wchar_t* ResolveParamNameForEditor(const std::uint8_t* param_entry) {
    const std::int32_t inline_length =
        *reinterpret_cast<const std::int32_t*>(param_entry + detail::kParamEntryInlineLengthOffset);
    if (inline_length > 7) {
        return *reinterpret_cast<const wchar_t* const*>(param_entry + detail::kParamEntryNameOffset);
    }
    return reinterpret_cast<const wchar_t*>(param_entry + detail::kParamEntryNameOffset);
}

}  // namespace detail

template <typename T>
class ParamEditor {
public:
    explicit ParamEditor(const SingletonRegistry& singletons, const wchar_t* param_name)
        : param_name_(param_name == nullptr ? L"" : param_name) {
        Initialize(singletons);
    }

    bool IsValid() const {
        return valid_;
    }

    T* AddEntry(std::uint32_t row_id) {
        if (!valid_) {
            return nullptr;
        }

        if (std::find(row_ids_.begin(), row_ids_.end(), row_id) != row_ids_.end()) {
            return nullptr;
        }

        row_ids_.push_back(row_id);
        row_data_.push_back(default_entry_);
        return &row_data_.back();
    }

    T* RetrieveEntry(std::uint32_t row_id) {
        if (!valid_) {
            return nullptr;
        }

        auto found = std::find(row_ids_.begin(), row_ids_.end(), row_id);
        if (found == row_ids_.end()) {
            return nullptr;
        }

        const std::size_t index = static_cast<std::size_t>(std::distance(row_ids_.begin(), found));
        if (index < original_row_count_) {
            auto* bytes = reinterpret_cast<std::uint8_t*>(header_);
            const auto data_offset = *reinterpret_cast<std::int32_t*>(
                bytes + detail::kTableRowsOffset + static_cast<int>(index) * detail::kTableRowStride +
                detail::kTableRowDataOffset);
            return reinterpret_cast<T*>(bytes + data_offset);
        }

        return &row_data_[index - original_row_count_];
    }

    bool AddNewParams() {
        if (!valid_ || row_data_.empty()) {
            return false;
        }

        const std::uint32_t param_size = *reinterpret_cast<std::uint32_t*>(param_pointer_ - 0x10);
        const std::uint32_t num_entries = original_row_count_;
        const std::size_t new_entries = row_data_.size();

        const std::uint32_t new_table_entry_size = static_cast<std::uint32_t>(sizeof(ParamHeaderRow) * new_entries);
        const std::uint32_t new_data_entry_size = static_cast<std::uint32_t>(sizeof(T) * new_entries);
        const std::uint32_t new_data_size = new_table_entry_size + new_data_entry_size;
        const std::size_t new_param_size = static_cast<std::size_t>(new_data_size) + param_size;

        constexpr std::size_t kParamPrefixSize = 0x10;
        std::vector<std::uint8_t> new_param_storage(new_param_size + kParamPrefixSize, 0);
        *reinterpret_cast<std::uint32_t*>(new_param_storage.data()) = static_cast<std::uint32_t>(new_param_size);
        auto* new_param_header = reinterpret_cast<ParamHeader*>(new_param_storage.data() + kParamPrefixSize);

        std::memcpy(new_param_header, header_, header_->data_offset);

        new_param_header->data_offset += new_table_entry_size;
        new_param_header->string_offset += new_data_size;
        new_param_header->param_type_offset += new_data_size;
        new_param_header->row_count = static_cast<std::uint16_t>(new_param_header->row_count + new_entries);

        auto* new_param_table = &new_param_header->table;
        for (std::uint32_t i = 0; i < num_entries; ++i) {
            new_param_table[i].param_offset += new_table_entry_size;
            new_param_table[i].string_offset += new_data_size;
        }

        const std::string param_type = std::string(reinterpret_cast<char*>(param_pointer_ + header_->string_offset));
        for (std::uint32_t i = num_entries; i < new_param_header->row_count; ++i) {
            new_param_table[i].row_id = row_ids_[i];
            new_param_table[i].param_offset = new_param_header->data_offset + (i * sizeof(T));
            new_param_table[i].string_offset = new_param_header->string_offset + static_cast<std::uint32_t>(param_type.size());
        }

        const uintptr_t new_param_pointer = reinterpret_cast<uintptr_t>(new_param_header);
        const uintptr_t new_data_pointer = new_param_pointer + new_param_header->data_offset;

        const std::size_t old_data_size = sizeof(T) * original_row_count_;
        std::memcpy(
            reinterpret_cast<void*>(new_data_pointer),
            reinterpret_cast<void*>(param_pointer_ + header_->data_offset),
            old_data_size);

        std::memcpy(
            reinterpret_cast<void*>(new_data_pointer + old_data_size),
            row_data_.data(),
            sizeof(T) * new_entries);

        std::memcpy(
            reinterpret_cast<void*>(new_param_pointer + new_param_header->string_offset),
            reinterpret_cast<void*>(param_pointer_ + header_->string_offset),
            param_size - header_->string_offset);

        return ReloadParam(std::move(new_param_storage));
    }

private:
    bool ReloadParam(std::vector<std::uint8_t>&& rebuilt_param) {
        if (header_holder_ == 0 || rebuilt_param.size() <= 0x10) {
            return false;
        }

        auto storage = std::make_shared<std::vector<std::uint8_t>>(std::move(rebuilt_param));
        auto* new_table = reinterpret_cast<std::uint8_t*>(storage->data() + 0x10);
        *reinterpret_cast<std::uint8_t**>(header_holder_ + detail::kParamHeaderTableOffset) = new_table;

        reloaded_param_storage_.push_back(std::move(storage));
        return true;
    }

    void Initialize(const SingletonRegistry& singletons) {
        const uintptr_t regulation_manager = singletons.GetObjectPointer("CSRegulationManager");
        if (regulation_manager == 0) {
            return;
        }

        const auto* regulation_bytes = reinterpret_cast<const std::uint8_t*>(regulation_manager);
        const auto* start = *reinterpret_cast<const std::uint8_t* const*>(
            regulation_bytes + detail::kRegulationParamListBeginOffset);
        const auto* end = *reinterpret_cast<const std::uint8_t* const*>(
            regulation_bytes + detail::kRegulationParamListEndOffset);
        if (start == nullptr || end == nullptr || end < start) {
            return;
        }

        const std::size_t num_params = static_cast<std::size_t>(end - start) / sizeof(std::uint64_t);
        for (std::size_t i = 0; i < num_params; ++i) {
            auto* param_entry = *reinterpret_cast<std::uint8_t* const*>(start + i * sizeof(std::uint64_t));
            if (param_entry == nullptr) {
                continue;
            }

            const wchar_t* current_name = detail::ResolveParamNameForEditor(param_entry);
            if (current_name == nullptr || std::wcscmp(current_name, param_name_.c_str()) != 0) {
                continue;
            }

            param_entry_ = reinterpret_cast<uintptr_t>(param_entry);
            header_holder_ = *reinterpret_cast<uintptr_t*>(param_entry_ + detail::kParamEntryHeaderOffset);
            if (header_holder_ == 0) {
                return;
            }

            param_pointer_ = *reinterpret_cast<uintptr_t*>(header_holder_ + detail::kParamHeaderTableOffset);
            if (param_pointer_ == 0) {
                return;
            }

            header_ = reinterpret_cast<ParamHeader*>(param_pointer_);
            original_row_count_ = header_->row_count;
            if (original_row_count_ == 0) {
                return;
            }
            if (original_row_count_ >= 2) {
                const auto* table_rows = &header_->table;
                const std::uint32_t runtime_entry_size = table_rows[1].param_offset - table_rows[0].param_offset;
                if (runtime_entry_size != sizeof(T)) {
                    return;
                }
            }

            auto* table_rows = &header_->table;
            for (std::uint32_t row_index = 0; row_index < original_row_count_; ++row_index) {
                row_ids_.push_back(table_rows[row_index].row_id);
            }

            auto* first_row = reinterpret_cast<T*>(
                reinterpret_cast<std::uint8_t*>(header_) + table_rows[0].param_offset);
            default_entry_ = *first_row;
            valid_ = true;
            return;
        }
    }

    std::wstring param_name_;
    uintptr_t param_entry_ = 0;
    uintptr_t header_holder_ = 0;
    uintptr_t param_pointer_ = 0;
    ParamHeader* header_ = nullptr;
    std::uint32_t original_row_count_ = 0;
    bool valid_ = false;

    T default_entry_{};
    std::vector<std::uint32_t> row_ids_;
    std::vector<T> row_data_;

    static inline std::vector<std::shared_ptr<std::vector<std::uint8_t>>> reloaded_param_storage_{};
};

}  // namespace ERD::Game
