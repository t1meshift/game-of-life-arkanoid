#pragma once

#include <vector>

namespace maslo {
    class TouchInputHandler {
    public:
        TouchInputHandler() = default;

        void addTouch(float x, float y) {
            m_points.emplace_back(x, y);
        }

        void setTouches(const std::vector<std::pair<float, float>>& points) {
            m_points = points;
        }

        void clearTouches() {
            m_points.clear();
        }

        [[nodiscard]] const std::vector<std::pair<float, float>>& getRawTouches() const {
            return m_points;
        }

    protected:
        std::vector<std::pair<float, float>> m_points;
    };
}
