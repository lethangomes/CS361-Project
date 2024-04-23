#include <iostream>
#include <zmq.hpp>

int main() 
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:5555");

    std::cout << "Generating" << std::endl;
    std::string message = "start";
    socket.send(zmq::buffer(message), zmq::send_flags::none);

    zmq::message_t response;
    socket.recv(response, zmq::recv_flags::none);

    message = "close";
    socket.send(zmq::buffer(message), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    socket.close();
}