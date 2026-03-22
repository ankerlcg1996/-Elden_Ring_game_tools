#pragma once

#include <filesystem>

namespace grace_test::runtime {

void run(const std::filesystem::path& folder);
void request_stop();

}  // namespace grace_test::runtime
