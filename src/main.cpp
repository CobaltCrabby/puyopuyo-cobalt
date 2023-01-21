#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "../stb/stb.cpp"
#include <iostream>
#include <cmath>

using namespace std;
//g++ -c main.cpp && g++ main.o -o main.exec -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor && ./main.exec

static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

class Line {
    unsigned int shaderProgram;
    unsigned int VBO, VAO;
    float vertices[6];

    public:
        Line () {}

        Line (float x1, float y1, float x2, float y2) {
            vertices[0] = x1;
            vertices[1] = y1;
            vertices[2] = 0.0f;
            vertices[3] = x2;
            vertices[4] = y2;
            vertices[5] = 0.0f;

            // build and compile our shader program
            // ------------------------------------
            // vertex shader
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);

            // fragment shader
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);

            // link shaders
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
            glBindBuffer(GL_ARRAY_BUFFER, 0); 

            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0); 
        }

        void draw() {
            //use shaders and bind VAO
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, 2);
        }

        ~Line() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteProgram(shaderProgram);
        }
};

class Puyo {
    unsigned int shaderProgram;
    unsigned int VBO, VAO, EBO, texture;
    float vertices[32] = {
        //vertex           texture
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, //top right
        1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,//bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,//bottom left
        -1.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f//top left
    };

    unsigned int indices[6] {
        0, 1, 3,
        1, 2, 3
    };

    const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "layout (location = 2) in vec3 aColor;\n"
    "out vec2 TexCoord;\n"
    "out vec3 ourColor;\n"
    "void main() {\n"
        "gl_Position = vec4(aPos, 1.0);\n"
        "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
        "ourColor = aColor;\n"
    "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "in vec3 ourColor;\n"
    "uniform sampler2D texture1;\n"
    "void main() {\n"
        "FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);\n"
    "}\0";

    public:
        Puyo(int x, int y, float r, float g, float b) {
            //setting the grid positions
            vertices[0] = -0.3f + 0.1f * x + 0.1f;
            vertices[1] = -0.6f + 0.1f * y + 0.1f;
            vertices[8] = vertices[0];
            vertices[9] = vertices[1] - 0.1f;
            vertices[16] = vertices[0] - 0.1f;
            vertices[17] = vertices[9];
            vertices[24] = vertices[16];
            vertices[25] = vertices[1];

            //setting color
            for (int i = 0; i < 4; i++) {
                vertices[5 + 8 * i] = r;
                vertices[6 + 8 * i] = g;
                vertices[7 + 8 * i] = b;
            }
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);

            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            //position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
            glEnableVertexAttribArray(0);

            //texture
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (sizeof(float) * 3));
            glEnableVertexAttribArray(1);

            //color
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (sizeof(float) * 5));
            glEnableVertexAttribArray(2);

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            
            int width, height, channels;

            unsigned char* data = stbi_load("../sprites/cobale_256x256.png", &width, &height, &channels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                cout << "image not loaded";
            }

            stbi_image_free(data);
        }

        Puyo() {
            
        }

        void draw() {
            glBindTexture(GL_TEXTURE_2D, texture);
            glUseProgram(shaderProgram);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
            //glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        ~Puyo() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteProgram(shaderProgram);
        }
};

int main(void) {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "icon");

    window = glfwCreateWindow(640, 640, "icon", NULL, NULL);
    glfwSetWindowTitle(window, "ぷよぷよcobalt");

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    double theta = 0;

    Line* grid[21];

    for (int i = 0; i < 7; i++) {
        grid[i] = new Line(-0.3f + 0.1 * i, 0.6f, -0.3f + 0.1f * i, -0.6f);
    }

    for (int i = 7; i < 20; i++) {
        grid[i] = new Line(-0.3f, -0.6f + 0.1f * (i - 7), 0.3f, -0.6f + 0.1f * (i - 7));
    }

    //grid x, y, color r, g, b
    Puyo* puyo[19] = {
        new Puyo(0, 0, 1.0f, 0.0f, 0.0f),
        new Puyo(1, 0, 1.0f, 0.0f, 0.0f),
        new Puyo(0, 1, 0.0f, 1.0f, 0.0f),
        new Puyo(1, 1, 0.0f, 1.0f, 0.0f),
        new Puyo(2, 1, 1.0f, 0.0f, 0.0f),
        new Puyo(0, 2, 0.0f, 1.0f, 0.0f),
        new Puyo(1, 2, 1.0f, 0.0f, 0.0f),
        new Puyo(2, 0, 0.0f, 0.0f, 1.0f),
        new Puyo(3, 0, 1.0f, 1.0f, 0.0f),
        new Puyo(4, 0, 1.0f, 1.0f, 0.0f),
        new Puyo(5, 0, 1.0f, 0.0f, 0.0f),
        new Puyo(3, 1, 0.0f, 0.0f, 1.0f),
        new Puyo(4, 1, 0.0f, 0.0f, 1.0f),
        new Puyo(5, 1, 1.0f, 0.0f, 0.0f),
        new Puyo(2, 2, 0.0f, 0.0f, 1.0f),
        new Puyo(3, 2, 1.0f, 1.0f, 0.0f),
        new Puyo(4, 2, 1.0f, 1.0f, 0.0f),
        new Puyo(4, 3, 1.0f, 0.0f, 0.0f),
        new Puyo(4, 4, 1.0f, 0.0f, 0.0f)
    };

    while (!glfwWindowShouldClose(window)) {
        //set viewport size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //draw...
        for (int i = 0; i < sizeof(puyo) / sizeof(puyo[0]); i++) {
            puyo[i]->draw();
        }

        for (int i = 0; i < 20; i++) {
            grid[i]->draw();
        }

        //check events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}