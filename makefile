main : UI mapGenerator

UI : UI.cpp map.h room.h messageParser.h UI.h
	g++ -Wall -std=c++11 -lzmq -o UI UI.cpp

mapGenerator : mapGenerator.cpp room.h map.h messageParser.h
	g++ -Wall -std=c++11 -lzmq -o mapGenerator mapGenerator.cpp

