#include <SDL.h>
#include "gl.h"
#include "camera.h"
#include "uniforms.h"
#include "shaders.h"
#include "object.h"
#include "triangle.h"
#include <iostream>
#include <vector>

Camera camera = setupInitialCamera();
std::vector<Model> models;
std::string planet;
bool hasMoon = false;

using namespace std;
void render() {
    for (auto model : models) {
        Uniforms uniform = model.uniforms;
        uniform.model = model.modelMatrix;

        // 1. Vertex Shader
        // vertex -> transformedVertices
        std::vector<Vertex> transformedVertices;

        for (int i = 0; i < model.vertices.size(); i+=3) {
            glm::vec3 v = model.vertices[i];
            glm::vec3 n = model.vertices[i+1];
            glm::vec3 t = model.vertices[i+2];

            auto vertex = Vertex{v, n, t};

            Vertex transformedVertex = vertexShader(vertex, uniform);
            transformedVertices.push_back(transformedVertex);
        }

        // 2. Primitive Assembly
        // transformedVertices -> triangles
        std::vector<std::vector<Vertex>> triangles = primitiveAssembly(transformedVertices);


        // 3. Rasterize
        // triangles -> Fragments
        std::vector<Fragment> fragments;
        for (const std::vector<Vertex>& triangleVertices : triangles) {
            std::vector<Fragment> rasterizedTriangle = triangle(
                    triangleVertices[0],
                    triangleVertices[1],
                    triangleVertices[2]
            );

            fragments.insert(
                    fragments.end(),
                    rasterizedTriangle.begin(),
                    rasterizedTriangle.end()
            );
        }


        // 4. Fragment Shader
        // Fragments -> colors

        for (Fragment fragment : fragments) {
            switch (model.shader) {
                case Shader::Earth:
                    point(earthFragmentShader(fragment));
                    break;
                case Shader::Sun:
                    point(sunFragmentShader(fragment));
                    break;
                case Shader::Moon:
                    point(moonFragmentShader(fragment));
                    break;
                case Shader::Jupiter:
                    point(jupiterFragmentShader(fragment));
                    break;
                case Shader::Uranus:
                    point(uranusFragmentShader(fragment));
                    break;
                case Shader::Mars:
                    point(plutoFragmentShader(fragment));
                    break;
                case Shader::Kepler186f:
                    point(keplerFragmentShader(fragment));
                    break;
                case Shader::Noise:
                    point(noiseFragmentShader(fragment));
                    break;
                case Shader::Ship:
                    point(shipFragmentShader(fragment));
                    break;
                default:
                    point(fragmentShader(fragment));
                    break;
            }
        }
    }
}

std::vector<glm::vec3> setupVertexFromObject(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec3>& texCoords){
    std::vector<glm::vec3> vertexBufferObject;

    for (const auto& face : faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            // Get the vertex position
            glm::vec3 vertexPosition = vertices[face.vertexIndices[i]];

            // Get the normal for the current vertex
            glm::vec3 vertexNormal = normals[face.normalIndices[i]];

            // Get the texture for the current vertex
            glm::vec3 vertexTexture = texCoords[face.texIndices[i]];

            // Add the vertex position and normal to the vertex array
            vertexBufferObject.push_back(vertexPosition);
            vertexBufferObject.push_back(vertexNormal);
            vertexBufferObject.push_back(vertexTexture);
        }
    }

    return vertexBufferObject;
}

std::vector<Model> updateCamera(std::vector<Model>& modelsToUpdate, Camera newCamera) {
    for (auto& model : modelsToUpdate) {
        model.uniforms.view = createViewMatrix(newCamera);
    }

    return modelsToUpdate;
}

