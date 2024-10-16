#include "battleshipgame.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPixmap>
#include <QPainter>
#include <QCoreApplication>
#include <cstdlib>
#include <ctime>

BattleshipGame::BattleshipGame(QWidget *parent)
    : QMainWindow(parent),
    numShips(3), userBoard(numShips), botBoard(numShips), currentShip(0),
    isPlacingShips(true), gameOver(false), difficulty("Easy"),
    currentDirection(0)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    createIcons();
    setupUI();
    botPlaceShips();
}

void BattleshipGame::createIcons() {
    QPixmap oceanPixmap(50, 50);
    oceanPixmap.fill(Qt::blue);
    oceanIcon = QIcon(oceanPixmap);

    QPixmap shipPixmap(50, 50);
    shipPixmap.fill(Qt::green);
    shipIcon = QIcon(shipPixmap);

    QPixmap hitPixmap(50, 50);
    hitPixmap.fill(Qt::red);
    QPainter hitPainter(&hitPixmap);
    hitPainter.setPen(QPen(Qt::white, 5));
    hitPainter.drawLine(0, 0, 50, 50);
    hitPainter.drawLine(50, 0, 0, 50);
    hitPainter.end();
    hitIcon = QIcon(hitPixmap);

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

    QLabel *difficultyLabel = new QLabel("Select Difficulty:");
    difficultyComboBox = new QComboBox;
    difficultyComboBox->addItems({"Easy", "Medium", "Hard"});
    connect(difficultyComboBox, &QComboBox::currentTextChanged, this, &BattleshipGame::onDifficultyChanged);

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

    topLayout->addWidget(difficultyLabel);
    topLayout->addWidget(difficultyComboBox);
    topLayout->addStretch();
    topLayout->addWidget(orientationBox);

    userGridLayout = new QGridLayout;
    QLabel *userBoardLabel = new QLabel("<b>Your Board</b>");
    userBoardLabel->setAlignment(Qt::AlignCenter);
    setupBoard(userGridLayout, false);

    botGridLayout = new QGridLayout;
    QLabel *botBoardLabel = new QLabel("<b>Bot's Board</b>");
    botBoardLabel->setAlignment(Qt::AlignCenter);
    setupBoard(botGridLayout, true);

    messageLabel = new QLabel("Place your ships on your board.");
    messageLabel->setAlignment(Qt::AlignCenter);

    restartButton = new QPushButton("Restart Game");
    exitButton = new QPushButton("Exit");
    connect(restartButton, &QPushButton::clicked, this, &BattleshipGame::onRestartClicked);
    connect(exitButton, &QPushButton::clicked, this, &BattleshipGame::onExitClicked);

    bottomLayout->addWidget(restartButton);
    bottomLayout->addWidget(exitButton);

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
                connect(button, &QPushButton::clicked, this, [this, row, col, button]() {
                    if (!gameOver && !isPlacingShips) {
                        userAttack(row, col, button);
                    }
                });
            } else {
                connect(button, &QPushButton::clicked, this, [this, row, col, button]() {
                    if (isPlacingShips) {
                        userPlaceShip(row, col, button);
                    }
                });
            }
            gridLayout->addWidget(button, row, col);
        }
    }
}

void BattleshipGame::userPlaceShip(int row, int col, QPushButton *button) {
    int shipSize = MIN_SHIP_SIZE + currentShip;
    bool isVertical = verticalRadio->isChecked();

    if (userBoard.isValidPosition(row, col, isVertical, shipSize)) {
        userBoard.placeShip(row, col, isVertical, shipSize);
        Ship& placedShip = userBoard.ships.last();
        for (const auto& pos : placedShip.positions) {
            QPushButton* cellButton = findButtonAt(pos.first, pos.second, userGridLayout);
            if (cellButton) {
                cellButton->setIcon(shipIcon);
            }
        }
        currentShip++;
        if (currentShip >= numShips) {
            isPlacingShips = false;
            messageLabel->setText("All ships placed! Start attacking the bot's board.");
        } else {
            messageLabel->setText("Ship placed! Place your next ship.");
        }
    } else {
        messageLabel->setText("Invalid position! Try again.");
    }
}

void BattleshipGame::userAttack(int row, int col, QPushButton *button) {
    if (gameOver || isPlacingShips) return;

    if (botBoard.grid[row][col] == 'X' || botBoard.grid[row][col] == 'O') {
        messageLabel->setText("You've already attacked this position. Try another.");
        return;
    }

    if (botBoard.attack(row, col)) {
        button->setIcon(hitIcon);
        messageLabel->setText("You hit a ship!");
        if (!botBoard.hasShipsRemaining()) {
            gameOver = true;
            messageLabel->setText("You win! Restart the game.");
        } else {
            botAttack();
        }
    } else {
        button->setIcon(missIcon);
        messageLabel->setText("You missed!");
        botAttack();
    }
}

void BattleshipGame::botAttack() {
    if (gameOver) return;

    if (difficulty == "Easy") {
        botEasyAttack();
    } else if (difficulty == "Medium") {
        botSmartAttack();
    } else {
        botHardAttack();
    }
}

bool BattleshipGame::isValidCell(int row, int col) {
    return row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE;
}

