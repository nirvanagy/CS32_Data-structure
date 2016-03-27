#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <list>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor

Actor:: Actor(StudentWorld* world, int imageID, int startX, int startY,Direction dir, double size, unsigned int depth):GraphObject(imageID,startX,startY,dir,size,depth),m_world(world){
    
    setVisible(true);
    m_alive=true;
}

StudentWorld* Actor:: getWorld(){
    return m_world;
}

bool Actor:: isAlive()const{
    return m_alive;
}

void Actor:: setDead(){
    m_alive=false;
    setVisible(false);
}

//change of possition when move in certain direction
void Actor::dirToCoord(Direction dir, int dis, int x, int y, int& nx, int& ny){
    switch (dir){
        case up:
            nx=x;
            ny=y+dis;
            break;
        case down:
            nx=x;
            ny=y-dis;
            break;
        case left:
            nx=x-dis;
            ny=y;
            break;
        case right:
            nx=x+dis;
            ny=y;
            break;
        default:
            break;
    }
}

void Actor::moveNStep(Direction dir,int n){
    
    int nx,ny;
    dirToCoord(dir, n, getX(), getY(),nx,ny);
    
    if (getWorld()->canActorMoveTo(this, nx, ny)) {
        moveTo(nx,ny);
    }
    else
        moveTo(getX(), getY());
}

//Dirt

Dirt:: Dirt(StudentWorld* world, int startX, int startY):Actor(world,IID_DIRT,startX,startY,right,0.25,3){
}

//Squirt

Squirt::Squirt(StudentWorld* world, int startX, int startY,Direction dir):Actor(world,IID_WATER_SPURT,startX,startY,dir,1.0,1){
    travelDist=4;
}


void Squirt::doSomething(){//need protesters' position!
    
    if(getWorld()->annoyProtesters(this, 2, 3)!=0){
        setDead();
        return;
    }
    
    if(travelDist!=0){
        int nx=0;
        int ny=0;
        dirToCoord(getDirection(), 1, getX(), getY(), nx, ny);

        bool canTravel=true;
        switch (getDirection()) {
            case GraphObject::up:
                nx=getX(); ny=getY()+5;
                break;
            case GraphObject::down:
                nx=getX(); ny=getY()-1;
                break;
            case GraphObject::left:
                nx=getX()-1; ny=getY();
                break;
            case GraphObject::right:
                nx=getX()+5; ny=getY();
                break;
            default:
                break;
        }


        if(nx!=getX()){
            for(int i=0;i<4;i++){
                if(getWorld()->isSpotOccupied(nx,ny+i))
                    canTravel=false;
            }
        }
        if (ny!=getY()) {
            for(int i=0;i<4;i++){
                if(getWorld()->isSpotOccupied(nx+i,ny))
                    canTravel=false;
            }
        }
        if (canTravel) {
            moveNStep(getDirection(),1);
            travelDist--;
            return;
        }
        else{
            setDead();
            return;
        }
    }
    else
        setDead();
}


//Boulder

Boulder::Boulder(StudentWorld* world, int startX, int startY):Actor(world,IID_BOULDER,startX,startY,down,1,1){
    m_state= stable;
    waitCountDown=29;
    
}
bool Boulder:: canActorsPassThroughMe()const{
    return false;
}
void Boulder:: doSomething(){
    
    if (!isAlive()) {
        return;
    }
    switch (m_state) {
        case stable:
            for (int i=0; i<4; i++) {
                if (getWorld()->withDirt(getX()+i, getY()-1)){
                    return;
                }
            }
            m_state=waiting;
            break;
            
        case waiting:
            if (!waitCountDown){
                m_state=falling;
                getWorld()->playSound(SOUND_FALLING_ROCK);
            }
            else
               waitCountDown--;
            break;
            
        case falling:
             //if heat agent
            getWorld()->annoyAllNearbyActors(this, 100, 3);
//            getWorld()->increaseScore((n*250));
            
            //if hit dirt, boulder or boundry of field
            int nx,ny;
            dirToCoord(down, 1, getX(), getY(), nx, ny);
            
            for(int i=0;i<4;i++){
                if(getWorld()->isSpotOccupied(nx+i,ny))
                    setDead();
            }
            moveNStep(down,1);

          
            break;
     
    }
    return;
    
}


