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
	char res[BUF_SIZE];
	int a, b, c;

	parsed = strtok(line, ":");
	strcpy(new_line, parsed);
	strcat(new_line, ":");
	//fprintf(stderr, "%s\n", new_line);
	//fflush(stderr);
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

	a = atoi(first); 
	b = atoi(second);

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
	strcat(new_line, "\n");
}

int main(int argc, char* argv[]) {
	char* buf = NULL;
	char new_line[BUF_SIZE];
	char* test;
	ssize_t buf_len;
	pid_t pid = getpid();
	size_t n = 0; /*dla getline*/

	while(1) {
		buf = NULL;
		buf_len = getline(&buf, &n, stdin);
		//fprintf(stderr, "buf:%s\n", buf);
		//fflush(stderr);
		if(buf[0] == '!') {			/*przekaż impuls i skończ pracę*/
		 	printf("!\n");
		 	fflush(stdout);
		 	break;
		}

		if(sign(buf[buf_len-2])) {	/*niepoliczone wyrazenie*/
			parse(buf, new_line);
			//fprintf(stderr, "new_line %d: %s\n", pid, new_line);
			//fflush(stderr);
			printf("%s", new_line);
			fflush(stdout);
		} else {					/*przekaz bez zmian*/
			printf("%s", buf);
			fflush(stdout);
		}
		
	}

	// return 0;
}