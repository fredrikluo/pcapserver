CXX = gcc
CXXFLAGS =
LDFLAGS =
LIBS = -lstdc++ -lpcap  -lexpat 
LD = gcc
BIN = pcapserver
SRC = $(shell find . -name *.cpp)
OBJ = $(SRC:%.cpp=%.o)

debug: CXXFLAG += -O0 -g -Wall -DDEBUG
debug: $(BIN)

release: CXXFLAG += -O2
release: $(BIN)

$(BIN): $(OBJ)
	$(LD) $(LDFLAGS)  -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -MMD -MT -MF -c $< -o $@

clean:
	rm -rf src/*.o  src/*.d pcapserver


