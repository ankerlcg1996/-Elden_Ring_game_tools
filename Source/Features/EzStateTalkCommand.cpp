#include "EzStateTalkCommand.hpp"

#include "../Game/Memory.hpp"
#include "../Main/Logger.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace ERD::Features {
namespace {

constexpr uintptr_t kNetPlayersOffset = 0x10EF8;
constexpr uintptr_t kPlayerHandleOffset = 0x8;

constexpr const char* kExecuteTalkCommandTemplate = R"(48 89 e5                mov    rbp,rsp
48 81 ec 98 05 00 00    sub    rsp,0x598
48 8d 8d e0 fb ff ff    lea    rcx,[rbp-0x420]
ba 00 00 00 00          mov    edx,0x0
e8 00 00 00 00          call   1b <_main+0x1b>
48 8d 8d c8 fb ff ff    lea    rcx,[rbp-0x438]
48 8d 95 c8 fa ff ff    lea    rdx,[rbp-0x538]
48 89 51 10             mov    QWORD PTR [rcx+0x10],rdx
4c 8d 85 a8 fa ff ff    lea    r8,[rbp-0x558]
4c 89 82 98 00 00 00    mov    QWORD PTR [rdx+0x98],r8
49 89 50 18             mov    QWORD PTR [r8+0x18],rdx
48 b8 00 00 00 00 00    movabs rax,0x0
00 00 00
48 89 42 40             mov    QWORD PTR [rdx+0x40],rax
bb 00 00 00 00          mov    ebx,0x0
89 9d 20 fc ff ff       mov    DWORD PTR [rbp-0x3e0],ebx
31 ff                   xor    edi,edi
48 8d 35 00 00 00 00    lea    rsi,[rip+0x0]        # 61 <param_loop>
000000000061 <param_loop>:
8b 04 be                mov    eax,DWORD PTR [rsi+rdi*4]
4c 8d 85 98 fa ff ff    lea    r8,[rbp-0x568]
41 89 00                mov    DWORD PTR [r8],eax
41 c7 40 08 02 00 00    mov    DWORD PTR [r8+0x8],0x2
00
8d 57 01                lea    edx,[rdi+0x1]
48 8d 8d e0 fb ff ff    lea    rcx,[rbp-0x420]
48 8b 01                mov    rax,QWORD PTR [rcx]
ff 50 08                call   QWORD PTR [rax+0x8]
ff c7                   inc    edi
39 df                   cmp    edi,ebx
7c d5                   jl     61 <param_loop>
48 8d 8d c8 fb ff ff    lea    rcx,[rbp-0x438]
48 8d 95 e0 fb ff ff    lea    rdx,[rbp-0x420]
e8 00 00 00 00          call   9f <param_loop+0x3e>
48 81 c4 98 05 00 00    add    rsp,0x598
c3                      ret)";

struct AobAbsoluteCandidate {
    const char* pattern = nullptr;
    std::ptrdiff_t offset_adjust = 0;
};

constexpr std::array<AobAbsoluteCandidate, 1> kExternalEventTempCtorCandidates{{
    {"C7 41 10 02 00 00 00 89 51", -0xD},
}};

constexpr std::array<AobAbsoluteCandidate, 1> kExecuteTalkCommandCandidates{{
    {"89 7D 80 48 8B 02 48 8B CA", -0x4F},
}};

struct Region {
    uintptr_t base = 0;
    std::size_t size = 0;
};

struct Pattern {
    std::vector<std::uint8_t> bytes;
    std::vector<std::uint8_t> masks;
};

struct CodeCaveLayout {
    std::array<std::int32_t, 4> params{};
    std::array<std::uint8_t, 0x200> code{};
};

uintptr_t g_external_event_temp_ctor = 0;
uintptr_t g_execute_talk_command = 0;
constexpr std::uint64_t kCodeCaveSearchStart = 0x40000000ull;
constexpr std::uint64_t kCodeCaveSearchEnd = 0x30000ull;
constexpr std::uint64_t kCodeCaveSearchStep = 0x10000ull;

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
                if (token.size() == 2) {
                    const int hi = HexValue(token[0]);
                    const int lo = HexValue(token[1]);
                    if (hi < 0 || lo < 0) {
                        return false;
                    }
                    byte = static_cast<std::uint8_t>((hi << 4) | lo);
                    mask = 0xFF;
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

bool RegionFromMainModuleText(Region& region) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<uintptr_t>(module) + dos->e_lfanew);
    const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const IMAGE_SECTION_HEADER* section = sections + i;
        if (_stricmp(reinterpret_cast<const char*>(section->Name), ".text") == 0) {
            region.base = reinterpret_cast<uintptr_t>(module) + section->VirtualAddress;
            region.size = section->Misc.VirtualSize;
            return true;
        }
    }

    return false;
}

bool MatchesPattern(const std::uint8_t* address, const Pattern& pattern) {
    for (std::size_t i = 0; i < pattern.bytes.size(); ++i) {
        if (((address[i] ^ pattern.bytes[i]) & pattern.masks[i]) != 0) {
            return false;
        }
    }
    return true;
}

