#pragma once
#include "puyo.h"
#include "color.h"
#include <queue>

class Queue {
    queue<Puyo**> q;

    public:
        void initQueue();
        Puyo** newPuyo();
        void draw();
};