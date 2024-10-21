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
    lastHit(-1, -1),
    gamePhase(PlacingShips),
    player1Board(numShips), player2Board(numShips),
    currentShipPlayer1(0), currentShipPlayer2(0),
    currentPlayer(1)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    createIcons();
    showStartupDialog();
    setupUI();
    if (currentMode == SinglePlayer) {
        botPlaceShips();
    }
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


void BattleshipGame::showStartupDialog() {
    QDialog *startupDialog = new QDialog(this);
    startupDialog->setWindowTitle("Select Game Mode");
    QVBoxLayout *dialogLayout = new QVBoxLayout;

    QPushButton *singlePlayerButton = new QPushButton("Single Player");
    QPushButton *multiplayerButton = new QPushButton("Multiplayer");

    dialogLayout->addWidget(new QLabel("Choose Game Mode:"));
    dialogLayout->addWidget(singlePlayerButton);
    dialogLayout->addWidget(multiplayerButton);
    startupDialog->setLayout(dialogLayout);

    connect(singlePlayerButton, &QPushButton::clicked, this, [&]() {
        currentMode = SinglePlayer;
        startupDialog->accept();
    });

    connect(multiplayerButton, &QPushButton::clicked, this, [&]() {
        currentMode = Multiplayer;
        startupDialog->accept();
    });

    startupDialog->exec();
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

    if (currentMode == SinglePlayer) {
        topLayout->addWidget(difficultyLabel);
        topLayout->addWidget(difficultyComboBox);
    }
    topLayout->addStretch();
    topLayout->addWidget(orientationBox);

    if (currentMode == SinglePlayer) {
        userGridLayout = new QGridLayout;
        botGridLayout = new QGridLayout;

        QLabel *userBoardLabel = new QLabel("<b>Your Board</b>");
        userBoardLabel->setAlignment(Qt::AlignCenter);
        QLabel *botBoardLabel = new QLabel("<b>Bot's Board</b>");
        botBoardLabel->setAlignment(Qt::AlignCenter);

        setupBoard(userGridLayout);
        setupBoard(botGridLayout, true);

        QVBoxLayout *userBoardLayout = new QVBoxLayout;
        userBoardLayout->addWidget(userBoardLabel);
        userBoardLayout->addLayout(userGridLayout);

        QVBoxLayout *botBoardLayout = new QVBoxLayout;
        botBoardLayout->addWidget(botBoardLabel);
        botBoardLayout->addLayout(botGridLayout);

        boardsLayout->addLayout(userBoardLayout);
        boardsLayout->addSpacing(50);
        boardsLayout->addLayout(botBoardLayout);

        messageLabel = new QLabel("Place your ships on your board.");
        messageLabel->setAlignment(Qt::AlignCenter);
    } else {
        player1GridLayout = new QGridLayout;
        player2GridLayout = new QGridLayout;

        QLabel *player1BoardLabel = new QLabel("<b>Player 1's Board</b>");
        player1BoardLabel->setAlignment(Qt::AlignCenter);
        QLabel *player2BoardLabel = new QLabel("<b>Player 2's Board</b>");
        player2BoardLabel->setAlignment(Qt::AlignCenter);

        setupBoard(player1GridLayout);
        setupBoard(player2GridLayout);

        // Initially hide Player 2's board
        for (int i = 0; i < player2GridLayout->count(); ++i) {
            QWidget *widget = player2GridLayout->itemAt(i)->widget();
            if (widget)
                widget->hide();
        }

        QVBoxLayout *player1BoardLayout = new QVBoxLayout;
        player1BoardLayout->addWidget(player1BoardLabel);
        player1BoardLayout->addLayout(player1GridLayout);

        QVBoxLayout *player2BoardLayout = new QVBoxLayout;
        player2BoardLayout->addWidget(player2BoardLabel);
        player2BoardLayout->addLayout(player2GridLayout);

        boardsLayout->addLayout(player1BoardLayout);
        boardsLayout->addSpacing(50);
        boardsLayout->addLayout(player2BoardLayout);

        messageLabel = new QLabel("Player 1: Place your ships on your board.");
        messageLabel->setAlignment(Qt::AlignCenter);
    }

    restartButton = new QPushButton("Restart Game");
    exitButton = new QPushButton("Exit");
    connect(restartButton, &QPushButton::clicked, this, &BattleshipGame::onRestartClicked);
    connect(exitButton, &QPushButton::clicked, this, &BattleshipGame::onExitClicked);

    bottomLayout->addWidget(restartButton);
    bottomLayout->addWidget(exitButton);

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

            if (currentMode == SinglePlayer) {
                if (isBotBoard) {
                    connect(button, &QPushButton::clicked, this, [=]() {
                        if (!gameOver && !isPlacingShips) {
                            userAttack(row, col, button);
                        }
                    });
                } else {
                    connect(button, &QPushButton::clicked, this, [=]() {
                        if (isPlacingShips) {
                            userPlaceShip(row, col, button);
                        }
                    });
                }
            } else {
                if (gridLayout == player1GridLayout) {
                    connect(button, &QPushButton::clicked, this, [=]() {
                        if (gamePhase == PlacingShips && currentPlayer == 1) {
                            multiplayerPlaceShip(row, col, button);
                        } else if (gamePhase == Attacking && currentPlayer == 2) {
                            multiplayerAttack(row, col, button);
                        }
                    });
                } else if (gridLayout == player2GridLayout) {
                    connect(button, &QPushButton::clicked, this, [=]() {
                        if (gamePhase == PlacingShips && currentPlayer == 2) {
                            multiplayerPlaceShip(row, col, button);
                        } else if (gamePhase == Attacking && currentPlayer == 1) {
                            multiplayerAttack(row, col, button);
                        }
                    });
                }
            }

            gridLayout->addWidget(button, row, col);
        }
    }
}

