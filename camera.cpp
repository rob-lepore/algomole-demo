#include "stdafx.h"
#include "camera.h"

Camera::Camera() {
    this->position = { 0,0,0,0 };
    this->target = { 0,0,0,0 };
    this->upVector = { 0,1,0,0 };
    direction = target - position;
    speed = 0.1;
}

Camera::Camera(glm::vec4 pos, glm::vec4 target, glm::vec4 up)
{
    this->position = pos;
    this->target = target;
    this->upVector = up;
    direction = target - position;
    speed = 0.1;
}


void Camera::moveForward()
{
    position += direction * speed;
    direction = target - position;
}

void Camera::moveBack()
{
    position -= direction * speed;
    direction = target - position;
}

void Camera::moveLeft()
{
    glm::vec3 slide_vector = glm::cross(glm::vec3(direction), glm::vec3(upVector)) * speed;
    position -= glm::vec4(slide_vector, 1);
    target -= glm::vec4(slide_vector,1);
    direction = target - position;

}

void Camera::moveRight()
{
    glm::vec3 slide_vector = glm::cross(glm::vec3(direction), glm::vec3(upVector)) * speed;
    position += glm::vec4(slide_vector, 1);
    target += glm::vec4(slide_vector, 1);
    direction = target - position;
}

void Camera::moveUp()
{
    glm::vec3 slide_vector = glm::normalize(glm::cross(glm::vec3(direction), glm::vec3(upVector)));
    glm::vec3 upDirection = glm::cross(glm::vec3(direction), slide_vector) * speed;
    position -= glm::vec4(upDirection, 0.0);
    target -= glm::vec4(upDirection, 0.0);
    direction = target - position;
}

void Camera::moveDown()
{
    glm::vec3 slide_vector = glm::normalize(glm::cross(glm::vec3(direction), glm::vec3(upVector)));
    glm::vec3 upDirection = glm::cross(glm::vec3(direction), slide_vector) * speed;
    position += glm::vec4(upDirection, 0.0);
    target += glm::vec4(upDirection, 0.0);
    direction = target - position;
}

void Camera::setSpeed(float s)
{
    speed = s;
}

void Camera::setPosition(glm::vec4 p)
{
    position = p;
    direction = target - position;
}

void Camera::setTarget(glm::vec4 t)
{
    target = t;
    direction = target - position;
}

glm::vec4 Camera::getPosition() {
    return position;
}

glm::vec4 Camera::getTarget() {
    return target;
}

glm::vec4 Camera::getUpVector() {
    return upVector;
}

glm::vec4 Camera::getDirection() {
    return direction;
}

void Camera::updateDirection()
{
    direction = target - position;
}
