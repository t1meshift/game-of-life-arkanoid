#include <random>
#include <fmt/format.h>
#include "ArkanoidScene.h"

namespace {
    constexpr int padWidth = 64;
    constexpr int padHeight = 12;
    constexpr int padY = 600 - padHeight - 32;

    constexpr int ballRadius = 8;

    constexpr float ballVelocity = 160.f; // in px/s
    constexpr float padVelocity = 240.f; // in px/s

    constexpr float cellSpawnProbability = 0.15;
    constexpr float cellUpdateTime = 2100.f; // in ms
    constexpr int cellOffsetY = 32;
    constexpr int cellWidth = 32;
    constexpr int cellHeight = 16;

    const raylib::Color bgColor = raylib::Color::Black();
    const raylib::Color hudColor = raylib::Color::White();

    const raylib::Color cellColor = raylib::Color::White();
    const raylib::Color cellBorderColor = raylib::Color::DarkGray();
    const raylib::Color padColor = raylib::Color::White();
    const raylib::Color ballColor = raylib::Color::White();
}

namespace maslo {
    ArkanoidScene::ArkanoidScene(size_t fieldWidth, size_t fieldHeight)
        : m_automata(fieldWidth, fieldHeight, CellAutomataRules::make34Life()),
        m_fieldWidth(fieldWidth), m_fieldHeight(fieldHeight) {}

    void ArkanoidScene::update(float dt) {
        // Handle input
        const auto& keys = KeyboardInputHandler::getRawKeys();
        for (auto key : keys) {
            if (key == KeyboardKey::KEY_SPACE && !m_gameStarted) {
                m_gameStarted = true;
                ballVX = ballVelocity;
                ballVY = -ballVelocity;
            }
            else if (key == KeyboardKey::KEY_LEFT) {
                padX -= padVelocity / 1000.f * dt;
            }
            else if (key == KeyboardKey::KEY_RIGHT) {
                padX += padVelocity / 1000.f * dt;
            }
        }

        if (!keys.empty()) {
            const auto& touches = TouchInputHandler::getRawTouches();
            if (!touches.empty() && !m_gameStarted) {
                m_gameStarted = true;
                return;
            }
            float padTouchDirection = 0.f;
            for (const auto& [touchX, _] : touches) {
                padTouchDirection += (touchX - 400) / 400;
            }
            padTouchDirection /= static_cast<float>(touches.size());
            padX += padVelocity * padTouchDirection / 1000.f * dt;
        }

        // Ball follows pad if the game has not been started
        if (!m_gameStarted) {
            ballX = padX + padWidth / 2.f;
            return;
        }

        handleCollisions();

        ballX += ballVX / 1000.f * dt;
        ballY += ballVY / 1000.f * dt;

        // Update cell automata
        m_dtSinceCellUpdate += dt;
        auto cellTicksElapsed = static_cast<int>(m_dtSinceCellUpdate / cellUpdateTime);
        if (cellTicksElapsed > 0) {
            while (cellTicksElapsed--) {
                m_automata.update();
            }
            m_dtSinceCellUpdate = 0;
        }
    }

    void ArkanoidScene::onAttach() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 1000000);

        for (int i = 0; i < m_fieldHeight; ++i) {
            for (int j = 0; j < m_fieldWidth; ++j) {
                if (distrib(gen) > static_cast<int>(distrib.max() * (1 - cellSpawnProbability))) {
                    m_automata.setCell(j, i, 1);
                }
            }
        }

        resetGame();
    }

    void ArkanoidScene::onDetach() {

    }

    std::pair<int, int> ArkanoidScene::cellXYToWorldXY(int x, int y) {
        return {x * cellWidth, cellOffsetY + (y * cellHeight)};
    }

    std::optional<std::pair<int, int>> ArkanoidScene::WorldXYToCellXY(int x, int y) {
        std::pair<int, int> result;
        if (x < 0 || x >= 800) {
            return std::nullopt;
        }
        if (y < cellOffsetY || y >= cellOffsetY + (m_fieldHeight * cellHeight)) {
            return std::nullopt;
        }

        auto suggestedX = x / cellWidth;
        auto suggestedY = (y - cellOffsetY) / cellHeight;
        return std::pair{suggestedX, suggestedY};
    }

    void ArkanoidScene::draw(raylib::Window& window) {
        // Draw cells
        for (auto y = 0; y < m_fieldHeight; ++y) {
            for (auto x = 0; x < m_fieldWidth; ++x) {
                auto cell = m_automata.getCell(x, y);
                auto [cellWorldX, cellWorldY] = cellXYToWorldXY(x, y);

                if (cell) {
                    cellColor.DrawRectangle(cellWorldX, cellWorldY, cellWidth, cellHeight);
                    cellBorderColor.DrawRectangleLines(cellWorldX, cellWorldY, cellWidth, cellHeight);
                }
            }
        }

        // Draw ball
        DrawCircleV({ballX, ballY},
                    ballRadius, static_cast<::Color>(ballColor));

        // Draw pad
        padColor.DrawRectangle(static_cast<int>(padX), padY, padWidth, padHeight);

        // Draw HUD
        hudColor.DrawText(fmt::format("Gen: {}", m_automata.getGeneration()), 0, 0, 24.f);
    }

    void ArkanoidScene::handleCollisions() {
        bool beep = false;
        if (padX < 0) {
            padX = 0;
        }

        if (padX + padWidth > 800) {
            padX = 800 - padWidth;
        }

        auto ballTop = ballY - ballRadius / 2.f;
        auto ballBottom = ballY + ballRadius / 2.f;
        auto ballLeft = ballX - ballRadius / 2.f;
        auto ballRight = ballX + ballRadius / 2.f;

        if (ballBottom >= 600) {
            resetGame();
            return;
        }

        if (ballBottom > padY && ballLeft >= padX && ballRight <= padX + padWidth) {
            ballVY *= -1;
            beep = true;
        }

        if (ballLeft <= 0 || ballRight >= 800) {
            ballVX *= -1;
            beep = true;
        }

        if (ballTop <= 0) {
            ballVY *= -1;
            beep = true;
        }

        if (auto cell = WorldXYToCellXY(static_cast<int>(ballX), static_cast<int>(ballY))) {
            auto [cellX, cellY] = cell.value();
            if (m_automata.getCell(cellX, cellY)) {
                m_automata.setCell(cellX, cellY, 0);
                // TODO: we actually must check horizontal/vertical/both collision type
                ballVX *= -1;
                ballVY *= -1;
                beep = true;
            }
        }

        if (beep) {
            // TODO audio?
        }
    }

    void ArkanoidScene::resetGame() {
        m_gameStarted = false;
        padX = (800 - padWidth) / 2.f;
        ballX = padX + padWidth / 2.f;
        ballY = padY - 1 - ballRadius / 2.f;
        ballVX = 0.f;
        ballVY = 0.f;
    }
}