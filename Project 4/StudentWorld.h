#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <algorithm>        // to use max( , )

///////////////////////////////////////////////////////////////////////////
//  StudentWorld Class
///////////////////////////////////////////////////////////////////////////
 
class StudentWorld : public GameWorld
{
public:
    // constructor
    StudentWorld(std::string assetDir) : GameWorld(assetDir), m_tunnelman(NULL), m_earth(NULL), m_actors(NULL), m_nbarrels(0), m_tick(0) { }
    
    // virtual destructor
    virtual ~StudentWorld() { cleanUp(); }
    // init function
    virtual int init();
    // move function
    virtual int move();
    void addNewActors();
    // cleanup function
    virtual void cleanUp();

    // actor accessor functions
    TunnelMan* getTunnelMan()                       { return m_tunnelman; }
    std::vector<Actor*> getActors()                 { return m_actors; }
    
    
    // used to access individual blocks of earth without accessor errors
    Earth* getEarth(int x, int y) {
        if ((x>=0 && x<=63) && (y>=0 && y<=59))
            return m_earth[x][y];
        else
            return NULL;
    }

    // accessor functions
    void resetTick()                        { m_tick = 0; }
    void addTick()                          { ++m_tick; }
    int getTick()                           { return m_tick; }
    void decTicksBeforeAddProtester()       { --m_ticksBeforeAddProtester; }
    void decTargetNumProtetsers()           { --m_targetNumProtetsers; }
    int getTicksBeforeAddProtester()        { return m_ticksBeforeAddProtester; }
    int getTargetNumProtetsers()            { return m_targetNumProtetsers; }
    void resetTicksBeforeAddProtester();    
    // helper functions
    std::string formatGameText(std::string &s, int lvl, int lives, int hlth, int wtr,
                                int gld, int oil_left, int sonar, int scr);
    bool removeEarthHelper(int x, int y);
    void getValidXY(int &x, int &y);
    void getValidWaterXY(int &x, int &y);
    void getValidBoulderXY(int &x, int &y);
    double calcDistance(double x1, double y1, double x2, double y2);
    void removeDeadGameObjects();
    bool compareBoulderHelper(int x, int y);
    void collectBarrel()        { --m_nbarrels; }
    void setDisplayText();
    bool isBoulder(int x, int y);             // returns true if the given coordinate contains a boulder
    bool isEarth(int x, int y);               // returns true if the 4x4 square with bottom-left (x,y) contains earth
    bool withinRadiusBoulder(int x, int y);   // returns true if given (x,y) is within a 3.0 radius of any boulder
    void illuminate(int x, int y);            // sets visible all game objects within a radius of 12.00 of tm
    Protester* getClosestProtester(int x, int y);   // returns a pointer to a protester within 3.0 radius or NULL
    GraphObject::Direction getDirectionExit(int x, int y) { return m_protesterpathtoexit[x][y]; }
    GraphObject::Direction getDirectionTM(int x, int y)   { return m_protesterpathtotm[x][y]; }
    // creates a grid of directions towards goal position, given the goal
    void generateProtesterPath(GraphObject::Direction map[][VIEW_HEIGHT], int startX, int startY);
    bool withinRadiusTunnelMan(double radius, int x, int y);
    bool annoyProtestersWithinRadius(int annoy, int x, int y);
    bool isValidMove(GraphObject::Direction dir , int x, int y);
    bool isValidCoord(int x, int y);        // returns if given coord overlaps earth/boulders
    void addActor(Actor* actor)                   { m_actors.push_back(actor); }
    int getDistanceToTunnelMan(int x, int y); // returns the shortest distance from hardcore protester to tunnelman, or -1 if there is no clear path

private:
    struct XY {
         XY(int x, int y): m_x(x), m_y(y) {}
         int getx(){return m_x;}
         int gety(){return m_y;}
         int m_x,m_y;
     };
    TunnelMan *m_tunnelman;                         // pointer to tunnelman
    std::vector<std::vector<Earth*>> m_earth;       // 2D vector of earth objects
    std::vector<Actor*> m_actors;                   // vector of every actor object
    int m_nbarrels;                                 // number of oil barrels left in the level
    
    // 2D grid (array) of possible protester paths to exit
    GraphObject::Direction m_protesterpathtoexit[VIEW_WIDTH][VIEW_HEIGHT];
    // 2D grid (array) of possible protester paths to tunnelman
    GraphObject::Direction m_protesterpathtotm[VIEW_WIDTH][VIEW_HEIGHT];
    int m_tick;
    int m_ticksBeforeAddProtester;                  // decrements until 0 to signal to add new
    int m_targetNumProtetsers;
};

#endif // STUDENTWORLD_H_
