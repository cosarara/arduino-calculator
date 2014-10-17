#define _GNU_SOURCE

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "float.h"
#include "math_extra.h"

#define DBUF_SIZE 10
#define STACK_SIZE 10
#define TMP_SIZE 10

#define NUM 1
#define OP 2

int is_digitf(char c);

#define BINARY 2
#define UNARY 1
int is_op(char c) {
	//return (!is_digitf(c) && c != '(' && c != ')');
	if (c == '\0') return 0;
	if (strchr("*/+-^rE", c)) return BINARY;
	if (strchr("#cstCSTlnR!", c)) return UNARY;
	return 0;
}

double eval_unary_d(char op, double x_i) {
	double result;
	//printf("opping %c %f\n", op, x_i);
	switch (op) {
	case '-' : case '#' :
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
		//puts("caca unary");
		exit(1);
	}
	//printf("\t= %f\n", result);
	return result;
}

double eval_pair_d(char op, double fst_i, double snd_i) {
	double result;
	//printf("opping %f %c %f\n", fst_i, op, snd_i);
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
		//puts("caca binary");
		exit(1);
	}
	//printf("\t= %f\n", result);
	return result;
}

void r_polish_slim_dbg(double* stack, char* types, int stack_i) {
	int i;
	//printf("debug: ");
	for (i = 0; i < stack_i; i++) {
		if (types[i] == NUM) {
			//printf("%f ", stack[i]);
		} else {
			//printf("%c ", *(char*)(&stack[i]));
		}
	}
	//printf("\n");
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
		//puts("----------------------------");
		//printf("stack size: %d\n", stack_i);
		//printf("i: %d\n", i);
		r_polish_slim_dbg(stack, types, stack_i);
		if (types[i] == NUM) {
			//printf("\t%f\n", stack[i]);
			//fmtDouble(stack[i], 6, tmp, 10);
			//puts(tmp);
			i++;
		} else {
			op = *(char*)(&stack[i]);
			//printf("\t%c\n", op);
			if (is_op(op) == UNARY || (op == '-' && i == 1)) {
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
				//i--;
			} else if (is_op(op) == BINARY) {
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
#if 0	
	int stack_i = 0;
	int len;
	double stack[DBUF_SIZE]; // wtf 80 bytes
	char types[DBUF_SIZE]; // Super hack!
	//char* start = in;
	char* p_ = in;
	//char lastc;
	char tmp[TMP_SIZE];

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
#endif
	return 0;
}

int get_prec(char op) {
	if (is_op(op) == UNARY) return 5;

	switch (op) {
	case '*' :
		return 3;
	case '/' :
		return 3;
	case '+' :
		return 2;
	case '-' :
		return 2;
	case '#' : // unary -
		return 2;
	case '^' :
		return 4;
	case 'E' :
		return 4;
	case 'r' : // base n root
		return 5;
	default :
		assert(0 && "caca");
		//exit(1);
	}
}

int is_left_as(char op) {
	return 1; // STUB
}

double yard(char* in) {
	char i;
	int len;
	char lastc = 0;
	char c = 0;
	char tmp[TMP_SIZE];
	char* start = in;
	int stack_i = 0;
	char stack[STACK_SIZE];
	int queue_i = 0;
	double queue[DBUF_SIZE];
	char types[DBUF_SIZE];
	char tmp_op;

	lastc = in[0];
	if (lastc == 0) return 0.0;
	for (i=1;;i++) {
			dbg("ll");
			delay(200);
		c = in[i];
			dbg("lall");
			delay(200);
		//printf("------- lc=%c\ti=%d\n", lastc, i);
		//if (c == 0) break;
		if (is_digitf(lastc) && !is_digitf(c)) {
			dbg("1");
			delay(200);
			// we have a number from start to &in[i-1]
			len = &in[i]-start;
			memcpy(tmp, start, len);
			tmp[len] = '\0';
			//printf("len %d\ntmp:", len);
			//puts(tmp);
			queue[queue_i] = atof(tmp);
			//printf("adding %f\n", queue[queue_i]);
			types[queue_i] = NUM;
			queue_i++;
			start = &in[i];
		} else if (is_op(lastc)) { // our ops are always 1 char long
			dbg("2");
			delay(200);
			while (stack_i > 0) {
				tmp_op = stack[stack_i-1];
				if (!is_op(tmp_op)) break;
				if ((is_left_as(lastc) && get_prec(lastc) <= get_prec(tmp_op)) ||
				    get_prec(lastc) < get_prec(tmp_op)) {
					// pop op off stack onto queue
					*(char*)(&queue[queue_i]) = tmp_op;
					//printf("adding %c\n", tmp_op);
					types[queue_i] = OP;
					queue_i++;
					stack_i--;
				} else break;
			}
			// push op onto stack
			if (lastc == '-' && ((i>=2 && in[i-2] == '(') || queue_i == 0)) {
				lastc = '#';
			}
			//printf("pushing %c\n", lastc);
			stack[stack_i++] = lastc;
			start = &in[i];
		} else if (lastc == '(') {
			//printf("pushing %c\n", lastc);
			stack[stack_i++] = lastc;
			start = &in[i];
		} else if (lastc == ')') {
			while (stack[stack_i-1] != '(') {
				// pop op from stack onto queue
				tmp_op = stack[stack_i-1];
				*(char*)(&queue[queue_i]) = tmp_op;
				//printf("%c\n", tmp_op);
				types[queue_i] = OP;
				queue_i++;
				stack_i--;
			}
			stack_i--; // drop (
			start = &in[i];
		}
			dbg("5");
			delay(200);
		lastc = c;
		if (c == '\0') break;
		if (queue_i > DBUF_SIZE) dbg("!"); //assert(0 && "burn");
		delay(200);
	}

	//stack_i--;
	while (stack_i > 0) {
		tmp_op = stack[stack_i-1];
		*(char*)(&queue[queue_i]) = tmp_op;
		//printf("adding %c\n", tmp_op);
		types[queue_i] = OP;
		queue_i++;
		stack_i--;
	}
	//puts("yay!");
	return r_polish_slim(queue, types, queue_i);
}

