#include "brick.h"
#include <iostream>

Brick::Brick(int x,int y){
    destroyed = false;
    rect.translate(x, y);
}

Brick::Brick(int x, int y, QString picture)
{
    image.load(picture);
    destroyed = false;
    rect = image.rect();
    rect.translate(x, y);
}

Brick::~Brick()
{
    std::cout << ("Brick deleted\n");
}

QRect Brick::getRect()
{
    return rect;
}

void Brick::setRect(QRect rct)
{
    rect = rct;
}

QImage & Brick::getImage()
{
    return image;
}

bool Brick::isDestroyed()
{
    return destroyed;
}

void Brick::setDestroyed(bool destr)
{
    destroyed = destr;
}

void Brick::moveBrick()
{
    rect.moveTo(rect.left()-2, rect.top());
}

