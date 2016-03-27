#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class FrackMan;
class Dirt;

class StudentWorld : public GameWorld
{
public:
    
    StudentWorld(std::string assetDir);
    
    virtual ~StudentWorld();
    
    int   randInt(int min, int max);

  
    
    bool withinRadius(Actor *a, int radius, int x, int y) const;
    
    bool canActorMoveTo(Actor *a, int x, int y);
    
    bool withDirt(int x, int y) const;
    
    void removeDirt(int x, int y);
    
    virtual int init();

    virtual int move();

    virtual void cleanUp();
    
    // Berralleft
    void setInitBerral(int num);
    int getBerral();
    void decBerral();
   
    //Bribe the Protester
    void bribeProtester(Actor* gold);
    
    // If the FrackMan is within radius of a, return a pointer to the
    // FrackMan, otherwise null.
    FrackMan* findNearbyFrackMan(Actor* a, int radius) const;
    
    
    // Clear a 4x4 region of dirt.
    void clearDirt(int x, int y);
    
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // Annoy Protesters within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyProtesters(Actor* annoyer, int points, int radius);
    
    // Is the spot occupied by dirt and boulder?
    bool isSpotOccupied(int x, int y) const;
    
    // Annoy all other actors within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyAllNearbyActors(Actor* annoyer, int points, int radius);
    
    //frackman and protesters are in samea straight horizontal or vertical line
    bool straightlySeeFrackman(Actor* a,int& disx,int&disy);
    //set number of protester
   // void setNumberOfProtester(int P);
    
    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);
    
    
private:
    
    FrackMan* m_frackMan;
    std::vector<Actor*> m_actors;
    Dirt* m_dirt[64][64];
    int m_berral;
    int m_ticksToAddProtester;
    //int m_ticketsCountDown;
    int m_numOfProtester;
    int m_numOfHardProtester;
   
    
    bool withBoulder(int x, int y) const;
    void addBoulder(int nBoulder);
    void addBerral(int nBerral);
    void addGoldNugget(int nGold);
    void updateDisplayText();
    std::string formatText(int score, int level, int lives, int health, int squirts, int gold,int sonar, int barrelsLeft);
    void addNewProtesters(int P,int T);
    void addNewGoodie();
    
    
};

#endif // STUDENTWORLD_H_
