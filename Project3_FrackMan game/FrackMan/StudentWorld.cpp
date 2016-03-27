
#include "StudentWorld.h"
#include <string>
#include "Actor.h"
#include "GameController.h"
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld:: StudentWorld(std::string assetDir): GameWorld(assetDir)
{
 
    
    for (int i=0; i<60; i++) {
        for (int j=0; j<60; j++) {
            m_dirt[i][j]=nullptr;
        }
        
    }
   
}

StudentWorld::~StudentWorld(){

    delete m_frackMan;
    
    vector<Actor*>::iterator it;
    for(it=m_actors.begin();it!=m_actors.end(); it++)
    {
        delete *it;
    }
    m_actors.erase(m_actors.begin(),m_actors.end());
    
    for (int i=0; i<64; i++) {
        for (int j=0; j<60; j++) {
            delete m_dirt[i][j];
        }
        
    }
}

int StudentWorld::randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}


bool StudentWorld:: withDirt(int x, int y)const{
    
    if (m_dirt[x][y]!=nullptr) {
        return true;
    }
    return false;
    
}
bool StudentWorld:: withBoulder(int x, int y) const{
    
    for (int i=0; i<m_actors.size(); i++) {
        
        if (!m_actors[i]->canActorsPassThroughMe()) {//Boulder!
            
            int dx=x-m_actors[i]->getX();
            int dy=y-m_actors[i]->getY();
            if (dx>=0 && dx<=4 && dy>=0 && dy <=4) {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld:: withinRadius(Actor *a, int radius, int x, int y) const{//within a's radius

    int cx=a->getX();
    int cy=a->getY();
    
    if ((x-cx)*(x-cx)+(y-cy)*(y-cy) <= radius*radius) {
        return true;
    }
    return  false;
}
//if a frackman is in radius;
FrackMan* StudentWorld:: findNearbyFrackMan(Actor* a, int radius) const{
    if (withinRadius(a, radius, m_frackMan->getX(), m_frackMan->getY())) {
        return m_frackMan;
    }
    return nullptr;
}

// Add an actor to the world.
void StudentWorld:: addActor(Actor* a){
    m_actors.push_back(a);
}

// Can actor move to x,y?

bool StudentWorld:: canActorMoveTo(Actor *a, int x, int y){//destination x,y
    
    int size=a->getSize();
    
    //in the field
    if (x<0 || x>(64-4*size) || y<0 || y>(64-4*size)){
        return false;
    }
    
    //encounter boulder
    //a->findEdge(a->getDirection(),x, y);
    
    for (int i=0; i<m_actors.size(); i++) {
        
        if (!m_actors[i]->canActorsPassThroughMe()) {//Boulder!
            if (a!=m_actors[i]) {//not a same Boulder!
                if (withinRadius(m_actors[i],3,x,y))
                    return false;
            }
            
        }
    }
    
    // dirt?
    if (!a->canDigThroughDirt()) {
        bool overlapDirt=false;
        int nx,ny;
        for (int i=0; i<4; i++) {
            a->dirToCoord(GraphObject::up, i, x, y, x, ny);
            for (int j=0; j<4; j++) {
                a->dirToCoord(GraphObject::right, j, x, ny, nx, ny);
                if (withDirt(nx, ny))
                    overlapDirt=true;
            }
        }
        return (!overlapDirt);
    }
    
    return true;
    
}


bool StudentWorld:: isSpotOccupied(int x, int y) const{//dirt or boulder
    if (withDirt(x,y)) {
        return true;
    }
    if (withBoulder(x, y)) {
        return true;
    }
    return false;

}

int StudentWorld:: annoyAllNearbyActors(Actor* annoyer, int points, int radius){
    int res=0;
    
    if (withinRadius(annoyer, radius, m_frackMan->getX(),m_frackMan->getY())) {
        m_frackMan->annoy(points);
        res++;
    }
    for (int i=0; i<m_actors.size(); i++) {

         if (m_actors[i]->huntsFrackMan()) {//Protester!!
             
             int x=m_actors[i]->getX();
             int y=m_actors[i]->getY();
             
             if (withinRadius(annoyer, radius, x, y)) {
                 
                 m_actors[i]->annoy(points);
                 if (m_actors[i]->mustLeave() && !m_actors[i]->Scored()) {
                     increaseScore(500);
                     m_actors[i]->setScored();
                 }
                 res++;
             }
             
         }
  
     }
  
    return res;
}

void StudentWorld:: removeDirt(int x, int y){
    int i;
    bool removed=false;
    GraphObject::Direction dir=m_frackMan->getDirection();
    switch (dir) {
        case GraphObject::up :
             for (i=0; i<4; i++) {
                 if (withDirt(x+i,y+3)) {
                     delete m_dirt[x+i][y+3];
                     m_dirt[x+i][y+3]=nullptr;
                     removed=true;
                 }
             }

            break;
        case GraphObject::down :
            for (i=0; i<4; i++) {
                if (withDirt(x+i,y)) {
                    delete m_dirt[x+i][y];
                    m_dirt[x+i][y]=nullptr;
                    removed=true;
                }
            }

            break;
        case GraphObject::left :
            for (i=0; i<4; i++) {
                if (withDirt(x,y+i)) {
                    delete m_dirt[x][y+i];
                    m_dirt[x][y+i]=nullptr;
                    removed=true;
                }
            }

            break;
        case GraphObject::right :
            for (i=0; i<4; i++) {
                if (withDirt(x+3, y+i)) {
                    delete m_dirt[x+3][y+i];
                    m_dirt[x+3][y+i]=nullptr;
                    removed=true;
                }
            }
            break;
        default:
            break;
    }
    
    if (removed)
        playSound(SOUND_DIG);
  
}

int StudentWorld:: annoyProtesters(Actor* annoyer, int points, int radius){
    int res=0;
    for (int i=0; i<m_actors.size(); i++) {
        
        if (m_actors[i]->huntsFrackMan()) {//protester!!
            int x=m_actors[i]->getX();
            int y=m_actors[i]->getY();
            
            if ( withinRadius(annoyer, radius, x, y)) {
                m_actors[i]->annoy(points);
                if (m_actors[i]->mustLeave() && !m_actors[i]->Scored()) {

                    if (m_actors[i]->canSenseFrackman())
                        increaseScore(250);
                    else{
                        increaseScore(100);
                    }
                    m_actors[i]->setScored();
                    
                }
                res++;
            }
            
        }
    }
    return res;
}

//void StudentWorld::setNumberOfProtester(int T){
//    m_numOfProtester=T;
//
//}

int StudentWorld::init(){

    // initial frackman
    m_frackMan=new FrackMan(this);
    
    //Dirt
    int i,j;
    for (i=0; i<30; i++) {
        for (j=0; j<60; j++) {
            m_dirt[i][j]=new Dirt(this,i,j);
        }
        
    }
    for (i=30; i<34; i++) {
        for (j=0; j<4; j++) {
            m_dirt[i][j]=new Dirt(this,i,j);
        }
    }
    for (i=34; i<64; i++) {
        for (j=0; j<60; j++) {
            m_dirt[i][j]=new Dirt(this,i,j);
        }
    }
    //protester
    addActor(new RegularProtester(this));
    m_numOfProtester=1;
    m_numOfHardProtester=0;
    //calculate ticks intervals to add protester
    int currT=200-getLevel();
    int T=min(25,currT);
    m_ticksToAddProtester=T;
    m_numOfProtester=1;
    m_numOfHardProtester=0;
    
    
    //Boulder
//    for (int i=0; i<15; i++) {
//        addActor(new Boulder(this,4*i,4*i));
//    }
    
    int level=getLevel();
    int B = min(level + 2, 6);
    addBoulder(B);
    
    //Oil
    //add Oil
    int L= min(2 + level, 20);
    setInitBerral(L);
    addBerral(L);
    
    //Gold
    int G = max(5-level/ 2, 2);
    addGoldNugget(G);
    
    
    return GWSTATUS_CONTINUE_GAME;
    
}


int StudentWorld::move()
{
    //setDisplatText
    updateDisplayText();
    
    //calculate ticks intervals to add protester
    int currT=200-getLevel();
    int T=min(25,currT);
    //calculate number of protester
    int currP= 2+getLevel()*1.5;
    int P=min(15,currP);
    
    //add new actor
    addNewProtesters(P,T);
    addNewGoodie();

    //ask FrackMan object to do something
        m_frackMan->doSomething();
    
    // ask players to do something
    for (int i=0; i<m_actors.size(); i++) {
        
        if (m_actors[i]->isAlive())
             m_actors[i]->doSomething();
        //If an actor does something that causes the FrackMan to give up
        if (!m_frackMan->isAlive()){
            decLives();
            playSound(SOUND_PLAYER_GIVE_UP);
            return GWSTATUS_PLAYER_DIED;
        }
        //if (theplayerCompletedTheCurrentLevel() == true) {
        if (getBerral()==0)
            return GWSTATUS_FINISHED_LEVEL;
    }
  
    
    //remove dead Actors
    for (int i=0; i<m_actors.size(); i++) {
        if(!m_actors[i]->isAlive()){
            if (m_actors[i]->huntsFrackMan()) {
                m_numOfProtester--;
            }
            delete m_actors[i];
            m_actors.erase(m_actors.begin()+i);
        }
        
    }
    // return the proper result
    if (!m_frackMan->isAlive()){
        decLives();
         playSound(SOUND_PLAYER_GIVE_UP);
        
        return GWSTATUS_PLAYER_DIED;
    }
    if (getBerral()==0)
        return GWSTATUS_FINISHED_LEVEL;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld:: cleanUp(){
    
    delete m_frackMan;
    
    vector<Actor*>::iterator it;
    for(it=m_actors.begin();it!=m_actors.end(); it++)
    {
        delete *it;
    }
    m_actors.erase(m_actors.begin(),m_actors.end());
    
    for (int i=0; i<64; i++) {
        for (int j=0; j<60; j++) {
            delete m_dirt[i][j];
        }
        
    }
}

//add new actors;
void StudentWorld:: addNewProtesters(int P, int T){
    if (m_ticksToAddProtester<=0 && m_numOfProtester<P ) {
        int n=getLevel()*10+30;
        int probabilityOfHardcore = min(90,n);
        int option=randInt(0,probabilityOfHardcore);
        if (option!=0){
            addActor(new HardcoreProtester(this));
            m_numOfHardProtester++;
        }
        else{
            addActor(new RegularProtester(this));
        }
        m_ticksToAddProtester=T;
        m_numOfProtester++;
    
    }
    else
        m_ticksToAddProtester--;
    return;

}

void StudentWorld::addBoulder(int nBoulder){

    while (nBoulder > 0)
    {
        bool canAdd=true;
        int x= randInt(0,60);
        int y= randInt(20,56);

        if (x>=26 && x<=33)
            continue;
        
        for (int i=0; i<m_actors.size(); i++) {
            if ( !m_actors[i]->canActorsPassThroughMe() || m_actors[i]->isBuried()) {
                if (withinRadius(m_actors[i], 6, x, y)) {
                    canAdd=false;
                }
            }
        }

        //to be added: distance btw 2 objects!!
        //if...
        if (canAdd) {
            addActor(new Boulder(this,x,y));
            clearDirt(x,y);
            nBoulder--;
        }
       
    }

}

void StudentWorld::addBerral(int nBerral){
    
    while (nBerral > 0)
    {
        bool canAdd=true;
        int x= randInt(0,60);
        int y= randInt(20,56);
        
        if (x>=26 && x<=33)
            continue;
        
        for (int i=0; i<m_actors.size(); i++) {
            if ( !m_actors[i]->canActorsPassThroughMe() || m_actors[i]->isBuried()) {
                if (withinRadius(m_actors[i], 6, x, y)) {
                    canAdd=false;
                }
            }
        }
        if (canAdd) {
            addActor(new OilBarrel(this,x,y));
            nBerral--;
        }
       
    }
    
}

void StudentWorld::addGoldNugget(int nGold){
    while (nGold > 0)
    {
        bool canAdd=true;
        int x= randInt(0,60);
        int y= randInt(20,56);
        
        if (x>=26 && x<=33)
            continue;
        
        for (int i=0; i<m_actors.size(); i++) {
            if ( !m_actors[i]->canActorsPassThroughMe() || m_actors[i]->isBuried()) {
                if (withinRadius(m_actors[i], 6, x, y)) {
                    canAdd=false;
                }
            }
        }
        if (canAdd) {
            addActor(new GoldNugget(this,x,y,false,false));
            nGold--;
        }
        
    }
    
}

void StudentWorld:: clearDirt(int x, int y){
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            
            delete m_dirt[x+i][y+j];
            m_dirt[x+i][y+j]=nullptr;
        
        }
    }
}

void StudentWorld::updateDisplayText(){
    
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_frackMan->getHitPoints()*10;
    int squirts =m_frackMan->getWater();
    int gold =m_frackMan->getGold();
    int sonar =m_frackMan->getSonar();
    int barrelsLeft = getBerral();
    string s=formatText(score, level, lives, health, squirts, gold,sonar, barrelsLeft);
    setGameStatText(s);
    
}

string StudentWorld::formatText(int score, int level, int lives, int health, int squirts, int gold,int sonar, int barrelsLeft){
    string output="Scr: ";
    ostringstream s; s.fill('0');
    s<<setw(6)<<score;
    output += s.str();output +="  Lvl: ";
    ostringstream lev; lev.fill(' ');
    lev<<setw(2)<<level;
    output += lev.str();output += "  Lives: ";
    ostringstream liv; //liv.fill(' ');
    liv<<setw(1)<<lives;
    
    output += liv.str();output+="  Hlth: ";
    ostringstream h; h.fill(' ');
    h<<setw(2)<<health;
    output += h.str(); output+="%  Wtr: ";
    ostringstream wtr; wtr.fill(' ');
    wtr<<setw(2)<<squirts;
    output+=wtr.str(); output+="  Gld: ";
    ostringstream gld; gld.fill(' ');
    gld<<setw(2)<<gold;
    output += gld.str();output+="  Sonar:";
    ostringstream snr; snr.fill(' ');
    snr<<setw(2)<<sonar;
    output += snr.str();output+="  Oil Left:";
    ostringstream oil; oil.fill(' ');
    oil<<setw(2)<<barrelsLeft;
    output += oil.str();
    return output;
}

int StudentWorld::getBerral(){
    return m_berral;
}
void StudentWorld::decBerral(){
    m_berral--;
}
void StudentWorld::setInitBerral(int num){
    m_berral=num;
    
}


bool StudentWorld:: straightlySeeFrackman(Actor* a,int &disx, int &disy){
//    if (a->getX()==m_frackMan->getX() || a->getY()==m_frackMan->getY()) {
//        disy=a->getY()-m_frackMan->getY();//a-frackman
//        disx=a->getX()-m_frackMan->getX();
//        return true;
//    }
    if (a->getY()==m_frackMan->getY()) {
        int x1=min(a->getX(),m_frackMan->getX());
        int x2=max(a->getX(),m_frackMan->getX());
        for (int x=x1+1; x<x2; x++) {
            for (int i=0; i<4; i++) {
                if(isSpotOccupied(x,a->getY()+i))
                    return false;
            }
        }
        disy=a->getY()-m_frackMan->getY();//a-frackman
        disx=a->getX()-m_frackMan->getX();
        return true;
    }
    if (a->getX()==m_frackMan->getX()) {
        int y1=min(a->getY(),m_frackMan->getY());
        int y2=max(a->getY(),m_frackMan->getY());
        for (int y=y1+1; y<y2; y++) {
            for (int i=0; i<4; i++) {
                if(isSpotOccupied(a->getX()+i,y))
                    return false;
            }
        }
    disy=a->getY()-m_frackMan->getY();//a-frackman
    disx=a->getX()-m_frackMan->getX();
    return true;
    }
    return false;

}

void StudentWorld:: bribeProtester(Actor* gold){
    for (int i=0; i<m_actors.size(); i++) {
        
        if (m_actors[i]->huntsFrackMan()) {//protester!!
            int x=m_actors[i]->getX();
            int y=m_actors[i]->getY();
            
            if ( withinRadius(gold, 3, x, y)) {
                gold->setDead();
                playSound(SOUND_PROTESTER_FOUND_GOLD);
                m_actors[i]->getBribed();
                //increaseScore(m_actors[i]->getScoreBribe());
                return;
            }
            
        }
    }
    return;
}

void StudentWorld:: addNewGoodie(){
    int T=300-10*getLevel();
    int ticksToLive=max(100,T);
    int G=getLevel()* 25 + 300;
    int whichGoodie=randInt(1, 5*G);
    if (whichGoodie>1 &&whichGoodie<=5) {
        while (true) {
            bool overLapDirt=false;
            int x=randInt(0,60);
            int y=randInt(0,60);
            for (int i=0; i<4; i++) {
                for (int j=0; j<4; j++) {
                    if (withDirt(x+i, y+j)) {
                        overLapDirt=true;
                    }
                }
            }
            if (!overLapDirt) {
                addActor(new WaterPool(this, x, y, ticksToLive));
                break;
            }
        }
        
    
  }
    if (whichGoodie==1) {
        addActor(new SonarKit(this,ticksToLive));
    }
 
}

// Reveal all objects within radius of x,y.
void StudentWorld:: revealAllNearbyObjects(int x, int y, int radius){
    if(withinRadius(m_frackMan,x, y,radius)){
        m_frackMan->setVisible(true);
    }
}

