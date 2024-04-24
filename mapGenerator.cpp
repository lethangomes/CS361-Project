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
        //split request into separate values
        std::vector<std::string> values = splitMessage(request, ',');
        Map map(std::stoi(values[0]),std::stoi(values[1]),std::stoi(values[2]));

        //add rooms of specified type
        std::vector<std::string> numTypeRooms = splitMessage(values[3], ';');
        for(int i = 0; i < numTypeRooms.size(); i++)
        {
            map.addRooms(i + SPAWN + 1, std::stoi(numTypeRooms[i]));
        }
        map.fullPrint(true);
        socket.send(zmq::buffer("done"), zmq::send_flags::none);
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
