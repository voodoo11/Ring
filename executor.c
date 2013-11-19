#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"

#define NUM_LENGTH 15
#define KILL_SIGN "!\n"

int math_sign(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/') {
		return 1;
	} else {
		return 0;
	}
}

void parse(char* line, char new_line[]) {
	char* parsed;
	char* first;
	char* second;
	char res[NUM_LENGTH];
	int a, b, c;

	/*ominiecie numeru linii*/
	parsed = strtok(line, ":");
	strcpy(new_line, parsed);
	strcat(new_line, ": ");

	/*zapamietanie pierwszych dwoch znakow*/
	parsed = strtok(NULL, " \n");
	first = parsed;
	parsed = strtok(NULL, " \n");
	second = parsed;

	/*szukanie pierwszego operatora i liczb do policzenia*/
	parsed = strtok(NULL, " \n");
	while(parsed != NULL && strcmp(parsed, "*") != 0 && strcmp(parsed, "-") != 0 && 
			strcmp(parsed, "+") != 0 && strcmp(parsed, "/") != 0) {

		strcat(new_line, first);
		strcat(new_line, " ");
		first = second; 
		second = parsed;
		parsed = strtok(NULL, " \n");
	}

	a = atoi(first); 
	b = atoi(second);

	/*wykonanie dzialania*/
	if(strcmp(parsed, "+") == 0) {
		c = a + b;
	} else if(strcmp(parsed, "-") == 0) {
		c = a - b;
	} else if(strcmp(parsed, "*") == 0) {
		c = a * b;
	} else if(strcmp(parsed, "/") == 0) {
		c = a / b;
	}

	sprintf(res, "%d", c);
	strcat(new_line, res);

	/*dopisanie reszty linii do nowego wyrazenia*/
	parsed = strtok(NULL, " \n");
	while(parsed != NULL) {
		strcat(new_line, " ");
		strcat(new_line, parsed);
		parsed = strtok(NULL, " \n");
	}
	strcat(new_line, "\n");
}

int main(int argc, char* argv[]) {
	char* buf;
	ssize_t buf_len;
	size_t n = 0; /*dla getline*/

	while(1) {
		buf = NULL;
		buf_len = getline(&buf, &n, stdin);
		if(buf[0] == '!') {			/*przekaz koniec i skończ prace*/
		 	printf(KILL_SIGN);
		 	fflush(stdout);
		 	free(buf);
		 	break;
		}

		if(math_sign(buf[buf_len-2])) {	/*niepoliczone wyrazenie*/
			char new_line[buf_len];
			parse(buf, new_line);
			printf("%s", new_line);
			fflush(stdout);
		} else {					/*przekaz bez zmian*/
			printf("%s", buf);
			fflush(stdout);
		}
		free(buf);
	}

	return 0;
}