#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
class StudentWorld;


///////////////////////////////////////////////////////////////////////////
//  Actor Class
///////////////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
    // constructor
    Actor(StudentWorld *sw, int imageID, int startX, int startY,
          Direction startDirection, float size, unsigned int depth)
    : GraphObject(imageID, startX, startY, startDirection, size, depth),
    m_isAlive(true),
    m_studentworld(sw)
    { setVisible(true); }
    
    // virtual destructor
    virtual ~Actor() { }
    
    // pure virtual functions
    virtual void doSomething()=0;
    virtual void beAnnoyed(int n)=0;
    
    // accessor functions
    bool isAlive()                  { return m_isAlive; }
    StudentWorld* getStudentWorld() { return m_studentworld; }
    
    // mutator functions
    void setDead() { m_isAlive = false; setVisible(false); }
    
    // helper functions
    double calcDistance(double x1, double y1, double x2, double y2);
    Direction generateRandomDirection(int x, int y);
    void fireSquirt(Direction dir, int x, int y);

private:
    bool m_isAlive;                         // indicates if the actor is alive
    StudentWorld *m_studentworld;           // pointer locates the Actor
};

///////////////////////////////////////////////////////////////////////////
//  Earth Class
///////////////////////////////////////////////////////////////////////////

class Earth : public Actor
{
public:
    // constructor
    Earth(StudentWorld *sw, int x, int y)
    : Actor(sw, TID_EARTH, x, y, right, 0.25, 3) { }
    
    // virtual destructor
    virtual ~Earth() { }
    
    // accessor functions
    virtual void doSomething() { }
    virtual void beAnnoyed(int n) { }
    
private:
};

///////////////////////////////////////////////////////////////////////////
//  TunnelMan Class
///////////////////////////////////////////////////////////////////////////

class TunnelMan : public Actor
{
public:
    // constructor
    TunnelMan (StudentWorld *sw)
    : Actor(sw, TID_PLAYER, 30, 60, right, 1.0, 0),
    m_gold(0),
    m_scharge(1),
    m_squirts(5),
    m_hit(10)
    { }

    // virtual destructor
    virtual ~TunnelMan() { }
    
    // virtual functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { m_hit -= n; }

    // game play functions
    void collectGold()          { ++m_gold; }
    void collectSonarKit()      { ++m_scharge; }
    void collectSquirt()        { m_squirts+=5; }
    
    // accessor functions
    int getGold()               { return m_gold; }
    int getSonarCharge()        { return m_scharge; }
    int getSquirts()            { return m_squirts; }
    int getHealth()             { return m_hit*10; }
    
    // mutator functions
    
private:
    int m_gold;
    int m_scharge;
    int m_squirts;
    int m_hit;
};

///////////////////////////////////////////////////////////////////////////
//  Boulder Class
///////////////////////////////////////////////////////////////////////////

class Boulder : public Actor
{
public:
    // constructor
    Boulder(StudentWorld *sw, int startX, int startY)
    : Actor(sw, TID_BOULDER, startX, startY, down, 1.0, 1),
    m_stable(true),
    m_waiting(false),
    m_falling(false)
    { }
    
    // virtual destructor
    virtual ~Boulder() { }
    
    // virtual functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }

    // accessor functions
    bool isStable()     {return m_stable;}
    bool isWaiting()    {return m_waiting; }
    bool isFalling()    {return m_falling; }
    int getTick()       { return m_tick; }

    // mutator functions

    void addTick()      { ++m_tick; }
    void resetTick()    { m_tick = 0; }
    void setStable()    { m_stable = true; m_waiting = false; m_falling = false;}
    void setWaiting()   { m_stable = false; m_waiting = true; m_falling = false; }
    void setFalling()   { m_stable = false; m_waiting = false; m_falling = true; }
    
private:
    bool m_stable;
    bool m_waiting;
    bool m_falling;
    int  m_tick;
};

///////////////////////////////////////////////////////////////////////////
//  Protester Class
///////////////////////////////////////////////////////////////////////////

class Protester : public Actor
{
public:
    // constructor
    Protester(StudentWorld *sw, int imageID);
    
    // virtual destructor
    virtual ~Protester() { }
    
    // virtual functions
    virtual bool isHard()=0;
    virtual void gotGold()=0;
    void doSomething();
    void beAnnoyed(int n);
    
    // accessor functions
    bool isLeavingField()   { return m_leavingfield; }
    int getHealth()         { return m_hit*10; }

    // mutator functions
    
    // if the protester is leaving the field, set m_ticksToWaitBetweenMoves to 0 to activate movement
    void setLeavingField()  { m_leavingfield = true; m_ticksToWaitBetweenMoves = 0; }
    // if the protester is stunned or staring at gold, extend m_ticksToWaitBetweenMoves
    void beStunned();
    void setHit(int n)      { m_hit = n; }      // set different m_hit for reg/hard protester
    GraphObject::Direction getPerpTurn();       // returns a possible perpendicular direction to move in or NONE
    // helper functions
    bool canMovePerp(); 
    void moveOne(GraphObject::Direction dir);       // moves the protester one towards direction, if valid
    bool isFacingTunnelMan();
    GraphObject::Direction getClearPathDirection();

private:
    bool m_leavingfield;
    int m_hit;
    int m_restingtick;
    int m_ticksSinceLastShout;
    int m_numSquaresToMove;
    int m_ticksSincePerpTurn;
    int m_ticksToWaitBetweenMoves;
};

