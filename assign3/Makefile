CC = gcc

CFLAGS = -g -Wall

SOURCE = storage_mgr.c \
         buffer_mgr.c \
         buffer_mgr_stat.c \
         record_mgr.c \
         rm_serializer.c \
         expr.c \
         dberror.c

EXE = test_assign3_1 

.CALL: all
all : $(EXE)

test_assign3_1: test_assign3_1.c $(SOURCE)
	$(CC) $(CFLAGS) -o test_assign3_1 test_assign3_1.c $(SOURCE)

.CALL: clean
clean:
	@rm -f $(EXE)
