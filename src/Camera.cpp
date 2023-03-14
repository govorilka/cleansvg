#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

namespace CleanSVG {

void Camera::setRatio(float ratio)
{
    m_ratio = ratio;
    updateMatrix();
}

void Camera::left()
{
    m_x -= 0.1f;
    updateMatrix();
}

void Camera::right()
{
    m_x += 0.1f;
    updateMatrix();
}

void Camera::up()
{
    m_y += 0.1f;
    updateMatrix();
}
void Camera::down()
{
    m_y -= 0.1f;
    updateMatrix();
}

void Camera::scroll(double xoffset, double yoffset)
{
    if (yoffset > 0) {
        m_scale = std::min(m_scale * 1.1, 10.0);
    } else if (yoffset < 0) {
        m_scale = std::max(m_scale / 1.1, 0.1);
    }
    updateMatrix();
}

const float* Camera::getData() const
{
    return glm::value_ptr(m_matrix);
}

void Camera::updateMatrix()
{
    glm::mat4 m(1.f);
    m = glm::translate(m, glm::vec3(m_x, m_y, 1.0));
    m = glm::scale(m, glm::vec3(m_scale, m_scale, 1.0));

    glm::mat4 p = glm::ortho(-m_ratio, m_ratio, -1.f, 1.f, 1.f, -1.f);
    m_matrix = p * m;
}

void Camera::reset()
{
    m_x = 0.F;
    m_y = 0.F;
    m_scale = 1.0F;
    updateMatrix();
}

} // namespace CleanSVG