//Agent

Agent::Agent(int hitPoints, StudentWorld* world, int imageID, int startX, int startY,Direction dir, double size, unsigned int depth): Actor(world,imageID,startX,startY,dir,size,depth){
    
    m_hitPonts=hitPoints;
    
}

int Agent::getHitPoints(){
    return m_hitPonts;
}
void  Agent::decHitPoints(int amt){
    m_hitPonts-=amt;
}

bool Agent::annoy(unsigned int amt){
    return false;
}



//FrackMan

FrackMan::FrackMan(StudentWorld* world):Agent(10,world, IID_PLAYER,30
                                              ,60,right,1.0,0){
    
    m_waterGun=5;//
    m_sonorCharge=1;
    m_goldNuggets=0;
    
    //Create a map of direction and key value
    dirMap[KEY_PRESS_DOWN]=down;
    dirMap[KEY_PRESS_UP]=up;
    dirMap[KEY_PRESS_LEFT]=left;
    dirMap[KEY_PRESS_RIGHT]=right;
    
};

bool FrackMan:: annoy(unsigned int amt){
    decHitPoints(amt);
    if (getHitPoints()<=0) {
        setDead();
    }
    return true;
    
}

bool FrackMan:: canDigThroughDirt() const{
    return true;
}
bool FrackMan:: canPickThingsUp() const{
    return true;
}

void FrackMan:: addGold(){
    m_goldNuggets++;
}
void FrackMan:: addSonar(){
    m_sonorCharge++;
}
void FrackMan:: addWater(){
    m_waterGun+=5;
}
unsigned int FrackMan:: getGold() const{
    return m_goldNuggets;
}
unsigned int FrackMan:: getSonar() const{
    return m_sonorCharge;
}
unsigned int FrackMan:: getWater() const{
    return m_waterGun;
}



void FrackMan::doSomething(){
    
    //check to see if it is currently alive
    if (!isAlive())
        return;
    
   // If the FrackMan’s 4x4 image currently overlaps any Dirt objects within the oil field, then it will call upon the StudentWorld object to remove dirt and make a sound
   
    getWorld()->removeDirt(getX(), getY());

    // Otherwise, the doSomething() method must check to see if the human player pressed a key,and move
    int key;
    
    if (getWorld()->getKey(key)==true) {
        switch (key) {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
                
            case KEY_PRESS_SPACE:
                
                if (m_waterGun!=0) {
                    m_waterGun--;
                    
                    int nx=0;
                    int ny=0;

                    bool canFire=true;
                    switch (getDirection()) {
                        case GraphObject::up:
                            nx=getX(); ny=getY()+8;
                            break;
                        case GraphObject::down:
                            nx=getX(); ny=getY()-4;
                            break;
                        case GraphObject::left:
                            nx=getX()-4; ny=getY();
                            break;
                        case GraphObject::right:
                            nx=getX()+8; ny=getY();
                            break;
                        default:
                            break;
                    }
                   
                    if(nx!=getX()){
                        for(int i=0;i<4;i++){
                           if(getWorld()->isSpotOccupied(nx,ny+i))
                               canFire=false;
                        }
                    }
                    if (ny!=getY()) {
                        for(int i=0;i<4;i++){
                            if(getWorld()->isSpotOccupied(nx+i,ny))
                                canFire=false;
                        }

                    }
                    if (canFire) {
                        dirToCoord(getDirection(),4,getX(),getY(), nx, ny);
                    
                        getWorld()->addActor(new Squirt(getWorld(),nx,ny,getDirection()));
                    }
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                }
                
                break;
            case KEY_PRESS_Z:
                if (m_sonorCharge>0) {
                    m_sonorCharge--;
                    getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
                    getWorld()->playSound(SOUND_SONAR);
                }
                break;
                
            case KEY_PRESS_UP:
            case KEY_PRESS_DOWN:
            case KEY_PRESS_LEFT:
            case KEY_PRESS_RIGHT:
                
                if (dirMap[key]!=getDirection())
                    setDirection(dirMap[key]);
                else
                    moveNStep(dirMap[key],1);
                
                break;
            case  KEY_PRESS_TAB:
                if (m_goldNuggets>0) {
                    getWorld()->addActor(new GoldNugget(getWorld(),getX(),getY(),true,true));
                    m_goldNuggets--;
                }
                
        }

    }

}

