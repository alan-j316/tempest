#pragma once
/**
 * @file Coordinator.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 24, 2026
 *
 * @brief Coordinates ECS lifecycle.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"

#include <memory>

namespace tempest
{
    class Coordinator
    {
    public:
        Coordinator()
            : _entityManager{ std::make_unique<EntityManager>() },
              _componentManager{ std::make_unique<ComponentManager>() },
              _systemManager{ std::make_unique<SystemManager>() }
        {}


        /**
         * @brief Create a new entity.
         * @return A new entity.
         */
        [[nodiscard]] Entity createEntity() noexcept { return _entityManager->create(); }


        /**
         * @brief Destroy the entity.
         * @param entity The entity to destroy.
         */
        void destroyEntity(const Entity entity) noexcept
        {
            _entityManager->destroy(entity);
            _systemManager->onEntityDestroyed(entity);
            _componentManager->onEntityDestroyed(entity);
        }


        template <typename Comp>
        void registerComponent() noexcept
        { _componentManager->registerComponent<Comp>(); }


        /**
         * @brief Add a component to @p entity.
         * @tparam Comp The type of component.
         * @param entity    The entity to add the component to.
         * @param component The component to add.
         */
        template <typename Comp>
        void addComponent(const Entity entity, Comp component) noexcept
        {
            // Add the component to the component manager
            _componentManager->add(entity, component);
            // Update teh entity's signature
            auto signature = _entityManager->getSignature(entity);
            signature.set(_componentManager->getComponentType<Comp>(), true);
            _entityManager->setSignature(entity, signature);

            // Notify the system that an entity signature has changed
            _systemManager->onSignatureChanged(entity, signature);
        }


        /**
         * @brief Remove a component from an entity.
         * @tparam Comp  The type of component to remove.
         * @param entity The entity from which the component needs to be removed.
         */
        template <typename Comp>
        void removeComponent(const Entity entity) noexcept
        {
            // Remove the component
            _componentManager->remove<Comp>(entity);

            // Remove the signature
            const auto signature = _entityManager->getSignature(entity);
            signature.set(_componentManager->getComponentType<Comp>(), false);
            _entityManager->setSignature(entity, signature);

            // Notify the system of the change
            _systemManager->onSignatureChanged(entity, signature);
        }


        /**
         * @brief Get the component for the given entity.
         * @tparam Comp  The component type.
         * @param entity The entity to whose component we need to get.
         * @return A reference to the component.
         */
        template <typename Comp>
        [[nodiscard]] Comp& getComponent(const Entity entity)
        { return _componentManager->getComponent<Comp>(entity); }


        /**
         * @brief Get the component type for the component.
         * @tparam Comp The component type.
         * @return A unique identifier for the component.
         */
        template <typename Comp>
        [[nodiscard]] ComponentType getComponentType() noexcept
        { return _componentManager->getComponentType<Comp>(); }


        /**
         * @brief Register a system with the ECS. Must only be done once.
         * @tparam Sys The system type.
         * @return A smart pointer to the registered system instance.
         */
        template <typename Sys>
        [[nodiscard]] std::shared_ptr<Sys> registerSystem()
        { return _systemManager->registerSystem<Sys>(); }



        /**
         * @brief Register a system with a signature.
         * @tparam Sys The system type.
         * @param signature The signature to set.
         */
        template <typename Sys>
        void setSystemSignature(const Signature signature) noexcept
        { _systemManager->setSignature<Sys>(signature); }


    private:
        std::unique_ptr<EntityManager> _entityManager;
        std::unique_ptr<ComponentManager> _componentManager;
        std::unique_ptr<SystemManager> _systemManager;
    };

} // namespace tempest