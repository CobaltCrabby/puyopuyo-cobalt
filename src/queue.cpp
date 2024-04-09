#include "queue.h"  

using namespace std;

Queue::Queue() {
    //cout << "lol!" << endl;
}

void Queue::initQueue() {
    for (int i = 0; i < 2; i++) {
        srand((unsigned) time(NULL) + i);
        srand(random());
        UIPuyo* puyo1 = new UIPuyo(0.6f, 0.6f - 0.3f * i, 0.1f, static_cast<color>((int) random() % 4));
        srand(random());
        UIPuyo* puyo2 = new UIPuyo(0.6f, 0.5f - 0.3f * i, 0.1f, static_cast<color>((int) random() % 4));

        tuple<UIPuyo*, UIPuyo*> newpair = make_tuple(puyo1, puyo2);
        qu.push(newpair);
    }
}

tuple<UIPuyo*, UIPuyo*> Queue::newPuyo(){
    tuple<UIPuyo*, UIPuyo*> back = qu.back();
    tuple<UIPuyo*, UIPuyo*> pair = qu.front();
    qu.pop();

    get<0>(back)->move();
    get<1>(back)->move();

    srand((unsigned) time(NULL));
    srand(random());
    UIPuyo* puyo1 = new UIPuyo(0.6f, 0.6f - 0.3f, 0.1f, static_cast<color>((int) random() % 4));
    srand(random());
    UIPuyo* puyo2 = new UIPuyo(0.6f, 0.5f - 0.3f, 0.1f, static_cast<color>((int) random() % 4));
    qu.push(make_tuple(puyo1, puyo2));

    return pair;
}

void Queue::draw() {
    tuple<UIPuyo*, UIPuyo*> first = qu.front();
    tuple<UIPuyo*, UIPuyo*> last = qu.back();

    get<0>(first)->draw();
    get<0>(last)->draw();
    get<1>(first)->draw();
    get<1>(last)->draw();
}