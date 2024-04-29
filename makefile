main : Game mapGenerator UI

UI : UI.cpp UI.h messageParser.h
	g++ -Wall -std=c++11 -lzmq -o UI UI.cpp

Game : Game.cpp map.h room.h messageParser.h Game.h
	g++ -Wall -std=c++11 -lzmq -o Game Game.cpp

mapGenerator : mapGenerator.cpp room.h map.h messageParser.h
	g++ -Wall -std=c++11 -lzmq -o mapGenerator mapGenerator.cpp

