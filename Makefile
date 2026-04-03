compile_copy:
	gcc -I lib/ -o fileio/copy fileio/copy.c lib/error_functions.c
	
compile_seek:
	gcc -I lib/ -o fileio/seek fileio/seek.c lib/error_functions.c lib/get_num.c
	
compile_tee:
	gcc -I lib/ -o exercise/chapter-4/tee exercise/chapter-4/tee.c lib/error_functions.c
	
compile_hcp:
	gcc -I lib/ -o exercise/chapter-4/hcp exercise/chapter-4/hcp.c lib/error_functions.c

.PHONY: compile_5.2
compile_5.2:
	gcc -I lib/ -o exercise/chapter-5/5.2 exercise/chapter-5/5.2.c lib/error_functions.c

.PHONY: compile_5.3
compile_5.3:
	gcc -I lib/ -o exercise/chapter-5/atomic_append exercise/chapter-5/5.3.c lib/error_functions.c lib/get_num.c

.PHONY: compile_5.4
compile_5.4:
	gcc -I lib/ -o exercise/chapter-5/dup_and_dup2 exercise/chapter-5/5.4.c lib/error_functions.c lib/get_num.c

.PHONY: compile_5.5
compile_5.5:
	gcc -I lib/ -o exercise/chapter-5/5.5 exercise/chapter-5/5.5.c lib/error_functions.c

.PHONY: compile_5.7
compile_5.7:
	gcc -I lib/ -o exercise/chapter-5/readv_writev exercise/chapter-5/5.7.c lib/error_functions.c
