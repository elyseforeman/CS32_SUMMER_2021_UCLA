#include "Actor.h"
#include "StudentWorld.h"
#include <vector>
#include <algorithm>        // to use max( , )
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////
//  Actor implementation
///////////////////////////////////////////////////////////////////////////
double Actor::calcDistance(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

///////////////////////////////////////////////////////////////////////////
//  TunnelMan implementation
///////////////////////////////////////////////////////////////////////////

void TunnelMan::doSomething()
{
    if (!isAlive())
        return;
    // If the player hits a directional key during the current tick, update TunnelMan’s location to the target square
    int key;
    if (getStudentWorld()->getKey(key))
    {
        switch (key)
        {
            case KEY_PRESS_LEFT:
                // CHECK IF COORDINATE IS VALID (x=0,y=0 to x=60,y=60,inclusive, not a boulder)
                if (getDirection() == left && getX()-1 >= 0 &&
                    !getStudentWorld()->withinRadiusBoulder(getX()-1, getY()))
                    moveTo(getX()-1, getY());
                else
                    setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() == right && getX()+1 <= 60 &&
                    !getStudentWorld()->withinRadiusBoulder(getX()+1, getY()))
                    moveTo(getX()+1, getY());
                else
                    setDirection(right);
                break;
            case KEY_PRESS_UP:
                if (getDirection() == up && getY()+1 <= 60 &&
                    !getStudentWorld()->withinRadiusBoulder(getX(), getY()+1))
                    moveTo(getX(), getY()+1);
                else
                    setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() == down && getY()-1 >= 0 &&
                    !getStudentWorld()->withinRadiusBoulder(getX(), getY()-1))
                    moveTo(getX(), getY()-1);
                else
                    setDirection(down);
                break;
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_SPACE:
                // fire a squirt
                if (m_squirts > 0)
                {
                    --m_squirts;
                    getStudentWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    fireSquirt(getDirection(), getX(), getY());
                }
                break;
            case 'Z':
            case 'z':
                // use sonar charge
                if (m_scharge > 0)
                {
                    --m_scharge;
                    getStudentWorld()->illuminate(getX(), getY());
                }
                break;
            case KEY_PRESS_TAB:
                // drop gold
                if (m_gold > 0)
                {
                    --m_gold;
                    GoldNugget *ptr = new GoldNugget(getStudentWorld(), getX(), getY(), true, false, true, false);
                    getStudentWorld()->addActor(ptr);
                }
                break;
        }
        if (getStudentWorld()->removeEarthHelper(getX(), getY()))
            getStudentWorld()->playSound(SOUND_DIG);
    }
    return; 
}

