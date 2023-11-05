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

void printCamera(Camera camera) {
    std::cout << "Camera position: " << camera.cameraPosition.x << ", " << camera.cameraPosition.y << ", " << camera.cameraPosition.z << std::endl;
    std::cout << "Camera target: " << camera.targetPosition.x << ", " << camera.targetPosition.y << ", " << camera.targetPosition.z << std::endl;
    std::cout << "Camera up: " << camera.upVector.x << ", " << camera.upVector.y << ", " << camera.upVector.z << std::endl;
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
    success = loadOBJ("../model/quinjet.obj", shipVertices, shipFaces, shipNormals, shipTexCoords);
    if (!success) {
        std::cerr << "Error loading OBJ file!" << std::endl;
        return 1;
    }

    // Process the OBJ file into rotationAnglePlanet VBO
    std::vector<glm::vec3> planetVBO = setupVertexFromObject(planetFaces, planetVertices, planetNormals, planetTexCoords);
    std::vector<glm::vec3> shipVBO = setupVertexFromObject(shipFaces, shipVertices, shipNormals, shipTexCoords);

    Uint32 frameStart, frameTime; // For calculating the frames per second

    // ##################################### Ship #####################################
    Uniforms shipUniform = planetBaseUniform(camera);
    float shipScale = 0.1f;

    glm::vec3 shipTranslationVector(0.0f, 0.4f, 13.5f);
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
    float earthScale = 0.3f;

    glm::vec3 earthRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 earthScaleFactor(earthScale, earthScale, earthScale);  // Scale of the model

    // Create model
    Model earthModel;
    earthModel.vertices = planetVBO;
    earthModel.uniforms = earthUniform;
    earthModel.shader = Shader::Earth;

    // ##################################### Jupiter #####################################
    Uniforms jupiterUniform = planetBaseUniform(camera);
    float jupiterScale = 0.7f;

    glm::vec3 jupiterRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 jupiterScaleFactor(jupiterScale, jupiterScale, jupiterScale);  // Scale of the model

    // Create model
    Model jupiterModel;
    jupiterModel.vertices = planetVBO;
    jupiterModel.uniforms = jupiterUniform;
    jupiterModel.shader = Shader::Jupiter;

    // ##################################### Uranus #####################################
    Uniforms uranusUniform = planetBaseUniform(camera);
    float uranusScale = 0.5f;

    glm::vec3 uranusRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 uranusScaleFactor(uranusScale, uranusScale, uranusScale);  // Scale of the model

    // Create model
    Model uranusModel;
    uranusModel.vertices = planetVBO;
    uranusModel.uniforms = uranusUniform;
    uranusModel.shader = Shader::Uranus;

    // ##################################### Mars #####################################
    Uniforms marsUniform = planetBaseUniform(camera);
    float marsScale = 0.4f;

    glm::vec3 marsRotationAxis(0.0f, 1.0f, 0.0f); // Rotate around the Y-axis every model
    glm::vec3 marsScaleFactor(marsScale, marsScale, marsScale);  // Scale of the model

    // Create model
    Model marsModel;
    marsModel.vertices = planetVBO;
    marsModel.uniforms = marsUniform;
    marsModel.shader = Shader::Mars;



    cout << "Starting loop" << endl;

    bool running = true;
    bool orbiting = true;

    float rotationSpeedPlanets = 1.0f;  // Base speed of the planets
    float orbitSpeedPlanets = 1.0f;  // Base speed of the planets

    float rotationAngleSun = 0.2f;

    float earthOrbitAngle = 0.0f;
    float earthDistanceToSun = 3.5f;
    float rotationAngleEarth = 0.0f;

    float marsOrbitAngle = 0.0f;
    float marsDistanceToSun = 5.0f;
    float rotationAngleMars = 0.0f;

    float jupiterOrbitAngle = 0.0f;
    float jupiterDistanceToSun = 6.5f;
    float rotationAngleJupiter = 0.0f;

    float uranusOrbitAngle = 0.0f;
    float uranusDistanceToSun = 8.0f;
    float rotationAngleUranus = 0.0f;

    while (running) {
        frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                float increment = 0.5f;
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_w:
                        std::cout << "w" << std::endl;
                        camera = moveUp(camera);
                        printCamera(camera);
                        break;
                    case SDLK_s:
                        std::cout << "s" << std::endl;
                        camera = moveDown(camera);
                        printCamera(camera);
                        break;
                    case SDLK_a:
                        std::cout << "a" << std::endl;
                        camera = moveLeft(camera);
                        printCamera(camera);
                        break;
                    case SDLK_d:
                        std::cout << "d" << std::endl;
                        camera = moveRight(camera);
                        printCamera(camera);
                        break;
                    case SDLK_UP:
                        std::cout << "up" << std::endl;
                        camera = zoomIn(camera);
                        printCamera(camera);
                        break;
                    case SDLK_DOWN:
                        std::cout << "down" << std::endl;
                        camera = zoomOut(camera);
                        printCamera(camera);
                        break;
                    case SDLK_LEFT:
                        rotationSpeedPlanets -= increment;
                        orbitSpeedPlanets -= increment;

                        break;
                    case SDLK_RIGHT:
                        rotationSpeedPlanets += increment;
                        orbitSpeedPlanets += increment;

                        break;
                    case SDLK_SPACE:
                        orbiting = !orbiting;
                        break;
                }
            }
        }

        // ##################################### Rotations #####################################

        rotationAngleSun += rotationSpeedPlanets * 0.2f;
        rotationAngleEarth += rotationSpeedPlanets * 0.8f;
        rotationAngleMars += rotationSpeedPlanets * 0.6f;
        rotationAngleJupiter += rotationSpeedPlanets * 0.4f;
        rotationAngleUranus += rotationSpeedPlanets * 0.3f;

        // ##################################### Orbits #####################################
        if(orbiting) {
            earthOrbitAngle += 1.0f * orbitSpeedPlanets;
            marsOrbitAngle += 0.8f * orbitSpeedPlanets;
            jupiterOrbitAngle += 0.6f * orbitSpeedPlanets;
            uranusOrbitAngle += 0.4f * orbitSpeedPlanets;
        }

        // ##################################### Ship #####################################
        shipUniform.model = createShipModelMatrix(shipTranslationVector, shipScaleFactor);
        shipModel.modelMatrix = shipUniform.model;


        // ##################################### Sun #####################################
        sunUniform.model = createModelMatrix(sunTranslationVector, sunScaleFactor, sunRotationAxis, rotationAngleSun);
        sunModel.modelMatrix = sunUniform.model;

        models.push_back(sunModel);

        // ##################################### Earth #####################################
        // move the planet around the sun on the x and y axis

        glm::vec3 earthTranslationVector(
                earthDistanceToSun * cos(glm::radians(earthOrbitAngle)),
                0.0f,
                earthDistanceToSun * sin(glm::radians(earthOrbitAngle))
        );
        earthUniform.model = createModelMatrix(earthTranslationVector, earthScaleFactor, earthRotationAxis, rotationAngleEarth);
        earthModel.modelMatrix = earthUniform.model;

        models.push_back(earthModel);

        // ##################################### Mars #####################################
        // move the planet around the sun on the x and y axis

        glm::vec3 marsTranslationVector(
                marsDistanceToSun * cos(glm::radians(marsOrbitAngle)),
                0.0f,
                marsDistanceToSun * sin(glm::radians(marsOrbitAngle))
        );
        marsUniform.model = createModelMatrix(marsTranslationVector, marsScaleFactor, marsRotationAxis, rotationAngleMars);
        marsModel.modelMatrix = marsUniform.model;

        models.push_back(marsModel);

        // ##################################### Jupiter #####################################
        // move the planet around the sun on the x and y axis

        glm::vec3 jupiterTranslationVector(
                jupiterDistanceToSun * cos(glm::radians(jupiterOrbitAngle)),
                0.0f,
                jupiterDistanceToSun * sin(glm::radians(jupiterOrbitAngle))
        );
        jupiterUniform.model = createModelMatrix(jupiterTranslationVector, jupiterScaleFactor, jupiterRotationAxis, rotationAngleJupiter);
        jupiterModel.modelMatrix = jupiterUniform.model;

        models.push_back(jupiterModel);

        // ##################################### Uranus #####################################
        // move the planet around the sun on the x and y axis

        glm::vec3 uranusTranslationVector(
                uranusDistanceToSun * cos(glm::radians(uranusOrbitAngle)),
                0.0f,
                uranusDistanceToSun * sin(glm::radians(uranusOrbitAngle))
        );
        uranusUniform.model = createModelMatrix(uranusTranslationVector, uranusScaleFactor, uranusRotationAxis, rotationAngleUranus);
        uranusModel.modelMatrix = uranusUniform.model;

        models.push_back(uranusModel);


        // ##################################### Render #####################################
        clear();

        models = updateCamera(models, camera);

        models.push_back(shipModel);

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
