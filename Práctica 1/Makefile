# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean runv createDirs

# Rules declaration
all: clean createDirs afin hill

createDirs:
	@mkdir -p $(OBJDIR)

afin: $(OBJDIR)/afin.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o
	@echo Compiling afin...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Afin compilation completed

hill: $(OBJDIR)/hill.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o $(OBJDIR)/matrix.o
	@echo Compiling hill...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Hill compilation completed

$(OBJDIR)/afin.o: src/afin.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/hill.o: src/hill.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/alphabet.o: srclib/alphabet.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/matrix.o: srclib/matrix.c includes/matrix.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) afin hill
	@echo Project cleaned

# Valgrind rules
runv_afin:
	@echo Debugging afin with valgrind...
	@valgrind --leak-check=full ./afin -C -m 26 -a 11 -b -3

runv_hill:
	@echo Debugging hill with valgrind...
	@valgrind --leak-check=full ./hill -C -m 26 -n 3 -k matrix/matrix.txt
