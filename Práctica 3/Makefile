# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs potencia

createDirs:
	@mkdir -p $(OBJDIR)

potencia: $(OBJDIR)/potencia.o
	@echo Compiling potencia...
	@$(CC) $(CFLAGS) -o$@ $^ -lgmp -L.
	@echo Potencia compilation completed

$(OBJDIR)/potencia.o: src/potencia.c
	@$(CC) $(CFLAGS) -c $< -o $@
clean:
	@rm -rf $(OBJDIR) potencia
	@echo Project cleaned

# Valgrind rules
runv_potencia:
	@echo Debugging potencia with valgrind...
	@valgrind --leak-check=full ./potencia 2 3 18
