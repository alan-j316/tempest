#pragma once
/**
 * @file VulkanGraphicsPipeline.h
 * @author Alan Abraham P Kochumon
 * @date Created on: October 29, 2026
 *
 * @brief Vulkan Graphics Pipeline.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "Application.h"
#include "Id.h"
#include "VulkanSwapChain.h"
#include "containers/Vector.h"
#include "renderer/RendererTypes.inl"
#include "renderer/vulkan/VulkanBuffer.h"
#include "renderer/vulkan/VulkanDevice.h"
#include "renderer/vulkan/VulkanTypes.inl"
#include "renderer/vulkan/pipelines/PipelineConfig.h"

#include <cstdint>


namespace tempest::renderer
{
    struct VulkanShaderObjectState
    {
        vk::DescriptorSet descriptorSet[MAX_FRAMES_IN_FLIGHT_DEF];
        VulkanDescriptorState descriptorStates[VULKAN_MAX_OBJECT_DESCRIPTOR_COUNT];
    };



    // TODO: Update to PMR vector

    using ShaderIndex = uint8_t;

    class VulkanGraphicsPipeline
    {
    public:
        /**
         * Create a graphics pipeline. Must only be called once per instance.
         * @param device The vulkan device.
         * @param swapChain
         * @param config The graphics pipeline configuration.
         * @param vmaAllocator
         * @param allocator
         * @return `true` if pipeline was create successfully and `false` otherwise.
         */
        bool create(const VulkanDevice& device, const VulkanSwapChain& swapChain, const GraphicsPipelineConfig& config,
                    const VmaAllocator& vmaAllocator, vk::AllocationCallbacks* allocator);

        /**
         * Create a shader. The shader is not bound to the pipeline and must be manually bound using @ref bindShader.
         * @param device The vulkan device.
         * @param pShaderCode The shader code binary.
         * @return An internal handle to the shader.
         */
        ShaderIndex createShader(const VulkanDevice& device, const std::vector<char>& pShaderCode);

        /**
         * Updates the global uniform buffer object for the frame.
         * @param index The index of the current rendering frame.
         * @param ubo        The uniform object to update with.
         * @param deltaTime
         */
        void updateGlobalUBO(size_t index, const GlobalUniform& ubo, float deltaTime) const noexcept;

        /**
         * Update the model uniform buffer.
         * @param cmdBuffer
         * @param device
         * @param renderData
         * @param imageIndex
         * @param deltaTime
         */
        void updateObjectData(vk::CommandBuffer cmdBuffer, vk::Device device, const GeometryRenderData& renderData,
                              uint32_t imageIndex, float deltaTime) noexcept;


        /**
         * Destroy the render pipeline.
         */
        void destroy(const VulkanDevice& device, vk::AllocationCallbacks* allocator);

        /// Get the Vulkan graphics pipeline.
        [[nodiscard]] const vk::raii::Pipeline& getBasePipeline() const { return _pipeline; }

        /// Get the Vulkan graphics pipeline.
        [[nodiscard]] vk::raii::Pipeline& getBasePipeline() { return _pipeline; }

        /// Get the global descriptor set for the current @p frameIndex.
        [[nodiscard]] vk::DescriptorSet descriptorSet(const uint32_t frameIndex) const noexcept
        { return _globalDescriptorSets[frameIndex]; }

        /// Get the Vulkan pipeline layout.
        [[nodiscard]] vk::PipelineLayout descriptorLayout() const noexcept { return *_pipelineLayout; }

        UID acquireResource(vk::Device device) noexcept;

        void releaseResource(UID id, vk::Device device) noexcept;


    private:
        vk::raii::PipelineLayout _pipelineLayout{ nullptr };
        vk::raii::Pipeline _pipeline{ nullptr };
        Vector<vk::raii::ShaderModule> _shaders{};


        // ------------------------ GLOBAL UBO ------------------------
        vk::DescriptorSet _globalDescriptorSets[MAX_FRAMES_IN_FLIGHT_DEF]{ nullptr, nullptr };
        vk::raii::DescriptorSetLayout _globalDescriptorSetLayout{ nullptr };
        vk::raii::DescriptorPool _globalDescriptorPool{ nullptr };
        VulkanBuffer _globalUniformBuffers[MAX_FRAMES_IN_FLIGHT_DEF]{};
        void* _pGlobalUniformBuffers[MAX_FRAMES_IN_FLIGHT_DEF]{ nullptr, nullptr }; // Used for persistent mapping

        // ------------------------ OBJECT UBO ------------------------
        VulkanShaderObjectState _objectStates[VULKAN_MAX_OBJECT_COUNT]{}; // TODO: Make dynamic
        vk::raii::DescriptorSetLayout _objectDescriptorSetLayout{ nullptr };
        vk::raii::DescriptorPool _objectDescriptorPool{ nullptr };
        VulkanBuffer _objectUniformBuffers{};
        UID _objectBufferUniformIndex{};
        void* _pObjectUnformBuffer{ nullptr };
    };
} // namespace tempest::renderer