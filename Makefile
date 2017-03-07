# This is the makefile that generates the executable

# Files to compile
FILES_C = main.c red-black-tree.c linked-list.c create-tree.c tree-utils.c

# Exectuable to generate
TARGET = main

# Compilation options
CFLAGS = -Wall -Werror -O

# Linker options 
LFLAGS = -lm 

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.

$(TARGET): $(FILES_O) Makefile
	gcc $(CFLAGS) $(FILES_C) -o $(TARGET) $(LFLAGS) 

all: $(TARGET) 

clean:
	/bin/rm $(TARGET) 
