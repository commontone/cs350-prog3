#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int i = 0;
	int procs = -1;
	char *end;

	printf("Hello, World!\n");

	if(argc != 2 || strstr(argv[1],"-u") != NULL) {
		fprintf(stderr, "This program demonstrates Pipes, Shared Memory, and Signals\n");
		fprintf(stderr, "prog3ipc <num-processes>\n");
		exit(1);
	}

	procs = (int)strtol(argv[1], &end, 10);
	if(*end) {
		fprintf(stderr, "Bad input detected. ASCII value: %d. Try again.\n", (int)(*end));
		fprintf(stderr, "This program demonstrates creating child processes of N recursive levels and M children per level\n");
		fprintf(stderr, "prog2tree [-u] [-N <num-levels>] [-M <num-children>] [-p] [-s <sleep-time]\n");
		exit(1);
	}
	if(procs > 32 || procs < 1) {
		fprintf(stderr, "The number of processes must be within [1,32]\n");
		fprintf(stderr, "This program demonstrates creating child processes of N recursive levels and M children per level\n");
		fprintf(stderr, "prog2tree [-u] [-N <num-levels>] [-M <num-children>] [-p] [-s <sleep-time]\n");
		exit(1);
	}
	printf("Asked for %d process(es).\n", procs);

	pid_t pid;
	pid = fork();
	if(pid < 0) { /* error */
		fprintf(stderr, "Fork failed");
		return 1;
	}
	if(pid == 0) { /*child process */
		printf("I'm the child!\n");

	} else { /* Parent Process */
		/* Parent will wait for the child to complete */
		wait(NULL);
		printf("Child Complete\n");
	}

	return 0;
}