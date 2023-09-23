#pragma once
#include <glm/glm.hpp>
#include <algomole/am.h>
#include "camera.h"


class Trackball
{
public:
    Trackball();

    void setEnabled(bool enable, int x, int y);
    glm::mat4 move(int x, int y);
    glm::vec3 arcBall(Camera camera, int x, int y);

private:
    bool m_moving;
    int m_lastX;
    int m_lastY;
    float m_velocity = 40;
    int m_width;
    int m_height;

    glm::vec3 getTrackBallPoint(int x, int y);
};