// Protester

Protester::Protester(StudentWorld* world,int hitPoints, unsigned int scoreBribe, int imageID):Agent(hitPoints,world,imageID,60,60,left,1.0,0){
    m_state=nonRest;
    m_mustLeave=false;
    m_score=scoreBribe;
    m_numSquaresToMoveInCurrentDirection=world->randInt(8, 60);
    int ticks=3-(world->getLevel())/4;
    m_ticksToNextMove=max(0,ticks);
    m_restCountdown=m_ticksToNextMove;
    m_shoutCount=15;
    m_isScored=false;
    m_turningCount=0;
}
void Protester::doSomething(){}

void Protester:: stuned(){
    setState(rest);
    int n=100-getWorld()->getLevel()*10;
    m_restCountdown=max(50, n);
}

bool  Protester::annoy(unsigned int amt){
    decHitPoints(amt);
    if (getHitPoints()<=0) {
        if (!mustLeave()) {
            setMustLeaveOilField();
//            getWorld()->increaseScore(100);
            getWorld()->playSound( SOUND_PROTESTER_GIVE_UP);
        }
    }
    else{
        stuned();
        getWorld()->playSound( SOUND_PROTESTER_ANNOYED);
    }
    return true;
}

void Protester:: setMustLeaveOilField(){
    m_mustLeave=true;
    setState(nonRest);
    
}

int Protester:: getScoreBribe()const{
    return m_score;
}
void Protester:: setNumSquaresToMoveInCurrentDirection(int n){
    m_numSquaresToMoveInCurrentDirection=n;
}
void Protester:: decNumSquare(){
     m_numSquaresToMoveInCurrentDirection--;
}
int Protester:: getNumSquare(){
    return m_numSquaresToMoveInCurrentDirection;
}

protesterState Protester:: getState() const{
    return m_state;
}
void Protester::  getBribed(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(getScoreBribe());
    setMustLeaveOilField();
}

void Protester:: setMoveDirection(int x, int y){
    if (x>getX()) {
        setDirection(right);
    }
    if (x<getX()) {
        setDirection(left);
    }
    if (y>getY()) {
        setDirection(up);
    }
    if (y<getY()) {
        setDirection(down);
    }
}


void Protester:: pathToLeave(){
    if (getX()==60 && getY()==60) {
        setDead();
        return;
    }
    
    queue<Points> pointsQueue;
    int record[61][61];
    for (int i=0; i<61; i++) {
        for (int j=0; j<61; j++) {
            record[i][j]=0;
        }
    }

    Points src;
    src.x=getX();
    src.y=getY();
    pointsQueue.push(src);
    record[getX()][getY()]=1;
    
    int r,c;
    while (!pointsQueue.empty()) {
        Points cur= pointsQueue.front();
        allPathToleave.push_back(cur);
        r=cur.x;
        c=cur.y;
        if (r==60&&c==60) {
            break;
        }
        pointsQueue.pop();
        if (r-1>=0 && r-1<=60 && getWorld()->canActorMoveTo(this,r-1,c)&& record[r-1][c]==0){
            Points pushin;
            pushin.x=r-1;
            pushin.y=c;
            pushin.nextIdex=allPathToleave.size()-1;
            pointsQueue.push(pushin);
            record[r-1][c]=1;
        }
        if (c+1>=0 && c+1<=60 && getWorld()->canActorMoveTo(this,r,c+1)&& record[r][c+1]==0){
            Points pushin;
            pushin.x=r;
            pushin.y=c+1;
            pushin.nextIdex=allPathToleave.size()-1;
            pointsQueue.push(pushin);
            record[r][c+1]=1;
        }
        if (r+1>=0 && r+1<=60 && getWorld()->canActorMoveTo(this,r+1,c)&& record[r+1][c]==0) {
            Points pushin;
            pushin.x=r+1;
            pushin.y=c;
            pushin.nextIdex=allPathToleave.size()-1;
            pointsQueue.push(pushin);
            record[r+1][c]=1;
        }
        if (c-1>=0 && c-1<=60 && getWorld()->canActorMoveTo(this,r,c-1)&& record[r][c-1]==0) {
            Points pushin;
            pushin.x=r;
            pushin.y=c-1;
            pushin.nextIdex=allPathToleave.size()-1;
            pointsQueue.push(pushin);
            record[r][c-1]=1;
        }
    
    }
    Points p=allPathToleave[allPathToleave.size()-1];
    while (!(p.x==getX() && p.y==getY())) {
        bestPathToleave.push(p);
        p=allPathToleave[p.nextIdex];
        
    }        
    
}

