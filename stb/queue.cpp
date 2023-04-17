#include "line.h"
#include "puyo.h"
#include <queue>

using namespace std;

class Queue {
    public:
        queue<Puyo*[2]> queue;

        void initQueue() {
            for (int i = 0; i < 2; i++) {
                //need to make puyo that is not on a grid
            }
        }

        Puyo* newPuyo() {

        }
};

//need to fix makefile and make all header files