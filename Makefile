SRC_FILES = $(wildcard *.c) $(wildcard obj/*.c) $(wildcard utilities/*.c)
EXE = main
BUILD_DIR = build/
OBJ = $(patsubst %.c,$(BUILD_DIR)%.o,$(SRC_FILES))
DEP = $(OBJ:.o=.d)

RAYLIB_DIR ?= $(HOME)/Documents/Coding/raylib/build/
LIB_DIRS = -L$(RAYLIB_DIR)lib
INCLUDE_DIRS = -I$(RAYLIB_DIR)include -I./include
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CFLAGS = -MMD -MP $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS) $(LIBS)

all: CFLAGS += -O2
all: $(EXE)

debug: CFLAGS += -DDEBUG -O0 -ggdb
debug: $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)%.o: %.c
	@ mkdir -p $(BUILD_DIR)
	@ mkdir -p $(BUILD_DIR)/obj
	@ mkdir -p $(BUILD_DIR)/utilities
	@ $(CC) $(CFLAGS) -o $@ -c $<

-include $(DEP)

.PHONY: cleanall
cleanall: clean cleandep

.PHONY: clean
clean:
	rm -f $(OBJ) $(EXE)

.PHONY: cleandep
cleandep:
	rm -f $(DEP)
