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
    
    //get map parameters from message
    int width = map.getInt("width");
    int height = map.getInt("height");
    int numRooms = map.getInt("numRooms");
    
    //create rooms
    Room** rooms = new Room * [width];
    for(int i = 0; i < width; i++)
    {
        rooms[i] = new Room[height];
    }

    //use response from mapGenerator to fill rooms
    for(int i = 0; i < numRooms; i++)
    {
        Room newRoom(map[std::to_string(i)]);
        rooms[newRoom.getX()][newRoom.getY()] = newRoom;
    }
    
    //sets rooms around spawn to be visible. REMOVE LATER
    rooms[width/2 + 1][height/2].setVisible(true);
    rooms[width/2 - 1][height/2].setVisible(true);
    rooms[width/2][height/2 + 1].setVisible(true);
    rooms[width/2][height/2 - 1].setVisible(true);

    fullPrint(true, width, height, rooms);
    UI_socket.send(zmq::buffer(mapPrintString(false, width, height, rooms)));
    

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

void fullPrint(bool printInvisible, int width, int height, Room ** rooms)
{
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            if(type == CLOSED || (!printInvisible && !rooms[i][j].getVisible()))
            {
                std::cout << "    ";
            }
            else
            {
                if(!printInvisible && !rooms[i][j].getRevealed())
                {
                    std::cout << " ? ";
                }
                else
                {
                    std::cout << " " << rooms[i][j].getTypeChar() << " ";
                }

                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED && rooms[i][j+1].getVisible()) std::cout << "-";
                else std::cout << " ";
            }
            
        }
        std::cout << std::endl;

        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            std::cout << " ";
            if(i == width - 1 || type == CLOSED)
            {
                std::cout << "   ";
            }
            else
            {
                if(rooms[i+1][j].getType() != CLOSED && (printInvisible || (rooms[i+1][j].getVisible() && rooms[i][j].getVisible()))) std::cout << "|  ";
                else std::cout << "   ";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        std::cout << std::endl;
    }
}

std::string mapPrintString(bool printInvisible, int width, int height, Room ** rooms)
{
    std::string mapString = "";
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            if(type == CLOSED || (!printInvisible && !rooms[i][j].getVisible()))
            {
                mapString += "____";
            }
            else
            {
                if(!printInvisible && !rooms[i][j].getRevealed())
                {
                    mapString += "_?_";
                }
                else
                {
                    mapString = mapString +  "_" + rooms[i][j].getTypeChar() + "_";
                }

                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED && (printInvisible || rooms[i][j+1].getVisible())) mapString += "-";
                else mapString += "_";
            }
            
        }
        mapString += '\n';

        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            mapString += "_";
            if(i == width - 1 || type == CLOSED)
            {
                mapString += "___";
            }
            else
            {
                if(rooms[i+1][j].getType() != CLOSED && (printInvisible || (rooms[i+1][j].getVisible() && rooms[i][j].getVisible()))) mapString += "|__";
                else mapString += "___";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        mapString += '\n';
    }

    return mapString;
}