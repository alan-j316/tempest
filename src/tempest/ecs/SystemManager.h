#pragma once
/**
 * @file SystemManager.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 24, 2026
 *
 * @brief Manager system registration, signatures and entity signature changed events.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Entity.h"
#include "System.h"

#include <cassert>
#include <memory>
#include <ranges>
#include <unordered_map>

namespace tempest
{
    class SystemManager
    {
    public:
        /**
         * @brief Register a system with the manager.
         * @tparam Sys The type of system to register.
         * @return A smart pointer to the registered system.
         */
        template <typename Sys>
        [[nodiscard]] constexpr std::shared_ptr<Sys> registerSystem() noexcept
        {
            const auto typeName = typeid(Sys).name();
            assert(!_systems.contains(typeName) && "[ECS]: System already registered!");
            auto system = std::make_shared<Sys>();
            _systems.insert({ typeName, system });
            return system;
        }


        /**
         * @brief Set the signature for the given system.
         * @tparam Sys      The system's type.
         * @param signature The signature to set.
         */
        template <typename Sys>
        constexpr void setSignature(Signature signature) noexcept
        {
            const auto typeName = typeid(Sys).name();
            assert(_systems.contains(typeName) && "[ECS]: System not registered yet!");
            _signatures.insert({ typeName, signature });
        }

        /**
         * @brief Trigger an event when the entity is destroyed.
         * @param entity The destroyed entity.
         */
        inline void onEntityDestroyed(const Entity entity) noexcept
        {
            // Remove the entity from all the systems.
            for (const auto& system : _systems | std::views::values)
            {
                system->entities.erase(entity);
            }
        }


        /**
         * @brief Trigger an event when an entity's signature is changed.
         * @param entity    The entity whose signature was changed.
         * @param signature The updated signature.
         */
        void onSignatureChanged(Entity entity, Signature signature) noexcept
        {
            for (const auto& [type, system] : _systems)
            {
                const auto& systemSignature = _signatures[type];
                // If entity signature matches the system insert it into the systems entities
                if ((signature & systemSignature) == systemSignature)
                {
                    system->entities.insert(entity);
                }
                else
                {
                    system->entities.erase(entity);
                }
            }
        }


    private:
        // Map of system and its respective signatures
        std::unordered_map<const char*, Signature> _signatures{};
        std::unordered_map<const char*, std::shared_ptr<System>> _systems{}; // Collection of all registered systems.
    };
} // namespace tempest