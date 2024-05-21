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

    //debug print
    std::cout << game.mapPrintString(true);

    //send data to print to UI
    Message UIdata;
    UIdata.addData("map", game.mapPrintString(false));
    game.checkAvailableCommands(UIdata);
    UI_socket.send(zmq::buffer(UIdata.toString()));
    
    //main loop
    while(gameLoop(UI_socket, game, UIdata) == 1);

    //close sockets
    message = "close";
    generator_socket.send(zmq::buffer(message), zmq::send_flags::none);
    generator_socket.recv(response, zmq::recv_flags::none);
    generator_socket.close();

    UI_socket.send(zmq::buffer(message), zmq::send_flags::none);
    UI_socket.close();
}

//generates a map with given settings
zmq::message_t generateMap(zmq::socket_t & socket, std::string settings)
{
    //converts string into message
    Message generationSettings(settings);
    std::cout << "Generating" << std::endl;

    //calls mapGenerator microservice to generate map
    zmq::message_t response;
    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);

    std::cout << response.to_string() << std::endl;

    //returns map
    return response;
}

//processes commands. Returns -1 if command unrecognized, 1 if quit, and 0 otherwise
int processCommand(int commandNum, Game &game)
{
    switch(commandNum)
    {
        case MOVE_NORTH:
            game.moveUp();
            break;
        case MOVE_SOUTH:
            game.moveDown();
            break;
        case MOVE_EAST:
            game.moveRight();
            break;
        case MOVE_WEST:
            game.moveLeft();
            break;
        case QUIT:
            return 1;
            break;
        default:
            std::cout << "Unrecognized command" << std::endl;
            return -1;
    }
    return 0;
}

int gameLoop(zmq::socket_t &UI_socket, Game &game, Message &UIdata)
{
    zmq::message_t response;
    UI_socket.recv(response, zmq::recv_flags::none);
    Message command(response.to_string());
    int commandNum = command.getInt("command");

    //process commands
    if(processCommand(commandNum, game) == 1) return 0;

    //get add map to UIdata
    UIdata.addData("map", game.mapPrintString(false));

    //figure out what commands are available
    game.checkAvailableCommands(UIdata);

    //add room description to message
    UIdata.addData("roomDesc", game.getRoomDescription());

    //send the UI game info
    UI_socket.send(zmq::buffer(UIdata.toString()));

    return 1;
}
