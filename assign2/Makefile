CALL: all
all : test_assign2_1 

test_assign2_1 : test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c
	gcc -o test_assign2_1 test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c

.CALL: clean
clean: test_assign2_1 
	rm *.o test_assign2_1 
