MAPGENPATH = ./Map\ Generator/
MAPUPPATH = ./Map\ Updater/
UIPATH = ./User\ interface/
EPROCPATH = ./Event\ Processor/

main : Game mapGenerator UI mapUpdater eventProcessor

UI : $(UIPATH)UI.cpp $(UIPATH)UI.h messageParser.h ports.h $(UIPATH)settings.h
	g++ -Wall -std=c++11 -lzmq -g -o UI $(UIPATH)UI.cpp

Game : Game.cpp room.h messageParser.h Game.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o Game Game.cpp

mapGenerator : $(MAPGENPATH)mapGenerator.cpp room.h $(MAPGENPATH)map.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o mapGenerator $(MAPGENPATH)mapGenerator.cpp

mapUpdater : $(MAPUPPATH)mapUpdater.cpp room.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o mapUpdater $(MAPUPPATH)mapUpdater.cpp

eventProcessor : $(EPROCPATH)eventProcessor.cpp room.h messageParser.h ports.h
	g++ -Wall -std=c++11 -lzmq -g -o eventProcessor $(EPROCPATH)eventProcessor.cpp

run : Game mapGenerator UI
	./mapGenerator &
	./Game &
	./mapUpdater &
	./eventProcessor &
	./UI
