CC=gcc

all: hello myrestart 
 
hello: hello.c
	$(CC) -g -O0 --static myckpt.c -o hello hello.c 

myrestart: myrestart.c
	$(CC) -g -O0 -static -Wl,-Ttext=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o myrestart myrestart.c 

restart: myrestart
	./myrestart myckpt

# Bundle the .o files (there's only one, here) into an "archive" lbrary: *.a
libckpt.a: myckpt.o
	ar cr libckpt.a myckpt.o

check: libckpt.a hello
	sleep 3 && kill -12 'pgrep -n hello' && sleep 2 && pkill -9 -n hello && make restart & ./hello

clean: 
	rm -rf myckpt myrestart hello

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz