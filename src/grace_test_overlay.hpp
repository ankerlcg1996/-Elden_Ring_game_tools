#pragma once

#include <filesystem>

namespace grace_test::overlay {

void initialize(const std::filesystem::path& folder);
void tick();
void shutdown();

}  // namespace grace_test::overlay
