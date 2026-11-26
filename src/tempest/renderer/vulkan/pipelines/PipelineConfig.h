#pragma once
/**
 * @file PipelineConfig.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 25, 2026
 *
 * @brief Pipeline Configuration
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

#include "VulkanTypes.inl"

namespace tempest::renderer
{

    struct GraphicsPipelineConfig
    {
        std::string vertShaderEntryPoint;
        std::vector<char> pVertShaderCode; // TODO: Change to PMR
        std::string fragShaderEntryPoint;
        std::vector<char> pFragShaderCode; // TODO: Change to PMR

        bool wireframe{ false };            /// Indicate whether pipeline draws only wireframes
        vk::Bool32 enableMsaa{ vk::False }; ///< Enable/Disable MSAA
        vk::SampleCountFlagBits msaaSamples{
            vk::SampleCountFlagBits::e1
        }; ///< MSAA sample count. Ignored if msaa is disabled
        vk::FrontFace winding{ vk::FrontFace::eClockwise };                     ///< Triangle winding order
        vk::CullModeFlags cullMode{ vk::CullModeFlagBits::eNone };              ///< Face culling
        vk::PrimitiveTopology topology{ vk::PrimitiveTopology::eTriangleList }; ///< Primitive topology
    };

} // namespace tempest::renderer