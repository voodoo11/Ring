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

	parsed = strtok(line, " \n");
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
	char buf[BUF_SIZE];
	char new_line[BUF_SIZE];
	char* test;
	ssize_t buf_len;
	pid_t pid = getpid();
	int n = 0; /*dla getline*/

	while(1) {
		getline(buf, &n, stdin);

		if(buf[0] == '!') {			/*przekaż impuls i skończ pracę*/
			printf("%c", '!\n');
			fflush(stdout);
			break;
		}

		if(sign(buf[buf_len-3])) {	/*niepoliczone wyrazenie*/
			parse(buf, new_line);
			printf("%s", new_line);
			fflush(stdout);
		} else {					/*przekaz bez zmian*/
			printf("%s", buf);
			fflush(stdout);
		}
	}
	// while(1) {
	// 	new_line[0] = '\0';
	// 	if((buf_len = read(0, buf, BUF_SIZE-1)) == -1) {
	// 		syserr("read from 0");
	// 	}
	// 	if(buf[buf_len-1] == '!') {
	// 		break;
	// 	}
	// 	//fprintf(stderr, "pid: %d ----> buf_len %d, ostatnim znakiem jest %c\n", pid, buf_len, buf[buf_len-3] );
	// 	if(buf[buf_len-3] == '+' || buf[buf_len-3] == '-' || buf[buf_len-3] == '*' || buf[buf_len-3] == '/') {
	// 		parse(buf, new_line);
	// 		if((write(1, new_line, strlen(new_line)+1)) == -1) {
	// 			syserr("read from 0");
	// 		}
	// 	} else {
	// 		if((write(1, buf, strlen(buf)+1)) == -1) {
	// 			syserr("read from 0");
	// 		}
	// 	}
	// }
	
	// if((write(1, "!", 1)) == -1) {
	// 	syserr("Error in killing babies");
	// } else {
	// 	fprintf(stderr, "Zabijam sie\n");
	// }
	// return 0;
}