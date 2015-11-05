#include "gameboard.h"
#include <QApplication>
#include <QDebug>
#include <QRect>
#include <QTimer>

#include "model.h"
#include "view.h"

GameBoard::GameBoard(Model *m, View *v) : QObject()
{
    this->model = m;
    this->view = v;
    this->view->setControl(this);
    gameStarted = true;
    xRelatif = -100;
    yRelatif = 0;
    iterBackground=0;

    timerId = startTimer(13);

    connect(this, SIGNAL(sendPaintIt()), view, SLOT(paintIt()));
}

int Gold::currentFrame = 0;
int Shock::currentFrame = 0;
int Flame::currentFrame = 0;
int Brick::speed = 5;

//----------------------------------------------------------------------------------------------------------------//

GameBoard::~GameBoard()
{
}

//----------------------------------------------------------------------------------------------------------------//

void GameBoard::stopGame()
{
    killTimer(timerId);
    gameStarted = false;
}

//----------------------------------------------------------------------------------------------------------------//

void GameBoard::timerEvent(QTimerEvent *event)
{
    encart();
    splashScreen();
    movementMario();
    movementMushroom();
    movementDarkEater();
    movementMysticTree();
    movementPeach();
    movementEncart();
    model->brickOrganisation();
    goldAnim();
    flameAnim();
    shockAnim();
    hurted();
    GameOver();
    Peach();
    emit sendPaintIt();
}

//----------------------------------------------------------------------------------------------------------------//

void GameBoard::movementMario()
{
    int y=model->getMario()->getRect().y();

    if(getIsJumping()){
        xRelatif+=4;
        if(getIsAttacking()){
            xRelatif += 7;
        }
        yRelatif=(-0.02*(xRelatif*xRelatif)+200);
        y = startJumpY-yRelatif;
        moveXMario(y);
        model->getMario()->setCurrentFrame(0);
        if(intersectTopMario()){
            xRelatif=0;
            yRelatif=0;
            startJumpY=model->getMario()->getRect().y();
            setIsJumping(false);
        }
    }

    if(intersectBottomMario()){
        xRelatif=0;
        yRelatif=0;
        startJumpY=model->getMario()->getRect().y();
        setIsJumping(false);
        moveXMario(y);

        if(getIsAttacking()){
            if(getModel()->getMario()->getIsLittle())
                getModel()->getShock()->move(model->getMario()->getRect().x() - 60, model->getMario()->getRect().y() - 25);
            else
                getModel()->getShock()->move(model->getMario()->getRect().x() - 50, model->getMario()->getRect().y());
            getModel()->getShock()->setShow(true);
            Shock::currentFrame = 0;
            intersectDarkEaterMario();
            intersectMysticTreeMario();
            setIsAttacking(false);
        }

        if(getIsMovingR() && tempMove == 1){
            model->getMario()->setCurrentFrame(model->getMario()->getCurrentFrame() + 57);
            if (model->getMario()->getCurrentFrame() >= 1190 )
                model->getMario()->setCurrentFrame(0);
            tempMove = 0;
        }
        else if(getIsMovingR())
            tempMove++;
        else if(getIsMovingL() && tempMove == 1){
            model->getMario()->setCurrentFrame(model->getMario()->getCurrentFrame() - 57);
            if (model->getMario()->getCurrentFrame() <= 0 )
                model->getMario()->setCurrentFrame(1191);
            tempMove = 0;
        }
        else if(getIsMovingL())
            tempMove++;
        else
            model->getMario()->setCurrentFrame(0);
    }

    if((!intersectBottomMario() && !getIsJumping())){
        qDebug() << "!intersectBottom";
        yRelatif=(-0.02*(xRelatif*xRelatif));
        xRelatif+=4;
        if(getIsAttacking()){
            xRelatif += 10;
        }
        y = startJumpY-yRelatif;
        moveXMario(y);
        model->getMario()->setCurrentFrame(0);

        if(getModel()->getMario()->getRect().x() > 500)
            getModel()->getMario()->setLife(0);
    }
    intersectGoldMario();
    intersectMushroomMario();
    intersectDarkEaterMario();
    intersectFlameMario();
    intersectMysticTreeMario();
    intersectPeachMario();
}

