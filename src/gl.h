// gl.h
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <array>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "color.h"
#include "fragment.h"
#include "noise.h"


struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> normalIndices;
    std::array<int, 3> texIndices;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

Color currentColor = {255, 255, 255, 255}; // Initially set to white
Color clearColor = {0, 0, 0, 255}; // Initially set to black
std::array<std::array<float, SCREEN_WIDTH>, SCREEN_HEIGHT> zbuffer;

// Function to set a specific pixel in the framebuffer to the currentColor
void point(Fragment f) {
    if (f.position.z < zbuffer[f.position.y][f.position.x]) {
        SDL_SetRenderDrawColor(renderer, f.color.r, f.color.g, f.color.b, f.color.a);
        SDL_RenderDrawPoint(renderer, f.position.x, f.position.y);
        zbuffer[f.position.y][f.position.x] = f.position.z;
    }
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error: Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error: Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    setupNoise();

    return true;
}

void setColor(const Color& color) {
    currentColor = color;
}


// Function to clear the framebuffer with the clearColor
void clear() {
    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(renderer);

    // reset the zbuffer
    for (auto &row : zbuffer) {
        std::fill(row.begin(), row.end(), 99999.0f);
    }

    // despues de limpiar el zbuffer, quiero simular estrellas
    // en el fondo, para eso voy a usar el ruido

    // voy a generar un numero aleatorio entre 0 y 1
    // si el numero es menor a 0.0001, voy a dibujar una estrella
    // en una posicion aleatoria de la pantalla
    // voy a usar el ruido para generar la posicion aleatoria
    // y voy a usar el ruido para generar el color de la estrella

    int numStars = 1000;
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    for (int i = 0; i < numStars; i++) {
        float random = (float)rand() / RAND_MAX;
        if (random != 0.0001f) {
            // necesito valores X y Y de 0 a SCREEN_WIDTH y SCREEN_HEIGHT aleatorios
            // voy a usar el ruido para generar esos valores
            float x = noise.GetNoise((float)i + ox, oy) * SCREEN_WIDTH;
            float y = noise.GetNoise((float)i + oy, ox) * SCREEN_HEIGHT;
            float z = noise.GetNoise((float)i + ox, oy) * 100.0f;

            // voy a generar un color aleatorio para la estrella
            float r = noise.GetNoise((float)i + ox, oy) * 255.0f;
            float g = noise.GetNoise((float)i + oy, ox) * 255.0f;
            float b = noise.GetNoise((float)i + ox, oy) * 255.0f;

            x = std::abs(x);
            y = std::abs(y);
            z = std::abs(z);

            r = std::abs(r);
            g = std::abs(g);
            b = std::abs(b);

//            std::cout << x << ", " << y << ", " << z << std::endl;
//            std::cout << r << ", " << g << ", " << b << std::endl;

            // voy a generar un punto de tamaño aleatorio para la estrella
            float size = noise.GetNoise((float)i + ox, oy) * 5.0f;

            Fragment f = {
                {x, y, z},
                Color{255, 255, 255},
                size
            };

            point(f);
        }
    }
}



