#pragma once
#include "uipuyo.h"
#include "color.h"
#include <tuple>
#include <queue>

class Queue {
    queue<tuple<UIPuyo*, UIPuyo*>> qu;

    public:
        Queue();
        void initQueue();
        tuple<UIPuyo*, UIPuyo*> newPuyo();
        void draw();
};