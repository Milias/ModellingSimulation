CXXFLAGS += -std=c++14 -pedantic -Wall -O3 -Iheader -I../Shared/json -I../Shared/header
TARGET = bin/week1

CPP = $(wildcard ../Shared/*.cpp) $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

all : bin $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LDFLAGS) -o $@

bin :
	mkdir -p bin

.PHONY : clean
clean :
	rm -f $(TARGET)
	rm -f *.o
	rm -f ../Shared/*.o
