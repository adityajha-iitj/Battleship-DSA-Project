// battleshipgame.h
#ifndef BATTLESHIPGAME_H
#define BATTLESHIPGAME_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QIcon>
#include "board.h"

class BattleshipGame : public QMainWindow {
    Q_OBJECT
public:
    explicit BattleshipGame(QWidget *parent = nullptr);

private:
    // Game Logic
    int numShips;
    Board userBoard;
    Board botBoard;
    int currentShip;
    bool isPlacingShips;
    bool gameOver;
    QString message;
    QString difficulty;

    // UI Components
    QWidget *centralWidget;
    QGridLayout *userGridLayout;
    QGridLayout *botGridLayout;
    QLabel *messageLabel;
    QRadioButton *horizontalRadio;
    QRadioButton *verticalRadio;
    QButtonGroup *orientationGroup;
    QPushButton *restartButton;
    QPushButton *exitButton;

    // Icons
    QIcon shipIcon;
    QIcon hitIcon;
    QIcon missIcon;
    QIcon oceanIcon;

    // Bot AI Variables
    QVector<QPair<int, int>> botTargets;
    QVector<QPair<int, int>> possibleMoves;
    bool huntingMode;

    // Methods
    void createIcons();
    void setupUI();
    void setupBoard(QGridLayout *gridLayout, bool isBotBoard);
    void botPlaceShips();
    void userPlaceShip(int row, int col);
    void userAttack(int row, int col, QPushButton *button);
    void botAttack();
    void botSmartAttack();
    bool isPositionInPossibleMoves(int row, int col);
    void addAdjacentPositions(int row, int col);
    void updateUserBoard();

private slots:
    void resetGame();
};

#endif // BATTLESHIPGAME_H