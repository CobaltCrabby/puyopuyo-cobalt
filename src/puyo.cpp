#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#include "color.h"

#include <iostream>

using namespace std;

const char* pvertexShaderSource = "#version 430 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in vec4 aColor;\n"
"out vec2 TexCoord;\n"
"out vec4 ourColor;\n"
"void main() {\n"
    "gl_Position = vec4(aPos, 1.0);\n"
    "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "ourColor = aColor;\n"
"}\0";

const char* pfragmentShaderSource = "#version 430 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"in vec4 ourColor;\n"
"uniform sampler2D texture1;\n"
"void main() {\n"
    "FragColor = texture(texture1, TexCoord) * ourColor;\n"
"}\0";

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
        Puyo() {
            x = 0; 
            y = 0; 
            r = 0; 
            g = 0; 
            b = 0; 
            g_x = 0;
            g_y = 0;
        }

        Puyo(int _x, int _y, enum color _c, int gx, int gy) {
            //update global grid positions and color
            x = _x;
            y = _y;
            r = 0;
            g = 0;
            b = 0;
            g_x = gx;
            g_y = gy;
            color = _c;

            switch (_c) {
                case red:
                    r = 1.0f;
                    break;
                case green:
                    g = 1.0f;
                    break;
                case blue:
                    b = 1.0f;
                    break;
                case yellow:
                    g = 1.0f; 
                    r = 1.0f;
                    break;
            }

            //draw setup
            drawInit();           
        }

        ~Puyo() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteProgram(shaderProgram);
        }

        void drawInit() {
            //setting the grid positions
            vertices[0] = (-g_x / 20.0f) + 0.1f * x + 0.1f;
            vertices[1] = (-g_y / 20.0f) + 0.1f * y + 0.1f;
            vertices[9] = vertices[0];
            vertices[10] = vertices[1] - 0.1f;
            vertices[18] = vertices[0] - 0.1f;
            vertices[19] = vertices[10];
            vertices[27] = vertices[18];
            vertices[28] = vertices[1];

            //setting color
            for (int i = 0; i < 4; i++) {
                vertices[5 + 9 * i] = r;
                vertices[6 + 9 * i] = g;
                vertices[7 + 9 * i] = b;
            }

            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &pvertexShaderSource, NULL);
            glCompileShader(vertexShader);

            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &pfragmentShaderSource, NULL);
            glCompileShader(fragmentShader);

            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

            //position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(0);

            //texture
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 3));
            glEnableVertexAttribArray(1);

            //color
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 5));
            glEnableVertexAttribArray(2);

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            //idk
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);           
            
            int width, height, channels;

            unsigned char* data = stbi_load("../sprites/cobale_256x256.png", &width, &height, &channels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                cout << "image not loaded" << endl;
            }

            stbi_image_free(data);
        }

        void draw() {
            glBindTexture(GL_TEXTURE_2D, texture);
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        }

        void move(int ax, int by) {
            x += ax;
            y += by;

            //setting the grid positions
            vertices[0] = (-g_x / 20.0f) + 0.1f * x + 0.1f;
            vertices[1] = (-g_y / 20.0f) + 0.1f * y + 0.1f;
            vertices[9] = vertices[0];
            vertices[10] = vertices[1] - 0.1f;
            vertices[18] = vertices[0] - 0.1f;
            vertices[19] = vertices[10];
            vertices[27] = vertices[18];
            vertices[28] = vertices[1];

            //drawInit();
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        }

        void display() {
            cout << "r: " << r << " g: " << g << " b: " << b << endl;
        }

        int getX() {
            return x;
        }

        int getY() {
            return y;
        }

        enum color getColor() {
            return color;
        }

        bool getPopChecked() {
            return popChecked;
        }

        void setPopChecked(bool b) {
            popChecked = b;
        }

        void setTransparency(float a) {
            vertices[8] = a;
            vertices[17] = a;
            vertices[26] = a;
            vertices[35] = a;

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        }
};