#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>

using namespace std;
//g++ -c main.cpp && g++ main.o -o main.exec -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor && ./main.exec
//g++ -c -g main.cpp && g++ main.o -o main.exec -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor && valgrind --track-origins=yes --log-file=cobalt.rpt ./main.exec
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

enum color{red, green, blue, yellow};

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

            //yandev fix true
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

        ~Puyo() {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteProgram(shaderProgram);
        }
};

const int dropSpeedLUT[12] {
    10,
    15,
    19,
    22,
    24,
    26,
    28,
    30,
    32,
    34,
    36,
    38
};
//could be thirteen if you have add puyos above the board

class Grid {
    int xSize, ySize;
    Line** gridLines;
    Puyo*** puyoGrid;
    Puyo* currPuyo;

    int bouncingNum = -1;
    int dropNum = -1;
    int dropIndex = 0;

    tuple<int, int> prevDrop = make_tuple(-1, -1);
    vector<tuple<int, int>> matched;

    public:
        Grid (int x, int y) {
            xSize = x;
            ySize = y;

            puyoGrid = new Puyo**[x];
            gridLines = new Line*[x + y + 2];

            for (int i = 0; i < x; i++) {
                puyoGrid[i] = new Puyo*[y];
                for (int j = 0; j < y; j++) {
                    puyoGrid[i][j] = nullptr;
                }
            }

            for (int i = 0; i < x + 1; i++) {
                gridLines[i] = new Line((-x / 20.0f) + 0.1 * i, (y / 20.0f), (-x / 20.0f) + 0.1f * i, (-y / 20.0f));
            }

            for (int i = 0; i < y + 1; i++) {
                gridLines[x + 1 + i] = new Line((-x / 20.0f), (-y / 20.0f) + 0.1f * i, (x / 20.0f), (-y / 20.0f) + 0.1f * i);
            }
        }

        void draw() {
            for (int i = 0; i < xSize; i++) {
                for (int j = 0; j < ySize; j++) {
                    if (puyoGrid[i][j] != nullptr) {
                        puyoGrid[i][j]->draw();
                    }
                }
            }

            for (int i = 0; i < xSize + ySize + 2; i++) {
                gridLines[i]->draw();
            }
        }

        void setCurrPuyo(Puyo* puyo) {
            currPuyo = puyo;
        }

        Puyo* getCurrPuyo() {
            return currPuyo;
        }

        Puyo* addPuyo(int x, int y, enum color c) {
            puyoGrid[x][y] = new Puyo(x, y, c, xSize, ySize);
            return puyoGrid[x][y];
        }

        bool move(int xInc, int yInc) {
            int px = currPuyo->getX();
            int py = currPuyo->getY();
            int nx = px + xInc;
            int ny = py + yInc;

            if (nx >= 0 && nx < xSize && ny >= 0 && ny < ySize && puyoGrid[nx][ny] == nullptr) {
                Puyo* temp = puyoGrid[nx][ny];
                currPuyo->move(xInc, yInc);
                puyoGrid[nx][ny] = currPuyo;
                puyoGrid[px][py] = temp;

                popPuyo(nx, ny);
            } else {
                return false;
            }
            return true;
        }

        void popPuyo(int _x, int _y) {
            vector<tuple<int, int>> match; 
            match = checkPops(_x, _y, matched, puyoGrid[_x][_y]->getColor());

            for (int i = 0; i < xSize; i++) {
                for (int j = 0; j < ySize; j++) {
                    if (puyoGrid[i][j] != nullptr) {
                        puyoGrid[i][j]->setPopChecked(false);
                    }
                }
            }

            if (match.size() >= 4) {
                startBouncingTimer();
                matched = match;
            }
        }

        vector<tuple<int, int>> checkPops(int x, int y, vector<tuple<int, int>> num, enum color c) {
            if (x < 0 || x >= xSize || y < 0 || y >= ySize || puyoGrid[x][y] == nullptr || puyoGrid[x][y]->getPopChecked() || puyoGrid[x][y]->getColor() != c) {
                return num;
            }

            num.push_back(make_tuple(x, y));
            puyoGrid[x][y]->setPopChecked(true);

            num = checkPops(x - 1, y, num, c);
            num = checkPops(x + 1, y, num, c);
            num = checkPops(x, y - 1, num, c);
            num = checkPops(x, y + 1, num, c);

            return num;
        }

        void deletePuyo(int _x, int _y) {
            Puyo* temp = puyoGrid[_x][_y];
            puyoGrid[_x][_y] = nullptr;
            delete temp;
            currPuyo = nullptr;
        }

        void bouncingTimer() {
            if (bouncingNum == -1) return;
            if (bouncingNum == 64) {
                for (int i = 0; i < matched.size(); i++) {
                    tuple<int, int> coords = matched.at(i);
                    deletePuyo(get<0>(coords), get<1>(coords));
                }
                bouncingNum = -1;
                matched.clear();
                startDropTimer();
                return;
            } 

            if (bouncingNum % 8 == 0) {
                for (int i = 0; i < matched.size(); i++) {
                    tuple<int, int> coords = matched.at(i);
                    puyoGrid[get<0>(coords)][get<1>(coords)]->setTransparency(1.0f);
                }
            } else if (bouncingNum % 8 == 2) {
                for (int i = 0; i < matched.size(); i++) {
                    tuple<int, int> coords = matched.at(i);
                    puyoGrid[get<0>(coords)][get<1>(coords)]->setTransparency(0.5f);
                }
            } else if (bouncingNum % 8 == 4) {
                for (int i = 0; i < matched.size(); i++) {
                    tuple<int, int> coords = matched.at(i);
                    puyoGrid[get<0>(coords)][get<1>(coords)]->setTransparency(0.0f);
                }
            } else if (bouncingNum % 8 == 6) {
                for (int i = 0; i < matched.size(); i++) {
                    tuple<int, int> coords = matched.at(i);
                    puyoGrid[get<0>(coords)][get<1>(coords)]->setTransparency(0.5f);
                }
            }
            bouncingNum++;
        }

