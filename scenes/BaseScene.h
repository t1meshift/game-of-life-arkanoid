#pragma once

#include "raylib-cpp.hpp"

namespace maslo {
    class BaseScene {
    public:
        BaseScene() = default;
        virtual ~BaseScene() = default;
        BaseScene(BaseScene&&) = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void update(float dt) = 0;
        virtual void draw(raylib::Window& window) = 0;
    };
}
