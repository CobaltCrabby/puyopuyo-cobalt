#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

class Number {
    GLuint shaderProgram;
    unsigned int VBO, VAO, EBO, texture;
    float x, y;
    int value;

    float vertices[24] = {
        //vertex           texture      value(?)
        1.0f, 1.0f, 0.0f,  0.1f, 0.0f,  0.0f,
        1.0f, -1.0f, 0.0f,  0.1f, 1.0f,  0.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  0.0f,
        -1.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f
    };

    unsigned int indices[6] {
        0, 1, 2,
        0, 2, 3
    };

    public:
        Number();

        Number(float _x, float _y, int val);

        ~Number();

        void drawInit();

        void draw();

        void updateValue(int val);
};