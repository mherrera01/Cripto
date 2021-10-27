# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs afin hill vigenere flujo permutacion IC kasiski

createDirs:
	@mkdir -p $(OBJDIR)

afin: $(OBJDIR)/afin.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o
	@echo Compiling afin...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Afin compilation completed

hill: $(OBJDIR)/hill.o $(OBJDIR)/alphabet.o $(OBJDIR)/euclides.o $(OBJDIR)/matrix.o $(OBJDIR)/padding.o
	@echo Compiling hill...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Hill compilation completed

vigenere: $(OBJDIR)/vigenere.o $(OBJDIR)/alphabet.o
	@echo Compiling vigenere...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo Vigenere compilation completed

flujo: $(OBJDIR)/flujo.o $(OBJDIR)/alphabet.o
	@echo Compiling flujo...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo Flujo compilation completed

permutacion: $(OBJDIR)/permutacion.o $(OBJDIR)/alphabet.o $(OBJDIR)/matrixMN.o $(OBJDIR)/padding.o
	@echo Compiling permutacion...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo Permutacion compilation completed

IC: $(OBJDIR)/IC.o $(OBJDIR)/alphabet.o $(OBJDIR)/coincidencia.o
	@echo Compiling IC...
	@$(CC) $(CFLAGS) -o$@ $^ -lm
	@echo IC compilation completed

kasiski: $(OBJDIR)/kasiski.o $(OBJDIR)/alphabet.o $(OBJDIR)/coincidencia.o
	@echo Compiling kasiski...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo Kasiski compilation completed

$(OBJDIR)/afin.o: src/afin.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/hill.o: src/hill.c includes/alphabet.h includes/matrix.h includes/padding.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/vigenere.o: src/vigenere.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/flujo.o: src/flujo.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/permutacion.o: src/permutacion.c includes/alphabet.h includes/matrixMN.h includes/padding.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/IC.o: src/IC.c includes/coincidencia.h includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/kasiski.o: src/kasiski.c includes/coincidencia.h includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/alphabet.o: srclib/alphabet.c includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/euclides.o: srclib/euclides.c includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/matrix.o: srclib/matrix.c includes/matrix.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/matrixMN.o: srclib/matrixMN.c includes/matrixMN.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/padding.o: srclib/padding.c includes/padding.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/coincidencia.o: srclib/coincidencia.c includes/coincidencia.h includes/alphabet.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) afin hill vigenere flujo permutacion IC kasiski
	@echo Project cleaned

# Valgrind rules
runv_afin:
	@echo Debugging afin with valgrind...
	@valgrind --leak-check=full ./afin -C -m 26 -a 11 -b -3

runv_hill:
	@echo Debugging hill with valgrind...
	@valgrind --leak-check=full ./hill -C -m 26 -n 3 -k matrix/matrix.txt

runv_vigenere:
	@echo Debugging vigenere with valgrind...
	@valgrind --leak-check=full ./vigenere -C -k CIPHER

runv_flujo:
	@echo Debugging flujo with valgrind...
	@valgrind --leak-check=full ./flujo -C -k 54362 -o testfiles/cifrado_flujo.txt

runv_permutacion:
	@echo Debugging permutacion with valgrind...
	@valgrind --leak-check=full ./permutacion -C -k1 0,2,3,1 -k2 2,1,3,0

runv_IC:
	@echo Debugging IC with valgrind...
	@valgrind --leak-check=full ./IC -i testfiles/cifrado_vigenere.txt

runv_kasiski:
	@echo Debugging kasiski with valgrind...
	@valgrind --leak-check=full ./kasiski -s 6 -i testfiles/cifrado_vigenere.txt
