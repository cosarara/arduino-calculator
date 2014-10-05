
#include "eval.h"
#include <string.h>
#include <stdio.h>

int main(int argc, const char* argv[]) {
	char s[100];
	char out[100];
	int r;

	if (argc != 2) {
		puts("Wrong number of arguments");
		return 1;
	}

	strcpy(s, (char*)argv[1]);
	r = eval(s, out);
	if (!r) puts(out);
	return r;
}

