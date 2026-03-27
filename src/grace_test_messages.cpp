#include "grace_test_messages.hpp"

#include "grace_test_talkscript.hpp"

#include <elden-x/utils/modutils.hpp>

#include <spdlog/spdlog.h>

#include <chrono>
#include <string>
#include <thread>

namespace grace_test::msg {
namespace {

static const unsigned int msgbnd_event_text_for_talk = 33;

namespace CS {
struct MsgRepository;
}

static CS::MsgRepository* msg_repository = nullptr;

static const wchar_t* (*get_message)(CS::MsgRepository* msg_repository,
                                     unsigned int unknown,
                                     unsigned int bnd_id,
                                     int msg_id);

bool contains_cjk(std::wstring_view text) {
    for (const wchar_t ch : text) {
        if ((ch >= 0x4E00 && ch <= 0x9FFF) ||
            (ch >= 0x3400 && ch <= 0x4DBF) ||
            (ch >= 0x3040 && ch <= 0x30FF) ||
            (ch >= 0xAC00 && ch <= 0xD7AF)) {
            return true;
        }
    }
    return false;
}

const wchar_t* get_message_detour(CS::MsgRepository* repository,
                                  unsigned int unknown,
                                  unsigned int bnd_id,
                                  int msg_id) {
    if (bnd_id == msgbnd_event_text_for_talk) {
        if (const wchar_t* custom = grace_test::talkscript::try_get_custom_message(msg_id)) {
            return custom;
        }
    }
    return get_message(repository, unknown, bnd_id, msg_id);
}

}  // namespace

void initialize() {
    auto msg_repository_address = modutils::scan<CS::MsgRepository*>({
        .aob = "48 8B 3D ?? ?? ?? ?? 44 0F B6 30 48 85 FF 75",
        .relative_offsets = {{3, 7}},
    });

    SPDLOG_INFO("Waiting for message repository...");
    while (!(msg_repository = *msg_repository_address)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    modutils::hook(
        {
            .aob = "8B DA 44 8B CA 33 D2 48 8B F9 44 8D 42 6F",
            .offset = 14,
            .relative_offsets = {{1, 5}},
        },
        get_message_detour,
        get_message);

    SPDLOG_INFO("Hooked event_text_for_talk messages for erdGameTools.");
}

std::wstring get_vanilla_message(unsigned int bnd_id, int msg_id) {
    if (msg_repository == nullptr || get_message == nullptr) {
        return {};
    }

    if (const wchar_t* text = get_message(msg_repository, 0, bnd_id, msg_id)) {
        return text;
    }
    return {};
}

std::string detect_game_text_language() {
    const std::wstring probe = get_vanilla_message(msgbnd_event_text_for_talk, event_text_for_talk_sort_chest);
    if (probe.empty()) {
        return "en-US";
    }
    return contains_cjk(probe) ? "zh-CN" : "en-US";
}

}  // namespace grace_test::msg