void Actor::fireSquirt(Direction dir, int x, int y)
{
    switch (dir)
    {
        case left:
        {
            // if the starting coordinate isn't occupied by earth or within a radius of 3.0 of a Boulder
            if (x-4 >= 0 && !getStudentWorld()->isEarth(x-4, y) &&
                !getStudentWorld()->withinRadiusBoulder(x-4,y))
            {
                Squirt *ptr = new Squirt(getStudentWorld(), getX()-4, getY(), left);
                getStudentWorld()->addActor(ptr);
                cout << "squirted to the left!" << endl;
            }
            break;
        }
        case right:
            if (x+4 <= 60 && !getStudentWorld()->isEarth(x+4, y) &&
                !getStudentWorld()->withinRadiusBoulder(x+4,y))
            {
                Squirt *ptr = new Squirt(getStudentWorld(), getX()+4, getY(), right);
                getStudentWorld()->addActor(ptr);
                cout << "squirted to the right!" << endl;
            }
            break;
        case down:
            if (y-4 >= 0 && !getStudentWorld()->isEarth(x, y-4) &&
                !getStudentWorld()->withinRadiusBoulder(x, y-4))
            {
                Squirt *ptr = new Squirt(getStudentWorld(), getX(), getY()-4, down);
                getStudentWorld()->addActor(ptr);
                cout << "squirted down!" << endl;
            }
            break;
        case up:
            if (y+4 <= 57 && !getStudentWorld()->isEarth(x, y+4) &&
                !getStudentWorld()->withinRadiusBoulder(x, y+4))
            {
                Squirt *ptr = new Squirt(getStudentWorld(), getX(), getY()+4, up);
                getStudentWorld()->addActor(ptr);
                cout << "squirted up!" << endl;
            }
            break;
        case none:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////
//  Boulder implementation
///////////////////////////////////////////////////////////////////////////

void Boulder::doSomething()
{
    addTick(); 
    if (!isAlive())
        return;
    // If the Boulder is currently in the stable state
    if (isStable())
    {
        // check 4 squares immediately below boulder
        // if there is no earth under boulder, set boulder to waiting
        int count = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (getStudentWorld()->getEarth(getX()+i,getY()-1) != NULL &&
                !getStudentWorld()->getEarth(getX()+i,getY()-1)->isAlive())
                ++count;

        }
        if (count == 4)
        {
            setWaiting();
            resetTick();
        }
    }
    // If the boulder is in a waiting state and 30 ticks have elapsed, transition into falling
    if (isWaiting())
    {
        if (getTick() >= 30)
        {
            setFalling();
            getStudentWorld()->playSound(SOUND_FALLING_ROCK);
        }
    }
    if (isFalling())
    {
        moveTo(getX(), getY()-1);
        if (getY() == 0)
        {
            setDead();
            return;
        }
        for (int i = 0; i < 4; ++i)
        {
            if (getStudentWorld()->getEarth(getX()+i,getY()-1) != NULL &&
                getStudentWorld()->getEarth(getX()+i,getY()-1)->isAlive())
            {
                setDead();
                return;
            }
        }
        if (getStudentWorld()->compareBoulderHelper(getX(),getY()))
        {
            setDead();
            return;
        }
        if (calcDistance(getX(), getY(),
        getStudentWorld()->getTunnelMan()->getX(),
        getStudentWorld()->getTunnelMan()->getY()) <= 3.00)
        {
            getStudentWorld()->getTunnelMan()->beAnnoyed(10);
            getStudentWorld()->getTunnelMan()->setDead();
        }
        // fall on (annoy) any protester within a radius of 3.00, then continue falling
        getStudentWorld()->annoyProtestersWithinRadius(10, getX(), getY());
    }
    return;
}

///////////////////////////////////////////////////////////////////////////
//  GoldNugget implementation
///////////////////////////////////////////////////////////////////////////

void GoldNugget::doSomething()
{
    // If not currently alive, then GoldNugget's doSomething() must return immediately
    if(!isAlive())
        return;
    ++m_tick;
    // if not currently visible AND the TunnelMan is within a radius of 4.0 of it (<= 4.00 units)
                        // calculate the distance between TunnelMan and the GoldNugget object
    if (!isVisible() && calcDistance(getX(), getY(),
                        getStudentWorld()->getTunnelMan()->getX(),
                        getStudentWorld()->getTunnelMan()->getY()) <= 4.00)
    {
        // gold nugget must make itself visible
        setVisibility(true);
        // doSomething() must return
        return;
    }
    // if is pickup-able by the TunnelMan AND the TunnelMan is within a radius of 3.0 (<= 3.00 units), then the goldNugget will activate and:
    if (isPickupableTM()
             && calcDistance(getX(), getY(),
                            getStudentWorld()->getTunnelMan()->getX(),
                            getStudentWorld()->getTunnelMan()->getY()) <= 3.00)
    {
        setDead();
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        getStudentWorld()->increaseScore(10);
        getStudentWorld()->getTunnelMan()->collectGold();
    }
    // if is pickup-able by Protestors AND a Protestor is within a radius of 3.0 (<= 3.00 units), then the goldNugget will activate and:
    if (isPickupableP() && getStudentWorld()->getClosestProtester(getX(), getY()) != NULL)
    {
        // set gold dead
        setDead();
        // SET PROTESTOR TO BRIBED ////
        getStudentWorld()->getClosestProtester(getX(), getY())->gotGold();
    }
    // if temporary, check if tick lifetime has elapsed
    if (!isPermanent())
    {
        if (m_tick >= 100)
            setDead();
    }
}

///////////////////////////////////////////////////////////////////////////
//  Sonar Kit implementation
///////////////////////////////////////////////////////////////////////////

void SonarKit::doSomething()
{
    ++m_tick;
    // check if currently alive
    if (!isAlive())
        return;
    // if the sonar kit is within a radius of 3.00 from TunnelMan, then it activates and...
    if (calcDistance(getX(), getY(), getStudentWorld()->getTunnelMan()->getX(), getStudentWorld()->getTunnelMan()->getY()) <= 3.00)
    {
        // set kit dead
        setDead();
        // play pick-up sound
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        // add a sonar charge to tunnelman's inventory
        getStudentWorld()->getTunnelMan()->collectSonarKit();
        // give tunnelman 75 points
        getStudentWorld()->increaseScore(75);
    }
    // Number of ticks T a Sonar Kit can exist: T = max(100, 300 – 10*current_level_number)
    // if its tick lifetime has elapsed, set its state to dead
    int level = getStudentWorld()->getLevel();
    int maxTick = max(100, 300 - 10*level);
    if (m_tick > maxTick)
        setDead();
}

///////////////////////////////////////////////////////////////////////////
//  Water Pool implementation
///////////////////////////////////////////////////////////////////////////

void WaterPool::doSomething()
{
    ++m_tick;
    // check if currently alive
    if (!isAlive())
        return;
    if (calcDistance(getX(), getY(), getStudentWorld()->getTunnelMan()->getX(), getStudentWorld()->getTunnelMan()->getY()) <= 3.00)
    {
        // set water pool dead
        setDead();
        // play pick-up sound
        getStudentWorld()->playSound(SOUND_GOT_GOODIE);
        // add 5 squirts of water to tunnelman's inventory
        getStudentWorld()->getTunnelMan()->collectSquirt();
        // increase player's score by 100 points
        getStudentWorld()->increaseScore(100);
    }
    // if tick lifetime elapsed, set dead
    int level = getStudentWorld()->getLevel();
    int maxTick = max(100, 300 - 10*level);
    if (m_tick > maxTick)
        setDead();
}

///////////////////////////////////////////////////////////////////////////
//  Squirt implementation
///////////////////////////////////////////////////////////////////////////

void Squirt::doSomething()
{
    cout << "squirt doSomething" << endl;
    // step 1 : if squirt is within radius of one or more protesters, annoy them by 2
    if(getStudentWorld()->annoyProtestersWithinRadius(2, getX(), getY()))
        setDead();
    // step 2 : if squirt travelled full distance, set dead
    if (m_dist <= 0)
        setDead();
    // step 3 + 4 : if squirt can move in current direction, move; else, setDead()
    if (getStudentWorld()->isValidMove(getDirection(), getX(), getY()))
    {
        moveOne(getDirection());
        // decrement m_dist to only travel this distance (4)
        --m_dist;
    }
    else
        setDead();
}
// if there is no dirt or boulder in the way, move to the next coordinate in current direction
void Squirt::moveOne(GraphObject::Direction dir)
{
    switch(dir)
    {
        case left:
            if (getStudentWorld()->isValidCoord(getX()-1, getY()))
                moveTo(getX()-1, getY());
            break;
        case right:
            if (getStudentWorld()->isValidCoord(getX()+1, getY()))
                moveTo(getX()+1, getY());
            break;
        case down:
            if (getStudentWorld()->isValidCoord(getX(), getY()-1))
                moveTo(getX(), getY()-1);
            break;
        case up:
            if (getStudentWorld()->isValidCoord(getX(), getY()+1))
                moveTo(getX(), getY()+1);
            break;
        case none:
            break;
    }
}
///////////////////////////////////////////////////////////////////////////
//  Barrel implementation
///////////////////////////////////////////////////////////////////////////

void Barrel::doSomething()
{
    if (!isAlive())
        return;
    // set visible when within tunnelman's radius
    if (!isVisible() && calcDistance(getX(), getY(),
                        getStudentWorld()->getTunnelMan()->getX(),
                        getStudentWorld()->getTunnelMan()->getY()) <= 4.00)
    {
        setVisibility(true);
        return;
    }
    if (calcDistance(getX(), getY(), getStudentWorld()->getTunnelMan()->getX(),
                     getStudentWorld()->getTunnelMan()->getY()) <= 3.00)
    {
        setDead();
        getStudentWorld()->playSound(SOUND_FOUND_OIL);
        getStudentWorld()->increaseScore(1000);
        getStudentWorld()->collectBarrel();
    }
}

///////////////////////////////////////////////////////////////////////////
//  Protester implementation
///////////////////////////////////////////////////////////////////////////

Protester::Protester(StudentWorld *sw, int imageID)
: Actor(sw, imageID, 56, 60, left, 1.0, 0), m_leavingfield(false),
m_numSquaresToMove(rand()%(53)+8), m_hit(0), m_restingtick(0),m_ticksSinceLastShout(0), m_ticksSincePerpTurn(0)
{ setVisible(true); int l = getStudentWorld()->getLevel(); m_ticksToWaitBetweenMoves = max(0, 3-l/4); }

void Protester::beStunned()
{
    // this will reset m_ticksToWaitBetweenMoves so the protester waits longer before next action
    m_ticksToWaitBetweenMoves = max(50, 100-int(getStudentWorld()->getLevel()*10));
}
void Protester::beAnnoyed(int amount)
{
    // can't be further annoyed while leaving field
    if (isLeavingField())
        return;
    // decrement hit-points by appropriate amount
    cerr << "original m_hit: " << m_hit << endl;
    m_hit -= amount;
    cerr << "after getting hit: " << m_hit << endl;

    // if hit, get stunned
    if (m_hit > 0)
    {
        getStudentWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        // be stunned (will extend resting state to N ticks)
        beStunned();
    }
    else
    {
        // if annoyed enough (m_hit/health < 0), begin leaving field
        setLeavingField();
        getStudentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        // reset resting tick to ensure activity next tick
        m_ticksToWaitBetweenMoves = 0;
        // if boulder annoyed it (amount = 10), increase player score by 500
        if (amount == 10)
            getStudentWorld()->increaseScore(500);
        else
            getStudentWorld()->increaseScore(100);
    }
}

void Protester::doSomething()
{
    // step 1 : return if dead, maintain timers
    if (!isAlive())
        return;
    
    // resting countdown
    // return while protester is resting (or stunned/admiring gold)
    if (m_ticksToWaitBetweenMoves > 0)
    {
        cerr<<"i'm resting"<<endl;
        --m_ticksToWaitBetweenMoves;          // decrement resting countdown
        return;
    }
    else
    {
        // reset ticksToWaitBetweenMoves
        int level = getStudentWorld()->getLevel();
        if (!isLeavingField())
            m_ticksToWaitBetweenMoves = max(0, (int)3-level/4);
        // increment these tick counters during "non-resting" ticks
        ++m_ticksSinceLastShout;
        ++m_ticksSincePerpTurn;
    }

    // step 3 : if isLeaving, either leave (set dead) or make a move towards exit
    if (isLeavingField())
    {
        // if the protester is at the exit, leave (setDead)
        if (getX()==60 && getY()==60)
        {
            setDead();
            return;
        }
        // else, move towards exit
        else
        {
            // function gives direction to move, accessed through map of optimal path generated inside move()
            GraphObject::Direction dir = getStudentWorld()->getDirectionExit(getX(), getY());
            moveOne(dir);
            return;
        }
    }

    // step 4 : shout/annoy when within radius and facing direction of tunnelman
    if (getStudentWorld()->withinRadiusTunnelMan(4.00, getX(), getY()) &&
        isFacingTunnelMan() && m_ticksSinceLastShout > 15 )
    {
        getStudentWorld()->playSound(SOUND_PROTESTER_YELL);            // protester yells
        getStudentWorld()->getTunnelMan()->beAnnoyed(2);               // annoy tunnelman
        m_ticksSinceLastShout = 0;                                     // reset shouting tick count
        return;
    }
    
    // step 5 HARDCORE PROTESTER SENSES TUNNELMAN
    if (isHard())
    {
        // compute M (distance in which hardcore protester can sense tunnelman)
        int M = 16 + (int)getStudentWorld()->getLevel() * 2;
        // If the Hardcore Protester is less than or equal to a total of M legal
        // horizontal or vertical moves away from the current location of the
        // TunnelMan (calculated using recursive function that calls the maze-solving function)
        if (getStudentWorld()->getDistanceToTunnelMan(getX(), getY()) != -1
            && getStudentWorld()->getDistanceToTunnelMan(getX(), getY()) <= M)
        {
            // set direction to move one square closer to tunnelman
            setDirection(getStudentWorld()->getDirectionTM(getX(), getY()));
            // move one square in this direction
            moveOne(getDirection());
            return;
        }
    }
    // step 5 : (both hard and regular) if protester is not within radius of the tunnelman, and is in a clear vertical or horizontal line from the tunnelamn, move towards player
    if (!getStudentWorld()->withinRadiusTunnelMan(4.00, getX(), getY()) && getClearPathDirection() != none)
    {
        cout << "vertical or horizontal from tunnelman" << endl;
        setDirection(getClearPathDirection());
        moveOne(getDirection());
        m_numSquaresToMove = 0;
        return;
    }
    
    // step 6: Protester can’t directly see the TunnelMan -> decrement numSquares // set new random valid direction
    --m_numSquaresToMove;
    if (m_numSquaresToMove <= 0)
    {
        // pick a random new direction to move
        setDirection(generateRandomDirection(getX(), getY()));
        // pick a new value for numSquaresToMoveInCurrentDirection
        cout << "is turning in random direction: " << getDirection() << endl;
        m_numSquaresToMove = rand()%53+8;
    }
    
    // step 7: if protester is at an intersection where it could turn and move one square perpendicularly and the regular protester hasn't made a perpendicular turn in the last 200 non-resting ticks, make a perp turn and move
    if (getPerpTurn() != GraphObject::none && m_ticksSincePerpTurn > 200)
    {
        cout << "turning perp!" << endl;
        cout << "direction i'm turning: " << getPerpTurn() << endl;
        setDirection(getPerpTurn());
        m_numSquaresToMove = rand()%53+8;
        m_ticksSincePerpTurn = 0;
        return;
    }
    
    // step 8 : (continuation of step 6 OR step 7, move one in whatever direction protester is facing
    moveOne(getDirection());
    
    // step 9: if regular protester is blocked from stepping in current direction, set m_num... to 0
    GraphObject::Direction dir = getDirection();
    switch (dir)
    {
        case left:
            if (!getStudentWorld()->isValidCoord(getX()-1, getY()))
                m_ticksSincePerpTurn = 0;
            break;
        case right:
            if (!getStudentWorld()->isValidCoord(getX()+1, getY()))
                m_ticksSincePerpTurn = 0;
            break;
        case down:
            if (!getStudentWorld()->isValidCoord(getX(), getY()-1))
                m_ticksSincePerpTurn = 0;
            break;
        case up:
            if (!getStudentWorld()->isValidCoord(getX(), getY()+1))
                m_ticksSincePerpTurn = 0;
            break;
        case none:
            break;
    }
        
}

GraphObject::Direction Protester::getClearPathDirection()
{
    int count = 0;
    // if protester is in a vertical line of sight to tunnelman...
    if ((getX() == getStudentWorld()->getTunnelMan()->getX()))
    {
        int distance = abs(getY() - getStudentWorld()->getTunnelMan()->getY());
        // check path DOWN (protester ABOVE tunnelman, turn DOWN)
        if (getY() > getStudentWorld()->getTunnelMan()->getY())
        {
            cerr << "protester ABOVE tunnelman, turn DOWN" << endl;
            for (int i = 0; i < distance; ++ i)
            {
                if (getStudentWorld()->isValidCoord(getX(), getY()-i))
                    ++count;
            }
            // if every unit of distance is valid, return down
            if (count == distance)
                return GraphObject::down;
        }
        // check path UP (protester BELOW tunnelman, turn UP)
        else
        {
            cout << "protester BELOW tunnelman, turn UP" << endl;
            for (int i = 0; i < distance; ++ i)
            {
                if (getStudentWorld()->isValidCoord(getX(), getY()+i))
                    ++count;
            }
            if (count == distance)
                return GraphObject::up;
        }
    }
    // if protester is in a horizontal line of sight to tunnelman...
    else if (getY() == getStudentWorld()->getTunnelMan()->getY())
    {
        int distance = abs(getX() - getStudentWorld()->getTunnelMan()->getX());
        // check path LEFT (protester TO THE RIGHT OF tunnelman, turn LEFT)
        if (getX() > getStudentWorld()->getTunnelMan()->getY())
        {
            cout << "protester TO THE RIGHT OF tunnelman, turn LEFT" << endl;
            for (int i = 0; i < distance; ++ i)
            {
                // if there are any visible earth blocks between p and tm, clearpath = false
                if(getStudentWorld()->isValidCoord(getX()-i, getY()))
                    ++count;
            }
            if (count == distance)
                return GraphObject::left;
        }
        // check path RIGHT (protester TO THE LEFT OF tunnelman, turn RIGHT)
        else
        {
            for (int i = 0; i < distance; ++ i)
            {
                if (getStudentWorld()->isValidCoord(getX()+i, getY()))
                    ++count;
            }
            if (count == distance)
                return GraphObject::right;
        }
    }
    return GraphObject::none;
}
bool Protester::isFacingTunnelMan()
{
    GraphObject::Direction dir = getDirection();
    return (dir == left && getStudentWorld()->getTunnelMan()->getX() <= getX()) ||
    // protester facing right, tunnelman must have a higher x
    (dir == right && getStudentWorld()->getTunnelMan()->getX() >= getX()) ||
    // protester facing down, tunnelman must have a lower y
    (dir == down && getStudentWorld()->getTunnelMan()->getY() <= getY()) ||
    // protester facing up, tunnelman must have a higher y
    (dir == up && getStudentWorld()->getTunnelMan()->getY() >= getY());
}
// checks if valid (no earth or boulder), then moves protester one in given direction
void Protester::moveOne(GraphObject::Direction dir)
{
    switch(dir)
    {
        case left:
            if (getStudentWorld()->isValidCoord(getX()-1, getY()))
                moveTo(getX()-1, getY());
            break;
        case right:
            if (getStudentWorld()->isValidCoord(getX()+1, getY()))
                moveTo(getX()+1, getY());
            break;
        case down:
            if (getStudentWorld()->isValidCoord(getX(), getY()-1))
                moveTo(getX(), getY()-1);
            break;
        case up:
            if (getStudentWorld()->isValidCoord(getX(), getY()+1))
                moveTo(getX(), getY()+1);
            break;
        case none:
            break;
    }
}
GraphObject::Direction Protester::getPerpTurn()
{
    GraphObject::Direction currDirection = getDirection();
    int x = getX();
    int y = getY();
    switch(currDirection)
    {
        case GraphObject::down:
        case GraphObject::up:
            if(getStudentWorld()->isValidCoord(x-1, y) && getStudentWorld()->isValidCoord(x+1, y))
            {
                // if both right and left are valid, choose one at random
               if (rand()%1<1)
                   return GraphObject::left;
               else
                   return GraphObject::right;
            }
            else if (getStudentWorld()->isValidCoord(x-1, y))
                return GraphObject::left;
            else if (getStudentWorld()->isValidCoord(x+1, y))
                return GraphObject::right;
            return GraphObject::none;
            break;
        case GraphObject::left:
        case GraphObject::right:
            if(getStudentWorld()->isValidCoord(x, y-1) && getStudentWorld()->isValidCoord(x, y+1))
            {
                // if both up and down are valid, choose one at random
               if (rand()%1<1)
                   return GraphObject::down;
               else
                   return GraphObject::up;
            }
            else if (getStudentWorld()->isValidCoord(x, y-1))
                return GraphObject::down;
            else if (getStudentWorld()->isValidCoord(x, y+1))
                return GraphObject::up;
            return GraphObject::none;
            break;
        case GraphObject::none:
            break;
    }
    return GraphObject::none;
}
void RegularProtester::gotGold()
{
    getStudentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getStudentWorld()->increaseScore(25);
    setLeavingField();
}
void HardcoreProtester::gotGold()
{
    getStudentWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getStudentWorld()->increaseScore(50);
    // pause to stare at nugget (extends m_tickstowaitbetweenmoves)
    beStunned();
}
// generate a random direction for the protester to move in when it can't directly see the tunnelman
GraphObject::Direction Actor::generateRandomDirection(int x, int y)
{
    cout << "generating a random direction" << endl;
    for (;;)
    {
        int random = (rand()%4)+1;
        switch (random)
        {
            case 1:
                if (getStudentWorld()->isValidCoord(x-1, y))
                    return left;
                break;
            case 2:
                if (getStudentWorld()->isValidCoord(x+1, y))
                    return right;
                break;
            case 3:
                if (getStudentWorld()->isValidCoord(x, y-1))
                    return down;
                break;
            case 4:
                if (getStudentWorld()->isValidCoord(x, y+1))
                    return up;
        }
    }
}

