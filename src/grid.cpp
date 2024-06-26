#include "grid.h"
#include <tuple>

#define _USE_MATH_DEFINES
#include <cmath>

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

Grid::Grid (int x, int y) {
    xSize = x;
    ySize = y;

    puyoGrid = new Puyo**[x];
    gridLines = new Line*[x + y + 2];
    queue = new Queue();
    chainNum = new Number(0.05f, -0.7f, 0);

    queue->initQueue();
    chainNum->drawInit();

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

Grid::~Grid() {
    for (int i = 0; i < xSize + ySize + 2; i++) {
        delete gridLines[i];
    }

    for (int i = 0; i < xSize; i++) {
        for (int j = 0; j < ySize; j++) {
            delete puyoGrid[i][j];
        }
    }
}

void Grid::draw() {
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

    queue->draw();
    chainNum->draw();
}

void Grid::setCurrPuyo(Puyo* puyo1, Puyo* puyo2) {
    currPuyo[0] = puyo1;
    currPuyo[1] = puyo2;
}

Puyo* Grid::getCurrPuyo(int i) {
    return currPuyo[i];
}

Puyo* Grid::addPuyo(int x, int y, enum color c) {
    puyoGrid[x][y] = new Puyo(x, y, c, xSize, ySize);
    return puyoGrid[x][y];
}

bool Grid::move(int xInc, int yInc) {
    if (timerRunning()) return false;
    
    int px[2];
    int py[2];
    int nx[2];
    int ny[2];

    for (int i = 0; i < 2; i++) {
        px[i] = currPuyo[i]->getX();
        py[i] = currPuyo[i]->getY();
        nx[i] = px[i] + xInc;
        ny[i] = py[i] + yInc;
    }

    for (int i = 0; i < 2; i++) {
        if ((nx[i] < 0 || nx[i] >= xSize || ny[i] < 0 || ny[i] >= ySize || 
            (puyoGrid[nx[i]][ny[i]] != currPuyo[(i + 1) % 2] && puyoGrid[nx[i]][ny[i]] != nullptr))) {
            if (yInc == -1) {
                startDropTimer();
                return true;
            } else {
                return false;    
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        currPuyo[i]->move(xInc, yInc);
        puyoGrid[px[i]][py[i]] = nullptr;
    }

    //need to be seperated so when replacing with nullptr it doesnt delete a currPuyo
    for (int i = 0; i < 2; i++) {
        puyoGrid[nx[i]][ny[i]] = currPuyo[i];
    }

    for (int i = 0; i < 2; i++) {
        if (yInc == -1 && (ny[i] == 0 || puyoGrid[nx[i]][ny[i] - 1] != nullptr && puyoGrid[nx[i]][ny[i] - 1] != currPuyo[(i + 1) % 2])) {
            startDropTimer();
            break;
        }
    }

    return true;
}

bool Grid::popPuyo(vector<tuple<int, int>> pops) {
    vector<tuple<int, int>> match;
    for (int i = 0; i < pops.size(); i++) {
        int _x = get<0>(pops.at(i));
        int _y = get<1>(pops.at(i));
        vector<tuple<int, int>> temp; 
        temp = checkPops(_x, _y, temp, puyoGrid[_x][_y]->getColor());

        //concatenate vectors
        if (temp.size() >= 4) {
            match.insert(match.end(), temp.begin(), temp.end());
        }
    }

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
        return true;
    }
    return false;
}

vector<tuple<int, int>> Grid::checkPops(int x, int y, vector<tuple<int, int>> num, enum color c) {
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

void Grid::deletePuyo(int _x, int _y) {
    Puyo* temp = puyoGrid[_x][_y];
    puyoGrid[_x][_y] = nullptr;
    delete temp;
    currPuyo[0] = nullptr;
    currPuyo[1] = nullptr;
}

void Grid::bouncingTimer() {
    if (bouncingNum == -1) return;
    if (bouncingNum == 64) {
        for (int i = 0; i < matched.size(); i++) {
            tuple<int, int> coords = matched.at(i);
            deletePuyo(get<0>(coords), get<1>(coords));
        }
        bouncingNum = -1;
        matched.clear();
        startDropTimer();
        chain++;
        chainNum->updateValue(chain);
        return;
    } 

    float transparency = -1.0f;

    switch(bouncingNum % 8) {
        case 0:
            transparency = 1.0f;
            break;
        case 2:
            transparency = 0.5f;
            break;
        case 4:
            transparency = 0.0f;
            break;
        case 6:
            transparency = 0.5f;
            break;
    }

    if (transparency != -1.0f) {
        for (int i = 0; i < matched.size(); i++) {
            tuple<int, int> coords = matched.at(i);
            puyoGrid[get<0>(coords)][get<1>(coords)]->setTransparency(transparency);
        }
    }
    bouncingNum++;
}

void Grid::dropTimer() {
    if (dropNum == -1) return;

    if (dropNum == dropSpeedLUT[dropIndex]) {
        vector<tuple<int, int>> dropped = applyGravity();
        if (dropped.size() != 0) {
            dropIndex++;
            updatePrevDrop(dropped);
        } else if (prevDrop.size() != 0) {
            dropNum = -1;
            dropIndex = 0;

            if (currPuyo[0] != nullptr) {
                prevDrop.push_back(make_tuple(currPuyo[0]->getX(), currPuyo[0]->getY()));
                prevDrop.push_back(make_tuple(currPuyo[1]->getX(), currPuyo[1]->getY()));
            }

            bool pop = popPuyo(prevDrop);

            prevDrop.clear();

            if (!pop) newCurrPuyo();

            return;
        } else if (prevDrop.size() == 0 && dropped.size() == 0) {
            if (currPuyo[0] == nullptr) {
                newCurrPuyo();
            } else {
                bool pop = false;
                vector<tuple<int, int>> pops{
                    make_tuple(currPuyo[0]->getX(), currPuyo[0]->getY()),
                    make_tuple(currPuyo[1]->getX(), currPuyo[1]->getY())
                };
                pop = popPuyo(pops);
                currPuyo[0] = nullptr;
                currPuyo[1] = nullptr;

                if (!pop) {
                    newCurrPuyo();
                }
            }

            dropNum = -1;
            dropIndex = 0;
            prevDrop.clear();
            return;
        }
    }
    dropNum++;
}

void Grid::startBouncingTimer() {
    bouncingNum = 0;
}

void Grid::startDropTimer() {
    dropNum = 0;
}

bool Grid::timerRunning() {
    return bouncingNum != -1 || dropNum != -1;
}

vector<tuple<int, int>> Grid::applyGravity() {
    vector<tuple<int, int>> dropping;
    
    for (int i = 0; i < xSize; i++) {
        for (int j = 1; j < ySize; j++) {
            if (puyoGrid[i][j] != nullptr && puyoGrid[i][j - 1] == nullptr) {
                puyoGrid[i][j]->move(0, -1);
                puyoGrid[i][j - 1] = puyoGrid[i][j];
                puyoGrid[i][j] = nullptr;
                dropping.push_back(make_tuple(i, j - 1));
            }
        }
    }

    return dropping;
}

void Grid::rotatePuyo(int direction) {
    if (timerRunning()) return;

    float x_ = cos((currPuyoRotation + direction) * M_PI / 2) - cos((currPuyoRotation) * M_PI / 2);
    float y_ = sin((currPuyoRotation + direction) * M_PI / 2) - sin((currPuyoRotation) * M_PI / 2);

    int x = currPuyo[1]->getX();
    int y = currPuyo[1]->getY();
    int tx = x_ + x;
    int ty = y_ + y;
    
    int offset = 0;

    int zx = currPuyo[0]->getX();
    int zy = currPuyo[0]->getY();

    if (tx < 0) {
        offset = 1;
    } else if (tx >= xSize) {
        offset = -1;
    } else if (puyoGrid[tx][ty] != nullptr) {
        return;
    }

    if (offset != 0 && ((puyoGrid[tx + offset][ty] != nullptr && puyoGrid[tx + offset][ty] != currPuyo[0]) || 
    (puyoGrid[zx + offset][zy] != nullptr && puyoGrid[zx + offset][ty] != currPuyo[1]))) {
        return;
    }

    currPuyo[0]->move(offset, 0);
    puyoGrid[zx][zy] = nullptr;
    puyoGrid[zx + offset][zy] = currPuyo[0];
    currPuyo[1]->move(x_ + offset, y_);
    puyoGrid[x][y] = nullptr;
    puyoGrid[tx + offset][ty] = currPuyo[1];
    currPuyoRotation += direction;
}

void Grid::newCurrPuyo() {
    srand((unsigned) time(NULL));
    tuple<UIPuyo*, UIPuyo*> newpair = queue->newPuyo();
    setCurrPuyo(
        addPuyo(2, 10, get<1>(newpair)->getColor()), 
        addPuyo(2, 11, get<0>(newpair)->getColor())
    );
     
    chain = 0;
    chainNum->updateValue(chain);
    currPuyoRotation = 1;
}

void Grid::updatePrevDrop(vector<tuple<int, int>> drop) {
    for (int i = 0; i < drop.size(); i++) {
        bool repeat = false;
        int _x = get<0>(drop.at(i));
        int _y = get<1>(drop.at(i));

        for (int j = 0; j < prevDrop.size(); j++) {
            int __x = get<0>(prevDrop.at(j));
            int __y = get<1>(prevDrop.at(j));
            if (_x == __x && _y + 1 == __y) {
                repeat = true;
                prevDrop.at(j) = drop.at(i);
            }
        }

        if (!repeat) {
            prevDrop.push_back(drop.at(i));
        }
    }
}

bool Grid::canOneColumn() {
    if (currPuyoRotation % 2 == 0) return false;
    bool rotate = true;

    for (int i = 0; i < 2; i++) {
        int x = currPuyo[i]->getX();
        int y = currPuyo[i]->getY();

        rotate = rotate && (x <= 0 || puyoGrid[x - 1][y] != nullptr) && (x >= xSize -1 || puyoGrid[x + 1][y] != nullptr);
    }
    return rotate;
}