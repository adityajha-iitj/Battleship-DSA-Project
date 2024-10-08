// battleshipgame.cpp
#include "battleshipgame.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QPainter>
#include <QCoreApplication>
#include <cstdlib>
#include <ctime>

BattleshipGame::BattleshipGame(QWidget *parent)
    : QMainWindow(parent),
    numShips(4),
    userBoard(numShips),
    botBoard(numShips),
    currentShip(0),
    isPlacingShips(true),
    gameOver(false),
    difficulty("Medium"),
    huntingMode(false)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    createIcons();
    setupUI();
    botPlaceShips();
}

void BattleshipGame::createIcons() {
    // Create oceanIcon
    QPixmap oceanPixmap(50, 50);
    oceanPixmap.fill(Qt::blue);
    oceanIcon = QIcon(oceanPixmap);

    // Create shipIcon
    QPixmap shipPixmap(50, 50);
    shipPixmap.fill(Qt::green);
    shipIcon = QIcon(shipPixmap);

    // Create hitIcon
    QPixmap hitPixmap(50, 50);
    hitPixmap.fill(Qt::red);
    QPainter hitPainter(&hitPixmap);
    hitPainter.setPen(QPen(Qt::white, 5));
    hitPainter.drawLine(0, 0, 50, 50);
    hitPainter.drawLine(50, 0, 0, 50);
    hitPainter.end();
    hitIcon = QIcon(hitPixmap);

    // Create missIcon
    QPixmap missPixmap(50, 50);
    missPixmap.fill(Qt::gray);
    missIcon = QIcon(missPixmap);
}

void BattleshipGame::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *boardsLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;

    // Orientation Selection
    QGroupBox *orientationBox = new QGroupBox("Ship Orientation");
    QHBoxLayout *orientationLayout = new QHBoxLayout;
    horizontalRadio = new QRadioButton("Horizontal");
    verticalRadio = new QRadioButton("Vertical");
    horizontalRadio->setChecked(true);
    orientationGroup = new QButtonGroup(this);
    orientationGroup->addButton(horizontalRadio);
    orientationGroup->addButton(verticalRadio);
    orientationLayout->addWidget(horizontalRadio);
    orientationLayout->addWidget(verticalRadio);
    orientationBox->setLayout(orientationLayout);

    topLayout->addWidget(orientationBox);

    // User Board
    userGridLayout = new QGridLayout;
    QLabel *userBoardLabel = new QLabel("<b>Your Board</b>");
    userBoardLabel->setAlignment(Qt::AlignCenter);
    setupBoard(userGridLayout, false);

    // Bot Board
    botGridLayout = new QGridLayout;
    QLabel *botBoardLabel = new QLabel("<b>Bot's Board</b>");
    botBoardLabel->setAlignment(Qt::AlignCenter);
    setupBoard(botGridLayout, true);

    // Message Label
    messageLabel = new QLabel("Place your ships on your board.");
    messageLabel->setAlignment(Qt::AlignCenter);

    // Restart and Exit Buttons
    restartButton = new QPushButton("Restart Game");
    exitButton = new QPushButton("Exit");
    connect(restartButton, &QPushButton::clicked, this, &BattleshipGame::resetGame);
    connect(exitButton, &QPushButton::clicked, this, &BattleshipGame::close);

    bottomLayout->addWidget(restartButton);
    bottomLayout->addWidget(exitButton);

    // Layout setup
    QVBoxLayout *userBoardLayout = new QVBoxLayout;
    userBoardLayout->addWidget(userBoardLabel);
    userBoardLayout->addLayout(userGridLayout);

    QVBoxLayout *botBoardLayout = new QVBoxLayout;
    botBoardLayout->addWidget(botBoardLabel);
    botBoardLayout->addLayout(botGridLayout);

    boardsLayout->addLayout(userBoardLayout);
    boardsLayout->addSpacing(50);
    boardsLayout->addLayout(botBoardLayout);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(boardsLayout);
    mainLayout->addWidget(messageLabel);
    mainLayout->addLayout(bottomLayout);

    centralWidget->setLayout(mainLayout);
    setWindowTitle("Battleship Game");
    resize(900, 700);
}

void BattleshipGame::setupBoard(QGridLayout *gridLayout, bool isBotBoard) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            QPushButton *button = new QPushButton;
            button->setFixedSize(50, 50);
            button->setIcon(oceanIcon);
            button->setIconSize(QSize(48, 48));
            if (isBotBoard) {
                connect(button, &QPushButton::clicked, [=]() {
                    if (!gameOver && !isPlacingShips) {
                        userAttack(row, col, button);
                    }
                });
            } else {
                connect(button, &QPushButton::clicked, [=]() {
                    if (isPlacingShips) {
                        userPlaceShip(row, col);
                    }
                });
            }
            gridLayout->addWidget(button, row, col);
        }
    }
}

void BattleshipGame::botPlaceShips() {
    for (int i = 0; i < botBoard.numShips; i++) {
        bool placed = false;
        int shipSize = MIN_SHIP_SIZE + rand() % (MAX_SHIP_SIZE - MIN_SHIP_SIZE + 1);
        while (!placed) {
            int row = rand() % GRID_SIZE;
            int col = rand() % GRID_SIZE;
            bool isVertical = rand() % 2;
            if (botBoard.isValidPosition(row, col, isVertical, shipSize)) {
                botBoard.placeShip(row, col, isVertical, shipSize);
                placed = true;
            }
        }
    }
}

