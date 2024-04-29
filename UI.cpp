#include "UI.h"

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
        Message map(generateMap(socket).to_string());
        
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

        for(int i = 0; i < numRooms; i++)
        {
            Room newRoom(map[std::to_string(i)]);
            rooms[newRoom.getX()][newRoom.getY()] = newRoom;
        }
        rooms[width/2 + 1][height/2].setVisible(true);
        rooms[width/2 - 1][height/2].setVisible(true);
        rooms[width/2][height/2 + 1].setVisible(true);
        rooms[width/2][height/2 - 1].setVisible(true);

        fullPrint(false, width, height, rooms);
    }
    

    message = "close";
    socket.send(zmq::buffer(message), zmq::send_flags::none);
    socket.recv(response, zmq::recv_flags::none);
    socket.close();
}

zmq::message_t generateMap(zmq::socket_t & socket)
{
    //settings format(for now): width,height,numRooms,[listofnumberofrooms]
    std::string settings;
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
                if(rooms[i+1][j].getType() != CLOSED && rooms[i+1][j].getVisible() && rooms[i][j].getVisible()) std::cout << "|  ";
                else std::cout << "   ";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        std::cout << std::endl;
    }
}