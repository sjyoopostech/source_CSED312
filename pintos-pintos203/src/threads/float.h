#ifndef THREADS_FLOAT_H
#define THREADS_FLOAT_H

#include <stdint.h>

/*Edited*/
/*All codes in this file are newly written content.*/

#define FRACTION (1 << 14) //num 1 in fixed point
#define INT_MAX ((1 << 31) - 1) //maximum value of int
#define INT_MIN (-(1 << 31)) //minimum value of int

//a,b denotes floating point number
//n denotes integer

int int_to_fp(int n); // n=>a :converge integer to fixed point float
int fp_to_int_round(int x); //a=>n :converge fixed point float to integer(rounded off)
int fp_to_int(int x); // a=>n :converge fixed point float to integer(rounded down)
int fp_add_fp(int x, int y); // a+b : Addition of floating - point numbers
int fp_add_int(int x, int n); // a+n : Addition of floating - point number and integer
int fp_sub_fp(int x, int y); // a-b : Subtraction of floating - point numbers
int fp_sub_int(int x, int n); // a-n : Subtraction of floating - point number and integer
int fp_mul_fp(int x, int y); // a*b : Multiplication of floating - point numbers
int fp_mul_int(int x, int y); // a*n : Multiplication of floating - point number and integer
int fp_div_fp(int x, int y); // a/b : Division of floating - point numbers
int fp_div_int(int x, int n); // a/n : Division of floating - point number and integer

#endif /* threads/float.h */