#include "UI.h"
#include "../ports.h"
#include <algorithm>
#include "../commands.h"
#include <chrono>
#include "settings.h"

static std::string commandNames[NUM_COMMANDS] = {"quit", "moveNorth", "moveSouth", "moveEast", "moveWest", "help", "restart", "save", "load"};
static std::string commandDescriptions[NUM_COMMANDS] = {"Closes the game", "Moves player north", "Moves player south", "Moves player east", "Moves player west", "Opens help menu", "Restarts the game" ,"Saves your progress to a file", "Loads game from file"};



int main()
{
    // construct a REQ (request) socket and connect to interface
    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
    socket.connect("tcp://localhost:" + std::string(UI_PORT));

    Settings settings;
    initializeSettings(settings);

    Message generationSettings;

    //intro message
    std::cout << "Welcome to my very cool and epic untitled game. In this game you run around a dungeon, collect gold, and bring it back to where you came in while avoiding monsters and traps." << std::endl;

    settingsPage(generationSettings, socket, settings);
    
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
        std::cout << message["roomDesc"] << std::endl;
        std::cout << message["eventText"] << std::endl << std::endl;

        if(!message["gameOver"].compare("True"))
        {
            break;
        }

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
        while(true)
        {
            //get input
            std::cout << "What would you like to do? ";
            std::string input;
            std::cin >> input;

            if(processCommand(input, commandAvailablity, command) == 1) break;
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

//return -1 if invalid command, return 0 if valid but game needs annother input, return 1 if valid command
int processCommand(std::string input, std::vector<bool> commandAvailablity, Message & command)
{
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
                    return 0;
                }
                else{
                    //valid input given
                    command.addData("command", std::to_string(i));
                    return 1;
                }
                
            }

            //command used exists, but cannot be used
            std::cout << commandNames[i] << " is not available right now" <<  std::endl;
            return -1;
        }
    }

    std::cout << "Command not recognized. Please pick one from the list above" << std::endl;
    return -1;
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

void changeSettings(Settings & settings)
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

        if(!userInput.compare("done")) break;

        if(!userInput.compare("default"))
        {
            //reset all settings to default values
            setDefaultVals(settings);
            continue;
        }

        //if user input a valid setting, move to update screen
        if(settings.find(userInput) != settings.end())
        {
            updateSetting(settings[userInput]);
        }

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

void updateSetting(Setting & setting)
{
    std::string description = setting.description;
    std::string name = setting.name;
    int defaultVal = setting.defaultVal;
    int currentVal = setting.value;
    int min = setting.min;
    int max = setting.max;
    int recMin = setting.recMin;
    int recMax = setting.recMax;
    std::string notes = setting.notes;

    //print setting info
    std::cout << description << std::endl;
    std::cout << "Default value: " << defaultVal << std::endl;
    std::cout << "Current value: " << currentVal << std::endl;
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
                        setting.value = newintVal;
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
                    setting.value = newintVal;
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

void settingsPage(Message & generationSettings, zmq::socket_t & socket, Settings& settings)
{
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
    setDefaultVals(settings);

    //change settings user wants to
    if(settingMode == 1)
    {
        std::cout << "-----------------------------------" << std::endl << std::endl;
        changeSettings(settings);
    }

    addSettingsToMessage(settings, generationSettings);

    //send settings to game microservice
    socket.send(zmq::buffer(generationSettings.toString()), zmq::send_flags::none);
}

void addSettingsToMessage(Settings & settings, Message & message)
{
    //add parameters to message
    message.addData("width", settings["width"].getVal());
    message.addData("height", settings["height"].getVal());
    message.addData("numRooms", settings["numRooms"].getVal());
    message.addData("numRoomType" + std::to_string(GOLD), settings["numGold"].getVal());
    message.addData("numRoomType" + std::to_string(MONSTER), settings["numMonster"].getVal());
    message.addData("numRoomType" + std::to_string(TRAP), settings["numTrap"].getVal());
}

//set values for dungeon settings to default
void setDefaultVals(Settings & settings)
{
    for(auto it = settings.begin(); it != settings.end(); it++)
    {
        std::string key = it->first;
        settings[key].value = settings[key].defaultVal;
    }
}

//set up settings structs
void initializeSettings(Settings& settings)
{
    for(int i = 0; i < NUM_SETTINGS; i++)
    {
        Setting newSetting;
        newSetting.name = SETTING_NAMES[i];
        newSetting.value = newSetting.defaultVal = SETTING_DEFAULTS[i];
        newSetting.min = SETTING_MINS[i];
        newSetting.max = SETTING_MAXES[i];
        newSetting.recMax = SETTING_REC_MAXES[i];
        newSetting.recMin = SETTING_REC_MINS[i];
        newSetting.notes = SETTING_NOTES[i];
        newSetting.description = SETTING_DESCRIPTIONS[i];

        settings[SETTING_NAMES[i]] = newSetting;
    }
}