void GameBoard::movementDarkEater()
{
    if(model->getDarkEaterBool()){
        if(model->getDarkEater()->getIsMovingL()){
            model->getDarkEater()->move(model->getDarkEater()->getRect().x() - 2, model->getDarkEater()->getRect().y());
            if(darkEaterTempo == 15){
                model->getDarkEater()->setCurrentFrame(model->getDarkEater()->getCurrentFrame() + 52);
                if (model->getDarkEater()->getCurrentFrame() >= 156 )
                    model->getDarkEater()->setCurrentFrame(1);
                darkEaterTempo = 0;
            }
            else{
                darkEaterTempo++;
            }
        }
    }
}

void GameBoard::movementPeach()
{
    if(model->getIsPeachBool()){
        if(model->getPeach()->getIsMovingL()){

            if(peachTempo == 30){
                model->getPeach()->setCurrentFrame(model->getPeach()->getCurrentFrame() + 50);
                if (model->getPeach()->getCurrentFrame() >= 200 )
                    model->getPeach()->setCurrentFrame(0);
                peachTempo = 0;
            }
            else{
                peachTempo++;
            }
        }
    }
}

void GameBoard::movementMushroom()
{
    for(int i = 0; i<model->getMushroom()->size(); i++){
        int x=model->getMushroom()->at(i)->getRect().x();
        if(model->getMushroom()->at(i)->getMoveCount()>0){
            model->getMushroom()->at(i)->setmoveCount(model->getMushroom()->at(i)->getMoveCount() - 1);
            if(getIsMovingR() && model->getMario()->getRect().x()>=350  && !intersectRightMario())
                model->getMushroom()->at(i)->move(x-Brick::speed, model->getMushroom()->at(i)->getRect().y() - 1);
            else
                model->getMushroom()->at(i)->move(x, model->getMushroom()->at(i)->getRect().y() - 1);
        }
        else
            moveBrick(x ,model->getMushroom()->at(i));
    }
}

void GameBoard::moveBrick(int x ,Brick * b)
{
    int speed;
    if(getIsMovingR() && model->getMario()->getRect().x()>=350  && !intersectRightMario())
        speed=Brick::speed ;
    else
        speed = 0;

    if(intersectBottomBrick(b)){
        if(b->getMoveX()){
            b->move(x-speed+2, b->getRect().y());
            if(intersectRightBrick(b))
                b->setMoveX(false);
        }
        else if(!b->getMoveX()){
            b->move(x-speed-2, b->getRect().y());
            if( intersectLeftBrick(b))
                b->setMoveX(true);
        }
        b->setYR(0);
        b->setXR(0);
        b->setStartY(b->getRect().y());
    }
    else{
        b->setYR(-0.02*(b->getXR()*b->getXR()));
        b->setXR(b->getXR()+3);
        int y = b->getStartY()- b->getYR();
        if(b->getMoveX()){
            b->move(x-speed+2, y);
            if(intersectRightBrick(b))
                b->setMoveX(false);
        }
        else if(!b->getMoveX()){
            b->move(x-speed-2,y);
            if( intersectLeftBrick(b))
                b->setMoveX(true);
        }
    }
}

void GameBoard::moveXMario(int y)
{
    int x=model->getMario()->getRect().x();
    if(getIsMovingL() && model->getMario()->getRect().x()>=2 && !intersectLeftMario())
        x -= Brick::speed;
    else if(getIsMovingR() && model->getMario()->getRect().x()<=350  && !intersectRightMario())
        x += Brick::speed;
    else if(getIsMovingR() && model->getMario()->getRect().x()>=350  && !intersectRightMario())
        movementMap();
    model->getMario()->move(x,y);
}

