#include "UI.h"
#include "ports.h"
#include <algorithm>

int main()
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:" + std::string(UI_PORT));

    Message generationSettings;

    //get settings from user(MOVE TO MICROSERVICE LATER)
    std::cout << "Enter the Width of the dungeon: ";
    std::string width;
    std::cin >> width;
    generationSettings.addData("width", width);

    std::cout << "Enter the Height of the dungeon: ";
    std::string height;
    std::cin >> height;
    generationSettings.addData("height", height);

    std::cout << "Enter the Number of rooms in the dungeon: ";
    std::string numRooms;
    std::cin >> numRooms;
    generationSettings.addData("numRooms", numRooms);

    for(int i = SPAWN + 1; i < NUMROOMTYPES; i++)
    {
        std::string temp;
        std::cout << "How many rooms of type " << i << " would you like?";
        std::cin >> temp;
        generationSettings.addData("numRoomType" + std::to_string(i), temp);
    }

    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);

    zmq::message_t response;
    socket.recv(response, zmq::recv_flags::none);

    std::string mapString = response.to_string();
    std::replace(mapString.begin(), mapString.end(), '_', ' ');
    std::cout << mapString << std::endl;


    socket.close();
    return 0;
}