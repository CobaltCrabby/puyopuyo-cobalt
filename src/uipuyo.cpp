#include "uipuyo.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

// const char* pvertexShaderSource = "#version 430 core\n"
// "layout (location = 0) in vec3 aPos;\n"
// "layout (location = 1) in vec2 aTexCoord;\n"
// "layout (location = 2) in vec4 aColor;\n"
// "out vec2 TexCoord;\n"
// "out vec4 ourColor;\n"
// "void main() {\n"
//     "gl_Position = vec4(aPos, 1.0);\n"
//     "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
//     "ourColor = aColor;\n"
// "}\0";

// const char* pfragmentShaderSource = "#version 430 core\n"
// "out vec4 FragColor;\n"
// "in vec2 TexCoord;\n"
// "in vec4 ourColor;\n"
// "uniform sampler2D texture1;\n"
// "void main() {\n"
//     "FragColor = texture(texture1, TexCoord) * ourColor;\n"
// "}\0";

UIPuyo::UIPuyo(float centerX, float topY, float size, enum color c) {
    switch (c) {
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

    drawInit(centerX, topY, size);
}

void UIPuyo::drawInit(float x, float y, float size) {
        // //setting the grid positions
        // vertices[0] = x;
        // vertices[1] = y;
        // vertices[9] = x;
        // vertices[10] = y - size;
        // vertices[18] = x - size;
        // vertices[19] = y - size;
        // vertices[27] = x - size;
        // vertices[28] = y;

        // //setting color
        // for (int i = 0; i < 4; i++) {
        //     vertices[5 + 9 * i] = r;
        //     vertices[6 + 9 * i] = g;
        //     vertices[7 + 9 * i] = b;
        // }

        // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        // glShaderSource(vertexShader, 1, &pvertexShaderSource, NULL);
        // glCompileShader(vertexShader);

        // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        // glShaderSource(fragmentShader, 1, &pfragmentShaderSource, NULL);
        // glCompileShader(fragmentShader);

        // shaderProgram = glCreateProgram();
        // glAttachShader(shaderProgram, vertexShader);
        // glAttachShader(shaderProgram, fragmentShader);
        // glLinkProgram(shaderProgram);

        // glDeleteShader(vertexShader);
        // glDeleteShader(fragmentShader);

        // glGenVertexArrays(1, &VAO);
        // glGenBuffers(1, &VBO);
        // glGenBuffers(1, &EBO);

        // glBindVertexArray(VAO);

        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // //position
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) 0);
        // glEnableVertexAttribArray(0);

        // //texture
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 3));
        // glEnableVertexAttribArray(1);

        // //color
        // glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*) (sizeof(float) * 5));
        // glEnableVertexAttribArray(2);

        // glGenTextures(1, &texture);
        // glBindTexture(GL_TEXTURE_2D, texture);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        // //idk
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);           
        
        // int width, height, channels;

        // unsigned char* data = stbi_load("../sprites/cobale_256x256.png", &width, &height, &channels, 0);
        // if (data) {
        //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //     glGenerateMipmap(GL_TEXTURE_2D);
        // } else {
        //     cout << "image not loaded" << endl;
        // }

        // stbi_image_free(data);
    }