CXXFLAGS += -std=c++14 -pedantic -Wall -g -Iheader -I../Shared/json -I../Shared/header
TARGET = bin/week2

CPP = $(wildcard *.cpp) $(wildcard ../Shared/*.cpp)
OBJ = $(notdir $(CPP:.cpp=.o))

all : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $(CPP) $(LDFLAGS)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