void Protester:: setState(protesterState state){
    m_state=state;
}

int Protester:: getRestTicksCountDown(){
    return m_restCountdown;
}
void Protester::resetTicksCounrDown(){
    m_restCountdown=m_ticksToNextMove;
}
void Protester::restTicksCountDown(){
    m_restCountdown--;
}

void Protester:: resetShoutCount(){
    m_shoutCount=0;
}
int Protester:: shoutCount(){
    return m_shoutCount++;
}

void Protester:: resetTurningCount(){
    m_turningCount=0;
}
int Protester:: turningCount(){
    return m_turningCount++;
}

bool Protester:: mustLeave(){
    return m_mustLeave;
}

bool Protester:: facingFrackman( Actor* frackman){
    int x=getX();
    int y=getY();
    
    if (frackman->getX()>= x && getDirection()==right){
        return true;
    }
    if (frackman->getX() <= x && getDirection()==left) {
        return true;
    }
    if (frackman->getY() >= y && getDirection()==up) {
        return true;
    }
    if (frackman->getY() <= y && getDirection()==down) {
        return true;
    }
   
    return false;
}


void Protester:: leaveOilField(){
    pathToLeave();
    if (bestPathToleave.size()!=0) {
    Points nextStep=bestPathToleave.top();
    bestPathToleave.pop();
    setMoveDirection(nextStep.x, nextStep.y);
    moveTo(nextStep.x, nextStep.y);
    }
}


void Protester:: moveDirectlyToFrackman(int disx,int disy){
    if (disx==0) {
        if (disy>0) {//up to frackman
            int i;
            for (i=disy-4; i>0; i--) {
                if(!getWorld()->canActorMoveTo(this, getX(), getY()-i))
                    break;
            }
            if (i==0) {
                setDirection(down);
                setNumSquaresToMoveInCurrentDirection(0);
                moveTo(getX(), getY()-1);
                setState(rest);
                resetTicksCounrDown();
            }
        }
        else{//down to frackman
            int j;
            for (j=-disy-4; j>0; j--) {
                if(!getWorld()->canActorMoveTo(this, getX(), getY()+j))
                    break;
            }
            if (j==0) {
                setDirection(up);
                setNumSquaresToMoveInCurrentDirection(0);
                moveTo(getX(), getY()+1);
                setState(rest);
                resetTicksCounrDown();
            }
        }
    }
    if (disy==0) {
        if (disx>0) {//right to frackman
            int i;
            for (i=disx-4; i>0; i--) {
                if(!getWorld()->canActorMoveTo(this, getX()-i, getY()))
                    break;
            }
            if (i==0) {
                setDirection(left);
                setNumSquaresToMoveInCurrentDirection(0);
                moveTo(getX()-1, getY());
                setState(rest);
                resetTicksCounrDown();
            }
        }
        else{//left to frackman
            int j;
            for (j=-disx-4; j>0; j--) {
                if(!getWorld()->canActorMoveTo(this, getX()+j, getY()))
                    break;
            }
            if (j==0) {
                setDirection(right);
                setNumSquaresToMoveInCurrentDirection(0);
                moveTo(getX()+1, getY());
                setState(rest);
                resetTicksCounrDown();
            }
        }
    }
    return;
}