        void dropTimer() {
            if (dropNum == -1) return;
            bool didGrav;

            if (dropNum == dropSpeedLUT[dropIndex]) {
                tuple<int, int> pop = applyGravity();
                if (get<0>(pop) != -1) {
                    dropIndex++;
                    prevDrop = pop;
                } else if (get<0>(prevDrop) != -1) {
                    dropNum = -1;
                    dropIndex = 0;
                    popPuyo(get<0>(prevDrop), get<1>(prevDrop));
                    prevDrop = make_tuple(-1, -1);
                }
            }
            dropNum++;
        }

        void startBouncingTimer() {
            bouncingNum = 0;
        }

        void startDropTimer() {
            dropNum = 0;
        }

        bool timerRunning() {
            return bouncingNum == -1 || dropNum == -1;
        }

        tuple<int, int> applyGravity() {
            int x = -1;
            int y = -1;

            for (int i = 0; i < xSize; i++) {
                for (int j = 1; j < ySize; j++) {
                    if (puyoGrid[i][j] != nullptr && puyoGrid[i][j - 1] == nullptr) {
                        puyoGrid[i][j]->move(0, -1);
                        puyoGrid[i][j - 1] = puyoGrid[i][j];
                        puyoGrid[i][j] = nullptr;
                        x = i;
                        y = j - 1;
                    }
                }
            }

            return make_tuple(x, y);
        }
};

void keyCallback(GLFWwindow* window);

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int gridX = 6; 
int gridY = 12;
Grid* grid;
int frameCount;

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
    glfwSwapInterval(1);
    double theta = 0;

    grid = new Grid(gridX, gridY);

    //grid x, y, color
    //this is gtr
    grid->addPuyo(0, 0, red);
    grid->addPuyo(0, 1, green);
    grid->addPuyo(0, 2, green);
    grid->addPuyo(1, 0, red);
    grid->addPuyo(1, 1, green);
    grid->addPuyo(1, 2, red);
    grid->addPuyo(2, 0, blue);
    grid->addPuyo(2, 1, red);
    grid->addPuyo(2, 2, blue);
    grid->addPuyo(3, 0, yellow);
    grid->addPuyo(3, 1, blue);
    grid->addPuyo(3, 2, red);
    grid->addPuyo(4, 0, yellow);
    grid->addPuyo(4, 1, blue);
    grid->addPuyo(4, 2, red);
    grid->addPuyo(5, 0, red);
    grid->addPuyo(5, 1, red);
    grid->addPuyo(5, 2, yellow);
    grid->addPuyo(5, 3, yellow);    

    grid->setCurrPuyo(grid->addPuyo(3, 10, green));

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTime = glfwGetTime();
    frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        //fps counter
        double currentTime = glfwGetTime();
        frameCount++;
            
        //printf("%f fps\n", double(frameCount / (currentTime - lastTime)));

        //set viewport size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        //background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //draw...
        grid->draw();

        //timers...
        grid->bouncingTimer();
        grid->dropTimer();

        //check events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        keyCallback(window);
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    ::exit(EXIT_SUCCESS);
}

int dasFrame = 0;
bool dasActive = false;
bool dasCharge = false;
int prevLeft;
int prevRight;
int prevDown;

void horizontalInput(int input, int prev, int x) {
    bool moved = true;
    if (input) {
        if (!prev) {
            moved = grid->move(x, 0);
            dasActive = false;
            dasCharge = true;
        } else {
            if (dasFrame == 11) {
                dasActive = true;
                dasCharge = false;
                dasFrame = 0;
            }

            if (dasActive && dasFrame == 2) {
                moved = grid->move(x, 0);
                dasFrame = 0;
            }
        }
    } else if (prev) {
        dasFrame = 0;
        dasActive = false;
        dasCharge = false;
    }

    if (!moved) {
        dasFrame = 0;
        dasActive = false;
    }
}

void keyCallback(GLFWwindow* window) {
    if (grid->getCurrPuyo() == nullptr) return;

    int left = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);

    // if (glfwGetKey(window, GLFW_KEY_UP)) {
    //     grid->move(0, 1);
    // }

    if (down && !dasCharge) {
        if (!prevDown && !(left || right)) {
           grid->move(0, -1);
        } else {
            //both no
            //das n
            //left yes
            //none yes
            if (dasFrame == 2 && !dasCharge) {
                grid->move(0, -1);
                dasFrame = 0;
            }
        }
    } else if (prevDown && !dasCharge && !(left || right)) {
        dasFrame = 0;
    }

    if (left && prevLeft || right && prevRight || down && prevDown) {
        dasFrame++;
    }

    horizontalInput(left, prevLeft, -1);
    horizontalInput(right, prevRight, 1);

    prevLeft = left;
    prevRight = right;
    prevDown = down;
}