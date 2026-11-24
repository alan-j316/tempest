#pragma once
/**
 * @file System.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 24, 2026
 *
 * @brief System super class.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Entity.h"

#include <set>


namespace tempest
{
    class System
    {
    public:
        std::set<Entity> entities{}; // Set of entities that have the components required by this system.
    };
} // namespace tempest