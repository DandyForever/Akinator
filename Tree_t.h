#pragma once

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iostream>

typedef char* TreeElem_t;

const int Crashcan1 = 666666666, Crashcan2 = 999999999, MaxTree = 1024;

const char LOG[] = "log.txt";

struct Node
{
    int canaryleft = Crashcan1;

    TreeElem_t value;
    Node* left  = nullptr;
    Node* right = nullptr;
    Node* first = nullptr;

    int canaryright = Crashcan2;
};

class Tree_t
{
    private:
        int canaryleft = Crashcan1;

        Node* first_node;
        int size;

        int canaryright = Crashcan2;

        bool dump                     (const int value,    FILE* file_error_info);
        bool dump                     (const double value, FILE* file_error_info);
        bool dump                     (const void* value,  FILE* file_error_info);

    public:
        bool NodeOK                   (const Node* node);
        bool TreeOK                   (const Node* current);
        void NodeDumps                (FILE* output_file, const Node* cur, const int parent, int* number, const char* side);
        void Dump                     ();

        Tree_t                        ();
        ~Tree_t                       ();

        Node* First                   ();
        int Size                      ();

        void DeleteNode               (Node* current);
        void PushFirst                (const TreeElem_t value);
        void PushLeft                 (Node* node, const TreeElem_t value);
        void PushRight                (Node* node, const TreeElem_t value);
};

