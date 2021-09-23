# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj
OBJECTS = $(OBJDIR)/main.o
PROGRAM = main

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean runv createDirs

# Rules declaration
all: clean createDirs $(PROGRAM)

createDirs:
	@mkdir -p $(OBJDIR)

$(PROGRAM): $(OBJECTS)
	@echo Compiling...
	@$(CC) $(CFLAGS) -o$@ $(OBJECTS) -lgmp -L.
	@echo Compilation completed

$(OBJDIR)/main.o: src/main.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) $(PROGRAM)
	@echo Project cleaned

runv:
	@echo Debugging with valgrind...
	@valgrind --leak-check=full ./$(PROGRAM)
