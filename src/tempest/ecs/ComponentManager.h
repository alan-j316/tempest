#pragma once
/**
 * @file ComponentManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief Manages component storage and manipulation.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "ComponentArray.h"

#include <memory>
#include <ranges>

namespace tempest
{

    class ComponentManager
    {
    public:
        /// Register the given component with the manager.
        /// @tparam Component The component type to register.
        template <typename Component>
        constexpr void registerComponent() noexcept
        {
            const auto typeName = typeid(Component).name();
            assert(_componentTypes.find(typeName) == _componentTypes.cend() &&
                   "[ECS]: Duplication registration. Component already registered!");

            // Add component to the maps
            _componentTypes.insert({ typeName, _nextAvailableType });
            _componentTypes.insert({ typeName, std::make_shared<ComponentArray<Component>>() });
            ++_nextAvailableType;
        }


        /// Get the component type for the @p Component
        /// @tparam Component The component to get the type for.
        template <typename Component>
        [[nodiscard]] constexpr ComponentType getComponentType() const noexcept
        {
            const auto typeName = typeid(Component).name();
            assert(_componentTypes.find(typeName) != _componentTypes.cend() && "[ECS]: Component not registered!");

            return _componentTypes.at(typeName);
        }


        /**
         * @brief Add a component to @p entity.
         * @tparam Component The component's type.
         * @param entity     The entity to add the component to.
         * @param component  The component's instantiation.
         */
        template <typename Component>
        constexpr void add(const Entity entity, Component component) noexcept
        { getComponentArray<Component>()->insert(entity, component); }


        /**
         * @brief Remove a component for the given entity.
         * @tparam Component The component's type.
         * @param entity     The entity whose component needs to be removed.
         */
        template <typename Component>
        constexpr void remove(const Entity entity) noexcept
        { getComponentArray<Component>()->remove(entity); }


        /**
         * @brief Get a component instance for the given entity.
         * @tparam Component The type of component to query for.
         * @param entity The entity whose component is being queried.
         * @return A reference to the entity's component.
         */
        template <typename Component>
        [[nodiscard]] Component& getComponent(const Entity entity) noexcept
        { return getComponentArray<Component>()->get(entity); }


        /**
         * @brief An event for triggering cleanups after the entity was destroyed.
         * @tparam The component type.
         */
        inline void onEntityDestroyed(const Entity entity) const noexcept
        {
            for (const auto& compArr : _componentArrays)
            {
                compArr.second->onEntityDestroyed(entity);
            }
        }



    private:
        // Map for registering component types with their respect type-id
        std::unordered_map<const char*, ComponentType> _componentTypes{};
        // Map from component type-id to their storage
        std::unordered_map<const char*, std::shared_ptr<IComponentArray>> _componentArrays{};
        // Running counter for the next available component type
        ComponentType _nextAvailableType{};

        /// Get a component array of hte querying type from the manager's storage.
        template <typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() const noexcept
        {
            const auto typeName = typeid(T).name();
            assert(_componentTypes.find(typeName) != _componentTypes.cend() && "[ECS]: Component type not registered!");

            return std::static_pointer_cast<ComponentArray<T>>(_componentArrays.at(typeName));
        }
    };

} // namespace tempest