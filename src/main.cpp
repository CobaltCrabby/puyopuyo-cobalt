#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include "grid.cpp"

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>

using namespace std;
static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

int gridX = 6; 
int gridY = 12;
Grid* grid;
int frameCount;

void keyCallback(GLFWwindow* window);

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

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
    grid->addPuyo(0, 0, red);
    grid->addPuyo(0, 1, green);
    grid->addPuyo(0, 2, green);
    grid->addPuyo(1, 0, red);
    grid->addPuyo(1, 1, green);
    grid->addPuyo(1, 2, red);
    grid->addPuyo(1, 3, red);
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

    grid->setCurrPuyo(grid->addPuyo(2, 10, green), grid->addPuyo(2, 11, red));

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
int prevA;
int prevS;
int lastA = 0;
int lastS = 0;
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
    if (grid->getCurrPuyo(0) == nullptr && grid->getCurrPuyo(1) == nullptr) return;

    int left = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);

    int a = glfwGetKey(window, GLFW_KEY_A);
    int s = glfwGetKey(window, GLFW_KEY_S);

    if (a && !prevA) {
        if (lastA <= 10 && grid->canOneColumn()) {
            grid->oneColumnRotate();
        } else {
            grid->rotatePuyo(1);
        }
        lastA = 0;
    } else {
        lastA++;
    }

    if (s && !prevS) {
        if (lastS <= 10 && grid->canOneColumn()) {
            grid->oneColumnRotate();
        } else {
            grid->rotatePuyo(-1);
        }
        lastS = 0; 
    } else {
        lastS++;
    }

    // if (glfwGetKey(window, GLFW_KEY_UP)) {
    //     grid->move(0, 1);
    // }

    if (down && !dasCharge) {
        if (!prevDown && !(left || right)) {
           grid->move(0, -1);
        } else {
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
    prevA = a;
    prevS = s;
}