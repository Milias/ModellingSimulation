CXXFLAGS += -std=c++14 -pedantic -Wall -O3 -Iheader -I../Shared/json -I../Shared/header
TARGET = bin/week3

CPP = $(wildcard ../Shared/*.cpp) $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

LATEX = $(wildcard report/latex/*.tex)
LATEXDIR = report/latex

.PHONY : all
all : bin data $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LDFLAGS) -o $@

bin :
	mkdir -p bin

data :
	mkdir -p data

.PHONY : latex
latex : $(LATEX)
	$(MAKE) -C $(LATEXDIR)

.PHONY : clean
clean :
	rm -f $(TARGET)
	rm -f *.o
	rm -f ../Shared/*.o
	$(MAKE) -C $(LATEXDIR) clean
