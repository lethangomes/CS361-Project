#pragma once

#define CLOSED -1
#define EMPTY 0
#define SPAWN 1

class Room {
    private:
    int x;
    int y;
    int type;
    bool available = false;

    public:
    Room(int, int, int);
    Room();
    
    int getType();
    void setType(int);
    void setAvailable(bool);
    bool getAvailable();
};

Room::Room() : type(CLOSED){}
Room::Room(int x, int y, int type) : x(x), y(y), type(type) {}

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