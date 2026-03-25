compile_copy:
	gcc -I lib/ -o fileio/copy fileio/copy.c lib/error_functions.c
	
compile_seek:
	gcc -I lib/ -o fileio/seek fileio/seek.c lib/error_functions.c lib/get_num.c
	
compile_tee:
	gcc -I lib/ -o exercise/chapter-4/tee exercise/chapter-4/tee.c lib/error_functions.c
	
compile_hcp:
	gcc -I lib/ -o exercise/chapter-4/hcp exercise/chapter-4/hcp.c lib/error_functions.c