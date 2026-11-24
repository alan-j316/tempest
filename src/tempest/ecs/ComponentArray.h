#pragma once
/**
 * @file ComponentArray.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief Storage for components.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Entity.h"

#include <cassert>
#include <array>
#include <unordered_map>

namespace tempest
{
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() noexcept                    = default;
        virtual void onEntityDestroyed(Entity entity) noexcept = 0;
    };



    template <typename T>
    class ComponentArray: IComponentArray
    {
    public:
        void insert(const Entity entity, T component) noexcept
        {
            assert(!_entityToIndexMap.contains(entity) && "Duplicate component entry for entity.");

            // Get a new packed index and added the component at that index.
            size_t newIndex             = _size;
            _entityToIndexMap[entity]   = newIndex;
            _indexToEntityMap[newIndex] = entity;
            _components[newIndex]       = std::move(component);
            ++_size;
        }

        /// Remove the component associated with @p entity.
        void remove(const Entity entity)
        {
            assert(_entityToIndexMap.contains(entity) && "Component doesn't exists on entity!");

            // Remove the element by swapping it out with the last element and updating all the references
            // to it.
            size_t index           = _entityToIndexMap[entity];
            size_t lastEntityIndex = _size - 1;
            _components[index]     = _components[lastEntityIndex];

            const Entity lastEntity   = _indexToEntityMap[lastEntityIndex];
            _entityToIndexMap[entity] = index;
            _indexToEntityMap[index]  = lastEntity;

            _entityToIndexMap.erase(entity);
            _indexToEntityMap.erase(lastEntityIndex);

            --_size;
        }


        [[nodiscard]] T& get(const Entity entity) const noexcept
        {
            assert(_entityToIndexMap.contains(entity) && "Component doesn't exists on entity!");
            return _components[_entityToIndexMap.at(entity)];
        }


        void onEntityDestroyed(const Entity entity) noexcept override
        {
            // Trigger/Signal
            // Remove the entity from component array if it was entity was destroyed
            if (_entityToIndexMap.contains(entity))
            {
                remove(entity);
            }
        }


    private:
        std::array<T, MAX_ENTITY> _components{};

        std::unordered_map<Entity, size_t> _entityToIndexMap{};
        std::unordered_map<size_t, Entity> _indexToEntityMap{};

        size_t _size{}; // Total Valid entries in the ECS.
    };

} // namespace tempest