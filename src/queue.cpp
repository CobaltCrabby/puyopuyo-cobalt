#include <queue>
#include "queue.h"  

using namespace std;

void Queue::initQueue() {
    for (int i = 0; i < 2; i++) {
        Puyo* array[2];
        for (int j = 0; j < 2; j++) {
            srand((unsigned) time(NULL));
            srand(random());
            array[j] = new Puyo(0, 0, static_cast<color>((int) random() % 4), 0, 0);
            array[j]->drawInit(0.7f, 0.8f - 0.12f * j - 0.2f * (i - 1), 0.12f);
        }
        //seg fault here lol!!!!! array pointer jank with queue?? idk
        //q.push(array);
    }
}

Puyo** Queue::newPuyo(){
    Puyo** puyo = q.front();
    q.pop();
    return puyo;
}

void Queue::draw() {
    Puyo** first = q.front();
    Puyo** last = q.back();

    for (int i = 0; i < 2; i++) {
        first[i]->draw();
        last[i]->draw();
    }
}