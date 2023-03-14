#pragma once

#include <glm/mat4x4.hpp>

namespace CleanSVG {

class Camera final
{
public:
    void setRatio(float ratio);

    void left();
    void right();
    void up();
    void down();

    void scroll(double xoffset, double yoffset);

    const float* getData() const;

    void reset();

private:
    void updateMatrix();

    float m_ratio = 0.f;

    float m_x = 0.F;
    float m_y = 0.F;
    float m_scale = 1.0F;

    glm::mat4 m_matrix;
};

} // namespace CleanSVG