uintptr_t FindPatternInText(const char* aob) {
    Pattern pattern{};
    Region text{};
    if (!ParsePattern(aob, pattern) || !RegionFromMainModuleText(text) || text.size < pattern.bytes.size()) {
        return 0;
    }

    const auto* begin = reinterpret_cast<const std::uint8_t*>(text.base);
    const auto* end = begin + text.size - pattern.bytes.size();
    for (const auto* cursor = begin; cursor <= end; ++cursor) {
        if (MatchesPattern(cursor, pattern)) {
            return reinterpret_cast<uintptr_t>(cursor);
        }
    }
    return 0;
}

template <std::size_t N>
uintptr_t FindByAbsoluteCandidates(const std::array<AobAbsoluteCandidate, N>& candidates) {
    for (const AobAbsoluteCandidate& candidate : candidates) {
        if (candidate.pattern == nullptr) {
            continue;
        }

        const uintptr_t match = FindPatternInText(candidate.pattern);
        if (match == 0) {
            continue;
        }

        const auto adjusted_signed =
            static_cast<std::int64_t>(match) + static_cast<std::int64_t>(candidate.offset_adjust);
        if (adjusted_signed <= 0) {
            continue;
        }

        return static_cast<uintptr_t>(adjusted_signed);
    }

    return 0;
}

std::vector<std::uint8_t> ParseAsmTemplate(std::string_view text) {
    std::vector<std::uint8_t> bytes;
    std::string line;
    line.reserve(128);
    auto flush_line = [&]() {
        std::size_t pos = 0;
        while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])) != 0) {
            ++pos;
        }
        while (pos < line.size()) {
            std::size_t token_end = pos;
            while (token_end < line.size() && std::isspace(static_cast<unsigned char>(line[token_end])) == 0) {
                ++token_end;
            }
            const std::string_view token(line.data() + pos, token_end - pos);
            if (token.size() != 2) {
                break;
            }
            const int hi = HexValue(token[0]);
            const int lo = HexValue(token[1]);
            if (hi < 0 || lo < 0) {
                break;
            }
            bytes.push_back(static_cast<std::uint8_t>((hi << 4) | lo));
            pos = token_end;
            while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])) != 0) {
                ++pos;
            }
        }
        line.clear();
    };

    for (char ch : text) {
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            flush_line();
            continue;
        }
        line.push_back(ch);
    }
    flush_line();
    return bytes;
}

void PatchRel32(
    std::vector<std::uint8_t>& code,
    std::size_t instruction_offset,
    uintptr_t instruction_base,
    uintptr_t absolute_target) {
    const uintptr_t instruction_address = instruction_base + instruction_offset;
    const uintptr_t next_ip = instruction_address + 5;
    const auto rel = static_cast<std::int32_t>(static_cast<std::int64_t>(absolute_target) - static_cast<std::int64_t>(next_ip));
    std::memcpy(code.data() + instruction_offset + 1, &rel, sizeof(rel));
}

void PatchRel32WithBase(
    std::vector<std::uint8_t>& code,
    std::size_t instruction_offset,
    uintptr_t instruction_base,
    std::size_t immediate_offset,
    uintptr_t absolute_target,
    std::size_t instruction_length) {
    const uintptr_t next_ip = instruction_base + instruction_offset + instruction_length;
    const auto rel = static_cast<std::int32_t>(static_cast<std::int64_t>(absolute_target) - static_cast<std::int64_t>(next_ip));
    std::memcpy(code.data() + instruction_offset + immediate_offset, &rel, sizeof(rel));
}

void PatchImmediate32(std::vector<std::uint8_t>& code, std::size_t offset, std::int32_t value) {
    std::memcpy(code.data() + offset, &value, sizeof(value));
}

void PatchImmediate64(std::vector<std::uint8_t>& code, std::size_t offset, std::uint64_t value) {
    std::memcpy(code.data() + offset, &value, sizeof(value));
}

bool ResolveEzStateFunctions() {
    if (g_external_event_temp_ctor == 0) {
        g_external_event_temp_ctor = FindByAbsoluteCandidates(kExternalEventTempCtorCandidates);
    }
    if (g_execute_talk_command == 0) {
        g_execute_talk_command = FindByAbsoluteCandidates(kExecuteTalkCommandCandidates);
    }
    return g_external_event_temp_ctor != 0 && g_execute_talk_command != 0;
}