void GameBoard::movementMap()
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        model->getFloors()->at(i)->moveBrick();
    }

    if(iterBackground == 2){
        for(int i = 0; i<model->getBackground()->size(); i++){
            model->getBackground()->at(i)->moveBrick();
        }
        iterBackground=0;
    }
    else{
        for(int i = 0; i<model->getBackground()->size(); i++){
            model->getBackground()->at(i)->move(model->getBackground()->at(i)->getRect().x(), model->getBackground()->at(i)->getRect().y());
        }
        iterBackground++;
    }

    for(int i = 0; i<model->getSafes()->size(); i++){
        model->getSafes()->at(i)->moveBrick();
    }

    for(int i = 0; i<model->getCompteur()->size(); i++){
        model->getCompteur()->at(i)->moveBrick();
    }

    for(int i = 0; i<model->getGold()->size(); i++){
        model->getGold()->at(i)->moveBrick();
    }

    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        model->getMysticTrees()->at(i)->move(model->getMysticTrees()->at(i)->getRect().x() - Brick::speed, model->getMysticTrees()->at(i)->getRect().y());
    }

    if(model->getDarkEaterBool())
        model->getDarkEater()->move(model->getDarkEater()->getRect().x() - Brick::speed, model->getDarkEater()->getRect().y());

    if(getModel()->getIsPeachBool())
        model->getPeach()->move(model->getPeach()->getRect().x() - Brick::speed, model->getPeach()->getRect().y());
}

//----------------------------------------------------------------------------------------------------------------//

bool GameBoard::intersectTopMario()
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(model->getMario()->intersectTop(model->getFloors()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getSafes()->size(); i++){
        if(model->getMario()->intersectTop(model->getSafes()->at(i)->getRect())){
            if(model->getSafes()->at(i)->getCapacity()){
                if(model->getSafes()->at(i)->getCapacity() == 2){
                    model->createMushroom(model->getSafes()->at(i)->getRect().x(), model->getSafes()->at(i)->getRect().y());
                    model->getSafes()->at(i)->setCapacity(1);
                }
            }else
                model->getSafes()->at(i)->setDestroyed(true);
            return true;
        }
    }
    return false;
}

bool GameBoard::intersectBottomMario()
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(model->getMario()->intersectBottom(model->getFloors()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getSafes()->size(); i++){
        if(model->getMario()->intersectBottom(model->getSafes()->at(i)->getRect())){
            if(getIsAttacking()){
                if(model->getSafes()->at(i)->getCapacity()){
                    if(model->getSafes()->at(i)->getCapacity() == 2){
                        model->createMushroom(model->getSafes()->at(i)->getRect().x(), model->getSafes()->at(i)->getRect().y());
                        model->getSafes()->at(i)->setCapacity(1);
                    }
                }else
                    model->getSafes()->at(i)->setDestroyed(true);
            }
            return true;
        }
    }

    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        if(model->getMario()->intersectBottom(model->getMysticTrees()->at(i)->getRect()))
            return true;
    }

    if(model->getDarkEaterBool()){
        if(model->getMario()->intersectBottom(model->getDarkEater()->getRect()))
            return true;
    }

    if(model->getPeach()){
        if(model->getMario()->intersectBottom(model->getPeach()->getRect()))
            return true;
    }
    return false;
}

