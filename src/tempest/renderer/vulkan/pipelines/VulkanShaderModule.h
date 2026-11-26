#pragma once
/**
 * @file VulkanShaderModule.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 26, 2026
 *
 * @brief Vulkan Shader Module Abstraction
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanDevice.h"
#include "VulkanTypes.inl"

namespace tempest::renderer
{
    /// Create a Vulkan shader module.
    inline vk::ShaderModule createShaderModule(const VulkanDevice& device, const std::vector<char>& shaderCode,
                                               const vk::AllocationCallbacks* pCallbacks) noexcept
    {
        vk::ShaderModuleCreateInfo shaderInfo{};
        shaderInfo.codeSize = static_cast<uint32_t>(shaderCode.size());
        shaderInfo.pCode    = reinterpret_cast<uint32_t const*>(shaderCode.data());

        return (*device.getLogicalDevice()).createShaderModule(shaderInfo, pCallbacks);
    }

    /// Destroy a Vulkan shader module.
    inline void destroyShaderModule(const VulkanDevice& device, const vk::ShaderModule shaderModule,
                                    const vk::AllocationCallbacks* pCallbacks) noexcept
    { (*device.getLogicalDevice()).destroyShaderModule(shaderModule, pCallbacks); }
} // namespace tempest::renderer