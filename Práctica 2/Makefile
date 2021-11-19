# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs seg-perf desCFB

createDirs:
	@mkdir -p $(OBJDIR)

seg-perf: $(OBJDIR)/seg-perf.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o
	@echo Compiling seg-perf...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Seg-perf compilation completed

desCFB: $(OBJDIR)/desCFB.o $(OBJDIR)/alphabet.o $(OBJDIR)/byte.o $(OBJDIR)/byteArray.o $(OBJDIR)/des.o
	@echo Compiling desCFB...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo DesCFB compilation completed

$(OBJDIR)/seg-perf.o: src/seg-perf.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/desCFB.o: src/desCFB.c includes/alphabet.h includes/byte.h includes/byteArray.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/alphabet.o: srclib/alphabet.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/des.o: srclib/des.c includes/des.h includes/DES_tables.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/byteArray.o: srclib/byteArray.c includes/byteArray.h includes/byte.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/byte.o: srclib/byte.c includes/byte.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) seg-perf desCFB
	@echo Project cleaned

# Valgrind rules
runv_seg-perf:
	@echo Debugging seg-perf with valgrind...
	@valgrind --leak-check=full ./seg-perf -P

runv_desCFB:
	@echo Debugging desCFB with valgrind...
	@valgrind --leak-check=full ./desCFB -C -S 32
