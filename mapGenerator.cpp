#include <iostream>
#include <zmq.hpp>
#include "map.h"
#include "messageParser.h"
#include "ports.h"

int processRequest(std::string request, zmq::socket_t& socket)
{
    //process request
    if(!request.compare("close"))
    {
        socket.send(zmq::buffer("closing"), zmq::send_flags::none);
        return 1;
    }
    else
    {
        Message generationSettings(request);
        //split request into separate values
        int width = generationSettings.getInt("width");
        int height = generationSettings.getInt("height");
        int numRooms = generationSettings.getInt("numRooms");

        Map map(width, height, numRooms);

        //add rooms of specified type
        for(int i = SPAWN + 1; i < NUMROOMTYPES; i++)
        {
            std::string roomCount = generationSettings["numRoomType" + std::to_string(i)];
            if(roomCount.compare("")) map.addRooms(i, std::stoi(roomCount));
        }

        //turn map into message
        Message generatedMap;
        generatedMap.addMap(map.getRooms(), width, height, numRooms);

        //send message
        socket.send(zmq::buffer(generatedMap.toString()), zmq::send_flags::none);
        return 0;
    }
}

int main() 
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:" +  std::string(MAP_GENERATOR_PORT));

    while(true)
    {
        //wait for message
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::none);
        std::cout << "Received " << request.to_string() << std::endl;
        if(processRequest(request.to_string(), socket) == 1) break;
    }

    socket.close();
}
