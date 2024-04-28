#pragma once

#define NUMROOMTYPES 5
#define CLOSED -1
#define EMPTY 0
#define SPAWN 1

class Room {
    private:
    int x;
    int y;
    int type;
    bool available = false;
    bool visible = false;

    public:
    Room(int, int, int);
    Room();
    Room(std::string);
    
    int getType();
    void setType(int);
    void setAvailable(bool);
    bool getAvailable();
    void setVisible(bool);
    bool getVisible();
    int getX();
    int getY();
    void setCoords(int, int);
    std::string toString();
};

Room::Room() : type(CLOSED){}
Room::Room(int x, int y, int type) : x(x), y(y), type(type) {}
Room::Room(std::string roomString)
{
    std::vector<std::string> values;
    values.push_back("");
    for(int i = 0; i < roomString.size(); i++)
    {
        if(roomString.at(i) == ';')
        {
            values.push_back("");
        }
        else
        {
            values.at(values.size() - 1) = values.at(values.size() - 1) + roomString.at(i);
        }
    }

    x = std::stoi(values[0]);
    y = std::stoi(values[1]);
    type = std::stoi(values[2]);
    available = std::stoi(values[3]);
    visible = std::stoi(values[4]);
}

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

int Room::getX()
{
    return x;
}

int Room::getY()
{
    return y;
}

std::string Room::toString()
{
    return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(type) + ";" + std::to_string(available) + ";" + std::to_string(visible);
}

void Room::setCoords(int newx, int newy)
{
    x = newx;
    y = newy;
}