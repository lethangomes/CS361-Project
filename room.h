#pragma once

#define NUMROOMTYPES 5
#define CLOSED -1
#define EMPTY 0
#define SPAWN 1

class Room {
    private:
    //int x;
    //int y;
    int type;
    bool available = false;
    bool visible = false;

    public:
    //Room(int, int, int);
    Room();
    
    int getType();
    void setType(int);
    void setAvailable(bool);
    bool getAvailable();
    void setVisible(bool);
    bool getVisible();
};

Room::Room() : type(CLOSED){}
//Room::Room(int x, int y, int type) : x(x), y(y), type(type) {}

int Room::getType()
{
    return type;
}

void Room::setType(int newType)
{
    type = newType;
}

void Room::setAvailable(bool newAvailable)
{
    available = newAvailable;
}

bool Room::getAvailable()
{
    return available;
}

void Room::setVisible(bool newVisible)
{
    visible = newVisible;
}

bool Room::getVisible()
{
    return visible;
}