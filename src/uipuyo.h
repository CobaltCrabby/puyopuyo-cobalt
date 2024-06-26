#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h> 

#include "color.h"
#include <iostream>

using namespace std;

class UIPuyo {
    GLuint shaderProgram;
    unsigned int VBO, VAO, EBO, texture;
    float gx, gy;
    float r, g, b;
    enum color color;

    float vertices[36] = {
        //vertex           texture      rgba color
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, //top right
        1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, //bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, //bottom left
        -1.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f //top left
    };

    unsigned int indices[6] {
        0, 1, 2,
        0, 2, 3
    };

    public:
        UIPuyo();

        UIPuyo(float x, float y, float size, enum color c);

        ~UIPuyo();

        void move();

        void drawInit(float x, float y, float size);

        void draw();

        enum color getColor();
};