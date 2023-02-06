
DIR     =
MDL     =
CC      = gcc 
CFLAG   = -c 
LINK    = gcc
LFLAG   = gcc

SOURCE  = storage_mgr
OBJ     = $(SOURCE).o \
			dberror.o \
			storage_mgr_page.o \
			storage_mgr_rblock.o \
			storage_mgr_wblock.o \


EXE     = $(SOURCE)

$(EXE) : $(OBJ)
	$(LINK) $(OBJ) -o $(EXE)

$(SOURCE).o  : $(SOURCE).c  
	$(CC) $(CFLAG) $(SOURCE).c
	
dberror.o: dberror.c
	    $(CC) $(CFLAG) dberror.c

storage_mgr_page.o: storage_mgr_page.c
	    $(CC) $(CFLAG) storage_mgr_page.c

storage_mgr_rblock.o: storage_mgr_rblock.c
	    $(CC) $(CFLAG) storage_mgr_rblock.c

storage_mgr_wblock.o: storage_mgr_wblock.c
	    $(CC) $(CFLAG) storage_mgr_wblock.c
