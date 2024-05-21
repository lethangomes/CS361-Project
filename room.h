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
    char getTypeChar(bool);
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

//constructors
Room::Room() : type(CLOSED){}
Room::Room(int x, int y, int type) : x(x), y(y), type(type) {}

//construct room from string. Used in conjunction with toString()
Room::Room(std::string roomString)
{
    //extract values from string
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

    //sets members to extracted values
    x = std::stoi(values[0]);
    y = std::stoi(values[1]);
    type = std::stoi(values[2]);
    available = std::stoi(values[3]);
    visible = std::stoi(values[4]);
    revealed = std::stoi(values[5]);
}

//gets room type
int Room::getType()
{
    return type;
}

//gets character representing room type
char Room::getTypeChar(bool considerVisibility = false)
{
    if(type == CLOSED) return ' ';
    if(!considerVisibility)
    {
        return typeChars[type];
    }
    else
    {
        if(revealed) return typeChars[type];
        if(visible) return '?';
        return ' ';
    }
}

//sets room type
void Room::setType(int newType)
{
    type = newType;
}

//sets the rooms availability
void Room::setAvailable(bool newAvailable)
{
    available = newAvailable;
}

//gets the rooms availability
bool Room::getAvailable()
{
    return available;
}

//sets the rooms visibility
void Room::setVisible(bool newVisible)
{
    visible = newVisible;
}

//gets the rooms visibility
bool Room::getVisible()
{
    return visible;
}

//changes if the room is revealed
void Room::setRevealed(bool newRevealed)
{
    revealed = newRevealed;
}

//gets if the room is revealed
bool Room::getRevealed()
{
    return revealed;
}

//gets rooms x coord
int Room::getX()
{
    return x;
}

//gets rooms y coord
int Room::getY()
{
    return y;
}

//converts room into a string. Used in conjunction with room(string) constructor
std::string Room::toString()
{
    return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(type) + ";" + std::to_string(available) + ";" + std::to_string(visible) + ";" + std::to_string(revealed);
}

//changes rooms coordinates
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