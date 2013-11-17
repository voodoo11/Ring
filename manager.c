#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

#define BUF_SIZE 1024

void read_line(FILE* input_fd, char** line) {
	size_t len = 0;
	free(*line);

	getline(line, &len, input_fd);
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
	int size_of_ring;
	int i;
	int buf_len;
	size_t n = 0;			/*dla getline*/
	int line_count = 1;
	int lines_number;
	char asd[] = {'a', 'b'};
	int loaded_lines = 0;
	int writed_lines = 0;
	char* buf = NULL;
	char* line = NULL;
	char input_file[] = "DATA/in";
	char output_file[] = "DATA/out";

	if (argc < 3) {
		syserr("Too few arguments");
	}

	size_of_ring = atoi(argv[1]);

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
	FILE* input_fd = fopen(input_file, "r");
	if(input_fd == NULL) {
		syserr("Error in open input file");
	}

	FILE* output_fd = fopen(output_file, "w");
	if(output_fd == NULL) {
		syserr("Error in open output file");
	}

	read_line(input_fd, &line);
	lines_number = atoi(line);

	do {
		/*załadowanie maksymalnej ilości wierszy do pierścienia*/
		while(loaded_lines < size_of_ring && line_count <= lines_number) {
			read_line(input_fd, &line);
			printf("%d:%s", line_count,line);
			fflush(stdout);
			++loaded_lines;
			++line_count;
		}

	 	/*oczekiwanie na wynik*/
	 	buf_len = getline(&buf, &n, stdin);

		if(!sign(buf[buf_len-2]) && buf[0] != '!') {	/*jeśli wyrażenie jest już obliczone*/
			fprintf(output_fd, "%s", buf);
			fflush(output_fd);
			--loaded_lines;
			++writed_lines;
	 	} else {										/*przekazanie dalej*/
	 		printf("%s", buf);
	 		fflush(stdout);
	 	}

	 	if(writed_lines == lines_number) {
	 		printf("!\n");
	 		fflush(stdout);
	 	}

	} 
	while(buf[0] != '!');

	free(buf);

	fclose(input_fd);
	fclose(output_fd);
	return 0;

}	/*main*/