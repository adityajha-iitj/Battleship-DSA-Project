// main.cpp
#include <QApplication>
#include "battleshipgame.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BattleshipGame game;
    game.show();

    return app.exec();
}
