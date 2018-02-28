all: prog3ipc

prog3ipc: prog3ipc.o
	gcc prog3ipc.o -o prog3ipc -g -lrt
	
prog3ipc.o: prog3ipc.c
	gcc -c prog3ipc.c -o prog3ipc.o -g -lrt
	
checkmem: prog3ipc
	valgrind --track-origins=yes ./prog3ipc $(ARGS)

run: prog3ipc
	./prog3ipc $(ARGS)

clean:
	rm *.o prog3ipc vgcore.*