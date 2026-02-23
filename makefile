CC=g++
CFLAGS=-Wall -std=c++17 -O3
HEADERS=BStarTree.h Contour.h FixedOutlineFloorplanning.h FloorPlanAlogrithm.h FloorplanFile.h HardModule.h Module.h Net.h Probability.h SettingParameters.h SoftModule.h TreePrinter.h Cost.h
SOURCES=FixedOutlineFloorplanning.cpp 
OBJECTS=$(SOURCES: .cpp=.o)
EXECUTABLE=../bin/main

all: $(SOURCES) $(EXECUTABLE) $(EXECUTABLE2)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -lpthread -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE) $(EXECUTABLE2)