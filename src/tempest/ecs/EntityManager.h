#pragma once
/**
 * @file EntityManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief Manager for creating, destroying, and recycling entities.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Entity.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <queue>


namespace tempest
{
    class EntityManager
    {
    public:
        EntityManager() noexcept
        {
            // Add all the entities to the free list
            for (size_t entity = 0; entity < MAX_ENTITY; ++entity)
            {
                _freeEntities.emplace(entity);
            }
        }


        /// Create an entity.
        [[nodiscard]] Entity create() noexcept
        {
            assert(_aliveEntities < MAX_ENTITY && "Limit reached! Cannot allocate any more entities.");

            const Entity id = _freeEntities.front();
            _freeEntities.pop();
            ++_aliveEntities;

            return id;
        }


        /// Destroy the @p entity.
        void destroy(Entity entity) noexcept
        {
            assert(entity < MAX_ENTITY && "Invalid Entity!");

            // Invalidate the signature and add the entity to the freelist.
            _signatures[entity].reset();
            _freeEntities.emplace(entity);
            --_aliveEntities;
        }

        /// Set the component signature for the entity.
        void setSignature(const Entity entity, Signature signature) noexcept
        {
            assert(entity < MAX_ENTITY && "Invalid Entity!");
            _signatures[entity] = std::move(signature);
        }

        /// Get the component signature for the @p entity.
        Signature getSignature(const Entity entity) const noexcept
        {
            assert(entity < MAX_ENTITY && "Invalid Entity!");
            return _signatures[entity];
        }


    private:
        std::queue<Entity> _freeEntities{}; /// < List of all free entities.
        std::array<Signature, MAX_ENTITY> _signatures{};

        uint32_t _aliveEntities{ 0 };
    };
} // namespace tempest