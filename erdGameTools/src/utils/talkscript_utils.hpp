#pragma once

#include <elden-x/ezstate/ezstate.hpp>
#include <elden-x/ezstate/talk_commands.hpp>

#include <array>
#include <memory>
#include <vector>

using int_expression = std::array<unsigned char, 6>;

static constexpr int_expression make_int_expression(int value) {
    return {
        0x82,
        static_cast<unsigned char>((value & 0x000000ff)),
        static_cast<unsigned char>((value & 0x0000ff00) >> 8),
        static_cast<unsigned char>((value & 0x00ff0000) >> 16),
        static_cast<unsigned char>((value & 0xff000000) >> 24),
        0xa1,
    };
}

static int get_ezstate_int_value(const er::ezstate::expression arg) {
    if (arg.size() == 2) {
        return arg[0] - 64;
    }

    if (arg.size() == 6 && arg[0] == 0x82) {
        return *reinterpret_cast<const int*>(&arg[1]);
    }

    return -1;
}

static constexpr std::array<unsigned char, 9> make_talk_list_result_expression(int value) {
    return {
        0x57,
        0x84,
        0x82,
        static_cast<unsigned char>((value & 0x000000ff)),
        static_cast<unsigned char>((value & 0x0000ff00) >> 8),
        static_cast<unsigned char>((value & 0x00ff0000) >> 16),
        static_cast<unsigned char>((value & 0xff000000) >> 24),
        0x95,
        0xa1,
    };
}

static auto placeholder_expression = make_int_expression(-1);
static auto generic_dialog_shop_message = make_int_expression(0);
static auto show_generic_dialog_shop_message_args =
    std::array{er::ezstate::expression(generic_dialog_shop_message)};
static auto true_expression = std::array<unsigned char, 2>{0x41, 0xa1};
static auto talk_menu_closed_expression = std::array<unsigned char, 15>{
    0x7b, 0x41, 0x40, 0x86, 0x41, 0x95, 0x7a, 0x40, 0x85, 0x40, 0x95, 0x98, 0x40, 0x95, 0xa1
};

class talkscript_menu_option {
public:
    std::array<unsigned char, 6> index;
    std::array<unsigned char, 6> message;
    std::array<unsigned char, 9> condition;
    std::array<er::ezstate::expression, 3> args;
    er::ezstate::transition transition;
    bool is_default = false;

    talkscript_menu_option()
        : talkscript_menu_option(-1, -1, nullptr) {}

    talkscript_menu_option(const talkscript_menu_option& other)
        : index(other.index),
          message(other.message),
          condition(other.condition),
          args{this->index, this->message, placeholder_expression},
          transition(other.transition.target_state,
                     other.is_default ? er::ezstate::expression{true_expression}
                                      : er::ezstate::expression{this->condition}),
          is_default(other.is_default) {}

    talkscript_menu_option(int index_value,
                           int message_id,
                           er::ezstate::state* target_state,
                           bool default_option = false)
        : index(make_int_expression(index_value)),
          message(make_int_expression(message_id)),
          condition(make_talk_list_result_expression(index_value)),
          args{this->index, this->message, placeholder_expression},
          transition(target_state,
                     default_option ? er::ezstate::expression{true_expression}
                                    : er::ezstate::expression{this->condition}),
          is_default(default_option) {}
};

class talkscript_menu_state : public er::ezstate::state {
public:
    std::vector<talkscript_menu_option> opts;
    std::unique_ptr<er::ezstate::event[]> events;
    er::ezstate::transition transition{&branch_state, er::ezstate::expression{talk_menu_closed_expression}};
    std::array<er::ezstate::transition*, 1> transition_array{&transition};

    er::ezstate::state branch_state;
    std::unique_ptr<er::ezstate::transition*[]> branch_transitions;

    talkscript_menu_state() {
        transitions = er::ezstate::transitions{transition_array};
    }

    explicit talkscript_menu_state(std::vector<talkscript_menu_option>&& opts_arg)
        : talkscript_menu_state() {
        rebuild(std::move(opts_arg));
    }

    void rebuild(std::vector<talkscript_menu_option>&& opts_arg) {
        opts = std::move(opts_arg);

        branch_transitions = std::make_unique<er::ezstate::transition*[]>(opts.size());
        events = std::make_unique<er::ezstate::event[]>(opts.size() + 3);

        std::size_t event_count = 0;
        std::size_t branch_transition_count = 0;
        events[event_count++] = {er::talk_command::close_shop_message};
        events[event_count++] = {er::talk_command::clear_talk_list_data};

        for (auto& opt : opts) {
            events[event_count++] = {er::talk_command::add_talk_list_data, opt.args};
            branch_transitions[branch_transition_count++] = &opt.transition;
        }

        events[event_count++] =
            {er::talk_command::show_shop_message, show_generic_dialog_shop_message_args};

        entry_events = er::ezstate::events{events.get(), event_count};
        transitions = er::ezstate::transitions{transition_array};
        branch_state.transitions =
            er::ezstate::transitions{branch_transitions.get(), branch_transition_count};
    }
};

struct callback_state : public er::ezstate::state {
    using Callback = void (*)(int);

    er::ezstate::transition transition;
    std::array<er::ezstate::transition*, 1> transition_array{&transition};
    Callback callback = nullptr;
    int payload = 0;

    callback_state(Callback callback_fn, int payload_value, er::ezstate::state* return_state)
        : transition(return_state, er::ezstate::expression{true_expression}),
          callback(callback_fn),
          payload(payload_value) {
        transitions = er::ezstate::transitions{transition_array};
    }

    void set_return_state(er::ezstate::state* return_state) {
        transition.target_state = return_state;
    }

    void execute() const {
        if (callback != nullptr) {
            callback(payload);
        }
    }
};