int main(int argc, char** argv) {
    if (!init()) {
        return 1;
    }

    // Planet
    std::vector<glm::vec3> planetVertices;
    std::vector<Face> planetFaces;
    std::vector<glm::vec3> planetNormals;
    std::vector<glm::vec3> planetTexCoords;

    // Moon
    std::vector<glm::vec3> moonVertices;
    std::vector<Face> moonFaces;
    std::vector<glm::vec3> moonNormals;
    std::vector<glm::vec3> moonTexCoords;

    // Ship
    std::vector<glm::vec3> shipVertices;
    std::vector<Face> shipFaces;
    std::vector<glm::vec3> shipNormals;
    std::vector<glm::vec3> shipTexCoords;

    // Load the OBJ file
    bool success = loadOBJ("../model/sphere.obj", planetVertices, planetFaces, planetNormals, planetTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }
    success = loadOBJ("../model/sphere.obj", moonVertices, moonFaces, moonNormals, moonTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }
    success = loadOBJ("../model/quinjet.obj", shipVertices, shipFaces, shipNormals, shipTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }

    // Process the OBJ file into rotationAnglePlanet VBO
    std::vector<glm::vec3> planetVBO = setupVertexFromObject(planetFaces, planetVertices, planetNormals, planetTexCoords);
    std::vector<glm::vec3> moonVBO = setupVertexFromObject(moonFaces, moonVertices, moonNormals, moonTexCoords);
    std::vector<glm::vec3> shipVBO = setupVertexFromObject(shipFaces, shipVertices, shipNormals, shipTexCoords);

    Uint32 frameStart, frameTime; // For calculating the frames per second

    // ##################################### Ship #####################################
    Uniforms shipUniform = planetBaseUniform(camera);
    float shipScale = 0.1f;

    glm::vec3 shipTranslationVector(0.0f, 0.4f, 3.0f);
    glm::vec3 shipRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 shipScaleFactor(shipScale, shipScale, shipScale);  // Scale of the model

    // Create model
    Model shipModel;
    shipModel.vertices = shipVBO;
    shipModel.uniforms = shipUniform;
    shipModel.shader = Shader::Ship;

    // ##################################### Sun #####################################
    Uniforms sunUniform = planetBaseUniform(camera);
    float sunScale = 1.0f;

    glm::vec3 sunTranslationVector(0.0f, 0.0f, 0.0f);  // Move the model to the center of the world
    glm::vec3 sunRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 sunScaleFactor(sunScale, sunScale, sunScale);  // Scale of the model

    // Create model
    Model sunModel;
    sunModel.vertices = planetVBO;
    sunModel.uniforms = sunUniform;
    sunModel.shader = Shader::Sun;

    // ##################################### Earth #####################################
    Uniforms earthUniform = planetBaseUniform(camera);
    float earthScale = 0.4f;

    glm::vec3 earthRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 earthScaleFactor(earthScale, earthScale, earthScale);  // Scale of the model

    // Create model
    Model earthModel;
    earthModel.vertices = planetVBO;
    earthModel.uniforms = earthUniform;
    earthModel.shader = Shader::Earth;

    // ##################################### Moon #####################################

    // Create Uniform for moon
    Uniforms moonUniform = moonBaseUniform(camera);
    float moonScale = 0.2f;

    glm::vec3 moonScaleFactor(moonScale, moonScale, moonScale);

    // Create model
    Model moonModel;
    moonModel.vertices = moonVBO;
    moonModel.uniforms = moonUniform;
    moonModel.shader = Shader::Moon;

    cout << "Starting loop" << endl;

    bool running = true;

    float rotationSpeedSun = 0.5f;
    float rotationAngleSun = 0.2f;

    float earthOrbitAngle = 0.0f;
    float earthDistanceToSun = 2.5f;
    float rotationSpeedEarth = 1.5f;
    float rotationAngleEarth = 0.0f;

    float moonOrbitAngle = 0.0f;
    float distanceToPlanet = 1.0f;
    float rotationSpeedMoon = 1.0f;
    float rotationAngleMoon = 0.0f;

    while (running) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                float increment = 0.1f;
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        rotationSpeedSun -= increment;
                        rotationSpeedEarth -= increment;
                        rotationSpeedMoon -= increment;

                        break;
                    case SDLK_RIGHT:
                        rotationSpeedSun += increment;
                        rotationSpeedEarth += increment;
                        rotationSpeedMoon += increment;

                        break;
                    case SDLK_SPACE:
//                        sunModel.shader = getNextPlanetTexture(sunModel.shader);
                        break;
                }
            }
        }

        // Camera update

//        camera.targetPosition = camera.targetPosition + glm::vec3{0.0f, 0.0f, 0.1f};
//        std::cout << camera.targetPosition.z << std::endl;
//        camera.cameraPosition = camera.cameraPosition + glm::vec3{0.0f, 0.1f, 0.0f};
//        std::cout << camera.cameraPosition.z << std::endl;

        // Rotations

        rotationAngleSun += rotationSpeedSun;
        rotationAngleEarth += rotationSpeedEarth;

        // ##################################### Ship #####################################
        shipUniform.model = createShipModelMatrix(shipTranslationVector, shipScaleFactor);
        shipModel.modelMatrix = shipUniform.model;

        models.push_back(shipModel);

        // ##################################### Sun #####################################
        sunUniform.model = createModelMatrix(sunTranslationVector, sunScaleFactor, sunRotationAxis, rotationAngleSun);
        sunModel.modelMatrix = sunUniform.model;

        models.push_back(sunModel);

        // ##################################### Earth #####################################
        // move the planet around the sun on the x and y axis
        earthOrbitAngle += 1.0f;
        glm::vec3 earthTranslationVector(
                earthDistanceToSun * cos(glm::radians(earthOrbitAngle)),
                0.0f,
                earthDistanceToSun * sin(glm::radians(earthOrbitAngle))
        );
        earthUniform.model = createModelMatrix(earthTranslationVector, earthScaleFactor, earthRotationAxis, rotationAngleEarth);
        earthModel.modelMatrix = earthUniform.model;

        models.push_back(earthModel);

        // ##################################### Moon #####################################
        // move the moon around the planet on the x and y axis
        moonOrbitAngle += 2.0f;
        glm::vec3 translationVectorMoon(
                distanceToPlanet * cos(glm::radians(moonOrbitAngle)),
                0.0f,
                distanceToPlanet * sin(glm::radians(moonOrbitAngle))
        );
        moonUniform.model = createModelMatrix(translationVectorMoon, moonScaleFactor, sunRotationAxis, rotationAngleMoon);
        moonModel.modelMatrix = moonUniform.model;

        if (hasMoon){
            models.push_back(moonModel);
        }

        clear();

        // Render
        models = updateCamera(models, camera);

//        for (auto model : models) {
//            for (int i = 0; i < 4; ++i) {
//                for (int j = 0; j < 4; ++j) {
//                    std::cout << model.uniforms.view[i][j] << " ";
//                }
//                std::cout << std::endl;
//            }
//        }

        render();

        models.clear();
        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);

        frameTime = SDL_GetTicks() - frameStart;

        // Calculate frames per second and update window title
        if (frameTime > 0) {
            std::ostringstream titleStream;
            titleStream << planet + " FPS: " << 1000.0 / frameTime;  // Milliseconds to seconds
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
