#include <QApplication>
#include "BattleshipGame.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    BattleshipGame game;
    game.show();
    
    return app.exec();
}
