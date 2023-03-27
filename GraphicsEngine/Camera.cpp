#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp, float sensitivity) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraPosition + glm::normalize(cameraTarget - cameraPosition);
        this->cameraUpDirection = cameraUp;
        this->sensitivity = sensitivity;
        this->direction = cameraTarget - cameraPosition;
    }

    Camera::Camera(){
        Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.5f);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    glm::mat4 Camera::getViewMatrixFromOrigin() {
        return glm::lookAt(glm::vec3(0.0f), direction, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 newCameraPosition;
        if (direction == MOVE_FORWARD) {
            cameraFrontDirection = cameraTarget - cameraPosition;
            newCameraPosition = cameraPosition + cameraFrontDirection * speed;
        }
        else if (direction == MOVE_BACKWARD) {
            cameraFrontDirection = cameraTarget - cameraPosition;
            newCameraPosition = cameraPosition - cameraFrontDirection * speed;
        }
        else if (direction == MOVE_RIGHT) {
            cameraFrontDirection = cameraTarget - cameraPosition;
            cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
            newCameraPosition = cameraPosition + cameraRightDirection * speed;
        }
        else if (direction == MOVE_LEFT) {
            cameraFrontDirection = cameraTarget - cameraPosition;
            cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
            newCameraPosition = cameraPosition - cameraRightDirection * speed;
        }
        else if (direction == MOVE_UP) {
            newCameraPosition = cameraPosition + cameraUpDirection * speed;
        }
        else if (direction == MOVE_DOWN) {
            newCameraPosition = cameraPosition - cameraUpDirection * speed;
        }

        cameraTarget += newCameraPosition - cameraPosition;
        cameraPosition = newCameraPosition;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float xpos, float ypos) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraTarget = cameraPosition + glm::normalize(direction);
    }
}