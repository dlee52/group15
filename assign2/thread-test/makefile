
DIR     =
MDL     =
CC      = gcc 
CFLAG   = -c 
LINK    = gcc
LFLAG   = gcc

SOURCE  = race_cond
OBJ     = $(SOURCE).o


EXE     = $(SOURCE)

$(EXE) : $(OBJ)
	$(LINK) $(OBJ) -o $(EXE)

$(SOURCE).o  : $(SOURCE).c  
	$(CC) $(CFLAG) $(SOURCE).c
