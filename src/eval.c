
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "float.h"
#include "math_extra.h"

//char eval_str[] = "2*22*2";
//char eval_str[] = "1+10";


#define PTRS_SIZE 16
#define STR_SIZE 16
#define RES_BUF_SIZE 16

void dbg(char*);

int is_digit(char c) {
	return (c >= '0' && c <= '9');
}

// Also accepts '.', for floats
int is_digitf(char c) {
	return (c == '.' || (c >= '0' && c <= '9'));
}

void cut_trailing_zeroes(char* s) {
	char* c = s + strlen(s) - 1;
	while (c != s && *c == '0') {
		*c = '\0';
		c--;
	}
	if (*c == '.') {
		*c = '\0';
	}
}

#if 0
int count_elements(char* elements[]) {
	int i;
	for (i=0; elements[i]!=0; i++);
	return i;
}

void free_elements(char* elements[]) {
	int i = 0;
	while (elements[i] != 0) {
		free(elements[i++]);
	}
}


int eval_unary(char op, char* x, char* out) {
	double result;
	double x_i = atof(x);
	switch (op) {
	case 'c' :
		result = cos(x_i);
		break;
	case 's' :
		result = sin(x_i);
		break;
	case 't' :
		result = tan(x_i);
		break;
	case 'C' :
		result = acos(x_i);
		break;
	case 'S' :
		result = asin(x_i);
		break;
	case 'T' :
		result = atan(x_i);
		break;
	case 'l' :
		result = log10(x_i);
		break;
	case 'n' :
		result = log(x_i);
		break;
	case 'R' :
		result = sqrt(x_i);
		break;
	case '!' :
		result = fact(x_i);
		break;
	default :
		return 1;
	}
	//snprintf(res, 25, "%f", result);
	fmtDouble(result, 6, out, RES_BUF_SIZE);
	cut_trailing_zeroes(out);
	return 0;
}

int eval_pair(char op, char* fst, char* snd, char* out) {
	double result;
	double fst_i = atof(fst);
	double snd_i = atof(snd);
	switch (op) {
	case '*' :
		result = fst_i * snd_i;
		break;
	case '/' :
		result = fst_i / snd_i;
		break;
	case '+' :
		result = fst_i + snd_i;
		break;
	case '-' :
		result = fst_i - snd_i;
		break;
	case '^' :
		result = pow(fst_i, snd_i);
		break;
	case 'r' : // base n root
		result = pow(snd_i, 1/fst_i);
		break;
	case 'E' :
		result = fst_i * pow(10, snd_i);
		break;
	default :
		return 1;
	}
	//snprintf(res, 25, "%f", result);
	fmtDouble(result, 6, out, RES_BUF_SIZE);
	cut_trailing_zeroes(out);
	return 0;
}

int c_in_elements(char c, char* elements[]) {
	int i;
	char* element;
	for (i=0;;i++) {
		element = elements[i];
		if (element == NULL) break;
		if (element[0] == c) return 1;
	}
	return 0;
}

void print_elements(char* elements[]) {
	char* s;
	int i = 0;
	s = elements[i];
	while (s != 0) {
		puts(s);
		i++;
		s = elements[i];
	}
	puts("--\n");
}

// This functions are not written in a recursive form to avoid
// memory problems.

char* pre_eval_pair(char* elements[], int i, char* op, char* left, char* right, char* out) {
	char etmp1[STR_SIZE];
	char etmp2[STR_SIZE];
	char* end = NULL;
	//printf("clear_bop: %d\n", (int)(sizeof(etmp1)+sizeof(etmp2)+sizeof(end)));

	memset(etmp1, 0, sizeof(etmp1));
	memset(etmp2, 0, sizeof(etmp2));

	memcpy(etmp1, left, op-left);

	if (elements[i+3] == NULL) {
		strcpy(etmp2, right);
		end = strchr(right, '\0');
	} else {
		end = elements[i+3];
		/*
		printf("right: %s\n", right);
		printf("end: %s\n", end);
		printf("right-end: %d\n", right-end);
		*/
		memcpy(etmp2, right, end-right);
	}
	if (eval_pair(*op, etmp1, etmp2, out)) return NULL;

	return end;
}

void get_elements(char* elements[], char* eval_str) {
	int i;
	char lastc = 0;
	char c = 0;
	int elements_i = 0;

	char* start = eval_str;
	//printf("get_elements: %d\n", (int)(sizeof(i)+sizeof(lastc)+sizeof(c)+sizeof(elements_i)+sizeof(start)));

	memset(elements, 0, PTRS_SIZE*sizeof(char*));

	for (i=0;;i++) {
		c = eval_str[i];
		// New thing, push previous
		if ((!is_digitf(lastc) && lastc != 0) || (is_digitf(lastc) && !is_digitf(c))) {
			elements[elements_i] = start;
			elements_i++;
			start = eval_str + i;
		}
		lastc = c;
		if (c == '\0') break;
	}
}