bool GameBoard::intersectLeftMario()
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(model->getMario()->intersectLeft(model->getFloors()->at(i)->getRect()))
            return true;
    }
    for(int i = 0; i<model->getSafes()->size(); i++){
        if(model->getMario()->intersectLeft(model->getSafes()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        if(model->getMario()->intersectLeft(model->getMysticTrees()->at(i)->getRect()))
            return true;
    }

    if(model->getPeach()){
        if(model->getMario()->intersectLeft(model->getPeach()->getRect()))
            return true;
    }

    return false;
}

bool GameBoard::intersectRightMario()
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(model->getMario()->intersectRight(model->getFloors()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getSafes()->size(); i++){
        if(model->getMario()->intersectRight(model->getSafes()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        if(model->getMario()->intersectRight(model->getMysticTrees()->at(i)->getRect()))
            return true;
    }

    if(model->getPeach()){
        if(model->getMario()->intersectRight(model->getPeach()->getRect()))
            return true;
    }

    return false;
}

void GameBoard::intersectGoldMario()
{
    for(int i = 0; i<model->getGold()->size(); i++){
        if(model->getMario()->intersect(model->getGold()->at(i)->getRect())){
            model->getGold()->at(i)->setDestroyed(true);
            model->getMario()->setGoldNumber(model->getMario()->getGoldNumber()+1);
        }
    }
}

void GameBoard::intersectPeachMario()
{
    if(getModel()->getIsPeachBool()){
        if(model->getMario()->intersect(model->getPeach()->getRect())){
            getModel()->getEncart()->setShow(true);
            encartTime = 0;
            if(getModel()->getEncart()->getType() != EncartType::LOVE){
                getModel()->createEncart(getModel()->getMario()->getRect().x(), getModel()->getMario()->getRect().y() - 100, ":images/speech.png");
                getModel()->getEncart()->setType(EncartType::LOVE);
            }
        }
    }
}

void GameBoard::intersectFlameMario()
{
    for(int i = 0; i<model->getFlame()->size(); i++){
        if(model->getMario()->intersect(model->getFlame()->at(i)->getRect()) && !model->getMario()->getUntouchable()){
            showBloodCount = 0;
            this->model->getMario()->setIsHurted(true);
        }
    }
}

void GameBoard::intersectDarkEaterMario()
{
    if(model->getDarkEaterBool()){
        if(model->getMario()->intersectBottom(model->getDarkEater()->getRect())
                && !getModel()->getDarkEater()->isDead()
                && !model->getMario()->getUntouchable()
                && model->getMario()->getIsAttacking()){
            model->getDarkEater()->setIsMovingL(false);
            getModel()->getDarkEater()->setDead(true);
        }
        else if(model->getMario()->intersectBottom(model->getDarkEater()->getRect())){
            return;
        }
        else if((model->getMario()->intersectRight(model->getDarkEater()->getRect())
                 || model->getMario()->intersectTop(model->getDarkEater()->getRect()))
                && !model->getMario()->getUntouchable()
                && !model->getDarkEater()->isDead()
                )
        {
            showBloodCount = 0;
            this->model->getMario()->setIsHurted(true);
        }
    }
}

void GameBoard::intersectMushroomMario()
{
    for(int i = 0; i<model->getMushroom()->size(); i++){
        if(model->getMario()->intersect(model->getMushroom()->at(i)->getRect())){
            model->getMushroom()->at(i)->setDestroyed(true);
            model->getMario()->setIsLittle(true);
            model->getMario()->setLife(model->getMario()->getLife() + 1);
            Brick::speed = 6;
        }
    }
}

bool GameBoard::intersectBottomBrick(Brick * m)
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(m->intersectBottom(model->getFloors()->at(i)->getRect()))
            return true;
    }
    for(int i = 0; i<model->getSafes()->size(); i++){
        if(m->intersectBottom(model->getSafes()->at(i)->getRect()))
            return true;
    }
    return false;
}

bool GameBoard::intersectLeftBrick(Brick * m)
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(m->intersectLeft(model->getFloors()->at(i)->getRect()))
            return true;
    }
    for(int i = 0; i<model->getSafes()->size(); i++){
        if(m->intersectLeft(model->getSafes()->at(i)->getRect()))
            return true;
    }
    return false;
}

bool GameBoard::intersectRightBrick(Brick * m)
{
    for(int i = 0; i<model->getFloors()->size(); i++){
        if(m->intersectRight(model->getFloors()->at(i)->getRect()))
            return true;
    }

    for(int i = 0; i<model->getSafes()->size(); i++){
        if(m->intersectRight(model->getSafes()->at(i)->getRect()))
            return true;
    }
    return false;
}

