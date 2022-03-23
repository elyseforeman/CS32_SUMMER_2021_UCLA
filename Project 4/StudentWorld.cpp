#include "StudentWorld.h"
#include "GameController.h"
#include <iostream>
#include <algorithm>        // to use max( , )
#include <string>
#include <queue>            // for maze solver
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

///////////////////////////////////////////////////////////////////////////
//  StudentWorld implementation
///////////////////////////////////////////////////////////////////////////

void Ask_all_actors_to_do_something()
{
// for each actor on the level:
// if (the actor is still alive)
// tell the actor to doSomething();
}

///////////////////////////////////////////////////////////////////////////
//  INIT function
///////////////////////////////////////////////////////////////////////////

int StudentWorld::init()
{
// START GAME TICK AT 0 (determines when actors are added)
    m_tick = 0;
    
// INITIALIZE TUNNELMAN
    m_tunnelman = new TunnelMan(this);
    
// INITIALIZE TICKS
    int level = getLevel();
    m_ticksBeforeAddProtester = max(25, 200-level);
    m_targetNumProtetsers = min(15, (int)(2+level*1.5));
    
    // initialize 2D vector of earth objects
    vector<vector<Earth*>> earth;
    for (int x = 0; x <= 63; ++x)
    {
        // create temporary row vector to fill earth
        vector<Earth*> temp;
        for (int y = 0; y <= 59; ++y)
        {
            // place earth objects everywhere outside of mine shaft
            if ( x < 30 || x > 33 || y < 4 )
                temp.push_back(new Earth(this, x, y));
            else
                // fills mine shaft with null to prevent accessor (uninitialized locations) errors
                temp.push_back(NULL);
        }
        earth.push_back(temp);
    }
    m_earth = earth;
    
// INITIALIZE A 2D VECTOR OF EARTH OBJECTS

    vector<Actor*> actors;
    m_actors = actors;

// ADD ALL ACTORS TO OIL FIELD THROUGH ADDACTOR()
        
    // add all boulders to first vector in m_actors
    // int B = min(current_level_number / 2 + 2, 9)
    int num_bould = min((int)level/2+2, 9);
    for (int b = 0; b < num_bould; ++b)
    {
        int x,y;
        getValidBoulderXY(x, y);
        addActor(new Boulder(this, x, y));
    }
    // add all oil barrels
    // int L = min(2 + current_level_number, 21)
    int num_barrel = min(2+(int)level, 2);
    m_nbarrels = num_barrel;
    for (int a = 0; a < num_barrel; ++a)
    {
        int x, y;
        getValidXY(x,y);
        cout << "Barrel Coordinates: (" << x << ", " << y << ")" << endl;
        addActor(new Barrel(this, x, y));
    }
    // add all gold nuggets
    // int G = max(5-current_level_number / 2, 2)
    int num_nug = max(5-(int)level/2, 2);
    for (int g = 0; g < num_nug; ++g)
    {
        int x,y;
        getValidXY(x, y);
        addActor(new GoldNugget(this, x, y, false, true, true, true));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

///////////////////////////////////////////////////////////////////////////
//  MOVE function
///////////////////////////////////////////////////////////////////////////

// ADD NEW ACTORS DURING EACH TICK
void StudentWorld::addNewActors()
{
    int level = getLevel();
    int addGoodiesChance = level*25+300;
    bool addGoodie = (rand()%addGoodiesChance) < 1;
    int addHardcoreChance = min(90, level*10+30);
    bool addHardcore = (rand()%addHardcoreChance) < 1;
    decTicksBeforeAddProtester();                       // decrements a tick counter for num protesters
    
    // The first Protester must be added to the oil field
    // during the very first tick of each level
    if (m_tick == 1)
    {
        if (addHardcore)
            addActor(new HardcoreProtester(this));
        else
            addActor(new RegularProtester(this));
        // subtracts from target num of protesters until it hits 0
        decTargetNumProtetsers();
        resetTicksBeforeAddProtester();
    }
    // add new protesters after every ticksBeforeAddProtester ticks until hitting targetNumProtesters
    else if (getTicksBeforeAddProtester() <= 0 && getTargetNumProtetsers() > 0)
    {
        if (addHardcore)
            addActor(new HardcoreProtester(this));
        else
            addActor(new RegularProtester(this));
        decTargetNumProtetsers();
        resetTicksBeforeAddProtester();
    }
    // add new goodies with a 1/(level*25+300) ticks chance
    if (addGoodie)
    {
        // there is a 1/5 chance that you should add a new sonar kit
        if (rand()%5<1)
            addActor(new SonarKit(this, 0, 60));
        //there is a 4/5 chance that you should add a new Water Goodie
        if (rand()%5<4)
        {
            int x, y;
            getValidWaterXY(x, y);
            addActor(new WaterPool(this, x, y));
        }
    }
}
void StudentWorld::resetTicksBeforeAddProtester()
{
    int level = getLevel();
    m_ticksBeforeAddProtester = max(25, 200-level);
}

int StudentWorld::move()
{
    ++m_tick;

// UPDATE DISPLAY TEXT
    setDisplayText();
    
// ADD NEW ACTORS
    addNewActors();

// GIVE EACH ACTOR A CHANCE TO DO SOMETHING
    
    // ask tunnelman to do something
    m_tunnelman->doSomething();
    
    //  generate a map for the protester to use to get to exit 
    generateProtesterPath(m_protesterpathtoexit, 60, 60);
    // generate a map for a hardcore protester to use to get to tunnelman (set goal to tunnelman's coordinates)
    generateProtesterPath(m_protesterpathtotm, m_tunnelman->getX(), m_tunnelman->getY());
 
    // iterate through every actor, making them all do something
    vector<Actor*>::iterator actors = m_actors.begin();
    while(actors != m_actors.end())
    {
        if((*actors)->isAlive())
            (*actors)->doSomething();
        ++actors;
    }
    actors = m_actors.begin();
    
// REMOVE DEAD ACTORS AT THE END OF EACH TICK
    removeDeadGameObjects();
    
// RETURN ONE OF THREE GAMECONSTANTS VALUES
    if (!m_tunnelman->isAlive())
    {
        // decrement lives by 1
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    // if the player has gathered all of the barrels of oil, finish the level
    if (m_nbarrels == 0)
        return GWSTATUS_FINISHED_LEVEL;
    return GWSTATUS_CONTINUE_GAME;
}

///////////////////////////////////////////////////////////////////////////
//  CLEANUP function
///////////////////////////////////////////////////////////////////////////
void StudentWorld::cleanUp()
{
    // delete TunnelMan
    delete m_tunnelman;
    
    // delete all remaining Earth
    // iterate over every row of m_earth
    for (int i = 0; i < m_earth.size(); ++i)
    {
        // iterate over elements of each row
        vector<Earth*>::iterator it = m_earth[i].begin();
        while (it != m_earth[i].end())
        {
            Earth *ptr = *it;
            delete ptr;
            it = m_earth[i].erase(it);
        }
    }
    // delete all remaining actors within the 2D actors array
    vector<Actor*>::iterator actors;
    actors = m_actors.begin();
    while (actors != m_actors.end())
    {
        Actor *ptr = *actors;
        delete ptr;
        actors = m_actors.erase(actors);
    }
}

///////////////////////////////////////////////////////////////////////////
//  HELPER functions
///////////////////////////////////////////////////////////////////////////

void StudentWorld::removeDeadGameObjects()
{
    // iterate through actors array to delete actors
    vector<Actor*>::iterator actors = m_actors.begin();
    while(actors != m_actors.end())
    {
        // if an actor is dead, remove it from the vector of actors and delete it frmo game
        if(!(*actors)->isAlive())
        {
            Actor *ptr = *actors;
            delete ptr;
            actors = m_actors.erase(actors);
        }
        else
            ++actors;
    }
}

// called to remove earth during game play
bool StudentWorld::removeEarthHelper(int x, int y)
{

    bool dug = false;
    for (int i = 0; i < 4; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            // if the x,y coordinates are within bounds
            if ((x+i>=0 && x+i<64) && (y+k>=0 && y+k<60))
            {
                // check for access errors
                if (m_earth[x+i][y+k] != NULL && m_earth[x+i][y+k]->isAlive())
                {
                    m_earth[x+i][y+k]->setDead();
                    dug = true;
                }
            }
        }
    }
    return dug;
}

string StudentWorld::formatGameText(string &s, int lvl, int lives, int hlth, int wtr,
                                    int gld, int oil_left, int sonar, int scr)
{
    
    // Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000

    // Lvl field must be 2 digits long, with leading spaces
    // setting leading spaces
    ostringstream oss_lvl;
    oss_lvl.fill(' ');
    oss_lvl<<setw(2)<<lvl;
    s+= "Lvl: ";
    s+= oss_lvl.str();
    // each statistic must be separated from the last statistic by two spaces.
    s+= "  ";

    // Lives field should be 1 digit long
    s+= "Lives: ";
    s+= lives+'0';
    s+= "  ";
    
    // The Hlth field should be 3 digits long with leading spaces
    ostringstream oss_hlth;
    oss_hlth.fill(' ');
    oss_hlth<<setw(3)<<hlth;
    s+= "Hlth: ";
    s+= oss_hlth.str();
    s+= "%  ";
    
    // The Wtr field should be 2 digits long, with a leading space
    ostringstream oss_wtr;
    oss_wtr.fill(' ');
    oss_wtr<<setw(2)<<wtr;
    s+= "Wtr: ";
    s+= oss_wtr.str();
    s+= "  ";
    
    // The Gld field should be 2 digits long, with a leading space
    ostringstream oss_gld;
    oss_gld.fill(' ');
    oss_gld<<setw(2)<<gld;
    s+= "Gld: ";
    s+= oss_gld.str();
    s+= "  ";
    
    // The Oil Left field should be 2 digits long, with a leading space
    ostringstream oss_oil_left;
    oss_oil_left.fill(' ');
    oss_oil_left<<setw(2)<<oil_left;
    s+= "Oil Left: ";
    s+= oss_oil_left.str();
    s+= "  ";
    
    // The Sonar field should be 2 digits long, with a leading space
    ostringstream oss_sonar;
    oss_sonar.fill(' ');
    oss_sonar<<setw(2)<<sonar;
    s+= "Sonar: ";
    s+= oss_sonar.str();
    s+= "  ";
    
    // The Scr must be exactly 6 digits long, with leading zeros
    ostringstream oss_scr;
    oss_scr.fill('0');
    oss_scr<<setw(6)<<scr;
    s+= "Scr: ";
    s+= oss_scr.str();
    return s;
}
void StudentWorld::setDisplayText()
{
    // Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
    int lvl = getLevel();
    int lives = getLives();
    int hlth = m_tunnelman->getHealth();
    int wtr = m_tunnelman->getSquirts();
    int gld = m_tunnelman->getGold();
    int oil_left = m_nbarrels;
    int sonar = m_tunnelman->getSonarCharge();
    int scr = getScore();
    string s="";
    formatGameText(s, lvl, lives, hlth, wtr, gld, oil_left, sonar, scr);
    setGameStatText(s);
}
// enters a do:while loop to generate a random (x,y) -- used for gold and oil coordinate generateion
void StudentWorld::getValidXY(int &x, int &y)
{
    bool isValid=false;
    do {
        x = rand() % 60;
        y = rand() % 56;
        isValid = true;
        // check new (x,y) coordinates against every other actor
        vector<Actor*>::iterator it = m_actors.begin();
        while (it != m_actors.end())
        {
            if (calcDistance(x, y, (*it)->getX(), (*it)->getY()) <= 6.0)
                isValid = false;
            ++it;
        }
        // change range of tunnel to 26 to account for width of gold/oil (4 instead of 0.25)
        if (((26 <= x && x <= 33) && (1 <= y && y <= 59)))
            isValid = false;
    } while(!isValid);
    cout << "Gold/OIL Coordinates: (" << x << ", " << y << ")" << endl;
}
// enters a do:while loop to generate a random (x,y) -- used for boulder (must be above y=20)
void StudentWorld::getValidBoulderXY(int &x, int &y)
{
    bool isValid = false;
    do {
        x = rand() % 60;
        y = rand()%(56-20+1)+20;
        isValid = true;
        // check new (x,y) coordinates against every other actor
        // check new (x,y) coordinates against every other actor
        vector<Actor*>::iterator it = m_actors.begin();
        while (it != m_actors.end())
        {
            if (calcDistance(x, y, (*it)->getX(), (*it)->getY()) <= 6.0)
                isValid = false;
            ++it;
        }
        if (((26 <= x && x <= 33) && (1 <= y && y <= 59)))
            isValid = false;
    } while(!isValid);
    cout << "Boulder Coordinates: (" << x << ", " << y << ")" << endl;
    removeEarthHelper(x, y);
}
// returns the Euclidian Distance between two coordinates
double StudentWorld::calcDistance(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}
// enters a do:while loop to generate a random (x,y) -- used for water (must be where there's no earth)
void StudentWorld::getValidWaterXY(int &x, int &y)
{
    bool isValid=false;
    do {
        x = rand() % 60;
        y = rand() % 56;
        isValid = true;
        // check if the earth vector contains earth in the 4x4 square around (x,y)
        for (int i = 0; i < 4; ++i)
        {
            for (int k = 0; k < 4; ++ k)
            {
                if (getEarth(x+i, y+k) != NULL && getEarth(x+i, y+k)->isAlive())
                    isValid = false;
            }
        }
        // check if (x,y) is within radius of any other game object
        vector<Actor*>::iterator it = m_actors.begin();
        while (it != m_actors.end())
        {
            if (calcDistance(x, y, (*it)->getX(), (*it)->getY()) <= 6.0)
                isValid = false;
            ++it;
        }
    } while(!isValid);
    cout << "Water Coordinates: (" << x << ", " << y << ")" << endl;
}
// returns true if a falling boulder falls onto another boulder
bool StudentWorld::compareBoulderHelper(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
     {
    // use dynamic-cast to get all boulder actors
    // Child *p = dynamic_cast<Childe *>(pParent) asks if the pointer pParent can be type cast safely to type Child
    // returns the address of the object if possible, or returns 0 otherwise
         // asks if the iterator it can be type cast safely to the type boulder
         Boulder *boulder = dynamic_cast<Boulder*>(*it);
         // if the boulder has been safely downcast (does not return NULL or 0)
         if (boulder)
         {
             // compare every (x,y) on the bottom of boulder
             // to every (x,y) on the top of a boulder that might be beneath it
             for (int i = 0; i < 4; ++i)
             {
                 int curBoulderX = x+i;
                 int curBoulderY = y;
                 for (int k = 0; k < 4; ++k)
                 {
                     int botBoulderX = boulder->getX()+k;
                     int botBoulderY = boulder->getY()+3;
                     // return true if any point on the bottom of the boulder aligns with the top of another boulder
                     if (curBoulderX == botBoulderX
                         && curBoulderY == botBoulderY)
                         return true;
                 }
             }
         }
         ++it;
     }
    return false;
}
// returns true if the 4x4 square with bottom-left (x,y) contains boulder
bool StudentWorld::isBoulder(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
     {
         Boulder *boulder = dynamic_cast<Boulder*>(*it);
         if(boulder)
         {
             int boulderx = boulder->getX();
             int bouldery = boulder->getY();
             for (int i = 0; i < 4; ++i)
             {
                 // if the given (x,y) is one of the 4x4 units of the boulder, return true
                 for (int k = 0; k < 4; ++k)
                 {
                     if (x == boulderx+i && y == bouldery+k)
                         return true;
                 }
             }
         }
         ++it; 
     }
    return false;
}
// returns true if the 4x4 square with bottom-left (x,y) contains earth
bool StudentWorld::isEarth(int x, int y)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int k = 0; k < 4; ++k)
        {
            if (getEarth(x+i, y+k) != NULL && getEarth(x+i, y+k)->isAlive())
                return true;
        }
    }
    return false;
}
// returns true if given (x,y) is within a 3.0 radius of any boulder
bool StudentWorld::withinRadiusBoulder(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
     {
         Boulder *boulder = dynamic_cast<Boulder*>(*it);
         if(boulder)
         {
            if(calcDistance(x, y, boulder->getX(), boulder->getY()) < 3.00)
                return true;
         }
         ++it;
     }
    return false;
}
// returns a pointer to a protester within 3.0 radius or NULL
Protester* StudentWorld::getClosestProtester(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
     {
         Protester *protester = dynamic_cast<Protester*>(*it);
         if(protester)
         {
            if(calcDistance(x, y, protester->getX(), protester->getY()) < 3.00)
                return protester;
         }
         ++it;
     }
    return NULL;
}
// annoys all protesters "annoy" amount within a radius of 3.00
// returns whether or not it successfully annoyed any protesters (so other functions can setDead)
bool StudentWorld::annoyProtestersWithinRadius(int annoy, int x, int y)
{
    bool annoyedProtester = false;
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
     {
         Protester *protester = dynamic_cast<Protester*>(*it);
         if(protester)
         {
            if(calcDistance(x, y, protester->getX(), protester->getY()) < 3.00)
            {
                protester->beAnnoyed(annoy);
                annoyedProtester = true;
            }
         }
         ++it;
     }
    return annoyedProtester;
}
// sets all hidden game objects within a radius of 12 to the player visible
void StudentWorld::illuminate(int x, int y)
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (calcDistance(x, y, (*it)->getX(), (*it)->getY()) <= 12.00)
            (*it)->setVisible(true);
        ++it;
    }
}
// creates a map of directions that are optimized for the protester to leave the map
void StudentWorld::generateProtesterPath(GraphObject::Direction map[][VIEW_HEIGHT], int startX, int startY)
{
    // initialize a 2D array (map)
    for (int i = 0; i < VIEW_WIDTH; i++)
        for (int j = 0; j < VIEW_HEIGHT; j++)
            map[i][j] = GraphObject::Direction::none;
    // initialize a queue to use for a breadth-first search to find the optimal path
    queue<XY> q;
    XY first(startX, startY);
    q.push(first);
    XY curr(0, 0);
    while (!q.empty()) {
        curr = q.front();
        q.pop();
        int x = curr.getx();
        int y = curr.gety();
        // check right
        // check if isValidCoord and if the map position is uninitialized (aka no crum has been dropped)
        if (isValidCoord(x+1, y) && map[x + 1][y] == GraphObject::Direction::none) {
            q.push(XY(x + 1, y));
            map[x + 1][y] = GraphObject::Direction::left;
        }
        // check left
        if (isValidCoord(x-1, y) && map[x - 1][y] == GraphObject::Direction::none) {
            q.push(XY(x - 1, y));
            map[x - 1][y] = GraphObject::Direction::right;
        }
        // check up
        if (isValidCoord(x, y+1) && map[x][y + 1] == GraphObject::Direction::none) {
            q.push(XY(x, y + 1));
            map[x][y + 1] = GraphObject::Direction::down;
        }
        // check down
        if (isValidCoord(x, y-1) && map[x][y - 1] == GraphObject::Direction::none) {
            q.push(XY(x, y - 1));
            map[x][y - 1] = GraphObject::Direction::up;
        }
    }
}

