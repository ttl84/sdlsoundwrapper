CC:=g++
#compile and link flags
ifeq ($(OS), Windows_NT)
	RM = del
	BIN = a.exe
	LDFLAGS += -lMingw32
else
	RM = rm
	BIN = a.out
endif
INCLUDE_DIR := -I"include/sdlsoundwrapper" -I"../SDL2/include"
LIB_DIR := lib
LDFLAGS+=
CFLAGS+=-std=c++11 -pedantic-errors -Wstrict-aliasing=0 -Wall -g -DDEBUG $(INCLUDE_DIR)

#file names and directories
DIR=src
SRC=$(wildcard $(DIR:%=%/*.cpp))
OBJ=$(SRC:.cpp=.o)
DEP=$(SRC:.cpp=.d)

#rules
all: $(OBJ)
	ar rvs $(LIB_DIR)/libsdlsoundwrapper.a $(OBJ)
-include $(DEP)
%.o: %.cpp
	$(CC) $< $(CFLAGS) -c -o $@ -MMD -MP 

run: $(BIN)
	$(BIN)
clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
.PHONY: all run clean
