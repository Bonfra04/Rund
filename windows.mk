SHELL = powershell.exe

DIR_ROOT = .
DIR_BIN	= $(DIR_ROOT)\bin
DIR_OBJ	= $(DIR_ROOT)\bin-int

C_FILES := $(shell get-childitem -s $$pwd\src\rund\*.c | select-object -expandproperty fullname | foreach { get-item $$_ | resolve-path -relative })
OBJ_FILES = $(C_FILES:%.c=$(DIR_OBJ)\\%.o)

CC_FLAGS = -g -O0 -c -I $(DIR_ROOT)\include -I $(DIR_ROOT)\src -Wno-address-of-packed-member

.PHONY: all lib clean

all: clean lib
	@echo "Linking demo.exe"
	@gcc $(CC_FLAGS) $(DIR_ROOT)\src\demo.c -o $(DIR_OBJ)\demo.o
	@gcc $(DIR_OBJ)\demo.o $(DIR_BIN)\rund.a -lgdi32 -o $(DIR_BIN)/demo.exe

lib: $(OBJ_FILES)
	@if ((test-path "$(DIR_BIN)") -bxor 1) { new-item -path $$pwd -name "$(DIR_BIN)" -itemtype "directory" >$$null 2>&1 }
	@ar rcs $(DIR_BIN)\rund.a $(OBJ_FILES)

clean:
	@if (test-path "$(DIR_BIN)") { remove-item -path "$(DIR_BIN)" -recurse }
	@if (test-path "$(DIR_OBJ)") { remove-item -path "$(DIR_OBJ)" -recurse }

$(OBJ_FILES): $(DIR_OBJ)\\%.o: %.c
	@echo "Compiling $(shell powershell /c split-path $< -leaf)"
	@if ((test-path "$(shell powershell /c split-path -path $@)") -bxor 1) { new-item -path $$pwd -name "$(shell powershell /c split-path -path $@)" -itemtype "directory" >$$null 2>&1 }
	@gcc $(CC_FLAGS) $< -o $@