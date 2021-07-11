#pragma once

#include <risc.hpp>

#include <vector>

struct GLFWwindow;

namespace vc::ui {

    class View;

    class Window {
    public:
        Window();
        ~Window();

        void loop();

        template<typename T, typename ... Args>
        auto& addView(Args&&... args) {
            auto view = new T(std::forward<Args>(args)...);
            this->views.push_back(view);

            return *view;
        }

    private:
        void frameBegin();
        void frame();
        void frameEnd();

        void initGLFW();
        void initImGui();
        void deinitGLFW();
        void deinitImGui();

        GLFWwindow *windowHandle = nullptr;

        double targetFps = 60.0;
        double lastFrameTime = 0.0;

        std::vector<View*> views;
    };

}