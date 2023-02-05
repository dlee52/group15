
DIR     =
MDL     =
CC      = gcc 
CFLAG   = -c 
LINK    = gcc
LFLAG   = gcc

SOURCE  = storage_mgr
OBJ     = $(SOURCE).o \
			dberror.o \
			storage_mgr_page.o 

EXE     = $(SOURCE)

$(EXE) : $(OBJ)
	$(LINK) $(OBJ) -o $(EXE)

$(SOURCE).o  : $(SOURCE).c  
	$(CC) $(CFLAG) $(SOURCE).c
	
dberror.o: dberror.c
	    $(CC) $(CFLAG) dberror.c
storage_mgr_page.o: storage_mgr_page.c
	    $(CC) $(CFLAG) storage_mgr_page.c
