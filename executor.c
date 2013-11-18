#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "err.h"


#define BUF_SIZE 1024

int sign(char c) {
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
	char a_s[15];
	char b_s[15];
	char res[BUF_SIZE];
	int a, b, c;

	parsed = strtok(line, ":");
	strcpy(new_line, parsed);
	strcat(new_line, ": ");
	parsed = strtok(NULL, " \n");
	first = parsed;
	parsed = strtok(NULL, " \n");
	second = parsed;

	parsed = strtok(NULL, " \n");
	while(parsed != NULL && strcmp(parsed, "*") != 0 && strcmp(parsed, "-") != 0 && 
			strcmp(parsed, "+") != 0 && strcmp(parsed, "/") != 0) {
		
		strcat(new_line, first);
		strcat(new_line, " ");
		first = second; 
		second = parsed;
		parsed = strtok(NULL, " \n");
	}
	strcpy(a_s, first);
	strcpy(b_s, second);
	a = atoi(a_s); 
	b = atoi(b_s);

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

	parsed = strtok(NULL, " \n");
	while(parsed != NULL) {
		strcat(new_line, " ");
		strcat(new_line, parsed);		
		parsed = strtok(NULL, " \n");
	}
	strcat(new_line, "\n\0");
}

int main(int argc, char* argv[]) {
	char* buf;
	char new_line[BUF_SIZE];
	ssize_t buf_len;
	size_t n = 0; /*dla getline*/

	while(1) {
		buf = NULL;
		buf_len = getline(&buf, &n, stdin);
		if(buf[0] == '!') {			/*przekaż koniec i skończ pracę*/
		 	printf("!\n");
		 	fflush(stdout);
		 	free(buf);
		 	break;
		}

		if(sign(buf[buf_len-2])) {	/*niepoliczone wyrazenie*/
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