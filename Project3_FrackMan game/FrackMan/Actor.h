#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <map>
#include <vector>
#include <stack>
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
class StudentWorld;

enum BoulderState {stable,waiting,falling};

class Actor : public GraphObject
{
public:
 
    Actor(StudentWorld* world, int imageID, int startX, int startY,Direction dir = right, double size = 1.0, unsigned int depth = 0);
    
    virtual ~Actor(){
    }
    
    
    virtual void doSomething()=0;
    
    //virtual
    virtual void addGold(){}
    virtual bool mustLeave(){return false;}
    virtual void setScored(){}
    virtual bool Scored(){return false;}
    //virtual int getScoreBribe()const{return 0;}
    virtual void getBribed(){}
    
    StudentWorld* getWorld();//get current world
    
    bool isAlive() const;// Is this actor alive
    
    void setDead();//Make actor dead
    
    //virtual int getHitPoints(){return 200;};
    
    virtual void moveNStep(Direction dir, int n);
    void dirToCoord(Direction dir, int dis,int x, int y, int& nx, int& ny);
    
    virtual bool annoy(unsigned int amt){
        return false;
    }
    
      // Can this actor pick items up? *Signal of agent!
    virtual bool canPickThingsUp() const{
        return false;
    }
    
    // Can this actor dig through dirt? *Signal of FrackMan!
    virtual bool canDigThroughDirt() const{
        return false;
    }
    // Does this actor hunt the FrackMan? *Signal of Protester!
    virtual bool huntsFrackMan() const{
        return false;
    }
    
    // Can other actors pass through this actor? *Signal of Boulder!!
    virtual bool canActorsPassThroughMe() const{
        return true;
    }
    // Can this actor need to be picked up to finish the level?*Signal of OilBarrel也许没用！！
    virtual bool needsToBePickedUpToFinishLevel() const{
        return false;
    }
    // Is this actor buried in the field? *Signal of Nugget and Barrel
    virtual bool isBuried() const{
        return false;
    }
    virtual bool canSenseFrackman(){
        return false;
    }
 
private:
    StudentWorld* m_world;
    bool m_alive;
 
};


class Dirt: public Actor
{
public:
    Dirt(StudentWorld* world, int startX, int startY);
    virtual~Dirt(){}
    virtual void doSomething(){}
};

class Squirt: public Actor{
public:
    Squirt(StudentWorld* world, int startX, int startY,Direction dir);
    virtual~Squirt(){}
    virtual void doSomething();
private:
    int travelDist;
    
};

class Boulder : public Actor
{
public:
    Boulder(StudentWorld* world, int startX, int startY);
    virtual ~Boulder(){}
    virtual void doSomething();
    virtual bool canActorsPassThroughMe() const;
    
private:
    BoulderState m_state;
    int waitCountDown;
};

class Agent: public Actor
{
public:
    
    Agent(int hitPoints, StudentWorld* world, int imageID, int startX, int startY,Direction dir = right, double size = 1.0, unsigned int depth = 0);
    virtual ~Agent(){}
    
    virtual int getHitPoints();
    virtual void decHitPoints(int amt);
    
    // Annoy this actor
    virtual bool annoy(unsigned int amt);
    virtual void setMustLeaveOilField(){}
    virtual bool mustLeave(){
        return false;
    }
    
private:
    int m_hitPonts;

};


class FrackMan: public Agent
{
public:
    
    FrackMan(StudentWorld* world);
    
    virtual ~FrackMan(){
        
        dirMap.erase(dirMap.begin(), dirMap.end());
    }
    
    virtual bool canDigThroughDirt() const;
    virtual bool canPickThingsUp() const;
    
    
    //Pick up gold
    virtual void addGold();
    
    // Pick up a sonar kit.
    void addSonar();
    
    // Pick up water.
    void addWater();
    
    // Get amount of gold
    unsigned int getGold() const;
    
    // Get amount of sonar charges
    unsigned int getSonar() const;
    
    // Get amount of water
    unsigned int getWater() const;
    //Annoy frackman
    virtual bool annoy(unsigned int amt);

    
    virtual void doSomething();
    
