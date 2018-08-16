# Generic GNUMakefile
ifneq (,)
This makefile requires GNU Make.
endif

PROGRAM = log-converter
SRC_DIR = .
OBJ_DIR = obj

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_FILES))
CC =  g++
CFLAGS = -Wall -pedantic -pedantic-errors -I. -I$(SRC_DIR)
LDFLAGS = 

all: $(PROGRAM)

$(PROGRAM): .depend $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(PROGRAM)

depend: .depend

.depend: cmd = $(CC) -MM -MF depend $(var); cat depend >> $(OBJ_DIR)/.depend;
.depend:
	@echo "Generating dependencies..."
	@mkdir -p $(OBJ_DIR)
	@$(foreach var, $(CPP_FILES), $(cmd))
	@rm -f depend

-include .depend

# These are the pattern matching rules. In addition to the automatic
# variables used here, the variable $* that matches whatever % stands for
# can be useful in special cases.
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%:$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f depend $(OBJ_DIR)/.depend $(OBJ_DIR)/*.o $(OBJ_DIR)/$(PROGRAM)

.PHONY: clean depend
