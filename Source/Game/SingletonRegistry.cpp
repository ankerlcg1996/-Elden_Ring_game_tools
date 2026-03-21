#include "SingletonRegistry.hpp"

#include "Memory.hpp"

#include <algorithm>
#include <cstring>

namespace ERD::Game {
namespace {

// 表示 PE 模块中的一段内存区域，例如 .text / .data / .rdata。
struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

// AOB 模式串解析后的字节和掩码。
// 掩码允许处理 CE 脚本中常见的 ? 通配符。
struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

struct SingletonInfo {
    uintptr_t storage_address = 0;
    std::string full_name;
    std::string simple_name;
};

std::size_t SafeStrnlen(const char* text, std::size_t max_len) {
    std::size_t length = 0;
    while (length < max_len && text[length] != '\0') {
        ++length;
    }
    return length;
}

// 读取主模块指定 section 的起止范围，供后面做 AOB 扫描。
bool RegionFromModule(Region& region, const char* module_name, const char* section_name) {
    HMODULE module = GetModuleHandleA(module_name);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<uintptr_t>(module) + dos->e_lfanew
    );

    if (section_name == nullptr) {
        region.base = reinterpret_cast<uintptr_t>(module);
        region.size = nt->OptionalHeader.SizeOfImage;
        return true;
    }

    const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER* section = sections + i;
        if (_stricmp(reinterpret_cast<const char*>(section->Name), section_name) == 0) {
            region.base = reinterpret_cast<uintptr_t>(module) + section->VirtualAddress;
            region.size = section->Misc.VirtualSize;
            return true;
        }
    }

    return false;
}

bool PointerInRegion(uintptr_t pointer, const Region& region) {
    return pointer >= region.base && pointer < (region.base + region.size);
}

int HexValue(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return -1;
}

// 把类似 "48 8B ? ? ? ? ?" 的模式串解析成字节序列。
bool ParsePattern(const char* aob, Pattern& pattern) {
    pattern.bytes.clear();
    pattern.masks.clear();

    std::string token;
    for (const char* cursor = aob;; ++cursor) {
        const char ch = *cursor;
        if (ch == ' ' || ch == '\0') {
            if (!token.empty()) {
                std::uint8_t byte = 0;
                std::uint8_t mask = 0;

                if (token.size() == 1) {
                    const int hi = HexValue(token[0]);
                    if (hi >= 0) {
                        byte = static_cast<std::uint8_t>(hi);
                        mask = 0x0F;
                    } else if (token[0] != '?') {
                        return false;
                    }
                } else if (token.size() == 2) {
                    const int hi = HexValue(token[0]);
                    const int lo = HexValue(token[1]);

                    if (hi >= 0) {
                        byte |= static_cast<std::uint8_t>(hi << 4);
                        mask |= 0xF0;
                    } else if (token[0] != '?') {
                        return false;
                    }

                    if (lo >= 0) {
                        byte |= static_cast<std::uint8_t>(lo);
                        mask |= 0x0F;
                    } else if (token[1] != '?') {
                        return false;
                    }
                } else {
                    return false;
                }

                pattern.bytes.push_back(byte);
                pattern.masks.push_back(mask);
                token.clear();
            }

            if (ch == '\0') {
                break;
            }
        } else {
            token.push_back(ch);
        }
    }

    return !pattern.bytes.empty();
}

bool MatchesPattern(const std::uint8_t* address, const Pattern& pattern) {
    for (std::size_t i = 0; i < pattern.bytes.size(); ++i) {
        if (((address[i] ^ pattern.bytes[i]) & pattern.masks[i]) != 0) {
            return false;
        }
    }
    return true;
}

std::vector<uintptr_t> SearchPattern(const Region& region, const Pattern& pattern, std::size_t max_results) {
    std::vector<uintptr_t> matches;
    if (pattern.bytes.empty() || region.size < pattern.bytes.size()) {
        return matches;
    }

    const auto* address = reinterpret_cast<const std::uint8_t*>(region.base);
    const auto* end = address + region.size - pattern.bytes.size();
    for (; address <= end; ++address) {
        if (MatchesPattern(address, pattern)) {
            matches.push_back(reinterpret_cast<uintptr_t>(address));
            if (matches.size() >= max_results) {
                break;
            }
        }
    }

    return matches;
}

