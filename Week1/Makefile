CXXFLAGS += -std=c++14 -pedantic -Wall -O3 -Iheader -I../Shared/json -I../Shared/header
TARGET = bin/week1

CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

.PHONY : all
all : bin shared $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(wildcard ../Shared/*.o) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LDFLAGS) -o $@

bin :
	mkdir -p bin

shared :
	$(MAKE) -C ../Shared

.PHONY : latex
latex :

.PHONY : clean
clean :
	rm -f $(TARGET)
	rm -f *.o
