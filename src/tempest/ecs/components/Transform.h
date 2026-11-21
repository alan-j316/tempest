#pragma once
/**
 * @file Transform.h
 * @author Alan Abraham P Kochumon
 * @date Created on: November 21, 2026
 *
 * @brief Transform Component
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <falcon/Falcon.h>

namespace tempest
{

    struct TransformComponent
    {
        flcn::Vec3F position;
        flcn::Quaternion<float> rotation;
        flcn::Vec3F scale;
    };
}