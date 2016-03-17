CXXFLAGS += -std=c++14 -pedantic -Wall -O3 -Iheader -I../Shared/json -I../Shared/header

CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

.PHONY : all
all : $(OBJ)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LDFLAGS) -o $@

.PHONY : latex
latex :

.PHONY : clean
clean :
	rm -f *.o