// Clear a binary operator
int clear_bop(char* elements[], char op) {
	char tmp[STR_SIZE];

	char* left = NULL;
	char* l_op = NULL; // local op
	char* right = NULL;
	char* end = NULL;
	int i;
	//printf("clear_bop: %d\n", (int)(sizeof(tmp)+sizeof(left)+sizeof(l_op)+sizeof(right)+sizeof(end)+sizeof(i)));

	memset(tmp, 0, sizeof(tmp));

	// Mentres trobem l'operador a elements[]
	while (c_in_elements(op, elements)) {
		//print_elements(elements);
		//new_elements_i = 0;
		for (i=0;; i++) {
			left = elements[i];
			l_op = elements[i+1];
			right = elements[i+2];

			if (left == 0 ||
			    l_op == 0 ||
			    right == 0) break;

			if (is_digitf(left[0]) && is_digitf(right[0]) &&
			    l_op[0] == op) {
				memset(tmp, 0, sizeof(tmp));
				end = pre_eval_pair(elements, i, l_op, left, right, tmp);
				if (end == NULL) {
					return 1; // ERROR
				}
				// Put the rest of elements in tmp, then everything back
				strcpy(strchr(tmp, 0), end);
				strcpy(left, tmp);
				get_elements(elements, elements[0]);
				break;
			}
		}
		//memcpy(elements, new_elements, PTRS_SIZE*sizeof(char*));
		//memset(new_elements, 0, PTRS_SIZE*sizeof(char*));
	}
	return 0;
}

char* pre_eval_unary(char* elements[], int i, char* op, char* arg, char* out) {
	char etmp[STR_SIZE];
	char* end = NULL;
	//printf("clear_bop: %d\n", (int)(sizeof(etmp1)+sizeof(etmp2)+sizeof(end)));

	memset(etmp, 0, sizeof(etmp));

	if (elements[i+2] == NULL) {
		strcpy(etmp, arg);
		end = strchr(arg, '\0');
	} else {
		end = elements[i+2];
		memcpy(etmp, arg, end-arg);
	}
	if (eval_unary(*op, etmp, out)) return NULL;

	return end;
}

// Clear an unary operator
int clear_uop(char* elements[], char op) {
	char tmp[STR_SIZE];

	char* thiselement = NULL;
	char* nextelement = NULL;
	char* end = NULL;
	int i;
	memset(tmp, 0, sizeof(tmp));

	while (c_in_elements(op, elements)) {
		for (i=0;; i++) {
			thiselement = elements[i];
			nextelement = elements[i+1];

			// End of string
			if (thiselement == 0 ||
			    nextelement == 0) break;

			if (is_digitf(nextelement[0]) && thiselement[0] == op) {
				memset(tmp, 0, sizeof(tmp));
				end = pre_eval_unary(elements, i, thiselement, nextelement, tmp);
				if (end == NULL) {
					return 1;
				}
				//dbg(result);
				strcpy(strchr(tmp, 0), end);
				strcpy(thiselement, tmp);
				get_elements(elements, elements[0]);
				break;
			}
		}
	}
	return 0;
}

int eval_subexpr(char* eval_str, char* out) {
	int failed = 0;

	char* elements[PTRS_SIZE];
	//printf("eval_subexpr: %d\n", (int)(sizeof(failed)+sizeof(elements)));

	if (eval_str[0] == '\0') {
		out[0] = '\0';
		return 0;
	}

	// Separar els elements de l'expressió
	get_elements(elements, eval_str);

	if (clear_uop(elements, 'c') ||
	    clear_uop(elements, 's') ||
	    clear_uop(elements, 't') ||
	    clear_uop(elements, 'C') ||
	    clear_uop(elements, 'S') ||
	    clear_uop(elements, 'T') ||
	    clear_uop(elements, 'l') ||
	    clear_uop(elements, 'n') ||
	    clear_uop(elements, 'R') ||
	    clear_uop(elements, '!') ||

	    clear_bop(elements, 'E') ||
	    clear_bop(elements, 'r') ||
	    clear_bop(elements, '^') ||

	    clear_bop(elements, '*') ||
	    clear_bop(elements, '/') ||
	    clear_bop(elements, '+') ||
	    clear_bop(elements, '-') ||
	    elements[0] == 0) {
		puts("ERROR");
		failed = 1;
	} else {
		//puts("END");
		//puts(elements[0]);
		strcpy(out, elements[0]);
	}
	//free_elements(elements);
	return failed;
}

//#define IN_ARDUINO
int clear_parens(char* expr, char* out) {
	uintptr_t start;
	uintptr_t len;
	char sub_expr[STR_SIZE];
	char sub_expr_tmp[STR_SIZE];
	//printf("clear_parens: %d\n", (int)(sizeof(start)+sizeof(len)+sizeof(sub_expr)+sizeof(sub_expr_tmp)));
#ifdef IN_ARDUINO
	dbg("hello0");
#endif
	while (strchr(expr, '(')) {
		start = (uintptr_t)strrchr(expr, '(') - (uintptr_t)expr + 1;
		len = (uintptr_t)(strchr(&expr[start], ')') - (uintptr_t)expr) - start;
		memcpy(sub_expr, &expr[start], len);
		sub_expr[len] = 0;
		//puts(sub_expr);
		if (eval_subexpr(sub_expr, sub_expr_tmp)) {
			return 1;
		}
		//memcpy(sub_expr, sub_expr_tmp, sizeof(sub_expr_tmp));
		//puts("subexpr:");
		//puts(sub_expr);
		// now sub_expr will be used as another tmp var, for the sake of memory usage
		strcpy(sub_expr, &expr[start+len+1]); // holds the tail
		strcpy(&expr[start-1], sub_expr_tmp); // put the middle
		strcpy(&expr[start+strlen(sub_expr_tmp)-1], sub_expr); // put the tail
		//puts(expr);
	}
#ifdef IN_ARDUINO
	dbg("hello1");
#endif
	if(eval_subexpr(expr, sub_expr)) {
		return 1;
	}
#ifdef IN_ARDUINO
	dbg("hello2");
#endif
	strcpy(out, sub_expr);
	return 0;
}
#endif

int eval(char* expr, char* out) {
	return 0;
	//return clear_parens(expr, out);
}