void BattleshipGame::userPlaceShip(int row, int col) {
    bool isVertical = verticalRadio->isChecked();
    int shipSize = MIN_SHIP_SIZE + currentShip;
    if (userBoard.isValidPosition(row, col, isVertical, shipSize)) {
        userBoard.placeShip(row, col, isVertical, shipSize);
        currentShip++;
        updateUserBoard();
        if (currentShip == userBoard.numShips) {
            isPlacingShips = false;
            message = "All ships placed. Start attacking!";
            messageLabel->setText(message);
        } else {
            message = QString("Ship placed. Place your next ship (%1 cells).").arg(MIN_SHIP_SIZE + currentShip);
            messageLabel->setText(message);
        }
    } else {
        message = "Invalid position. Try again.";
        messageLabel->setText(message);
    }
}

void BattleshipGame::userAttack(int row, int col, QPushButton *button) {
    if (botBoard.grid[row][col] == 'X' || botBoard.grid[row][col] == 'O') {
        message = "You already attacked this position.";
        messageLabel->setText(message);
        return;
    }

    if (botBoard.attack(row, col)) {
        button->setIcon(hitIcon);
        message = "Hit!";
    } else {
        button->setIcon(missIcon);
        message = "Miss!";
    }
    messageLabel->setText(message);

    if (!botBoard.hasShipsRemaining()) {
        gameOver = true;
        QMessageBox::information(this, "Game Over", "Congratulations! You won!");
        return;
    }

    QCoreApplication::processEvents(); // Allow UI to update before bot's turn
    botAttack();
}

void BattleshipGame::botAttack() {
    // Since only Medium difficulty exists, always use smart attack
    botSmartAttack();

    if (!userBoard.hasShipsRemaining()) {
        gameOver = true;
        QMessageBox::information(this, "Game Over", "The bot won. Better luck next time!");
    }
}

void BattleshipGame::botSmartAttack() {
    int botRow = -1, botCol = -1;
    bool hit = false;

    // Select target position
    if (huntingMode && !possibleMoves.isEmpty()) {
        // Continue hunting in the vicinity of the last hit
        QPair<int, int> target = possibleMoves.takeFirst();
        botRow = target.first;
        botCol = target.second;
    } else {
        // Search for a new target
        huntingMode = false; // Ensure hunting mode is off when starting a new search
        possibleMoves.clear(); // Clear any leftover moves
        do {
            botRow = rand() % GRID_SIZE;
            botCol = rand() % GRID_SIZE;
        } while (userBoard.grid[botRow][botCol] == 'X' || userBoard.grid[botRow][botCol] == 'O');
    }

    hit = userBoard.attack(botRow, botCol);
    QPushButton button = qobject_cast<QPushButton>(userGridLayout->itemAtPosition(botRow, botCol)->widget());

    if (hit) {
        button->setIcon(hitIcon);
        message += QString("\nBot hits at %1%2!").arg(QChar('A' + botCol)).arg(botRow + 1);
        addAdjacentPositions(botRow, botCol);
        huntingMode = true;
    } else {
        button->setIcon(missIcon);
        message += QString("\nBot misses at %1%2.").arg(QChar('A' + botCol)).arg(botRow + 1);
    }

    messageLabel->setText(message);
    message.clear(); // Clear the message after updating
}

bool BattleshipGame::isPositionInPossibleMoves(int row, int col) {
    for (const auto& pos : possibleMoves) {
        if (pos.first == row && pos.second == col) {
            return true;
        }
    }
    return false;
}

void BattleshipGame::addAdjacentPositions(int row, int col) {
    QVector<QPair<int, int>> directions = {
        qMakePair(-1, 0), // Up
        qMakePair(1, 0),  // Down
        qMakePair(0, -1), // Left
        qMakePair(0, 1)   // Right
    };

    for (const auto& dir : directions) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (newRow >= 0 && newRow < GRID_SIZE && newCol >= 0 && newCol < GRID_SIZE) {
            // Check if the position has not been attacked and is not already in possibleMoves
            if (userBoard.grid[newRow][newCol] != 'X' && userBoard.grid[newRow][newCol] != 'O' &&
                !isPositionInPossibleMoves(newRow, newCol)) {
                possibleMoves.append(qMakePair(newRow, newCol));
            }
        }
    }
}

void BattleshipGame::updateUserBoard() {
    for (const auto& ship : userBoard.ships) {
        for (const auto& pos : ship.positions) {
            QPushButton button = qobject_cast<QPushButton>(userGridLayout->itemAtPosition(pos.first, pos.second)->widget());
            button->setIcon(shipIcon);
        }
    }
}

void BattleshipGame::resetGame() {
    userBoard = Board(numShips);
    botBoard = Board(numShips);
    currentShip = 0;
    isPlacingShips = true;
    gameOver = false;
    message = "Place your ships on your board.";
    messageLabel->setText(message);
    huntingMode = false;
    possibleMoves.clear();

    // Clear user board
    for (int i = 0; i < userGridLayout->count(); ++i) {
        QPushButton button = qobject_cast<QPushButton>(userGridLayout->itemAt(i)->widget());
        button->setIcon(oceanIcon);
        button->setEnabled(true);
    }

    // Clear bot board
    for (int i = 0; i < botGridLayout->count(); ++i) {
        QPushButton button = qobject_cast<QPushButton>(botGridLayout->itemAt(i)->widget());
        button->setIcon(oceanIcon);
        button->setEnabled(true);
    }

    botPlaceShips();
}