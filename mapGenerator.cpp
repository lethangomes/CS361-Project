#include <iostream>
#include <zmq.hpp>
#include "map.h"
#include "messageParser.h"

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
        map.fullPrint(true);

        Message generatedMap;
        generatedMap.addData("width", generationSettings["width"]);
        generatedMap.addData("height", generationSettings["height"]);
        generatedMap.addData("numRooms", generationSettings["numRooms"]);

        int count = 0;
        for(int i = 0; i < width; i++)
        {
            for(int j = 0; j < height; j++)
            {
                if(map.getRoom(i, j).getType() != CLOSED)
                {
                    generatedMap.addData(std::to_string(count++) , map.getRoom(i, j).toString());
                }
            }
        }

        socket.send(zmq::buffer(generatedMap.toString()), zmq::send_flags::none);
        return 0;
    }
}

int main() 
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");

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
