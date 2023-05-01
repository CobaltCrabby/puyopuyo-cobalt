#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include "color.h"
#include <iostream>

using namespace std;

class Puyo {
    GLuint shaderProgram;
    unsigned int VBO, VAO, EBO, texture;
    int x, y;
    float r, g, b;
    int g_x, g_y;
    bool popChecked = false;
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
        Puyo();

        Puyo(int _x, int _y, enum color _c, int gx, int gy);

        ~Puyo();

        void drawInit();
        void drawInit(float x, float y, float size);

        void draw();

        void move(int ax, int by);

        void display();

        int getX();

        int getY();

        enum color getColor();

        bool getPopChecked();

        void setPopChecked(bool b);

        void setTransparency(float a);
};