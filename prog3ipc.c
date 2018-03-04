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
#include <time.h>

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
	const char *name = "rburton2_sharedmemory";
	int sharefd;
	void *share;
	int fifofd = -1;
	int pipefd[2];
	const char *myfifo = "/tmp/rburton2_fifo";

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
	const int SIZE = (procs+1)*sizeof(int);
	*j = procs;
	
	printf("ALIVE: Level %d process with pid=%d, child of ppid=%d\n", *j, getpid(), getppid());
	/* Create the memory 'file' location */
	sharefd = shm_open(name, O_CREAT | O_RDWR, 0666);
	/*fprintf(stderr, "%d\n", sharefd);*/
	/*printf("TOP SHARE FD: %s\n", strerror(errno) );*/
	/* Cut the memory space to size */
	ftruncate(sharefd, SIZE);
	/* memory map the shared memory object */
	share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
	
	((int*)share)[0] = getpid();
	*j = procs-1;
	int loopNum;
	uid = getppid();
	int begin_int = procs;
	char begin[100];
	sprintf(begin,"%d",begin_int);
	
	/* This loop is just a wrapper to support the process chain.
	   The 'recursive' nature of the process chain is independent */
	for(loopNum = procs-1; loopNum>0; loopNum--) {
		pipe(pipefd);
		pid_t pid;
		pid = fork();
		if(pid < 0) { /* error */
			fprintf(stderr, "Fork failed");
			exit(1);
		} else if(pid == 0 && procs > 1) { /* Subchild */
			
			uid = getppid();
			printf("ALIVE: Level %d process with pid=%d, child of ppid=%d\n", atoi(begin)-1, getpid(), getppid());
			/* Create the memory 'file' location */
			sharefd = shm_open(name, O_RDWR, 0666);
			/* memory map the shared memory object */
			share = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharefd, 0);
			/*printf("CHILD SHARE FD: %s\n", strerror(errno) );*/
			/*fprintf(stderr, "%d\n", sharefd);*/
			
			while(1) {
				if(read(pipefd[READ], buffer, 100 )>0) {
					/*printf("Received: %s\n", buffer);*/
					for(i=0;i<100;i++) {
						begin[i]=buffer[i];
					}
					break;
				}
			}
			*j = atoi(begin);
			((int*)share)[procs-atoi(begin)] = getpid(); /* MUST BE REPLACED BY PIPE */
			if(atoi(begin) == 1) {
				fifofd = open(myfifo, O_WRONLY);
				write(fifofd, "hey\n\0", 6);
				pause();
			}
		} else { /* Subparent */
			int beg_temp = atoi(begin);
			beg_temp = beg_temp - 1;
			sprintf(begin,"%d",beg_temp);
			write(pipefd[WRITE], begin, 6);
			
			if(loopNum == procs-1) {
				mkfifo(myfifo, 0666);
				/*printf("fifo SHARE FD: %s\n", strerror(errno) );*/
				fifofd = open(myfifo, O_RDONLY);
				while(1) {
					if(read(fifofd, buffer, 100 )>0) {
						/*printf("Received: %s\n", buffer);*/
						if (strstr(buffer,"hey\n\0")) {
							break;
						}
					}
				}
				for(i=0;i<procs;i++) {
					printf("%d\n", ((int*)share)[i] );
				}
				for(i=1;i<procs;i++) {
					/*fprintf(stderr, "Killing this: %d\n", ((int*)share)[i] );*/
					int c = kill(((int*)share)[i],SIGINT);
					waitpid(((int*)share)[i], 0, 0);
					usleep(100);
				}
				
				waitpid(((int*)share)[procs-1], 0, 0);
				/*printf("Waited for: %d\n", ((int*)share)[procs-1]);*/
				int qa;
				for(qa=5;qa<1000000;qa++) {
				}
				printf("EXITING: Level %d process with pid=%d, child of ppid=%d\n", procs, getpid(), uid);
				shm_unlink(name);
				usleep(1000);
				exit(0);
			}
			break;
		}
	}
	
	if(procs==1) {
		printf("%d\n",getpid());
		printf("EXITING: Level %d process with pid=%d, child of ppid=%d\n", 1, getpid(), uid);
		exit(0);
	}
	pause();
	return 0;
}

void handle_signal() {
	printf("EXITING: Level %d process with pid=%d, child of ppid=%d\n", *j, getpid(), uid);
	exit(0);
	
}














