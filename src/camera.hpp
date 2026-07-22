#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const float SPEED = 2.5f;
static const float SENSITIVITY = 0.1f;

enum Direction {
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

class Camera {
public:
    glm::vec3 Position, Front, Up, Right, WorldUp;
    float Yaw, Pitch, Fov, CameraSpeed, MouseSensitivity;

    Camera(float initYaw, float initPitch, float initFov) {
        // Set passed values
        Yaw = initYaw;
        Pitch = initPitch;
        Fov = initFov;
        CameraSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;

        // Initial values for camera vectors
        Position = glm::vec3(0.0f, 0.0f, 3.0f);
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        // Update our vectors
        _UpdateVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void MouseUpdate(float xoffset, float yoffset) {
        // Apply a sensitivity modifier
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // Adjust pitch and yaw
        Yaw += xoffset;
        Pitch += yoffset;

        // Constrain pitch to +/- 89 degrees
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // Update vectors
        _UpdateVectors();
    }

    void ScrollUpdate(float offset) {
        Fov -= (float)offset;
        if (Fov < 1.0f)
            Fov = 1.0f;
        if (Fov > 45.0f)
            Fov = 45.0f;

        // Update vectors
        _UpdateVectors();
    }

    void MovementUpdate(Direction direction, float deltaTime) {
        const float velocity = CameraSpeed * deltaTime;
        switch (direction) {
            case FRONT:
                Position += velocity * Front;
                break;
            case BACK:
                Position -= velocity * Front;
                break;
            case LEFT:
                Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
                break;
            case RIGHT:
                Position += glm::normalize(glm::cross(Front, Up)) * velocity;
                break;
            default:
                break;
        };

        // Update vectors
        _UpdateVectors();
    }

private:
    void _UpdateVectors() {
        // Get Front
        glm::vec3 direction;
        direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        direction.y = sin(glm::radians(Pitch));
        direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(direction);
        // Get Up and Right
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

#endif