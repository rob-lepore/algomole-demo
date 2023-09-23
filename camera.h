#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    Camera(glm::vec4, glm::vec4, glm::vec4);

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveForward();
    void moveBack();

    glm::vec4 getPosition();
    glm::vec4 getTarget();
    glm::vec4 getUpVector();
    glm::vec4 getDirection();

    void setSpeed(float);
    void setPosition(glm::vec4);
    void setTarget(glm::vec4);
    void updateDirection();

private:
    glm::vec4 position;
    glm::vec4 target;
    glm::vec4 upVector;
    glm::vec4 direction;

    float speed;
};




