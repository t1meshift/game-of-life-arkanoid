#pragma once

#include <optional>
#include "BaseScene.h"
#include "../input/KeyboardInputHandler.h"
#include "../tools/CellAutomata.h"

namespace maslo {
    class ArkanoidScene : public BaseScene, public KeyboardInputHandler {
    public:
        explicit ArkanoidScene(size_t fieldWidth, size_t fieldHeight);
        void update(float dt) override;
        void onAttach() override;
        void onDetach() override;
        void draw(raylib::Window& window) override;
    private:
        static std::pair<int, int> cellXYToWorldXY(int x, int y);
        std::optional<std::pair<int, int>> WorldXYToCellXY(int x, int y);
        void handleCollisions();
        void resetGame();
    private:
        CellAutomata m_automata;
        size_t m_fieldWidth, m_fieldHeight;
        float padX = 0;
        float ballX = 0, ballY = 0;
        float ballVX = 0, ballVY = 0;
        float m_dtSinceCellUpdate = 0.f;
        bool m_gameStarted = false;
    };
}