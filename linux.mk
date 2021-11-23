DIR_ROOT = .
DIR_BIN	= ./bin
DIR_OBJ	= ./bin-int

C_FILES = $(shell find . -wholename "./src/rund/*.c")
OBJ_FILES = $(C_FILES:%.c=$(DIR_OBJ)/%.o)

CC_FLAGS = -g -O0 -c -I $(DIR_ROOT)/include -I $(DIR_ROOT)/src -Wno-address-of-packed-member

.PHONY: all clean

all: clean lib
	@mkdir -p $(DIR_BIN)
	@echo "Linking demo.elf"
	@gcc -g -c -I $(DIR_ROOT)/include $(DIR_ROOT)/src/demo.c -o $(DIR_OBJ)/demo.o
	@gcc $(DIR_OBJ)/demo.o $(DIR_BIN)/rund.a -lX11 -lGL -o $(DIR_BIN)/demo.elf

lib: $(OBJ_FILES)
	@mkdir -p $(DIR_BIN)
	@ar rcs $(DIR_BIN)/rund.a $(OBJ_FILES)

clean:
	@rm -rf $(DIR_BIN) $(DIR_OBJ)

$(OBJ_FILES): $(DIR_OBJ)/%.o: %.c
	@echo "Compiling $(shell basename $<)"
	@mkdir -p $(shell dirname $@)
	@gcc $(CC_FLAGS) $< -o $@