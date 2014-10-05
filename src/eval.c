
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "float.h"

//char eval_str[] = "2*22*2";
//char eval_str[] = "1+10";


#define BUF_SIZE 10
#define RES_BUF_SIZE 10

void dbg(char*);

int is_digit(char c) {
	return (c >= '0' && c <= '9');
}

// Also accepts '.', for floats
int is_digitf(char c) {
	return (c == '.' || (c >= '0' && c <= '9'));
}

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

char* eval_unary(char op, char* x) {
	double result;
	double x_i = atof(x);
	char* res;
	switch (op) {
	case 'c' :
		result = cos(x_i);
		break;
	default :
		result = 12345;
		break;
	}
	res = malloc(RES_BUF_SIZE);
	if (res == NULL) {
		return NULL;
	}
	//snprintf(res, 25, "%f", result);
	fmtDouble(result, 6, res, RES_BUF_SIZE);
	cut_trailing_zeroes(res);
	return res;
}

char* eval_pair(char op, char* fst, char* snd) {
	double result;
	double fst_i = atof(fst);
	double snd_i = atof(snd);
	char* res;
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
	default :
		result = 12345;
		break;
	}
	res = malloc(RES_BUF_SIZE);
	if (res == NULL) {
		return NULL;
	}
	//snprintf(res, 25, "%f", result);
	fmtDouble(result, 6, res, RES_BUF_SIZE);
	cut_trailing_zeroes(res);
	return res;
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

// Clear a binary operator
int clear_bop(char* elements[], char op) {
	char* new_elements[BUF_SIZE];
	int new_elements_i = 0;

	char* prevelement = NULL;
	char* thiselement = NULL;
	char* nextelement = NULL;
	char* result = NULL;
	int i;

	memset(new_elements, 0, sizeof(new_elements));

	// Mentres trobem l'operador a elements[]
	while (c_in_elements(op, elements)) {
		//print_elements(elements);
		new_elements_i = 0;
		for (i=1;; i++) {
			prevelement = elements[i-1];
			thiselement = elements[i];
			nextelement = elements[i+1];

			// End of string
			if (prevelement == 0 ||
			    thiselement == 0 ||
			    nextelement == 0) break;

			if (is_digitf(prevelement[0]) && is_digitf(nextelement[0]) &&
			    thiselement[0] == op) {
				result = eval_pair(thiselement[0], prevelement, nextelement);
				if (result == NULL) {
					return 1;
				}
				//dbg(result);
				new_elements[new_elements_i++] = result;
				free(prevelement);
				free(thiselement);
				free(nextelement);
				prevelement = thiselement = nextelement = NULL;
				// Copy everything after this and break
				// new_elements[new_elements_i:BUF_SIZE-i-2] = elements[i+2:BUF_SIZE-i-2]
				i += 2;
				memcpy(&new_elements[new_elements_i], &elements[i],
						(BUF_SIZE-i)*sizeof(char*));
				break;
			} else {
				new_elements[new_elements_i++] = prevelement;
			}
		}
		memcpy(elements, new_elements, BUF_SIZE*sizeof(char*));
		memset(new_elements, 0, BUF_SIZE*sizeof(char*));
	}
	return 0;
}

// Clear an unary operator
int clear_uop(char* elements[], char op) {
	char* new_elements[BUF_SIZE];
	int new_elements_i = 0;

	char* thiselement = NULL;
	char* nextelement = NULL;
	char* result = NULL;
	int i;
	memset(new_elements, 0, sizeof(new_elements));

	while (c_in_elements(op, elements)) {
		new_elements_i = 0;
		for (i=0;; i++) {
			thiselement = elements[i];
			nextelement = elements[i+1];

			// End of string
			if (thiselement == 0 ||
			    nextelement == 0) break;
				puts("ok");

			if (is_digitf(nextelement[0]) && thiselement[0] == op) {
				result = eval_unary(thiselement[0], nextelement);
				if (result == NULL) {
					return 1;
				}
				//dbg(result);
				new_elements[new_elements_i++] = result;
				free(thiselement);
				free(nextelement);
				i += 2;
				// new_elements[new_elements_i:BUF_SIZE-i-2] = elements[i+2:BUF_SIZE-i-2]
				memcpy(&new_elements[new_elements_i], &elements[i], (BUF_SIZE-i)*sizeof(char*));
				break;
			} else {
				new_elements[new_elements_i++] = thiselement;
			}
		}
		memcpy(elements, new_elements, BUF_SIZE*sizeof(char*));
		memset(new_elements, 0, BUF_SIZE*sizeof(char*));
	}
	return 0;
}

int eval_subexpr(char* eval_str, char* out) {
	int failed = 0;
	int i;
	char lastc = 0;
	char c = 0;

	char* elements[BUF_SIZE];
	int elements_i = 0;

	char element[BUF_SIZE];
	int element_i = 0;
	char* prevelement = NULL;

	memset(elements, 0, sizeof(elements));

	if (eval_str[0] == '\0') {
		out[0] = '\0';
		return 0;
	}

	// Separar els elements de l'expressió
	for (i=0;;i++) {
		c = eval_str[i];
		// New thing, push previous
		if ((!is_digitf(lastc) && lastc != 0) || (is_digitf(lastc) && !is_digitf(c))) {
			if (element[0] != 0) {
				//puts("next");
				// Guardem aquest element a elements[] i anem pel següent
				prevelement = strdup(element);
				element[0] = 0;
				element_i = 0;
				// XXX: test if null
				elements[elements_i] = prevelement;
				elements_i++;
			}
		}
		element[element_i++] = c;
		element[element_i] = 0;

		lastc = c;
		if (c == '\0') break;
	}
	//i = 0;
	//while (elements[i] != 0) {
	//	puts(elements[i++]);
	//}

	if (clear_uop(elements, 'c') ||
	    clear_bop(elements, '*') ||
	    clear_bop(elements, '/') ||
	    clear_bop(elements, '+') ||
	    clear_bop(elements, '-') ||
	    elements[0] == 0) {
		puts("ERROR");
		failed = 1;
	} else {
		puts("END");
		puts(elements[0]);
		strcpy(out, elements[0]);
	}
	free_elements(elements);
	return failed;
}

//#define IN_ARDUINO
int clear_parens(char* expr, char* out) {
	uintptr_t start;
	uintptr_t len;
	char sub_expr[BUF_SIZE];
#ifdef IN_ARDUINO
	dbg("hello0");
#endif
	while (strchr(expr, '(')) {
		start = (uintptr_t)strrchr(expr, '(') - (uintptr_t)expr + 1;
		len = (uintptr_t)(strchr(&expr[start], ')') - (uintptr_t)expr) - start;
		memcpy(sub_expr, &expr[start], len);
		sub_expr[len] = 0;
		//puts(sub_expr);
		if (eval_subexpr(sub_expr, sub_expr)) {
			return 1;
		}
		//puts("subexpr:");
		//puts(sub_expr);
		strcpy(&expr[start-1], sub_expr);
		strcpy(&expr[start+strlen(sub_expr)-1], &expr[start+len+1]);
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

int eval(char* expr, char* out) {
	return clear_parens(expr, out);
}



