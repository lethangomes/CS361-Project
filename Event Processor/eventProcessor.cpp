#include <iostream>
#include <zmq.hpp>
#include "../messageParser.h"
#include "../ports.h"
#include "../commands.h"

static const std::string EVENT_TRIGGER_MSG[NUMROOMTYPES] = {
    "Nothing happened",
    "You consider leaving, but decide to continue searching for treasure",
    "You found a huge bar of gold! You quickly stuff it into your backpack",
    "The creature lunges at you!",
    "Rocks drop from the ceiling and you barely get out of the way. You lose your map in the confusion"
};

void updateMap(Message & message, zmq::socket_t & updater_socket, int type, Message & eventResults)
{
    //send message
    message.addData("updateType", "special");
    message.addInt("event", type);
    updater_socket.send(zmq::buffer(message.toString()), zmq::send_flags::none);

    //recieve message
    zmq::message_t response;
    updater_socket.recv(response, zmq::recv_flags::none);
    eventResults = Message(response.to_string());
    eventResults.addData("updated", "True");
    eventResults.addData("GameOver", "False");
}

void checkForEvent(Message & message, zmq::socket_t & socket, zmq::socket_t & updater_socket)
{
    //get map from message
    int width, height, numRooms;
    Room ** rooms = message.makeMap(width, height, numRooms);
    int playerX = message.getInt("x");
    int playerY = message.getInt("y");
    int type = rooms[playerX][playerY].getType();
    Message eventResults;

    switch(type)
    {
        case CLOSED:
            std::cout << "You broke something" << std::endl;
            eventResults.addData("updated", "False");
            //kill the player?
            break;
        case EMPTY:
            std::cout << "No Event triggered" << std::endl;
            eventResults.addData("updated", "False");
            break;
        case SPAWN:
            std::cout << "Spawn Event triggered" << std::endl;
            eventResults.addData("updated", "False");
            if(message.getInt("gold") >= 1)
            {
                eventResults.addData("victory", "True");
                eventResults.addData("eventText", "You exit the dungeon with your treasure");
            }
            break;
        case GOLD:
            std::cout << "Gold Event triggered" << std::endl;
        case TRAP:
            std::cout << "Trap event triggered" << std::endl;
            updateMap(message, updater_socket, type, eventResults);
            break;
        case MONSTER:
            std::cout << "Monster Event triggered" << std::endl;
            eventResults.addData("updated", "True");
            eventResults.addData("GameOver", "True");
            break;
        default:
            std::cout << "Unrecognized room type" << std::endl;
            eventResults.addData("updated", "False");
    }

    //add event message to message
    if(!eventResults["eventText"].compare("") && type != CLOSED)
    {
        eventResults.addData("eventText", EVENT_TRIGGER_MSG[type]);
    }
    else if (type == CLOSED)
    {
        eventResults.addData("eventText", "You are literally inside the walls of the dungeon. How did this even happen? Whatever, time to debug");
    }
    socket.send(zmq::buffer(eventResults.toString()), zmq::send_flags::none);
}

int main()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:" +  std::string(EVENT_PROCESSOR_PORT));

    //construct a socket to connect to map updater
    zmq::socket_t updater_socket(context, zmq::socket_type::req);
    updater_socket.connect("tcp://localhost:" +  std::string(MAP_UPDATER_PORT));

    while(true)
    {
        //wait for message
        zmq::message_t request;
        socket.recv(request, zmq::recv_flags::none);
        std::string requestStr = request.to_string();

        //process request
        if(!requestStr.compare("close"))
        {
            socket.send(zmq::buffer("closing"), zmq::send_flags::none);
            break;
        }
        
        Message message(request.to_string());
        
        checkForEvent(message, socket, updater_socket);
    }
}