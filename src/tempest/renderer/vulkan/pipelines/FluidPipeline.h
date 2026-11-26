#pragma once
/**
 * @file FluidPipeline.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 25, 2026
 *
 * @brief Vulkan Fluid Pipeline.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include "Application.h"
#include "PipelineConfig.h"
#include "VulkanTypes.inl"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanSwapChain.h"

namespace tempest::renderer
{

    class FluidPipeline
    {
    public:
        void create(const VulkanDevice& device, const VulkanSwapChain& swapChain, const GraphicsPipelineConfig& config,
                    const VmaAllocator& vmaAllocator, vk::AllocationCallbacks* pAllocator) noexcept;

        void destroy(const VulkanDevice& device, const VmaAllocator& vmaAllocator,
                     const vk::AllocationCallbacks* pAllocator) const;

        void bind(vk::CommandBuffer cmdBuffer) const;

        void updateGlobalUBO(const GlobalUniform& ubo, uint32_t frameNumber) const noexcept;

        [[nodiscard]] vk::DescriptorSet descriptorSet(const uint32_t frameIndex) const
        { return _globalDescriptorSets[frameIndex]; }

        [[nodiscard]] vk::Pipeline base() const noexcept { return _pipeline; }
        [[nodiscard]] vk::PipelineLayout layout() const noexcept { return _pipelineLayout; }

    private:
        vk::PipelineLayout _pipelineLayout{};
        vk::Pipeline _pipeline{};

        // ------------------------ GLOBAL UBO ------------------------
        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT_DEF> _globalDescriptorSets{ nullptr, nullptr };
        vk::DescriptorSetLayout _globalDescriptorLayouts{ nullptr };
        vk::DescriptorPool _globalDescriptorPool{ nullptr };
        std::array<VulkanBuffer, MAX_FRAMES_IN_FLIGHT_DEF> _globalUniformBuffers{};
        std::array<void*, MAX_FRAMES_IN_FLIGHT_DEF> _pGlobalUniformBuffers{};


        // ------------------------ OBJECT UBO ------------------------
        std::array<vk::DescriptorSet, MAX_FRAMES_IN_FLIGHT_DEF> _objectDescriptorSets{ nullptr, nullptr };
        vk::DescriptorSetLayout _objectDescriptorLayouts{ nullptr };
        vk::DescriptorPool _objectDescriptorPool{ nullptr };
        std::array<VulkanBuffer, MAX_FRAMES_IN_FLIGHT_DEF> _objectUniformBuffers{};
        std::array<void*, MAX_FRAMES_IN_FLIGHT_DEF> _pObjectUniformBuffers{};
    };
} // namespace tempest::renderer
