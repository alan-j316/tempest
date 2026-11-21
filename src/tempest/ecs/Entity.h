#pragma once
/**
 * @file Entity.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief ECS - Entity
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "components/Component.h"

#include <bitset>
#include <cstdint>

namespace tempest
{
    using Entity = uint32_t;

    constexpr Entity MAX_ENTITY = 5000;

    using Signature = std::bitset<MAX_COMPONENTS>;
} // namespace tempest