// RegularProtester

RegularProtester:: RegularProtester(StudentWorld* world):Protester(world,5, 25, IID_PROTESTER){
}
void RegularProtester::doSomething(){
    
    if (!isAlive()) {
        return;
    }
    
    if (getState()==rest) {
        
        restTicksCountDown();
        if (getRestTicksCountDown()==0) {
            setState(nonRest);
            return;
        }
        
        return;
    }
    
    if (mustLeave()) {
        leaveOilField();
        setState(rest);
        resetTicksCounrDown();
        return;
    }

    
    //If frackman is nearby
    Actor* frackman=getWorld()->findNearbyFrackMan(this, 4);
    int disx,disy;
    if (frackman!=nullptr) {
        if (facingFrackman(frackman) && shoutCount()>14) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            frackman->annoy(2);
            resetShoutCount();
            return;
        }
    }
    //4 units away, Can see frackman directly
    else if (getWorld()->straightlySeeFrackman(this,disx,disy)){
        //move directly to frackman
        moveDirectlyToFrackman(disx,disy);
        return;
    }
    //cannot see frackman directly

    if(getNumSquare()<=0){
        bool canMove=false;
        Direction dirpre=getDirection();
        while (canMove==false) {
            int n=getWorld()->randInt(1,4);
            switch (n) {
                case 1: setDirection(up); break;
                case 2: setDirection(down); break;
                case 3: setDirection(left); break;
                case 4: setDirection(right); break;
                default: break;
            }
            int nx,ny;
            dirToCoord(getDirection(), 1, getX(), getY(), nx, ny);
            canMove=getWorld()->canActorMoveTo(this, nx, ny);
        }
        int newStepsNum=getWorld()->randInt(8,60);
        setNumSquaresToMoveInCurrentDirection(newStepsNum);
        if ((dirpre-2.5)*(getDirection()-2.5)<0) {//change direction
            resetTurningCount();
        }
    }
    //make a turn
    if (turningCount()>=201) {
        int nx[2],ny[2];
        bool canTurn[2]={false,false};
        if (getDirection()==left || getDirection()== right) {
            Direction turning[2]={up,down};
            dirToCoord(up, 1, getX(), getY(), nx[0], ny[0]);
            dirToCoord(down, 1, getX(), getY(), nx[1], ny[1]);
            canTurn[0]=getWorld()->canActorMoveTo(this, nx[0], ny[0]);
            canTurn[1]=getWorld()->canActorMoveTo(this, nx[1], ny[1]);
            if (canTurn[0] && !canTurn[1]) {
                setDirection(turning[0]);
            }
            if (canTurn[1] && !canTurn[0]) {
                setDirection(turning[1]);
            }
            if (canTurn[1] && canTurn[0]) {
                int i=getWorld()->randInt(0,1);
                setDirection(turning[i]);
            }
        }
        
        else if(getDirection()==up || getDirection()== down) {
            Direction turning[2]={left,right};
            dirToCoord(left, 1, getX(), getY(), nx[0], ny[0]);
            dirToCoord(right, 1, getX(), getY(), nx[1], ny[1]);
            canTurn[0]=getWorld()->canActorMoveTo(this, nx[0], ny[0]);
            canTurn[1]=getWorld()->canActorMoveTo(this, nx[1], ny[1]);
            if (canTurn[0] && !canTurn[1]) {
                setDirection(turning[0]);
            }
            if (canTurn[1] && !canTurn[0]) {
                setDirection(turning[1]);
            }
            if (canTurn[1] && canTurn[0]) {
                int i=getWorld()->randInt(0,1);
                setDirection(turning[i]);
            }
        }
    
        if (canTurn[0]||canTurn[1]) {
            int newStepsNum=getWorld()->randInt(8,60);
            setNumSquaresToMoveInCurrentDirection(newStepsNum);
            resetTurningCount();
        }
    }
 
 //#8&9
    moveNStep(getDirection(), 1);
    int nextx,nexty;
    dirToCoord(getDirection(),1, getX(),getY(),nextx,nexty);
    if (getWorld()->canActorMoveTo(this, nextx,nexty)) {
         decNumSquare();
    }
    else{
        setNumSquaresToMoveInCurrentDirection(0);
    }
   
    setState(rest);
    resetTicksCounrDown();
}

