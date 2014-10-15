
#include "eval.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "float.h"

int r_polish(char* in);
double yard(char* in);

int main(int argc, const char* argv[]) {
	char s[100];
	char out[100];
	int r;
	double result;

	if (argc != 2) {
		puts("Wrong number of arguments");
		return 1;
	}

	result = yard((char*)argv[1]);
	printf("%f\n", result);
	fmtDouble(result, 6, out, 10);
	puts(out);
	//puts("&&&&");
	//r_polish((char*)argv[1]);
	exit(0);

	strcpy(s, (char*)argv[1]);
	r = eval(s, out);
	if (!r) puts(out);
	return r;
}