void BattleshipGame::botEasyAttack() {
    int row, col;
    do {
        row = rand() % GRID_SIZE;
        col = rand() % GRID_SIZE;
    } while (userBoard.grid[row][col] == 'X' || userBoard.grid[row][col] == 'O');

    QPushButton *button = findButtonAt(row, col, userGridLayout);
    if (userBoard.attack(row, col)) {
        button->setIcon(hitIcon);
        messageLabel->setText("Bot hit your ship!");
        if (!userBoard.hasShipsRemaining()) {
            gameOver = true;
            messageLabel->setText("Bot wins! Game over.");
        }
    } else {
        button->setIcon(missIcon);
        messageLabel->setText("Bot missed!");
    }
}


QPushButton *BattleshipGame::findButtonAt(int row, int col, QGridLayout *layout) {
    QLayoutItem *item = layout->itemAtPosition(row, col);
    if (item) {
        return qobject_cast<QPushButton *>(item->widget());
    }
    return nullptr;
}

void BattleshipGame::resetGame() {
    currentShip = 0;
    isPlacingShips = true;
    gameOver = false;
    userBoard = Board(numShips);
    botBoard = Board(numShips);
    lastHits.clear();
    huntingMode = false;
    possibleMoves.clear();
    currentDirection = 0;

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            QPushButton *button = findButtonAt(row, col, userGridLayout);
            if (button) {
                button->setIcon(oceanIcon);
            }
            button = findButtonAt(row, col, botGridLayout);
            if (button) {
                button->setIcon(oceanIcon);
            }
        }
    }
    messageLabel->setText("Place your ships on your board.");
    botPlaceShips();
}

void BattleshipGame::botPlaceShips() {
    for (int i = 0; i < numShips; ++i) {
        int shipSize = MIN_SHIP_SIZE + i;
        bool placed = false;
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

void BattleshipGame::onDifficultyChanged(const QString &difficulty) {
    this->difficulty = difficulty;
}

void BattleshipGame::onRestartClicked() {
    resetGame();
}

void BattleshipGame::onExitClicked() {
    QCoreApplication::quit();
}

void BattleshipGame::botSmartAttack() {
    if (gameOver) return;

    int botRow = -1, botCol = -1;
    bool hit = false;

    if (huntingMode && !possibleMoves.isEmpty()) {
        QPair<int, int> target = possibleMoves.takeFirst();
        botRow = target.first;
        botCol = target.second;
    } else {
        huntingMode = false;
        possibleMoves.clear();
        do {
            botRow = rand() % GRID_SIZE;
            botCol = rand() % GRID_SIZE;
        } while (userBoard.grid[botRow][botCol] == 'X' || userBoard.grid[botRow][botCol] == 'O');
    }

    hit = userBoard.attack(botRow, botCol);
    QPushButton* button = findButtonAt(botRow, botCol, userGridLayout);

    if (hit) {
        button->setIcon(hitIcon);
        messageLabel->setText(QString("Bot hits at %1%2!").arg(QChar('A' + botCol)).arg(botRow + 1));
        addAdjacentPositions(botRow, botCol);
        huntingMode = true;
    } else {
        button->setIcon(missIcon);
        messageLabel->setText(QString("Bot misses at %1%2.").arg(QChar('A' + botCol)).arg(botRow + 1));
    }

    if (!userBoard.hasShipsRemaining()) {
        gameOver = true;
        messageLabel->setText("Bot wins! Game over.");
    }
}

bool BattleshipGame::isPositionInPossibleMoves(int row, int col) {
    return possibleMoves.contains(qMakePair(row, col));
}

void BattleshipGame::addAdjacentPositions(int row, int col) {
    QVector<QPair<int, int>> adjDirections = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    for (const auto& dir : adjDirections) {
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (isValidCell(newRow, newCol)) {
            if (userBoard.grid[newRow][newCol] != 'X' && userBoard.grid[newRow][newCol] != 'O' &&
                !isPositionInPossibleMoves(newRow, newCol)) {
                possibleMoves.append({newRow, newCol});
            }
        }
    }
}

void BattleshipGame::botHardAttack() {
    if (gameOver) return;

    if (!lastHits.isEmpty()) {
        QPair<int, int> lastHit = lastHits.last();
        int newRow = lastHit.first + directions[currentDirection].first;
        int newCol = lastHit.second + directions[currentDirection].second;

        if (isValidCell(newRow, newCol)) {
            if (userBoard.grid[newRow][newCol] != 'X' && userBoard.grid[newRow][newCol] != 'O') {
                QPushButton *button = findButtonAt(newRow, newCol, userGridLayout);
                if (userBoard.attack(newRow, newCol)) {
                    button->setIcon(hitIcon);
                    lastHits.append({newRow, newCol});
                    messageLabel->setText("Bot hit your ship!");
                    if (!userBoard.hasShipsRemaining()) {
                        gameOver = true;
                        messageLabel->setText("Bot wins! Game over.");
                    }
                } else {
                    button->setIcon(missIcon);
                    messageLabel->setText("Bot missed!");
                    currentDirection = (currentDirection + 1) % 4;
                }
                return;
            }
        }

        currentDirection = (currentDirection + 1) % 4;
        if (currentDirection == 0) {
            lastHits.pop_back();
            if (!lastHits.isEmpty()) {
                botHardAttack();
            } else {
                botSmartAttack();
            }
        } else {
            botHardAttack();
        }
    } else {
        botSmartAttack();
    }
}