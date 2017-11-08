///STACK
///-----
#pragma once

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iostream>

typedef int StackElem_t;

const int Min_size_st = 1024, Add_size_st = 1024;
const int Crash_can1 = 6666666, Crash_can2 = 9999999;
const StackElem_t Crash_can_dat1 = (StackElem_t) 12555555, Crash_can_dat2 = (StackElem_t) 22555555;
const double EPS = 1e-6;
const int Small_ptr = 7;

class Stack_t
{
    private:
        int canaryleft;

        StackElem_t* data;
        int count_st;
        int size_st;
        int _hash;
        int _hash_data;

        int canaryright;

        void dump                      (const int value, FILE* file_error_info);
        void dump                      (const double value, FILE* file_error_info);
        void dump                      (const void* value, FILE* file_error_info);
        int Hash_val                   ();
        int Hash_data                  ();
        void Grow                      ();

    public:
        bool OK                        ();
        void Dump                      ();
        bool Empty                     ();
        Stack_t                        ();
        ~Stack_t                       ();
        void Push                      (const StackElem_t value);
        StackElem_t Pop                ();
        int Size                       ();
};
