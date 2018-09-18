
.PHONY:one clean


SRCCXX=$(wildcard *.cpp)
BINCXX=$(SRCCXX:%.cpp=%)

CC=gcc
CCFLAGES=-Wall
CXX=g++
CXXFLAGES=-Wall -std=c++11


one:$(BINCXX)



$(BINCXX):%:%.cpp
	$(CXX) $(CXXFLAGES) $^ -o $@

clean:
	rm -f $(BINCXX)
