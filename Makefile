compile_copy:
	gcc -I lib/ -o fileio/copy fileio/copy.c lib/error_functions.c
	
compile_seek:
	gcc -I lib/ -o fileio/seek fileio/seek.c lib/error_functions.c lib/get_num.c