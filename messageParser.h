#pragma once
#include <iostream>
#include <vector>
#include <map>



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
    std::vector<std::string> values = splitMessage(dataString, ',');
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
    data[key] = val;
}

std::string Message::toString()
{
    std::string message = "";
    std::map<std::string, std::string>::iterator it = data.begin();
    while(it != data.end())
    {
        message = message + it->first + ":" + it->second + ",";
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