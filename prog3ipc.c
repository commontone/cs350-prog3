#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv) {
	int i = 0;
	int procs = -1;
	char *end;
	const char *name = "myshare";
	const int SIZE = 4096;
	int sharefd;
	void *share;

	if(argc != 2 || strstr(argv[1],"-u") != NULL) {
		fprintf(stderr, "This program demonstrates Pipes, Shared Memory, and Signals\n");
		fprintf(stderr, "prog3ipc <num-processes>\n");
		exit(1);
	}

	procs = (int)strtol(argv[1], &end, 10);
	if(*end) {
		fprintf(stderr, "Bad input detected. ASCII value: %d. Try again.\n", (int)(*end));
		fprintf(stderr, "This program demonstrates Pipes, Shared Memory, and Signals\n");
		fprintf(stderr, "prog3ipc <num-processes>\n");
		exit(1);
	}
	if(procs > 32 || procs < 1) {
		fprintf(stderr, "The number of processes must be within [1,32]\n");
		fprintf(stderr, "This program demonstrates Pipes, Shared Memory, and Signals\n");
		fprintf(stderr, "prog3ipc <num-processes>\n");
		exit(1);
	}
	printf("Asked for %d process(es).\n", procs);

	pid_t pid;
	pid = fork();
	if(pid < 0) { /* error */
		fprintf(stderr, "Fork failed");
		exit(1);
	}
	if(pid == 0) { /*child process */
		printf("I'm the child!\n");

	} else { /* Parent Process */
		/* Parent will wait for the child to complete */
		printf("I'm the parent.\n");
		/* Create the memory 'file' location */
		sharefd = shm_open(name, O_CREAT | O_RDWR, 2468);
		/* Cut the memory space to size */
		ftruncate(sharefd, SIZE);
		/* memory map the shared memory object */
		share = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, sharefd, 0);
		int sendval = getpid();
		int* send = &sendval;
		memcpy(share,send,sizeof(sendval));
		printf("%d\n", (int)(*(int*)share) );
		
		wait(NULL);
		printf("Child Complete\n");
		int res = munmap(share, SIZE);
		printf("Unmap result: %d\n",res);
	}

	return 0;
}















