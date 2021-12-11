# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs potencia primo

createDirs:
	@mkdir -p $(OBJDIR)

potencia: $(OBJDIR)/potencia.o
	@echo Compiling potencia...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Potencia compilation completed

primo: $(OBJDIR)/primo.o $(OBJDIR)/miller-rabin.o
	@echo Compiling primo...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Primo compilation completed

$(OBJDIR)/potencia.o: src/potencia.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/primo.o: src/primo.c includes/miller-rabin.h includes/firstPrimes.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/miller-rabin.o: srclib/miller-rabin.c includes/miller-rabin.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) potencia primo
	@echo Project cleaned

# Valgrind rules
runv_potencia:
	@echo Debugging potencia with valgrind...
	@valgrind --leak-check=full ./potencia 2 3 18

runv_primo:
	@echo Debugging primo with valgrind...
	@valgrind --leak-check=full ./primo -b 16 -p 90
