#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"



zmq::message_t generateMap(zmq::socket_t & socket)
{
    //settings format(for now): width,height,numRooms,[listofnumberofrooms]
    std::string settings;
    
    //get settings from user(MOVE TO MICROSERVICE LATER)
    std::cout << "Enter the Width of the dungeon: ";
    std::string width;
    std::cin >> width;
    std::cout << "Enter the Height of the dungeon: ";
    std::string height;
    std::cin >> height;
    std::cout << "Enter the Number of rooms in the dungeon: ";
    std::string numRooms;
    std::cin >> numRooms;

    std::string roomTypeCounts = "";
    for(int i = SPAWN + 1; i < NUMROOMTYPES; i++)
    {
        std::string temp;
        std::cout << "How many rooms of type " << i << " would you like?";
        std::cin >> temp;
        roomTypeCounts = roomTypeCounts + temp;
        if(i != NUMROOMTYPES - 1 ) roomTypeCounts = roomTypeCounts + ";";
    }

    std::cout << "Generating" << std::endl;
    std::string message = width + "," + height + "," + numRooms + "," + roomTypeCounts;

    zmq::message_t response;
    socket.send(zmq::buffer(message), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    return response;
}

int main() 
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5555");

    std::string message;
    zmq::message_t response;

    while(true)
    {
        generateMap(socket);
    }
    

    message = "close";
    socket.send(zmq::buffer(message), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    socket.close();
}