// returns true if the given (x,y) does not get out of bounds, overlap with earth/boulders/boulder radius
bool StudentWorld::isValidCoord(int x, int y)
{
    // make sure move doesn't go out of bounds
    if (x < 0 || x > 60 || y < 0 || y > 60)
        return false;

    // invalid if the 4x4 square with bottom-left (x,y) contains an earth pixel
    if (isEarth(x,y))
        return false;

    // invalid if the 4x4 square with bottom-left (x,y) is within a radius of 3.0 units of any Boulders
    if (withinRadiusBoulder(x, y))
        return false;
    
    // invalid if the 4x4 square with bottom-left (x,y) contains a boulder pixel
    if (isBoulder(x, y))
        return false;
    
    return true;
}
bool StudentWorld::isValidMove(GraphObject::Direction dir , int x, int y)
{
    switch (dir)
    {
        case GraphObject::left:
            --x;
            break;
        case GraphObject::right:
            ++x;
            break;
        case GraphObject::down:
            --y;
            break;
        case GraphObject::up:
            ++y;
            break;
        case GraphObject::none:
            break;
    }
    // make sure move doesn't go out of bounds
    if (x < 0 || x > 60 || y < 0 || y > 60)
        return false;

    // invalid if the 4x4 square with bottom-left (x,y) contains an earth pixel
    if (isEarth(x,y))
        return false;

    // invalid if the 4x4 square with bottom-left (x,y) is within a radius of 3.0 units of any Boulders
    if (withinRadiusBoulder(x, y))
        return false;
    
    // invalid if the 4x4 square with bottom-left (x,y) contains a boulder pixel
    if (isBoulder(x, y))
        return false;
    
    return true;
}
// returns true if the given coordinates are within the given radius
bool StudentWorld::withinRadiusTunnelMan(double radius, int x, int y)
{
    return calcDistance(x, y, m_tunnelman->getX(), m_tunnelman->getY()) <= radius;
}
// use the same search maze solving algorithm that generates a path to a target in order to solve for the distance to tunnelman recursively -- follow the directions given by the maze solver, then recursively call getDistance until (x,y) reaches tunnelmam's (x,y)
int StudentWorld::getDistanceToTunnelMan(int x, int y)
{
    // base case: protester is at the same coordinate as tunnelman
    if (m_tunnelman->getX() == x && m_tunnelman->getY() == y)
        return 0;
    // if the current coordinate requires 1 step to the left,
    // then the entire path distance must be 1 + the rest of the path distance (same logic for other directions)
    if(m_protesterpathtotm[x][y] == Actor::left)
        return 1 + getDistanceToTunnelMan(x-1,y);
    else if (m_protesterpathtotm[x][y] == Actor::right)
        return 1 + getDistanceToTunnelMan(x+1, y);
    else if (m_protesterpathtotm[x][y] == Actor::down)
        return 1 + getDistanceToTunnelMan(x, y-1);
    else if (m_protesterpathtotm[x][y] == Actor::up)
        return 1 + getDistanceToTunnelMan(x, y+1);
    // return -1 if there is no clear path to tunnelman
    return -1;
}
