main : Game mapGenerator UI mapUpdater

UI : UI.cpp UI.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o UI UI.cpp

Game : Game.cpp map.h room.h messageParser.h Game.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o Game Game.cpp

mapGenerator : mapGenerator.cpp room.h map.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o mapGenerator mapGenerator.cpp

mapUpdater : mapUpdater.cpp room.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o mapUpdater mapUpdater.cpp

run : Game mapGenerator UI
	./mapGenerator&
	./Game&
	./UI
