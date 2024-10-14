#include "board.h"

Board::Board(int numShips) : numShips(numShips) {
    grid = QVector<QVector<char>>(GRID_SIZE, QVector<char>(GRID_SIZE, '~'));
}

bool Board::isValidPosition(int row, int col, bool isVertical, int shipLength) {
    if (isVertical) {
        if (row + shipLength > GRID_SIZE) return false;
        for (int i = 0; i < shipLength; i++) {
            if (grid[row + i][col] != '~') return false;
        }
    } else {
        if (col + shipLength > GRID_SIZE) return false;
        for (int i = 0; i < shipLength; i++) {
            if (grid[row][col + i] != '~') return false;
        }
    }
    return true;
}

void Board::placeShip(int row, int col, bool isVertical, int shipLength, char symbol) {
    Ship ship = {row, col, isVertical, shipLength};
    if (isVertical) {
        for (int i = 0; i < shipLength; i++) {
            grid[row + i][col] = symbol;
            ship.positions.append(qMakePair(row + i, col));
        }
    } else {
        for (int i = 0; i < shipLength; i++) {
            grid[row][col + i] = symbol;
            ship.positions.append(qMakePair(row, col + i));
        }
    }
    ships.append(ship);
}

bool Board::attack(int row, int col) {
    if (grid[row][col] == 'S') {
        grid[row][col] = 'X';
        return true;
    } else if (grid[row][col] == '~') {
        grid[row][col] = 'O';
    }
    return false;
}

bool Board::hasShipsRemaining() {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            if (cell == 'S') return true;
        }
    }
    return false;
}