#include "Game.h"

int main() 
{
    // construct a socket to connect to map generator
    zmq::context_t context{1};
    zmq::socket_t generator_socket{context, zmq::socket_type::req};
    generator_socket.connect("tcp://localhost:" + std::string(MAP_GENERATOR_PORT));

    //construct a socket to connect to UI
    zmq::socket_t UI_socket(context, zmq::socket_type::rep);
    UI_socket.bind("tcp://*:" +  std::string(UI_PORT));

    //construct a socket to connect to map updater
    zmq::socket_t updater_socket(context, zmq::socket_type::req);
    updater_socket.connect("tcp://localhost:" +  std::string(MAP_UPDATER_PORT));

    //construct a socket to connect to event processor
    zmq::socket_t event_socket(context, zmq::socket_type::req);
    event_socket.connect("tcp://localhost:" +  std::string(EVENT_PROCESSOR_PORT));

    std::string message;
    zmq::message_t response;

    //get settings from UI
    UI_socket.recv(response, zmq::recv_flags::none);
    Message map(generateMap(generator_socket, response.to_string()).to_string());
    //Message map(regenerateMap(generator_socket).to_string());
    Game game(map);
    movementUpdate(game, updater_socket);

    //debug print
    std::cout << game.mapPrintString(true);

    //send data to print to UI
    Message UIdata;
    UIdata.addData("map", game.mapPrintString(false));
    game.checkAvailableCommands(UIdata);
    UI_socket.send(zmq::buffer(UIdata.toString()));
    
    //main loop
    while(gameLoop(UI_socket, updater_socket, game, UIdata, generator_socket, event_socket) == 1);

    //close sockets
    message = "close";
    generator_socket.send(zmq::buffer(message), zmq::send_flags::none);
    generator_socket.recv(response, zmq::recv_flags::none);
    generator_socket.close();

    updater_socket.send(zmq::buffer(message), zmq::send_flags::none);
    updater_socket.recv(response, zmq::recv_flags::none);
    updater_socket.close();

    event_socket.send(zmq::buffer(message), zmq::send_flags::none);
    event_socket.recv(response, zmq::recv_flags::none);
    event_socket.close();
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
int processCommand(int commandNum, Game &game, zmq::socket_t & updater_socket, zmq::socket_t & generator_socket)
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
        case RESTART:
            game.restart(generator_socket);
            break;
        case SAVE:
            std::cout << "Save not implemented" << std::endl;
            break;
        case LOAD:
            std::cout << "Load not implemented" << std::endl;
            break;
        default:
            std::cout << "Unrecognized command" << std::endl;
            return -1;
    }

    //update map based on movement
    if((commandNum >= MOVE_NORTH && commandNum <= MOVE_WEST) || commandNum == RESTART)
    {
        movementUpdate(game, updater_socket);
        return 0;
    }

    return 0;
}

void movementUpdate(Game & game, zmq::socket_t & updater_socket)
{
    Message updateMessage;
    updateMessage.addMap(game.getMap(), game.getWidth(), game.getHeight(), game.getNumRooms());
    updateMessage.addData("updateType", "movement");
    updateMessage.addData("x", std::to_string(game.getPlayerX()));
    updateMessage.addData("y", std::to_string(game.getPlayerY()));
    updater_socket.send(zmq::buffer(updateMessage.toString()), zmq::send_flags::none);

    zmq::message_t response;
    updater_socket.recv(response, zmq::recv_flags::none);
    Message updatedRooms(response.to_string());
    game.updateMap(updatedRooms);
}

int processEvent(Game & game, zmq::socket_t & event_socket, Message & UIdata)
{
    //send map info
    Message message;
    message.addMap(game.getMap(), game.getWidth(), game.getHeight(), game.getNumRooms());
    message.addData("x", std::to_string(game.getPlayerX()));
    message.addData("y", std::to_string(game.getPlayerY()));
    message.addInt("gold", game.getGold());
    event_socket.send(zmq::buffer(message.toString()), zmq::send_flags::none);

    //get updated rooms
    zmq::message_t response;
    event_socket.recv(response, zmq::recv_flags::none);
    Message updatedRooms(response.to_string());

    UIdata.addData("eventText", updatedRooms["eventText"]);

    //update rooms
    if(!updatedRooms["gotGold"].compare("True")) game.addGold();
    if(!updatedRooms["victory"].compare("True")) return 1;
    if(!updatedRooms["updated"].compare("False")) return 0;
    if(!updatedRooms["GameOver"].compare("True")) return 1;
    game.updateMap(updatedRooms);
    return 0;
}

int gameLoop(zmq::socket_t &UI_socket, zmq::socket_t &updater_socket, Game &game, Message &UIdata, zmq::socket_t & generator_socket, zmq::socket_t & event_socket)
{
    zmq::message_t response;
    UI_socket.recv(response, zmq::recv_flags::none);
    Message command(response.to_string());
    int commandNum = command.getInt("command");

    //process commands
    if(processCommand(commandNum, game, updater_socket, generator_socket) == 1) return 0;

    //process events
    int gameStatus = processEvent(game, event_socket, UIdata);

    //get add map to UIdata
    UIdata.addData("map", game.mapPrintString(gameStatus == 1));

    //figure out what commands are available
    game.checkAvailableCommands(UIdata);

    //add room description to message
    UIdata.addData("roomDesc", game.getRoomDescription());

    if(gameStatus == 1) //if processEvent is 1, game over
    {
        game.killPlayer();
        UIdata.addData("gameOver", "True");
        UI_socket.send(zmq::buffer(UIdata.toString()));
        return 0; 
    }

    //send the UI game info
    UI_socket.send(zmq::buffer(UIdata.toString()));

    return 1;
}
