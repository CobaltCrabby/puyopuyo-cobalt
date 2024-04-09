#pragma once

#include "line.h"
#include "puyo.h"
#include "uipuyo.h"
#include "queue.h"
#include <vector>
#include <tuple>

class Grid {
    int xSize, ySize;
    Line** gridLines;
    Puyo*** puyoGrid;
    Puyo* currPuyo[2];
    Queue* queue;

    int bouncingNum = -1;
    int dropNum = -1;
    int dropIndex = 0;
    
    int currPuyoRotation = 1;

    int chain = 0;

    vector<tuple<int, int>> prevDrop;
    vector<tuple<int, int>> matched;

    public:
        Grid (int x, int y);

        ~Grid();

        void draw();

        void setCurrPuyo(Puyo* puyo1, Puyo* puyo2);

        Puyo* getCurrPuyo(int i);

        Puyo* addPuyo(int x, int y, enum color c);

        bool move(int xInc, int yInc);

        bool popPuyo(vector<tuple<int, int>> pops);

        vector<tuple<int, int>> checkPops(int x, int y, vector<tuple<int, int>> num, enum color c);

        void deletePuyo(int _x, int _y);

        void bouncingTimer();

        void dropTimer();

        void startBouncingTimer();

        void startDropTimer();

        bool timerRunning();

        vector<tuple<int, int>> applyGravity();

        void rotatePuyo(int direction);

        void newCurrPuyo();

        void updatePrevDrop(vector<tuple<int, int>> drop);

        bool canOneColumn();
};