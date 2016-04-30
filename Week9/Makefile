CXXFLAGS += -std=c++14 -pedantic -Wall -g -Iheader -I../Shared/json -I../Shared/header
TARGET = bin/week9

CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

LATEX = $(wildcard report/latex/*.tex)
LATEXDIR = report/latex

.PHONY : all
all : bin data shared $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(wildcard ../Shared/*.o) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LDFLAGS) -o $@

bin :
	mkdir -p bin

data :
	mkdir -p data

shared :
	$(MAKE) -C ../Shared

.PHONY : latex
latex : $(LATEX)
	$(MAKE) -C $(LATEXDIR)

.PHONY : clean
clean :
	rm -f $(TARGET)
	rm -f *.o
	$(MAKE) -C $(LATEXDIR) clean
	$(MAKE) -C ../Shared clean
