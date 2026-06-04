#pragma once

namespace grace_test::talkscript {

void initialize();
void reset_runtime_state();
const wchar_t* try_get_custom_message(int msg_id);

}  // namespace grace_test::talkscript
