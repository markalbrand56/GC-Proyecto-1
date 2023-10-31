// camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 cameraPosition;
    glm::vec3 targetPosition;
    glm::vec3 upVector;
};


glm::mat4 createViewMatrix(const Camera& camera) {
    return glm::lookAt(camera.cameraPosition, camera.targetPosition, camera.upVector);
}

Camera setupInitialCamera() {
    Camera camera{};
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 4.5f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    return camera;
}
