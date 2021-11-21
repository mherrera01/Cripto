# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs seg-perf desECB desCFB

createDirs:
	@mkdir -p $(OBJDIR)

seg-perf: $(OBJDIR)/seg-perf.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o
	@echo Compiling seg-perf...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Seg-perf compilation completed

desECB: $(OBJDIR)/desECB.o $(OBJDIR)/bits.o $(OBJDIR)/des.o
	@echo Compiling desECB...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo DesECB compilation completed

desCFB: $(OBJDIR)/desCFB.o $(OBJDIR)/bits.o $(OBJDIR)/des.o
	@echo Compiling desCFB...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo DesCFB compilation completed

$(OBJDIR)/seg-perf.o: src/seg-perf.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/desCFB.o: src/desCFB.c includes/bits.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/desECB.o: src/desECB.c includes/bits.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/alphabet.o: srclib/alphabet.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/des.o: srclib/des.c includes/bits.h includes/des.h includes/DES_tables.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/bits.o: srclib/bits.c includes/bits.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) seg-perf desCFB desECB
	@echo Project cleaned

# Valgrind rules
runv_seg-perf:
	@echo Debugging seg-perf with valgrind...
	@valgrind --leak-check=full ./seg-perf -P

runv_desECB:
	@echo Debugging desECB with valgrind...
	@valgrind --leak-check=full ./desECB -C

runv_desCFB:
	@echo Debugging desCFB with valgrind...
	@valgrind --leak-check=full ./desCFB -C -S 32
