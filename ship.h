// ship.h
#ifndef SHIP_H
#define SHIP_H

#include <QVector>
#include <QPair>

struct Ship {
    int row;
    int col;
    bool isVertical;
    int size;
    QVector<QPair<int, int>> positions;
};

#endif // SHIP_H
