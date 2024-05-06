using namespace std;

#include "number.h"
#include "../stb/stb_image.h"

const char* vertexSource = "#version 430 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in float num;\n"
"out vec2 TexCoord;\n"
"void main() {\n"
    "gl_Position = vec4(aPos, 1.0);\n"
    "TexCoord = vec2(aTexCoord.x + num / 10.0, aTexCoord.y);\n"
"}\0";

const char* fragmentSource = "#version 430 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"
"void main() {\n"
    "FragColor = texture(texture1, TexCoord);\n"
"}\0";

Number::Number() {
    x = 0.0f;
    y = 0.0f;
    value = 0;

    drawInit();
}

Number::Number(float _x, float _y, int val) {
    x = _x;
    y = _y;
    value = val;

    drawInit();
}

Number::~Number() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void Number::drawInit() {
    //setting the grid positions
    vertices[0] = x;
    vertices[1] = y;
    vertices[6] = vertices[0];
    vertices[7] = vertices[1] - (0.1f * 5.0f/3.0f);
    vertices[12] = vertices[0] - 0.1f;
    vertices[13] = vertices[7];
    vertices[18] = vertices[12];
    vertices[19] = vertices[1];

    //idk man
    vertices[5] = (float) value;
    vertices[11] = vertices[5];
    vertices[17] = vertices[5];
    vertices[23] = vertices[5];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    //texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    //value
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 5));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //idk
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);           
    
    int width, height, channels;

    unsigned char* data = stbi_load("../sprites/numbers.png", &width, &height, &channels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "image not loaded" << endl;
    }

    stbi_image_free(data);
}

void Number::draw() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void Number::updateValue(int val) {
    value = val;

    //idk man
    drawInit();
    cout << value << endl;
}