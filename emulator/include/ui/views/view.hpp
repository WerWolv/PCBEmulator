#pragma once

#include <risc.hpp>

#include <imgui.h>
#include <imgui_vc_extensions.h>
#include <fontawesome_font.h>

#include <string>
#include <string_view>

namespace vc::ui {

    class View {
    public:
        explicit View(std::string_view name) : viewName(name) { }
        virtual ~View() = default;

        virtual void drawContent() = 0;

        virtual void draw() final {
            if (ImGui::Begin(this->viewName.c_str())) {
                this->drawContent();
                ImGui::End();
            }
        }

    private:
        std::string viewName;
    };

}