class RegularProtester : public Protester
{
public:
    // constructor
    RegularProtester(StudentWorld* sw) : Protester(sw, TID_PROTESTER) { setHit(5); }
    // virtual destructor
    virtual ~RegularProtester() {}
    
    // implementations of pure virtual functions
    virtual bool isHard() { return false; }
    virtual void gotGold();
private:
};

class HardcoreProtester : public Protester
{
public:
    // constructor
    HardcoreProtester(StudentWorld* sw) : Protester(sw, TID_HARD_CORE_PROTESTER) { setHit(20); }
    // virtual destructor
    virtual ~HardcoreProtester() {}
    
    // virtual functions
    virtual bool isHard() { return true; }

    virtual void gotGold();
    
private:
};

///////////////////////////////////////////////////////////////////////////
//  Squirt Class
///////////////////////////////////////////////////////////////////////////

class Squirt : public Actor
{
public:
    // constructor
    Squirt (StudentWorld *sw, int startX, int startY, Direction dir)
    : Actor(sw, TID_WATER_SPURT, startX, startY, dir, 1.0, 1),
    m_dist(4) { setVisible(true); }
    
    // virtual destructor
    virtual ~Squirt() { }
    
    // virtual functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }
    
    // helper functions
    void moveOne(GraphObject::Direction dir);       // checks if direction is valid, then moves
private:
    int m_dist;
};


///////////////////////////////////////////////////////////////////////////
//  Goodie Class
///////////////////////////////////////////////////////////////////////////

class Goodie : public Actor
{
public:
    // constructor
    Goodie(StudentWorld *sw, int imageID, int startX, int startY,
           Direction startDirection, unsigned int depth, bool visibility,
           bool pickupabiltyTM, bool pickupabiltyP, bool permanence)
    : Actor(sw, imageID, startX, startY, startDirection, 1.0, depth),
    m_pickupabilityTM(pickupabiltyTM),
    m_pickupabilityP(pickupabiltyP),
    m_permanence(permanence),
    m_visibility(visibility)
    { setVisibility(visibility); }
    
    // virtual destructor
    virtual ~Goodie() { }
    
    // virtual functions
    virtual void doSomething()=0;
    virtual void beAnnoyed(int n)=0;
    
    // accessor functions
    bool isVisible()        { return m_visibility; }
    bool isPickupableTM()   { return m_pickupabilityTM;}
    bool isPickupableP()    { return m_pickupabilityP;}
    bool isPermanent()      { return m_permanence; }
    
    // mutator functions
    void setTemporary()         { m_permanence = false; }
    void setPermanent()         { m_permanence = true; }
    void setVisibility(bool tf) { setVisible(tf); m_visibility = tf; }
    
private:
    bool m_pickupabilityTM;
    bool m_pickupabilityP;
    bool m_permanence;
    bool m_visibility;
};

class Barrel : public Goodie
{
public:
    // constructor
    Barrel(StudentWorld *sw, int startX, int startY)
    : Goodie(sw, TID_BARREL, startX, startY, right, 2, false, true, false, true) { }
    
    // virtual destructor
    virtual ~Barrel() { }
    
    // virtual void functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }

    // accessor functions
    // mutator functions
    
private:
};

class GoldNugget : public Goodie
{
public:
    // constructor
    GoldNugget(StudentWorld *sw, int startX, int startY, bool visibility, bool pickupabiltyTM, bool pickupabiltyP, bool permanence)
    : Goodie(sw, TID_GOLD, startX, startY, right, 2, visibility, pickupabiltyTM, pickupabiltyP, permanence),
    m_tick(0)
    { }
    
    // virtual destructor
    virtual ~GoldNugget() { }
    
    // virtual void functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }

    // accessor functions
    // mutator functions
    
private:
    int m_tick;
};

class SonarKit : public Goodie
{
public:
    // constructor
    SonarKit(StudentWorld *sw, int startX, int startY)
    : Goodie(sw, TID_SONAR, startX, startY, right, 2, true, true, false, false),
    m_tick(0)
    { }
    
    // virtual destructor
    virtual ~SonarKit() { }
        
    // virtual functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }

    // accessor functions
    // mutator functions
    
private:
    int m_tick;
};

class WaterPool : public Goodie
{
public:
    // constructor
    WaterPool(StudentWorld *sw, int startX, int startY)
    : Goodie(sw, TID_WATER_POOL, startX, startY, right, 2, true, true, false, false),
    m_tick(0)
    { }
    
    // virtual destructor
    virtual ~WaterPool() { }
        
    // virtual functions
    virtual void doSomething();
    virtual void beAnnoyed(int n) { }

    // accessor functions
    // mutator functions
    
private:
    int m_tick;
};

#endif // ACTOR_H_
