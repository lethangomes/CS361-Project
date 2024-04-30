#include "Game.h"

int main() 
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t generator_socket{context, zmq::socket_type::req};
    generator_socket.connect("tcp://localhost:" + std::string(MAP_GENERATOR_PORT));

    //construct a socket to connect to UI
    zmq::socket_t UI_socket(context, zmq::socket_type::rep);
    UI_socket.bind("tcp://*:" +  std::string(UI_PORT));

    std::string message;
    zmq::message_t response;

    //get settings from UI
    UI_socket.recv(response, zmq::recv_flags::none);
    Message map(generateMap(generator_socket, response.to_string()).to_string());
    
    Game game(map);

    //sets rooms around spawn to be visible. REMOVE LATER
    game.updateVisibleRooms();

    //debug print
    //game.fullPrint(true);

    //send UI map to print
    UI_socket.send(zmq::buffer(game.mapPrintString(false)));
    
    //main loop
    while(true)
    {
        UI_socket.recv(response, zmq::recv_flags::none);
        Message command(response.to_string());
        std::string commandString= command["command"];

        if(commandString.compare("w") == 0)
        {
            game.moveUp();
        }
        if(commandString.compare("a") == 0)
        {
            game.moveLeft();
        }
        if(commandString.compare("s") == 0)
        {
            game.moveDown();
        }
        if(commandString.compare("d") == 0)
        {
            game.moveRight();
        }
        if(commandString.compare("close") == 0)
        {
            UI_socket.send(zmq::buffer("close"));
            break;
        }

        //figure out what commands are available
        //microservice?? No. Maybe change it to one later

        //add room description to message. Maybe add it as a function of Room

        //send the UI game info
        UI_socket.send(zmq::buffer(game.mapPrintString(false)));
    }

    //close sockets
    message = "close";
    generator_socket.send(zmq::buffer(message), zmq::send_flags::none);
    generator_socket.recv(response, zmq::recv_flags::none);
    generator_socket.close();


    UI_socket.close();
}

zmq::message_t generateMap(zmq::socket_t & socket, std::string settings)
{
    Message generationSettings(settings);
    std::cout << "Generating" << std::endl;

    zmq::message_t response;
    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    return response;
}



