#pragma once
#include <iostream>
#include <string>
#include <zmq.hpp>
#include "room.h"
#include "messageParser.h"
#include "ports.h"

zmq::message_t generateMap(zmq::socket_t & socket, std::string);

class Game{
    private:
        int playerX;
        int playerY;
        int width;
        int height;
        int numRooms;
        Room** rooms;

    public:
        Game(int, int, int);
        Game(Message);
        ~Game();
        void setRoom(Room newRoom, int, int);
        void fullPrint(bool);
        std::string mapPrintString(bool);
        void updateVisibleRooms();
        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();
};

Game::~Game()
{
    for(int i = 0; i < width; i++)
    {
        free(rooms[i]);
    }
    free(rooms);
}

Game::Game(Message message)
{
    rooms = message.makeMap(width, height, numRooms);
    playerX = width/2;
    playerY = height/2;
}

Game::Game(int width, int height, int numRooms) : width(width), height(height), numRooms(numRooms)
{
    //create rooms
    rooms = new Room * [width];
    for(int i = 0; i < width; i++)
    {
        rooms[i] = new Room[height];
    }

    playerX = width/2;
    playerY = height/2;
}

void Game::updateVisibleRooms()
{
    if(playerX + 1 < width)
    {
        rooms[playerX + 1][playerY].setVisible(true);
    }
    if(playerY + 1 < height)
    {
        rooms[playerX][playerY + 1].setVisible(true);
    }
    if(playerX - 1 >= 0)
    {
        rooms[playerX - 1][playerY].setVisible(true);
    }
    if(playerY - 1 >= 0)
    {
        rooms[playerX][playerY - 1].setVisible(true);
    }

    rooms[playerX][playerY].setRevealed(true);
}
   

void Game::setRoom(Room newRoom, int x, int y)
{
    rooms[x][y] = newRoom;
}

void Game::fullPrint(bool printInvisible)
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
                    if(i == playerX && j == playerY) std::cout << " * ";
                    else std::cout << " " << rooms[i][j].getTypeChar() << " ";
                }

                if(j != height - 1 && rooms[i][j+1].getType() != CLOSED && (printInvisible || rooms[i][j+1].getVisible())) std::cout << "-";
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

std::string Game::mapPrintString(bool printInvisible)
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
                    if(i == playerX && j == playerY) mapString = mapString +  "_*_";
                    else mapString = mapString +  "_" + rooms[i][j].getTypeChar() + "_";
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

void Game::moveUp()
{
    if(playerX != 0 && rooms[playerX - 1][playerY].getType() != CLOSED)
    {
        playerX -= 1;
        updateVisibleRooms();
    }    
}

void Game::moveDown()
{
    if(playerX != width -1 && rooms[playerX + 1][playerY].getType() != CLOSED)
    {
        playerX += 1;
        updateVisibleRooms();
    }
}

void Game::moveLeft()
{
    if(playerY != 0 && rooms[playerX][playerY - 1].getType() != CLOSED)
    {
        playerY -= 1;
        updateVisibleRooms();
    }
    
}

void Game::moveRight()
{
    if(playerY != height-1 && rooms[playerX][playerY + 1].getType() != CLOSED)
    {
        playerY += 1;
        updateVisibleRooms();
    }
}