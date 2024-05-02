#include "UI.h"
#include "ports.h"
#include <algorithm>
#include "commands.h"
#include <chrono>

#define DEFAULT_WIDTH 10
#define DEFAULT_HEIGHT 10
#define DEFAULT_NUMROOMS 25
#define DEFAULT_NUM_GOLD 1
#define DEFAULT_NUM_MONSTER 3
#define DEFAULT_NUM_TRAP 4

static std::string commandNames[NUM_COMMANDS] = {"quit", "moveNorth", "moveSouth", "moveEast", "moveWest", "help"};
static std::string commandDescriptions[NUM_COMMANDS] = {"Closes the game", "Moves player north", "Moves player south", "Moves player east", "Moves player west", "Opens help menu"};

int main()
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:" + std::string(UI_PORT));

    Message generationSettings;

    //intro message
    std::cout << "Welcome to my very cool and epic untitled game. In this game you run around a dungeon, collect gold, and bring it back to where you came in while avoiding monsters and traps." << std::endl;

    //Ask if user wants default or custom dungeon
    std::cout << "Would you like to generate the dungeon with default settings, or generate a dungeon with custom settings for a more specific experience" << std::endl;
    std::cout << std::endl <<"Note - depending on what you want to change and how familiar you are with the available settings, setting up a custom generated dungeon may take a while and may have unintended effects on gameplay." << std::endl;
    
    //get setting mode(default/custom) from user
    int settingMode;
    do
    {
        std::cout << std::endl << "Enter 0 for default settings. Enter 1 to go to settings page" ;
        std::cin >> settingMode;
    } while (settingMode != 0 && settingMode != 1);

    //set settings to default values
    std::string width = std::to_string(DEFAULT_WIDTH);
    std::string height = std::to_string(DEFAULT_HEIGHT);
    std::string numRooms = std::to_string(DEFAULT_NUMROOMS);
    std::string numRoomTypes[NUMROOMTYPES - SPAWN - 1] = {
        std::to_string(DEFAULT_NUM_GOLD),
        std::to_string(DEFAULT_NUM_MONSTER),
        std::to_string(DEFAULT_NUM_TRAP)
    };

    //change settings user wants to
    if(settingMode == 1)
    {
        std::cout << "-----------------------------------" << std::endl << std::endl;
        changeSettings(width, height, numRooms, numRoomTypes);
    }

    //add parameters to message
    generationSettings.addData("width", width);
    generationSettings.addData("height", height);
    generationSettings.addData("numRooms", numRooms);
    for(int i = SPAWN + 1; i < NUMROOMTYPES; i++)
    {
        generationSettings.addData("numRoomType" + std::to_string(i), numRoomTypes[i - SPAWN - 1]);
    }

    //send settings to game microservice
    auto t1 = std::chrono::high_resolution_clock::now();
    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " <<std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
    
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
        std::vector<bool> commandAvailablity(NUM_COMMANDS);
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
                        //Special case if command is help
                        if(i == HELP)
                        {
                            std::cout << "Which command do you need help with?(enter \"all\" for list of all commands): ";
                            std::string helpArg;
                            std::cin >> helpArg;
                            if(!helpArg.compare("all"))
                            {
                                std::cout << std::endl;
                                printHelp(commandAvailablity);
                            }
                            else
                            {
                                printHelp(helpArg);
                            }
                            break;
                        }
                        else{
                            //valid input given
                            command.addData("command", std::to_string(i));
                            isValid = true;
                            break;
                        }
                        
                    }

                    //command used exists, but cannot be used
                    std::cout << commandNames[i] << " is not available right now" <<  std::endl;
                    break;
                }
            }

            if(!isValid && input.compare(commandNames[HELP]))
            {
                //command given does not exist
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

//splits message into substrings separated by the deliminating character and returns vector of those substrings
std::vector<std::string> splitMessage(std::string message, char delim) 
{
    std::vector<std::string> values;
    values.push_back("");
    for(int i = 0; i < message.size(); i++)
    {
        if(message.at(i) == delim)
        {
            values.push_back("");
        }
        else
        {
            values.at(values.size() - 1) = values.at(values.size() - 1) + message.at(i);
        }
    }
    
    return values;
}

//prints help menu
void printHelp(std::vector<bool> commandAvailability)
{
    //print avaialble commands
    std::cout << "Available commands: " << std::endl;
    for(int i = 0; i < NUM_COMMANDS; i++)
    {
        if(commandAvailability[i])
        {
            std::cout << commandNames[i] << "\t - " << commandDescriptions[i] << std::endl;
        }
    }
    std::cout << std::endl;

    //print unavailable commands
    std::cout << "Unavailable commands: " << std::endl;
    for(int i = 0; i < NUM_COMMANDS; i++)
    {
        if(!commandAvailability[i])
        {
            std::cout << commandNames[i] << " - " << commandDescriptions[i] << std::endl;
        }
    }
    std::cout << std::endl;
}

void printHelp(std::string arg)
{
    //figure out what command the user asked for
    std::cout << std::endl;
    int commandRequested = -1;
    for(int i = 0; i < NUM_COMMANDS; i++)
    {
        if(!arg.compare(commandNames[i]))
        {
            commandRequested = i;
            break;
        }
    }

    //user asked for command that doesn't exist
    if(commandRequested == -1)
    {
        std::cout << "Command " << arg << " does not exist" << std::endl;
        return;
    }

    //prints information about specified command
    switch(commandRequested)
    {
        case QUIT:
            printHelpInfo(commandRequested, "Imediately closes the game without saving", "Always available");
            break;
        case MOVE_NORTH:
            printHelpInfo(commandRequested, "Moves the player North(up)", "Can only be used when there is a room to the north of the player");
            break;
        case MOVE_WEST:
            printHelpInfo(commandRequested, "Moves the player West(Left)", "Can only be used when there is a room to the west of the player");
            break;
        case MOVE_EAST:
            printHelpInfo(commandRequested, "Moves the player East(Right)", "Can only be used when there is a room to the east of the player");
            break;
        case MOVE_SOUTH:
            printHelpInfo(commandRequested, "Moves the player South(down)", "Can only be used when there is a room to the south of the player");
            break;
        case HELP:
            printHelpInfo(commandRequested, "Displays list of all commands or specific information about specific commands", "Always available");
            break;
    }
}

//prints information about a given command
void printHelpInfo(int command, std::string description, std::string availability)
{
    std::cout << commandNames[command] << ": " << description << std::endl << std::endl;
    std::cout << "Availability: " << availability << std::endl;
}

void changeSettings(std::string& width, std::string& height, std::string& numRooms, std::string numRoomTypes[NUMROOMTYPES - 2])
{
    std::string userInput = "";
    while(userInput.compare("done"))
    {
        //print settings the user can change
        std::cout << "Please choose the settings to change for the custom dungeon:\n" << std::endl;
        std::cout << "numRooms - The total number of rooms that will be generated" << std::endl;
        std::cout << "width - The maximum width of the dungeon" << std::endl;
        std::cout << "height - The maximum height of the dungeon" << std::endl;
        std::cout << "numGold - The number of gold rooms to be generated" << std::endl;
        std::cout << "numMonster - The number of monsters the game starts with" << std::endl;
        std::cout << "numTrap - The number of rooms generated with traps" << std::endl;
        std::cout << std::endl << "Please enter the name of the setting you would like to change. Enter \"done\" when you are finished. Enter \"default\" to revert all settings to their default values" << std::endl;

        //get user input
        std::cin >> userInput;
        std::cout << std::endl;

        std::cout << "-----------------------------------" << std::endl << std::endl;

        if(!userInput.compare("default"))
        {
            //reset all settings to default values
            //ADD CONFIRMATION LATER
            width = std::to_string(DEFAULT_WIDTH);
            height = std::to_string(DEFAULT_HEIGHT);
            numRooms = std::to_string(DEFAULT_NUMROOMS);
            numRoomTypes[0] = std::to_string(DEFAULT_NUM_GOLD);
            numRoomTypes[1] = std::to_string(DEFAULT_NUM_MONSTER);
            numRoomTypes[2] = std::to_string(DEFAULT_NUM_TRAP);
        }

        //numRooms
        if(!userInput.compare("numRooms")) updateSetting(userInput ,numRooms, DEFAULT_NUMROOMS, 10, (std::stoi(width) * std::stoi(height))/2, 5, 400,
            "the dungeon will only generated up to width * height rooms, even if this setting is set to a larger value. Additionally, if you try to generate more special rooms(gold, traps, and monsters) than there are total rooms, some of the special rooms will not generate", 
            "numRooms - The total number of rooms that will be generated");
        
        //height
        //width and height in the code are the opposite of what you'd intuitively think they were so I switched them for the user
        if(!userInput.compare("width")) updateSetting(userInput ,height, DEFAULT_HEIGHT, 5, 15, 3, 20,
         "the dungeon generates in a rectangular grid. This setting changes how large that grid is horizontally. If the grid doesn't have enough space for the number of rooms you generate some of those rooms won't generate", 
         "width - The maximum width of the dungeon");
        
        //width
        if(!userInput.compare("height")) updateSetting(userInput ,width, DEFAULT_WIDTH, 5, 15, 3, 20,
         "the dungeon generates in a rectangular grid. This setting changes how large that grid is vertically. If the grid doesn't have enough space for the number of rooms you generate some of those rooms won't generate", 
         "height - The maximum height of the dungeon");

        //numGold
        if(!userInput.compare("numGold")) updateSetting(userInput ,numRoomTypes[0], DEFAULT_NUM_GOLD, 1, 5, 1, 100,
         "Gold rooms are the win condition of this game. Your goal is to find as many of them as you can and leave without dying", 
         "numGold - The number of rooms that will spawn with gold");

        //numMonster
        if(!userInput.compare("numMonster")) updateSetting(userInput ,numRoomTypes[1], DEFAULT_NUM_MONSTER, 3, 5, 0, 100,
         "Monsters are enemies that roam the dungeon. Entering a room with one will instantly kill you. They can be killed, which will remove them from the map", 
         "numMonster - The number of rooms that will spawn with monsters in them");

        //numTrap
        if(!userInput.compare("numTrap")) updateSetting(userInput ,numRoomTypes[2], DEFAULT_NUM_TRAP, 3, 10, 0, 100,
         "Trapped rooms are rooms that cause a random negative effect for the player", 
         "numTrap - The number of rooms that will spawn with traps");

        std::cout << "-----------------------------------" << std::endl << std::endl;
    }
}

bool validateInput(std::string input)
{
    for(int i = 0; i < input.size(); i++)
    {
        if(!std::isdigit(input[i])) return false;
    }
    return true;
}

void updateSetting(std::string name, std::string & setting, int defaultVal, int recMin, int recMax, int min, int max, std::string notes, std::string description)
{
    //print setting info
    std::cout << description << std::endl;
    std::cout << "Default value: " << defaultVal << std::endl;
    std::cout << "Current value: " << setting << std::endl;
    std::cout << "Accepted range: " << min << " - " << max << std::endl;
    std::cout << "Reccomended range: " << recMin << " - " << recMax << std::endl;
    std::cout << "Notes: " << notes << std::endl;
    std::string newVal;
    

    //checks if user wants to cancel
    while(newVal.compare("cancel"))
    {
        std::cout << "Enter the new value or “cancel” to go back to the settings page: ";
        std::cin >> newVal;

        std::cout << std::endl;

        //check input
        if(validateInput(newVal))
        {
            //input is an integer
            int newintVal = std::stoi(newVal);

            //check if input is within allowed range
            if(newintVal < min)
            {
                std::cout << "Invalid input. " << name << " must be greater than " << min << std::endl;
            }
            else if(newintVal > max)
            {
                std::cout << "Invalid input. " << name << " must be less than " << max << std::endl;
            }
            else
            {
                //check if input is outside reccomended range
                if(newintVal >= recMax || newintVal <= recMin)
                {
                    //input outside reccomended range
                    std::cout << "You are attempting to set " << name << " to a value outside the recommended range. This may cause problems with generation or negatively impact your experience. Are you sure you want to continue(y/n)" << std::endl;
                    std::string confirmation;
                    std::cin >> confirmation;
                    if(!confirmation.compare("y"))
                    {
                        //confirm change
                        setting = newVal;
                        break;
                    }
                    else
                    {
                        //cancel change
                        std::cout << "canceling" << std::endl;
                        break;
                    }
                }
                else
                {
                    //input is inside reccomended range
                    setting = newVal;
                    break;
                }
            }
        }
        else if(newVal.compare("cancel"))
        {
            //input is not an integer
            std::cout << "Invalid input. " << name << " must be a positive integer" << std::endl;
        }
    }
}