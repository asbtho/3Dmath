OUTPUT=doom-like
SRC=*.cpp
INCLUDE=./include
LIB=./lib

all:
	g++ $(SRC) -I$(INCLUDE) -L$(LIB)  -lSDL3 -o $(OUTPUT)

clean:
	rm $(OUTPUT)

run:
	./$(OUTPUT)
