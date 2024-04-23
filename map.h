#pragma once
#include "room.h"
#include <iostream>

class Map {
    private:
        Room** rooms;
        int width;
        int height;
        int numRooms;
        
        void checkAvailableSpots(int *, int*, int&, int, int);

    public:
        Map(int, int, int);

        void print();
        void fullPrint(bool);
};

Map::Map(int width, int height, int numRooms) : width(width), height(height), numRooms(numRooms)
{
    srand(time(NULL));

    //make sure we don't try to generate more rooms than can fit on the map
    if(numRooms > width * height)
    {
        numRooms = width * height;
    }

    //create rooms
    rooms = new Room * [width];
    for(int i = 0; i < width; i++)
    {
        rooms[i] = new Room[height];
    }

    //sets spawn in the middle of the map
    std::cout << "test" << width << height<<std::endl;

    rooms[width/2][height/2].setType(SPAWN);

    //creates lists to track where new rooms can be added
    int numOpenSpots = 0;
    int * openSpotsX = new int[width * height];
    int * openSpotsY = new int[width * height];
    checkAvailableSpots(openSpotsX, openSpotsY, numOpenSpots, width/2, height/2);


    //adds specified number of rooms
    for(int i = 0; i < numRooms - 1; i++)
    {
        
        //pick random room that is connected to a valid room and set it to be empty
        int index = rand()%numOpenSpots;
        int x = openSpotsX[index];
        int y = openSpotsY[index];
        openSpotsX[index] = openSpotsX[numOpenSpots - 1];
        openSpotsY[index] = openSpotsY[numOpenSpots - 1];
        numOpenSpots--;
        rooms[x][y].setType(EMPTY);
        //updates list of available spots for new rooms
        checkAvailableSpots(openSpotsX, openSpotsY, numOpenSpots, x, y);
    }

    free(openSpotsX);
    free(openSpotsY);
}

void Map::checkAvailableSpots(int * openSpotsX, int* openSpotsY, int& numOpenSpots, int x, int y)
{
    if(x + 1 < width && rooms[x+1][y].getType() == CLOSED && !rooms[x+1][y].getAvailable())
    {
        rooms[x+1][y].setAvailable(true);
        openSpotsX[numOpenSpots] = x + 1;
        openSpotsY[numOpenSpots] = y;
        numOpenSpots++;
    }
    if(x - 1 >= 0 && rooms[x-1][y].getType() == CLOSED && !rooms[x-1][y].getAvailable())
    {
        rooms[x-1][y].setAvailable(true);
        openSpotsX[numOpenSpots] = x - 1;
        openSpotsY[numOpenSpots] = y;
        numOpenSpots++;
    }
    if(y + 1 < height && rooms[x][y+1].getType() == CLOSED && !rooms[x][y+1].getAvailable())
    {
        rooms[x][y+1].setAvailable(true);
        openSpotsX[numOpenSpots] = x;
        openSpotsY[numOpenSpots] = y + 1;
        numOpenSpots++;
    }
    if(y - 1 >= 0 && rooms[x][y-1].getType() == CLOSED && !rooms[x][y-1].getAvailable())
    {
        rooms[x][y-1].setAvailable(true);
        openSpotsX[numOpenSpots] = x;
        openSpotsY[numOpenSpots] = y - 1;
        numOpenSpots++;
    }
}

void Map::print()
{
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            std::cout << rooms[i][j].getType() << "\t";
        }
        std::cout << std::endl;
    }
}

void Map::fullPrint(bool printInvisible)
{
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int type = rooms[i][j].getType();
            if(type == CLOSED)
            {
                std::cout << "    ";
            }
            else
            {
                std::cout << " " << type << " ";

                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED) std::cout << "-";
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
                if(rooms[i+1][j].getType() != CLOSED) std::cout << "|  ";
                else std::cout << "   ";
            }
            //std::cout << rooms[i][j].getType() << "\t";
        }
        std::cout << std::endl;
    }
}