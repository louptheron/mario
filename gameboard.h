#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QKeyEvent>
#include <QMutableMapIterator>
#include <QObject>

#include "safe.h"
#include "floor.h"
#include "mario.h"
#include "model.h"


class View;


class GameBoard : QObject
{
    Q_OBJECT

public:
    GameBoard(Model *m, View *v);
    ~GameBoard();
    void stopGame();
    void movementMario();
    void moveXMario(int );
    void movementMap();
    void movementBackground();
    void movementMushroom();
    void moveBrick(int ,Brick *);
    void splashScreen();
    void hurted();
    bool intersectBottomMario();
    bool intersectTopMario();
    bool intersectRightMario();
    bool intersectLeftMario();
    void intersectGoldMario();
    void intersectMushroomMario();
    void intersectDarkEaterMario();
    void intersectFlameMario();
    bool intersectBottomBrick(Brick *);
    bool intersectRightBrick(Brick *);
    bool intersectLeftBrick(Brick *);
    void goldAnim();
    void flameAnim();
    void movementDarkEater();
    void fantom();
    inline bool getIsMovingR(){ return getModel()->getMario()->getIsMovingR(); }
    inline bool getIsMovingL(){ return getModel()->getMario()->getIsMovingL(); }
    inline bool getIsJumping(){ return getModel()->getMario()->getIsJumping(); }
    inline void setIsMovingR(bool is){ getModel()->getMario()->setIsMovingR(is); }
    inline void setIsMovingL(bool is){ getModel()->getMario()->setIsMovingL(is); }
    inline void setIsJumping(bool is){ getModel()->getMario()->setIsJumping(is); }
    inline std::tuple<> getAllObjects();
    inline Model *getModel(){ return this->model; }
    inline qreal getOpacity(){ return opacity; }
    inline void setOpacity(qreal opacity){this->opacity = opacity; }
    inline int getXRelatif(){ return xRelatif; }
    inline void setXRelatif(int x){this->xRelatif = x; }
    void timerEvent(QTimerEvent *event);



private:
    Model *model;
    View *view;
    int timerId;
    bool gameStarted;
    int xRelatif;
    int yRelatif;
    int startJumpY;
    int iterBackground;
    int tempMove = 0;
    int marioSize;
    int darkEaterTempo = 0;
    int tempGold = 0;
    int tempFlame = 0;
    qreal opacity = 1;
    bool isLittle;
    int showBloodCount = 0;

signals:
    void sendPaintIt();

};
#endif // GAMEBOARD_H
