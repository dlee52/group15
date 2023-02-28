
DIR     =
MDL     =
CC      = gcc 
CFLAG   = -c 
LINK    = gcc
LFLAG   = gcc

SOURCE  = test_assign2_1
OBJ     = $(SOURCE).o \
			dberror.o \
			storage_mgr.o \
			buffer_mgr.o 


EXE     = $(SOURCE)

$(EXE) : $(OBJ)
	$(LINK) $(OBJ) -o $(EXE)

$(SOURCE).o  : $(SOURCE).c  
	$(CC) $(CFLAG) $(SOURCE).c
	
dberror.o: dberror.c
	    $(CC) $(CFLAG) dberror.c

storage_mgr.o: storage_mgr.c
	    $(CC) $(CFLAG) storage_mgr.c

buffer_mgr.o: buffer_mgr.c
	    $(CC) $(CFLAG) buffer_mgr.c


