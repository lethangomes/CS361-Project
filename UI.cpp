#include <iostream>
#include <zmq.hpp>

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
        int input;

        std::cin >> input;

        if(input == 0) break;
        std::cout << "Generating" << std::endl;
        message = "start";
        socket.send(zmq::buffer(message), zmq::send_flags::none);
        socket.recv(response, zmq::recv_flags::none);
    }
    

    message = "close";
    socket.send(zmq::buffer(message), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    socket.close();
}