void BattleshipGame::userPlaceShip(int row, int col, QPushButton *button) {
    if (currentShip < numShips) {
        int shipLength = MIN_SHIP_SIZE + (rand() % (MAX_SHIP_SIZE - MIN_SHIP_SIZE + 1));
        bool isVertical = verticalRadio->isChecked();
        if (userBoard.isValidPosition(row, col, isVertical, shipLength)) {
            userBoard.placeShip(row, col, isVertical, shipLength);
            for (int i = 0; i < shipLength; ++i) {
                QPushButton *shipButton = isVertical ?
                                              findButtonAt(row + i, col, userGridLayout) :
                                              findButtonAt(row, col + i, userGridLayout);
                shipButton->setIcon(shipIcon);
            }
            currentShip++;
            if (currentShip == numShips) {
                isPlacingShips = false;
                messageLabel->setText("All ships placed! Attack the bot's ships.");
            }
        } else {
            QMessageBox::warning(this, "Invalid Position", "You cannot place a ship here.");
        }
    }
}
void BattleshipGame::userAttack(int row, int col, QPushButton *button) {
    if (botBoard.attack(row, col)) {
        button->setIcon(hitIcon);
        messageLabel->setText("Hit!");
        if (!botBoard.hasShipsRemaining()) {
            gameOver = true;
            messageLabel->setText("You win! All bot's ships are sunk!");
        } else {
            botAttack();
        }
    } else {
        button->setIcon(missIcon);
        messageLabel->setText("Miss!");
        botAttack();
    }
}

void BattleshipGame::botAttack() {
    if (!gameOver) {
        if (difficulty == "Easy") {
            botEasyAttack();
        } else if (difficulty == "Medium") {
            botMediumAttack();
        } else if (difficulty == "Hard") {
            botHardAttack(lastHit.first, lastHit.second);
        }
        if (!userBoard.hasShipsRemaining()) {
            gameOver = true;
            messageLabel->setText("Bot wins! All your ships are sunk!");
        }
    }
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
    } else {
        button->setIcon(missIcon);
    }
}

void BattleshipGame::botMediumAttack() {
    if (botTargets.isEmpty()) {
        botEasyAttack();
    } else {
        int index = rand() % botTargets.size();
        QPair<int, int> target = botTargets[index];
        QPushButton *button = findButtonAt(target.first, target.second, userGridLayout);
        if (userBoard.attack(target.first, target.second)) {
            button->setIcon(hitIcon);
            lastHit = target;
            botTargets.removeAt(index);
        } else {
            button->setIcon(missIcon);
            botTargets.removeAt(index);
        }
    }
}

void BattleshipGame::botHardAttack(int lastHitRow, int lastHitCol) {
    QVector<QPair<int, int>> directions = {
        qMakePair(-1, 0), qMakePair(1, 0), qMakePair(0, -1), qMakePair(0, 1)
    };

    for (const auto& dir : directions) {
        int newRow = lastHitRow + dir.first;
        int newCol = lastHitCol + dir.second;
        if (newRow >= 0 && newRow < GRID_SIZE && newCol >= 0 && newCol < GRID_SIZE) {
            if (userBoard.grid[newRow][newCol] != 'X' && userBoard.grid[newRow][newCol] != 'O') {
                botTargets.append(qMakePair(newRow, newCol));
            }
        }
    }

    if (!botTargets.isEmpty()) {
        int index = rand() % botTargets.size();
        QPair<int, int> target = botTargets[index];

        QPushButton *button = findButtonAt(target.first, target.second, userGridLayout);
        if (userBoard.attack(target.first, target.second)) {
            button->setIcon(hitIcon);
            lastHit = target;
            botTargets.removeAt(index);
        } else {
            button->setIcon(missIcon);
            botTargets.removeAt(index);
        }
    } else {
        botEasyAttack();
    }
}