// 调游戏内部函数拿到 FD4 单例名。
// 这里用 SEH 包一层，避免误调用坏指针时直接把游戏打崩。
const char* TryInvokeGetSingletonName(uintptr_t function_address, uintptr_t runtime_class_ptr) {
    using GetSingletonNameFn = const char* (*)(uintptr_t);
    const char* full_name = nullptr;
    __try {
        full_name = reinterpret_cast<GetSingletonNameFn>(function_address)(runtime_class_ptr);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        full_name = nullptr;
    }
    return full_name;
}

// 根据 CT 里同类思路，通过扫描 FD4Singleton 注册逻辑恢复所有已知单例。
std::vector<SingletonInfo> FindFd4Singletons() {
    Region text{};
    Region data{};
    Region rdata{};
    if (!RegionFromModule(text, nullptr, ".text") ||
        !RegionFromModule(data, nullptr, ".data") ||
        !RegionFromModule(rdata, nullptr, ".rdata")) {
        return {};
    }

    Pattern pattern{};
    if (!ParsePattern(
            "48 8b ? ? ? ? ? "
            "48 85 ? "
            "75 2e "
            "48 8d 0d ? ? ? ? "
            "e8 ? ? ? ? "
            "4c 8b c8 "
            "4c 8d 05 ? ? ? ? "
            "ba ? ? 00 00 "
            "48 8d 0d ? ? ? ? "
            "e8 ? ? ? ?",
            pattern)) {
        return {};
    }

    const auto candidates = SearchPattern(text, pattern, 0x4000);
    std::vector<SingletonInfo> singletons;
    singletons.reserve(candidates.size());

    for (uintptr_t candidate : candidates) {
        int32_t rel32 = 0;

        // 单例静态存储地址。
        if (!ReadValue(candidate + 3, rel32)) {
            continue;
        }
        const uintptr_t static_address = candidate + 7 + rel32;
        if (!PointerInRegion(static_address, data)) {
            continue;
        }

        // 单例运行时类型信息。
        if (!ReadValue(candidate + 15, rel32)) {
            continue;
        }
        const uintptr_t runtime_class_ptr = candidate + 19 + rel32;
        if (!PointerInRegion(runtime_class_ptr, data)) {
            continue;
        }

        // 读取单例名的函数地址。
        if (!ReadValue(candidate + 20, rel32)) {
            continue;
        }
        const uintptr_t get_singleton_name = candidate + 24 + rel32;
        if (!PointerInRegion(get_singleton_name, text)) {
            continue;
        }

        // 注册源文件字符串，确保确实命中 FD4Singleton 逻辑。
        if (!ReadValue(candidate + 42, rel32)) {
            continue;
        }
        const uintptr_t filepath_ptr_address = candidate + 46 + rel32;
        if (!PointerInRegion(filepath_ptr_address, rdata)) {
            continue;
        }

        const char* filepath_ptr = reinterpret_cast<const char*>(filepath_ptr_address);
        const std::size_t filepath_len = SafeStrnlen(filepath_ptr, 256);
        if (filepath_len < 14 || filepath_len == 256) {
            continue;
        }
        if (std::strcmp(filepath_ptr + filepath_len - 14, "FD4Singleton.h") != 0) {
            continue;
        }

        const char* full_name = TryInvokeGetSingletonName(get_singleton_name, runtime_class_ptr);
        if (full_name == nullptr) {
            continue;
        }

        const char* simple_name = std::strrchr(full_name, ':');
        simple_name = (simple_name == nullptr) ? full_name : (simple_name + 1);

        const auto duplicate = std::find_if(
            singletons.begin(),
            singletons.end(),
            [static_address](const SingletonInfo& info) { return info.storage_address == static_address; }
        );
        if (duplicate != singletons.end()) {
            continue;
        }

        singletons.push_back(SingletonInfo{
            static_address,
            std::string(full_name),
            std::string(simple_name),
        });
    }

    return singletons;
}

}  // namespace

bool SingletonRegistry::Initialize() {
    storage_by_name_.clear();

    const auto singletons = FindFd4Singletons();
    for (const SingletonInfo& info : singletons) {
        storage_by_name_.emplace(info.simple_name, info.storage_address);
    }

    return !storage_by_name_.empty();
}

uintptr_t SingletonRegistry::GetStorage(std::string_view name) const {
    const auto it = storage_by_name_.find(std::string(name));
    return (it == storage_by_name_.end()) ? 0 : it->second;
}

uintptr_t SingletonRegistry::GetObjectPointer(std::string_view name) const {
    const uintptr_t storage = GetStorage(name);
    if (storage == 0) {
        return 0;
    }

    uintptr_t object = 0;
    if (!ReadValue(storage, object)) {
        return 0;
    }

    return object;
}

}  // namespace ERD::Game