    std::map <int,Direction> dirMap;// a map of Direction and key value, all constant values

private:
    int m_waterGun;
    int m_sonorCharge;
    int m_goldNuggets;
    
    
};

enum protesterState{rest,nonRest};

//for finding the shortest way to leave
struct Points
{
    int x;
    int y;
    unsigned long nextIdex;
};


class Protester : public Agent
{
    
public:
    Protester(StudentWorld* world,int hitPoints, unsigned int scoreBribe, int imageID);
    virtual ~Protester(){}
    virtual void doSomething();
    
    virtual bool huntsFrackMan() const{
        return true;
    }
    
    // Set state to having given up protest
    virtual void setMustLeaveOilField();
    virtual bool mustLeave();
    
    int getScoreBribe()const;
    
    //num of squares move in same direction
    void setNumSquaresToMoveInCurrentDirection(int n);
    int getNumSquare();
    void decNumSquare();
    
    
    //get STATE
    protesterState getState() const;
    //set State
    void setState(protesterState state);
    
    //getTicksToNextMove
    int  getRestTicksCountDown();
    void restTicksCountDown();
    void resetTicksCounrDown();
    void stuned();
    
    //Coutd for shouting
    void resetShoutCount();
    int shoutCount();
    
    //Coutd for turning
    void resetTurningCount();
    int turningCount();
    
    
    //move direction
    void setMoveDirection(int disx, int disy);
    //leave oil field
    void leaveOilField();
    
    // face Frackman or not
    bool facingFrackman(Actor* frackman);
    //move directly to Frackman
    void moveDirectlyToFrackman(int x, int y);
    //annoy protester
     virtual bool annoy(unsigned int amt);
    //bribed
    virtual void getBribed();
    
    //scored
    virtual void setScored(){
        m_isScored=true;
    }
    virtual bool Scored(){return m_isScored;}

    
private:
    int m_score;
    bool m_mustLeave;
    protesterState m_state;
    int m_numSquaresToMoveInCurrentDirection;//[8,60]
    int m_ticksToNextMove;
    int m_restCountdown;
    int m_shoutCount;
    int m_turningCount;
    bool m_isScored;
    
    std::vector<Points> allPathToleave;
    std::stack<Points> bestPathToleave;
    void pathToLeave();
};

class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world);
    virtual ~RegularProtester(){}
    virtual void doSomething();
   
   // virtual void addGold();
};

class HardcoreProtester : public Protester
{
public:
    HardcoreProtester(StudentWorld* world);
    virtual ~HardcoreProtester(){}
    virtual void doSomething();
     virtual void getBribed();
    virtual  bool annoy(unsigned int amt);
    virtual bool canSenseFrackman(){
        return true;
    }
    // virtual void addGold();
    bool findNextStepToFrackman(Actor*f, int M,int&x,int&y);
private:
//    std::vector<Points> allPathtoFrackman;
//    std::stack<Points> bestPathToFrackman;
};

class ActivatingObject: public Actor
{
public:
    ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
                     bool initallyActivate);
    virtual void doSomething(){}
    // Set number of ticks until this object dies
    virtual ~ActivatingObject(){}
    void setTicksToLive(int ticks);
    void liveCountDown(){
        m_ticksToLive--;
    }
    int getTickstoLive(){
        return m_ticksToLive;
    }
    
    bool isActivate() const;
    void activate();
    
private:
    int m_ticksToLive;
    bool m_activate;
 
};

class OilBarrel: public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY);
    virtual ~OilBarrel(){}
    virtual void doSomething();
    virtual bool isBuried()const;

    
};

class GoldNugget : public ActivatingObject
{
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool temporary,bool visible);
    ~GoldNugget(){}
    virtual void doSomething();
private:
    bool m_istemporary;
    //int m_tempTicks;
};

class WaterPool : public ActivatingObject
{
public:
    WaterPool(StudentWorld* world, int startX, int startY,int ticksToLive);
    virtual void doSomething();
};

class SonarKit : public ActivatingObject
{
public:
    SonarKit(StudentWorld* world,int ticksToLive);
    virtual void doSomething();
};



