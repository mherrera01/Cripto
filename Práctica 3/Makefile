# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs potencia primo vegas

createDirs:
	@mkdir -p $(OBJDIR)

potencia: $(OBJDIR)/potencia.o
	@echo Compiling potencia...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Potencia compilation completed

primo: $(OBJDIR)/primo.o $(OBJDIR)/randomPrime.o $(OBJDIR)/miller-rabin.o
	@echo Compiling primo...
	@$(CC) $(CFLAGS) -o$@ $^ -lm -lgmp -L.
	@echo Primo compilation completed

vegas: $(OBJDIR)/vegas.o $(OBJDIR)/randomPrime.o $(OBJDIR)/miller-rabin.o $(OBJDIR)/euclides.o
	@echo Compiling vegas...
	@$(CC) $(CFLAGS) -o$@ $^ -lm -lgmp -L.
	@echo Vegas compilation completed

$(OBJDIR)/potencia.o: src/potencia.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/primo.o: src/primo.c includes/randomPrime.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/vegas.o: src/vegas.c includes/randomPrime.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/randomPrime.o: srclib/randomPrime.c includes/randomPrime.h includes/miller-rabin.h includes/firstPrimes.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/miller-rabin.o: srclib/miller-rabin.c includes/miller-rabin.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) potencia primo vegas
	@echo Project cleaned

# Valgrind rules
runv_potencia:
	@echo Debugging potencia with valgrind...
	@valgrind --leak-check=full ./potencia 2 3 18

runv_primo:
	@echo Debugging primo with valgrind...
	@valgrind --leak-check=full ./primo -b 16 -p 0.08

runv_vegas:
	@echo Debugging vegas with valgrind...
	@valgrind --leak-check=full ./vegas -b 16