//HardcoreProtester
HardcoreProtester:: HardcoreProtester(StudentWorld* world):Protester(world,20, 50, IID_HARD_CORE_PROTESTER){
}
void    HardcoreProtester::doSomething(){
    
    if (!isAlive()) {
        return;
    }
    
    if (getState()==rest) {
        
        restTicksCountDown();
        if (getRestTicksCountDown()==0) {
            setState(nonRest);
            return;
        }
        
        return;
    }
    
    if (mustLeave()) {
        leaveOilField();
        setState(rest);
        resetTicksCounrDown();
        return;
    }
    
    
    //If frackman is nearby
    Actor* frackman=getWorld()->findNearbyFrackMan(this, 4);
    int disx,disy;
    if (frackman!=nullptr) {
        if (facingFrackman(frackman) && shoutCount()>14) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            frackman->annoy(2);
            resetShoutCount();
            return;
        }
    }//dis<M
    else {
        Actor* frackman=getWorld()->findNearbyFrackMan(this, 100);
        int M=16+getWorld()->getLevel()*2;
        int nextx,nexty;
        if(findNextStepToFrackman(frackman,M,nextx,nexty)){
            setMoveDirection(nextx, nexty);
            moveTo(nextx, nexty);
            setState(rest);
            resetTicksCounrDown();
            return;
        }
//       findPathToFrackman(frackman);
//        if (bestPathToFrackman.size()<=M) {
//            if (bestPathToFrackman.size()!=0) {
//                Points nextStep=bestPathToFrackman.top();
//                bestPathToFrackman.pop();
//                setMoveDirection(nextStep.x, nextStep.y);
//                moveTo(nextStep.x, nextStep.y);
//                return;
//            }
//        }
    //4 units away, Can see frackman directly
        if (getWorld()->straightlySeeFrackman(this,disx,disy)){
        //move directly to frackman
        moveDirectlyToFrackman(disx,disy);
        return;
        }
    }
    //cannot see frackman directly
    
    if(getNumSquare()<=0){
        bool canMove=false;
        Direction dirpre=getDirection();
        while (canMove==false) {
            int n=getWorld()->randInt(1,4);
            switch (n) {
                case 1: setDirection(up); break;
                case 2: setDirection(down); break;
                case 3: setDirection(left); break;
                case 4: setDirection(right); break;
                default: break;
            }
            int nx,ny;
            dirToCoord(getDirection(), 1, getX(), getY(), nx, ny);
            canMove=getWorld()->canActorMoveTo(this, nx, ny);
        }
        int newStepsNum=getWorld()->randInt(8,60);
        setNumSquaresToMoveInCurrentDirection(newStepsNum);
        if ((dirpre-2.5)*(getDirection()-2.5)<0) {//change direction
            resetTurningCount();
        }
    }
    //make a turn
    if (turningCount()>=201) {
        int nx[2],ny[2];
        bool canTurn[2]={false,false};
        if (getDirection()==left || getDirection()== right) {
            Direction turning[2]={up,down};
            dirToCoord(up, 1, getX(), getY(), nx[0], ny[0]);
            dirToCoord(down, 1, getX(), getY(), nx[1], ny[1]);
            canTurn[0]=getWorld()->canActorMoveTo(this, nx[0], ny[0]);
            canTurn[1]=getWorld()->canActorMoveTo(this, nx[1], ny[1]);
            if (canTurn[0] && !canTurn[1]) {
                setDirection(turning[0]);
            }
            if (canTurn[1] && !canTurn[0]) {
                setDirection(turning[1]);
            }
            if (canTurn[1] && canTurn[0]) {
                int i=getWorld()->randInt(0,1);
                setDirection(turning[i]);
            }
        }
        
        else if(getDirection()==up || getDirection()== down) {
            Direction turning[2]={left,right};
            dirToCoord(left, 1, getX(), getY(), nx[0], ny[0]);
            dirToCoord(right, 1, getX(), getY(), nx[1], ny[1]);
            canTurn[0]=getWorld()->canActorMoveTo(this, nx[0], ny[0]);
            canTurn[1]=getWorld()->canActorMoveTo(this, nx[1], ny[1]);
            if (canTurn[0] && !canTurn[1]) {
                setDirection(turning[0]);
            }
            if (canTurn[1] && !canTurn[0]) {
                setDirection(turning[1]);
            }
            if (canTurn[1] && canTurn[0]) {
                int i=getWorld()->randInt(0,1);
                setDirection(turning[i]);
            }
        }
        
        if (canTurn[0]||canTurn[1]) {
            int newStepsNum=getWorld()->randInt(8,60);
            setNumSquaresToMoveInCurrentDirection(newStepsNum);
            resetTurningCount();
        }
    }
    
    //#8&9
    moveNStep(getDirection(), 1);
    int nextx,nexty;
    dirToCoord(getDirection(),1, getX(),getY(),nextx,nexty);
    if (getWorld()->canActorMoveTo(this, nextx,nexty)) {
        decNumSquare();
    }
    else{
        setNumSquaresToMoveInCurrentDirection(0);
    }
    
    setState(rest);
    resetTicksCounrDown();
}

