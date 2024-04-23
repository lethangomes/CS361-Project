#include <iostream>
#include <zmq.hpp>
#include "map.h"

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

        //process request
        if(!request.to_string().compare("close"))
        {
            socket.send(zmq::buffer("closing"), zmq::send_flags::none);
            break;
        }
        else
        {
            Map map(10,10,30);
            map.print();
            socket.send(zmq::buffer("done"), zmq::send_flags::none);
        }

    }

    socket.close();
}