void GameBoard::intersectMysticTreeMario()
{
    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        if(model->getMario()->intersectBottom(model->getMysticTrees()->at(i)->getRect())
                && !getModel()->getMysticTrees()->at(i)->isDead()
                && model->getMario()->getIsAttacking()){

            getModel()->getMysticTrees()->at(i)->setDead(true);
            model->getMysticTrees()->at(i)->setIsMovingL(false);
            getModel()->getShock()->move(model->getMysticTrees()->at(i)->getRect().x() - 50, model->getMysticTrees()->at(i)->getRect().y() - 50);
            getModel()->getShock()->setShow(true);
            Shock::currentFrame = 0;

        }
        else if(model->getMario()->intersectBottom(model->getMysticTrees()->at(i)->getRect())){
            return;
        }
        else if((model->getMario()->intersectRight(model->getMysticTrees()->at(i)->getRect())
                 || model->getMario()->intersectLeft(model->getMysticTrees()->at(i)->getRect()))
                && !model->getMario()->getUntouchable()
                && !model->getMysticTrees()->at(i)->isDead()){
            showBloodCount = 0;
            this->model->getMario()->setIsHurted(true);
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------//

void GameBoard::splashScreen()
{
    if(getModel()->getSplashScreen()->getType() == SplashScreenType::GO){
        int x=model->getSplashScreen()->getRect().x();
        int y=model->getSplashScreen()->getRect().y();
        y--;
        if(model->getSplashScreen()->getRect().bottom() > 0 && model->getSplashScreen()->getIsSplashScreen())
            model->getSplashScreen()->move(x, y);
        else
            model->getSplashScreen()->setIsSplashScreen(false);
    }
}

//-----------------------------------------------------------------------------------------------------------------------//

void GameBoard::goldAnim()
{
    if(tempGold == 20){
        Gold::currentFrame += 40;
        if (Gold::currentFrame >= 120)
            Gold::currentFrame = 0;
        tempGold = 0;
    }
    else
        tempGold++;
}

void GameBoard::shockAnim(){
    if(tempShock == 3){
        Shock::currentFrame += 66;
        if (Shock::currentFrame >= 200){
            Shock::currentFrame = 0;
            getModel()->getShock()->setShow(false);
        }
        tempShock = 0;
    }
    else
        tempShock++;
}

void GameBoard::flameAnim(){
    if(tempFlame == 10){
        Flame::currentFrame += 58;
        if (Flame::currentFrame >= 520)
            Flame::currentFrame = 3;
        tempFlame = 0;
    }
    else
        tempFlame++;
    for(int i = 0; i<model->getFlame()->size(); i++){
        int x=model->getFlame()->at(i)->getRect().x();
        moveBrick(x ,model->getFlame()->at(i));
    }
}

void GameBoard::hurted()
{
    if(model->getMario()->getIsHurted()){
        model->getMario()->setUntouchable(true);
        if(showBloodCount >= 15){
            this->getModel()->getBlood()->setStopBlood(true);
            showBloodCount = 0;
        }
        else{
            showBloodCount++;
        }

        this->getModel()->getBlood()->move(model->getMario()->getRect().x() - 20, model->getMario()->getRect().y() - 5);

        if(getModel()->getMario()->getInvicible() == 0){
            getModel()->getEncart()->setShow(true);
            encartTime = 0;
            if(getModel()->getEncart()->getType() != EncartType::HURT){
                getModel()->createEncart(getModel()->getMario()->getRect().x(), getModel()->getMario()->getRect().y() - 100, ":images/speech_hell.png");
                getModel()->getEncart()->setType(EncartType::HURT);
            }
            model->getMario()->setLife(model->getMario()->getLife() - 1);
        }
        if(getModel()->getMario()->getInvicible() > 100){
            getModel()->getMario()->setUntouchable(false);
            getModel()->getMario()->setIsHurted(false);
            getModel()->getBlood()->setStopBlood(false);
            getModel()->getMario()->setInvicible(0);
            getModel()->getEncart()->setShow(false);
        }
        else{
            getModel()->getMario()->setInvicible(getModel()->getMario()->getInvicible() + 1);
        }


    }
}

void GameBoard::movementEncart(){
    if(getModel()->getEncart()->getShow())
        getModel()->getEncart()->move(getModel()->getMario()->getRect().x(), getModel()->getMario()->getRect().y() - 100);
}

void GameBoard::fantom()
{
    if(model->getMario()->getDieRect().bottom() > model->getMario()->getRect().top() - 200){
        int x=model->getMario()->getDieRect().x();
        int y=model->getMario()->getDieRect().y();
        y = y - 3;
        model->getMario()->moveDie(x, y);
    }
    else {
        getModel()->getMario()->setUntouchable(false);
        getModel()->getMario()->setIsHurted(false);
        getModel()->getBlood()->setStopBlood(false);
    }
}

void GameBoard::movementMysticTree(){
    for(int i = 0; i<model->getMysticTrees()->size(); i++){
        if(model->getMysticTrees()->at(i)->getIsMovingL()){
            if(model->getMysticTrees()->at(i)->getMoveCount() > 0){
                model->getMysticTrees()->at(i)->setmoveCount(model->getMysticTrees()->at(i)->getMoveCount() - 2);
                model->getMysticTrees()->at(i)->move(model->getMysticTrees()->at(i)->getRect().x(), model->getMysticTrees()->at(i)->getRect().y() - 2);
            }
            else if(model->getMysticTrees()->at(i)->getMoveCount() > - 120){
                model->getMysticTrees()->at(i)->setmoveCount(model->getMysticTrees()->at(i)->getMoveCount() - 2);
                model->getMysticTrees()->at(i)->move(model->getMysticTrees()->at(i)->getRect().x(), model->getMysticTrees()->at(i)->getRect().y() + 2);
            }
            else{
                model->getMysticTrees()->at(i)->setmoveCount(120);
            }
        }

    }
}

bool GameBoard::GameOver(){
    if(getModel()->getMario()->getLife() < 0 || getModel()->getMario()->getRect().y() > 500){
        getModel()->getEncart()->setShow(true);
        encartTime = 0;
        //getModel()->createEncart(getModel()->getMario()->getRect().x(), getModel()->getMario()->getRect().y() - 100, ":images/speech_fuck.png");
        if(getModel()->getSplashScreen()->getType() != SplashScreenType::GAME_OVER){
            getModel()->createGameOver(220, 100);
            getModel()->getSplashScreen()->setType(SplashScreenType::GAME_OVER);
        }
        model->getSplashScreen()->setIsSplashScreen(true);
        return true;
    }
    else
        return false;
}

bool GameBoard::Completed(){
    if(getModel()->getMario()->getGoldNumber() >= 100){
        if(getModel()->getSplashScreen()->getType() != SplashScreenType::COMPLETED){
            getModel()->createCompleted(360, 120);
            getModel()->getSplashScreen()->setType(SplashScreenType::COMPLETED);
        }
        model->getSplashScreen()->setIsSplashScreen(true);
        return true;
    }
    else
        return false;
}

void GameBoard::Peach(){
    if(getModel()->getMario()->getGoldNumber() > 2 && !getModel()->getIsPeachBool()){
        getModel()->createPeach(getModel()->getMario()->getRect().x() + 200, 340);
        getModel()->setIsPeachBool(true);
    }
}

void GameBoard::encart(){
    if(getModel()->getMario()->getGoldNumber() == 1){
        if(getModel()->getEncart()->getType() != EncartType::GOLD){
            getModel()->createEncart(getModel()->getMario()->getRect().x(), getModel()->getMario()->getRect().y() - 100, ":images/speech_gold.png");
            getModel()->getEncart()->setType(EncartType::GOLD);
        }
        encartTime = 0;
        getModel()->getEncart()->setShow(true);
    }

    if(getModel()->getEncart()->getType() == EncartType::GOLD){
        if(encartTime > 100){
            getModel()->getEncart()->setShow(false);
            getModel()->getEncart()->setType(EncartType::NONE);
        }
        else
            encartTime++;
    }
    else if(getModel()->getEncart()->getType() == EncartType::FUCK || getModel()->getEncart()->getType() == EncartType::LOVE){
        if(encartTime > 30){
            getModel()->getEncart()->setShow(false);
            getModel()->getEncart()->setType(EncartType::NONE);
        }
        else
            encartTime++;
    }
}
