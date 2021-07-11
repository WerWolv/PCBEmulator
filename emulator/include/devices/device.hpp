#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_vc_extensions.h>

namespace vc::dev {

    class Device {
    public:
        Device() = default;
        virtual ~Device() = default;

        virtual void tick() = 0;
        virtual bool needsUpdate() = 0;
        virtual void reset() = 0;
    };

}