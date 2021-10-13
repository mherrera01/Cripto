# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj
OBJECTS = $(OBJDIR)/afin.o $(OBJDIR)/euclides.o
PROGRAM = afin

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

$(OBJDIR)/afin.o: src/afin.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: src/euclides.c src/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) $(PROGRAM)
	@echo Project cleaned

runv:
	@echo Debugging with valgrind...
	@valgrind --leak-check=full ./$(PROGRAM) -C -m 26 -a 11 -b -3
