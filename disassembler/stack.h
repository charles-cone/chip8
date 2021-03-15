//
// Created by Charlie Cone on 2/8/21.
//

#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#include "stdlib.h"
#include "stdio.h"

struct stack {
    int size;
    int len;
    int *buf;
};

struct stack* initStack(int size);

void insertVal(struct stack* st, int val);
void deleteStack(struct stack* st);
void sortContents(struct stack* st);
void printStack(struct stack* st);

int popVal(struct stack* st);

#endif //CHIP8_STACK_H
