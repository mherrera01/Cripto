# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs seg-perf

createDirs:
	@mkdir -p $(OBJDIR)

seg-perf: $(OBJDIR)/seg-perf.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o
	@echo Compiling seg-perf...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Seg-perf compilation completed

$(OBJDIR)/seg-perf.o: src/seg-perf.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/alphabet.o: srclib/alphabet.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) seg-perf
	@echo Project cleaned

# Valgrind rules
runv_seg-perf:
	@echo Debugging seg-perf with valgrind...
	@valgrind --leak-check=full ./seg-perf -P
