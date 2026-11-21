#pragma once
/**
 * @file Component.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief ECS - Component
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include <cstdint>

namespace tempest
{
    using ComponentType = uint8_t;
    constexpr ComponentType MAX_COMPONENTS = 32;
} // namespace tempest