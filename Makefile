DIR_ROOT = .
DIR_BIN	= ./bin
DIR_OBJ	= ./bin-int

C_FILES = $(shell find . -name "*.c")
OBJ_FILES = $(C_FILES:%.c=$(DIR_OBJ)/%.o)

CC_FLAGS = -g -O0 -c -I $(DIR_ROOT)/include

.PHONY: all clean

all: clean $(OBJ_FILES)
	@mkdir -p $(DIR_BIN)
	@echo "Linking rund.elf"
	@gcc $(OBJ_FILES) -lX11 -lGL -o $(DIR_BIN)/rund.elf

clean:
	@rm -rf $(DIR_BIN) $(DIR_OBJ)

$(OBJ_FILES): $(DIR_OBJ)/%.o: %.c
	@echo "Compiling $(shell basename $<)"
	@mkdir -p $(shell dirname $@)
	@gcc $(CC_FLAGS) $< -o $@