bool HardcoreProtester:: findNextStepToFrackman(Actor*f, int M,int&x,int&y){
    
    queue<Points> pointsQueue;
    int record[61][61];
    for (int i=0; i<61; i++) {
        for (int j=0; j<61; j++) {
            record[i][j]=0;
        }
    }
    std::vector<Points> allPathtoFrackman;
    std::stack<Points> bestPathToFrackman;
    Points src;
    src.x=getX();
    src.y=getY();
    pointsQueue.push(src);
    record[getX()][getY()]=1;
    
    int r,c;
    while (!pointsQueue.empty()) {
        Points cur= pointsQueue.front();
       allPathtoFrackman.push_back(cur);
        r=cur.x;
        c=cur.y;
        if (r==f->getX()&&c==f->getY()) {
            break;
        }
        pointsQueue.pop();
        if (r-1>=0 && r-1<=60 && getWorld()->canActorMoveTo(this,r-1,c)&& record[r-1][c]==0){
            Points pushin;
            pushin.x=r-1;
            pushin.y=c;
            pushin.nextIdex=allPathtoFrackman.size()-1;
            pointsQueue.push(pushin);
            record[r-1][c]=1;
        }
        if (c+1>=0 && c+1<=60 && getWorld()->canActorMoveTo(this,r,c+1)&& record[r][c+1]==0){
            Points pushin;
            pushin.x=r;
            pushin.y=c+1;
            pushin.nextIdex=allPathtoFrackman.size()-1;
            pointsQueue.push(pushin);
            record[r][c+1]=1;
        }
        if (r+1>=0 && r+1<=60 && getWorld()->canActorMoveTo(this,r+1,c)&& record[r+1][c]==0) {
            Points pushin;
            pushin.x=r+1;
            pushin.y=c;
            pushin.nextIdex=allPathtoFrackman.size()-1;
            pointsQueue.push(pushin);
            record[r+1][c]=1;
        }
        if (c-1>=0 && c-1<=60 && getWorld()->canActorMoveTo(this,r,c-1)&& record[r][c-1]==0) {
            Points pushin;
            pushin.x=r;
            pushin.y=c-1;
            pushin.nextIdex=allPathtoFrackman.size()-1;
            pointsQueue.push(pushin);
            record[r][c-1]=1;
        }
        
    }
    Points p=allPathtoFrackman[allPathtoFrackman.size()-1];
    while (!(p.x==getX() && p.y==getY())) {
        bestPathToFrackman.push(p);
        p=allPathtoFrackman[p.nextIdex];
    }
    if (bestPathToFrackman.size()<=M && bestPathToFrackman.size()>0) {
        Points next=bestPathToFrackman.top();
        x=next.x;
        y=next.y;
        return true;
    }
    return false;
}

