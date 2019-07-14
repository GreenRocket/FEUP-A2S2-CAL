PROG := SpeedMail

SRC := $(wildcard src/*.cpp)

OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)  # one dependency file for each source

INC1 := include

CC := g++
CPPFLAGS := -I$(INC1) -Wall -Wextra #-Werror

$(PROG): $(OBJ)
	$(CC) -o $@ $^
	cp $(PROG) $(HOME)/bin

-include $(DEP)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.cpp
	@$(CPP) $(CPPFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: all clean run

all: $(PROG)

clean:
	rm -f $(OBJ) $(DEP) $(PROG)

run: all
	./$(PROG)