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
    // print target position
//     std::cout << "target position: " << camera.targetPosition.x << ", " << camera.targetPosition.y << ", " << camera.targetPosition.z << std::endl;

    // print camera position
//     std::cout << "camera position: " << camera.cameraPosition.x << ", " << camera.cameraPosition.y << ", " << camera.cameraPosition.z << std::endl;
    return glm::lookAt(camera.cameraPosition, camera.targetPosition, camera.upVector);
}

Camera setupInitialCamera() {
    Camera camera{};
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 4.5f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    return camera;
}

/*
 * CAMERA MOVEMENTS
 *
 * Si la camara se mueve para arriba, la camara debe mantenerse a la misma distancia del target, por lo que deben compensarse (con trigonometria) los movimientos en x y z.
 *
 * */

float cameraMoveAngle = 5.0f;

Camera moveUp(Camera camera) {
    // Si se llama esta funcion, la cámara va a rotar hacia arriba
    // esto significa que la camara se va a mover en el eje y y en el eje z, manteniendo la misma distancia hacia el target

    // calcular el ángulo de rotacion
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(camera.cameraPosition.y, camera.cameraPosition.z);

    angle = currentAngle - angle;

    // calcular el desplazamiento en y y en z
    float y = glm::sin(angle) * glm::length(camera.cameraPosition);
    float z = glm::cos(angle) * glm::length(camera.cameraPosition);

    // actualizar la posicion de la camara
    camera.cameraPosition.y = y;
    camera.cameraPosition.z = z;

    return camera;
}

glm::vec3 moveShipUp (glm::vec3& shipTranslationVector) {
    // follow the camera
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(shipTranslationVector.y, shipTranslationVector.z);

    angle = currentAngle - angle;

    // calcular el desplazamiento en y y en z
    float y = glm::sin(angle) * glm::length(shipTranslationVector);
    float z = glm::cos(angle) * glm::length(shipTranslationVector);

    // actualizar la posicion de la camara
    shipTranslationVector.y = y;
    shipTranslationVector.z = z;

    return shipTranslationVector;
}

Camera moveDown(Camera camera) {
    // Si se llama esta funcion, la cámara va a rotar hacia arriba
    // esto significa que la camara se va a mover en el eje y y en el eje z, manteniendo la misma distancia hacia el target

    // calcular el ángulo de rotacion
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(camera.cameraPosition.y, camera.cameraPosition.z);

    angle += currentAngle;

    // calcular el desplazamiento en y y en z
    float y = glm::sin(angle) * glm::length(camera.cameraPosition);
    float z = glm::cos(angle) * glm::length(camera.cameraPosition);

    // actualizar la posicion de la camara
    camera.cameraPosition.y = y;
    camera.cameraPosition.z = z;

    return camera;
}

glm::vec3 moveShipDown (glm::vec3& shipTranslationVector) {
    // follow the camera
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(shipTranslationVector.y, shipTranslationVector.z);

    angle += currentAngle;

    // calcular el desplazamiento en y y en z
    float y = glm::sin(angle) * glm::length(shipTranslationVector);
    float z = glm::cos(angle) * glm::length(shipTranslationVector);

    // actualizar la posicion de la camara
    shipTranslationVector.y = y;
    shipTranslationVector.z = z;

    return shipTranslationVector;
}

Camera moveLeft(Camera camera) {
    // Si se llama esta funcion, la cámara va a rotar hacia arriba
    // esto significa que la camara se va a mover en el eje x y en el eje z, manteniendo la misma distancia hacia el target

    // calcular el ángulo de rotacion
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre x y z
    float currentAngle = glm::atan(camera.cameraPosition.x, camera.cameraPosition.z);

    angle = currentAngle - angle;

    // calcular el desplazamiento en x y en z
    float x = glm::sin(angle) * glm::length(camera.cameraPosition);
    float z = glm::cos(angle) * glm::length(camera.cameraPosition);

    // actualizar la posicion de la camara
    camera.cameraPosition.x = x;
    camera.cameraPosition.z = z;

    return camera;
}

glm::vec3 moveShipLeft (glm::vec3& shipTranslationVector) {
    // follow the camera
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(shipTranslationVector.x, shipTranslationVector.z);

    angle = currentAngle - angle;

    // calcular el desplazamiento en y y en z
    float x = glm::sin(angle) * glm::length(shipTranslationVector);
    float z = glm::cos(angle) * glm::length(shipTranslationVector);

    // actualizar la posicion de la camara
    shipTranslationVector.x = x;
    shipTranslationVector.z = z;

    return shipTranslationVector;
}

Camera moveRight(Camera camera) {
    // Si se llama esta funcion, la cámara va a rotar hacia arriba
    // esto significa que la camara se va a mover en el eje x y en el eje z, manteniendo la misma distancia hacia el target

    // calcular el ángulo de rotacion
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre x y z
    float currentAngle = glm::atan(camera.cameraPosition.x, camera.cameraPosition.z);

    angle += currentAngle;

    // calcular el desplazamiento en x y en z
    float x = glm::sin(angle) * glm::length(camera.cameraPosition);
    float z = glm::cos(angle) * glm::length(camera.cameraPosition);

    // actualizar la posicion de la camara
    camera.cameraPosition.x = x;
    camera.cameraPosition.z = z;

    return camera;
}

glm::vec3 moveShipRight (glm::vec3& shipTranslationVector) {
    // follow the camera
    float angle = glm::radians(cameraMoveAngle);

    // obtener el ángulo actual entre y y z
    float currentAngle = glm::atan(shipTranslationVector.x, shipTranslationVector.z);

    angle += currentAngle;

    // calcular el desplazamiento en y y en z
    float x = glm::sin(angle) * glm::length(shipTranslationVector);
    float z = glm::cos(angle) * glm::length(shipTranslationVector);

    // actualizar la posicion de la camara
    shipTranslationVector.x = x;
    shipTranslationVector.z = z;

    return shipTranslationVector;
}

Camera zoomIn(Camera camera) {
    // prevent zooming in too much
    // si la tangente del ángulo entre y y z es mayor a 1, significa que la camara se está acercando demasiado al target
    float distance = glm::length(camera.cameraPosition);
    std::cout << "camera position z: " << distance << std::endl;
    if (distance <= 3.6f)
        return camera;

    // print z
    camera.cameraPosition.z -= 0.1f;
    return camera;
}

glm::vec3 zoomInShip (glm::vec3& shipTranslationVector) {
    // prevent zooming in too much
    // si la tangente del ángulo entre y y z es mayor a 1, significa que la camara se está acercando demasiado al target
    float distance = glm::length(shipTranslationVector);
    std::cout << "ship position z: " << distance << std::endl;
    if (distance <= 3.6f)
        return shipTranslationVector;

    // print z
    shipTranslationVector.z -= 0.1f;
    return shipTranslationVector;
}

Camera zoomOut(Camera camera) {
    // prevent zooming out too much
    if (camera.cameraPosition.z >= 15.0f)
        return camera;
    camera.cameraPosition.z += 0.1f;
    return camera;
}

glm::vec3 zoomOutShip (glm::vec3& shipTranslationVector) {
    // prevent zooming out too much
    if (shipTranslationVector.z >= 15.0f)
        return shipTranslationVector;
    shipTranslationVector.z += 0.1f;
    return shipTranslationVector;
}
