#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

#define KILL_SIGN "!\n"
#define FILE_NAME_LEN 100

/*sprawdza czy znak jest operatorem matematycznym*/
int math_sign(char c) {
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
	char* line = NULL;
	char data_dir[] = "DATA/";
	char input_file[FILE_NAME_LEN];
	char output_file[FILE_NAME_LEN];
	FILE* input_fd;
	FILE* output_fd;

	/*sprawdzenie ilosci argumentow*/
	if (argc <= 3) {
		syserr("Too few arguments\n");
	}

	/*wczytanie argumentow*/
	if((size_of_ring = atoi(argv[1])) <= 0) {
		syserr("Too few executors\n");
	}

	/*zlozenie nazw plikow*/
	strcpy(input_file, data_dir);
	strcpy(output_file, data_dir);
	strcat(input_file, argv[2]);
	strcat(output_file, argv[3]);

	/*stworzenie pierwszego pipe'a*/
	if(pipe(pipe_out) == -1) {
		syserr("Erro in creating pipe_out\n");
	}
	/*zduplikowanie pipe'a na STDIN, ktory bedzie dziedziczony przez
	pierwszego egzekutora i podmieniany za kazdym obrotem petli przez
	rodzica*/
	if((dup2(pipe_out[0], STDIN_FILENO)) == -1){
		syserr("MANAGER: Error in dup pipe_out[0]\n");
	}

	/*tworzenie pierscienia*/
	for(i = 0; i < size_of_ring; i++) {
		if(pipe(pipe_in) == -1) syserr("Error in creating pipe_in\n");
		switch(fork()) {
			case -1:
				syserr("Error in fork\n");

			case 0:
				/*zduplikowanie wyjscia do nastepnego egzekutora*/
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
				/*zduplikowanie wejscia, dziedziczone przy nastepnym
				obrocie petli*/
				if((dup2(pipe_in[0], STDIN_FILENO)) == -1) {
					syserr("PARENT: Error in dup pipe_in[0]\n");
				}
				if(close(pipe_in[1]) == -1) {
					syserr("PARENT: Error in close pipe_in[1]\n");
				}
				if(close(pipe_in[0]) == -1) {
					syserr("PARENT: Error in close pipe_in[0]\n");
				}
		}
	}

	/*zduplikowanie deskryptora wyjscia do pierwszego egzekutora*/
	if((dup2(pipe_out[1], STDOUT_FILENO)) == -1) {
		syserr("MANAGER: error in dup pipe_out[1]\n");
	}

	/*zamkniecie niepotrzebnych deskryptorow*/
	if(close(pipe_out[0]) == -1) {
		syserr("MANAGER: Error in close pipe_out[0]\n");
	}
	if(close(pipe_out[1]) == -1) {
		syserr("MANAGER: Error in close pipe_out[1]\n");
	}

	/*otwarcie plikow*/
	input_fd = fopen(input_file, "r");
	if(input_fd == NULL) {
		syserr("Error in open input file\n");
	}

	output_fd = fopen(output_file, "w");
	if(output_fd == NULL) {
		syserr("Error in open output file\n");
	}

	/*wczytanie pierwszej linii z pliku*/
	getline(&line, &n, input_fd);
	lines_number = atoi(strtok(line,"\n"));
	free(line);

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

		if(!math_sign(buf[buf_len-2]) && buf[0] != '!') {	/*jesli wyrazenie jest juz obliczone*/
			fprintf(output_fd, "%s", buf);
			fflush(output_fd);
			--loaded_lines;
			++writed_lines;

	 	} else {	/*przekazanie dalej*/
	 		printf("%s", buf);
	 		fflush(stdout);
	 	}	
	} while(writed_lines != lines_number);

	free(buf);

	/*przekazanie znaku konca pracy*/
 	if(writed_lines == lines_number) {
 		printf(KILL_SIGN);
 		fflush(stdout);
 	}

 	/*zamkniecie plikow*/
	if(fclose(input_fd) == EOF) {
		syserr("Error in close input_fd\n");
	}
	if(fclose(output_fd) == EOF) {
		syserr("Error in close output_fd\n");
	}

	/*czekanie na dzieci*/
	for(i=0; i<size_of_ring; i++) {
		if(wait(0) == -1) {
			syserr("Error in wait\n");
		}
	}

	return 0;
}