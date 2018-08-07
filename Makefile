CC		= gcc
CXX		= g++
OBJ		= encrack.o
BIN		= encrack
CXXFLAGS	= -Wall -O2
LDFLAGS		= -lpthread -lcrypto

all: $(OBJ)
	$(CXX) $(CCFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

.PHONY: clean
clean: 
	rm -f $(BIN) $(OBJ)

%.o: %.cpp
	$(CXX) $(CCFLAGS) -c $<