void BattleshipGame::multiplayerPlaceShip(int row, int col, QPushButton *button) {
    Board &currentBoard = (currentPlayer == 1) ? player1Board : player2Board;
    QGridLayout *currentGridLayout = (currentPlayer == 1) ? player1GridLayout : player2GridLayout;
    int &currentShip = (currentPlayer == 1) ? currentShipPlayer1 : currentShipPlayer2;

    if (currentShip < numShips) {
        int shipLength = MIN_SHIP_SIZE + (rand() % (MAX_SHIP_SIZE - MIN_SHIP_SIZE + 1));
        bool isVertical = verticalRadio->isChecked();
        if (currentBoard.isValidPosition(row, col, isVertical, shipLength)) {
            currentBoard.placeShip(row, col, isVertical, shipLength);
            for (int i = 0; i < shipLength; ++i) {
                QPushButton *shipButton = isVertical ?
                                              findButtonAt(row + i, col, currentGridLayout) :
                                              findButtonAt(row, col + i, currentGridLayout);
                shipButton->setIcon(shipIcon);
            }
            currentShip++;
            if (currentShip == numShips) {
                if (currentPlayer == 1) {
                    // Switch to Player 2
                    currentPlayer = 2;
                    messageLabel->setText("Player 2: Place your ships on your board.");

                    // Hide Player 1's board and show Player 2's board
                    for (int i = 0; i < player1GridLayout->count(); ++i) {
                        QWidget *widget = player1GridLayout->itemAt(i)->widget();
                        if (widget)
                            widget->hide();
                    }
                    for (int i = 0; i < player2GridLayout->count(); ++i) {
                        QWidget *widget = player2GridLayout->itemAt(i)->widget();
                        if (widget)
                            widget->show();
                    }
                } else {
                    // Both players have placed ships, start the game
                    gamePhase = Attacking;
                    currentPlayer = 1;
                    messageLabel->setText("Player 1's turn to attack.");

                    // Hide all ships on both boards
                    for (int i = 0; i < player1GridLayout->count(); ++i) {
                        QPushButton *btn = qobject_cast<QPushButton *>(player1GridLayout->itemAt(i)->widget());
                        if (btn && btn->icon().cacheKey() == shipIcon.cacheKey())
                            btn->setIcon(oceanIcon);
                    }
                    for (int i = 0; i < player2GridLayout->count(); ++i) {
                        QPushButton *btn = qobject_cast<QPushButton *>(player2GridLayout->itemAt(i)->widget());
                        if (btn && btn->icon().cacheKey() == shipIcon.cacheKey())
                            btn->setIcon(oceanIcon);
                    }

                    // Show opponent's board
                    for (int i = 0; i < player1GridLayout->count(); ++i) {
                        QWidget *widget = player1GridLayout->itemAt(i)->widget();
                        if (widget)
                            widget->hide();
                    }
                    for (int i = 0; i < player2GridLayout->count(); ++i) {
                        QWidget *widget = player2GridLayout->itemAt(i)->widget();
                        if (widget)
                            widget->show();
                    }
                }
            }
        } else {
            QMessageBox::warning(this, "Invalid Position", "You cannot place a ship here.");
        }
    }
}

