# Macros definition
CC = gcc
CFLAGS = -g -Wall -pedantic

OBJDIR = obj

# We specify that the files are not related with the rules, so that the
# makefile will not search for a file called as a rule
.PHONY: all clean createDirs

# Rules declaration
all: clean createDirs seg-perf desECB desCFB TDEA-CFB DESSboxesLinearity AESSboxesLinearity avalancha

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

TDEA-CFB: $(OBJDIR)/TDEA-CFB.o $(OBJDIR)/bits.o $(OBJDIR)/des.o
	@echo Compiling TDEA-CFB...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo TDEA-CFB compilation completed

DESSboxesLinearity: $(OBJDIR)/DESSboxesLinearity.o $(OBJDIR)/bits.o
	@echo Compiling DESSboxesLinearity...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo DESSboxesLinearity compilation completed

AESSboxesLinearity: $(OBJDIR)/AESSboxesLinearity.o $(OBJDIR)/bits.o
	@echo Compiling AESSboxesLinearity...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo AESSboxesLinearity compilation completed

avalancha: $(OBJDIR)/avalancha.o $(OBJDIR)/bits.o $(OBJDIR)/des.o
	@echo Compiling Avalancha...
	@$(CC) $(CFLAGS) -o$@ $^
	@echo Avalancha compilation completed

$(OBJDIR)/seg-perf.o: src/seg-perf.c includes/alphabet.h includes/euclides.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/desCFB.o: src/desCFB.c includes/bits.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/desECB.o: src/desECB.c includes/bits.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/TDEA-CFB.o: src/TDEA-CFB.c includes/bits.h includes/des.h
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/DESSboxesLinearity.o: src/DESSboxesLinearity.c includes/bits.h includes/DES_tables.h 
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/AESSboxesLinearity.o: src/AESSboxesLinearity.c includes/bits.h includes/AES_tables.h 
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/avalancha.o: src/avalancha.c includes/bits.h includes/des.h
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
	@rm -rf $(OBJDIR) seg-perf desCFB desECB TDEA-CFB DESSboxesLinearity AESSboxesLinearity avalancha
	@echo Project cleaned

# Valgrind rules
runv_seg-perf:
	@echo Debugging seg-perf with valgrind...
	@valgrind --leak-check=full ./seg-perf -P

runv_desECB:
	@echo Debugging desECB with valgrind...
	@valgrind --leak-check=full ./desECB -C -o testfiles/cifrado_desECB.bin

runv_desCFB:
	@echo Debugging desCFB with valgrind...
	@valgrind --leak-check=full ./desCFB -C -iv 76BA344C7F3B5E49 -S 32 -o testfiles/cifrado_desCFB.bin

runv_TDEA-CFB:
	@echo Debugging TDEA-CFB with valgrind...
	@valgrind --leak-check=full ./TDEA-CFB -C -iv 76BA344C7F3B5E49 -S 32 -o testfiles/cifrado_TDEA-CFB.bin

runv_DESSboxesLinearity:
	@echo Debugging DESSboxesLinearity with valgrind...
	@valgrind --leak-check=full ./DESSboxesLinearity

runv_AESSboxesLinearity:
	@echo Debugging AESSboxesLinearity with valgrind...
	@valgrind --leak-check=full ./AESSboxesLinearity
