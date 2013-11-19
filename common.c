#include "common.h"

/*sprawdza czy znak jest operatorem matematycznym*/
int math_sign(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/') {
		return 1;
	} else {
		return 0;
	}
}
