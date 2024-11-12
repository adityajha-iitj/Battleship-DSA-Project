#ifndef BATTLESHIPGAME_H
#define BATTLESHIPGAME_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVector>
#include <QIcon>
#include "Board.h"

enum AttackDirection { UNKNOWN, HORIZONTAL, VERTICAL };

class BattleshipGame : public QMainWindow {
    Q_OBJECT

public:
    explicit BattleshipGame(QWidget *parent = nullptr);

private:
    enum GameMode { SinglePlayer, Multiplayer };
    GameMode currentMode;

    // Single-player variables
    int numShips;
    Board userBoard;
    Board botBoard;
    int currentShip;
    bool isPlacingShips;
    bool gameOver;
    QString message;
    QString difficulty;
    QVector<QPair<int, int>> botTargets;
    QPair<int, int> lastHit;
    QComboBox *shipLengthComboBox;
    bool huntingMode;
    QVector<QPair<int, int>> possibleMoves;
    QVector<QPair<int, int>> probabilityVector;
     AttackDirection attackDirection = UNKNOWN;

    // Multiplayer variables
    enum GamePhase { PlacingShips, Attacking };
    GamePhase gamePhase;
    Board player1Board;
    Board player2Board;
    int currentShipPlayer1;
    int currentShipPlayer2;
    int currentPlayer; // 1 or 2

    QWidget *centralWidget;
    QGridLayout *userGridLayout; // Used for single-player and multiplayer
    QGridLayout *botGridLayout;  // Used for single-player
    QGridLayout *player1GridLayout; // Used for multiplayer
    QGridLayout *player2GridLayout; // Used for multiplayer
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
    QIcon leftShipIcon;
    QIcon rightShipIcon;
    QIcon middleShipIcon;

    QIcon upperShipIcon;
    QIcon middleVerticalShipIcon;
    QIcon lowerShipIcon;

    // Private functions
    void createIcons();
    void setupUI();
    void setupBoard(QGridLayout *gridLayout, bool isBotBoard = false);
    void userPlaceShip(int row, int col, QPushButton *button);
    void userAttack(int row, int col, QPushButton *button);
    void botAttack();
    void botEasyAttack();
    void botMediumAttack();
    void botHardAttack();
    QPushButton *findButtonAt(int row, int col, QGridLayout *layout);
    void resetGame();
    void botPlaceShips();
    void showStartupDialog();
    void botAttackExpertMode();
    void initializeProbabilityVector(int row, int col);
    void identifyAndQueuePossibleTargets(int row, int col);
    void updateProbabilityVector(int row, int col);
    bool isShipSunk(int row, int col);
    void resetSearchForNextShip();
    void determineAttackDirection(int row, int col);
    void queueLineTargets(int row, int col);
        void botSmartAttack();
    void addAdjacentPositions(int row, int col);
    bool isPositionInPossibleMoves(int row, int col);
    QVector<QPair<int, int>> lastHits;

    bool isValidCell(int row, int col);
    QVector<QPair<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    int currentDirection;


    // Multiplayer functions
    void multiplayerPlaceShip(int row, int col, QPushButton *button);
    void multiplayerAttack(int row, int col, QPushButton *button);
    void switchTurns();

private slots:
    void onDifficultyChanged(const QString &difficulty);
    void onRestartClicked();
    void onExitClicked();
};

#endif // BATTLESHIPGAME_H
