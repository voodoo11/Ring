/*******************************************
*	Jakub Kowalski
*	nr indeksu: 334674
*	mail: jk334674@students.mimuw.edu.pl
********************************************/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "err.h"
#include "common.h"

#define FILE_NAME_LEN 100

int main(int argc, char* argv[]) {
	size_t n = 0;		/*dla getline*/
	int pipe_out[2];
	int pipe_in[2];	
	int size_of_ring;
	int i;
	int buf_len;
	int lines_count = 1;	/*linie odczytane z pliku*/
	int lines_number;	/*ilosc linii w pliku*/
	int loaded_lines = 0;	/*ilosc linii w pierscieniu*/
	int writed_lines = 0;	/*linie zapisane do pliku*/
	char* buf = NULL;	/*bufor do odbioru danych z pierscienia*/
	char* line = NULL;	/*bufor do odczytu z pliku*/
	char data_dir[] = "DATA/";
	char input_file[FILE_NAME_LEN];
	char output_file[FILE_NAME_LEN];
	FILE* input_fp;
	FILE* output_fp;

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

	/*zduplikowanie pipe'a na STDOUT,
	będzie nadpisywany w forkach przez odpowiedzi*/
	if((dup2(pipe_out[1], STDOUT_FILENO)) == -1) {
		syserr("MANAGER: Error in dup pipe_out[1]\n");
	}
	/*zduplikowanie pipe'a na STDIN, ktory bedzie dziedziczony przez
	pierwszego egzekutora i podmieniany za kazdym obrotem petli przez
	rodzica*/
	if((dup2(pipe_out[0], STDIN_FILENO)) == -1) {
		syserr("MANAGER: Error in dup pipe_out[0]\n");
	}

	/*zamkniecie niepotrzebynch pipe'ow*/
	if(close(pipe_out[0]) == -1) {
		syserr("MANAGER: Error in close pipe_out[0]\n");
	}
	if(close(pipe_out[1]) == -1) {
		syserr("MANAGER: Error in close pipe_out[1]\n");
	}

	/*tworzenie pierscienia*/
	for(i = 0; i < size_of_ring; ++i) {
		if(pipe(pipe_in) == -1) syserr("Error in creating pipe_in\n");
		switch(fork()) {
			case -1:
				syserr("Error in fork\n");

			case 0:
				/*zduplikowanie wyjscia do nastepnego 
				egzekutora*/
				if((dup2(pipe_in[1], STDOUT_FILENO)) == -1 ) {
					syserr("C:Error in dup pipe_in[1]\n");
				}
				if(close(pipe_in[0]) == -1) {
					syserr("C:Error in close pipe_in[0]\n");
				}
				if(close(pipe_in[1]) == -1) {
					syserr("C:Error in close pipe_in[1]\n");
				}

				execl("./executor", "executor", (char *) 0);
				syserr("Error in execl(executor).\n");

			default:
				/*zduplikowanie wejscia, dziedziczone przy 
				nastepnym obrocie petli przez dziecko*/
				if((dup2(pipe_in[0], STDIN_FILENO)) == -1) {
					syserr("P:Error in dup pipe_in[0]\n");
				}
				if(close(pipe_in[1]) == -1) {
					syserr("P:Error in close pipe_in[1]\n");
				}
				if(close(pipe_in[0]) == -1) {
					syserr("P:Error in close pipe_in[0]\n");
				}
		}
	}

	/*otwarcie plikow*/
	input_fp = fopen(input_file, "r");
	if(input_fp == NULL) {
		syserr("Error in open input file\n");
	}

	output_fp = fopen(output_file, "w");
	if(output_fp == NULL) {
		syserr("Error in open output file\n");
	}

	/*wczytanie pierwszej linii z pliku*/
	getline(&line, &n, input_fp);
	lines_number = atoi(strtok(line,"\n"));
	free(line);

	/*odczytywanie, przesylanie i zapisywanie danych*/
	do {
		/*zaladowanie maksymalnej ilosci wierszy do pierscienia*/
		while(loaded_lines < size_of_ring && 
				lines_count <= lines_number) {
			line = NULL;
			getline(&line, &n, input_fp);
			printf("%d: %s", lines_count,line);
			fflush(stdout);
			++loaded_lines;
			++lines_count;
			free(line);
		}

		buf = NULL;
	 	/*oczekiwanie na wynik*/
	 	if((buf_len = getline(&buf, &n, stdin)) < 1) {
	 		syserr("MANAGER: Blad oczytu z STDIN");
	 	}

	 	/*jesli wyrazenie jest juz obliczone*/
		if(!math_sign(buf[buf_len-2])) {
			fprintf(output_fp, "%s", buf);
			fflush(output_fp);
			--loaded_lines;
			++writed_lines;

	 	} else {	/*przekazanie niepoliczonego dalej*/
	 		printf("%s", buf);
	 		fflush(stdout);
	 	}
	 	free(buf);
	} while(writed_lines != lines_number);

	/*przekazanie znaku konca pracy*/
 	if(writed_lines == lines_number) {
 		printf(KILL_SIGN);
 		fflush(stdout);
 	}

 	/*zamkniecie plikow*/
	if(fclose(input_fp) == EOF) {
		syserr("Error in close input_fp\n");
	}
	if(fclose(output_fp) == EOF) {
		syserr("Error in close output_fp\n");
	}

	/*czekanie na dzieci*/
	for(i=0; i<size_of_ring; ++i) {
		if(wait(0) == -1) {
			syserr("Error in wait\n");
		}
	}

	return 0;
}