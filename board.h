// board.h
#ifndef BOARD_H
#define BOARD_H

#include <QVector>
#include <QPair>
#include "ship.h"

const int GRID_SIZE = 7;
const int MIN_SHIP_SIZE = 3;
const int MAX_SHIP_SIZE = 5;

class Board {
public:
    QVector<QVector<char>> grid;
    QVector<Ship> ships;
    int numShips;

    Board(int numShips);

    bool isValidPosition(int row, int col, bool isVertical, int shipLength);
    void placeShip(int row, int col, bool isVertical, int shipLength, char symbol = 'S');
    bool attack(int row, int col);
    bool hasShipsRemaining();
};

#endif // BOARD_H
