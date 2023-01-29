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
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

class Line {
    unsigned int shaderProgram;
    unsigned int VBO, VAO;
    float vertices[6];

    public:
        Line() {};
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


    const char* pvertexShaderSource = "#version 430 core\n"
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

    const char* pfragmentShaderSource = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "in vec3 ourColor;\n"
    "uniform sampler2D texture1;\n"
    "void main() {\n"
        "FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);\n"
    "}\0";

class Puyo {
    GLuint shaderProgram;
    unsigned int VBO, VAO, EBO, texture;
    int x, y;
    float r, g, b;
    int g_x, g_y;
    bool isDrawReady = false;

    float vertices[32] = {
        //vertex           texture
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, //top right
        1.0f, -1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f, //bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f, //bottom left
        -1.0f, 1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f //top left
    };

    unsigned int indices[6] {
        0, 1, 3,
        1, 2, 3
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

        Puyo(int _x, int _y, float _r, float _g, float _b, int gx, int gy) {
            //update global grid positions and color
            x = _x;
            y = _y;
            r = _r;
            g = _g;
            b = _b;
            g_x = gx;
            g_y = gy;

            //draw setup
            drawInit();           
        }

        void drawInit() {
            //setting the grid positions
            vertices[0] = (-g_x / 20.0f) + 0.1f * x + 0.1f;
            vertices[1] = (-g_y / 20.0f) + 0.1f * y + 0.1f;
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
            isDrawReady = true;
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
            vertices[8] = vertices[0];
            vertices[9] = vertices[1] - 0.1f;
            vertices[16] = vertices[0] - 0.1f;
            vertices[17] = vertices[9];
            vertices[24] = vertices[16];
            vertices[25] = vertices[1];

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

        bool getDrawReady() {
            return isDrawReady;
        }

        int getX() {
            return x;
        }

        int getY() {
            return y;
        }

        ~Puyo() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            if (glIsProgram(shaderProgram)) {
                glDeleteProgram(shaderProgram);
            }
        }
};

class Grid {
    int xSize, ySize;
    Line* gridLines;
    Puyo** puyoGrid;
    Puyo currPuyo;

    public:
        Grid (int x, int y) {
            xSize = x;
            ySize = y;

            puyoGrid = new Puyo*[x];
            gridLines = new Line[x + y + 2];

            for (int i = 0; i < x; i++) {
                puyoGrid[i] = new Puyo[y];
            }

            for (int i = 0; i < x + 1; i++) {
                gridLines[i] = *(new Line((-x / 20.0f) + 0.1 * i, (y / 20.0f), (-x / 20.0f) + 0.1f * i, (-y / 20.0f)));
            }

            for (int i = 0; i < y + 1; i++) {
                gridLines[x + 1 + i] = *(new Line((-x / 20.0f), (-y / 20.0f) + 0.1f * i, (x / 20.0f), (-y / 20.0f) + 0.1f * i));
            }
        }

        void draw() {
            for (int i = 0; i < xSize; i++) {
                for (int j = 0; j < ySize; j++) {
                    if (puyoGrid[i][j].getDrawReady()) {
                        puyoGrid[i][j].draw();
                    }
                }
            }

            for (int i = 0; i < xSize + ySize + 2; i++) {
                gridLines[i].draw();
            }
        }

        void setCurrPuyo(Puyo* puyo) {
            currPuyo = *puyo;
        }

        Puyo* getCurrPuyo() {
            return &currPuyo;
        }

        Puyo* addPuyo(int x, int y, float r, float g, float b) {
            //needs to be dereferenced pointer ????
            puyoGrid[x][y] = *(new Puyo(x, y, r, g, b, xSize, ySize));
            return &(puyoGrid[x][y]);
        }

        void move(int xInc, int yInc) {
            int px = currPuyo.getX();
            int py = currPuyo.getY();
            int nx = px + xInc;
            int ny = py + yInc;

            if (nx >= 0 && nx < xSize && ny >= 0 && ny < ySize && !puyoGrid[nx][ny].getDrawReady()) {
                Puyo temp = puyoGrid[nx][ny];
                currPuyo.move(xInc, yInc);
                puyoGrid[nx][ny] = currPuyo;
                puyoGrid[px][py] = temp;
            }
        }
};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int gridX = 6; 
int gridY = 12;
Grid* grid;

int main(void) {
    GLFWwindow* window;

    if (!glfwInit())
        ::exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "icon");

    window = glfwCreateWindow(640, 640, "icon", NULL, NULL);
    glfwSetWindowTitle(window, "ぷよぷよcobalt");

    if (!window) {
        glfwTerminate();
        ::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    double theta = 0;

    grid = new Grid(gridX, gridY);

    //grid x, y, color r, g, b]
    grid->addPuyo(0, 0, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(0, 1, 0.0f, 1.0f, 0.0f);
    grid->addPuyo(0, 2, 0.0f, 1.0f, 0.0f);
    grid->addPuyo(1, 0, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(1, 1, 0.0f, 1.0f, 0.0f);
    grid->addPuyo(1, 2, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(2, 0, 0.0f, 0.0f, 1.0f);
    grid->addPuyo(2, 1, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(2, 2, 0.0f, 0.0f, 1.0f);
    grid->addPuyo(3, 0, 1.0f, 1.0f, 0.0f);
    grid->addPuyo(3, 1, 0.0f, 0.0f, 1.0f);
    grid->addPuyo(3, 2, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(4, 0, 1.0f, 1.0f, 0.0f);
    grid->addPuyo(4, 1, 0.0f, 0.0f, 1.0f);
    grid->addPuyo(4, 2, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(5, 0, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(5, 1, 1.0f, 0.0f, 0.0f);
    grid->addPuyo(5, 2, 1.0f, 1.0f, 0.0f);
    grid->addPuyo(5, 3, 1.0f, 1.0f, 0.0f);    

    grid->setCurrPuyo(grid->addPuyo(3, 10, 0.0f, 1.0f, 0.0f));
    glfwSetKeyCallback(window, keyCallback);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    while (!glfwWindowShouldClose(window)) {
        //set viewport size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        //background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //draw...
        grid->draw();

        //check events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    ::exit(EXIT_SUCCESS);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_LEFT:
            grid->move(-1, 0);
            break;
        case GLFW_KEY_RIGHT:
            grid->move(1, 0);
            break;
        case GLFW_KEY_DOWN:
            grid->move(0, -1);
            break;
    }
}