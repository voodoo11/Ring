/*******************************************
*	Jakub Kowalski
*	nr indeksu: 334674
*	mail: jk334674@students.mimuw.edu.pl
********************************************/

#include "common.h"

/*sprawdza czy znak jest operatorem matematycznym*/
int math_sign(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/') {
		return 1;
	} else {
		return 0;
	}
}
