#ifndef BATTLESHIPGAME_H
#define BATTLESHIPGAME_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QIcon>
#include "board.h"

class BattleshipGame : public QMainWindow {
    Q_OBJECT
public:
    BattleshipGame(QWidget *parent = nullptr);

private:
    int numShips;
    Board userBoard;
    Board botBoard;
    int currentShip;
    bool isPlacingShips;
    bool gameOver;
    QString message;
    QString difficulty;

    QWidget *centralWidget;
    QGridLayout *userGridLayout;
    QGridLayout *botGridLayout;
    QLabel *messageLabel;
    QComboBox *difficultyComboBox;
    QRadioButton *horizontalRadio;
    QRadioButton *verticalRadio;
    QButtonGroup *orientationGroup;
    QPushButton *restartButton;
    QPushButton *exitButton;

    QIcon shipIcon;
    QIcon hitIcon;
    QIcon missIcon;
    QIcon oceanIcon;
    bool huntingMode;
    QVector<QPair<int, int>> possibleMoves;
    QVector<QPair<int, int>> lastHits;

    void botSmartAttack();
    void addAdjacentPositions(int row, int col);
    bool isPositionInPossibleMoves(int row, int col);

    QVector<QPair<int, int>> botTargets;
    QVector<QPair<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    int currentDirection;

    void createIcons();
    void setupUI();
    void setupBoard(QGridLayout *gridLayout, bool isBotBoard);
    void userPlaceShip(int row, int col, QPushButton *button);
    void userAttack(int row, int col, QPushButton *button);
    void botAttack();
    void botEasyAttack();

    void botHardAttack();
    QPushButton *findButtonAt(int row, int col, QGridLayout *layout);
    void resetGame();
    void botPlaceShips();
    bool isValidCell(int row, int col);

private slots:
    void onDifficultyChanged(const QString &difficulty);
    void onRestartClicked();
    void onExitClicked();
};

#endif // BATTLESHIPGAME_H