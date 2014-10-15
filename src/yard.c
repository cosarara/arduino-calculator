#define _GNU_SOURCE

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "float.h"
#include "math_extra.h"

#define NUM 1
#define OP 2

int is_digitf(char c);

#define BINARY 2
#define UNARY 1
int is_op(char c) {
	return (!is_digitf(c) && c != '(' && c != ')');
	//if (strchr("*/+-^rE", c)) return BINARY;
	//if (strchr("cstCSTlnR!", c)) return UNARY;
	//return 0;
}

double eval_unary_d(char op, double x_i) {
	double result;
	switch (op) {
	case '-' :
		result = -x_i;
		break;
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
		puts("caca unary");
		exit(1);
	}
	return result;
}

double eval_pair_d(char op, double fst_i, double snd_i) {
	double result;
	printf("opping %f %c %f\n", fst_i, op, snd_i);
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
		puts("caca binary");
		exit(1);
	}
	printf("\t= %f\n", result);
	return result;
}

void r_polish_slim_dbg(double* stack, char* types, int stack_i) {
	int i;
	printf("debug: ");
	for (i = 0; i < stack_i; i++) {
		if (types[i] == NUM) {
			printf("%f ", stack[i]);
		} else {
			printf("%c ", *(char*)(&stack[i]));
		}
	}
	printf("\n");
}

double r_polish_slim(double* stack, char* types, int stack_i) {
	int i = 0;
	int j;
	//char* start = in;
	char op;
	//char lastc;
	double a, b, result;

	r_polish_slim_dbg(stack, types, stack_i);
	// stack_i is stack length from now on
	//stack_i += 1;
	while (stack_i > 1) { // TODO: time out
		printf("stack size: %d\n", stack_i);
		if (types[i] == NUM) {
			printf("\t%f\n", stack[i]);
			//fmtDouble(stack[i], 6, tmp, 10);
			//puts(tmp);
			i++;
		} else {
			op = *(char*)(&stack[i]);
			printf("\t%c\n", op);
			if (is_op(op) == UNARY || (op == '-' && stack_i == 2)) {
				a = stack[i-1];
				result = eval_unary_d(op, a);
				stack[i-1] = result;
				for (j=1; j < stack_i-i; j++) {
					stack[i+j-1] = stack[i+j];
				}
				for (j=1; j < stack_i-i; j++) {
					types[i+j-1] = types[i+j];
				}
				stack_i -= 1;
				i--;
			}
			if (is_op(op) == BINARY) {
				a = stack[i-2];
				b = stack[i-1];
				result = eval_pair_d(op, a, b);
				stack[i-2] = result;
				for (j=1; j < stack_i-i; j++) {
					stack[i+j-2] = stack[i+j];
				}
				for (j=1; j < stack_i-i; j++) {
					types[i+j-2] = types[i+j];
				}
				stack_i -= 2;
				i--;
				//printf("%c", *(char*)(&stack[i]));
			}
		}
	}
	return stack[0];
}

int r_polish(char* in) {
	int stack_i = 0;
	int len;
	double stack[10]; // wtf 80 bytes
	char types[10]; // Super hack!
	//char* start = in;
	char* p_ = in;
	//char lastc;
	char tmp[10];

	//puts("size:");
	//printf("%d\n", (int)sizeof(stack));

	puts(p_);
	while (*p_ != 0) {
		if (is_digitf(*p_)) {
			len = strchrnul(p_, ' ') - p_; // off by one?
			assert(len < 10);
			memcpy(tmp, p_, len);
			tmp[len+1] = '\0';

			stack[stack_i] = atof(tmp);
			printf("%f\n", stack[stack_i]);
			types[stack_i] = NUM;
			p_ += len;
			if (*p_ == '\0') break;
			p_++;
			stack_i++;
		} else {
			printf("%c\n", *p_);
			*(char*)(&stack[stack_i]) = *p_;
			types[stack_i] = OP;
			stack_i++;
			p_++;
		}
		if (*p_ == ' ') p_++;
	}
	puts("out:");
	r_polish_slim(stack, types, stack_i);
	//printf("lal %f\n", stack[0]);
	fmtDouble(stack[0], 6, tmp, 10);
	puts(tmp);
	return stack[0];
}

int get_prec(char op) {
	switch (op) {
	case '*' :
		return 3;
	case '/' :
		return 3;
	case '+' :
		return 2;
	case '-' :
		return 2;
	case '^' :
		return 4;
	case 'E' :
		return 4;
	case 'r' : // base n root
		return 5;
	default :
		puts("caca");
		exit(1);
	}
}

int is_left_as(char op) {
	return 1; // STUB
}

double yard(char* in) {
	int i;
	int len;
	char lastc = 0;
	char c = 0;
	char tmp[10];
	char* start = in;
	int stack_i = 0;
	char stack[10];
	int queue_i = 0;
	double queue[10];
	char types[10];
	char tmp_op;

	for (i=0;;i++) {
		c = in[i];
		printf("------- c=%c\ti=%d\n", c, i);
		//if (c == 0) break;
		if (is_digitf(lastc) && !is_digitf(c)) {
			// we have a number from start to &in[i-1]
			len = &in[i]-start;
			memcpy(tmp, start, len);
			tmp[len] = '\0';
			printf("len %d\ntmp:", len);
			puts(tmp);
			queue[queue_i] = atof(tmp);
			printf("%f\n", queue[queue_i]);
			types[queue_i] = NUM;
			queue_i++;
			start = &in[i];
		}
		if (is_op(lastc)) { // our ops are always 1 char long
			while (stack_i > 1) {
				tmp_op = stack[stack_i-1];
				if ((is_left_as(lastc) && get_prec(lastc) <= get_prec(tmp_op)) ||
					get_prec(lastc) < get_prec(tmp_op)) {
					*(char*)(&queue[queue_i]) = tmp_op;
					printf("%c\n", tmp_op);
					types[queue_i] = OP;
					queue_i++;
					stack_i--;
				} else break;
			}
			stack[stack_i++] = lastc;
			start = &in[i];
		}
		lastc = c;
		if (c == '\0') break;
	}

	stack_i--;
	while (stack_i != 0) {
		tmp_op = stack[stack_i];
		*(char*)(&queue[queue_i]) = tmp_op;
		printf("%c\n", tmp_op);
		types[queue_i] = OP;
		queue_i++;
		stack_i--;
	}
	puts("yay!");
	return r_polish_slim(queue, types, queue_i);
}

