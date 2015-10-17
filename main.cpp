#include "gameboard.h"
#include <QDesktopWidget>
#include <QApplication>
#include "model.h"
#include "view.h"

void center(QWidget &widget)
{
    int x, y;
    int screenWidth;
    int screenHeight;

    int WIDTH = 1000;
    int HEIGHT = 500;

    QDesktopWidget *desktop = QApplication::desktop();

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    x = (screenWidth - WIDTH) / 2;
    y = (screenHeight - HEIGHT) / 2;

    widget.setGeometry(x, y, WIDTH, HEIGHT);
    widget.setFixedSize(WIDTH, HEIGHT);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Model m;
    View v;
    GameBoard window(&m, &v);
    v.setWindowTitle("Mario Game");
    v.show();
    center(v);
    return app.exec();
}
