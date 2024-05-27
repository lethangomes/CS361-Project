#include <iostream>
#include <zmq.hpp>
#include "../messageParser.h"
#include "../ports.h"
#include "../commands.h"

void movementRoomUpdate(int x, int y, int width, int height, Room ** map, Message & message, int & numUpdates)
{
    if(x >= 0 && x < width && y>= 0 && y < height)
    {
        if(!map[x][y].getVisible() && map[x][y].getType() != CLOSED)
        {
            map[x][y].setVisible(true);
            message.addData(std::to_string(numUpdates++), map[x][y].toString());
        }
    }
}

Message movementUpdate(int x, int y, Message & message)
{
    int width, height, numRooms;
    Room ** map = message.makeMap(width, height, numRooms);
    Message response;
    int numUpdates = 0;

    //make rooms around player visible
    movementRoomUpdate(x + 1, y, width, height, map, response, numUpdates);
    movementRoomUpdate(x - 1, y, width, height, map, response, numUpdates);
    movementRoomUpdate(x, y + 1, width, height, map, response, numUpdates);
    movementRoomUpdate(x, y - 1, width, height, map, response, numUpdates);

    map[x][y].setRevealed(true);
    response.addData(std::to_string(numUpdates++), map[x][y].toString());

    response.addInt("numUpdates", numUpdates);
    return response;
}

Message specialUpdate(int type, Message & message)
{
    int width, height, numRooms;
    Room ** map = message.makeMap(width, height, numRooms);
    Message response;
    int numUpdates = 0;
    int x = message.getInt("x");
    int y = message.getInt("y");

    switch(type)
    {
        case GOLD:
            //remove gold room from map
            response.addData("gotGold", "True");
            map[x][y].setType(EMPTY);
            response.addData(std::to_string(numUpdates++), map[x][y].toString());
            break;
        case TRAP:
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height; j++)
                {
                    if(map[i][j].getRevealed() == true)
                    {
                        map[i][j].setRevealed(false);
                        response.addData(std::to_string(numUpdates++), map[i][j].toString());
                    }
                }
            }
            break;
        default:
            std::cout << "invalid event" << std::endl;

    }

    response.addInt("numUpdates", numUpdates);
    return response;
}

int main() 
{
    srand(time(NULL));
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    socket.bind("tcp://*:" +  std::string(MAP_UPDATER_PORT));

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

        Message message(requestStr);
        if(!message["updateType"].compare("movement"))
        {
            Message response = movementUpdate(message.getInt("x"), message.getInt("y"), message);
            std::cout << response.toString() << std::endl;
            socket.send(zmq::buffer(response.toString()), zmq::send_flags::none);
        }
        else if(!message["updateType"].compare("special"))
        {
            Message response = specialUpdate(message.getInt("event"), message);
            std::cout << response.toString() << std::endl;
            socket.send(zmq::buffer(response.toString()), zmq::send_flags::none);
        }
        else
        {
            std::cout << "Unrecognized update type" << std::endl;
        }
    }

    socket.close();
}