void BattleshipGame::multiplayerAttack(int row, int col, QPushButton *button) {
    Board &opponentBoard = (currentPlayer == 1) ? player2Board : player1Board;
    QGridLayout *opponentGridLayout = (currentPlayer == 1) ? player2GridLayout : player1GridLayout;

    if (opponentBoard.grid[row][col] == 'X' || opponentBoard.grid[row][col] == 'O') {
        QMessageBox::warning(this, "Invalid Move", "You have already attacked this position.");
        return;
    }

    if (opponentBoard.attack(row, col)) {
        button->setIcon(hitIcon);
        messageLabel->setText(QString("Player %1 hit a ship!").arg(currentPlayer));
        if (!opponentBoard.hasShipsRemaining()) {
            messageLabel->setText(QString("Player %1 wins! All opponent's ships are sunk!").arg(currentPlayer));
            gamePhase = PlacingShips; // End the game
        }
    } else {
        button->setIcon(missIcon);
        messageLabel->setText(QString("Player %1 missed.").arg(currentPlayer));
    }

    // Switch turns
    currentPlayer = (currentPlayer == 1) ? 2 : 1;
    if (gamePhase != PlacingShips) {
        messageLabel->setText(QString("Player %1's turn to attack.").arg(currentPlayer));

        // Hide current board and show opponent's board
        QGridLayout *currentGridLayout = (currentPlayer == 1) ? player2GridLayout : player1GridLayout;
        QGridLayout *previousGridLayout = (currentPlayer == 1) ? player1GridLayout : player2GridLayout;

        for (int i = 0; i < previousGridLayout->count(); ++i) {
            QWidget *widget = previousGridLayout->itemAt(i)->widget();
            if (widget)
                widget->hide();
        }
        for (int i = 0; i < currentGridLayout->count(); ++i) {
            QWidget *widget = currentGridLayout->itemAt(i)->widget();
            if (widget)
                widget->show();
        }
    }
}


QPushButton *BattleshipGame::findButtonAt(int row, int col, QGridLayout *layout) {
    QLayoutItem *item = layout->itemAtPosition(row, col);
    if (item != nullptr) {
        return qobject_cast<QPushButton *>(item->widget());
    }
    return nullptr;
}

void BattleshipGame::onDifficultyChanged(const QString &selectedDifficulty) {
    difficulty = selectedDifficulty;
    if (difficulty == "Easy") {
        numShips = 4;
    } else if (difficulty == "Medium") {
        numShips = 5;
    } else if (difficulty == "Hard") {
        numShips = 3;
    }
    resetGame();
    messageLabel->setText("Difficulty changed to " + difficulty + ". Place your ships.");
}

void BattleshipGame::resetGame() {
    userBoard = Board(numShips);
    botBoard = Board(numShips);
    player1Board = Board(numShips);
    player2Board = Board(numShips);
    currentShip = 0;
    isPlacingShips = true;
    gameOver = false;
    botTargets.clear();
    lastHit = qMakePair(-1, -1);
    currentShipPlayer1 = 0;
    currentShipPlayer2 = 0;
    currentPlayer = 1;
    gamePhase = PlacingShips;

    if (currentMode == SinglePlayer) {
        // Clear the grids
        for (int i = 0; i < userGridLayout->rowCount(); ++i) {
            for (int j = 0; j < userGridLayout->columnCount(); ++j) {
                QPushButton *button = findButtonAt(i, j, userGridLayout);
                if (button) {
                    button->setIcon(oceanIcon);
                }
            }
        }

        for (int i = 0; i < botGridLayout->rowCount(); ++i) {
            for (int j = 0; j < botGridLayout->columnCount(); ++j) {
                QPushButton *button = findButtonAt(i, j, botGridLayout);
                if (button) {
                    button->setIcon(oceanIcon);
                }
            }
        }

        messageLabel->setText("Place your ships on your board.");
        botPlaceShips();
    } else {
        // Multiplayer reset
        // Clear the grids
        for (int i = 0; i < player1GridLayout->rowCount(); ++i) {
            for (int j = 0; j < player1GridLayout->columnCount(); ++j) {
                QPushButton *button = findButtonAt(i, j, player1GridLayout);
                if (button) {
                    button->setIcon(oceanIcon);
                    button->show();
                }
            }
        }

        for (int i = 0; i < player2GridLayout->rowCount(); ++i) {
            for (int j = 0; j < player2GridLayout->columnCount(); ++j) {
                QPushButton *button = findButtonAt(i, j, player2GridLayout);
                if (button) {
                    button->setIcon(oceanIcon);
                    button->hide();
                }
            }
        }

        messageLabel->setText("Player 1: Place your ships on your board.");
    }
}

void BattleshipGame::botPlaceShips() {
    for (int i = 0; i < numShips; i++) {
        int shipLength = MIN_SHIP_SIZE + (rand() % (MAX_SHIP_SIZE - MIN_SHIP_SIZE + 1));
        bool isVertical = rand() % 2;
        int row, col;
        do {
            row = rand() % GRID_SIZE;
            col = rand() % GRID_SIZE;
        } while (!botBoard.isValidPosition(row, col, isVertical, shipLength));
        botBoard.placeShip(row, col, isVertical, shipLength);
    }
}

void BattleshipGame::onRestartClicked() {
    resetGame();
}

void BattleshipGame::onExitClicked() {
    QApplication::quit();
}

// Include the MOC file if your build system doesn't do it automatically
#include "battleshipgame.moc"
