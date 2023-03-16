#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

const char *lvertexShaderSource = "#version 430 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *lfragmentShaderSource = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(125.0f/255.0f, 1.0f, 125.0f/255.0f, 1.0f);\n"
    "}\n\0";

class Line {
    GLuint shaderProgram;
    unsigned int VBO, VAO;
    float vertices[6];

    public:
        Line() { }

        Line (float x1, float y1, float x2, float y2) {
            vertices[0] = x1;
            vertices[1] = y1;
            vertices[2] = 0.0f;
            vertices[3] = x2;
            vertices[4] = y2;
            vertices[5] = 0.0f;

            // vertex shader
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &lvertexShaderSource, NULL);
            glCompileShader(vertexShader);

            // fragment shader
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &lfragmentShaderSource, NULL);
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