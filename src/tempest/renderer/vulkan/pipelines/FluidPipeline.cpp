/**
 * @file FluidPipeline.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: November 25, 2026
 *
 * @brief Implementation of member function declared in FluidPipeline.h
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "FluidPipeline.h"

#include "Application.h"
#include "Asserts.h"
#include "Logger.h"
#include "Utils.h"
#include "VulkanShaderModule.h"

namespace tempest::renderer
{

    void FluidPipeline::create(const VulkanDevice& device, const VulkanSwapChain& swapChain,
                               const GraphicsPipelineConfig& config, const VmaAllocator& vmaAllocator,
                               vk::AllocationCallbacks* pAllocator) noexcept
    {
        // ------------------------ SHADER CREATION ------------------------
        const auto vertShader = createShaderModule(device, config.pVertShaderCode, pAllocator);
        const auto fragShader = createShaderModule(device, config.pFragShaderCode, pAllocator);
        vk::PipelineShaderStageCreateInfo vertStageInfo{
            .stage  = vk::ShaderStageFlagBits::eVertex,
            .module = vertShader,
            .pName  = config.vertShaderEntryPoint.c_str(),
        };
        vk::PipelineShaderStageCreateInfo fragStageInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                                         .module = fragShader,
                                                         .pName  = config.fragShaderEntryPoint.c_str() };

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderInfos{ vertStageInfo, fragStageInfo };


        // ------------------------ DYNAMIC STATES ------------------------
        std::array dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates    = dynamicStates.data(),
        };


        // ------------------------ VERTEX INPUT ------------------------
        const auto bindingDescription = getBindingDescription<core::Vertex>(0, vk::VertexInputRate::eVertex);
        const auto attribDescription  = getAttributeDescription();
        vk::PipelineVertexInputStateCreateInfo vertexInfo{ .vertexBindingDescriptionCount   = 1,
                                                           .pVertexBindingDescriptions      = &bindingDescription,
                                                           .vertexAttributeDescriptionCount = attribDescription.size(),
                                                           .pVertexAttributeDescriptions = attribDescription.data() };


        // ------------------------ INPUT ASSEMBLY ------------------------
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ .topology               = config.topology,
                                                                    .primitiveRestartEnable = vk::False };


        // ------------------------ VIEWPORT STATE ------------------------
        // Configured during draw
        vk::PipelineViewportStateCreateInfo viewPortInfo{ .viewportCount = 1, .scissorCount = 1 };

        // ------------------------ RASTERIZATION ------------------------
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{ .depthClampEnable        = vk::False,
                                                                    .rasterizerDiscardEnable = vk::False,
                                                                    .polygonMode             = config.wireframe
                                                                        ? vk::PolygonMode::eLine
                                                                        : vk::PolygonMode::eFill,
                                                                    .cullMode                = config.cullMode,
                                                                    .depthBiasClamp          = vk::False,
                                                                    .lineWidth               = 1.0f };

        // ------------------------ MSAA ------------------------
        vk::PipelineMultisampleStateCreateInfo multisampleInfo{ .rasterizationSamples = config.msaaSamples,
                                                                .sampleShadingEnable  = config.enableMsaa };

        // ------------------------ DEPTH & STENCIL ------------------------
        // TODO: ENABLE
        // vk::PipelineDepthStencilStateCreateInfo depthStencilInfo {
        //
        // };

        // ------------------------ COLOR BLENDING ------------------------
        vk::PipelineColorBlendAttachmentState colorAttachmentInfo{
            .blendEnable         = vk::True,
            .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
            .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
            .colorBlendOp        = vk::BlendOp::eAdd,
            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
            .alphaBlendOp        = vk::BlendOp::eAdd,
            .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };

        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{
            .logicOpEnable   = vk::False,
            .logicOp         = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments    = &colorAttachmentInfo,
        };

        // ------------------------ GLOBAL DESCRIPTORS ------------------------
        vk::DescriptorSetLayoutBinding uboBindingLayout{ .binding         = 0,
                                                         .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                                         .descriptorCount = 1,
                                                         .stageFlags      = vk::ShaderStageFlagBits::eVertex };
        vk::DescriptorSetLayoutCreateInfo descriptorInfo{ .bindingCount = 1, .pBindings = &uboBindingLayout };
        vk::DescriptorPoolSize globalPoolSize{
            .type            = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = swapChain.getImageCount(),
        };
        vk::DescriptorPoolCreateInfo poolCreateInfo{ .flags   = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                     .maxSets = swapChain.getImageCount(),
                                                     .poolSizeCount = 1,
                                                     .pPoolSizes    = &globalPoolSize };
        vk::Device dev = *device.getLogicalDevice();
        if (auto result = dev.createDescriptorPool(&poolCreateInfo, pAllocator, &_globalDescriptorPool);
            result != vk::Result::eSuccess)
        {
            log::error("There was an error creating the global descriptor pool!");
            return;
        }

        if (auto result = dev.createDescriptorSetLayout(&descriptorInfo, pAllocator, &_globalDescriptorLayouts);
            result != vk::Result::eSuccess)
        {
            log::error("There was an error creating the global descriptor layout!");
            return;
        }


        // ------------------------ OBJECT DESCRIPTORS ------------------------
        // TODO: Add if required


        // ------------------------ PIPELINE LAYOUT ------------------------
        vk::PushConstantRange pushConstantRange{ .stageFlags = vk::ShaderStageFlagBits::eVertex,
                                                 .offset     = 0,
                                                 .size       = sizeof(ModelUniform) };
        std::array descriptorLayouts{ _globalDescriptorLayouts };
        vk::PipelineLayoutCreateInfo pipelineInfo{ .setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size()),
                                                   .pSetLayouts    = descriptorLayouts.data(),
                                                   .pushConstantRangeCount = 1,
                                                   .pPushConstantRanges    = &pushConstantRange };

        if (auto result = dev.createPipelineLayout(&pipelineInfo, pAllocator, &_pipelineLayout);
            result != vk::Result::eSuccess)
        {
            log::error("There was an error creating the pipeline layout!");
            return;
        }


        // ------------------------ PIPELINE ------------------------
        const auto format = swapChain.getFormat().format;
        vk::PipelineRenderingCreateInfo renderingInfo{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &format };

        vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
            .pNext               = &renderingInfo,
            .stageCount          = shaderInfos.size(),
            .pStages             = shaderInfos.data(),
            .pVertexInputState   = &vertexInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState      = &viewPortInfo,
            .pRasterizationState = &rasterizationInfo,
            .pMultisampleState   = &multisampleInfo,
            .pDepthStencilState  = nullptr,
            .pColorBlendState    = &colorBlendInfo,
            .pDynamicState       = &dynamicStateInfo,
            .layout              = _pipelineLayout, // TODO: Update to enable depth testing
        };
        if (auto result = dev.createGraphicsPipeline(nullptr, graphicsPipelineInfo, pAllocator);
            result.result != vk::Result::eSuccess)
        {
            log::error("There was an error creating the graphics pipeline!");
            return;
        }
        else
        {
            _pipeline = result.value;
        }


        // ------------------------ GLOBAL UBO ------------------------
        for (size_t i = 0; i < swapChain.getMaxFramesInFlight(); ++i)
        {
            _globalUniformBuffers[i] = VulkanBuffer();
            const BufferConfig uboConfig{
                .size        = sizeof(GlobalUniform),
                .usageFlags  = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                .property    = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                .sharingMode = vk::SharingMode::eExclusive
            };
            _globalUniformBuffers[i].create(vmaAllocator, uboConfig);
            _pGlobalUniformBuffers[i] = _globalUniformBuffers[i].map();
        }

        // ------------------------ DESCRIPTOR SET ------------------------
        Vector globalDescriptorLayouts(makeVector<vk::DescriptorSetLayout>());
        for (size_t i = 0; i < swapChain.getMaxFramesInFlight(); ++i)
        {
            globalDescriptorLayouts.push_back(_globalDescriptorLayouts);
        }
        vk::DescriptorSetAllocateInfo descriptorAllocInfo{
            .descriptorPool     = _globalDescriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(globalDescriptorLayouts.size()),
            .pSetLayouts        = globalDescriptorLayouts.data(),
        };
        if (auto result = dev.allocateDescriptorSets(&descriptorAllocInfo, _globalDescriptorSets.data());
            result != vk::Result::eSuccess)
        {
            log::error("There was an error creating the graphics pipeline!");
            return;
        }

        // ------------------------ DESCRIPTOR WRITES ------------------------
        for (size_t i = 0; i < swapChain.getMaxFramesInFlight(); ++i)
        {
            vk::DescriptorBufferInfo descriptorBufferInfo{ .buffer = _globalUniformBuffers[i].buffer(),
                                                           .offset = 0,
                                                           .range  = sizeof(GlobalUniform) };
            vk::WriteDescriptorSet descriptorWrite{ .dstSet          = _globalDescriptorSets[i],
                                                    .dstBinding      = 0,
                                                    .dstArrayElement = 0,
                                                    .descriptorCount = 1,
                                                    .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                                    .pBufferInfo     = &descriptorBufferInfo };
            dev.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }

        // ------------------------ DESTROY SHADER MODULES ------------------------
        destroyShaderModule(device, vertShader, pAllocator);
        destroyShaderModule(device, fragShader, pAllocator);

        log::info("Fluid Pipeline created!");
    }


    void FluidPipeline::destroy(const VulkanDevice& device, const VmaAllocator& vmaAllocator,
                                const vk::AllocationCallbacks* pAllocator) const
    {
        const vk::Device dev = *(device.getLogicalDevice());
        for (auto buffer : _globalUniformBuffers)
        {
            buffer.unmap();
            buffer.destroy();
        }

        dev.destroyPipeline(_pipeline);
        dev.destroyPipelineLayout(_pipelineLayout);

        dev.destroyDescriptorSetLayout(_globalDescriptorLayouts, pAllocator);
        dev.destroyDescriptorPool(_globalDescriptorPool, pAllocator);
    }


    void FluidPipeline::bind(const vk::CommandBuffer cmdBuffer) const
    { cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline); }


    void FluidPipeline::updateGlobalUBO(const GlobalUniform& ubo, const uint32_t frameNumber) const noexcept
    {
        TEMPEST_ASSERT(frameNumber < MAX_FRAMES_IN_FLIGHT_DEF,
                       "[GraphicsPipeline/updateObjectData]: Frame Index out of bounds!");
        memcpy(_pGlobalUniformBuffers[frameNumber], &ubo, sizeof(GlobalUniform));
    }
} // namespace tempest::renderer