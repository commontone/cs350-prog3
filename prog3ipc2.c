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
#include <errno.h>
#include <sys/mman.h>
#include <signal.h>

#define READ 0
#define WRITE 1

void handle_signal();

int procs = -1;
int uid = 0;
int q3 = -1;
int *j = &q3;
int main(int argc, char **argv) {
	signal(SIGINT,handle_signal);
	int i = 0;
	procs = -1;
	char *end;
	const char *name = "nobodyLovesMe.dat";
	int sharefd;
	void *share;
	int pipefd[2];

	
	
	char buffer[100];
	

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
	const int SIZE = (procs+1)*sizeof(int);
	*j = procs;
	
	pipe(pipefd);
	fprintf(stderr, "j check:  %d\n", *j);
	pid_t pid;
	pid = fork();
	if(pid < 0) { /* error */
		fprintf(stderr, "Fork failed");
		exit(1);
	}
	if(pid>0) { /* Parent Process */
		/* Create the memory 'file' location */
		sharefd = shm_open(name, O_CREAT | O_RDWR, 2468);
		/* Cut the memory space to size */
		ftruncate(sharefd, SIZE);
		/* memory map the shared memory object */
		share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
		printf("PARENT SHARE FD: %s\n", strerror(errno) );
		
		
		const char *myfifo = "/tmp/stayawayfromMYfifo.dat";
		mkfifo(myfifo, 5638);
		int fifofd = open(myfifo, O_RDONLY);
		close(pipefd[READ]);
		
		close(pipefd[WRITE]);
		
		printf("I'm the parent.\n");
		
		int sendval = getpid();
		int* send = &sendval;
		((int*)share)[0] = getpid();
		
		/* Parent will wait for the child to complete */
		sleep(1);
		
		
		printf("Child Complete\n");
		
		for(i=1;i<procs+1;i++) {
			printf("Killing this: %d\n", ((int*)share)[i] );
			kill(((int*)share)[i],SIGINT);
			printf("Sending Kill\n");
		}
		
		int res = munmap(share, SIZE);
		
		/*printf("Unmap result: %d\n",res);*/
	} else if(pid==0) { /*child process */
		*j = procs;
		fprintf(stderr, "j5 check:  %d\n", *j);
		
		uid = getppid();
		printf("ALIVE: Level %d process with pid=%d, child of ppid=%d\n", procs, getpid(), getppid());
		close(pipefd[WRITE]);
		close(pipefd[READ]);
		/* Create the memory 'file' location */
		sharefd = shm_open(name, O_RDWR, 2468);
		/* memory map the shared memory object */
		share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
		printf("CHILD SHARE FD: %s\n", strerror(errno) );
		fprintf(stderr, "%d\n", sharefd);
		((int*)share)[procs-(*j)] = getpid();
		printf("Kid PID: %d\n", getpid() );
		
		
		
		
		
		
		int loopNum = *j;
		for(loopNum = procs-1;loopNum>0;loopNum--) {
			fprintf(stderr, "j6 check:  %d\n", *j);
			pid_t pidc;
			pidc = fork();
			fprintf(stderr, "j4 check:  %d\n", *j);
			if(pidc < 0) { /* error */
				fprintf(stderr, "Fork failed");
				exit(1);
			} else if(pidc == 0) { /* Subchild */
				uid = getppid();
				printf("ALIVE: Level %d process with pid=%d, child of ppid=%d\n", procs, getpid(), getppid());
				close(pipefd[WRITE]);
				close(pipefd[READ]);
				/* Create the memory 'file' location */
				sharefd = shm_open(name, O_RDWR, 2468);
				/* memory map the shared memory object */
				share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
				printf("CHILD SHARE FD: %s\n", strerror(errno) );
				fprintf(stderr, "%d\n", sharefd);
				((int*)share)[procs-(*j)] = getpid();
				printf("Kid PID: %d\n", getpid() );
				/*printf("%d\n", ((int*)share)[0] );*/
				/*shm_unlink(name);*/
			} else { /* Subparent */
				
				break;
			}
		}
		
		
		
		pause();
	}
	fprintf(stderr, "j check:  %d\n", *j);
	return 0;
}

void handle_signal() {
	fprintf(stderr, "j check2:  %d\n", *j);
	printf("EXITING: Level %d process with pid=%d, child of ppid=%d\n", *j, getpid(), uid);
	exit(0);
	
}














