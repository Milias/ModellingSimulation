CXXFLAGS += -std=c++14 -pedantic -Wall -O3 -Iheader -I../Shared/Selene/include -I../Shared/json -I../Shared/header
LDFLAGS += -llua
TARGET = bin/week1

CPP = $(wildcard *.cpp) $(wildcard ../Shared/*.cpp)
OBJ = $(notdir $(CPP:.cpp=.o))

all : $(OBJ)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $(CPP) $(LDFLAGS)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
