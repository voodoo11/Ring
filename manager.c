#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

#define BUF_SIZE 1024

void read_line(char* filename, char** line) {
	FILE* fp;
	size_t len = 0;

	fp = fopen(filename, "r");

	if(fp == NULL) {
		syserr("Error in open file");
	}

	getline(line, &len, fp);
}

int main(int argc, char* argv[]) {
	int pipe_out[2];	/*rura do najbliższego sąsiada*/
	int pipe_in[2];		/*rura do poprzedniego sąsiada*/
	int size_of_ring = atoi(argv[1]);
	int out_dsc;
	int i;
	int buf_len;
	char buf[BUF_SIZE];
	char* line;
	char filename[100];

	if(pipe(pipe_out) == -1) syserr("E: pipe_out");
	if((dup2(pipe_out[0], STDIN_FILENO)) == -1) syserr("MANAGER: Error in dup pipe_out[0]");

	for(i = 0; i < size_of_ring; i++) {
		if(pipe(pipe_in) == -1) syserr("E: pipe_in\n");
		//if(i == size_of_ring -1) out_dsc = pipe_in[0];
		switch(fork()) {
			case -1:
				syserr("Error in fork\n");

			case 0:				
				if((dup2(pipe_in[1], STDOUT_FILENO)) == -1 ) {
					syserr("Fork: Error in pipe in dup\n");
				}

				if(close(pipe_out[0]) == -1) syserr("FORK: Error in close pipe_out[0]\n");
				if(close(pipe_in[0]) == -1) syserr("FORK: Error in close pipe_in[0]\n");
				if(close(pipe_in[1]) == -1) syserr("FORK: Error in close pipe_in[1]\n");
				if(close(pipe_out[1]) == -1) syserr("FORK: Error in close pipe_out[1]\n");

				execl("./executor", "executor\n", (char *) 0);
				syserr("Error in execl(executor).\n");

			default:

				if((dup2(pipe_in[0], STDIN_FILENO)) == -1) syserr("PARENT: Error in dup pipe_in[0]");
				if(close(pipe_in[1]) == -1) syserr("PARENT: Error in close pipe_in[1]\n");
				if(close(pipe_in[0]) == -1) syserr("PARENT: Error in close pipe_in[0]\n");

		}
	}

	if((dup2(pipe_out[1], STDOUT_FILENO)) == -1) syserr("MANAGER: error in dup pipe_out[1]");
	fprintf(stderr, "pipe_out[0]: %d\n", pipe_out[0]);
	fprintf(stderr, "pipe_out[1]: %d\n", pipe_out[1]);
	if(close(pipe_out[0]) == -1) syserr("MANAGER: Error in close pipe_out[0]");
	if(close(pipe_out[1]) == -1) syserr("MANAGER: Error in close pipe_out[1]");
	write(1, "chuj", 5);
	read(0, buf, BUF_SIZE-1);
	fprintf(stderr, "buf: %s\n", buf);
	// 	read_line("DATA/in", &line);
// /*	printf("wczytana linia:%s", line);
// 	printf("rozmiar: %d\n", strlen(line));*/
// 	write(0, line, strlen(line)+1);

// 	if((buf_len = read(0, buf, BUF_SIZE-1)) == -1) {
// 			syserr("Error in read from last child\n");
// 	}
// 	while(buf[buf_len-3] == '+' || buf[buf_len-3] == '-' || buf[buf_len-3] == '*' || buf[buf_len-3] == '/') {
// 		if((write(0, buf, strlen(buf)+1)) == -1) {
// 			syserr("Error in write to child");
// 		}
// 		if((buf_len = read(0, buf, BUF_SIZE-1)) == -1) {
// 			syserr("Error in read from last child\n");
// 		}

// 	}
// 	if((write(1, "!", 1)) == -1) {
// 		syserr("MANAGER: Error in killing babies");
// 	}
/*
	printf("znak: %c\n", buf[buf_len-2]);

	printf("len: %d \n", buf_len);*/

	//printf("\nbuf:%s!", buf);
	//wait(0);

	// char chuj[100] = "abcd";
	// int a,b;7

	return 0;

}	/*main*/