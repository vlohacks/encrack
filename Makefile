CC		= gcc
CXX		= g++
OBJ		= encrack.o MatcherOptions.o MatcherFirstAscii.o MatcherLibMagic.o MatcherFactory.o Wordlist.o Subject.o Worker.o
BIN		= encrack
CXXFLAGS	= -Wall -O2
LDFLAGS		= -lpthread -lcrypto -lmagic

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

.PHONY: clean
clean: 
	rm -f $(BIN) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<