void HardcoreProtester:: getBribed(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(getScoreBribe());
    stuned();
}

bool HardcoreProtester:: annoy(unsigned int amt){
    decHitPoints(amt);
    if (getHitPoints()<=0) {
        if (!mustLeave()) {
            setMustLeaveOilField();
//            getWorld()->increaseScore(250);
            getWorld()->playSound( SOUND_PROTESTER_GIVE_UP);
        }
    }
    else{
        stuned();
        getWorld()->playSound( SOUND_PROTESTER_ANNOYED);
    }
    return true;
}

// ActivatingObject

ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
                                   bool initallyActivate): Actor(world, imageID, startX, startY,right, 1, 2){
    setVisible(initallyActivate);
    m_activate=initallyActivate;
}

void ActivatingObject::setTicksToLive(int ticks){
    m_ticksToLive=ticks;
}

bool ActivatingObject:: isActivate()const{
    return m_activate;
}
void ActivatingObject:: activate(){
    m_activate=true;
}

// OilBarrel

OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY):ActivatingObject(world,startX,startY,IID_BARREL,false){
    
}

void OilBarrel:: doSomething(){
    if (!isAlive()) {
        return;
    }
   
    if (!isActivate()) {
        Actor* frackMan=getWorld()->findNearbyFrackMan(this, 4);
        if (frackMan!=nullptr) {
            setVisible(true);
            activate();
            return;
        }
    }
    else{
        Actor* frackMan=getWorld()->findNearbyFrackMan(this, 3);
        if (frackMan!=nullptr) {
            setDead();
            getWorld()->playSound(SOUND_FOUND_OIL);
            getWorld()->increaseScore(1000);
            getWorld()->decBerral();
        }
    }
    return;
}

bool OilBarrel:: isBuried() const{
    return true;
}

//GoldNugget

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temporary,bool visible):ActivatingObject(world,startX,startY,IID_GOLD,visible){
    m_istemporary=temporary;
    setTicksToLive(100);
    //m_tempTicks=100;
}

void GoldNugget:: doSomething(){
    if (!isAlive()) {
        return;
    }
    if (!isActivate() && m_istemporary==false) {
        Actor* frackMan=getWorld()->findNearbyFrackMan(this, 4);
        if (frackMan!=nullptr) {
            setVisible(true);
            activate();
            return;
        }
    }
    if (isActivate()&&  m_istemporary==false){
        Actor* frackMan=getWorld()->findNearbyFrackMan(this, 3);
        if (frackMan!=nullptr) {
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->increaseScore(10);
            frackMan->addGold();
        }
    }
    if (m_istemporary==true){
        if (getTickstoLive()==0) {
            setDead();
            return;
        }
        liveCountDown();
        getWorld()->bribeProtester(this);
        
    }
    return;
    

}

//water pool

WaterPool:: WaterPool(StudentWorld* world, int startX, int startY,int ticksToLive):ActivatingObject(world,startX,startY,IID_WATER_POOL,true){
    setTicksToLive(ticksToLive);
    activate();
}

void WaterPool::doSomething(){
    if (!isAlive()) {
        return;
    }
    if (getTickstoLive()>0) {
        FrackMan* f=getWorld()->findNearbyFrackMan(this, 3);
        if (f!=nullptr) {
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            f->addWater();
            f->getWorld()->increaseScore(100);
        }
        else{
            liveCountDown();
        }
    }else{
        setDead();
    
    }
    return;
}

SonarKit::SonarKit(StudentWorld* world,int ticksToLive):ActivatingObject(world,0,60,IID_SONAR,true){
    setTicksToLive(ticksToLive);
    activate();
}
void SonarKit::doSomething(){
    if (!isAlive()) {
        return;
    }
    if (getTickstoLive()>0) {
        FrackMan* f=getWorld()->findNearbyFrackMan(this, 3);
        if (f!=nullptr) {
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            f->addSonar();
            f->getWorld()->increaseScore(75);
        }
        else{
            liveCountDown();
        }
    }else{
        setDead();
        
    }
    return;
}


