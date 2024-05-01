#pragma once

#define NUMROOMTYPES 5
#define CLOSED -1
#define EMPTY 0
#define SPAWN 1
#define GOLD 2
#define MONSTER 3
#define TRAP 4

class Room {
    private:
    int x;
    int y;
    int type;
    char typeChars[NUMROOMTYPES] = {'O', 'S', 'G', 'M', 'T'};
    bool available = false; 
    bool visible = false;
    bool revealed = false;

    public:
    Room(int, int, int);
    Room();
    Room(std::string);
    
    int getType();
    char getTypeChar();
    void setType(int);
    std::string getDescription();
    void setAvailable(bool);
    bool getAvailable();
    void setVisible(bool);
    bool getVisible();
    void setRevealed(bool);
    bool getRevealed();
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
    revealed = std::stoi(values[5]);
}

int Room::getType()
{
    return type;
}

char Room::getTypeChar()
{
    if(type == CLOSED) return ' ';
    return typeChars[type];
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

void Room::setRevealed(bool newRevealed)
{
    revealed = newRevealed;
}

bool Room::getRevealed()
{
    return revealed;
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
    return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(type) + ";" + std::to_string(available) + ";" + std::to_string(visible) + ";" + std::to_string(revealed);
}

void Room::setCoords(int newx, int newy)
{
    x = newx;
    y = newy;
}

//gets description to print when player enters room
std::string Room::getDescription()
{
    switch(type)
    {
        case CLOSED:
            return "How did you get here?";
        case EMPTY:
            return "You enter a large, empty room. There's nothing of interest";
        case SPAWN:
            return "You're in the room you entered through";
        case GOLD:
            return "The room you entered is filled with more treasure than you could ever need";
        case TRAP: 
            return "As you enter the room, you hear a soft click. You have stepped into a trap";
        case MONSTER:
            return "You enter a large room. There is a large, shadowy form lurking in the corner";
        default:
            return "You forgot to add a description for this room dumbass";
    }
}