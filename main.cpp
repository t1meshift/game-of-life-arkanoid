#include <memory>
#include <raylib-cpp.hpp>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "scenes/ArkanoidScene.h"

void gameLoop(raylib::Window& window);

namespace {
    std::unique_ptr<maslo::BaseScene> scene;
}

int main() {
    raylib::Window window(800, 600, "Game of Life Arkanoid");

    SetTargetFPS(60);

    scene = std::make_unique<maslo::ArkanoidScene>(25, 25);

    if (scene) {
        scene->onAttach();
    }

    while (!window.ShouldClose()) {
       gameLoop(window);
    }

    if (scene) {
        scene->onDetach();
    }

    return 0;
}

void gameLoop(raylib::Window& window) {
    window.BeginDrawing();
    if (scene) {
        window.ClearBackground(raylib::Color::Black());

        scene->draw(window);

        if (auto* keyboardHandler = dynamic_cast<maslo::KeyboardInputHandler*>(scene.get())) {
            auto prevKeys = keyboardHandler->getRawKeys();
            std::erase_if(prevKeys, [](int k) { return !IsKeyDown(k); });
            keyboardHandler->setKeys(prevKeys);
            while (auto key = GetKeyPressed()) {
                keyboardHandler->addKey(key);
            }
        }

        if (auto* touchHandler = dynamic_cast<maslo::TouchInputHandler*>(scene.get())) {
            touchHandler->clearTouches();
            auto touchCount = raylib::Touch::GetPointCount();
            for (int i = 0; i < touchCount; ++i) {
                auto [x, y] = raylib::Touch::GetPosition(i);
                touchHandler->addTouch(x, y);
                TraceLog(LOG_INFO, "Touch @ %.3f %.3f", x, y);
            }
        }

        scene->update(window.GetFrameTime() * 1000.f);
    }
    //window.DrawFPS();
    window.EndDrawing();
}
