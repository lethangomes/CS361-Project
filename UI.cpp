#include "UI.h"
#include "ports.h"
#include <algorithm>
#include "commands.h"

static std::string commandNames[NUM_COMMANDS] = {"quit", "moveNorth", "moveSouth", "moveEast", "moveWest"};

int main()
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:" + std::string(UI_PORT));

    Message generationSettings;

    //get settings from user
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

    //get number of each room to add
    for(int i = SPAWN + 1; i < NUMROOMTYPES; i++)
    {
        std::string temp;
        std::cout << "How many rooms of type " << i << " would you like?";
        std::cin >> temp;
        generationSettings.addData("numRoomType" + std::to_string(i), temp);
    }

    //send settings to game microservice
    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);
    
    //main game loop
    while(true)
    {
        //print map
        zmq::message_t response;
        socket.recv(response, zmq::recv_flags::none);
        std::string responseString = response.to_string();

        Message message(responseString);
        
        //print map and room description
        std::cout << message["map"] << std::endl;
        std::cout << message["roomDesc"] << std::endl << std::endl;

        //get possible commands from game and print them
        std::cout << "Commands: " << std::endl;
        bool commandAvailablity[NUM_COMMANDS];
        for(int i = 0; i < NUM_COMMANDS; i++)
        {
            commandAvailablity[i] = message.getInt("command" + std::to_string(i));
            if(commandAvailablity[i])
            {
                std::cout << commandNames[i] << std::endl;
            }
        }
        std::cout << std::endl;

        //get command from user
        Message command;
        bool isValid = false;
        while(!isValid)
        {
            //get input
            std::cout << "What would you like to do? ";
            std::string input;
            std::cin >> input;

            //validate input
            for(int i = 0; i < NUM_COMMANDS; i++)
            {
                if(!input.compare(commandNames[i]))
                {
                    if(commandAvailablity[i])
                    {
                        //valid input given
                        command.addData("command", std::to_string(i));
                        isValid = true;
                        break;
                    }
                    std::cout << commandNames[i] << " is not available right now" <<  std::endl;
                    continue;
                }
            }

            if(!isValid)
            {
                std::cout << "Command not recognized. Please pick one from the list above" << std::endl;
            }
        }
        
        socket.send(zmq::buffer(command.toString()), zmq::send_flags::none);

        if(command.getInt("command") == QUIT)
        {
            socket.recv(response, zmq::recv_flags::none);
            break;
        }
    }

    socket.close();
    return 0;
}

void printMap(std::string mapString)
{
    std::replace(mapString.begin(), mapString.end(), '_', ' ');
    std::cout << mapString << std::endl;
}