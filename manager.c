#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

#define BUF_SIZE 1024

void read_line(int* input_fd, char line[]) {
	size_t len = 0;
	line = NULL;

	getline(line, &len, input_fd);
}

void write_to_file(int* output_fd, char line[]) {

}

int sign(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/') {
		return 1;
	} else {
		return 0;
	}
}

int main(int argc, char* argv[]) {
	int pipe_out[2];	/*rura do najbliższego sąsiada*/
	int pipe_in[2];		/*rura do poprzedniego sąsiada*/
	int size_of_ring = atoi(argv[1]);
	int i;
	int buf_len;
	int n = 0;			/*dla getline*/
	int line_count = 0;
	int lines_number;
	int loaded_lines = 0;
	int writed_lines = 0;
	char buf[BUF_SIZE];
	char line[BUF_SIZE];
	char input_file[] = "DATA/in";
	char output_file[] = "DATA/out";

	if(pipe(pipe_out) == -1) syserr("E: pipe_out");
	if((dup2(pipe_out[0], STDIN_FILENO)) == -1) syserr("MANAGER: Error in dup pipe_out[0]");

	for(i = 0; i < size_of_ring; i++) {
		if(pipe(pipe_in) == -1) syserr("E: pipe_in\n");
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
	if(close(pipe_out[0]) == -1) syserr("MANAGER: Error in close pipe_out[0]");
	if(close(pipe_out[1]) == -1) syserr("MANAGER: Error in close pipe_out[1]");





	/*otwarcie plików*/
	input_fd = fopen(input_file, "r");
	if(input_fd == NULL) {
		syserr("Error in open input file");
	}

	output_fd = fopen(output_file, "w");
	if(output_fd == NULL) {
		seserr("Error in open output file");
	}
	/*
	TODO wczytaj pierwsza linie i ustaw numerek jako lines_number
	*/

	do {
		/*załadowanie maksymalnej ilości wierszy do pierścienia*/
		while(loaded_lines <= size_of_ring) {
			read_line(input_fd, line);
			printf("%d:%s", line_count, line);
			fflush(stdout);
			++loaded_lines;
		}

		/*oczekiwanie na wynik*/
		buf == NULL;
		getline(buf, &n, stdin);

		if(!sign(buf[buf_len-3])) {				/*jeśli wyrażenie jest już obliczone*/
			//write_to_file(output_fd, buf);
			fprintf(stderr, "%s\n", buf);
			fflush(stderr);
			--loaded_line1s;
			++writed_lines;
		} else {								/*przekazanie dalej*/
			printf("%s", buf);
			fflush(stdout);
		}

		if(writed_lines == lines_number) {
			printf("%s", "!\n");
			fflush(stdout);
		}

	} 
	while(buf[buf_len-3] != '!');


/*	write(1, "chuj", 5);
	read(0, buf, BUF_SIZE-1);
	fprintf(stderr, "buf: %s\n", buf);*/
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