void* AllocateCodeCaveNearModule(std::size_t size) {
    HMODULE module = GetModuleHandleW(nullptr);
    if (module == nullptr) {
        return nullptr;
    }

    const uintptr_t module_base = reinterpret_cast<uintptr_t>(module);
    const uintptr_t search_start = module_base > kCodeCaveSearchStart ? module_base - kCodeCaveSearchStart : 0;
    const uintptr_t search_end = module_base > kCodeCaveSearchEnd ? module_base - kCodeCaveSearchEnd : 0;

    for (uintptr_t address = search_end; address > search_start; address -= kCodeCaveSearchStep) {
        void* allocated = VirtualAlloc(
            reinterpret_cast<void*>(address),
            size,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_EXECUTE_READWRITE);
        if (allocated != nullptr) {
            return allocated;
        }
        if (address < kCodeCaveSearchStep) {
            break;
        }
    }

    return VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

std::uint64_t GetPlayerHandle(const Game::SingletonRegistry& singletons) {
    const uintptr_t world_chr_man = singletons.GetObjectPointer("WorldChrMan");
    uintptr_t slots_root = 0;
    uintptr_t player_base = 0;
    std::uint64_t player_handle = static_cast<std::uint64_t>(-1);
    if (world_chr_man == 0 ||
        !Game::ReadValue(world_chr_man + kNetPlayersOffset, slots_root) ||
        slots_root == 0 ||
        !Game::ReadValue(slots_root, player_base) ||
        player_base == 0 ||
        !Game::ReadValue(player_base + kPlayerHandleOffset, player_handle)) {
        return static_cast<std::uint64_t>(-1);
    }
    return player_handle;
}

bool RunShellcodeAndWait(void* code_address) {
    auto thread_proc = [](LPVOID parameter) -> DWORD {
        auto shellcode = reinterpret_cast<void(*)()>(parameter);
        __try {
            shellcode();
            return 0;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return 1;
        }
    };

    HANDLE thread = CreateThread(
        nullptr,
        0,
        thread_proc,
        code_address,
        0,
        nullptr);
    if (thread == nullptr) {
        return false;
    }

    const DWORD wait_result = WaitForSingleObject(thread, 10000);
    DWORD exit_code = 0;
    GetExitCodeThread(thread, &exit_code);
    CloseHandle(thread);
    return wait_result == WAIT_OBJECT_0 && exit_code == 0;
}

}  // namespace

bool ExecuteEzStateTalkCommand(
    const Game::SingletonRegistry& singletons,
    const EzStateTalkCommand& command) {
    if (!ResolveEzStateFunctions()) {
        Main::Logger::Instance().Error("ExecuteEzStateTalkCommand failed: function addresses not resolved.");
        return false;
    }

    const std::uint64_t chr_handle = command.use_player_handle ? GetPlayerHandle(singletons) : 0;
    if (command.use_player_handle && chr_handle == static_cast<std::uint64_t>(-1)) {
        Main::Logger::Instance().Error("ExecuteEzStateTalkCommand failed: player handle unavailable.");
        return false;
    }

    auto* cave = static_cast<CodeCaveLayout*>(AllocateCodeCaveNearModule(sizeof(CodeCaveLayout)));
    if (cave == nullptr) {
        Main::Logger::Instance().Error("ExecuteEzStateTalkCommand failed: VirtualAlloc returned null.");
        return false;
    }

    std::fill(cave->params.begin(), cave->params.end(), 0);
    for (int index = 0; index < command.param_count && index < static_cast<int>(cave->params.size()); ++index) {
        cave->params[static_cast<std::size_t>(index)] =
            static_cast<std::int32_t>(command.params[static_cast<std::size_t>(index)]);
    }

    std::vector<std::uint8_t> code = ParseAsmTemplate(kExecuteTalkCommandTemplate);
    if (code.empty() || code.size() > cave->code.size()) {
        Main::Logger::Instance().Error("ExecuteEzStateTalkCommand failed: shellcode template parse error.");
        VirtualFree(cave, 0, MEM_RELEASE);
        return false;
    }

    const uintptr_t code_base = reinterpret_cast<uintptr_t>(cave->code.data());
    const uintptr_t params_base = reinterpret_cast<uintptr_t>(cave->params.data());

    PatchImmediate32(code, 0x11 + 1, command.command_id);
    PatchRel32(code, 0x16, code_base, g_external_event_temp_ctor);
    PatchImmediate64(code, 0x3F + 2, chr_handle);
    PatchImmediate32(code, 0x4D + 1, std::max(command.param_count, 0));
    PatchRel32WithBase(code, 0x5A, code_base, 3, params_base, 7);
    PatchRel32(code, 0x9A, code_base, g_execute_talk_command);

    std::memcpy(cave->code.data(), code.data(), code.size());

    const bool ok = RunShellcodeAndWait(cave->code.data());
    std::ostringstream stream;
    stream << "ExecuteEzStateTalkCommand cmd=" << command.command_id
           << " paramCount=" << command.param_count
           << " usePlayerHandle=" << (command.use_player_handle ? 1 : 0)
           << " externalEventTempCtor=0x" << std::hex << g_external_event_temp_ctor
           << " executeTalkCommand=0x" << std::hex << g_execute_talk_command
           << " result=" << (ok ? "ok" : "failed");
    Main::Logger::Instance().Info(stream.str().c_str());
    VirtualFree(cave, 0, MEM_RELEASE);
    return ok;
}

}  // namespace ERD::Features
