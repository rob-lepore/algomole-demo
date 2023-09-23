#include "stdafx.h"
#include "trackball.h"


#include "glm/gtx/quaternion.hpp"


using namespace am::gfx;

Trackball::Trackball()
{
    m_width = 630;
    m_height = 500;
    m_moving = false;
}

void Trackball::setEnabled(bool enable, int x, int y) {
    m_moving = enable;
    m_lastX = x;
    m_lastY = y;
}

glm::mat4 Trackball::move(int x, int y) {
    using namespace glm;
    if (m_moving)
    {
        vec3 current = normalize(getTrackBallPoint(x, y));
        vec3 previous = normalize(getTrackBallPoint(m_lastX, m_lastY));

        m_lastX = x;
        m_lastY = y;

        vec3 rotationVector = cross(previous, current);

        float cosine = dot(previous, current);

        if (cosine > 1)
            cosine = 1.0;
        if (cosine < -1)
            cosine = -1.0;

        //Calcoliamo l'angolo utilizzando l'arcocoseno
        float theta = (acos(cosine)) * m_velocity;

        //Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
        quat rotation = angleAxis(-theta, rotationVector);
        //Trasformiamo il quaternione in matrice di rotazione
        mat4 rotationMatrix = toMat4(rotation);
        return rotationMatrix;
    }

}

glm::vec3 Trackball::getTrackBallPoint(int x, int y)
{
    float delta, tmp;
    glm::vec3 point = {};
    //map to [-1;1]
    point.x = (2.0f * x - m_width) / m_width;
    point.y = (m_height - 2.0f * y) / m_height;

    //Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
    tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
    delta = 1.0f - tmp;
    if (delta > 0.0f)
        point.z = sqrt(delta);
    else
        point.z = 0;

    return point;

}


glm::vec3 Trackball::arcBall(Camera camera, int x, int y) {
    glm::vec4 position_(camera.getPosition().x, camera.getPosition().y, camera.getPosition().z, 1);
    glm::vec4 pivot(camera.getTarget().x, camera.getTarget().y, camera.getTarget().z, 1);//target? maybe direction

    // step 1 : Calculate the amount of rotation given the mouse movement.
    float deltaAngleX = (2 * M_PI / (float)m_width); // a movement from left to right = 2*PI = 360 deg
    float deltaAngleY = (M_PI / (float)m_height);  // a movement from top to bottom = PI = 180 deg
    float xAngle = (m_lastX - x) * deltaAngleX;
    float yAngle = (m_lastY - y) * deltaAngleY;

    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(camera.getPosition()), glm::vec3(camera.getTarget()), glm::vec3(camera.getUpVector()));

    // Extra step to handle the problem when the camera direction is the same as the up vector
    float cosAngle = glm::dot(
        glm::vec3(-glm::transpose(viewMatrix)[2]),
        glm::vec3(camera.getUpVector())
    );//front?
//printf("cosAngle: %f\n\n", cosAngle);
    if (isnan(cosAngle)) {
        deltaAngleY = 0;
    }
    if (cosAngle > 0.99f)
        deltaAngleY = 0;

    // step 2: Rotate the camera around the pivot point on the first axis.
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, glm::vec3(camera.getUpVector()));
    position_ = (rotationMatrixX * (position_ - pivot)) + pivot;

    // step 3: Rotate the camera around the pivot point on the second axis.
    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, glm::vec3(glm::transpose(viewMatrix)[0]));
    glm::vec3 finalPosition = (rotationMatrixY * (position_ - pivot)) + pivot;

    glm::vec3 position = glm::vec3(finalPosition);
    // Update the camera view (we keep the same lookat and the same up vector)
    //(finalPosition, app->m_camera.GetLookAt(), app->m_upVector);

    // Update the mouse position for the next rotation
    m_lastX = x;
    m_lastY = y;

    return position;
}
