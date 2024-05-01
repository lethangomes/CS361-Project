#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "room.h"
#include <algorithm>

#define DELIM_CHAR '%'


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

Message::Message(std::string dataString)
{
    std::vector<std::string> values = splitMessage(dataString, DELIM_CHAR);
    for(int i = 0; i < values.size(); i++)
    {
        
        std::vector<std::string> value = splitMessage(values[i], ':');
        addData(value[0], value[1]);
    }
}

Message::Message(const std::vector<std::string>& keys, const std::vector<std::string>& values)
{
    for(int i = 0; i < keys.size(); i++)
    {
        addData(keys[i], values[i]);
    }
}

const std::map<std::string, std::string> & Message::getData()
{
    return data;
}

void Message::addData(std::string key, std::string val)
{
    std::replace(val.begin(), val.end(), '_', ' ');
    data[key] = val;
}

std::string Message::toString()
{
    std::string message = "";
    std::map<std::string, std::string>::iterator it = data.begin();
    while(it != data.end())
    {
        std::string val = it->second;
        std::replace(val.begin(), val.end(), ' ', '_');
        message = message + it->first + ":" + it->second + DELIM_CHAR;
        it++;
    }
    message.pop_back();
    return message;
}

std::string Message::operator [](std::string key)
{
    if(data.find(key) == data.end())
    {
        return "";
    }
    return data[key];
}

int Message::getInt(std::string key)
{
    return std::stoi(data[key]);
}

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