#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

int sign(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/') {
		return 1;
	} else {
		return 0;
	}
}

int main(int argc, char* argv[]) {
	size_t n = 0;		/*dla getline*/
	int pipe_out[2];
	int pipe_in[2];	
	int size_of_ring;
	int i;
	int buf_len;	
	int lines_count = 1;
	int lines_number;
	int loaded_lines = 0;
	int writed_lines = 0;
	char* buf = NULL;
	char* line;
	char input_file[40];
	char output_file[40];
	char* lines_number_s = NULL;
	char data_dir[] = "DATA/";
	FILE* input_fd;
	FILE* output_fd;

	if (argc <= 3) {
		syserr("Too few arguments");
	}

	/*wczytanie argumentow*/
	if((size_of_ring = atoi(argv[1])) <= 0) {
		syserr("Too few executors");
	}

	strcpy(input_file, data_dir);
	strcpy(output_file, data_dir);
	strcat(input_file, argv[2]);
	strcat(output_file, argv[3]);

	fprintf(stderr, "in: %s\n", input_file);
	fprintf(stderr, "out: %s\n", output_file);
	fflush(stderr);

	/*tworzenie pierscienia*/
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
				if(close(pipe_out[0]) == -1) {
					syserr("FORK: Error in close pipe_out[0]\n");
				}
				if(close(pipe_in[0]) == -1) {
					syserr("FORK: Error in close pipe_in[0]\n");
				}
				if(close(pipe_in[1]) == -1) {
					syserr("FORK: Error in close pipe_in[1]\n");
				}
				if(close(pipe_out[1]) == -1) {
					syserr("FORK: Error in close pipe_out[1]\n");
				}

				execl("./executor", "executor\n", (char *) 0);
				syserr("Error in execl(executor).\n");

			default:

				if((dup2(pipe_in[0], STDIN_FILENO)) == -1) {
					syserr("PARENT: Error in dup pipe_in[0]");
				}
				if(close(pipe_in[1]) == -1) {
					syserr("PARENT: Error in close pipe_in[1]\n");
				}
				if(close(pipe_in[0]) == -1) {
					syserr("PARENT: Error in close pipe_in[0]\n");
				}
		}
	}

	if((dup2(pipe_out[1], STDOUT_FILENO)) == -1) {
		syserr("MANAGER: error in dup pipe_out[1]");
	}
	if(close(pipe_out[0]) == -1) {
		syserr("MANAGER: Error in close pipe_out[0]");
	}
	if(close(pipe_out[1]) == -1) {
		syserr("MANAGER: Error in close pipe_out[1]");
	}

	/*otwarcie plikow*/
	input_fd = fopen(input_file, "r");
	if(input_fd == NULL) {
		syserr("Error in open input file");
	}

	output_fd = fopen(output_file, "w");
	if(output_fd == NULL) {
		syserr("Error in open output file");
	}

	line = NULL;
	getline(&lines_number_s, &n, input_fd);
	lines_number = atoi(strtok(lines_number_s,"\n"));
	free(lines_number_s);

	do {		
		/*zaladowanie maksymalnej ilosci wierszy do pierscienia*/
		while(loaded_lines < size_of_ring && lines_count <= lines_number) {
			line = NULL;
			getline(&line, &n, input_fd);
			printf("%d: %s", lines_count,line);
			fflush(stdout);
			++loaded_lines;
			++lines_count;
			free(line);
		}

		free(buf);
		buf = NULL;
	 	/*oczekiwanie na wynik*/
	 	buf_len = getline(&buf, &n, stdin);

		if(!sign(buf[buf_len-2]) && buf[0] != '!') {	/*jesli wyrazenie jest juz obliczone*/
			fprintf(output_fd, "%s", buf);
			fflush(output_fd);
			--loaded_lines;
			++writed_lines;
	 	} else {										/*przekazanie dalej*/
	 		printf("%s", buf);
	 		fflush(stdout);
	 	}

	 	/*przekazanie znaku konca pracy*/
	 	if(writed_lines == lines_number) {
	 		printf("!\n");
	 		fflush(stdout);
	 	}	
	} while(buf[0] != '!');

	free(buf);
	if(fclose(input_fd) == EOF) {
		syserr("Error in close input_fd");
	}
	if(fclose(output_fd) == EOF) {
		syserr("Error in close output_fd");
	}

	return 0;
}