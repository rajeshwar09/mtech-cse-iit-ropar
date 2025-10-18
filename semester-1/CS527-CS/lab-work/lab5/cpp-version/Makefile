CXX     = g++
CXXFLAGS= -Wall -Wextra -std=c++17 -O2 -Iinclude
LDFLAGS = 
SRC_DIR = src
INC_DIR = include

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/App.cpp $(SRC_DIR)/Bus.cpp $(SRC_DIR)/Devices.cpp $(SRC_DIR)/Frame.cpp $(SRC_DIR)/CRC16.cpp
OBJS = $(SRCS:.cpp=.o)
BIN  = lab5

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(BIN)

.PHONY: all clean
