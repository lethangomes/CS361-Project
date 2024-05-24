#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "room.h"
#include <algorithm>

#define DELIM_CHAR '%'

//This class is used to send messages between services
class Message {
    private:
    std::map<std::string, std::string> data;
    std::vector<std::string> splitMessage(std::string, char);

    public:
    //default
    Message();
    //convert string to data
    Message(std::string);
    //convert data to dict
    Message(const std::vector<std::string>& , const std::vector<std::string>&);

    //add entry to data
    void addData(std::string, std::string);
    void addInt(std::string, int);
    const std::map<std::string, std::string> & getData();
    //convert to string
    std::string toString();
    std::string operator [](std::string);
    int getInt(std::string);
    void addMap(Room** rooms, int, int, int);
    Room** makeMap(int&, int&, int&);
};

//default constructor
Message::Message()
{}

//splits message into substrings separated by the deliminating character and returns vector of those substrings
std::vector<std::string> Message::splitMessage(std::string message, char delim) 
{
    std::vector<std::string> values;
    values.push_back("");
    for(int i = 0; i < message.size(); i++)
    {
        if(message.at(i) == delim)
        {
            values.push_back("");
        }
        else
        {
            values.at(values.size() - 1) = values.at(values.size() - 1) + message.at(i);
        }
    }
    
    return values;
}

//converts a string into a message
Message::Message(std::string dataString)
{
    std::vector<std::string> values = splitMessage(dataString, DELIM_CHAR);
    for(int i = 0; i < values.size(); i++)
    {
        
        std::vector<std::string> value = splitMessage(values[i], ':');
        addData(value[0], value[1]);
    }
}

//converts 2 vectors into a message
Message::Message(const std::vector<std::string>& keys, const std::vector<std::string>& values)
{
    for(int i = 0; i < keys.size(); i++)
    {
        addData(keys[i], values[i]);
    }
}

//returns the data in message
const std::map<std::string, std::string> & Message::getData()
{
    return data;
}

//adds a new piece of data to the message
void Message::addData(std::string key, std::string val)
{
    std::replace(val.begin(), val.end(), '_', ' ');
    data[key] = val;
}

//adds a new piece of data to the message
void Message::addInt(std::string key, int val)
{
    addData(key, std::to_string(val));
}

//converts the message to a string. Can be used in conjunction with the constructor that
//converts a string to a message
std::string Message::toString()
{
    std::string message = "";
    std::map<std::string, std::string>::iterator it = data.begin();

    //converts spaces to "_" because ZMQ doesn't like whitespace
    while(it != data.end())
    {
        //std::cout << it->first << std::endl;
        std::string val = it->second;
        std::replace(val.begin(), val.end(), ' ', '_');

        //keys and values are seperated by ":". Key-value pairs are seperated by DELIM_CHAR
        message = message + it->first + ":" + it->second + DELIM_CHAR;
        it++;
    }

    //remove the extra DELIM_CHAR at the end
    message.pop_back();
    return message;
}

//overrides the [] operator to get info from the dictionary
std::string Message::operator [](std::string key)
{
    if(data.find(key) == data.end())
    {
        return "";
    }
    return data[key];
}

//gets data from the message and converts the data to an int
int Message::getInt(std::string key)
{
    return std::stoi(data[key]);
}

//converts an array of rooms into data that can be stored in the message
void Message::addMap(Room ** rooms, int width, int height, int numRooms)
{
    addData("width", std::to_string(width));
    addData("height", std::to_string(height));
    addData("numRooms", std::to_string(numRooms));

    int count = 0;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(rooms[i][j].getType() != CLOSED)
            {
                addData("Room" + std::to_string(count++) , rooms[i][j].toString());
            }
        }
    }
}

//creates an array of rooms using data in the message
Room ** Message::makeMap(int& width, int& height, int& numRooms)
{
    width = getInt("width");
    height = getInt("height");
    numRooms = getInt("numRooms");

    //create rooms
    Room ** rooms = new Room * [width];
    for(int i = 0; i < width; i++)
    {
        rooms[i] = new Room[height];
    }

    //add rooms to map
    for(int i = 0; i < numRooms; i++)
    {
        Room newRoom(data["Room" + std::to_string(i)]);
        rooms[newRoom.getX()][newRoom.getY()] = newRoom;
    }

    return rooms;
}