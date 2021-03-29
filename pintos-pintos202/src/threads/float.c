#include "threads/float.h"

// n=>a :converge integer to fixed point float
int int_to_fp(int n) {
	return n * FRACTION;
}

//a=>n :converge fixed point float to integer(rounded off)
int fp_to_int_round(int x) {
	return x / FRACTION;
}

// a=>n :converge fixed point float to integer(rounded down)
int fp_to_int(int x) {
	if (x>=0 ) {
		return (x + FRACTION / 2) / FRACTION;
	}
	else {
		return (x - FRACTION / 2) / FRACTION;
	}
}

// a+b : Addition of floating - point numbers
int fp_add_fp(int x, int y) {
	return x + y;
}

// a+n : Addition of floating - point number and integer
int fp_add_int(int x, int n) {
	return x + int_to_fp(n);
}

// a-b : Subtraction of floating - point numbers
int fp_sub_fp(int x, int y) {
	return x - y;
}

// a-n : Subtraction of floating - point number and integer
int fp_sub_int(int x, int n) {
	return x - int_to_fp(n);
}

// a*b : Multiplication of floating - point numbers
int fp_mul_fp(int x, int y) {
	return ((int64_t)x) * y / FRACTION;
}

// a*n : Multiplication of floating - point number and integer
int fp_mul_int(int x, int n) {
	return x * n;
}
// a/b : Division of floating - point numbers
int fp_div_fp(int x, int y) {
	return ((int64_t)x)* FRACTION / y;
}

// a/n : Division of floating - point number and integer
int fp_div_int(int x, int n) {
	return x / n;
}